//-------------------------------------------------------------------------
//
// File:		Vector3C.cpp
// Desc:		3D vector class.
// Author:		memon <memon@inside.org>
//
//-------------------------------------------------------------------------
//	Copyright (c) 2002 Moppi Productions. All Rights Reserved.
//  This file is part of Eskimo Fishing Trip game. For conditions of 
//  distribution and use, see the accompanying license.txt file.
//  http://moppi.inside.org/
//-------------------------------------------------------------------------

#include "vector3c.h"
#include <math.h>


using namespace PajaTypes;


Vector3C::Vector3C( float32 x, float32 y, float32 z )
{
	m_vec[0] = x;
	m_vec[1] = y;
	m_vec[2] = z;
}

Vector3C::Vector3C( const Vector3C& a )
{
	m_vec[0] = a.m_vec[0];
	m_vec[1] = a.m_vec[1];
	m_vec[2] = a.m_vec[2];
}

Vector3C::Vector3C( float32 a[3] )
{
	m_vec[0] = a[0];
	m_vec[1] = a[1];
	m_vec[2] = a[2];
}

Vector3C::~Vector3C()
{
	// empty
}

Vector3C
Vector3C::normalize() const
{
	Vector3C	rRes( *this );

	float32  d = (m_vec[0] * m_vec[0]) + (m_vec[1] * m_vec[1]) + (m_vec[2] * m_vec[2]);
	if( d != 0.0 ) {
		float32	ood;
		ood = 1.0f / (float32)sqrt( d );
		rRes.m_vec[0] *= ood;
		rRes.m_vec[1] *= ood;
		rRes.m_vec[2] *= ood;
	}

	return rRes;
}

bool
Vector3C::operator==( const Vector3C& a ) const
{
	return (m_vec[0] == a[0] && m_vec[1] == a[1] && m_vec[2] == a[2]);
}

bool
Vector3C::operator!=( const Vector3C& a ) const
{
	return (m_vec[0] != a[0] || m_vec[1] != a[1] || m_vec[2] != a[2]);
}

Vector3C
Vector3C::cross( const Vector3C& b ) const
{
	return Vector3C( m_vec[1] * b.m_vec[2] - m_vec[2] * b.m_vec[1],
					 m_vec[2] * b.m_vec[0] - m_vec[0] * b.m_vec[2],
					 m_vec[0] * b.m_vec[1] - m_vec[1] * b.m_vec[0] );
}
