#ifdef __WXMSW__
    #include <wx/msw/msvcrt.h>      // redefines the new() operator 
#endif 

#include "CameraNotifier.h"

CameraNotifier::CameraNotifier(wxWindow* parent, int id, Camera* camera, int exposureTime)
 : wxThread ()
{
	m_parent = parent;
	m_camera = camera;
	m_exposureTime = exposureTime;
	m_id = id;
}

static wxString shutterStateNames[] = {"ready", "mirror lockup", "wait before mirror lockup", "exposition", "wait before frame", "aborting..."};

void CameraNotifier::SendEvent (CameraNotifier::CameraShootAction shootAction, int currentTime)
{
	wxCommandEvent ev (wxEVT_COMMAND_BUTTON_CLICKED, m_id);
	ev.SetInt (shootAction);
	if (shootAction == CameraNotifier::SHOOT_NOTIFY_TIME) {
		Camera::ShootingState state = m_camera->GetShootingState();

		wxString stateText = wxString::Format ("Frame %d of %d : %s %d/%d s", 
			m_camera->GetCurrentFrame() + 1,
			m_camera->GetFrameCount(), 
			shutterStateNames[state], 
			currentTime, 
			(state == Camera::CSS_SHOOTING ? m_camera->GetExposureTime() : m_camera->GetDelayBeforeShoot()) / 1000
			);
		ev.SetString (stateText);
		}
	wxPostEvent (m_parent, ev);
}

void CameraNotifier::OnExit ()
{
}

#define CAMERA_SHOOT_TIMEOUT		5 // sec

#define TN_SLEEP_TIME				1000 // ms	
#define TN_TIMEOUT(s)				(s*1000/TN_SLEEP_TIME)

wxThread::ExitCode CameraNotifier::Entry ()
{
	m_abort = FALSE;

	bool isShooting = m_camera->IsShooting();
	Camera::ShootingState state = m_camera->GetShootingState();
	int totalTime = (state == Camera::CSS_SHOOTING ? m_camera->GetExposureTime() : m_camera->GetDelayBeforeShoot()) / 1000;
	int timeout = TN_TIMEOUT (totalTime + CAMERA_SHOOT_TIMEOUT);

	while (!m_abort && timeout-- > 0 && isShooting) {
		SendEvent (CameraNotifier::SHOOT_NOTIFY_TIME, totalTime - timeout + CAMERA_SHOOT_TIMEOUT - 1);
		wxThread::Sleep (TN_SLEEP_TIME);
		if (!m_abort && m_camera != NULL) {
			isShooting = m_camera->IsShooting();
			if (isShooting && m_camera->GetShootingState() != state) {
				state = m_camera->GetShootingState();
				totalTime = (state == Camera::CSS_SHOOTING ? m_camera->GetExposureTime() : m_camera->GetDelayBeforeShoot()) / 1000;
				timeout = TN_TIMEOUT (totalTime + CAMERA_SHOOT_TIMEOUT);
				}
			}
		}

	CameraNotifier::CameraShootAction shootAction;
	if (m_abort) {
		shootAction = CameraNotifier::SHOOT_ABORT;
		}
	else if (timeout <= 0) {
		shootAction = CameraNotifier::SHOOT_TIMEOUT;
		}
	else {
		shootAction = CameraNotifier::SHOOT_STOP_NORMALLY;
		}

	// notify parent
	SendEvent (shootAction);

	return NULL;
}

void CameraNotifier::Abort ()
{
	m_abort = TRUE;
}


