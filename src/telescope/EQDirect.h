/*
** Name:        EQDirect.h
** Purpose:     EQDirect telescope device
** Author:      Kvi
** Modified by:
** Created:     20/07/11
** Copyright:   (c) 2011 Kvi
** Licence:     wxWindows licence
*/

#ifndef _EQDIRECT_H_
#define _EQDIRECT_H_

#include "../Telescope.h"

class EQDirect :
	public Telescope
{
public:
	EQDirect(wxString comPort);
};

#endif // _EQDIRECT_H_
