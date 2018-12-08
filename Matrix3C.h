//-------------------------------------------------------------------------
//
// File:		Matrix3C.h
// Desc:		3x4 matrix class.
// Author:		memon <memon@inside.org>
//
//-------------------------------------------------------------------------
//	Copyright (c) 2002 Moppi Productions. All Rights Reserved.
//  This file is part of Eskimo Fishing Trip game. For conditions of 
//  distribution and use, see the accompanying license.txt file.
//  http://moppi.inside.org/
//-------------------------------------------------------------------------

#ifndef __ESKIMO_MATRIX3_H__
#define __ESKIMO_MATRIX3_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


#include "PajaTypes.h"
#include "Vector3C.h"
#include "QuatC.h"

namespace PajaTypes {

	class QuatC;

	//! 3x4 matrix class.
	/*!	Matrix3C class defines a 3 by 4 matrix class which is
		used by the Demopaja system The matrix class implements
		standard set of methods and overdriven operators for easy use. 

		This class is implemented by the system. 
	*/
	class Matrix3C {
	public:

		//! Default constructor
		Matrix3C();

		//! Copy constructor
		Matrix3C( const Matrix3C& rMat );

		//! Contructor from array of floats.
		/*!	Creates a new vector with the values as the array pointed by the argument.
			The array should contain 12 values, 3 for each row in the matrix.
			The rows in the array are arranged linearly. 
		*/
		Matrix3C( const float32* pMat );

		//! Default destructor.
		virtual ~Matrix3C();

		//! Multiplies two matrices and returns the result.
		Matrix3C			operator*( const Matrix3C& rMat ) const;

		//! Returns reference to the vector at a row pointed by the index. 
		/*!	The index values from 0 to 2 corresponds to the rows in the matrix.
			There are two versions of this method, this version enables to assign values. 
		*/
		Vector3C&			operator[]( int32 i );

		//! Returns reference to the vector at a row pointed by the index. 
		/*!	The index values from 0 to 2 corresponds to the rows in the matrix.
			There are two versions of this method this version is for retrieving the values. 
		*/
		const Vector3C&		operator[]( int32 i ) const;

		//! Multiplies the specified vector by the specified matrix and returns the result.
		friend Vector3C		operator*( const Matrix3C& rMat, const Vector3C& rVec );
		
		//! Multiplies the specified vector by the specified matrix and returns the result.
		friend Vector3C		operator*( const Vector3C& rVec, const Matrix3C& rMat );
		
		//! Multiplies the specified vector by the specified matrix and stores the result to the vector. 
		friend Vector3C&	operator*=( Vector3C& rVec, const Matrix3C& rMat );

		//! Sets the matrix as identity matrix.
		Matrix3C&	set_identity();

		//! Sets the matrix to identity and then translates the matrix to the value of the argument. 
		Matrix3C&	set_trans( const Vector3C& v );

		//! Sets the matrix to identity and then scales the matrix to the value of the argument. 
		Matrix3C&	set_scale( const Vector3C& v );

		//! Converts quaternion to matrix and stores the result to the matrix.
		Matrix3C&	set_rot( const QuatC& rQuat );

		//! Inverts the matrix and returns the result.
		Matrix3C	inverse() const;



	private:
		Vector3C	m_rMat[4];
	};



	//
	// Inlines
	//

	inline
	Vector3C&
	Matrix3C::operator[]( int i )
	{
		assert( i >= 0 && i < 4 );
		return m_rMat[i];
	}

	inline
	const Vector3C&
	Matrix3C::operator[]( int i ) const
	{
		assert( i >= 0 && i < 4 );
		return m_rMat[i];
	}

	inline
	Vector3C
	operator*( const Matrix3C& m, const Vector3C& v )
	{
		return Vector3C( v[0] * m[0][0] + v[1] * m[1][0] + v[2] * m[2][0] + m[3][0],
						 v[0] * m[0][1] + v[1] * m[1][1] + v[2] * m[2][1] + m[3][1],
						 v[0] * m[0][2] + v[1] * m[1][2] + v[2] * m[2][2] + m[3][2] );
	}

	inline
	Vector3C
	operator*( const Vector3C& v, const Matrix3C& m )
	{
		return Vector3C( v[0] * m[0][0] + v[1] * m[1][0] + v[2] * m[2][0] + m[3][0],
						 v[0] * m[0][1] + v[1] * m[1][1] + v[2] * m[2][1] + m[3][1],
						 v[0] * m[0][2] + v[1] * m[1][2] + v[2] * m[2][2] + m[3][2] );
	}

	inline
	Vector3C&
	operator*=( Vector3C& v, const Matrix3C& m )
	{
		Vector3C	rTmp = v;
		v[0] = rTmp[0] * m[0][0] + rTmp[1] * m[1][0] + rTmp[2] * m[2][0] + m[3][0];
		v[1] = rTmp[0] * m[0][1] + rTmp[1] * m[1][1] + rTmp[2] * m[2][1] + m[3][1];
		v[2] = rTmp[0] * m[0][2] + rTmp[1] * m[1][2] + rTmp[2] * m[2][2] + m[3][2];
		return v;
	}

};	// namespace PajaTypes

#endif