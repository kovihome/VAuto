#include <wx/wx.h>
#include <wx/xrc/xmlres.h>
#include <wx/intl.h>
#include "VAutoDialog.h"

class VAutoApp : public wxApp
{
public:
	virtual bool OnInit ();
};


IMPLEMENT_APP (VAutoApp)

bool VAutoApp::OnInit()
{
    wxImage::AddHandler(new wxGIFHandler);
	wxImage::AddHandler(new wxPNGHandler);
    wxXmlResource::Get()->InitAllHandlers();
	wxString resFile = _T("res/VAuto.xrc");
    wxXmlResource::Get()->Load(resFile);


	wxLocale* locale = new wxLocale ();
	locale->Init(wxLocale::GetSystemLanguage());
	locale->AddCatalogLookupPathPrefix (wxT("locale"));
	locale->AddCatalog (wxT("VAuto"));

	wxString localeName = locale->GetName ();

	VAutoDialog *dlg = new VAutoDialog (); 
	dlg->ShowModal ();

	return false;
}