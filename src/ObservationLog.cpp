#include <wx/filename.h>
#include "ObservationLog.h"


ObservationLog* ObservationLog::instance = NULL;

ObservationLog::ObservationLog()
{
	Clear ();
}

ObservationLog::~ObservationLog()
{
}

ObservationLog& ObservationLog::Instance()
{
	if (instance == NULL) {
		instance = new ObservationLog ();
		}
	return *instance;
}

void ObservationLog::SetTargetName (const wxString& targetName)
{
	m_TargetName = targetName;
}

void ObservationLog::SetCoord (const Coordinate& coord)
{
	m_Coord = coord;
}

void ObservationLog::SetStartTime (const wxDateTime& startTime)
{
	m_StartTime = startTime;
}

void ObservationLog::SetFrameIndex (int frameIndex)
{
	m_FrameIndex = frameIndex;
}

void ObservationLog::SetExpositonTime (int expositionTime)
{
	m_ExpositionTime = expositionTime;
}

void ObservationLog::SetStatus (const wxString& status)
{
	m_Status = status;
}


void ObservationLog::Flush ()
{
	if (m_TargetName == "")
		m_TargetName = "unknown object";
	wxString ra, decl;
	m_Coord.ToString (ra, decl);
	wxString logText = wxString::Format ("%s;%s;%s;%s;%d;%d;%s\n",
		m_TargetName, 
		ra,
		decl,
		m_StartTime.FormatISODate() + " " + m_StartTime.FormatISOTime(),
		m_FrameIndex, 
		m_ExpositionTime, 
		m_Status);
	wxFile logf (m_FileName, wxFile::write_append);
	logf.Write (logText);
	logf.Close ();
}

void ObservationLog::SetPath (const wxString& path)
{
	wxDateTime d = wxDateTime::Now(); 
	wxString fileName = wxString::Format ("vauto-%04d%02d%02d", d.GetYear(), d.GetMonth(), d.GetDay());
	wxFileName fn (path, fileName, wxT(".log"));
	int k = 1;
	while (wxFileName::FileExists (fn.GetFullPath())) {
		fn.SetName (wxString::Format ("%s-%d", fileName, k++));
		}
	m_FileName = fn.GetFullPath();
}

void ObservationLog::Clear ()
{
	m_TargetName.Empty();
	m_StartTime = wxDateTime::Now ();
	m_FrameIndex = 0;
	m_ExpositionTime = 0;
	m_Status.Empty();
}

