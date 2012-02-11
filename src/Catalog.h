#pragma once
#include "Target.h"
#include <wx/wx.h>
#include <wx/dynarray.h>

WX_DECLARE_OBJARRAY (Target, TargetList);

class Catalog :
	public wxObject
{
private:
	wxString m_name;
	wxString m_path;
	wxArrayString m_prefixList;
	TargetList m_cache;
public:
	Catalog ();
	Catalog (const wxString& name, const wxString& path);
	virtual ~Catalog();
	wxString& GetName () { return m_name; }
	void SetName (const wxString& name) { m_name = name; }
	void AddPrefix (const wxString& prefix) { m_prefixList.Add (prefix.Lower()); }
	bool HasPrefix (const wxString& prefix) { return (m_prefixList.Index(prefix.c_str()) != wxNOT_FOUND); }
	bool MatchPrefix (const wxString& objectName);

	Target& FindTarget (const wxString& targetName);
	TargetList& GetAll ();
private:
	void LoadCache ();

};
