//-------------------------------------------------------------------------
//
// File:		QuatC.h
// Desc:		Quaternion class.
// Author:		memon <memon@inside.org>
//
//-------------------------------------------------------------------------
//	Copyright (c) 2002 Moppi Productions. All Rights Reserved.
//  This file is part of Eskimo Fishing Trip game. For conditions of 
//  distribution and use, see the accompanying license.txt file.
//  http://moppi.inside.org/
//-------------------------------------------------------------------------

#ifndef __ESKIMO_QUATC_H__
#define __ESKIMO_QUATC_H__

#include "PajaTypes.h"
#include "Vector3C.h"
#include "Matrix3C.h"

namespace PajaTypes {

	class Matrix3C;

	//! Quaternion class.
	/*!	QuatC class defines a quaternion class which is used by
		the Demopaja system. The quaternion class implements
		standard set of methods and overdriven operators for easy use. 

		This class is implemented by the system. 
	*/
	class QuatC {
	public:
		//! Default constructor.
		QuatC( float32 f32X = 0, float32 f32Y = 0, float32 f32Z = 0, float32 f32W = 1 );

		//! copy constructor.
		QuatC( const QuatC& rQuat );

		//! Default destructor.
		virtual ~QuatC();

		//! Converts a matrix to quaternion and stores the result to the quaternion.
		void			from_rot_matrix( const Matrix3C& rMat );

		//! Converts the quaternion to matrix and returns the result.
		Matrix3C		to_rot_matrix() const;

		//! Creates quaternion from axis-angle presentation and stores the result in the quaternion.
		void			from_axis_angle( const Vector3C& rVec, float32 f32Angle );

		//! Creates quaternion from axis-angle presentation and stores the result in the quaternion.
		void			from_axis_angle( float32 f32X, float32 f32Y, float32 f32Z, float32 f32Angle );

		//! Converts the quaternion to axis-angle presentation and stores the result to the arguments.
		void			to_axis_angle( Vector3C& rVec, float32& f32Angle ) const;

		//! Converts the quaternion to axis-angle presentation and stores the result to the arguments.
		void			to_axis_angle( float32& f32X, float32& f32Y, float32& f32Z, float32& f32Angle ) const;

		//! Negates the quaternion and returns the result.
		QuatC			operator-() const;

		//! Return reference to the component pointed by the index.
		/*!	The index values from 0 to 3 corresponds to components X, Y, Z, and W respectively.
			There are two versions of this method, this version is to assign values. 
		*/
		float32&		operator[]( int32 i );

		//! Return reference to the component pointed by the index.
		/*!	The index values from 0 to 3 corresponds to components X, Y, Z, and W respectively.
			There are two versions of this method, this version is for retrieving the values. 
		*/
		const float32&	operator[]( int32 i ) const;

		//! Adds the quaternion specified by argument from the quaternion and returns the result. 
		QuatC			operator+( const QuatC& rQuat ) const;

		//! Substracts the quaternion specified by argument from the quaternion and returns the result. 
		QuatC			operator-( const QuatC& rQuat ) const;

		//! Multiplies the quaternion specified by argument from the quaternion and returns the result. 
		QuatC			operator*( const QuatC& rQuat ) const;

		//! Multiplies the quaternion by a scalar value specified by the argument and returns the result. 
		QuatC			operator*( float32 f32Scalar ) const;

		//! Multiplies the quaternion by a scalar value specified by the argument and returns the result. 
		friend QuatC	operator*( float32 f32Scalar, const QuatC& rQuat );
  
		//! Returns true if both quaternions are equal, else false. 
		bool			operator==( const QuatC& rQuat ) const;

		//! Returns true if both quaternions are non-equal, else false. 
		bool			operator!=( const QuatC& rQuat ) const;

		//! Computes the quaternion dot product and returns the result.
		float32			dot( const QuatC& rQuat ) const;

		//! Return the squared-length of the quaternion. 
		float32			norm() const;

		//! Converts the quaternion to unit-length and inverses it and returns the result.
		/*! Use this method to inverse non-unit-length quaternions. */
		QuatC			inverse() const;

		//! Inverses the quaternion and returns the result.
		/*! Use this method to inverse unit-length quaternions. */
		QuatC			unit_inverse() const;

		//! Normalizes the quaternion to unit length and returns the result. 
		QuatC			normalize() const;


	private:
		float32	m_f32Quat[4];

	};


	//
	//
	// inlines
	//
	//

	inline
	float32&
	QuatC::operator[]( int32 i )
	{
		assert( i >= 0 && i < 4 );
		return m_f32Quat[i];
	}

	inline
	const float32&
	QuatC::operator[]( int32 i ) const
	{
		assert( i >= 0 && i < 4 );
		return m_f32Quat[i];
	}

	inline
	bool
	QuatC::operator==( const QuatC& a ) const
	{
		return (m_f32Quat[0] == a.m_f32Quat[0]) &&
			   (m_f32Quat[1] == a.m_f32Quat[1]) &&
			   (m_f32Quat[2] == a.m_f32Quat[2]) &&
			   (m_f32Quat[3] == a.m_f32Quat[3]);
	}

	inline
	bool
	QuatC::operator!=( const QuatC& a ) const
	{
		return (m_f32Quat[0] != a.m_f32Quat[0]) ||
			   (m_f32Quat[1] != a.m_f32Quat[1]) ||
			   (m_f32Quat[2] != a.m_f32Quat[2]) ||
			   (m_f32Quat[3] != a.m_f32Quat[3]);
	}

	inline
	QuatC
	QuatC::operator+( const QuatC& rQuat ) const
	{
		return QuatC( m_f32Quat[0] + rQuat.m_f32Quat[0],
					  m_f32Quat[1] + rQuat.m_f32Quat[1],
					  m_f32Quat[2] + rQuat.m_f32Quat[2],
					  m_f32Quat[3] + rQuat.m_f32Quat[3] );
	}

	inline
	QuatC
	QuatC::operator-( const QuatC& rQuat ) const
	{
		return QuatC( m_f32Quat[0] - rQuat.m_f32Quat[0],
					  m_f32Quat[1] - rQuat.m_f32Quat[1],
					  m_f32Quat[2] - rQuat.m_f32Quat[2],
					  m_f32Quat[3] - rQuat.m_f32Quat[3] );
	}

	inline
	QuatC
	QuatC::operator*( float32 f32Scalar ) const
	{
		return QuatC( m_f32Quat[0] * f32Scalar,
					  m_f32Quat[1] * f32Scalar,
					  m_f32Quat[2] * f32Scalar,
					  m_f32Quat[3] * f32Scalar );
	}

	inline
	QuatC
	operator*( float32 f32Scalar, const QuatC& rQuat )
	{
		return QuatC( rQuat[0] * f32Scalar,
					  rQuat[1] * f32Scalar,
					  rQuat[2] * f32Scalar,
					  rQuat[3] * f32Scalar );
	}

	inline
	QuatC
	QuatC::operator-() const
	{
		QuatC	rRes = *this; //->normalize();
		rRes = rRes.normalize();
		return QuatC( -rRes.m_f32Quat[0],
					  -rRes.m_f32Quat[1],
					  -rRes.m_f32Quat[2],
					   rRes.m_f32Quat[3] );
	}

	inline
	float32
	QuatC::dot( const QuatC& rQuat ) const
	{
		return m_f32Quat[0] * rQuat.m_f32Quat[0] +
			   m_f32Quat[1] * rQuat.m_f32Quat[1] +
			   m_f32Quat[2] * rQuat.m_f32Quat[2] +
			   m_f32Quat[3] * rQuat.m_f32Quat[3];
	}

	inline
	float32
	QuatC::norm() const
	{
		return m_f32Quat[0] * m_f32Quat[0] +
			   m_f32Quat[1] * m_f32Quat[1] +
			   m_f32Quat[2] * m_f32Quat[2] +
			   m_f32Quat[3] * m_f32Quat[3];
	}


	inline
	QuatC
	QuatC::unit_inverse() const
	{
		return QuatC( -m_f32Quat[0], -m_f32Quat[1], -m_f32Quat[2], m_f32Quat[3]);
	}


}

#endif