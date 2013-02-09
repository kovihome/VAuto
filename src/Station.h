/*
** Name:        Coordinate.h
** Purpose:     Coordinate holder and konerter class
** Author:      Kvi
** Modified by:
** Created:     26/09/10
** Copyright:   (c) 2011 Kvi
** Licence:     wxWindows licence
*/

#ifndef _STATION_H_
#define _STATION_H_

class Station
{
private:
	double longitude;
	double latitude;
public:
	Station() : longitude(0.0), latitude(0.0) {}
	Station(double aLongitude, double aLatitude) : longitude(aLongitude), latitude(aLatitude) {}
	virtual ~Station() {}
	double GetLongitude() { return longitude; }
	void SetLongitude(double aLongitude) { longitude = aLongitude; }
	double GetLatitude() { return latitude; }
	void SetLatitude(double aLatitude) { latitude = aLatitude; }
};

#endif // _STATION_H_
