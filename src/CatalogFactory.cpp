#ifdef __WXMSW__
    #include <wx/msw/msvcrt.h>      // redefines the new() operator 
#endif 

#include "CatalogFactory.h"
#include <wx/arrimpl.cpp>
#include <wx/dir.h>

WX_DEFINE_OBJARRAY (CatalogPool);

CatalogFactory* CatalogFactory::instance = NULL;

//CatalogPool CatalogFactory::m_pool;

CatalogFactory& CatalogFactory::Instance ()
{
	if (instance == NULL) {
		instance = new CatalogFactory ();
		}
	return *instance;
}

CatalogFactory::CatalogFactory()
{
}

CatalogFactory::~CatalogFactory()
{
	m_pool.Clear ();
}



Catalog& CatalogFactory::Add (const wxString& catalogName, const wxString& path)
{
	Catalog* newCatalog = new Catalog (catalogName, path);
	m_pool.Add (*newCatalog);
	wxLogDebug ("Catalog added to the catalog pool: %s", newCatalog->GetName ());
	return *newCatalog;
}

Catalog& CatalogFactory::Get (const wxString& catalogName, const wxString& path)
{
	for (unsigned int j = 0; j < m_pool.GetCount(); j++) {
		Catalog& catalog = m_pool.Item (j);
		if (catalog.GetName () == catalogName)
			return catalog;
		}

	Catalog* catalog = new Catalog (catalogName, path);
	return *catalog;
}

Target& CatalogFactory::FindObject (const wxString& objectName)
{
	for (unsigned int j = 0; j < m_pool.GetCount(); j++) {
		if (m_pool[j].MatchPrefix (objectName)) {
			Target& target = m_pool[j].FindTarget (objectName);
			if (target.IsValid()) {
				return target;
				}
			}
		}

	return *new Target ();
}

int CatalogFactory::AddAll(const wxString& extension, const wxString& path)
{
	int catalogCount = 0;
	wxString ext = extension;

	if (!extension.Contains ("."))
		ext = wxT("*.") + extension;
	else if (extension.StartsWith ("."))
		ext = wxT("*") + extension;

	wxDir catalogDir (path);
	wxString catalogFileName;

	bool cont = catalogDir.GetFirst (&catalogFileName, ext, wxDIR_FILES);
	while (cont) {
		Add (catalogFileName, path);
		catalogCount++;
		cont = catalogDir.GetNext (&catalogFileName);
		}

	return catalogCount;
}

Catalog& CatalogFactory::Open(const wxString& catalogName, const wxString& path)
{
	Catalog* newCatalog = new Catalog (catalogName, path);
	wxLogDebug ("Catalog opened: %s", newCatalog->GetName ());
	return *newCatalog;
}
