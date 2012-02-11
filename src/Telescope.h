/*
** Name:        Telescope.h
** Purpose:     Base class for Telescope Devices
** Author:      Kvi
** Modified by:
** Created:     20/07/11
** Copyright:   (c) 2011 Kvi
** Licence:     wxWindows licence
*/

#ifndef _TELESCOPE_H_
#define _TELESCOPE_H_

#include "Coordinate.h"
#include <wx/wx.h>

class Telescope
{
public:
	Telescope() {};
	Telescope(wxString comPort, int timeOut = 3500, int baudRate = 9600);
	virtual wxString GetMountType ();

	virtual wxString GetModel ();
	virtual wxString Slew (Coordinate& coord);
	virtual wxString SendCommand (wxString command, int expectedAnswerSize);
	virtual void ClosePort ();
	virtual bool IsConnected ();
	virtual wxString GetVersion ();
	virtual bool IsAligned ();
	virtual bool IsMoving ();
	virtual bool WaitUntilMove ();
	virtual Coordinate& GetEquatorialCoords ();
	virtual void MoveToEquatorialCoords (Coordinate& coord);
	virtual void StopMoving ();
	virtual void SetTracking (int trackingMode);
private:
	wxString m_comPort;
	int m_timeOut;
	int m_baudRate;
};

#endif // _TELESCOPE_H_
