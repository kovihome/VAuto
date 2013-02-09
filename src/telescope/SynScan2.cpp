#ifdef __WXMSW__
    #include <wx/msw/msvcrt.h>      // redefines the new() operator 
#endif 

#include "SynScan2.h"

SynScan2::SynScan2(wxString comPort)
	: Telescope (comPort)
{
	m_isMoving = FALSE;
}

wxString SynScan2::GetMountType ()
{
	return "SysScan2";
}

wxString SynScan2::GetModel ()
{
	return "SynScan V2.05";
}

bool SynScan2::IsConnected ()
{
	/*
	** When the mount is moving, we don!t send any command to it, just wait for the end of the movement.
	*/
	if (m_isMoving)
		return TRUE;

	wxString answer = SendCommand ("?", 1);
	return answer.Length() > 0;
}

/*
** No command to get version info.
*/
wxString SynScan2::GetVersion ()
{
	return "2.05";
}

/*
** No command to get align state. Supposing to be aligned.
*/
bool SynScan2::IsAligned ()
{
	return true;
}

/*
** No special command to get moving state, 
**	but the mount does not reply immediately for the start moving command, just after it stopped.
**	In the MoveToEquatorialCoords() we does no wait for the answer, just returns.
**	Here we try to 
*/
bool SynScan2::IsMoving ()
{
	if (!m_isMoving)
		return FALSE;

	wxString answer = SendCommand ("", 1);
//	wxString answer = "@";

	if (answer.Length() > 0 && answer.GetChar(0) == '@') {
		m_isMoving = FALSE;
		return FALSE;
		}
	else {
		return TRUE;
		}
}

Coordinate& SynScan2::GetEquatorialCoords ()
{
	Coordinate* coord = new Coordinate ();

	/*
	** When the mount is moving, we don!t send any command to it, just wait for the end of the movement.
	*/
	if (m_isMoving)
		return *coord;

	wxString answer = SendCommand ("E", 4);
	if (answer.Length() > 0) {
		unsigned short ra = *(unsigned short *) answer.Left(2).c_str();
		unsigned short decl = *(unsigned short *) answer.Mid(2,2).c_str();
		coord->FromBin16 (ra, decl);
		}
	return *coord;
}

void SynScan2::MoveToEquatorialCoords (Coordinate& coord)
{
	unsigned short ra, decl;
	coord.ToBin16 (ra, decl);
	wxString sRa = wxString ((const wxChar*) ra, 2);
	wxString sDecl = wxString ((const wxChar*) decl, 2);
	wxString command = "R" + sRa + sDecl;
	wxString answer = SendCommand (command, 0);
	m_isMoving = TRUE;
}

