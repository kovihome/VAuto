/*
** Name:        Coordinate.h
** Purpose:     Coordinate holder and konerter class
** Author:      Kvi
** Modified by:
** Created:     26/09/10
** Copyright:   (c) 2011 Kvi
** Licence:     wxWindows licence
*/

#ifndef _COORDINATE_H_
#define _COORDINATE_H_

#include <wx/wx.h>

class Coordinate
{
public:
	Coordinate() : _ra(0.0), _decl(0.0) {};
	Coordinate(double ra, double decl);
	void ToBin16 (unsigned short& ra, unsigned short& decl);
	void ToHex24 (wxString& ra, wxString& decl);
	void FromBin16 (unsigned short ra, unsigned short decl);
	void FromHex24 (wxString& ra, wxString& decl);
	void ToString (wxString& ra, wxString& decl);
	void ToStringEq (wxString& ra, wxString& decl);
	void FromString (wxString& ra, wxString& decl);
private:
	double _ra;
	double _decl;

	double _fromHex (wxString hex);
	wxString _toHex (double val);
	double _fromBin16 (unsigned short bin);
	unsigned short _toBin16 (double val);
	wxString _dms (double val, int prec);
	double _invdms (wxString& string);
};

#endif // _COORDINATE_H_
