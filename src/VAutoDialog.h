/*
** Name:        VAutoDialog.h
** Purpose:     VAuto main dialog window
** Author:      Kvi
** Modified by:
** Created:     26/09/10
** Copyright:   (c) 2011 Kvi
** Licence:     wxWindows licence
*/

#ifndef _VAUTO_DIALOG_H_
#define _VAUTO_DIALOG_H_

#include "Camera.h"
#include "Target.h"
#include "Telescope.h"
#include "TelescopeNotifier.h"
#include "CameraNotifier.h"
#include "Catalog.h"
#include <wx/wx.h>
#include <wx/listctrl.h>
#include <wx/log.h>

class VAutoDialog : public wxDialog
{
public:
	VAutoDialog ();
	~VAutoDialog ();
private:

	enum State {
		STATE_READY = 0,
		STATE_MOVING,
		STATE_SHOOTING
		} m_State;
	bool m_Auto;

	// controls
	wxTextCtrl* m_ObjectName;
	wxStaticText* m_DisplayName;
	wxStaticText* m_DisplayRa;
	wxStaticText* m_DisplayDecl;
	wxStaticText* m_PlanFile;
	wxStaticText* m_StatusBar;
	wxButton* m_MoveButton;
	wxButton* m_ShootButton;
	wxButton* m_AbortButton;
	wxButton* m_AutoButton;
	wxButton* m_SkipButton;
	wxBitmapButton* m_FindObject;
	wxBitmapButton* m_buttonOptions;
	wxBitmapButton* m_buttonOpenPlan;
	wxBitmapButton* m_buttonDeletePlan;
	wxSpinCtrl* m_Duration;
	wxSpinCtrl* m_FrameCount;
	wxListCtrl* m_PlanList;

	wxStaticText* m_DeviceText;

	wxString m_logFile;
	wxString m_comPort;
	bool m_lockup;
	int m_lockupDelay;
	int m_frameDelay;
	wxString m_comPortEq;
	bool m_manualGoto;
	int m_cameraType;
	wxString m_mountType;

	Target m_CurrentTarget;
	Target m_saveCurrentTarget;
	
	Catalog m_Plan;
	bool m_HasPlan;

	Telescope* m_telescope;
	TelescopeNotifier* m_telescopeNotifier;

	Camera* m_camera;
	CameraNotifier* m_cameraNotifier;

	wxLogWindow* m_LogWindow;

	// event handler functions
	void OnInitDialog(wxInitDialogEvent& event);
	void OnOptions(wxCommandEvent &ev);
	void OnOpenPlan(wxCommandEvent& ev);
	void OnDeletePlan(wxCommandEvent& ev);
	void OnAuto(wxCommandEvent& ev);
	void OnFindObject(wxCommandEvent& ev);
	void OnAbort(wxCommandEvent& ev);
	void OnExit(wxCommandEvent& ev);
	void OnAbout(wxCommandEvent& ev);
	void OnMove(wxCommandEvent& ev);
	void OnShoot(wxCommandEvent& ev);
	void OnSkip(wxCommandEvent& ev);
	void OnNotifyTelescopeStop (wxCommandEvent& ev);
	void OnNotifyCameraShoot (wxCommandEvent& ev);
	void OnSelectPlanItem (wxListEvent& ev);

	// worker funtions
	void DoDeletePlan();
	void DoInvokeOptionsDialog();
	void DoDisableControls (bool inProgress);
	void Abort();
	void DisplayCurrentTarget ();
	bool DoMove ();
	bool DoShoot ();


	void Log(wxString& status, int frameCount = -1);
	void SetStatusText(const wxString& text);
	void StartExposition(bool fileControlled);
	bool NextPlanTarget();

	void SetState (State state);
	State GetState ();

	void SetDeviceText ();

	DECLARE_EVENT_TABLE();
};

#endif // _VAUTO_DIALOG_H_
