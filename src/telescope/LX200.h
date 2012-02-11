/*
** Name:        LX200.h
** Purpose:     LX200 telescope device
** Author:      Kvi
** Modified by:
** Created:     20/07/11
** Copyright:   (c) 2011 Kvi
** Licence:     wxWindows licence
*/

#ifndef _LX200_H_
#define _LX200_H_

#include "../Telescope.h"

class LX200 :
	public Telescope
{
public:
	LX200(wxString comPort);
	wxString GetMountType();

	wxString GetModel ();
	bool IsConnected ();
	wxString GetVersion ();
	bool IsAligned ();
	bool IsMoving ();
	Coordinate& GetEquatorialCoords ();
	void MoveToEquatorialCoords (Coordinate& coord);
	void StopMoving ();
private:
	bool highPrecision;
	void SetHighPrecision ();
};

#endif // _LX200_H_
