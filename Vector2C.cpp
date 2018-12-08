//-------------------------------------------------------------------------
//
// File:		Vector2C.cpp
// Desc:		2D vector class.
// Author:		memon <memon@inside.org>
//
//-------------------------------------------------------------------------
//	Copyright (c) 2002 Moppi Productions. All Rights Reserved.
//  This file is part of Eskimo Fishing Trip game. For conditions of 
//  distribution and use, see the accompanying license.txt file.
//  http://moppi.inside.org/
//-------------------------------------------------------------------------

#include "Vector2C.h"
#include "PajaTypes.h"
#include <math.h>


using namespace PajaTypes;


Vector2C::Vector2C( float32 x, float32 y )
{
	m_vec[0] = x;
	m_vec[1] = y;
}

Vector2C::Vector2C( const Vector2C& a )
{
	m_vec[0] = a.m_vec[0];
	m_vec[1] = a.m_vec[1];
}

Vector2C::Vector2C( const float32 a[2] )
{
	m_vec[0] = a[0];
	m_vec[1] = a[1];
}

Vector2C::~Vector2C()
{
	// empty
}

Vector2C
Vector2C::normalize() const
{
	Vector2C	rRes( *this );

	float32  f32D = (m_vec[0] * m_vec[0]) + (m_vec[1] * m_vec[1]);
	if( f32D != 0.0 ) {
		float32	f32OOD = 1.0f / (float32)sqrt( f32D );
		rRes.m_vec[0] *= f32OOD;
		rRes.m_vec[1] *= f32OOD;
	}

	return rRes;
}

bool
Vector2C::operator==( const Vector2C& a ) const
{
	return (m_vec[0] == a[0] && m_vec[1] == a[1]);
}

bool
Vector2C::operator!=( const Vector2C& a ) const
{
	return (m_vec[0] != a[0] || m_vec[1] != a[1]);
}

