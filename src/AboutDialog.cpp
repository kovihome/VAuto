#include "AboutDialog.h"
#include <wx/xrc/xmlres.h>

BEGIN_EVENT_TABLE (AboutDialog, wxDialog)
	EVT_BUTTON( XRCID("wxID_CLOSE"), AboutDialog::OnCancel)
END_EVENT_TABLE()

AboutDialog::AboutDialog(wxWindow *parent)
{
	wxXmlResource::Get()->LoadDialog(this, parent, "About");
	wxStaticText *m_about = XRCCTRL(*this, "ID_ABOUT", wxStaticText);

//	global appName,version,copyrigths
	wxString appName = "VAuto";
	wxString version = "Version 1.2 build 1002";
	wxString copyrights = "Copyrights (c) Kvi, 2009-2011.";

//	SetDeviceText (self)
	m_about->SetLabel (wxString::Format ("%s, %s\n%s", appName, version, copyrights));
}

void AboutDialog::OnCancel(wxCommandEvent& ev)
{
	EndModal (wxID_CANCEL);
}

