/*
** Name:        OptionsDialog.h
** Purpose:     VAuto Options dialog window
** Author:      Kvi
** Modified by:
** Created:     26/09/10
** Copyright:   (c) 2011 Kvi
** Licence:     wxWindows licence
*/

#ifndef _VAUTO_OPTIONS_H_
#define _VAUTO_OPTIONS_H_

#include <wx\wx.h>

#define CAMERA_TYPE			"cameraType"
#define CAMERA_PORT			"cameraPort"
#define CAMERA_LOCKUP		"cameraLockup"
#define CAMERA_LOCKUP_DELAY	"cameraLockupDelay"
#define CAMERA_FRAME_DELAY	"cameraFrameDelay"
#define MOUNT_TYPE			"mountType"
#define MOUNT_PORT			"mountPort"
#define MANUAL_GOTO			"manualGoto"

class OptionsDialog :
	public wxDialog
{
public:

	OptionsDialog(wxWindow *parent);

private:

	wxCheckBox* m_cameraLockup;
	wxChoice* m_cameraPort;
	wxChoice* m_telescopePort;
	wxChoice* m_telescopeType;
	wxChoice* m_cameraType;
	wxSpinCtrl* m_lockupDelay;
	wxSpinCtrl* m_frameDelay;
	wxStaticText* m_titleLockupDelay;

	wxStaticText* m_DeviceText;

	wxArrayString m_Telescopes;
	wxArrayString m_Cameras;
	wxArrayString m_ComPorts;

	bool m_CameraUseSerial;
	bool m_CameraUseUsb;
	bool m_CameraHasMLU;

	void FillCombo(wxChoice *combo, wxArrayString& items, int selectedIndex);
	void FillPortCombo(wxChoice * combo, wxArrayString& ports, wxString& defPort, bool isCamera);
	void DetermineCameraCapabilities ();
	void OnOK(wxCommandEvent& ev);
	void OnCancel(wxCommandEvent& ev);
	void OnLockup(wxCommandEvent& ev);
	void OnChangeTelescope (wxCommandEvent& ev);
	void OnChangeCamera (wxCommandEvent& ev);

	void SetDeviceText ();

	DECLARE_EVENT_TABLE();
};

#endif // _VAUTO_OPTIONS_H_
