/*
** Name:        TelescopeNotifier.h
** Purpose:     Background thread for notifying main dialog about telescope moving events
** Author:      Kvi
** Modified by:
** Created:     27/07/11
** Copyright:   (c) 2011 Kvi
** Licence:     wxWindows licence
*/

#ifndef _TELESCOPE_NOTIFIER_H_
#define _TELESCOPE_NOTIFIER_H_

#include "Telescope.h"
#include <wx/wx.h>
#include <wx/thread.h>

class TelescopeNotifier :
	public wxThread
{
public:
	enum TelescopeStopAction {
		STOP_NORMALLY = 0,
		STOP_DISCONNECT,
		STOP_TIMEOUT,
		STOP_ABORT
	};
private:
	Telescope* m_telescope;
	wxWindow* m_parent;
	bool m_abort;
	int m_id;
public:
	TelescopeNotifier(wxWindow* parent, int id, Telescope* telescope);
	virtual ~TelescopeNotifier() {};
	void OnExit ();
	void Abort ();
private:
	void SendEvent (TelescopeNotifier::TelescopeStopAction stopAction);
	ExitCode Entry ();
};

#endif // _TELESCOPE_NOTIFIER_H_
