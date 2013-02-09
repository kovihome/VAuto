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
	CanonEosUsb(bool needInit = true);
	CanonEosUsb(const wxString& port, bool needInit = true);
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
	bool Connect(void);
	bool Disconnect(void);
	wxString GetPropertyValueString(EdsPropertyID propId);
	unsigned long GetPropertyValueLong(EdsPropertyID propId);
	void SetPropertyValueLong(EdsPropertyID propId, unsigned long value);
	unsigned long GetCFnValueLong (unsigned long cfn);
	void SetCameraTime();
protected:
	wxString GetLastImageFileName(void);
private:
	unsigned long m_isoSpeedOriginal;
public:
	unsigned long GetIsoSpeed();				// Read the ISO speed from the camera
	bool SetIsoSpeed(unsigned long isoSpeed);	// Sets the camera ISO speed
	double GetShutterSpeed ();					// Reads the shutterspeed from the camera
	bool SetShutterSpeed (double shutterSpeed);	// Sets the camera shutter speed
};

#endif // _CANON_EOS_USB_H_
