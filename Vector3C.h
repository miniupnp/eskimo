//-------------------------------------------------------------------------
//
// File:		Vector3C.h
// Desc:		3D vector class.
// Author:		memon <memon@inside.org>
//
//-------------------------------------------------------------------------
//	Copyright (c) 2002 Moppi Productions. All Rights Reserved.
//  This file is part of Eskimo Fishing Trip game. For conditions of 
//  distribution and use, see the accompanying license.txt file.
//  http://moppi.inside.org/
//-------------------------------------------------------------------------

#ifndef __ESKIMO_VECTOR3_H__
#define __ESKIMO_VECTOR3_H__


#include "PajaTypes.h"
#include <math.h>
#include <assert.h>

namespace PajaTypes {

	//! 3D vector class.
	/*!	Vector3C class defines a three dimensional vector class which is used by
		the Demopaja system. The vector class implements standard set of
		methods and overdriven opertators for easy use. 

		This class is implemented by the system.
	*/
	class Vector3C {

	public:
		//! Default constructor.
		Vector3C( float32 f32X = 0, float32 f32Y = 0, float32 f32Z = 0 );

		//! Copy constructor.
		Vector3C( const Vector3C& rVec );

		//! Creates a new vector with the values as the array pointed by the argument.
		Vector3C( float32 f32A[3] );

		//! Default destructor.
		virtual ~Vector3C();

		//!	Returns reference to the component pointed by the index.
		/*!	The index values from 0 to 2 corresponds to components X, Y, and Z respectively.
			There are two versions of this method, this version is to assign values. 
		*/
		float32&			operator[]( int32 i );

		//!	Returns reference to the component pointed by the index.
		/*!	The index values from 0 to 2 corresponds to components X, Y, and Z respectively.
			There are two versions of this method, this version is for retrieving the values. 
		*/
		const float32&		operator[]( int32 i ) const;

		//! Negated vector.
		Vector3C			operator-() const;

		//! Returns true if both vectors are equal, else false.
		bool				operator==( const Vector3C& rVec ) const;

		//! Returns true if both vectors are not equal, else false. 
		bool				operator!=( const Vector3C& rVec ) const;

		//! Substracts the vector specified by argument from the vector and stores the result.
		Vector3C&			operator-=( const Vector3C& rVec );

		//! Adds the vector specified by argument from the vector and stores the result.
		Vector3C&			operator+=( const Vector3C& rVec );

		//! Multiplies the vector by a scalar value specified by the argument and stores the result.
		Vector3C&			operator*=( float32 f32S );

		//! Divides the vector by a scalar value specified by the argument and stores the result.
		Vector3C&			operator/=( float32 f32S );

		//! Multiplies the vector by a scalar value specified by the argument and returns the result.
		Vector3C			operator*( float32 f32S );

		//! Divides the vector by a scalar value specified by the argument and returns the result.
		Vector3C			operator/( float32 f32S );

		//! Substracts the vector specified by argument from the vector and returns the result.
		Vector3C			operator-( const Vector3C& rVec ) const;

		//! Adds the vector specified by argument from the vector and returns the result.
		Vector3C			operator+( const Vector3C& rVec ) const;

		//! Multiplies the vector specified by argument from the vector and returns the result.
		/*! The multiplication is done component vise (that is, each corresponding component is multiplied). */
		Vector3C			operator*( const Vector3C& rVec ) const;				// component vice mult

		//! Multiplies the vector by a scalar value specified by the argument and returns the result.
		friend Vector3C		operator*( float32 f32S, const Vector3C& rVec );

		//! Normalizes the vector to unit size and returns the result.
		Vector3C			normalize() const;

		//! Returns the length of the vector.
		float32				length() const;

		//! Computes the dot product of the two vectors and returns the result.
		float32				dot( const Vector3C& v ) const;

		//! Computes the cross product of the two vectors and returns the result. 
		Vector3C			cross( const Vector3C& v ) const;

	private:
		float32	m_vec[3];
	};



	//
	//
	// Inlines
	//
	//

	inline
	float32&
	Vector3C::operator[]( int32 i )
	{
		assert( i >= 0 && i < 4 );
		return m_vec[i];
	}

	inline
	const float32&
	Vector3C::operator[]( int32 i ) const
	{
		assert( i >= 0 && i < 3 );
		return m_vec[i];
	}

	inline
	Vector3C
	Vector3C::operator-() const
	{
		return Vector3C( -m_vec[0], -m_vec[1], -m_vec[2] );
	}

	inline
	Vector3C&
	Vector3C::operator-=( const Vector3C& a )
	{
		m_vec[0] -= a.m_vec[0];
		m_vec[1] -= a.m_vec[1];
		m_vec[2] -= a.m_vec[2];
		return *this;
	}

	inline
	Vector3C&
	Vector3C::operator+=( const Vector3C& a )
	{
		m_vec[0] += a.m_vec[0];
		m_vec[1] += a.m_vec[1];
		m_vec[2] += a.m_vec[2];
		return *this;
	}

	inline
	Vector3C&
	Vector3C::operator*=( float32 s )
	{
		m_vec[0] *= s;
		m_vec[1] *= s;
		m_vec[2] *= s;
		return *this;
	}

	inline
	Vector3C&
	Vector3C::operator/=( float32 s )
	{
		s = 1.0f / s;
		m_vec[0] *= s;
		m_vec[1] *= s;
		m_vec[2] *= s;
  		return *this;
	}

	inline
	Vector3C
	Vector3C::operator*( float32 s )
	{
		return Vector3C( m_vec[0] * s, m_vec[1] * s, m_vec[2] * s );
	}


	inline
	Vector3C
	Vector3C::operator/( float32 s )
	{
		s = 1.0f / s;
		return Vector3C( m_vec[0] * s, m_vec[1] * s, m_vec[2] * s );
	}

	inline
	Vector3C
	Vector3C::operator-( const Vector3C& a ) const
	{
		return Vector3C( m_vec[0] - a.m_vec[0], m_vec[1] - a.m_vec[1], m_vec[2] - a.m_vec[2] );
	}

	inline
	Vector3C
	Vector3C::operator+( const Vector3C& a ) const
	{
		return Vector3C( m_vec[0] + a.m_vec[0], m_vec[1] + a.m_vec[1], m_vec[2] + a.m_vec[2] );
	}

	inline
	Vector3C
	Vector3C::operator*( const Vector3C& a ) const
	{
		return Vector3C( m_vec[0] * a.m_vec[0], m_vec[1] * a.m_vec[1], m_vec[2] * a.m_vec[2] );
	}

	inline
	float32
	Vector3C::length() const
	{
		return (float32)sqrt( (m_vec[0] * m_vec[0]) + (m_vec[1] * m_vec[1]) + (m_vec[2] * m_vec[2]) );
	}

	inline
	float32
	Vector3C::dot( const Vector3C& v ) const
	{
		return (m_vec[0] * v.m_vec[0]) + (m_vec[1] * v.m_vec[1]) + (m_vec[2] * v.m_vec[2]);
	}

	inline
	Vector3C
	operator*( float32 s, const Vector3C& v )
	{
		return Vector3C( v.m_vec[0] * s, v.m_vec[1] * s, v.m_vec[2] * s );
	}


};	// namespace PajaTypes


#endif
