#include "Catalog.h"
#include <wx/wfstream.h>
#include <wx/txtstrm.h>
#include <wx/tokenzr.h>
#include <wx/arrimpl.cpp>
#include <wx/filename.h>

WX_DEFINE_OBJARRAY (TargetList);

Catalog::Catalog () : m_name("")
{
	m_prefixList.Clear ();
}

Catalog::Catalog (const wxString& name, const wxString& path) : m_name(name), m_path(path)
{
	m_prefixList.Clear ();
}

Catalog::~Catalog()
{
}

void Catalog::LoadCache ()
{
	m_cache.Clear();
	wxFileName fn (m_path, m_name);
	fn.SetExt ("csv");
	wxFileInputStream fis (fn.GetFullPath());
	if (!fis.IsOk()) {
		wxLogDebug ("File not found: " + fn.GetFullPath());
		return;
		}

	wxTextInputStream tis (fis);
	wxString line;
	wxStringTokenizer stk;
	wxString name, sRa, sDecl, sFrameCount, sExpTime;
	Coordinate coord;
	Target newTarget;
	long expTime;
	long expCount;
	while (!fis.Eof()) {
		line = tis.ReadLine ();
		if (line == "")
			continue;
	
//		wxStringTokenizer stk (line, ";");
		stk.SetString (line, ";");
		name = stk.GetNextToken ();
		if (name == "")
			continue;

		sRa = stk.GetNextToken ();
		sDecl = stk.GetNextToken ();
		if (sRa == "" || sDecl == "")
			continue;

		expTime = 0;
		expCount = 0;
		if (stk.HasMoreTokens()) {
			sFrameCount = stk.GetNextToken ();
			sExpTime = stk.GetNextToken ();
			if (!sFrameCount.ToLong (&expCount))
				expCount = 0;
			if (!sExpTime.ToLong (&expTime))
				expTime = 0;
			}
		coord.FromString (sRa, sDecl);
//		Target* newTarget = new Target (name, coord);
		newTarget.SetName (name);
		newTarget.SetCoord (coord);
		if (expCount != 0 && expTime != 0) {
//			newTarget->SetExposureCount (expCount);
//			newTarget->SetExposureTime (expTime);
			newTarget.SetExposureCount (expCount);
			newTarget.SetExposureTime (expTime);
			}
		else {
			newTarget.SetExposureCount (0);
			newTarget.SetExposureTime (0);
			}
//		m_cache.Add (*newTarget);
		m_cache.Add (newTarget);
		}
}


Target& Catalog::FindTarget (const wxString& targetName)
{
	wxString objName = targetName.Lower ();

	if (m_cache.IsEmpty ()) {
		LoadCache ();
		}

//		if objname.startswith (self.prefix ()):
//			objname = objname[len(self.prefix()):].lstrip()

	for (unsigned int j = 0; j < m_cache.GetCount (); j++) {
		if (objName == m_cache[j].GetName().Lower()) {
			return m_cache[j];
			}
		}

	return *new Target();
}

TargetList& Catalog::GetAll ()
{
	if (m_cache.IsEmpty()) {
		LoadCache ();
		}
	return m_cache;
}

bool Catalog::MatchPrefix (const wxString& objectName)
{
	int pnum = m_prefixList.GetCount();

	/*
	** If no prefix was set, all object name are accepted
	*/
	if (pnum == 0)
		return true;

	/*
	** Search for prefix is match with the object name
	*/
	wxString name = objectName.Lower ();
	for (unsigned int j = 0; j < m_prefixList.GetCount(); j++) {
		if (name.StartsWith (m_prefixList[j])) {
			return true;
			}
		}
	return false;
}


