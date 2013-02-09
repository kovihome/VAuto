#ifdef __WXMSW__
    #include <wx/msw/msvcrt.h>      // redefines the new() operator 
#endif 

#include "../common.h"
#include "CanonEosUsb.h"
#include "../../../VTools/src/Properties.h"
#include "../../include/canon/EDSDK.h"

#define BATTERY_AC			0xffffffff

static struct {
	unsigned long val;
	unsigned long iso; 
	} isoMapping[] = {
		{0x28,      6},
		{0x30,     12},
		{0x38,     25},
		{0x40,     50},
		{0x48,    100},
		{0x4b,    125},
		{0x50,    200},
		{0x53,    250},
		{0x55,    320},
		{0x58,    400},
		{0x5b,    500},
		{0x5d,    640},
		{0x60,    800},
		{0x63,   1000},
		{0x65,   1250},
		{0x68,   1600},
		{0x70,   3200},
		{0x78,   6400},
		{0x80,  12800},
		{0x88,  25600},
		{0x90,  51200},
		{0x98, 102400}
		};

#define SHUTTER_SPEED_BULB	0x0c
	
static struct {
	unsigned long val;
	unsigned long speed;
	char norm;
	} shutterSpeedMapping[] = {
		{0x10, 30, 'I'},
		{0x13, 25, 'I'},
		{0x14, 20, 'I'},
		{0x18, 15, 'I'},
		{0x1b, 13, 'I'},
		{0x1c, 10, 'I'},
		{0x20,  8, 'I'},
		{0x24,  6, 'I'},
		{0x25,  5, 'I'},
		{0x28,  4, 'I'},
		{0x2b, 32, 'D'},
		{0x2c,  3, 'I'},
		{0x2d, 25, 'D'},
		{0x30,  2, 'I'},
		{0x33, 16, 'D'},
		{0x34, 15, 'D'},
		{0x35, 13, 'D'},
		{0x38,  1, 'I'},
		{0x3b,  8, 'D'},
		{0x3c,  7, 'D'},
		{0x3d,  6, 'D'},
		{0x40,  5, 'D'},
		{0x43,  4, 'D'},
		{0x44,  3, 'D'},
		{0x48,  4, 'R'},
		{0x4b,  5, 'R'},
		{0x4c,  6, 'R'},
		{0x50,  8, 'R'},
		{0x54, 10, 'R'},
		{0x5d, 25, 'R'},
		{0x60, 30, 'R'},
		{0x63, 40, 'R'},
		{0x64, 45, 'R'},
		{0x65, 50, 'R'},
		{0x68, 60, 'R'},
		{0x6b, 80, 'R'},
		{0x6c, 90, 'R'},
		{0x6d, 100, 'R'},
		{0x70, 125, 'R'},
		{0x73, 160, 'R'},
		{0x74, 180, 'R'},
		{0x75, 200, 'R'},
		{0x78, 250, 'R'},
		{0x7b, 320, 'R'},
		{0x7c, 350, 'R'},
		{0x7d, 400, 'R'},
		{0x80, 500, 'R'},
		{0x83, 640, 'R'},
		{0x84, 750, 'R'},
		{0x85, 800, 'R'},
		{0x88, 1000, 'R'},
		{0x8b, 1250, 'R'},
		{0x8c, 1500, 'R'},
		{0x8d, 1600, 'R'},
		{0x90, 2000, 'R'},
		{0x93, 2500, 'R'},
		{0x94, 3000, 'R'},
		{0x95, 3200, 'R'},
		{0x98, 4000, 'R'},
		{0x9c, 6000, 'R'},
		{0x9d, 6400, 'R'},
		{0xa0, 8000, 'R'}
		};

CanonEosUsb::CanonEosUsb(bool needInit) 
	: Camera(needInit)
	, m_isoSpeedOriginal(0)
{
	if (needInit)
		Initialize ();
}

CanonEosUsb::CanonEosUsb(const wxString& port, bool needInit) 
	: Camera(port, needInit)
{
	if (needInit)
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
		case DCP_HAS_MLU:
		case DCP_CAN_SET_ISO_SPEED:
			return true;
		}
	return false;
}

const wxString aeModes[] = {
	"Program AE",
	"Shutter-Speed Priority AE",
	"Aperture Priority AE",
	"Manual Exposure",
	"Bulb",
	"Auto Depth-of-Field AE",
	"Depth-of-Field AE",
	"Camera Settings Registered",
	"Lock",
	"Auto",
	"Night Scene Portrait",
	"Sports",
	"Portait",
	"Landscape",
	"Close-Up",
	"Flash Off",
	"",
	"",
	"",
	"Creative Auto",
	"",
	"Photo In Movie"
	};


bool CanonEosUsb::Initialize()
{
	wxLogDebug ("Initializing Canon SDK");
	m_Camera = NULL;
#if SIMULATE_CONNECTION_CAMERA == 0
	EdsError error = EdsInitializeSDK ();
	bool cameraFound = FindCamera ();
	if (cameraFound) {

		if (Connect()) {

			wxString productName = GetPropertyValueString (kEdsPropID_ProductName);
			wxLogDebug (wxString::Format ("Product name: %s", productName));

			wxString firmwareVersion = GetPropertyValueString (kEdsPropID_FirmwareVersion);
			wxLogDebug (wxString::Format ("Firmware version: %s", firmwareVersion));

			unsigned long batteryLevel = GetPropertyValueLong (kEdsPropID_BatteryLevel);
			if (batteryLevel != 0xffffffff)
				wxLogDebug (wxString::Format ("Battery level: %ld", batteryLevel));
			else
				wxLogDebug ("Battery level: AC");

			unsigned long imageQuality = GetPropertyValueLong (kEdsPropID_ImageQuality);
			unsigned long imageType = (imageQuality >> 20) & 0xf;
			wxString imageTypeS = "Unknown";
			switch (imageType) {
				case 1:
					imageTypeS = "JPEG";
					break;
				case 2:
					imageTypeS = "CRW";
					break;
				case 4:
					imageTypeS = "RAW";
					break;
				case 6:
					imageTypeS = "CR2";
					break;
				}
			wxLogDebug (wxString::Format ("Image quality: 0x%4lx, image type: %s", imageQuality, imageTypeS));

			// save camera's original iso speed
//			unsigned long isoSpeedV = GetPropertyValueLong (kEdsPropID_ISOSpeed);
//			unsigned long isoSpeed = (unsigned long)(100.0 * pow (2.0, (((double)isoSpeedV)/8.0) - 9.0) + 0.5);
			m_isoSpeedOriginal = GetIsoSpeed ();
			wxLogDebug (wxString::Format ("Camera original ISO speed: %ld", m_isoSpeedOriginal));

			unsigned long expTimeV = GetPropertyValueLong (kEdsPropID_Tv);
			if (expTimeV == 0xC) {
				wxLogDebug ("Exposure time: Bulb");
				}
			else {
				unsigned long expTime = (unsigned long)(pow (2.0, (7.0 - ((double)expTimeV)/8.0)) + 0.5);
				wxLogDebug (wxString::Format ("Exposure time: %ld (%lx)", expTime, expTimeV));
				}

			unsigned long availableShots = GetPropertyValueLong (kEdsPropID_AvailableShots);
			wxLogDebug (wxString::Format ("Available shots: %ld", availableShots));

			unsigned long lensStatus = GetPropertyValueLong (kEdsPropID_LensStatus);
			wxLogDebug (wxString::Format ("Lens status: %s", lensStatus == 1 ? "attached" : "not attached"));

			unsigned long aeMode = GetPropertyValueLong (kEdsPropID_AEMode);
			wxLogDebug (wxString::Format ("AE mode: %s (%lx)", aeMode == 0xffffffff ? "Unknown" : aeModes[aeMode], aeMode));

			unsigned long driveMode = GetPropertyValueLong (kEdsPropID_DriveMode);
			wxLogDebug (wxString::Format ("Drive mode: %ld (%lx)", driveMode, driveMode));

			unsigned long evfMode = GetPropertyValueLong (kEdsPropID_Evf_Mode);
			wxLogDebug (wxString::Format ("EVF mode (live view): %s", evfMode == 0 ? "disabled" : "enabled"));

			// refresh camera date and time
			SetCameraTime ();

//			unsigned long longExpNoise = GetCFnValueLong (3);
//			wxLogDebug (wxString::Format ("Long exposure noise correction: %ld (%lx)", longExpNoise, longExpNoise));

//			unsigned long largeSensNoise = GetCFnValueLong (4);
//			wxLogDebug (wxString::Format ("Large sensitivity noise correction: %ld (%lx)", largeSensNoise, largeSensNoise));

//			EdsError err = EDS_ERR_OK;
//			EdsDataType dataType;
//			EdsUInt32 dataSize;
//			wxString result;
//			for (unsigned long j = 1; j < 0xA00; j++) {
//
//				err = EdsGetPropertySize (m_Camera, j, 0 , &dataType, &dataSize);
//				if (err == EDS_ERR_OK) {
//					if (dataType == kEdsDataType_String) {
//						EdsChar* propertyValue = new EdsChar[dataSize];
//						err = EdsGetPropertyData (m_Camera, j, 0 , dataSize, propertyValue);
//						if (err == EDS_ERR_OK) {
//							result = wxString (propertyValue);
//							wxLogDebug (wxString::Format ("property %lx value %s: ", j, result));
//							}
//						else {
//							wxLogDebug (wxString::Format ("property %lx data error: %lx", j, err));
//							}
//						delete propertyValue;
//						}
//					else if (dataType == kEdsDataType_UInt32) {
//						EdsUInt32 propertyValue = 0;
//						err = EdsGetPropertyData (m_Camera, j, 0 , dataSize, &propertyValue);
//						if (err == EDS_ERR_OK) {
//							wxLogDebug (wxString::Format ("property %lx value %lx: ", j, propertyValue));
//							}
//						else {
//							wxLogDebug (wxString::Format ("property %lx data error: %lx", j, err));
//							}
//
//						}
//					else if (dataType == kEdsDataType_Int32) {
//						EdsInt32 propertyValue = 0;
//						err = EdsGetPropertyData (m_Camera, j, 0 , dataSize, &propertyValue);
//						if (err == EDS_ERR_OK) {
//							wxLogDebug (wxString::Format ("property %lx value %lx: ", j, propertyValue));
//							}
//						else {
//							wxLogDebug (wxString::Format ("property %lx data error: %lx", j, err));
//							}
//
//						}
//					else {
//						wxLogDebug (wxString::Format ("property %lx type %lx size %ld ", j, dataType, dataSize));
//						}
//					}
//				else {
////					if (err != EDS_ERR_UNIMPLEMENTED && err != EDS_ERR_NOT_SUPPORTED)
//					if (err != EDS_ERR_PROPERTIES_UNAVAILABLE)
//						wxLogDebug (wxString::Format ("property %lx error: %lx", j, err));
//					}	
//				}

//			Disconnect ();
			}
		else {
			wxLogDebug ("Error: cannot connect to the camera.");
			}
		}
#else
	bool cameraFound = true;
#endif
	return cameraFound;
}

void CanonEosUsb::Finish ()
{
	wxLogDebug ("Terminating Canon SDK");
	if (m_initialzed) {

#if SIMULATE_CONNECTION_CAMERA == 0

		// restore camera's original iso speed
		SetIsoSpeed (m_isoSpeedOriginal);
	
		if (m_Camera != NULL) {
			Disconnect ();
			EdsError error = EdsRelease (m_Camera);
			}
		EdsTerminateSDK ();
#endif
		}
}

bool CanonEosUsb::FindCamera()
{
#if SIMULATE_CONNECTION_CAMERA == 0
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
#if SIMULATE_CONNECTION_CAMERA == 0

	if (m_Camera == NULL) {
		if (!Initialize ()) {
			return false;
			}
		}

//	if (!Connect ()) {
//		wxLogError ("DoInitShoot failed.");
//		return false;
//		}

	// check wether the camera is alive yet
	// ???

	// check there is no shooting started earlier
	// ???

	// check battery level
	unsigned long BATTERY_MIN_LEVEL = Properties::Instance().GetInt(CAMERA_BATTERY_MIN_LEVEL);
	unsigned long batteryLevel = GetPropertyValueLong (kEdsPropID_BatteryLevel);
	if (batteryLevel != BATTERY_AC && batteryLevel < BATTERY_MIN_LEVEL) {
//		Disconnect ();
		wxLogWarning ("DoInitShoot warning: battery level is low: %ld", batteryLevel);
		wxMessageBox (_("Battery level is low.\n\nChange battery to a full charged one, or connect an AC adapter."), _("Battery Low"), wxICON_EXCLAMATION | wxID_OK, NULL);
		return false;
		}

	// check available shots
	unsigned long availableShots = GetPropertyValueLong (kEdsPropID_AvailableShots);
	if (availableShots < m_frameCount) {
//		Disconnect ();
		wxLogWarning (wxString::Format ("DoInitShoot warning: not enough available shots: %ld", availableShots));
		wxMessageBox (_("Not enough available shots.\n\nChange the card to an empty one, or donwload and delete images form the actual card."), _("Not Enough Shots Available"), wxICON_EXCLAMATION | wxID_OK, NULL);
		return false;
		}


	// set exposure parameters (mirror lockup, etc.)
	// ???

	// lock camera UI
	EdsError error = EdsSendStatusCommand (m_Camera, kEdsCameraStatusCommand_UILock, 0);
	if (error != EDS_ERR_OK) {
		wxLogError ("DoInitShoot error in UI lock: %d", error);
//		Disconnect ();
		return false;
		}

	// if mirror lockup is executed via live view, starts now
//	if (!DoStartMLU ())
//	{
//		wxLogError ("DoInitShoot error in start live view: %d", error);
//		EdsSendStatusCommand (m_Camera, kEdsCameraStatusCommand_UIUnLock, 0);
//		Disconnect ();
//		return false;
//	}
#endif
	wxLogDebug ("DoInitShoot done.");
	return true;
}

bool CanonEosUsb::DoStartShoot ()
{
	// start bulb shooting
	m_BulbCommandUsed = false;
#if SIMULATE_CONNECTION_CAMERA == 0
	EdsError error = EdsSendCommand (m_Camera, kEdsCameraCommand_PressShutterButton, kEdsCameraCommand_ShutterButton_Completely_NonAF);
	if (error == EDS_ERR_NOT_SUPPORTED) {
		m_BulbCommandUsed = true;
		wxLogDebug (_("DoStartShoot: ShutterButton command is not supperted. BulbStart is used instead."));
		error = EdsSendCommand (m_Camera, kEdsCameraCommand_BulbStart, 0);
		}

	// error occured at starting of shooting, unlock camera UI
	if (error != EDS_ERR_OK) {
//		EdsSendStatusCommand (m_Camera, kEdsCameraStatusCommand_UIUnLock, 0);
//		wxLogError (_("DoStartShoot error in bulb start: %d"), error);
//		Disconnect ();
		return false;
		}
#endif
	wxLogDebug ("DoStartShoot done.");
	return true;
}

bool CanonEosUsb::DoEndShoot ()
{
#if SIMULATE_CONNECTION_CAMERA == 0
	EdsError error;
	// stop bulb shooting
	if (!m_BulbCommandUsed) {
		error = EdsSendCommand (m_Camera, kEdsCameraCommand_PressShutterButton, kEdsCameraCommand_ShutterButton_OFF);
		if (error != EDS_ERR_OK)
			wxLogError ("DoEndShoot error: %d", error);
		}
	else {
		error = EdsSendCommand (m_Camera, kEdsCameraCommand_BulbEnd, 0);
		if (error != EDS_ERR_OK)
			wxLogError ("DoEndShoot error: %d", error);
		}
#endif
	wxLogDebug ("DoEndShoot done.");

//	wxString imageFileName = GetLastImageFileName ();

	return true;
}

bool CanonEosUsb::DoTermShoot ()
{
#if SIMULATE_CONNECTION_CAMERA == 0
	// unlock camera UI
	EdsError error = EdsSendStatusCommand (m_Camera, kEdsCameraStatusCommand_UIUnLock, 0);
	if (error != EDS_ERR_OK)
		wxLogError ("DoTermShoot error: %d", error);

	// close camera session
//	Disconnect ();
#endif
	wxLogDebug (_("DoTermShoot done."));
	return true;
}

/*
** TODO: if camera does not support live view, lock up mirror with button press (or ?)
*/
bool CanonEosUsb::DoStartMLU ()
{
#if SIMULATE_CONNECTION_CAMERA == 0
	EdsError error;
	EdsUInt32 device;
	error = EdsGetPropertyData (m_Camera, kEdsPropID_Evf_OutputDevice, 0, sizeof(device), &device);
	if (error != EDS_ERR_OK) {
		wxLogError ("DoStartMLU failed: %d", error);
		return false;
		}
	device |= kEdsEvfOutputDevice_PC;
	error = EdsSetPropertyData (m_Camera, kEdsPropID_Evf_OutputDevice, 0, sizeof(device), &device);
	if (error != EDS_ERR_OK) {
		wxLogError ("DoStartMLU failed: %d", error);
		return false;
		}
#endif
	wxLogDebug ("DoStartMLU done.");
	return true;
}

/*
** TODO: if camera does not support live view, nothing to do (the shutter closed at the end of the exposition)
*/
bool CanonEosUsb::DoTermMLU ()
{
#if SIMULATE_CONNECTION_CAMERA == 0
	EdsError error;
	EdsUInt32 device;
	error = EdsGetPropertyData (m_Camera, kEdsPropID_Evf_OutputDevice, 0, sizeof(device), &device);
	if (error != EDS_ERR_OK) {
		wxLogError ("DoTermMLU failed: %d", error);
		return false;
		}
	device &= ~kEdsEvfOutputDevice_PC;
	error = EdsSetPropertyData (m_Camera, kEdsPropID_Evf_OutputDevice, 0, sizeof(device), &device);
	if (error != EDS_ERR_OK) {
		wxLogError ("DoTermMLU failed: %d", error);
		return false;
		}
#endif
	wxLogDebug ("DoTermMLU done.");
	return true;
}

bool CanonEosUsb::Connect(void)
{
#if SIMULATE_CONNECTION_CAMERA == 0
	CoInitializeEx( NULL, COINIT_APARTMENTTHREADED );
	EdsError error = EdsOpenSession (m_Camera);
	if (error != EDS_ERR_OK) {
		wxLogError (_("Connect error: cannot open camera session (%d)"), error);
		CoUninitialize();
		return false;
		}
#endif
	return true;
}

bool CanonEosUsb::Disconnect(void)
{
#if SIMULATE_CONNECTION_CAMERA == 0
	EdsCloseSession (m_Camera);
	CoUninitialize();
#endif
	return true;
}

wxString CanonEosUsb::GetPropertyValueString (EdsPropertyID propId)
{
	EdsError err = EDS_ERR_OK;
	EdsDataType dataType;
	EdsUInt32 dataSize;
	wxString result;
	err = EdsGetPropertySize (m_Camera, propId, 0 , &dataType, &dataSize);
	if (err == EDS_ERR_OK) {
		EdsChar* propertyValue = new EdsChar[dataSize];
		err = EdsGetPropertyData (m_Camera, propId, 0 , dataSize, propertyValue);
		if (err == EDS_ERR_OK) {
			result = wxString (propertyValue);
			}
		else {
			wxLogDebug (wxString::Format ("EdsGetPropertyData error: %d", err));
			}
		delete propertyValue;
		}
	else {
		wxLogDebug (wxString::Format ("EdsGetPropertySize error: %d", err));
		}	

	return result;
}

unsigned long CanonEosUsb::GetPropertyValueLong(EdsPropertyID propId)
{
	EdsError err = EDS_ERR_OK;
	EdsDataType dataType;
	EdsUInt32 dataSize;
	long result = -1;
	err = EdsGetPropertySize (m_Camera, propId, 0 , &dataType, &dataSize);
	if (err == EDS_ERR_OK) {
		EdsUInt32 propertyValue;
		err = EdsGetPropertyData (m_Camera, propId, 0 , dataSize, &propertyValue);
		if (err == EDS_ERR_OK) {
			result = propertyValue;
			}
		else {
			wxLogDebug (wxString::Format ("EdsGetPropertyData error: %d", err));
			}
		}
	else {
		wxLogDebug (wxString::Format ("EdsGetPropertySize error: %d", err));
		}	

	return result;
}

void CanonEosUsb::SetPropertyValueLong(EdsPropertyID propId, unsigned long value)
{
	EdsError err = EDS_ERR_OK;
	err = EdsSetPropertyData (m_Camera, propId, 0, sizeof(value), (EdsUInt32 *) &value);
	if (err != EDS_ERR_OK) {
		wxLogDebug (wxString::Format ("EdsSetPropertyData error: %d", err));
		}
}

void CanonEosUsb::SetCameraTime()
{
	EdsError err = EDS_ERR_OK;
	EdsDataType dataType;
	EdsUInt32 dataSize;
	long result = -1;

	err = EdsGetPropertySize (m_Camera, kEdsPropID_DateTime, 0 , &dataType, &dataSize);
	if (err == EDS_ERR_OK) {
		EdsTime propertyValue;
		err = EdsGetPropertyData (m_Camera, kEdsPropID_DateTime, 0 , dataSize, &propertyValue);
		if (err == EDS_ERR_OK) {

			wxDateTime cameraTime;
			cameraTime.Set (propertyValue.day, (wxDateTime::Month) (int) (propertyValue.month - 1), propertyValue.year, 
				propertyValue.hour, propertyValue.minute, propertyValue.second, propertyValue.milliseconds);
			wxLogDebug ("Camera time: " + cameraTime.Format ("%Y-%m-%d %H:%M:%S"));

			wxDateTime dt = wxDateTime::Now ();
			wxLogDebug ("Computer time: " + dt.Format ("%Y-%m-%d %H:%M:%S"));
			
			propertyValue.year = dt.GetYear ();
			propertyValue.month = dt.GetMonth () + 1;
			propertyValue.day = dt.GetDay ();
			propertyValue.hour = dt.GetHour ();
			propertyValue.minute = dt.GetMinute ();
			propertyValue.second = dt.GetSecond ();
			propertyValue.milliseconds = dt.GetMillisecond ();

			err = EdsSetPropertyData (m_Camera, kEdsPropID_DateTime, 0, dataSize, &propertyValue);
			if (err != EDS_ERR_OK) {
				wxLogError (wxString::Format ("SetCameraTime set property error: %d", err));
				}
			}
		else {
			wxLogError (wxString::Format ("SetCameraTime get property error: %d", err));
			}
		}
	else {
		wxLogError (wxString::Format ("SetCameraTime get dproperty size error: %d", err));
		}	
}



unsigned long CanonEosUsb::GetCFnValueLong (unsigned long cfn)
{
	EdsError err = EDS_ERR_OK;
	EdsDataType dataType;
	EdsUInt32 dataSize;
	long result = -1;
	err = EdsGetPropertySize (m_Camera, kEdsPropID_CFn, cfn , &dataType, &dataSize);
	if (err == EDS_ERR_OK) {
		EdsUInt32 propertyValue;
		err = EdsGetPropertyData (m_Camera, kEdsPropID_CFn, cfn , dataSize, &propertyValue);
		if (err == EDS_ERR_OK) {
			result = propertyValue;
			}
		else {
			wxLogDebug (wxString::Format ("EdsGetPropertyData error: %d", err));
			}
		}
	else {
		wxLogDebug (wxString::Format ("EdsGetPropertySize error: %d", err));
		}	

	return result;
}


wxString CanonEosUsb::GetLastImageFileName()
{
	EdsVolumeRef volume;
	EdsDirectoryItemRef dirItem = NULL;
	EdsDirectoryItemRef directoryItem = NULL;
	EdsDirectoryItemInfo dirItemInfo;
	EdsError err = EDS_ERR_OK;
	EdsUInt32 count = 0;

	// Get the number of camera volumes
	err = EdsGetChildCount(m_Camera, &count);
	if (err == EDS_ERR_OK && count == 0)	{
		err = EDS_ERR_DIR_NOT_FOUND;
		}

	// Get initial volume
	if (err == EDS_ERR_OK) {
		err = EdsGetChildAtIndex(m_Camera, 0 , &volume);
		}

	if (err == EDS_ERR_OK) {

		// Get number of items under the volume
		err = EdsGetChildCount(volume, &count);
		if (err == EDS_ERR_OK && count == 0) {
			err = EDS_ERR_DIR_NOT_FOUND;
			}

		// Get DCIM folder
		for (unsigned int i = 0 ; i < count && err == EDS_ERR_OK; i++) {

			// Get the ith item under the specifed volume
			if (err == EDS_ERR_OK) {
				err = EdsGetChildAtIndex(volume, i , &dirItem);
				}

			// Get retrieved item information
			if(err == EDS_ERR_OK) {
				err = EdsGetDirectoryItemInfo(dirItem, &dirItemInfo);
				}

			// Indicates whether or not the retrieved item is a DCIM folder.
			if (err == EDS_ERR_OK) {
				if (wxStricmp (dirItemInfo.szFileName, "DCIM") == 0 && dirItemInfo.isFolder == (EdsBool) true) {
					directoryItem = dirItem;
					break;
					}
				}

			// Release retrieved item
			if (dirItem) {
				EdsRelease (dirItem);
				dirItem = NULL;
				}
			}

		// DCIM is found here
		// Get number of items under the volume
		err = EdsGetChildCount(directoryItem, &count);
		if (err == EDS_ERR_OK && count == 0) {
			err = EDS_ERR_DIR_NOT_FOUND;
			}
			
		if (err == EDS_ERR_OK) {
			for (unsigned int i = 0 ; i < count && err == EDS_ERR_OK; i++) {

					// Get the ith item under the specifed volume
					if (err == EDS_ERR_OK) {
						err = EdsGetChildAtIndex(directoryItem, i , &dirItem);
						}
		
					// Get retrieved item information
					if(err == EDS_ERR_OK) {
						err = EdsGetDirectoryItemInfo(dirItem, &dirItemInfo);
	
						}

					if (dirItem) {
						EdsRelease (dirItem);
						dirItem = NULL;
						}

					}
			EdsRelease (directoryItem);
			directoryItem = NULL;
			}

		}

	return wxString();
}

// Read the ISO speed from the camera
unsigned long CanonEosUsb::GetIsoSpeed()
{
	unsigned long isoSpeedV = GetPropertyValueLong (kEdsPropID_ISOSpeed);
	if (isoSpeedV == 0xffffffff)
		return 0;
	unsigned long isoSpeed = 0;
	for (unsigned int j = 0; j < dim(isoMapping); j++) {
		if (isoMapping[j].val == isoSpeedV) {
			isoSpeed = isoMapping[j].iso;
			break;
			}
		}
	if (isoSpeed == 0) {
		isoSpeed = (unsigned long)(100.0 * pow (2.0, (((double)isoSpeedV)/8.0) - 9.0) + 0.5);
		}
	return isoSpeed;
}

// // Sets the camera ISO speed
bool CanonEosUsb::SetIsoSpeed(unsigned long isoSpeed)
{
	if (isoSpeed == 0)
		return false;
	unsigned long isoValue = 0;
	for (unsigned int j = 0; j < dim(isoMapping); j++) {
		if (isoMapping[j].iso == isoSpeed) {
			isoValue = isoMapping[j].val;
			break;
			}
		}
	if (isoValue == 0) {
		isoValue = (unsigned long)((log10 (((double)isoSpeed)/100.0) / 0.30103 + 9.0) * 8.0 + 0.5);
		}

	SetPropertyValueLong (kEdsPropID_ISOSpeed, isoValue);

	return true;
}

double CanonEosUsb::GetShutterSpeed ()
{
	unsigned long shutterSpeedV = GetPropertyValueLong (kEdsPropID_Tv);
	if (shutterSpeedV == 0xffffffff)
		return 0.0;
	if (shutterSpeedV == SHUTTER_SPEED_BULB)
		return -1.0;
	double shutterSpeed = 0.0;
	for (unsigned int j = 0; j < dim(shutterSpeedMapping); j++) {
		if (shutterSpeedMapping[j].val == shutterSpeedV) {
			switch (shutterSpeedMapping[j].norm) {
				case 'I':
					shutterSpeed = (double) shutterSpeedMapping[j].speed;
					break;
				case 'T':
					shutterSpeed = 10.0 * (double) shutterSpeedMapping[j].speed;
					break;
				case 'R':
					shutterSpeed = 1.0 / (double) shutterSpeedMapping[j].speed;
					break;
				}
			break;
			}
		}
	return shutterSpeed;
}

bool CanonEosUsb::SetShutterSpeed (double shutterSpeed)
{
	return false;
}


