#ifdef __WXMSW__
    #include <wx/msw/msvcrt.h>      // redefines the new() operator 
#endif 

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

#define READ_BUFFER_SIZE	128
#define READ_POLL_TIME		100 // ms
#define READ_TIMEOUT		3500 // ms

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

	char *buffer = new char[READ_BUFFER_SIZE + 1];
	buffer[READ_BUFFER_SIZE] = 0;
	char *bufferPtr = buffer;

	int timeout = READ_TIMEOUT; // ms
	int bytesRead = 0;
	int br;

	while (timeout > 0) {
		wxThread::Sleep (READ_POLL_TIME);
		br = port->Read (bufferPtr, READ_BUFFER_SIZE - bytesRead);
		if (br == 0 && bufferPtr != buffer)
			break;
		if (br != 0) {
			bufferPtr += br * sizeof(char);
			bytesRead += br;
			if (bytesRead >= READ_BUFFER_SIZE) {
				wxLogDebug ("Serial Buffer is full, command: %s, answer: %s", command, buffer);
				port->Close ();
				delete buffer;
				delete port;
				return "";
				}
			}
		timeout -= READ_POLL_TIME;
		}
//	if (expectedAnswerSize > 0) {
//		int bytesRead = port->Read (buffer, expectedAnswerSize);
//		}
	port->Close ();
	delete port;
//	if (bytesRead != expectedAnswerSize) {
//		return "";
//		}
//	buffer[expectedAnswerSize] = 0;
	buffer[bytesRead] = 0;
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

