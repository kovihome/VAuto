/*
** Name:        Target.h
** Purpose:     Target Object entity
** Author:      Kvi
** Modified by:
** Created:     27/07/11
** Copyright:   (c) 2011 Kvi
** Licence:     wxWindows licence
*/

#ifndef _TARGET_H_
#define _TARGET_H_

#include "Coordinate.h"
#include <wx/wx.h>

class Target :
	public wxObject
{
private:
	wxString m_name;
	Coordinate m_coord;
	int m_exposureTime;
	int m_exposureCount;
	bool m_valid;
public:
	Target() : m_name(""), m_coord(), m_exposureTime(0), m_exposureCount(0), m_valid(false)  {}
	Target (const wxString& name, const Coordinate& coord) : m_name(name), m_coord(coord), m_exposureTime(0), m_exposureCount(0), m_valid(true) {}
	Target (const wxString& name, const Coordinate& coord, int exposureTime, int exposureCount) : m_name(name), m_coord(coord), m_exposureTime(exposureTime), m_exposureCount(exposureCount), m_valid(true) {}
	Target (const Target& target) : m_name(target.m_name), m_coord(target.m_coord), m_exposureTime(target.m_exposureTime), m_exposureCount(target.m_exposureCount), m_valid(target.m_valid) {}
	Target& operator= (const Target& that) { 
		this->m_name = that.m_name; 
		this->m_coord = that.m_coord; 
		this->m_exposureTime = that.m_exposureTime;
		this->m_exposureCount = that.m_exposureCount;
		this->m_valid = that.m_valid; 
		return *this; }
	virtual ~Target(void) {}
	wxString& GetName () { return m_name; }
	void SetName (const wxString& name) { m_name = name; }
	Coordinate& GetCoord () { return m_coord; }
	void SetCoord (const Coordinate& coord) { m_coord = coord; m_valid = true; }
	int GetExposureTime () { return m_exposureTime; }
	void SetExposureTime (int exposureTime) { m_exposureTime = exposureTime; }
	int GetExposureCount () { return m_exposureCount; }
	void SetExposureCount (int exposureCount) { m_exposureCount = exposureCount; }
	bool IsValid () { return m_valid; }
};

#endif // _TARGET_H_

