#include "Telescope.h"
#include "../include/ctb/serport.h"

Telescope::Telescope(wxString comPort, int timeOut, int baudRate)
	: m_comPort(comPort), m_timeOut(timeOut), m_baudRate(baudRate)
{
}

wxString Telescope::GetMountType ()
{
	return "None";
}


wxString Telescope::GetModel ()
{
	return "";
}

wxString Telescope::Slew (Coordinate& coord)
{
	return "";
}

wxString Telescope::SendCommand (wxString command, int expectedAnswerSize)
{
	wxSerialPort* port = new wxSerialPort ();
	if (port->Open (m_comPort) < 0) {
		delete port;
		return "";
		}

//	port->SetBaudRate (m_baudRate);
	port->SetBaudRate (wxBAUD_9600);
	port->Write ((char *) command.c_str(), command.Length());

	int bytesRead = -1;
	char buffer[128];
	if (expectedAnswerSize > 0) {
		int bytesRead = port->Read (buffer, expectedAnswerSize);
		}
	port->Close ();
	delete port;
	if (bytesRead != expectedAnswerSize) {
		return "";
		}
	buffer[expectedAnswerSize] = 0;
	return *new wxString(buffer);
}

void Telescope::ClosePort ()
{
}

bool Telescope::IsConnected ()
{
	return false;
}

wxString Telescope::GetVersion ()
{
	return "";
}

bool Telescope::IsAligned ()
{
	return false;
}

bool Telescope::IsMoving ()
{
	wxLogDebug (wxString::Format ("Telescope::IsMoving do nothing, it is a skeleton class."));
	return false;
}

bool Telescope::WaitUntilMove ()
{
	int guard = 60;
	while (IsMoving () && guard-- > 0) {
		wxThread::Sleep (1000);
		}
	return guard > 0;
}

Coordinate& Telescope::GetEquatorialCoords ()
{
	Coordinate& dummy = *new Coordinate();
	return dummy;
}

void Telescope::MoveToEquatorialCoords (Coordinate& coord)
{
}

void Telescope::StopMoving ()
{
}

void Telescope::SetTracking (int trackingMode)
{
}

