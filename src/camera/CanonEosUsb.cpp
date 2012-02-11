#include "CanonEosUsb.h"
#include "../../include/canon/EDSDK.h"

/*
**	Set SIMULATE_CONNECTION to 0 for operation DSLR normally
**	Set SIMULATE_CONNECTION to 1 for simulate operations, but not connected to DSLR hardware
*/
#define SIMULATE_CONNECTION 1


CanonEosUsb::CanonEosUsb() 
	: Camera()
{
	Initialize ();
}

CanonEosUsb::CanonEosUsb(const wxString& port) 
	: Camera(port)
{
	Initialize ();
}

CanonEosUsb::~CanonEosUsb()
{
	Finish ();
}

bool CanonEosUsb::HasFeature (DriverFeature feature)
{
	switch (feature) {
		case DCP_USE_USB:		
			return true;
		}
	return false;
}


bool CanonEosUsb::Initialize()
{
	wxLogDebug ("Initializing Canon SDK");
	m_Camera = NULL;
	EdsError error = EdsInitializeSDK ();
	return FindCamera ();
}

void CanonEosUsb::Finish ()
{
	wxLogDebug ("Terminating Canon SDK");
#if SIMULATE_CONNECTION == 0
	if (m_Camera != NULL) {
		EdsError error = EdsRelease (m_Camera);
		}
	EdsTerminateSDK ();
#endif
}

bool CanonEosUsb::FindCamera()
{
#if SIMULATE_CONNECTION == 0
	if (m_Camera == NULL) {

		EdsUInt32 cameraCount;
		EdsError error = EdsGetCameraList (&m_CameraList);
		error = EdsGetChildCount (m_CameraList, &cameraCount);
		if (cameraCount == 0) {
			// no camera was found
			wxLogMessage (_("No camera was found."));
			m_Camera = NULL;
			return false;
			}
	
		// get first camera
		error = EdsGetChildAtIndex (m_CameraList, 0, &m_Camera);
		if (error == EDS_ERR_OK) {

			EdsDeviceInfo deviceInfo;
			wxString cameraSubtypes[] = {"Canon legacy protocol cameras", "Canon PTP cameras", "Canon PTP-IP cameras"};
			error = EdsGetDeviceInfo (m_Camera, &deviceInfo);

			wxString desc = wxString (deviceInfo.szDeviceDescription);
			wxString subType = cameraSubtypes[deviceInfo.deviceSubType];

			wxLogMessage (_("Camera found. Type: %s, protocol: %s"), desc, subType);
			}
	}
	return (m_Camera != NULL);
#else
	return true;
#endif
}

wxString CanonEosUsb::GetCameraType ()
{
	return "CanonEosUsb";
}

/*
** bulb start nem mûködik az 1100D-nél, helyette a press shutter button megy
** kEdsCameraCommand_PressShutterButton - 50D, 5D Mark II, 5D Mark II (fw 1.0.7), 500D, 7D, 550D, 60D, 600D, 1100D
*/

bool CanonEosUsb::DoInitShoot ()
{
#if SIMULATE_CONNECTION == 0
	CoInitializeEx( NULL, COINIT_APARTMENTTHREADED );

	EdsError error = EdsOpenSession (m_Camera);
	if (error != EDS_ERR_OK) {
		wxLogError (_("Cancel shooting: cannot open camera session (%d)"), error);
		CoUninitialize();
		return false;
		}

	// check wether the camera is alive yet
	// ???

	// check there is no shooting started earlier
	// ???

	// set exposure parameters (ISO, mirror lockup, etc.)
	// ???

	// lock camera UI
//	uiLocked = false;
	error = EdsSendStatusCommand (m_Camera, kEdsCameraStatusCommand_UILock, 0);
	if (error != EDS_ERR_OK) {
		EdsCloseSession (m_Camera);
		wxLogError (_("Cancel shooting: error in UI lock (%d)"), error);
		CoUninitialize();
		return false;
		}
#endif
	return true;
}

bool CanonEosUsb::DoStartShoot ()
{
	// start bulb shooting
	m_BulbCommandUsed = false;
#if SIMULATE_CONNECTION == 0
	EdsError error = EdsSendCommand (m_Camera, kEdsCameraCommand_PressShutterButton, kEdsCameraCommand_ShutterButton_Completely_NonAF);
	if (error == EDS_ERR_NOT_SUPPORTED) {
		m_BulbCommandUsed = true;
		wxLogDebug (_("ShutterButton command is not supperted. BulbStart is used instead."));
		error = EdsSendCommand (m_Camera, kEdsCameraCommand_BulbStart, 0);
		}

	// error occured at starting of shooting, unlock camera UI
	if (error != EDS_ERR_OK) {
		EdsSendStatusCommand (m_Camera, kEdsCameraStatusCommand_UIUnLock, 0);
		EdsCloseSession (m_Camera);
		wxLogError (_("Cancel shooting: error in bulb start (%d)"), error);
		CoUninitialize();
		return false;
		}
#endif
	return true;
}

bool CanonEosUsb::DoEndShoot ()
{
#if SIMULATE_CONNECTION == 0
	EdsError error;
	// stop bulb shooting
	if (!m_BulbCommandUsed)
		error = EdsSendCommand (m_Camera, kEdsCameraCommand_PressShutterButton, kEdsCameraCommand_ShutterButton_OFF);
	else
		error = EdsSendCommand (m_Camera, kEdsCameraCommand_BulbEnd, 0);
#endif
	return true;
}

bool CanonEosUsb::DoTermShoot ()
{
#if SIMULATE_CONNECTION == 0
	// unlock camera UI
	EdsError error = EdsSendStatusCommand (m_Camera, kEdsCameraStatusCommand_UIUnLock, 0);

	// close camera session
	error = EdsCloseSession (m_Camera);

	CoUninitialize();
	wxLogDebug (_("End shooting."));
#endif
	return true;
}

bool CanonEosUsb::DoStartMLU ()
{
	EdsError error;
	EdsUInt32 device;
	error = EdsGetPropertyData (m_Camera, kEdsPropID_Evf_OutputDevice, 0, sizeof(device), &device);
	if (error != EDS_ERR_OK) {
		return false;
		}
	device |= kEdsEvfOutputDevice_PC;
	error = EdsSetPropertyData (m_Camera, kEdsPropID_Evf_OutputDevice, 0, sizeof(device), &device);
	if (error != EDS_ERR_OK) {
		return false;
		}
	return true;
}

bool CanonEosUsb::DoTermMLU ()
{
	EdsError error;
	EdsUInt32 device;
	error = EdsGetPropertyData (m_Camera, kEdsPropID_Evf_OutputDevice, 0, sizeof(device), &device);
	if (error != EDS_ERR_OK) {
		return false;
		}
	device &= ~kEdsEvfOutputDevice_PC;
	error = EdsSetPropertyData (m_Camera, kEdsPropID_Evf_OutputDevice, 0, sizeof(device), &device);
	if (error != EDS_ERR_OK) {
		return false;
		}
	return true;
}
