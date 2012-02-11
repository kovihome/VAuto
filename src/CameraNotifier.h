/*
** Name:        CameraNotifier.h
** Purpose:     Background thread for notifying main dialog about camera shooting events
** Author:      Kvi
** Modified by:
** Created:     11/09/11
** Copyright:   (c) 2011 Kvi
** Licence:     wxWindows licence
*/

#ifndef _CAMERA_NOTIFIER_H_
#define _CAMERA_NOTIFIER_H_

#include "Camera.h"
#include <wx/wx.h>
#include <wx/thread.h>

class CameraNotifier :
	public wxThread
{
public:
	enum CameraShootAction {
		SHOOT_STOP_NORMALLY = 100,
		SHOOT_TIMEOUT,
		SHOOT_NOTIFY_TIME,
		SHOOT_ABORT
	};
private:
	Camera* m_camera;
	wxWindow* m_parent;
	bool m_abort;
	int m_id;
	int m_exposureTime;
public:
	CameraNotifier(wxWindow* parent, int id, Camera* camera, int exposureTime);
	virtual ~CameraNotifier() {};
	void OnExit ();
	void Abort ();
private:
	void SendEvent (CameraNotifier::CameraShootAction shootAction, int currentExposureTime = -1);
	ExitCode Entry ();
};

#endif // _CAMERA_NOTIFIER_H_
