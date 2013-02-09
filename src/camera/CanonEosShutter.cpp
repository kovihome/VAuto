#ifdef __WXMSW__
    #include <wx/msw/msvcrt.h>      // redefines the new() operator 
#endif 

#include "../common.h"
#include "CanonEosShutter.h"

CanonEosShutter::~CanonEosShutter()
{
}

wxString CanonEosShutter::GetCameraType ()
{
	return "CanonEosShutter";
}

bool CanonEosShutter::HasFeature (DriverFeature feature)
{
	switch (feature) {
		case DCP_USE_SERIAL:
		case DCP_HAS_MLU:
		case DCP_MLU_FOR_EVERY_FRAME:
			return true;
		}
	return false;
}

bool CanonEosShutter::FindCamera()
{
	return true;
}

bool CanonEosShutter::Initialize()
{
	wxLogDebug ("Initializing Canon Shutter");
	m_SerialPort = new wxSerialPort ();
	return true;
}

void CanonEosShutter::Finish ()
{
	wxLogDebug ("Terminating Canon Shutter");
#if SIMULATE_CONNECTION_CAMERA == 0
	if (m_SerialPort->IsOpen ())
		m_SerialPort->Close ();
#endif
	delete m_SerialPort;
}

bool CanonEosShutter::DoInitShoot ()
{
#if SIMULATE_CONNECTION_CAMERA == 0
#endif
	return true;
}

bool CanonEosShutter::DoStartShoot ()
{
#if SIMULATE_CONNECTION_CAMERA == 0
	m_SerialPort->Open (m_port);
#endif
	return true;
}

bool CanonEosShutter::DoEndShoot ()
{
#if SIMULATE_CONNECTION_CAMERA == 0
	if (m_SerialPort->IsOpen())
		m_SerialPort->Close ();
#endif
	return true;
}

bool CanonEosShutter::DoTermShoot ()
{
#if SIMULATE_CONNECTION_CAMERA == 0
#endif
	wxLogDebug (_("End shooting."));
	return true;
}

bool CanonEosShutter::DoStartMLU ()
{
#if SIMULATE_CONNECTION_CAMERA == 0
	m_SerialPort->Open (m_port);
#endif
	wxThread::Sleep (1000);
#if SIMULATE_CONNECTION_CAMERA == 0
	if (m_SerialPort->IsOpen())
		m_SerialPort->Close ();
#endif
	return true;
}

bool CanonEosShutter::DoTermMLU ()
{
	return true;
}



