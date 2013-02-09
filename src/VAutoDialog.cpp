#include "VAutoDialog.h"
#include "OptionsDialog.h"
#include "AboutDialog.h"
#include "CameraFactory.h"
#include "Catalog.h"
#include "CatalogFactory.h"
#include "TelescopeFactory.h"
#include "Target.h"
#include "TelescopeNotifier.h"
#include "CameraNotifier.h"
#include "ObservationLog.h"
#include "Station.h"
#include "../../VTools/src/Properties.h"
#include <wx/xrc/xmlres.h>
#include <wx/filename.h>
#include <wx/textfile.h>
#include <wx/spinctrl.h>
#include <wx/stdpaths.h>
#include <wx/dir.h>

#define APP_NAME		"VAuto"
#define APP_VERSION		"1.2"
#define APP_BUILD		"1003"
#define APP_COPYRIGHTS	"Copyrights (c) Kvi, 2009-2012." 
#define CONFIG_FILE		"vauto.cfg"

#define CATALOG_NAME_GCVS		"gcvs"
#define CATALOG_NAME_NSV		"nsv"
#define CATALOG_NAME_NGC		"ngc"
#define CATALOG_NAME_IC			"ic"
#define CATALOG_NAME_PROGRAM	"program"

#define BGCOLOR_LISTCTRL_ODD	RGB(235,235,254)
#define BGCOLOR_LISTCTRL_EVEN	RGB(255,255,255)
#define BGCOLOR_LISTCTRL_HILITE	RGB(192,192,250)

#include "../res/plan_ok.xpm"
#include "../res/plan_up.xpm"
#include "../res/plan_down.xpm"
#include "../res/plan_disabled.xpm"
//#include "../res/plan_moon.xpm"

#define PLAN_ICON_OK		0
#define PLAN_ICON_UP		1
#define PLAN_ICON_DOWN		2
#define PLAN_ICON_DISABLED	3
//#define PLAN_ICON_MOON		4

//constellations = [
//	'And','Ant','Aps','Aqr','Aql','Ara','Ari','Aur','Boo','Cae','Cam','Cnc',
//	'CVn','CMa','CMi','Cap','Car','Cas','Cen','Cep','Cet','Cha','Cir','Col',
//	'Com','CrA','CrB','Crv','Crt','Cru','Cyg','Del','Dor','Dra','Equ','Eri',
//	'For','Gem','Gru','Her','Hor','Hya','Hyi','Ind','Lac','Leo','LMi','Lep',
//	'Lib','Lup','Lyn','Lyr','Men','Mic','Mon','Mus','Nor','Oct','Oph','Ori',
//	'Pav','Peg','Per','Phe','Pic','Psc','PsA','Pup','Pyx','Ret','Sge','Sgr',
//	'Sco','Scl','Sct','Ser','Sex','Tau','Tel','Tri','TrA','Tuc','UMa','UMi',
//	'Vel','Vir','Vol','Vul'
//	]
 
//def FormatObjectName (name):
//	r = name.split()
//	if len(r) == 2:
//		lp = r[1].lower()
//		for con in constellations:
//			if lp == con.lower():
//				return r[0].upper() + ' ' + con
//	return name.upper()
 
#define TELESCOPE_NOTIFIER_ID	999
#define CAMERA_NOTIFIER_ID		998

BEGIN_EVENT_TABLE (VAutoDialog,wxDialog)
	EVT_INIT_DIALOG	(VAutoDialog::OnInitDialog)
	EVT_BUTTON (XRCID ("ID_OPTIONS"), VAutoDialog::OnOptions)
	EVT_BUTTON (XRCID ("ID_OPENPLAN"), VAutoDialog::OnOpenPlan)
	EVT_BUTTON (XRCID ("ID_DELETEPLAN"), VAutoDialog::OnDeletePlan)
	EVT_BUTTON (XRCID ("ID_ABOUT"), VAutoDialog::OnAbout)
	EVT_BUTTON (XRCID ("wxID_EXIT"), VAutoDialog::OnExit)
	EVT_BUTTON (XRCID ("ID_FIND_OBJECT"), VAutoDialog::OnFindObject)
	EVT_BUTTON (XRCID ("ID_AUTO"), VAutoDialog::OnAuto)
	EVT_BUTTON (XRCID ("ID_ABORT"), VAutoDialog::OnAbort)
	EVT_BUTTON (XRCID ("ID_MOVE"), VAutoDialog::OnMove)
	EVT_BUTTON (XRCID ("ID_SHOOT"), VAutoDialog::OnShoot)
	EVT_BUTTON (XRCID ("ID_SKIP"), VAutoDialog::OnSkip)
	// ID_OBJNAME -- find object when press Enter
	EVT_LIST_ITEM_SELECTED (XRCID("ID_PLAN_LIST"), VAutoDialog::OnSelectPlanItem)
	EVT_BUTTON (TELESCOPE_NOTIFIER_ID, VAutoDialog::OnNotifyTelescopeStop)
	EVT_BUTTON (CAMERA_NOTIFIER_ID, VAutoDialog::OnNotifyCameraShoot)
END_EVENT_TABLE()


VAutoDialog::VAutoDialog ()
{
	wxXmlResource::Get()->LoadDialog(this, NULL, "MainDialog");
	SetIcon (wxIcon ("res/dslr.ico"));

	m_DeviceText = XRCCTRL(*this, "DEVICE_TEXT", wxStaticText);

	m_FindObject = XRCCTRL(*this, "ID_FIND_OBJECT", wxBitmapButton);
	m_buttonOptions = XRCCTRL(*this, "ID_OPTIONS", wxBitmapButton);
	m_buttonOpenPlan = XRCCTRL(*this, "ID_OPENPLAN", wxBitmapButton);
	m_buttonDeletePlan = XRCCTRL(*this, "ID_DELETEPLAN", wxBitmapButton);

	m_MoveButton = XRCCTRL(*this, "ID_MOVE", wxButton);
	m_ShootButton = XRCCTRL(*this, "ID_SHOOT", wxButton);
	m_AbortButton = XRCCTRL(*this, "ID_ABORT", wxButton);
	m_AutoButton = XRCCTRL(*this, "ID_AUTO", wxButton);
	m_SkipButton = XRCCTRL(*this, "ID_SKIP", wxButton);

	m_StatusBar = XRCCTRL(*this, "ID_STATUS_BAR", wxStaticText);
	m_StatusCoords = XRCCTRL(*this, "ID_STATUS_COORDS", wxStaticText);

	m_ObjectName = XRCCTRL(*this, "ID_OBJNAME", wxTextCtrl);

	m_DisplayName = XRCCTRL(*this, "ID_NAME", wxStaticText);
	m_DisplayRa = XRCCTRL(*this, "ID_RA", wxStaticText);
	m_DisplayDecl = XRCCTRL(*this, "ID_DECL", wxStaticText);

	m_Duration = XRCCTRL(*this, "ID_DURATION", wxSpinCtrl);
	m_FrameCount = XRCCTRL(*this, "ID_FRAMECOUNT", wxSpinCtrl);

	m_PlanList = XRCCTRL(*this, "ID_PLAN_LIST", wxListCtrl);
	wxSize s = m_PlanList->GetSize();
	int sss = (s.GetWidth () - 24) / 2;
	m_PlanList->InsertColumn (0, "", wxLIST_FORMAT_LEFT, 24);
	m_PlanList->InsertColumn (1, _("Object name"), wxLIST_FORMAT_LEFT, sss);
	m_PlanList->InsertColumn (2, _("Coords"), wxLIST_FORMAT_LEFT, sss);
	m_PlanFile = XRCCTRL(*this, "ID_PLANFILE", wxStaticText);

	wxImageList* imageList = new wxImageList (13, 13);
	imageList->Add (wxBitmap (plan_ok_xpm));
	imageList->Add (wxBitmap (plan_up_xpm));
	imageList->Add (wxBitmap (plan_down_xpm));
	imageList->Add (wxBitmap (plan_disabled_xpm));
//	imageList->Add (wxBitmap (plan_moon_xpm));
	m_PlanList->SetImageList (imageList, wxIMAGE_LIST_SMALL);

}

VAutoDialog::~VAutoDialog ()
{
	delete m_PlanList->GetImageList (wxIMAGE_LIST_SMALL);
	delete m_PlanList;
	delete m_DeviceText;
	delete m_FindObject;
	delete m_buttonOptions;
	delete m_buttonOpenPlan;
	delete m_buttonDeletePlan;
	delete m_MoveButton;
	delete m_ShootButton;
	delete m_AbortButton;
	delete m_AutoButton;
	delete m_SkipButton;
	delete m_StatusBar;
	delete m_ObjectName;
	delete m_DisplayName;
	delete m_DisplayRa;
	delete m_DisplayDecl;
	delete m_Duration;
	delete m_FrameCount;
	delete m_PlanFile;
}


void VAutoDialog::OnInitDialog (wxInitDialogEvent& event)
{
	m_LogWindow = new wxLogWindow (this, "VAuto Log", false);
	wxLogDebug (wxString::Format ("%s, V%s", APP_NAME, APP_VERSION));
	wxLogDebug (wxString::Format ("%s", APP_COPYRIGHTS));

	m_Auto = false;
	SetState (STATE_READY);

	SetStatusCoords ("");

	m_HasPlan = false;

	wxStandardPaths sp;
	wxString logPath = sp.GetDataDir();
	wxString planPath = sp.GetDocumentsDir();

	/*
	** VAuto properties defaults and load
	*/
	Properties::Instance().Put (CAMERA_TYPE, "Canon EOS USB");	// Canon EOS on USB
	Properties::Instance().Put (MOUNT_TYPE, "NexStar");			// NexStar/GoTo mount
	Properties::Instance().Put (CAMERA_PORT, "COM1");
	Properties::Instance().Put (CAMERA_LOCKUP, true);
	Properties::Instance().Put (CAMERA_LOCKUP_DELAY, 5);
	Properties::Instance().Put (CAMERA_FRAME_DELAY, 5);
	Properties::Instance().Put (CAMERA_ISO_SPEED, "800");
	Properties::Instance().Put (CAMERA_BATTERY_MIN_LEVEL, 30);
	Properties::Instance().Put (MOUNT_PORT, "COM2");
	Properties::Instance().Put (MANUAL_GOTO, true);
	Properties::Instance().Put (STATION_LONGITUDE, "");
	Properties::Instance().Put (STATION_LATITUDE, "");
	Properties::Instance().Put (DEBUG_LOG_WINDOW, true);
	Properties::Instance().Put (NEXT_FRAMESTACK_ID, 1);
	Properties::Instance().Put (OBSERVATION_LOG_FOLDER, logPath);
	Properties::Instance().Put (PLAN_FOLDER, planPath);
	Properties::Instance().Load (CONFIG_FILE);

	/*
	** Create telescope
	*/
	m_telescope = TelescopeFactory::Instance().GetTelescope (
		Properties::Instance().Get(MOUNT_TYPE),
		Properties::Instance().Get(MOUNT_PORT));

	/*
	** Create camera
	*/
	m_camera = CameraFactory::Instance().GetCamera (
		Properties::Instance().Get (CAMERA_TYPE),
		Properties::Instance().Get (CAMERA_PORT));

	/*
	** TODO: Lehet hogy egszerûbb lenne, ha a kamera megkapná az egész Properties cuccot, és
	**	beállítaná magának, amit lehet. Ugyan ez az opciók módosításánál.
	*/
	if (m_camera != NULL)
	{
		if (m_camera->HasFeature (Camera::DCP_CAN_SET_ISO_SPEED)) {
			unsigned long isoSpeed = wxAtol (Properties::Instance().Get (CAMERA_ISO_SPEED));
			m_camera->SetIsoSpeed (isoSpeed);
			}
	}

	SetDeviceText ();

	/*
	** Create path for observation log files
	*/
//	wxStandardPaths sp;
//	wxString logPath = sp.GetDataDir();
//	if (logPath.Last () != '/' && logPath.Last() != '\\')
//		logPath += "/";
//	logPath += "logs";
//	if (!wxDirExists(logPath)) {
//		wxMkdir (logPath);
//		}
//	ObservationLog::Instance().SetPath (logPath);
	ObservationLog::Instance().SetPath (Properties::Instance().Get (OBSERVATION_LOG_FOLDER));

	/*
	** Load catalogs: GCVS, NSV, NGC/IC, Program
	*/
	wxString dataPath = sp.GetLocalDataDir () + "/catalogs";
	wxLogDebug ("Data Path: " + dataPath);

//	CatalogFactory::Instance().Add (CATALOG_NAME_GCVS, dataPath);
//	CatalogFactory::Instance().Add (CATALOG_NAME_NSV, dataPath);
//	CatalogFactory::Instance().Get (CATALOG_NAME_NSV, "").AddPrefix (CATALOG_NAME_NSV);
//	CatalogFactory::Instance().Add (CATALOG_NAME_NGC, dataPath);
//	CatalogFactory::Instance().Get (CATALOG_NAME_NGC, "").AddPrefix (CATALOG_NAME_NGC);
//	CatalogFactory::Instance().Get (CATALOG_NAME_NGC, "").AddPrefix (CATALOG_NAME_IC);
//	CatalogFactory::Instance().Add (CATALOG_NAME_PROGRAM, dataPath);

	CatalogFactory::Instance().AddAll (wxT("*.cat"), dataPath);

	/*
	** Clear current target info
	*/
	m_DisplayName->SetLabel (_("(no selected object)"));
	m_DisplayRa->SetLabel ("");
	m_DisplayDecl->SetLabel ("");

	DoDisableControls (FALSE);

	m_LogWindow->Show (Properties::Instance().GetBool(DEBUG_LOG_WINDOW));

	Refresh ();
}


void VAutoDialog::OnOptions(wxCommandEvent &ev)
{
	DoInvokeOptionsDialog ();
}

void VAutoDialog::OnOpenPlan(wxCommandEvent& ev)
{
	/*
	** Select the plan file
	*/
	wxFileDialog *fd = new wxFileDialog (this, _("Open plan file"), Properties::Instance().Get (PLAN_FOLDER), wxEmptyString, wxT("*.plan"), wxFD_OPEN | wxFD_FILE_MUST_EXIST);
	int rv = fd->ShowModal();
	if (rv == wxID_CANCEL)
		return;

	wxFileName fn (fd->GetPath ());
	Properties::Instance().Put (PLAN_FOLDER, fn.GetPath());
	Properties::Instance().Save (CONFIG_FILE);

	/*
	** Open the plan file as a catalog
	*/
	m_Plan = CatalogFactory::Instance().Open (fn.GetFullName(), fn.GetPath());

	m_PlanList->DeleteAllItems ();

	/*
	** Fill the plan list with the targets in the plan file
	*/
	TargetList& targets = m_Plan.GetAll ();
	if (targets.GetCount () == 0)
		return;

	wxListItem item;
	item.m_mask = wxLIST_MASK_TEXT;

	for (unsigned int j = 0; j < targets.GetCount(); j++) {

		item.m_itemId = j;

		// first column: icon
		//
		item.m_col = 0;
		item.m_text = "";
		m_PlanList->InsertItem (item);

		if (!IsTargetAboveHorizon (targets[j]))
			m_PlanList->SetItemImage (j, PLAN_ICON_DISABLED);
		else
			m_PlanList->SetItemImage (j, PLAN_ICON_OK);

		// second column: name
		//
		item.m_col = 1;
		item.m_text = targets[j].GetName();
		m_PlanList->SetItem (item);
		if ((j % 2) == 1)
			m_PlanList->SetItemBackgroundColour (item, wxColor(BGCOLOR_LISTCTRL_ODD));

		// thirs column: coords
		//
		item.m_col = 2;
		wxString ra, decl;
		targets[j].GetCoord ().ToStringEq(ra, decl);
		item.m_text = ra + " " + decl;
		m_PlanList->SetItem (item);

		}

	m_HasPlan = true;
	DoDisableControls (false);

	// save original values
	m_saveCurrentTarget = m_CurrentTarget;
	m_saveCurrentTarget.SetExposureCount (m_FrameCount->GetValue());
	m_saveCurrentTarget.SetExposureTime (m_Duration->GetValue());

	// display plan file name
	wxString fname = fn.GetFullName ();
	m_PlanFile->SetLabel (fname);
}

void VAutoDialog::OnDeletePlan(wxCommandEvent& ev)
{
	DoDeletePlan ();
}

void VAutoDialog::DoDeletePlan(void)
{
	m_PlanList->DeleteAllItems ();
	m_HasPlan = false;

	// restore original values
	m_CurrentTarget = m_saveCurrentTarget;
	DisplayCurrentTarget ();

	m_PlanFile->SetLabel (_("(none)"));

	DoDisableControls (false);
}

void VAutoDialog::OnSelectPlanItem (wxListEvent& ev)
{
	unsigned int selected = ev.GetIndex();

	TargetList& targets = m_Plan.GetAll ();
	m_CurrentTarget = targets[selected];
	DisplayCurrentTarget ();

	m_SkipButton->Enable (selected < targets.GetCount() - 1);
}

void VAutoDialog::Log(wxString& status, int frameCount)
{
	wxString objectName = m_CurrentTarget.GetName ();
	if (objectName == "")
		objectName = "unknown object";
	if (frameCount == -1)
		frameCount = m_CurrentTarget.GetExposureCount();
	wxString logText = wxString::Format ("%s;%d;%d;%s\n", objectName, frameCount, m_CurrentTarget.GetExposureTime(), status);
	wxFile logf (m_logFile, wxFile::write_append);
	logf.Write (logText);
	logf.Close ();
}

void VAutoDialog::SetStatusText (const wxString& text)
{
	m_StatusBar->SetLabel (text);
}

void VAutoDialog::SetStatusCoords (const wxString& text)
{
	m_StatusCoords->SetLabel (text);
	m_StatusCoords->Refresh ();
}

void VAutoDialog::DoInvokeOptionsDialog(void)
{
	wxString mountType = Properties::Instance().Get(MOUNT_TYPE);
	wxString mountPort = Properties::Instance().Get(MOUNT_PORT);
	wxString cameraType = Properties::Instance().Get(CAMERA_TYPE);
	wxString cameraPort = Properties::Instance().Get(CAMERA_PORT);
	wxString observationLogPath = Properties::Instance().Get (OBSERVATION_LOG_FOLDER);
	wxString isoSpeed = Properties::Instance().Get (CAMERA_ISO_SPEED);

	OptionsDialog *dlg = new OptionsDialog (this);
	int rv = dlg->ShowModal ();
	if (rv == wxID_OK) {
		Properties::Instance().Save(CONFIG_FILE);

		if (Properties::Instance().Get(MOUNT_TYPE) != mountType ||
			Properties::Instance().Get(MOUNT_PORT) != mountPort) {
			TelescopeFactory::Instance().ClearTelescope();
			m_telescope = TelescopeFactory::Instance().GetTelescope (
				Properties::Instance().Get(MOUNT_TYPE),
				Properties::Instance().Get(MOUNT_PORT));
			}

		bool newCamera = false;
		if (Properties::Instance().Get(CAMERA_TYPE) != cameraType ||
			Properties::Instance().Get(CAMERA_PORT) != cameraPort) {
//			CameraFactory::Instance().ClearCamera();
			wxDELETE (m_camera);
			m_camera = CameraFactory::Instance().GetCamera (
				Properties::Instance().Get(CAMERA_TYPE),
				Properties::Instance().Get(CAMERA_PORT));
			newCamera = true;
			}

		if (Properties::Instance().Get (OBSERVATION_LOG_FOLDER) != observationLogPath) {
			ObservationLog::Instance().SetPath (Properties::Instance().Get (OBSERVATION_LOG_FOLDER));
			}

		if (m_camera->HasFeature (Camera::DCP_CAN_SET_ISO_SPEED) && (newCamera || Properties::Instance().Get(CAMERA_ISO_SPEED) != isoSpeed)) {
			unsigned long iso = wxAtol (Properties::Instance().Get(CAMERA_ISO_SPEED));
			if (iso > 0)
				m_camera->SetIsoSpeed (iso);
			}

		SetDeviceText ();

		DoDisableControls (FALSE);

		m_LogWindow->Show (Properties::Instance().GetBool(DEBUG_LOG_WINDOW));
		}
}

void VAutoDialog::OnAuto(wxCommandEvent& ev)
{
	// Go into auto state
	m_Auto = true;
	DoDisableControls (false);

	// Get next object from list
	long selected = m_PlanList->GetNextItem (0, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
	if (selected == -1)	{
		if (!NextPlanTarget ()) {
			m_Auto = false;
			SetState (STATE_READY);
			DoDisableControls (false);
			return;
			}
		selected = m_PlanList->GetNextItem (0, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
		}

	TargetList& targets = m_Plan.GetAll ();
	m_CurrentTarget = targets[selected];

	// Start to move
	if (!DoMove ()) {
		m_Auto = false;
		SetState (STATE_READY);
		DoDisableControls (false);
		return;
		}

	// The rest is on the notification methods...

}

/*
** Find coords for the object
*/
void VAutoDialog::OnFindObject(wxCommandEvent& ev)
{
	wxString objectName = m_ObjectName->GetValue ();
	if (objectName.IsEmpty()) {
		if (m_CurrentTarget.GetName().IsEmpty())
			return;
		m_CurrentTarget.SetName ("");
		m_CurrentTarget.GetCoord().FromBin16(0,0);
//		m_DisplayRa->SetLabel ("");
//		m_DisplayDecl->SetLabel ("");
		DisplayCurrentTarget ();
		m_ObjectName->SetFocus();
		}

	else {
		Target& target = CatalogFactory::Instance().FindObject (objectName);
		if (!target.IsValid()) {
			wxMessageBox (_("Object not found in program or available catalogs."), _("Find Object"), wxICON_EXCLAMATION | wxOK, this);
			return;
			}
		m_CurrentTarget = target;
		DisplayCurrentTarget ();
	
		// set the focus to the Move button
		m_MoveButton->SetFocus ();
		}

	Refresh ();
}

void VAutoDialog::DisplayCurrentTarget ()
{
	//	show coordinates
	wxString sRa, sDecl;
	m_CurrentTarget.GetCoord().ToStringEq (sRa, sDecl);

	m_DisplayName->SetLabel (m_CurrentTarget.GetName().IsEmpty() ? _("(no selected object)") : m_CurrentTarget.GetName());
	m_DisplayRa->SetLabel (sRa);
	m_DisplayDecl->SetLabel (sDecl);

	if (m_CurrentTarget.GetExposureCount() > 0) {
		m_FrameCount->SetValue (m_CurrentTarget.GetExposureCount());
		m_Duration->SetValue (m_CurrentTarget.GetExposureTime());
		}
}


void VAutoDialog::Abort()
{
	wxLogDebug (wxT("VAutoDialog::Abort starts aborting."));
	if (m_State == STATE_MOVING) {
		wxLogDebug (wxT("VAutoDialog::Abort detects telescope moving state."));
		if (m_telescopeNotifier->IsAlive()) {
			wxLogDebug (wxT("VAutoDialog::Abort abort notifier thread."));
			m_telescopeNotifier->Abort ();
			}
		else {
			wxLogDebug (wxT("VAutoDialog::Abort notifier thread is not alive now."));
			}
		}
	else if (m_State == STATE_SHOOTING) {
		wxLogDebug (wxT("VAutoDialog::Abort detects camera shooting state."));
		if (m_cameraNotifier->IsAlive ()) {
			wxLogDebug (wxT("VAutoDialog::Abort abort notifier thread."));
			m_camera->AbortShooting ();
			m_cameraNotifier->Abort ();
			}
		else {
			wxLogDebug (wxT("VAutoDialog::Abort notifier thread is not alive now."));
			}
		}
	SetState (STATE_READY);
	DoDisableControls (FALSE);
}


void VAutoDialog::OnAbort(wxCommandEvent& ev)
{
	Abort(); 
}

void VAutoDialog::OnExit(wxCommandEvent& ev)
{
	Abort ();
	m_LogWindow->Flush ();
	wxDELETE (m_camera);
	EndModal (wxID_CLOSE);
}

void VAutoDialog::OnAbout(wxCommandEvent& ev)
{
	AboutDialog *about = new AboutDialog (this);
	about->ShowModal ();
}

/*
**	disable controls
**	ha nincs távcsõ:				Move, Auto
**	ha nincs kamera:				Shoot, Auto, NumFrames, Duration
**	ha nincs plan kiválasztva:		Auto, Skip
**	amikor mozog vagy fényképez:	csak az Abort és az Exit enabled
*/
void VAutoDialog::DoDisableControls (bool inProgress)
{
	inProgress = m_Auto || m_State == STATE_MOVING || m_State == STATE_SHOOTING;
	boolean hasTelescope = m_telescope->GetMountType() != "None";
	boolean hasCamera = m_camera->GetCameraType() != "None";

	m_MoveButton->Enable (hasTelescope && !inProgress);

	m_ShootButton->Enable (hasCamera && !inProgress);
	m_Duration->Enable (hasCamera && !inProgress && !m_HasPlan && !m_Auto);
	m_FrameCount->Enable (hasCamera && !inProgress && !m_HasPlan && !m_Auto);

	m_AbortButton->Enable ((hasTelescope || hasCamera) && inProgress);

	m_buttonOpenPlan->Enable (!inProgress);
	m_buttonDeletePlan->Enable (m_HasPlan && !inProgress);
	m_AutoButton->Enable (m_HasPlan && hasTelescope && hasCamera && !inProgress);
	m_SkipButton->Enable (m_HasPlan && !inProgress);

	m_ObjectName->Enable (!m_HasPlan);
	m_FindObject->Enable (!m_HasPlan);
}

/*
** Move mount to the selected target
**		get and validate coords
**		check if coords is below hotizon
**		disable move, shoot, auto, openplan, closeplan, skip buttons
**		set status to moving
**		display moving on status bar
**		start move
**		enable abort button
**		(move finish event or abort button will terminate this state)
*/

bool VAutoDialog::DoMove ()
{
	wxString mbTitle = _("Moving telescope");

	// get coords
	Coordinate& coord = m_CurrentTarget.GetCoord ();

	// check whether telescope is connected
	if (!m_telescope->IsConnected()) {
		wxMessageBox (_("The telescope is not connected. Check cables or telescope power."), mbTitle, wxICON_EXCLAMATION | wxOK, this);
		return false;
		}

	// check whether telescope is aligned
	if (!m_telescope->IsAligned()) {
		wxMessageBox (_("The telescope is not aligned correctly. Align it first."), mbTitle, wxICON_EXCLAMATION | wxOK, this);
		return false;
		}

	// check whether telescope is not moving
	if (m_telescope->IsMoving ()) {
		wxMessageBox (_("The telescope is currently moving. Wait until it stops."), mbTitle, wxICON_EXCLAMATION | wxOK, this);
		return false;
		}

	// set status to moving and display state on status bar
	SetState (STATE_MOVING);
	DoDisableControls (true);

	// command mount to start moving
	wxLogDebug (wxT("VAutoDialog::OnMove send move command to telescope."));
	m_telescope->MoveToEquatorialCoords (coord);

	// enable abort button
	m_AbortButton->Enable (true);

	// set the focus on the abort button
	m_AbortButton->SetFocus ();

	// start telescope notifier thread
	wxLogDebug (wxT("VAutoDialog::OnMove starts telescope notifier."));
	m_telescopeNotifier = new TelescopeNotifier (this, TELESCOPE_NOTIFIER_ID, m_telescope);
	wxThreadError err = m_telescopeNotifier->Create ();
	wxLogDebug (wxString::Format("VAutoDialog::OnMove creates notification thread. result: %d", err));
	if (err != wxTHREAD_NO_ERROR) {
		delete m_telescopeNotifier;
		Abort();
		return false;
		}

	err = m_telescopeNotifier->Run ();
	wxLogDebug (wxString::Format("VAutoDialog::OnMove starts notification thread. result: %d", err));
	if (err != wxTHREAD_NO_ERROR) {
		delete m_telescopeNotifier;
		Abort();
		return false;
		}

	return true;
}

void VAutoDialog::OnMove(wxCommandEvent& ev)
{
	wxString mbTitle = _("Moving telescope");
	wxLogDebug (wxT("VAutoDialog::OnMove starts telescope moving."));

	// Check whether target is valid
	if (!m_CurrentTarget.IsValid()) {
		wxMessageBox (_("No valid target is selected to move."), mbTitle, wxICON_EXCLAMATION | wxOK, this);
		return;
		}

	// Check whether the target is above the horizon
//	double lon = wxAtof (Properties::Instance().Get(STATION_LONGITUDE));
//	double lat = wxAtof (Properties::Instance().Get(STATION_LATITUDE));
//	Station station (lon, lat);
//	if (!m_CurrentTarget.GetCoord().AboveHorizon(station, wxDateTime::Now().GetJulianDayNumber()))

	if (!IsTargetAboveHorizon (m_CurrentTarget))
	{
		wxMessageBox (_("The object currently is under the horizon"), GetTitle(), wxICON_EXCLAMATION | wxOK, this);
		return;
	}

	DoMove ();
}

void VAutoDialog::OnNotifyTelescopeStop (wxCommandEvent& ev)
{
	bool accidentalStop = false;
	wxLogDebug (wxT("VAutoDialog::OnNotifyTelescopeStop telescope notification arrived."));
	TelescopeNotifier::TelescopeStopAction stopAction = (TelescopeNotifier::TelescopeStopAction) ev.GetInt ();
	switch (stopAction) {
		case TelescopeNotifier::STOP_NORMALLY:
			break;
		case TelescopeNotifier::STOP_DISCONNECT:
			accidentalStop = true;
			wxMessageBox (_("The telescope is disconnected. Check cables and power."), _("Moving telescope"), wxICON_EXCLAMATION | wxOK, this);
			break;
		case TelescopeNotifier::STOP_TIMEOUT:
			accidentalStop = true;
			wxMessageBox (_("The telescope is not stopped before timeout expired."), _("Moving telescope"), wxICON_EXCLAMATION | wxOK, this);
			break;
		case TelescopeNotifier::STOP_ABORT:
			accidentalStop = true;
			wxMessageBox (_("The telescope stopped by the user."), _("Moving telescope"), wxICON_EXCLAMATION | wxOK, this);
			break;
		case TelescopeNotifier::DISPLAY_COORD:
			SetStatusCoords (ev.GetString ());
			break;
		}

	if (!m_Auto) {

		SetState (STATE_READY);
		DoDisableControls (false);
		m_ShootButton->SetFocus ();
	
		}

	// in auto mode, start to make exposition
	else {

		if (accidentalStop) {

			m_Auto = false;
			SetState (STATE_READY);
			DoDisableControls (false);
			m_ShootButton->SetFocus ();

			}

		else if (!DoShoot ()) {

			m_Auto = false;
			SetState (STATE_READY);
			DoDisableControls (false);
			m_ShootButton->SetFocus ();

			}

		}
}

#define MIN_EXPOSURE_TIME	2
#define MAX_EXPOSURE_TIME	3600
#define MIN_FRAME_COUNT		1
#define MAX_FRAME_COUNT		999

bool VAutoDialog::DoShoot ()
{
	int exposureTime = m_Duration->GetValue ();
	if (exposureTime < MIN_EXPOSURE_TIME || exposureTime > MAX_EXPOSURE_TIME) {
		wxMessageBox (wxString::Format(_("Time of the exposition must be between %d and %d"), MIN_EXPOSURE_TIME, MAX_EXPOSURE_TIME), GetTitle (), wxICON_EXCLAMATION | wxOK, this);
		return false;
		}
	int frameCount = m_FrameCount->GetValue ();
	if (frameCount < MIN_FRAME_COUNT || frameCount > MAX_FRAME_COUNT) {
		wxMessageBox (wxString::Format(_("Number of frames must be between %d and %d"), MIN_FRAME_COUNT, MAX_FRAME_COUNT), GetTitle (), wxICON_EXCLAMATION | wxOK, this);
		return false;
		}
	int delayBeforeShoot = Properties::Instance().GetInt (CAMERA_FRAME_DELAY);
	if (delayBeforeShoot < 0 || delayBeforeShoot > 999)
		delayBeforeShoot = 0;

	SetState (STATE_SHOOTING);
	DoDisableControls (true);

	ObservationLog::Instance().SetTargetName (m_CurrentTarget.GetName().IsEmpty() ? m_ObjectName->GetValue() : m_CurrentTarget.GetName());
	ObservationLog::Instance().SetCoord (m_CurrentTarget.GetCoord());

	int stackId = Properties::Instance().GetInt (NEXT_FRAMESTACK_ID);
	Properties::Instance().Put (NEXT_FRAMESTACK_ID, stackId + 1);
	Properties::Instance().Save (CONFIG_FILE);
	ObservationLog::Instance().SetFrameStackId (stackId);

	m_camera->ShootMany (frameCount, 1000 * exposureTime, 1000 * delayBeforeShoot);

	// enable abort button
	m_AbortButton->Enable (true);

	// set the focus on the abort button
	m_AbortButton->SetFocus ();

	// start camera notifier thread
	wxLogDebug (wxT("VAutoDialog::OnShoot starts camera notifier."));
	m_cameraNotifier = new CameraNotifier (this, CAMERA_NOTIFIER_ID, m_camera, exposureTime);
	wxThreadError err = m_cameraNotifier->Create ();
	wxLogDebug (wxString::Format("VAutoDialog::OnShoot creates notification thread. result: %d", err));
	if (err != wxTHREAD_NO_ERROR) {
		delete m_cameraNotifier;
		Abort();
		return false;
		}

	err = m_cameraNotifier->Run ();
	wxLogDebug (wxString::Format("VAutoDialog::OnShoot starts notification thread. result: %d", err));
	if (err != wxTHREAD_NO_ERROR) {
		delete m_cameraNotifier;
		Abort();
		return false;
		}

	return true;
}

void VAutoDialog::OnShoot(wxCommandEvent& ev)
{
	DoShoot ();
}


void VAutoDialog::OnNotifyCameraShoot (wxCommandEvent& ev)
{
	bool accidentalStop = false;
	CameraNotifier::CameraShootAction shootAction = (CameraNotifier::CameraShootAction) ev.GetInt ();
//	wxLogDebug (wxString::Format("Camera notification arrived: %d", shootAction));

	switch (shootAction) {
		case CameraNotifier::SHOOT_NOTIFY_TIME: 
			{
				SetStatusText (ev.GetString ());
				return;
			}
			break;
		case CameraNotifier::SHOOT_STOP_NORMALLY:
//			ObservationLog::Instance().SetStatus (wxT("OK"));
//			ObservationLog::Instance().Flush ();
			break;
		case CameraNotifier::SHOOT_TIMEOUT:
			m_camera->AbortShooting ();
//			ObservationLog::Instance().SetStatus (LOG_STATUS_TIMEOUT);
//			ObservationLog::Instance().Flush ();
			wxMessageBox (_("The camera is not stop shooting before timeout expired."), _("Shooting"), wxICON_EXCLAMATION | wxOK, this);
			accidentalStop = true;
			break;
		case CameraNotifier::SHOOT_ABORT:
//			ObservationLog::Instance().SetStatus (wxT("Failed:abort"));
//			ObservationLog::Instance().Flush ();
			wxMessageBox (_("The exposition stopped by the user."), _("Shooting"), wxICON_EXCLAMATION | wxOK, this);
			accidentalStop = true;
			break;
		}

	if (!m_Auto) {

		if (m_HasPlan)
			NextPlanTarget ();
		SetState (STATE_READY);
		DoDisableControls (false);
		m_MoveButton->SetFocus ();

		}

	else {

		if (!NextPlanTarget ()) {

			// TODO: A plan file-t töröljük az utolsó target után?

			m_Auto = false;
			SetState (STATE_READY);
			DoDisableControls (false);
			m_MoveButton->SetFocus ();

			}

		else {

			if (!DoMove ()) {

				m_Auto = false;
				SetState (STATE_READY);
				DoDisableControls (false);
				m_MoveButton->SetFocus ();

				}

			}

		}
}

bool VAutoDialog::NextPlanTarget()
{
	TargetList& targets = m_Plan.GetAll();

	int selectedIndex = m_PlanList->GetNextItem (-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED) + 1;
	int itemCount = m_PlanList->GetItemCount ();

	for (; selectedIndex < itemCount && !IsTargetAboveHorizon(targets[selectedIndex]); selectedIndex++);

	if (selectedIndex == itemCount)
		return false;

	m_PlanList->SetItemState (selectedIndex, wxLIST_STATE_SELECTED | wxLIST_STATE_FOCUSED, wxLIST_STATE_SELECTED | wxLIST_STATE_FOCUSED);
	m_PlanList->EnsureVisible (selectedIndex);

	m_CurrentTarget = targets[selectedIndex];
	DisplayCurrentTarget ();
	m_MoveButton->SetFocus ();
	return true;
}

void VAutoDialog::OnSkip(wxCommandEvent& ev)
{
	NextPlanTarget ();
}

void VAutoDialog::SetState (State state)
{
	if (m_State == state)
		return;

	m_State = state;
	wxString stateText;
	switch (m_State) {
		case STATE_MOVING:
			stateText = _("Moving...");
			break;
		case STATE_SHOOTING:
			stateText = _("Shooting...");
			break;
		default:
			stateText = m_Auto ? _("Auto") : _("Ready.");
			break;
		}
	SetStatusText (stateText);
	wxLogStatus (stateText);
}

VAutoDialog::State VAutoDialog::GetState ()
{
	return m_State;
}

void VAutoDialog::SetDeviceText ()
{
	wxString text = "";
	if (m_camera != NULL) {
		text += _("Camera:") + m_camera->GetCameraType();
		}
	if (m_telescope != NULL) {
		if (!text.IsEmpty())
			text += ", ";
		text += _("Mount: ") + m_telescope->GetMountType();
		}
	m_DeviceText->SetLabel (text);
}

bool VAutoDialog::IsTargetAboveHorizon (Target& target)
{
	// Check whether the target is above the horizon
	double lon = wxAtof (Properties::Instance().Get(STATION_LONGITUDE));
	double lat = wxAtof (Properties::Instance().Get(STATION_LATITUDE));
	Station station (lon, lat);
	return target.GetCoord().AboveHorizon(station, wxDateTime::Now().GetJulianDayNumber());
}
