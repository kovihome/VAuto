#pragma once
#include "Target.h"
#include <wx/wx.h>
#include <wx/dynarray.h>

/*
	Catalog/Plan file header lines:

			#<property_name>;<csv_field_number>;<type>

	Catalog properties:

			#CATALOG_NAME
			#CATALOG_PREFIX

	Catalog header field properties and type (if needed):

			
			#OBJECT_NAME
			#OBJECT_REFERENCE			// N - non-existent object, R - reference to another object
			#REFERENCED_OBJECT_NAME		// if #OBJECT_REFERENCE is R
			#HARVARD_NUMBER
			#AUID
			#RA					DEG | SEXA
			#DECL				DEG | SEXA
			#TYPE
			#MIN_MG
			#MAX_MG
			#MG_COLOR
			#PERIOD

			#EXPOSURE_COUNT
			#EXPOSURE_TIME
			

*/

WX_DECLARE_OBJARRAY (Target, TargetList);

class Catalog :
	public wxObject
{
private:
	wxString m_name;
	wxString m_filename;
	wxString m_path;
	wxArrayString m_prefixList;
	TargetList m_cache;
public:
	Catalog ();
	Catalog (const wxString& filename, const wxString& path);
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

public:
	void ReadHeader();
};
