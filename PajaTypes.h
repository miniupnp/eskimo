//-------------------------------------------------------------------------
//
// File:		ClassDescC.h
// Desc:		General purpose types.
// Author:		memon <memon@inside.org>
//
//-------------------------------------------------------------------------
//	Copyright (c) 2002 Moppi Productions. All Rights Reserved.
//  This file is part of Eskimo Fishing Trip game. For conditions of 
//  distribution and use, see the accompanying license.txt file.
//  http://moppi.inside.org/
//-------------------------------------------------------------------------

#ifndef __ESKIMO_PAJATYPES_H__
#define __ESKIMO_PAJATYPES_H__


//! Generic types used in Demopaja.
/*!	The PajaTypes namespace contains all basic data types used in Demopaja, such
	as integer types, floating-point types, vectors, matrices, quaternion, and color.
*/
namespace PajaTypes {

	typedef char			int8;		//!< Signed 8-bit integer.
	typedef unsigned char	uint8;		//!< Unsigned 8-bit integer.
	typedef short			int16;		//!< Signed 16-bit integer.
	typedef unsigned short	uint16;		//!< Unsigned 16-bit integer.
	typedef int				int32;		//!< Signed 32-bit integer.
	typedef unsigned int	uint32;		//!< Unsigned 32-bit integer.

	typedef float			float32;	//!< Single precicion floating point value.
	typedef double			float64;	//!< Double precicion floating point value.
	typedef long double		float80;	//!< Extra precicion floating point value.

};	// namespace PajaTypes

#ifndef M_PI
#define M_PI 3.1415926536				//!< M_PI (the value of Pi) for compiler with uncomplete math.h.
#endif

#endif