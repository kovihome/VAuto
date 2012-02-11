/*
** Name:        ObservationLog.h
** Purpose:     VAuto Observations Log singleton class
** Author:      Kvi
** Modified by:
** Created:     20/09/11
** Copyright:   (c) 2011 Kvi
** Licence:     wxWindows licence
*/

#ifndef _OBSERVATION_LOG_H_
#define _OBSERVATION_LOG_H_

#include <wx/wx.h>
#include <wx/file.h>
#include "Coordinate.h"

#define LOG_STATUS_OK		wxT("OK")
#define LOG_STATUS_ABORT	wxT("Abort")
#define LOG_STATUS_ERROR	wxT("Error")

class ObservationLog :
	private wxFile
{
private:
	static ObservationLog* instance;
	ObservationLog();
	ObservationLog(ObservationLog&);
	ObservationLog& operator= (ObservationLog&);
	~ObservationLog();

	wxString m_FileName;

	wxString m_TargetName;
	Coordinate m_Coord;
	wxDateTime m_StartTime;
	int m_FrameIndex;
	int m_ExpositionTime;
	wxString m_Status;

	void Clear ();

public:
	static ObservationLog& Instance();

	void SetTargetName (const wxString& targetName);
	void SetCoord (const Coordinate& coord);
	void SetStartTime (const wxDateTime& startTime);
	void SetFrameIndex (int frameIndex);
	void SetExpositonTime (int expositionTime);
	void SetStatus (const wxString& status);

	void Flush ();
	void SetPath (const wxString& path);
};

#endif // _OBSERVATION_LOG_H_
