/*
** Name:        Camera.h
** Purpose:     Base class for all camera device classes
** Author:      Kvi
** Modified by:
** Created:     20/07/11
** Copyright:   (c) 2011 Kvi
** Licence:     wxWindows licence
*/

#ifndef _CAMERA_SHOOT_TIMER_H_
#define _CAMERA_SHOOT_TIMER_H_

#include <wx/wx.h>

class Camera;

class CameraShootTimer : public wxTimer
{
public:
	CameraShootTimer (Camera* cameraController) : wxTimer() { m_CameraController = cameraController; }
private:
	Camera* m_CameraController;
	void Notify ();
};

class Camera :
	public wxObject
{
public:
	enum ShootingState {
		CSS_READY = 0,
		CSS_MLU,
		CSS_MLU_DELAY,
		CSS_SHOOTING,
		CSS_SHOOTING_DELAY
		};
	enum DriverFeature {
		DCP_USE_SERIAL = 1,
		DCP_USE_USB,
		DCP_HAS_MLU,
		DCP_MLU_FOR_EVERY_FRAME
		};

	Camera() : m_ShootTimer(this) {};
	Camera(const wxString& port);
	Camera(const Camera& camera);
	virtual ~Camera();
	Camera& operator= (const Camera& camera);

	wxString GetName () { return name; }
	void SetName (wxString name) { this->name = name; }

	virtual wxString GetCameraType ();
	virtual bool HasFeature (DriverFeature feature) { return false; }

	virtual void SetTime (const wxDateTime& time) {}

	virtual void ShootMany (long frameCount, long exposureTime, long delayDeforeShoot);
	virtual void Shoot (long exposureTime);
	virtual void AbortShooting ();
	virtual bool IsShooting () { return (m_shootingState != CSS_READY); }

	void DoNextShootingAction ();

	virtual void GetImage (const wxString& imageFileName) {}

	virtual int GetState () { return 0; }
	virtual wxString GetProperty (const wxString& propertyName) { return ""; }
	virtual void SetProperty (const wxString& propertyName, const wxString& propertyValue) {}

	int GetFrameCount () { return m_frameCount; }
	int GetCurrentFrame () { return m_currentFrame; }
	int GetExposureTime () { return m_exposureTime; }
	ShootingState GetShootingState () { return m_shootingState; }
	int GetDelayBeforeShoot () { return m_delayBeforeShoot; }

protected:

	wxString name;
	wxString m_port;
	long m_frameCount;
	long m_currentFrame;
	long m_exposureTime;
	long m_delayBeforeShoot;
	long m_delayBeforeMLU;
	bool m_useMLU;
	bool m_mluBeforeEveryFrame;
	ShootingState m_shootingState;
	CameraShootTimer m_ShootTimer;

	virtual bool Initialize();
	virtual void Finish ();
	virtual bool FindCamera();
	virtual bool DoInitShoot ();
	virtual bool DoStartShoot ();
	virtual bool DoEndShoot ();
	virtual bool DoTermShoot ();
	virtual bool DoStartMLU ();
	virtual bool DoTermMLU ();

};

#endif // _CAMERA_SHOOT_TIMER_H_
