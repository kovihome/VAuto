/*
** common.h -- commonly used declarations (defines, macros, externs. etc.)
** -----------------------------------------------------------------------
*/

#ifndef _VAUTO_COMMON_H_
#define _VAUTO_COMMON_H_

/*
**	Set SIMULATE_CONNECTION_CAMERA to 0 for operation DSLR normally
**	Set SIMULATE_CONNECTION_CAMERA to 1 for simulate operations, but not connected to DSLR hardware
*/
#define SIMULATE_CONNECTION_CAMERA 0

/*
**	Set SIMULATE_CONNECTION_TELESCOPE to 0 for operation telescope normally
**	Set SIMULATE_CONNECTION_TELESCOPE to 1 for simulate operations, but not connected to telescope hardware
*/
#define SIMULATE_CONNECTION_TELESCOPE 1



/*
** common macros
** -------------
*/

//
// dim(x) -- dimension of an array, typesafe version (DDJ No.395)
//
#ifdef _DEBUG
#define dim(x) ( \
	0 * sizeof ( reinterpret_cast<const ::Bad_arg_to_dim*>(x) ) + \
	0 * sizeof ( ::Bad_arg_to_dim::check_type((x), &(x)) ) + \
	sizeof(x)/sizeof(x[0]) )
class Bad_arg_to_dim
{
public:
	class Is_pointer; // intentionally incomplete type
	class Is_array {};
	template<typename T>
	static Is_pointer check_type (const T*, const T* const*);
	static Is_array check_type (const void *, const void *);
};
#else
	#define dim(x) (sizeof(x)/sizeof(x[0]))
#endif











#endif // _VAUTO_COMMON_H_
