//-------------------------------------------------------------------------
//
// File:		QuatC.cpp
// Desc:		Quaternion class.
// Author:		memon <memon@inside.org>
//
//-------------------------------------------------------------------------
//	Copyright (c) 2002 Moppi Productions. All Rights Reserved.
//  This file is part of Eskimo Fishing Trip game. For conditions of 
//  distribution and use, see the accompanying license.txt file.
//  http://moppi.inside.org/
//-------------------------------------------------------------------------

#ifdef WIN32
#include <windows.h>
#endif
#include "PajaTypes.h"
#include "Vector3C.h"
#include "Matrix3C.h"
#include "QuatC.h"
#include <math.h>
#include <assert.h>

using namespace PajaTypes;

// constants
const float32	QUAT_EPSILON = 1.0e-6f;
const QuatC		QUAT_IDENTITY( 1, 0, 0, 0 );
const QuatC		QUAT_ZERO( 0, 0, 0, 0 );


QuatC::QuatC( float32 f32X, float32 f32Y, float32 f32Z, float32 f32W )
{
	m_f32Quat[0] = f32X;
	m_f32Quat[1] = f32Y;
	m_f32Quat[2] = f32Z;
	m_f32Quat[3] = f32W;
}

QuatC::QuatC( const QuatC& rQuat )
{
	m_f32Quat[0] = rQuat.m_f32Quat[0];
	m_f32Quat[1] = rQuat.m_f32Quat[1];
	m_f32Quat[2] = rQuat.m_f32Quat[2];
	m_f32Quat[3] = rQuat.m_f32Quat[3];
}

QuatC::~QuatC()
{
	// empty
}

void
QuatC::from_rot_matrix( const Matrix3C& rRot )
{
	// Algorithm in Ken Shoemake's article in 1987 SIGGRAPH course notes
	// article "Quaternion Calculus and Fast Animation".

	float32	f32Trace = rRot[0][0] + rRot[1][1] + rRot[2][2];
	float32	f32Root;

	if ( f32Trace > QUAT_EPSILON ) {
		// |w| > 1/2, may as well choose w > 1/2
		f32Root = (float32)sqrt( f32Trace + 1.0f );  // 2w
		m_f32Quat[3] = 0.5f * f32Root;
		f32Root = 0.5f / f32Root;  // 1/(4w)
		m_f32Quat[0] = (rRot[2][1] - rRot[1][2]) * f32Root;
		m_f32Quat[1] = (rRot[0][2] - rRot[2][0]) * f32Root;
		m_f32Quat[2] = (rRot[1][0] - rRot[0][1]) * f32Root;
	}
	else {
		// |w| <= 1/2
		static int32	s_i32Next[3] = { 1, 2, 0 };
		int32	i = 0;
		if( rRot[1][1] > rRot[0][0] )
			i = 1;
		if( rRot[2][2] > rRot[i][i] )
			i = 2;
		int32	j = s_i32Next[i];
		int32	k = s_i32Next[j];

		f32Root = (float32)sqrt( rRot[i][i] - rRot[j][j] - rRot[k][k] + 1.0f );
		m_f32Quat[i] = 0.5f * f32Root;
		f32Root = 0.5f / f32Root;
		m_f32Quat[3] = (rRot[k][j] - rRot[j][k]) * f32Root;
		m_f32Quat[j] = (rRot[j][i] + rRot[i][j]) * f32Root;
		m_f32Quat[k] = (rRot[k][i] + rRot[i][k]) * f32Root;
	}
}

Matrix3C
QuatC::to_rot_matrix() const
{
	Matrix3C rRot;

/*
	float32 f32Tx  = 2.0f * m_f32Quat[0];
	float32 f32Ty  = 2.0f * m_f32Quat[1];
	float32 f32Tz  = 2.0f * m_f32Quat[2];

	float32 f32Txx = f32Tx * m_f32Quat[0];
	float32 f32Txy = f32Ty * m_f32Quat[0];
	float32 f32Txz = f32Tz * m_f32Quat[0];

	float32 f32Tyy = f32Ty * m_f32Quat[1];
	float32 f32Tyz = f32Tz * m_f32Quat[1];
	float32 f32Tzz = f32Tz * m_f32Quat[2];

	float32 f32Twx = f32Tx * m_f32Quat[3];
	float32 f32Twy = f32Ty * m_f32Quat[3];
	float32 f32Twz = f32Tz * m_f32Quat[3];


	rRot[0][0] = 1.0f - (f32Tyy + f32Tzz);
	rRot[0][1] = f32Txy - f32Twz;
	rRot[0][2] = f32Txz + f32Twy;

	rRot[1][0] = f32Txy + f32Twz;
	rRot[1][1] = 1.0f - (f32Txx + f32Tzz);
	rRot[1][2] = f32Tyz - f32Twx;

	rRot[2][0] = f32Txz - f32Twy;
	rRot[2][1] = f32Tyz + f32Twx;
	rRot[2][2] = 1.0f - (f32Txx + f32Tyy);

	rRot[3][0] = 0;
	rRot[3][1] = 0;
	rRot[3][2] = 0;
*/

	float wx, wy, wz, xx, yy, yz, xy, xz, zz, x2, y2, z2;

	// calculate coefficients
	x2 = m_f32Quat[0] + m_f32Quat[0];
	y2 = m_f32Quat[1] + m_f32Quat[1]; 
	z2 = m_f32Quat[2] + m_f32Quat[2];

	xx = m_f32Quat[0] * x2;
	xy = m_f32Quat[0] * y2;
	xz = m_f32Quat[0] * z2;

	yy = m_f32Quat[1] * y2;
	yz = m_f32Quat[1] * z2;
	zz = m_f32Quat[2] * z2;

	wx = m_f32Quat[3] * x2;
	wy = m_f32Quat[3] * y2;
	wz = m_f32Quat[3] * z2;

	rRot[0][0] = 1.0 - (yy + zz);
	rRot[0][1] = xy - wz;
	rRot[0][2] = xz + wy;

	rRot[1][0] = xy + wz;
	rRot[1][1] = 1.0 - (xx + zz);
	rRot[1][2] = yz - wx;

	rRot[2][0] = xz - wy;
	rRot[2][1] = yz + wx;
	rRot[2][2] = 1.0 - (xx + yy);

	rRot[3][0] = 0;
	rRot[3][1] = 0;
	rRot[3][2] = 0;

	return rRot;
}

void
QuatC::from_axis_angle( const Vector3C& rVec, float32 f32Angle )
{
	from_axis_angle( rVec[0], rVec[1], rVec[2], f32Angle );
}

void
QuatC::from_axis_angle( float32 f32X, float32 f32Y, float32 f32Z, float32 f32Angle )
{
	float32	f32HalfAngle = 0.5f * f32Angle;
	float32	f32Sin = (float32)sin( f32HalfAngle );
	m_f32Quat[0] = f32Sin * f32X;
	m_f32Quat[1] = f32Sin * f32Y;
	m_f32Quat[2] = f32Sin * f32Z;
	m_f32Quat[3] = (float32)cos( f32HalfAngle );
}

void
QuatC::to_axis_angle( Vector3C& rVec, float32& f32Angle ) const
{
	float32	f32X, f32Y, f32Z;
	to_axis_angle( f32X, f32Y, f32Z, f32Angle );
	rVec[0] = f32X;
	rVec[1] = f32Y;
	rVec[2] = f32Z;
}

void
QuatC::to_axis_angle( float32& f32X, float32& f32Y, float32& f32Z, float32& f32Angle ) const
{
	QuatC	rQ;
	rQ = normalize();

	float32	f32Omega = (float32)acos( rQ[3] );
	f32Angle = f32Omega * 2.0f;
	float32	f32Sin = (float32)sin( f32Omega );

	if( fabs( f32Sin ) > QUAT_EPSILON ) {
		f32Sin = 1.0f / f32Sin;
		f32X = m_f32Quat[0] * f32Sin;
		f32Y = m_f32Quat[1] * f32Sin;
		f32Z = m_f32Quat[2] * f32Sin;
	}
	else {
		f32X = 1.0f;
		f32Y = 0.0f;
		f32Z = 0.0f;
	}
}

QuatC
QuatC::normalize() const
{
	float32	f32Len = (float32)sqrt( m_f32Quat[0] * m_f32Quat[0] + m_f32Quat[1] * m_f32Quat[1] + m_f32Quat[2] * m_f32Quat[2] + m_f32Quat[3] * m_f32Quat[3] );
	f32Len = 1.0f / f32Len;
	return QuatC( m_f32Quat[0] * f32Len, m_f32Quat[1] * f32Len, m_f32Quat[2] * f32Len, m_f32Quat[3] * f32Len );
}

QuatC
QuatC::operator*( const QuatC& rQuat ) const
{
	QuatC	rRes;

	rRes.m_f32Quat[0] = (m_f32Quat[3] * rQuat.m_f32Quat[0]) + (m_f32Quat[0] * rQuat.m_f32Quat[3]) + (m_f32Quat[1] * rQuat.m_f32Quat[2]) - (m_f32Quat[2] * rQuat.m_f32Quat[1]);
	rRes.m_f32Quat[1] = (m_f32Quat[3] * rQuat.m_f32Quat[1]) + (m_f32Quat[1] * rQuat.m_f32Quat[3]) + (m_f32Quat[2] * rQuat.m_f32Quat[0]) - (m_f32Quat[0] * rQuat.m_f32Quat[2]);
	rRes.m_f32Quat[2] = (m_f32Quat[3] * rQuat.m_f32Quat[2]) + (m_f32Quat[2] * rQuat.m_f32Quat[3]) + (m_f32Quat[0] * rQuat.m_f32Quat[1]) - (m_f32Quat[1] * rQuat.m_f32Quat[0]);
	rRes.m_f32Quat[3] = (m_f32Quat[3] * rQuat.m_f32Quat[3]) - (m_f32Quat[0] * rQuat.m_f32Quat[0]) - (m_f32Quat[1] * rQuat.m_f32Quat[1]) - (m_f32Quat[2] * rQuat.m_f32Quat[2]);

	return rRes;
}

QuatC
QuatC::inverse() const
{
	float32	f32D = m_f32Quat[0] * m_f32Quat[0] + m_f32Quat[1] * m_f32Quat[1] + m_f32Quat[2] * m_f32Quat[2] + m_f32Quat[3] * m_f32Quat[3];
	if( f32D == 0.0f )
		f32D = 1;
	else
		f32D = 1.0f / f32D;

	return QuatC( -m_f32Quat[0] * f32D, -m_f32Quat[1] * f32D, -m_f32Quat[2] * f32D, m_f32Quat[3] * f32D );
}


