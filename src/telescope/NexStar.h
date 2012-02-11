/*
** Name:        NexStar.h
** Purpose:     NexStar telescope device
** Author:      Kvi
** Modified by:
** Created:     20/07/11
** Copyright:   (c) 2011 Kvi
** Licence:     wxWindows licence
*/

#ifndef _NEXSTAR_H_
#define _NEXSTAR_H_

#include "../Telescope.h"

class NexStar :
	public Telescope
{
public:
	NexStar(wxString comPort);
	wxString GetMountType();

	wxString GetModel ();
	bool IsConnected ();
	wxString GetVersion ();
	bool IsAligned ();
	bool IsMoving ();
	Coordinate& GetEquatorialCoords ();
	void MoveToEquatorialCoords (Coordinate& coord);
	void StopMoving ();
};

#endif // _NEXSTAR_H_
