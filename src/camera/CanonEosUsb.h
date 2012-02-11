/*
** Name:        CanonEosUsb.h
** Purpose:     Canon EOS device with USB control
** Author:      Kvi
** Modified by:
** Created:     29/08/11
** Copyright:   (c) 2011 Kvi
** Licence:     wxWindows licence
*/

#ifndef _CANON_EOS_USB_H_
#define _CANON_EOS_USB_H_

#include "../Camera.h"
#include "../../include/canon/EDSDK.h"

class CanonEosUsb :
	public Camera
{
public:
	CanonEosUsb();
	CanonEosUsb(const wxString& port);
	~CanonEosUsb();

	wxString GetCameraType ();
	bool HasFeature (DriverFeature feature);

private:
	EdsCameraListRef m_CameraList;
	EdsCameraRef m_Camera;

	bool m_BulbCommandUsed;

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

#endif // _CANON_EOS_USB_H_
