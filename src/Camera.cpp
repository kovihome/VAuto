#ifdef __WXMSW__
    #include <wx/msw/msvcrt.h>      // redefines the new() operator 
#endif 

#include "Camera.h"
#include "ObservationLog.h"
#include "../../VTools/src/Properties.h"

void CameraShootTimer::Notify ()
{
	wxLogDebug ("Shooting timer ends.");
	m_CameraController->DoNextShootingAction ();
}

Camera::Camera(const wxString& port, bool needInit)
	: m_port(port), m_ShootTimer(this)
{
	m_initialzed = needInit;
}

Camera::Camera(const Camera& camera, bool needInit)
	: m_ShootTimer(this)
{
	m_initialzed = needInit;
}

Camera::~Camera()
{
}

wxString Camera::GetCameraType ()
{
	return "None";
}

bool Camera::Initialize()
{
	return true;
}

void Camera::Finish ()
{
}

bool Camera::FindCamera()
{
	return true;
}

void Camera::ShootMany (long frameCount, long exposureTime, long delayBeforeShoot)
{
	m_frameCount = frameCount;
	m_exposureTime = exposureTime;
	m_delayBeforeShoot = delayBeforeShoot;
	m_shootingState = CSS_READY;
	m_currentFrame = 0;

	/*
	** Check wether camera is available yet
	*/
	if (!FindCamera ()) {
		wxLogDebug ("Cancel shooting: no camera was found.");
		return;
		}

	wxLogDebug ("Start shooting for %ld", exposureTime);

	/*
	** Write exposure time to observations log
	*/
	ObservationLog::Instance().SetExpositonTime (exposureTime/1000);

	/*
	** Initialize shooting sequence
	*/
	if (!DoInitShoot ())
		return;

	/*
	** If MLU is enabled, starts with MLU
	*/
	m_useMLU = Properties::Instance().GetBool(CAMERA_LOCKUP) && HasFeature (DCP_HAS_MLU);
	if (m_useMLU) {

		m_mluBeforeEveryFrame = HasFeature (DCP_MLU_FOR_EVERY_FRAME);

		/*
		** If MLU delay presents, starts timer with MLU delay
		*/
		m_delayBeforeMLU = 1000 * Properties::Instance().GetInt(CAMERA_LOCKUP_DELAY);
		if (m_delayBeforeMLU > 0) {
			m_shootingState = CSS_MLU_DELAY;
			m_ShootTimer.Start (m_delayBeforeMLU, wxTIMER_ONE_SHOT);
			return;
			}

		/*
		** MLU delay was not present, starts directly with MLU
		*/
		else {
			m_shootingState = CSS_MLU;
			if (!DoStartMLU()) {
				DoTermShoot ();
				m_shootingState = CSS_READY;
				return;
				}
			}
		}

	/*
	** If no MLU needed, and shoot delay presents, starts timer with shoot delay
	*/
	if (delayBeforeShoot > 0) {

		m_shootingState = CSS_SHOOTING_DELAY;
		m_ShootTimer.Start (delayBeforeShoot, wxTIMER_ONE_SHOT);

		}

	/*
	** No MLU, no shoot delay, starts with shooting immediately
	*/
	else {

		/*
		** Write current frame index to observations log
		*/
		ObservationLog::Instance().SetFrameIndex (m_currentFrame);
		ObservationLog::Instance().SetStartTime (wxDateTime::Now());

		if (!DoStartShoot ()) {
			if (m_useMLU)
				DoTermMLU ();
			DoTermShoot ();
			m_shootingState = CSS_READY;
			return;
			}

		// now wait exposure time ends
		m_shootingState = CSS_SHOOTING;
		m_ShootTimer.Start (exposureTime, wxTIMER_ONE_SHOT);

		}

}

void Camera::DoNextShootingAction ()
{
	switch (m_shootingState) {

		/*
		** Delay before MLU timer expires, starts MLU and than starts
		*/
		case CSS_MLU_DELAY:
			m_shootingState = CSS_MLU;
			if (!DoStartMLU ()) {
				DoTermShoot ();
				return;
				}
			else if (m_delayBeforeShoot > 0) {
				m_shootingState = CSS_SHOOTING_DELAY;
				m_ShootTimer.Start (m_delayBeforeShoot, wxTIMER_ONE_SHOT);
				}
			else {
				/*
				** Write current frame index to observations log
				*/
				ObservationLog::Instance().SetFrameIndex (m_currentFrame);
				ObservationLog::Instance().SetStartTime (wxDateTime::Now());

				if (!DoStartShoot ()) {
					if (m_useMLU)
						DoTermMLU ();
					DoTermShoot ();
					return;
					}
				// now wait exposure time ends
				m_shootingState = CSS_SHOOTING;
				m_ShootTimer.Start (m_exposureTime, wxTIMER_ONE_SHOT);
				}
			break;

		/*
		** Delay before shoot timer has expired, starts shooting
		*/
		case CSS_SHOOTING_DELAY:
			/*
			** Write current frame index to observations log
			*/
			ObservationLog::Instance().SetFrameIndex (m_currentFrame);
			ObservationLog::Instance().SetStartTime (wxDateTime::Now());

			if (!DoStartShoot ()) {
				if (m_useMLU)
					DoTermMLU ();
				DoTermShoot ();
				return;
				}
			m_shootingState = CSS_SHOOTING;
			m_ShootTimer.Start (m_exposureTime, wxTIMER_ONE_SHOT);
			break;

		/*
		** Shooting timer has expired, the current frame is ready.
		** If there are no more frames, terminates frame sequence
		** Otherwise take next frame.
		*/
		case CSS_SHOOTING:
			DoEndShoot ();

			/*
			** Write current frame index to observations log
			*/
			ObservationLog::Instance().SetStatus (LOG_STATUS_OK);
			ObservationLog::Instance().Flush ();
			
			// this was the last frame, finish making frames
			if (++m_currentFrame == m_frameCount) {
				if (m_useMLU)
					DoTermMLU ();
				DoTermShoot ();
				m_shootingState = CSS_READY;
				break;
				}
			// use MLU before next frame, if needed
			else if (m_useMLU && m_mluBeforeEveryFrame) {
				DoTermMLU();
				if (m_delayBeforeMLU > 0) {
					m_shootingState = CSS_MLU_DELAY;
					m_ShootTimer.Start (m_delayBeforeMLU, wxTIMER_ONE_SHOT);
					break;
					}
				else {
					m_shootingState = CSS_MLU;
					if (!DoStartMLU ()) {
						DoTermShoot ();
						m_shootingState = CSS_READY;
						return;
						}
					break;
					}
				}
			if (m_delayBeforeShoot > 0) {
				m_shootingState = CSS_SHOOTING_DELAY;
				m_ShootTimer.Start (m_delayBeforeShoot, wxTIMER_ONE_SHOT);
				}
			else {
				/*
				** Write current frame index to observations log
				*/
				ObservationLog::Instance().SetFrameIndex (m_currentFrame);
				ObservationLog::Instance().SetStartTime (wxDateTime::Now());

				if (!DoStartShoot ()) {
					if (m_useMLU)
						DoTermMLU ();
					DoTermShoot ();
					return;
					}
				// now wait exposure time ends
				m_shootingState = CSS_SHOOTING;
				m_ShootTimer.Start (m_exposureTime, wxTIMER_ONE_SHOT);
				}
			break;

		}
}


/*
**
*/

bool Camera::DoInitShoot ()
{
	return true;
}

bool Camera::DoStartShoot ()
{
	return true;
}

bool Camera::DoEndShoot ()
{
	return true;
}

bool Camera::DoTermShoot ()
{
	return true;
}

bool Camera::DoStartMLU ()
{
	return true;
}

bool Camera::DoTermMLU ()
{
	return true;
}

void Camera::Shoot (long exposureTime)
{
	m_exposureTime = exposureTime;

	wxLogDebug ("Start shooting for %ld", exposureTime);

	if (!FindCamera ()) {
		wxLogDebug ("Cancel shooting: no camera was found.");
		return;
		}

	if (!DoInitShoot ())
		return;

	if (!DoStartShoot ())
		return;

	// now wait exposure time ends
	wxLogDebug ("Start shooting timer.");
	m_ShootTimer.Start (exposureTime, wxTIMER_ONE_SHOT);
}

void Camera::AbortShooting ()
{
	if (IsShooting()) {
		wxLogDebug ("Aborting...");
		m_ShootTimer.Stop ();
		switch (m_shootingState) {
			case CSS_MLU_DELAY:
				wxLogDebug ("Aborting delay only");
				break;
			case CSS_MLU:
				wxLogDebug ("Aborting MLU phase.");
				break;
			case CSS_SHOOTING_DELAY:
				wxLogDebug ("Aborting delay only");
				break;
			case CSS_SHOOTING:
				wxLogDebug ("Aborting shoot");
				DoEndShoot ();
				ObservationLog::Instance().SetStatus (LOG_STATUS_ABORT);
				ObservationLog::Instance().Flush ();
				break;
			}
		if (m_useMLU)
			DoTermMLU ();
		DoTermShoot ();
		m_shootingState = CSS_READY;
		}
	else
	{
		wxLogDebug ("Aborting not needed.");
	}
}


