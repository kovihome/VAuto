#ifdef __WXMSW__
    #include <wx/msw/msvcrt.h>      // redefines the new() operator 
#endif 

#include "EQDirect.h"

EQDirect::EQDirect(wxString comPort)
	: Telescope (comPort)
{
}

