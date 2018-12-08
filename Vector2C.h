//-------------------------------------------------------------------------
//
// File:		Vector2C.h
// Desc:		2D vector class.
// Author:		memon <memon@inside.org>
//
//-------------------------------------------------------------------------
//	Copyright (c) 2002 Moppi Productions. All Rights Reserved.
//  This file is part of Eskimo Fishing Trip game. For conditions of 
//  distribution and use, see the accompanying license.txt file.
//  http://moppi.inside.org/
//-------------------------------------------------------------------------

#ifndef __ESKIMO_VECTOR2_H__
#define __ESKIMO_VECTOR2_H__


#include "PajaTypes.h"
#include <math.h>
#include <assert.h>

namespace PajaTypes {

	//! 2D Vector class.
	/*!	Vector2C class defines a two dimensional vector class which is used by
		the Demopaja system. The vector class implements standard set of methods
		and overdriven opertators for easy use. 

		This class is implemented by the system. 
	*/
	class Vector2C {

	public:
		//! Default constructor.
		Vector2C( float32 f32X = 0, float32 f32Y = 0 );
		
		//! Copy constructor
		Vector2C( const Vector2C& rVec );
		
		//! Creates a new vector with the values as the array pointed by the argument.
		Vector2C( const float32 f32A[2] );

		//! Default destructor.
		virtual ~Vector2C();

		//! Return reference to the component pointed by the index. 
		/*!	The index values from 0 to 1 corresponds to components X, and Y respectively.
			There are two versions of this method, this version is to assign values. 
		*/
		float32&			operator[]( int32 i );

		//! Return reference to the component pointed by the index. 
		/*!	The index values from 0 to 1 corresponds to components X, and Y respectively.
			There are two versions of this method, this version is for retrieving the values. 
		*/
		const float32&		operator[]( int32 i ) const;

		//! Returns Negated vector.
		Vector2C			operator-() const;

		//! Return true if both vectors are equal, else false. 
		bool				operator==( const Vector2C& rVec ) const;

		//! Return true if both vectors are not equal, else false. 
		bool				operator!=( const Vector2C& rVec ) const;

		//! Substracts the vector specified by argument from the vector and stores the result.
		Vector2C&			operator-=( const Vector2C& rVec );

		//! Adds the vector specified by argument from the vector and stores the result.
		Vector2C&			operator+=( const Vector2C& rVec );

		//! Multiplies the vector by a scalar value specified by the argument and stores the result.
		Vector2C&			operator*=( float32 f32S );

		//! Divides the vector by a scalar value specified by the argument and stores the result.
		Vector2C&			operator/=( float32 f32S );

		//! Multiplies the vector by a scalar value specified by the argument and returns the result. 
		Vector2C			operator*( float32 f32S ) const;

		//! Divides the vector by a scalar value specified by the argument and returns the result.
		Vector2C			operator/( float32 f32S ) const;

		//! Substracts the vector specified by argument from the vector and returns the result. 
		Vector2C			operator-( const Vector2C& rVec ) const;

		//! Adds the vector specified by argument from the vector and returns the result. 
		Vector2C			operator+( const Vector2C& rVec ) const;

		//! Multiplies the vector specified by argument from the vector and returns the result.
		/*!	The multiplication is done component vise (that is, each corresponding component is multiplied).  */
		Vector2C			operator*( const Vector2C& rVec ) const;

		//! Multiplies the vector by a scalar value specified by the argument and returns the result. 
		friend Vector2C		operator*( float32 s, const Vector2C& rVec );

		//! Normalizes the vector to unit size and returns the result.
		Vector2C			normalize() const;

		//! Returns the length of the vector.
		float32				length() const;

		//! Computes the dot product of the two vectors and returns the result.
		float32				dot( const Vector2C& rVec ) const;


	private:
		float32	m_vec[2];
	};

	//
	//
	// Inlines
	//
	//

	inline
	float32&
	Vector2C::operator[]( int32 i )
	{
		assert( i >= 0 && i < 2 );
		return m_vec[i];
	}

	inline
	const float32&
	Vector2C::operator[]( int32 i ) const
	{
		assert( i >= 0 && i < 2 );
		return m_vec[i];
	}

	inline
	Vector2C
	Vector2C::operator-() const
	{
		return Vector2C( -m_vec[0], -m_vec[1] );
	}

	inline
	Vector2C&
	Vector2C::operator-=( const Vector2C& a )
	{
		m_vec[0] -= a.m_vec[0];
		m_vec[1] -= a.m_vec[1];
  
		return *this;
	}

	inline
	Vector2C&
	Vector2C::operator+=( const Vector2C& a )
	{
		m_vec[0] += a.m_vec[0];
		m_vec[1] += a.m_vec[1];
  
		return *this;
	}

	inline
	Vector2C&
	Vector2C::operator*=( float32 s )
	{
		m_vec[0] *= s;
		m_vec[1] *= s;
  
		return *this;
	}

	inline
	Vector2C&
	Vector2C::operator/=( float32 s )
	{
		s = 1.0f / s;
		m_vec[0] *= s;
		m_vec[1] *= s;
  
		return *this;
	}

	inline
	Vector2C
	Vector2C::operator*( float32 s ) const
	{
		return Vector2C( m_vec[0] * s, m_vec[1] * s );
	}


	inline
	Vector2C
	Vector2C::operator/( float32 s ) const
	{
		s = 1.0f / s;
		return Vector2C( m_vec[0] * s, m_vec[1] * s );
	}

	inline
	Vector2C
	Vector2C::operator-( const Vector2C& a ) const
	{
		return Vector2C( m_vec[0] - a.m_vec[0], m_vec[1] - a.m_vec[1] );
	}

	inline
	Vector2C
	Vector2C::operator+( const Vector2C& a ) const
	{
		return Vector2C( m_vec[0] + a.m_vec[0], m_vec[1] + a.m_vec[1] );
	}

	inline
	Vector2C
	Vector2C::operator*( const Vector2C& a ) const
	{
		return Vector2C( m_vec[0] * a.m_vec[0], m_vec[1] * a.m_vec[1] );
	}

	inline
	float32
	Vector2C::length() const
	{
		return (float32)sqrt( (m_vec[0] * m_vec[0]) + (m_vec[1] * m_vec[1]) );
	}

	inline
	float32
	Vector2C::dot( const Vector2C& v ) const
	{
		return (m_vec[0] * v.m_vec[0]) + (m_vec[1] * v.m_vec[1]);
	}

	inline
	Vector2C
	operator*( float32 s, const Vector2C& v )
	{
		return Vector2C( v.m_vec[0] * s, v.m_vec[1] * s );
	}


};	// namespace PajaTypes


#endif