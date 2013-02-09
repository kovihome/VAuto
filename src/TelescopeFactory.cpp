#ifdef __WXMSW__
    #include <wx/msw/msvcrt.h>      // redefines the new() operator 
#endif 

/*
** TelescopeFactory.h -- Identify and create Telescope object
*/

#include "TelescopeFactory.h"
#include "telescope/NexStar.h"
#include "telescope/SynScan2.h"
#include "telescope/LX200.h"

#define MOUNT_NONE		"None"
#define MOUNT_NEXSTAR	"NexStar"
#define MOUNT_SYNSCAN2	"SynScan2"
#define MOUNT_LX200		"LX200"
//#define MOUNT_EQDIRECT	"EQDirect"

TelescopeFactory* TelescopeFactory::instance = NULL;

TelescopeFactory::TelescopeFactory()
{
	ClearTelescope ();
}

TelescopeFactory& TelescopeFactory::Instance()
{
	if (instance == NULL) {
		instance = new TelescopeFactory ();
		}
	return *instance;
}

wxArrayString& TelescopeFactory::EnumerateDrivers ()
{
	wxArrayString& driverNames = *new wxArrayString();
	driverNames.Add (MOUNT_NONE);
	driverNames.Add (MOUNT_NEXSTAR);
	driverNames.Add (MOUNT_SYNSCAN2);
	driverNames.Add (MOUNT_LX200);
//	driverNames.Add (MOUNT_EQDIRECT);
	return driverNames;
}

wxString& TelescopeFactory::GetDriverDisplayName (const wxString& driverType)
{
	if (driverType == MOUNT_NONE)
		return *new wxString(_("No telescope"));
	if (driverType == MOUNT_NEXSTAR)
		return *new wxString(_("NexStar/GoTo"));
	if (driverType == MOUNT_SYNSCAN2)
		return *new wxString(_("SynScan V2.05"));
	if (driverType == MOUNT_LX200)
		return *new wxString(_("LX-200"));
//	if (driverType == MOUNT_EQDIRECT)
//		return *new wxString(_("EQDirect"));
	return *new wxString(_("Unknown"));

}


Telescope* TelescopeFactory::GetTelescope (wxString mountType, wxString comPort)
{
	if (m_mount == NULL) {

		// NexStar/GoTo
		if (mountType == MOUNT_NEXSTAR) {
			m_mount = new NexStar (comPort);
//			if (!m_mount->IsConnected()) {
//				m_mount->ClosePort ();
//				Telescope *mountV205 = new SynScan2 (comPort);
//				if (mountV205->IsConnected ()) {
//					m_mount = mountV205;
//					}
//				else {
//					mountV205->ClosePort ();
//					m_mount = new Telescope (comPort);
//					}
//				}
			}
		else if (mountType == MOUNT_SYNSCAN2) {
			m_mount = new SynScan2 (comPort);
			}

		// LX-200
		else if (mountType == MOUNT_LX200){
			m_mount = new LX200 (comPort);
			}

		// No telescope
		else {
			m_mount = new Telescope ();
			}
		}
	return m_mount;
}

void TelescopeFactory::ClearTelescope ()
{
	m_mount = NULL;
}
