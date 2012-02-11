#include "CatalogFactory.h"
#include <wx/arrimpl.cpp>

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

Catalog& CatalogFactory::Add (const wxString& catalogName, const wxString& path)
{
	Catalog* newCatalog = new Catalog (catalogName, path);
	m_pool.Add (*newCatalog);
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
