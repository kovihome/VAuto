#include "Coordinate.h"

Coordinate::Coordinate(double ra, double decl)
{
	_ra = ra;
	_decl = decl;
}

void Coordinate::ToBin16 (unsigned short& ra, unsigned short& decl)
{
	ra = _toBin16 (_ra);
	decl = _toBin16 (_decl);
}

void Coordinate::ToHex24 (wxString& ra, wxString& decl)
{
	ra = _toHex (_ra);
	decl = _toHex (_decl);
}

void Coordinate::FromHex24 (wxString& ra, wxString& decl)
{
	_ra = (double) (_fromHex (ra)) / 46603.3777777778;
	_decl = (double) (_fromHex (decl)) / 46603.3777777778;
}

void Coordinate::FromBin16 (unsigned short ra, unsigned short decl)
{
	_ra = (double) (_fromBin16 (ra)) / 182.0444444444;
	_decl = (double) (_fromBin16 (decl)) / 182.0444444444;
}

void Coordinate::ToString (wxString& ra, wxString& decl)
{
	ra = _dms(_ra/15.0, 2);
	decl = _dms(_decl, 2);
}

void Coordinate::ToStringEq (wxString& ra, wxString& decl)
{
	ra = _dms(_ra/15.0, 1);
	decl = _dms(_decl, 0);
}

void Coordinate::FromString (wxString& ra, wxString& decl)
{
	_ra = _invdms(ra) * 15.0;
	_decl = _invdms (decl);
}

double Coordinate::_fromBin16 (unsigned short bin)
{
	return (double) bin;
}

double Coordinate::_fromHex (wxString hex)
{
	double value = 0.0;
	for (unsigned int j = 0; j < hex.Length (); j++) {
		char c = hex.GetChar (j);
		//cv = ord(c)
		if (c >= '0' && c <= '9')
			c -= '0';
		else if (c >= 'a' && c <= 'f')
			c -= 'a';
		else
			c -= 'A';
		value = 16 * value + c;
		}
	return value;
}

unsigned short Coordinate::_toBin16 (double val)
{
	unsigned short d = (unsigned short)(val * 182.0444444444 + 0.5);
	if (d < 0)
		d = 0xffff + d;
	return d;
}

wxString Coordinate::_toHex (double val)
{
	unsigned short d = (unsigned short) (val * 46603.3777777778 + 0.5);
	if (d < 0)
		d = 0xffffff + d;
	wxString d_s = wxString::Format ("%06x", d);
	d_s = d_s.Pad (6, '0');
	return d_s.Upper();
}

wxString Coordinate::_dms (double val, int prec)
{
	bool positive = true;
	if (val < 0.0) {
		val = -val;
		positive = false;
		}

	double d = floor (val);
	double w = 60.0 * (val - d);
	int d_i = (int) d;
	if (!positive) {
		d_i = -d_i;
		}
	if (prec == 1) {
		return wxString::Format ("%02d %04.1f", d_i, w);
		}
	else if (prec == 0) {
		int w_i = int (w + 0.5);
		if (w_i >= 60) {
			w_i = 0;
			d_i += 1;
			}
		return wxString::Format ("%+03d %02d", d_i, w_i);
		}
	else {
		double m = floor (w);
		double s = 60.0 * (w - m);
		return wxString::Format ("%02d %02d %04.2f",  d_i, int(m), s);
		}
}

double Coordinate::_invdms (wxString& string)
{
	bool positive = true;
	if (string[0] == '-') {
		positive = false;
		string = string.Mid (1);
		}
	else if (string[0] == '+') {
		positive = true;
		string = string.Mid (1);
		}
	double v;
	if (string[2] == '.') {
		string.ToDouble (&v);
		}
	else {
		string.Left (2).ToDouble (&v);
		string = string.Mid(2).Strip(wxString::leading);
		if (string.Length() > 2 && string[2] == '.') {
			double w;
			string.ToDouble (&w);
			v += w / 60.0;
			}
		else {
			double w;
			string.Left(2).ToDouble (&w);
			v += w / 60.0;
			string = string.Mid(2).Strip(wxString::leading);
			if (string.Length() > 0) {
				double t;
				string.ToDouble (&t);
				v += t / 3600.0;
				}
			}
		}
	if (!positive) {
		v = -v;
		}
	return v;
}
