/*
** Name:        CameraFactory.h
** Purpose:     Factory class to create camera object from camera type string
** Author:      Kvi
** Modified by:
** Created:     20/07/11
** Copyright:   (c) 2011 Kvi
** Licence:     wxWindows licence
*/

#ifndef _CAMERA_FACTORY_H_
#define _CAMERA_FACTORY_H_

#include "Camera.h"
#include <wx/wx.h>
#include <wx/dynarray.h>
#include <wx/arrstr.h>

/*
** Camera object (Canon cameras)
**
**		status: UILock - user interface disabled (substate of DirectTransferRelease)
**				UILockRelease - user interface enabled (substate of DirectTransferRelease)
**				DirectTransfer - currently data is transferred
**				DirectTransferRelease - data is not transferred currently
**		properties:	AtCapture !!! 
**					Product name
**					Body ID
**					Owner
**					Manufacturer
**					System Time !!!
**					Firmware version
**					Battery State !!!
**					Custom Function Settings !!!
**					Personal Function Settings
**					Image Destination
**					Linear Processing On/Off
**					WB control value
**					Shootimg Mode
**					Drive Mode
**					ISO Setting Value !!!
**					Shutter Speed !!!
**					Number of Available Shots !!!
**					Noise reduction
**					Flash Usage
**					Red-eye reduction
**					Lens state

*/

WX_DECLARE_OBJARRAY (Camera, CameraArray);

class CameraFactory
{
private:
	CameraFactory();
	CameraFactory(const CameraFactory&);
	CameraFactory& operator= (CameraFactory&);

	static CameraFactory* instance;
	Camera* m_camera; 
public:
	static CameraFactory& Instance();
	wxArrayString& EnumerateDrivers ();
	wxString& GetDriverDisplayName (const wxString& driverType);
	CameraArray EnumerateCameras ();
	Camera* GetCamera (const wxString& cameraType, const wxString& comPort);
	void ClearCamera ();

};

#endif // _CAMERA_FACTORY_H_
