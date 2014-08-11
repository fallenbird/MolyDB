


#ifndef _JK_UTILITY_LX3BG8E_H_
#define _JK_UTILITY_LX3BG8E_H_


#ifdef WIN32

#define JK_SPRITF( dst, fmt, ... )  sprintf_s( dst, fmt, __VA_ARGS__ )

#else
#define JK_SPRITF(dst, fmt, ... ) sprintf( dst, fmt, __VA_ARGS__ )
#endif


#endif
