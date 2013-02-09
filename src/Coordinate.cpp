#ifdef __WXMSW__
    #include <wx/msw/msvcrt.h>      // redefines the new() operator 
#endif 

#include "Coordinate.h"

#define deg2rad(d) (d*PI/180.0)
#define rad2deg(r) (r*180.0/PI)
#define hour2deg(h) (h*360.0/24.0)
#define sgn(x) (x<0?-1:1)
#define PI 3.1415926

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
	if (_decl > 180.0)
		_decl -= 360.0;
}

void Coordinate::FromBin16 (unsigned short ra, unsigned short decl)
{
	_ra = (double) (_fromBin16 (ra)) / 182.0444444444;
	_decl = (double) (_fromBin16 (decl)) / 182.0444444444;
	if (_decl > 180.0)
		_decl -= 360.0;
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
			c += 10 - 'a';
		else
			c += 10 - 'A'; 
		value = 16 * value + c;
		}
	return value;
}

unsigned short Coordinate::_toBin16 (double val)
{
	unsigned short d = (unsigned short)(val * 182.0444444444 + 0.5);
	if (val < 0.0)
		d = 0xffff + d; // ???
	return d;
}	

wxString Coordinate::_toHex (double val)
{
	unsigned long d = (unsigned long) (val * 46603.3777777778 + 0.5);
	if (val < 0.0)
		d &= 0x00ffffff;
	wxString d_s = wxString::Format ("%06x", d);
	return d_s.Upper();
}

wxString Coordinate::_dms (double val, int prec)
{
	bool positive = true;
	char sign = '+';
	if (val < 0.0) {
		val = -val;
		positive = false;
		sign = '-';
		}

	double d = floor (val);
	double w = 60.0 * (val - d);
	int d_i = (int) d;
//	if (!positive) {
//		d_i = -d_i;
//		}
	if (prec == 1) {
		return wxString::Format ("%c%02d %04.1f", sign, d_i, w);
		}
	else if (prec == 0) {
		int w_i = int (w + 0.5);
		if (w_i >= 60) {
			w_i = 0;
			d_i += 1;
			}
		return wxString::Format ("%c%02d %02d", sign, d_i, w_i);
		}
	else {
		double m = floor (w);
		double s = 60.0 * (w - m);
		return wxString::Format ("%c%02d %02d %04.2f",  sign, d_i, int(m), s);
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

double Coordinate::RangeDegrees (double angle)
{
	if (angle >= 0.0 && angle < 360.0)
		return angle;
	int temp = (int) (angle / 360.0);
	if (angle < 0.0)
		temp -= 1;
	temp *= 360;
	return angle - (double) temp;
}

double Coordinate::MeanGST (double jd)
{
    double T = (jd - 2451545.0) / 36525.0;
    // calc mean angle
    double sidereal = 280.46061837 + (360.98564736629 * (jd - 2451545.0)) + (0.000387933 * T * T) - (T * T * T / 38710000.0);
    // add a convenient multiple of 360 degrees
    sidereal = RangeDegrees (sidereal);
    // change to hours
    sidereal *= 24.0 / 360.0;
    return sidereal;
}

double Coordinate::MeanLST (Station& station, double jd)
{
	double mgst = MeanGST (jd);
	double mlst = mgst + (24 * station.GetLongitude()) / 360.0;
	return mlst;
}

void Coordinate::EquatorialToHorizontal (Station& station, double ha, double decl, double* az, double* h)
{
	/*
		Convert equitorial to horizontal coordinates.
	    [Meeus-1998: equations 13.5, 13.6]
		Note that azimuth is measured westward starting from the south.
	    This is not a good formula for using near the poles.
		Parameters:
			H : hour angle in radians
	        decl : declination in radians
		Returns:
			azimuth in radians
	        altitude in radians
	*/
    
    double cosH = cos(ha);
    double sinLat = sin(deg2rad(station.GetLatitude()));
    double cosLat = cos(deg2rad(station.GetLatitude()));
    *az = atan2(sin(ha), cosH * sinLat - tan(decl) * cosLat);
    *h = asin(sinLat * sin(decl) + cosLat * cos(decl) * cosH);
}


double Coordinate::GetAltitude (Station& station, double jd)
{
	double ha = deg2rad (hour2deg(MeanLST (station, jd))) - deg2rad(_ra);
	double az, h;
	EquatorialToHorizontal (station, ha, deg2rad (_decl), &az, &h);
	return rad2deg(h);
}

bool Coordinate::AboveHorizon (Station& station, double jd)
{
	double alt = GetAltitude (station, jd);
	return (alt > 0.0);
}

