//-------------------------------------------------------------------------
//
// File:		ColorC.h
// Desc:		RGBA color class.
// Author:		memon <memon@inside.org>
//
//-------------------------------------------------------------------------
//	Copyright (c) 2002 Moppi Productions. All Rights Reserved.
//  This file is part of Eskimo Fishing Trip game. For conditions of 
//  distribution and use, see the accompanying license.txt file.
//  http://moppi.inside.org/
//-------------------------------------------------------------------------

#ifndef __ESKIMO_COLORC_H__
#define __ESKIMO_COLORC_H__

#include "PajaTypes.h"
#include <assert.h>


namespace PajaTypes {

	//! RGBA color class.
	/*!	Color class defines a four component color type.
		The components are red, green, blue and alpha.
		All components are handled similar inside the class. 

		The assumed color range is 0..1.

		This class is implemented by the system. 
	*/
	class ColorC
	{
	public:
		
		//! Copy constructor
		ColorC( const ColorC& rCol );
		
		//! Default constructor
		ColorC( float32 f32R = 0, float32 f32G = 0, float32 f32B = 0, float32 f32A = 1.0f );

		//! Default destructor
		~ColorC();

		//! Converts color from unsigned integer values.
		/*! All values are in range 0..255. */
		void			convert_from_uint8( uint8 ui8R, uint8 ui8G, uint8 ui8B, uint8 ui8A = 255 );

		//! Constant float pointer cast operator.
						operator const float32*();

		//! Returns component of the color.
		/*!	The index values from 0 to 3 corresponds to components
			R, G, B, and A respectively. There are two versions of
			this method, this version enables to assign values. 
		*/
		float32&		operator[]( int32 i );

		//! Returns component of the color.
		/*!	The index values from 0 to 3 corresponds to components
			R, G, B, and A respectively. There are two versions of
			this method, this version is for retrieving the values. 
		*/
		const float32&	operator[]( int32 i ) const;

		//! Returns negated color.
		ColorC			operator-() const;

		//! Returns true if both colors are equal, else false.
		bool			operator==( const ColorC& rCol ) const;

		//! Returns true if colors are not equal, else false.
		bool			operator!=( const ColorC& rCol ) const;

		//! Subtracts the color specified by the argument from the color. 
		ColorC&			operator-=( const ColorC& rCol );

		//! Adds the color specified by the argument to the color.
		ColorC&			operator+=( const ColorC& rCol );

		//! Multiplies each color component by the specified number. 
		ColorC&			operator*=( float32 f32S );

		//! Divides each color component by the specified number.
		ColorC&			operator/=( float32 f32S );

		//! Multiplies each color component by the specified number and returns the result.
		ColorC			operator*( float32 f32S ) const;

		//! Divides each color component by the specified number and returns the result.
		ColorC			operator/( float32 f32S ) const;

		//! Subtracts the color specified by the argument from the color and returns the result.
		ColorC			operator-( const ColorC& rCol ) const;

		//! Adds the color specified by the argument to the color and returns the result.
		ColorC			operator+( const ColorC& rCol ) const;

		//! Multiplies each component of the color by matching component in the specified argument and returns the result.
		ColorC			operator*( const ColorC& rCol ) const;

		//! Multiplies each color component by the specified number and returns the result.
		friend ColorC	operator*( float32 f32S, const ColorC& rCol );

	private:
		float32		m_col[4];
	};


	//
	//
	// Inlines
	//
	//

	inline
	ColorC::operator const float*()
	{
		return m_col;
	}

	inline
	float&
	ColorC::operator[]( int i )
	{
		assert( i >= 0 && i < 4 );
		return m_col[i];
	}

	inline
	const float&
	ColorC::operator[]( int i ) const
	{
		assert( i >= 0 && i < 4 );
		return m_col[i];
	}

	inline
	ColorC
	ColorC::operator-() const
	{
		return ColorC( -m_col[0], -m_col[1], -m_col[2], -m_col[3] );
	}

	inline
	ColorC&
	ColorC::operator-=( const ColorC& a )
	{
		m_col[0] -= a.m_col[0];
		m_col[1] -= a.m_col[1];
		m_col[2] -= a.m_col[2];
		m_col[3] -= a.m_col[3];
		return *this;
	}

	inline
	ColorC&
	ColorC::operator+=( const ColorC& a )
	{
		m_col[0] += a.m_col[0];
		m_col[1] += a.m_col[1];
		m_col[2] += a.m_col[2];
		m_col[3] += a.m_col[3];
		return *this;
	}

	inline
	ColorC&
	ColorC::operator*=( float s )
	{
		m_col[0] *= s;
		m_col[1] *= s;
		m_col[2] *= s;
		m_col[3] *= s;
		return *this;
	}

	inline
	ColorC&
	ColorC::operator/=( float s )
	{
		s = 1.0f / s;
		m_col[0] *= s;
		m_col[1] *= s;
		m_col[2] *= s;
		m_col[3] *= s;
		return *this;
	}

	inline
	ColorC
	ColorC::operator*( float s ) const
	{
		return ColorC( m_col[0] * s, m_col[1] * s, m_col[2] * s, m_col[3] * s );
	}


	inline
	ColorC
	ColorC::operator/( float s ) const
	{
		s = 1.0f / s;
		return ColorC( m_col[0] * s, m_col[1] * s, m_col[2] * s, m_col[3] * s );
	}

	inline
	ColorC
	ColorC::operator-( const ColorC& a ) const
	{
		return ColorC( m_col[0] - a.m_col[0], m_col[1] - a.m_col[1], m_col[2] - a.m_col[2], m_col[3] - a.m_col[3] );
	}

	inline
	ColorC
	ColorC::operator+( const ColorC& a ) const
	{
		return ColorC( m_col[0] + a.m_col[0], m_col[1] + a.m_col[1], m_col[2] + a.m_col[2], m_col[3] + a.m_col[3] );
	}

	inline
	ColorC
	ColorC::operator*( const ColorC& a ) const
	{
		return ColorC( m_col[0] * a.m_col[0], m_col[1] * a.m_col[1], m_col[2] * a.m_col[2], m_col[3] * a.m_col[3] );
	}

	inline
	ColorC
	operator*( float s, const ColorC& v )
	{
		return ColorC( v.m_col[0] * s, v.m_col[1] * s, v.m_col[2] * s, v.m_col[3] * s );
	}


};	// namespace PajaTypes

#endif	// __ESKIMO_COLORC_H_