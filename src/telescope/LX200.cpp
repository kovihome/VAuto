#ifdef __WXMSW__
    #include <wx/msw/msvcrt.h>      // redefines the new() operator 
#endif 

#include "LX200.h"

LX200::LX200(wxString comPort)
	: Telescope (comPort)
{
	highPrecision = false;
}

wxString LX200::GetMountType ()
{
	return "LX200";
}



wxString LX200::GetModel ()
{
	return "LX-200 V" + GetVersion ();
}

// ?????????
bool LX200::IsConnected ()
{
	wxString answer = SendCommand ("Kx", 2);
	return answer.Length() > 0;
}

wxString LX200::GetVersion ()
{
	wxString answer = SendCommand (":GVN#", 5);
	return wxString::Format ("%c.%c", answer.GetChar(0), answer.GetChar(1)); // ?????????
}

// ?????????
bool LX200::IsAligned ()
{
	wxString answer = SendCommand ("J", 2);
	return answer.Length() > 0 && answer.GetChar(0) == 1;
}

// ?????????
bool LX200::IsMoving ()
{
	wxString answer = SendCommand ("L", 2);
	return answer.Length() > 0 && answer.GetChar(0) == '1';
}

void LX200::SetHighPrecision ()
{
	if (highPrecision == true) {
		return;
		}
	SendCommand (":U#", 0);
	highPrecision = true;
}

Coordinate& LX200::GetEquatorialCoords ()
{
	SetHighPrecision ();

	Coordinate* coord = new Coordinate ();
	wxString answerRa = SendCommand (":GR#", 9);
	wxString answerDecl = SendCommand (":GD#", 10);
	if (answerRa.Length() > 0 && answerDecl.Length() > 0) {
		coord->FromHex24 (answerRa.Left (6), answerRa.Mid (9, 15));	// ?????????
		}
	return *coord;
}
	
void LX200::MoveToEquatorialCoords (Coordinate& coord)
{
	SetHighPrecision ();

	wxString sRa, sDecl;
	coord.ToHex24(sRa, sDecl);								// ?????????
	wxString command = "r" + sRa + "00," + sDecl + "00";	// ?????????
	wxString answer = SendCommand (":SrHH:MM:SS#", 1);		// ?????????
	answer = SendCommand (":SdsDD*MM:SS#", 1);				// ?????????
	answer = SendCommand (":MS#", 1);
}

void LX200::StopMoving ()
{
	wxString answer = SendCommand (":Q#", 1);
}
