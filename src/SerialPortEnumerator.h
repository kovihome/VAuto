/*
** Serial Port Enumerator utility class
*/

#ifndef _SERIAL_PORT_ENUMERAOT_H_
#define _SERIAL_PORT_ENUMERAOT_H_

#include <wx/wx.h>

class SerialPortEnumerator
{
public:
	static wxArrayString& Enumerate ();
private:
	SerialPortEnumerator();
	~SerialPortEnumerator();
};

#endif // _SERIAL_PORT_ENUMERAOT_H_
