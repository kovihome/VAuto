#pragma once

#include "Catalog.h"
#include <wx/wx.h>
#include <wx/dynarray.h>

WX_DECLARE_OBJARRAY (Catalog, CatalogPool);

class CatalogFactory
{
private:
	CatalogFactory();
	CatalogFactory(CatalogFactory&);
	CatalogFactory& operator= (CatalogFactory&);
	virtual ~CatalogFactory();

	static CatalogFactory* instance;
	CatalogPool m_pool;
public:
	static CatalogFactory& Instance ();
	Catalog& Add (const wxString& catalogName, const wxString& path);
	Catalog& Get (const wxString& catalogName, const wxString& path);
	Target& FindObject (const wxString& objectName);
	int AddAll(const wxString& extension, const wxString& path);
	Catalog& Open(const wxString& catalogName, const wxString& path);
};
