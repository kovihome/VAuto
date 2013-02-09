#ifdef __WXMSW__
    #include <wx/msw/msvcrt.h>      // redefines the new() operator 
#endif 

#include "AboutDialog.h"
#include "CameraFactory.h"
#include <wx/xrc/xmlres.h>

BEGIN_EVENT_TABLE (AboutDialog, wxDialog)
	EVT_BUTTON( XRCID("wxID_CLOSE"), AboutDialog::OnCancel)
END_EVENT_TABLE()

AboutDialog::AboutDialog(wxWindow *parent)
{
	wxXmlResource::Get()->LoadDialog(this, parent, "About");
	wxStaticText *m_about = XRCCTRL(*this, "ID_ABOUT", wxStaticText);

	wxString appName = "VAuto";
	wxString version = "Version 1.2 build 1005";
	wxString copyrights = "Copyrights (c) Kvi, 2009-2012.";

//	CameraArray& ca = CameraFactory::Instance().EnumerateCameras();
//	wxString t = "";
//	for (unsigned int j = 0; j < ca.Count(); j ++) {
//		t += ca.Item (j).GetName() + "\n";
//		}

//	m_about->SetLabel (wxString::Format ("%s, %s\n%s\n\nonline camera: %s", appName, version, copyrights, t));
	m_about->SetLabel (wxString::Format ("%s, %s\n%s", appName, version, copyrights));
}

void AboutDialog::OnCancel(wxCommandEvent& ev)
{
	EndModal (wxID_CANCEL);
}

