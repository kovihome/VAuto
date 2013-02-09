#ifdef __WXMSW__
    #include <wx/msw/msvcrt.h>      // redefines the new() operator 
#endif 

#include "OptionsDialog.h"
#include "SerialPortEnumerator.h"
#include "TelescopeFactory.h"
#include "CameraFactory.h"
#include "../../VTools/src/Properties.h"
#include <wx/xrc/xmlres.h>
#include <wx/spinctrl.h>
#include <wx/dirdlg.h>


BEGIN_EVENT_TABLE(OptionsDialog,wxDialog)
	EVT_BUTTON		(XRCID("ID_OK"					),	OptionsDialog::OnOK				)
	EVT_BUTTON		(XRCID("ID_CANCEL"				),	OptionsDialog::OnCancel			)
	EVT_BUTTON		(XRCID("ID_LOG_FOLDER_SEARCH"	),	OptionsDialog::OnChangeLogFolder)
	EVT_CHECKBOX	(XRCID("ID_LOCKUP"				),	OptionsDialog::OnLockup			)
	EVT_CHOICE		(XRCID("ID_MOUNT_TYPE"			),	OptionsDialog::OnChangeTelescope)
	EVT_CHOICE		(XRCID("ID_CAMERA_TYPE"			),	OptionsDialog::OnChangeCamera	)
END_EVENT_TABLE()


OptionsDialog::OptionsDialog(wxWindow *parent)
: m_observationLogFolder(NULL)
{
//	wxXmlResource::Get()->LoadDialog(this, parent, "Options");
	wxXmlResource::Get()->LoadDialog(this, parent, "OptionsNew");

	m_DeviceText = XRCCTRL(*this, "DEVICE_TEXT", wxStaticText);

	wxButton* m_buttonOk = XRCCTRL(*this, "ID_OK", wxButton);
	wxButton* m_buttonCancel = XRCCTRL(*this, "ID_CANCEL", wxButton);

	m_debugLogWindow = XRCCTRL(*this, "ID_DEBUG_LOG_WINDOW", wxCheckBox);
	m_observationLogFolder = XRCCTRL(*this, "ID_LOG_FOLDER", wxTextCtrl);

	m_cameraLockup = XRCCTRL(*this, "ID_LOCKUP", wxCheckBox);
	m_cameraPort = XRCCTRL(*this, "ID_COMPORT", wxChoice);
	m_cameraType = XRCCTRL(*this, "ID_CAMERA_TYPE", wxChoice);
	m_lockupDelay = XRCCTRL(*this, "ID_LOCKUPDELAY", wxSpinCtrl);
	m_frameDelay = XRCCTRL(*this, "ID_FRAMEDELAY", wxSpinCtrl);
	m_titleLockupDelay = XRCCTRL(*this, "IDT_LOCKUPDELAY", wxStaticText);
	m_isoSpeed = XRCCTRL(*this, "ID_ISOSPEED", wxChoice);
	m_batteryMinLevel = XRCCTRL(*this, "ID_BATTERY_MIN_LEVEL", wxSlider);

	m_telescopePort = XRCCTRL(*this, "ID_COMPORT_EQ6", wxChoice);
	m_telescopeType = XRCCTRL(*this, "ID_MOUNT_TYPE", wxChoice);

	m_stationLongitude = XRCCTRL(*this, "ID_LONGITUDE", wxTextCtrl);
	m_stationLatitude = XRCCTRL(*this, "ID_LATITUDE", wxTextCtrl);

	m_ComPorts = SerialPortEnumerator::Enumerate ();
	bool lockup = Properties::Instance().GetBool (CAMERA_LOCKUP);

	m_Telescopes = TelescopeFactory::Instance().EnumerateDrivers();
	wxString defTelescope = Properties::Instance().Get(MOUNT_TYPE);
	wxArrayString telescopeNames;
	int defTelescopeIndex = 0;
	for (unsigned int j = 0; j < m_Telescopes.GetCount(); j++) {
		telescopeNames.Add (TelescopeFactory::Instance().GetDriverDisplayName(m_Telescopes[j]));
		if (defTelescope == m_Telescopes[j])
			defTelescopeIndex = j;
		}

	m_Cameras = CameraFactory::Instance().EnumerateDrivers();
	wxString defCamera = Properties::Instance().Get(CAMERA_TYPE);
	wxArrayString cameraNames;
	int defCameraIndex = 0;
	for (unsigned int j = 0; j < m_Cameras.GetCount(); j++) {
		cameraNames.Add (CameraFactory::Instance().GetDriverDisplayName(m_Cameras[j]));
		if (defCamera == m_Cameras[j])
			defCameraIndex = j;
		}

	FillCombo (m_cameraType, cameraNames, defCameraIndex);
	DetermineCameraCapabilities ();
	FillPortCombo (m_cameraPort, m_ComPorts, Properties::Instance().Get(CAMERA_PORT), true);
	m_cameraLockup->SetValue (lockup);
	m_lockupDelay->SetValue (Properties::Instance().GetInt(CAMERA_LOCKUP_DELAY));
	m_titleLockupDelay->Enable (lockup);
	m_lockupDelay->Enable (lockup);
	m_frameDelay->SetValue (Properties::Instance().GetInt (CAMERA_FRAME_DELAY));
	FillCombo (m_telescopeType, telescopeNames, defTelescopeIndex);
	FillPortCombo (m_telescopePort, m_ComPorts, Properties::Instance().Get(MOUNT_PORT), false);

	m_isoSpeed->SetStringSelection (Properties::Instance().Get(CAMERA_ISO_SPEED));
	m_isoSpeed->Enable (m_CameraCanSetIsoSpeed);

	m_stationLongitude->SetValue (Properties::Instance().Get(STATION_LONGITUDE));
	m_stationLatitude->SetValue (Properties::Instance().Get(STATION_LATITUDE));

	m_debugLogWindow->SetValue (Properties::Instance().GetBool(DEBUG_LOG_WINDOW));
	m_observationLogFolder->SetValue (Properties::Instance().Get (OBSERVATION_LOG_FOLDER));

	m_batteryMinLevel->SetValue (Properties::Instance().GetInt(CAMERA_BATTERY_MIN_LEVEL));

	SetDeviceText ();
}

void OptionsDialog::FillCombo(wxChoice *combo, wxArrayString& items, int selectedIndex)
{
	combo->Clear();
	for (unsigned int j = 0; j < items.GetCount (); j++) {
		combo->Append (items.Item (j));
		}
	combo->SetSelection (selectedIndex);
}

void OptionsDialog::DetermineCameraCapabilities ()
{
	Camera* camera = CameraFactory::Instance().GetCamera (m_Cameras[m_cameraType->GetSelection()], "", false);
	m_CameraUseSerial = camera->HasFeature (Camera::DCP_USE_SERIAL);
	m_CameraUseUsb = camera->HasFeature (Camera::DCP_USE_USB);
	m_CameraHasMLU = camera->HasFeature (Camera::DCP_HAS_MLU);
	m_CameraCanSetIsoSpeed = camera->HasFeature (Camera::DCP_CAN_SET_ISO_SPEED);
}

void OptionsDialog::FillPortCombo(wxChoice *combo, wxArrayString& ports, wxString& defPort, bool isCamera)
{
	combo->Clear();
	if ((isCamera ? m_CameraUseSerial : true)) {

		bool defExists = false;
		for (unsigned int j = 0; j < ports.GetCount (); j++) {
			combo->Append (ports.Item (j));
			if (ports.Item (j) == defPort)
				defExists = true;
			}
		if (defExists)
			combo->SetStringSelection (defPort);
		else
			combo->SetSelection (0);
		}

	else if (m_CameraUseUsb) {
		combo->Append ("USB");
		combo->SetSelection (0);
		}
}

void OptionsDialog::OnChangeTelescope (wxCommandEvent& ev)
{
	SetDeviceText ();
	m_telescopePort->Enable (ev.GetSelection() != 0);
}

void OptionsDialog::OnChangeCamera (wxCommandEvent& ev)
{
	SetDeviceText ();
	DetermineCameraCapabilities ();
	FillPortCombo (m_cameraPort, m_ComPorts, Properties::Instance().Get(CAMERA_PORT), true);
	m_cameraPort->Enable (ev.GetSelection() != 0);
	m_cameraLockup->SetValue (m_CameraHasMLU);
	m_cameraLockup->Enable (m_CameraHasMLU);
	m_lockupDelay->Enable (m_CameraHasMLU);
	m_frameDelay->Enable (ev.GetSelection() != 0);
	m_isoSpeed->Enable (m_CameraCanSetIsoSpeed);
	m_batteryMinLevel->Enable (m_CameraUseUsb);
}

void OptionsDialog::OnOK(wxCommandEvent& ev)
{
	Properties::Instance().Put (CAMERA_TYPE, m_Cameras[m_cameraType->GetSelection()]);
	Properties::Instance().Put (CAMERA_PORT, m_cameraPort->GetStringSelection());
	Properties::Instance().Put (CAMERA_LOCKUP, m_cameraLockup->IsChecked());
	Properties::Instance().Put (CAMERA_LOCKUP_DELAY, m_lockupDelay->GetValue());
	Properties::Instance().Put (CAMERA_FRAME_DELAY, m_frameDelay->GetValue());
	Properties::Instance().Put (CAMERA_ISO_SPEED, m_isoSpeed->GetStringSelection());
	Properties::Instance().Put (CAMERA_BATTERY_MIN_LEVEL, m_batteryMinLevel->GetValue());
	Properties::Instance().Put (MOUNT_TYPE, m_Telescopes[m_telescopeType->GetSelection()]);
	Properties::Instance().Put (MOUNT_PORT, m_telescopePort->GetStringSelection());
	Properties::Instance().Put (STATION_LONGITUDE, m_stationLongitude->GetValue());
	Properties::Instance().Put (STATION_LATITUDE, m_stationLatitude->GetValue());
	Properties::Instance().Put (DEBUG_LOG_WINDOW, m_debugLogWindow->IsChecked());
	Properties::Instance().Put (OBSERVATION_LOG_FOLDER, m_observationLogFolder->GetValue());
	EndModal (wxID_OK); 
}


void OptionsDialog::OnLockup(wxCommandEvent& ev)
{
	bool lock = m_cameraLockup->IsChecked();
	m_titleLockupDelay->Enable (lock);
	m_lockupDelay->Enable (lock);
}

void OptionsDialog::OnCancel(wxCommandEvent& ev)
{
	EndModal (wxID_CANCEL); 
}

void OptionsDialog::SetDeviceText ()
{
	wxString text = "";
	text += _("Camera:") + m_Cameras[m_cameraType->GetSelection()];
	text += ", ";
	text += _("Mount: ") + m_Telescopes[m_telescopeType->GetSelection()];
	m_DeviceText->SetLabel (text);
}

void OptionsDialog::OnChangeLogFolder(wxCommandEvent& ev)
{
	wxDirDialog dd (this, _("Select a folder for observation log."), m_observationLogFolder->GetValue());
	if (dd.ShowModal () == wxID_OK) {
		m_observationLogFolder->SetValue (dd.GetPath());
		}
}
