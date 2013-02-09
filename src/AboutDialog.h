/*
** Name:        AboutDialog.h
** Purpose:     VAuto About dialog
** Author:      Kvi
** Modified by:
** Created:     20/07/11
** Copyright:   (c) 2011 Kvi
** Licence:     wxWindows licence
*/

#ifndef _ABOUT_DIALOG_H_
#define _ABOUT_DIALOG_H_

#include <wx/wx.h>

class AboutDialog :
	public wxDialog
{
public:
	AboutDialog(wxWindow *parent);
private:
	void OnCancel(wxCommandEvent& ev);
	DECLARE_EVENT_TABLE();
};

#endif // _ABOUT_DIALOG_H_
