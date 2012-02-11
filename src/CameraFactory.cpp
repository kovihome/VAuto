#include "CameraFactory.h"
#include <wx/arrimpl.cpp>
#include "camera/CanonEosUsb.h"
#include "camera/CanonEosShutter.h"
#include "../include/canon/EDSDK.h"

#define CAMERA_NONE				"None"
#define CAMERA_CANONEOSUSB		"CanonEosUsb"
#define CAMERA_CANONEOSSHUTTER	"CanonEosShutter"

WX_DEFINE_OBJARRAY (CameraArray);

wxString cameraSubtypes[] = {"Canon legacy protocol cameras", "Canon PTP cameras", "Canon PTP-IP cameras"};

CameraFactory* CameraFactory::instance = NULL;

CameraFactory& CameraFactory::Instance()
{ 
	if (instance == NULL) {
		instance = new CameraFactory ();
		}
	return *instance; 
};

CameraFactory::CameraFactory()
{
	ClearCamera ();
}


wxArrayString& CameraFactory::EnumerateDrivers ()
{
	wxArrayString& driverNames = *new wxArrayString();
	driverNames.Add (CAMERA_NONE);
	driverNames.Add (CAMERA_CANONEOSUSB);
	driverNames.Add (CAMERA_CANONEOSSHUTTER);
	return driverNames;
}

wxString& CameraFactory::GetDriverDisplayName (const wxString& driverType)
{
	if (driverType == CAMERA_NONE)
		return *new wxString(_("No camera"));
	if (driverType == CAMERA_CANONEOSUSB)
		return *new wxString(_("Canon EOS USB"));
	if (driverType == CAMERA_CANONEOSSHUTTER)
		return *new wxString(_("Canon EOS Shutter"));
	return *new wxString(_("Unknown"));
}

CameraArray CameraFactory::EnumerateCameras ()
{
	EdsCameraListRef cameraList;
	EdsUInt32 cameraCount;
	EdsCameraRef camera;
	EdsDeviceInfo deviceInfo;
	CameraArray *ca = new CameraArray ();

	EdsError error = EdsInitializeSDK ();

	error = EdsGetCameraList (&cameraList);
	error = EdsGetChildCount (cameraList, &cameraCount);
	for (EdsUInt32 j = 0; j < cameraCount; j++) {

		error = EdsGetChildAtIndex (cameraList, j, &camera);
		error = EdsGetDeviceInfo (camera, &deviceInfo);

		wxString desc = wxString (deviceInfo.szDeviceDescription);
		wxString subType = cameraSubtypes[deviceInfo.deviceSubType];

		Camera camera;
		camera.SetName (wxString::Format ("type: %s, protocol: %s", desc, subType));

		ca->Add (camera);

		}


	error = EdsTerminateSDK ();

	return *ca;
}


Camera* CameraFactory::GetCamera (const wxString& cameraType, const wxString& comPort)
{
	if (m_camera != NULL && m_camera->GetCameraType() != cameraType) {
		delete m_camera;
		m_camera = NULL;
		}

	if (m_camera == NULL) {

		// Canon EOS DSLR on USB port
		if (cameraType == CAMERA_CANONEOSUSB) {
			m_camera = new CanonEosUsb ();
			}

		// Canon EOS Serial Shutter
		else if (cameraType == CAMERA_CANONEOSSHUTTER) {
			m_camera = new CanonEosShutter (comPort);
			}

		// No camera
		else {
			m_camera = new Camera ();
			}
		}
	return m_camera;

}

void CameraFactory::ClearCamera ()
{
	m_camera = NULL;
}

