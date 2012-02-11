/*
** Name:        SynScan2.h
** Purpose:     SynScan V2.05 telescope device
** Author:      Kvi
** Modified by:
** Created:     20/07/11
** Copyright:   (c) 2011 Kvi
** Licence:     wxWindows licence
*/

#ifndef _SYNSCAN2_H_
#define _SYNSCAN2_H_

#include "../Telescope.h"

class SynScan2 :
	public Telescope
{
public:
	SynScan2(wxString comPort);
	wxString GetMountType();

	wxString GetModel ();
	bool IsConnected ();
	wxString GetVersion ();
	bool IsAligned ();
	bool IsMoving ();
	Coordinate& GetEquatorialCoords ();
	void MoveToEquatorialCoords (Coordinate& coord);

private:
	bool m_isMoving;
};

#endif _SYNSCAN2_H_
