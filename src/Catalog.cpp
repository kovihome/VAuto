#ifdef __WXMSW__
    #include <wx/msw/msvcrt.h>      // redefines the new() operator 
#endif 

#include "Catalog.h"
#include "../../VTools/src/Properties.h"
#include <wx/wfstream.h>
#include <wx/txtstrm.h>
#include <wx/tokenzr.h>
#include <wx/arrimpl.cpp>
#include <wx/filename.h>

WX_DEFINE_OBJARRAY (TargetList);

Catalog::Catalog () : m_filename(""), m_name("")
{
	m_prefixList.Clear ();
}

Catalog::Catalog (const wxString& filename, const wxString& path) : m_filename(filename), m_path(path)
{
	m_prefixList.Clear ();
	ReadHeader ();
}

Catalog::~Catalog()
{
	m_cache.Clear ();
	m_prefixList.Clear();
}

void Catalog::LoadCache ()
{
	m_cache.Clear();
	wxFileName fn (m_path, m_filename);
//	fn.SetExt ("csv");
	wxFileInputStream fis (fn.GetFullPath());
	if (!fis.IsOk()) {
		wxLogDebug ("File not found: " + fn.GetFullPath());
		return;
		}

	setlocale (LC_NUMERIC, "C");
	wxTextInputStream tis (fis);
	wxString line;
	wxStringTokenizer stk;
	wxString name, sRa, sDecl, sFrameCount, sExpTime;
	Coordinate coord;
	Target newTarget;
	long expTime;
	long expCount;

	wxArrayString arrLine;
	long ixObjectName = 0;
	long ixRa = 1;
	long ixDecl = 2;
	long ixExposureCount = -1;
	long ixExposureTime = -1;
	bool raSexa = true;
	bool declSexa = true;

	while (!fis.Eof()) {
		line = tis.ReadLine ();
		if (line == "")
			continue;

		arrLine = wxStringTokenize (line, ";\n");

		// field descriptor header lines
		if (arrLine[0].GetChar(0) == '#') {
			if (arrLine[0] == "#OBJECT_NAME") {
				arrLine[1].ToLong (&ixObjectName);
				}
			else if (arrLine[0] == "#RA") {
				arrLine[1].ToLong (&ixRa);
				raSexa = (arrLine[2] == "SEXA");
				}
			else if (arrLine[0] == "#DECL") {
				arrLine[1].ToLong (&ixDecl);
				declSexa = (arrLine[2] == "SEXA");
				}
			else if (arrLine[0] == "#EXPOSURE_COUNT") {
				arrLine[1].ToLong (&ixExposureCount);
				}
			else if (arrLine[0] == "#EXPOSURE_TIME") {
				arrLine[1].ToLong (&ixExposureTime);
				}
			continue;
			}
	
		// normal data lines
		name = arrLine[ixObjectName];
		if (name == "")
			continue;

		if (arrLine.GetCount() > ixRa) {
			sRa = arrLine[ixRa];
			sDecl = arrLine[ixDecl];
			if (sRa == "" || sDecl == "")
				continue;

			expTime = 0;
			expCount = 0;
			if (ixExposureCount != -1) {
				arrLine[ixExposureCount].ToLong (&expCount);
				}
			if (ixExposureTime != -1) {
				arrLine[ixExposureTime].ToLong (&expTime);
				}

			newTarget.SetName (name);
			if (raSexa) {
				Coordinate c;
				c.FromString (sRa, sDecl);
				newTarget.SetCoord (c);
				}
			else {
				newTarget.SetCoord (Coordinate (wxAtof (sRa), wxAtof (sDecl)));
				}
			newTarget.SetExposureCount (expCount);
			newTarget.SetExposureTime (expTime);
	
			m_cache.Add (newTarget);
			}
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



void Catalog::ReadHeader()
{
	wxFileName fn (m_path, m_filename);
	wxFileInputStream fis (fn.GetFullPath());
	if (!fis.IsOk()) {
		wxLogDebug ("File not found: " + fn.GetFullPath());
		return;
		}

	wxTextInputStream tis (fis);
	wxString line;
	wxArrayString arrLine;
	while (!fis.Eof()) {
		line = tis.ReadLine ();
		if (line == "")
			continue;
		if (line.GetChar (0) == '#') {
			arrLine = wxStringTokenize (line, ";\n");
			if (arrLine[0] == "#CATALOG_NAME") {
				SetName (arrLine[1]);
				}
			else if (arrLine[0] == "#CATALOG_PREFIX") {
				AddPrefix (arrLine[1]);
				}
			}
		else {
			break;
			}
		}
}
