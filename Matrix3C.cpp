//-------------------------------------------------------------------------
//
// File:		Matrix3C.cpp
// Desc:		3x4 matrix class.
// Author:		memon <memon@inside.org>
//
//-------------------------------------------------------------------------
//	Copyright (c) 2002 Moppi Productions. All Rights Reserved.
//  This file is part of Eskimo Fishing Trip game. For conditions of 
//  distribution and use, see the accompanying license.txt file.
//  http://moppi.inside.org/
//-------------------------------------------------------------------------

#include "Matrix3C.h"
#include "Vector3C.h"
#include "QuatC.h"
#include <math.h>
#include <assert.h>

using namespace PajaTypes;


Matrix3C::Matrix3C()
{
	// empty
}

Matrix3C::Matrix3C( const Matrix3C& m )
{
	m_rMat[0] = m[0];
	m_rMat[1] = m[1];
	m_rMat[2] = m[2];
	m_rMat[3] = m[3];
}

Matrix3C::Matrix3C( const float32* m )
{
	for( int32 i = 0; i < 4; i++ )
		for( int32 j = 0; j < 3; j++ )
			m_rMat[i][j] = m[i * 3 + j];
}


Matrix3C::~Matrix3C()
{
	// empty
}



Matrix3C
Matrix3C::operator*( const Matrix3C& m ) const
{
	Matrix3C	result;

	for( int32 row = 0; row < 4; row++ ) {
		for( int32 col = 0; col < 3; col++ ) {
			result[row][col] = m_rMat[row][0] * m[0][col] + m_rMat[row][1] * m[1][col] + m_rMat[row][2] * m[2][col];
			if( row == 3 )
				result[row][col] += m[3][col];
		}
	}
  
	return result;
}


Matrix3C&
Matrix3C::set_identity()
{
	for( int32 i = 0; i < 4; i++ )
		for( int32 j = 0; j < 3; j++ )
			if( i == j )
				m_rMat[i][j] = 1.0;
			else
				m_rMat[i][j] = 0.0;

	return *this;
}


Matrix3C&
Matrix3C::set_trans( const Vector3C& v )
{
	set_identity();
	m_rMat[3] = v;
	return *this;
}

Matrix3C&
Matrix3C::set_scale( const Vector3C& v )
{
	set_identity();
	m_rMat[0][0] = v[0];
	m_rMat[1][1] = v[1];
	m_rMat[2][2] = v[2];
	return *this;
}

Matrix3C&
Matrix3C::set_rot( const QuatC& rQ )
{
	float wx, wy, wz, xx, yy, yz, xy, xz, zz, x2, y2, z2;

	// calculate coefficients
	x2 = rQ[0] + rQ[0];
	y2 = rQ[1] + rQ[1]; 
	z2 = rQ[2] + rQ[2];

	xx = rQ[0] * x2;
	xy = rQ[0] * y2;
	xz = rQ[0] * z2;

	yy = rQ[1] * y2;
	yz = rQ[1] * z2;
	zz = rQ[2] * z2;

	wx = rQ[3] * x2;
	wy = rQ[3] * y2;
	wz = rQ[3] * z2;

	m_rMat[0][0] = 1.0 - (yy + zz);
	m_rMat[0][1] = xy - wz;
	m_rMat[0][2] = xz + wy;

	m_rMat[1][0] = xy + wz;
	m_rMat[1][1] = 1.0 - (xx + zz);
	m_rMat[1][2] = yz - wx;

	m_rMat[2][0] = xz - wy;
	m_rMat[2][1] = yz + wx;
	m_rMat[2][2] = 1.0 - (xx + yy);

	m_rMat[3][0] = 0;
	m_rMat[3][1] = 0;
	m_rMat[3][2] = 0;


	return *this;
}


Matrix3C
Matrix3C::inverse() const
{
	Matrix3C	rRes;

	// calc determinant
	float32	f32S = m_rMat[0][0] * (m_rMat[1][1] * m_rMat[2][2] - m_rMat[1][2] * m_rMat[2][1]) +
					m_rMat[0][1] * (m_rMat[1][2] * m_rMat[2][0] - m_rMat[1][0] * m_rMat[2][2]) +
					m_rMat[0][2] * (m_rMat[1][0] * m_rMat[2][1] - m_rMat[1][1] * m_rMat[2][0]);

	// matrix is singular... return identity
	if( !f32S ) {
		rRes.set_identity();
		return rRes;
	}

	f32S = 1.0f / f32S;

	// Invert the matrix
	rRes[0][0] = f32S * (m_rMat[1][1] * m_rMat[2][2] - m_rMat[1][2] * m_rMat[2][1]);
	rRes[0][1] = f32S * (m_rMat[0][2] * m_rMat[2][1] - m_rMat[0][1] * m_rMat[2][2]);
	rRes[0][2] = f32S * (m_rMat[0][1] * m_rMat[1][2] - m_rMat[0][2] * m_rMat[1][1]);
	rRes[1][0] = f32S * (m_rMat[1][2] * m_rMat[2][0] - m_rMat[1][0] * m_rMat[2][2]);
	rRes[1][1] = f32S * (m_rMat[0][0] * m_rMat[2][2] - m_rMat[0][2] * m_rMat[2][0]);
	rRes[1][2] = f32S * (m_rMat[0][2] * m_rMat[1][0] - m_rMat[0][0] * m_rMat[1][2]);
	rRes[2][0] = f32S * (m_rMat[1][0] * m_rMat[2][1] - m_rMat[1][1] * m_rMat[2][0]);
	rRes[2][1] = f32S * (m_rMat[0][1] * m_rMat[2][0] - m_rMat[0][0] * m_rMat[2][1]);
	rRes[2][2] = f32S * (m_rMat[0][0] * m_rMat[1][1] - m_rMat[0][1] * m_rMat[1][0]);
	rRes[3][0] = -m_rMat[3][0] * rRes[0][0] - m_rMat[3][1] * rRes[1][0] - m_rMat[3][2] * rRes[2][0]; 
	rRes[3][1] = -m_rMat[3][0] * rRes[0][1] - m_rMat[3][1] * rRes[1][1] - m_rMat[3][2] * rRes[2][1]; 
	rRes[3][2] = -m_rMat[3][0] * rRes[0][2] - m_rMat[3][1] * rRes[1][2] - m_rMat[3][2] * rRes[2][2]; 

	return rRes;	
}
