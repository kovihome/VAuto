/*
** Name:        TelescopeFactory.h
** Purpose:     Factory class for creating Telescope object from telescope type string
** Author:      Kvi
** Modified by:
** Created:     20/07/11
** Copyright:   (c) 2011 Kvi
** Licence:     wxWindows licence
*/

#ifndef _TELESCOPE_FACTORY_H_
#define _TELESCOPE_FACTORY_H_

#include "Telescope.h"
#include <wx/wx.h>
#include <wx/arrstr.h>

class TelescopeFactory
{
private:
	TelescopeFactory();
	TelescopeFactory(const TelescopeFactory&);
	TelescopeFactory& operator= (TelescopeFactory&);

	static TelescopeFactory* instance;
	Telescope *m_mount;
public:
	static TelescopeFactory& Instance();
	wxArrayString& EnumerateDrivers ();
	wxString& GetDriverDisplayName (const wxString& driverType);
	Telescope* GetTelescope (wxString mountType, wxString comPort); 
	void ClearTelescope ();
};

#endif // _TELESCOPE_FACTORY_H_
