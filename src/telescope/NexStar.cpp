#include "NexStar.h"

#define SIMULATE_CONNECTION 1

#if SIMULATE_CONNECTION == 1
static bool moving = FALSE;
static wxDateTime moveuntil;
#endif

NexStar::NexStar(wxString comPort)
	: Telescope (comPort)
{
}

wxString NexStar::GetMountType ()
{
	return "NexStar";
}


wxString NexStar::GetModel ()
{
	return "NexStar V" + GetVersion ();
}

bool NexStar::IsConnected ()
{
	wxLogDebug (wxString::Format ("NexStar::IsConnected send command: Kx"));
#if SIMULATE_CONNECTION == 0
	wxString answer = SendCommand ("Kx", 2);
#else
	wxString answer = "x#";
#endif
	wxLogDebug (wxString::Format ("NexStar::IsConnected recieve answer: %s", answer));
	return answer.Length() > 0;
}

wxString NexStar::GetVersion ()
{
	wxLogDebug (wxString::Format ("NexStar::GetVersion send command: V"));
#if SIMULATE_CONNECTION == 0
	wxString answer = SendCommand ("V", 3);
#else
	wxString answer = "22#";
#endif
	wxLogDebug (wxString::Format ("NexStar::GetVersion recieve answer: %s", answer));
	return wxString::Format ("%c.%c", answer.GetChar(0), answer.GetChar(1));
}

bool NexStar::IsAligned ()
{
	wxLogDebug (wxString::Format ("NexStar::IsAligned send command: J"));
#if SIMULATE_CONNECTION == 0
	wxString answer = SendCommand ("J", 2);
#else
	wxString answer = "\x01#";
#endif
	wxLogDebug (wxString::Format ("NexStar::IsAligned recieve answer: %s", answer));
	return answer.Length() > 0 && answer.GetChar(0) == 1;
}

bool NexStar::IsMoving ()
{
	wxLogDebug (wxString::Format ("NexStar::IsMoving send command: L"));
#if SIMULATE_CONNECTION == 0
	wxString answer = SendCommand ("L", 2);
#else
	wxString answer;
	if (moving && moveuntil.IsEarlierThan (wxDateTime::Now())) {
		moving = FALSE;
		}
	if (moving)
		answer = "1#";
	else
		answer = "0#";
#endif
	wxLogDebug (wxString::Format ("NexStar::IsMoving recieve answer: %s", answer));
	return answer.Length() > 0 && answer.GetChar(0) == '1';
}

Coordinate& NexStar::GetEquatorialCoords ()
{
	Coordinate* coord = new Coordinate ();
	wxLogDebug (wxString::Format ("NexStar::GetEquatorialCoords send command: e"));
#if SIMULATE_CONNECTION == 0
	wxString answer = SendCommand ("e", 18);
#else
	wxString answer = "12345600,12345600#";
#endif
	wxLogDebug (wxString::Format ("NexStar::GetEquatorialCoords recieve answer: %s", answer));
	if (answer.Length() > 0) {
		coord->FromHex24 (answer.Left (6), answer.Mid (9, 15));
		}
	return *coord;
}

void NexStar::MoveToEquatorialCoords (Coordinate& coord)
{
	wxString sRa, sDecl;
	coord.ToHex24(sRa, sDecl);
	wxString command = "r" + sRa + "00," + sDecl + "00";
	wxLogDebug (wxString::Format ("NexStar::MoveToEquatorialCoords send command: %s", command));
#if SIMULATE_CONNECTION == 0
	wxString answer = SendCommand (command, 1);
#else
	moving = TRUE;
	wxDateTime now = wxDateTime::Now ();
	moveuntil = now.Add (*new wxTimeSpan(0,0,10));
	wxString answer = "#";
#endif
	wxLogDebug (wxString::Format ("NexStar::MoveToEquatorialCoords recieve answer: %s", answer));
}

void NexStar::StopMoving ()
{
	wxLogDebug (wxString::Format ("NexStar::StopMoving send command: M"));
#if SIMULATE_CONNECTION == 0
	wxString answer = SendCommand ("M", 1);
#else
	moving = FALSE;
	wxString answer = "#";
#endif
	wxLogDebug (wxString::Format ("NexStar::StopMoving recieve answer: %s", answer));
}
