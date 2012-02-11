/*
** Name:        CanonEosShutter.h
** Purpose:     Canon EOS device with Serial Shutter control
** Author:      Kvi
** Modified by:
** Created:     29/08/11
** Copyright:   (c) 2011 Kvi
** Licence:     wxWindows licence
*/

#ifndef _CANON_EOS_SHUTTER_H_
#define _CANON_EOS_SHUTTER_H_

#include "../Camera.h"
#include "../../include/ctb/serport.h"

class CanonEosShutter :
	public Camera
{
public:
	CanonEosShutter() : Camera() {}
	CanonEosShutter(const wxString& port) : Camera(port) {}
	~CanonEosShutter();
	wxString GetCameraType ();
	bool HasFeature (DriverFeature feature);

private:
	wxSerialPort* m_SerialPort;

	bool Initialize();
	bool FindCamera();
	void Finish ();
	bool DoInitShoot ();
	bool DoStartShoot ();
	bool DoEndShoot ();
	bool DoTermShoot ();
	bool DoStartMLU ();
	bool DoTermMLU ();
};

#endif // _CANON_EOS_SHUTTER_H_
