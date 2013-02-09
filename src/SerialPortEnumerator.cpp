#ifdef __WXMSW__
    #include <wx/msw/msvcrt.h>      // redefines the new() operator 
#endif 

/*
** Serial Port Enumerator utility class
*/

// For compilers that support precompilation, includes "wx.h".
#include <wx/wxprec.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

// includes
#ifndef WX_PRECOMP
#include <wx/log.h>
#endif

#include "SerialPortEnumerator.h"
#include <wx/arrstr.h>

#if defined(__WXOSX__) || defined(__WXGTK__)
//	#include <termios.h>
	#include <sys/ioctl.h>
//	#include <getopt.h>
	#include <dirent.h>
#elif defined(__WXMSW__)
	#include <setupapi.h>
	#define MAX_SERIAL_PORTS 256
	#ifdef __MINGW32__
		#define INITGUID
		#include <initguid.h> // needed for dev-c++ & DEFINE_GUID
	#endif
#endif


SerialPortEnumerator::SerialPortEnumerator()
{
}

SerialPortEnumerator::~SerialPortEnumerator()
{
}

#ifdef __WXMSW__
// needed for serial bus enumeration:
//4d36e978-e325-11ce-bfc1-08002be10318}
	DEFINE_GUID (GUID_SERENUM_BUS_ENUMERATOR, 0x4D36E978, 0xE325, 0x11CE, 0xBF, 0xC1, 0x08, 0x00, 0x2B, 0xE1, 0x03, 0x18);
#endif

wxArrayString& SerialPortEnumerator::Enumerate ()
{
	wxArrayString *portCollection = new wxArrayString ();

#ifdef __WXMSW__
	
	HDEVINFO hDevInfo = NULL;
	SP_DEVINFO_DATA DeviceInterfaceData;
	int i = 0;
	DWORD dataType, actualSize = 0;
	unsigned char dataBuf[MAX_PATH + 1];
	
	// Search device set
	hDevInfo = SetupDiGetClassDevs((struct _GUID *)&GUID_SERENUM_BUS_ENUMERATOR,0,0,DIGCF_PRESENT);
	if (hDevInfo) {
		while (TRUE) {
			ZeroMemory(&DeviceInterfaceData, sizeof(DeviceInterfaceData));
			DeviceInterfaceData.cbSize = sizeof(DeviceInterfaceData);
			if (!SetupDiEnumDeviceInfo(hDevInfo, i, &DeviceInterfaceData)) {
				// SetupDiEnumDeviceInfo failed
				break;
			}
		
			if (SetupDiGetDeviceRegistryProperty (hDevInfo, &DeviceInterfaceData, SPDRP_FRIENDLYNAME, &dataType,
				 dataBuf, sizeof(dataBuf), &actualSize)) {
			
//				sprintf(portNamesFriendly[nPorts], "%s", dataBuf);
//				portNamesShort[nPorts][0] = 0;
				
				// turn blahblahblah(COM4) into COM4
			
				char *begin = NULL;
				char *end = NULL;
				begin = wxStrstr((char *)dataBuf, "COM");
			    if (begin) {
					end = wxStrstr(begin, ")");
					if (end) {
						*end = 0;   // get rid of the )...
						portCollection->Add (*new wxString (begin));
						}
					}
				}
			i++;
			}
		}
	SetupDiDestroyDeviceInfoList(hDevInfo);
	

#endif

#if defined(__WXGTK__) || defined(__WXOSX__)
	struct dirent *entry;
	DIR *dir = opendir("/dev");
	wxString str = "";
	if (dir != NULL) {
        printf("ofSerial: listing devices\n");
        while ((entry = readdir(dir)) != NULL) {
			str = new wxString ((char *)entry->d_name);
			if ( str.Left(3) == "cu." ) {
				portCollection->Add (str);
				}
		    }
		}
#endif

	return *portCollection;
}


