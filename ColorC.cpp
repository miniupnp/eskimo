//-------------------------------------------------------------------------
//
// File:		ColorC.cpp
// Desc:		RGBA color class.
// Author:		memon <memon@inside.org>
//
//-------------------------------------------------------------------------
//	Copyright (c) 2002 Moppi Productions. All Rights Reserved.
//  This file is part of Eskimo Fishing Trip game. For conditions of 
//  distribution and use, see the accompanying license.txt file.
//  http://moppi.inside.org/
//-------------------------------------------------------------------------

#include "PajaTypes.h"
#include "ColorC.h"


using namespace PajaTypes;


ColorC::ColorC( const ColorC& col )
{
	m_col[0] = col.m_col[0];
	m_col[1] = col.m_col[1];
	m_col[2] = col.m_col[2];
	m_col[3] = col.m_col[3];
}

ColorC::ColorC( float32 r, float32 g, float32 b, float32 a )
{
	m_col[0] = r;
	m_col[1] = g;
	m_col[2] = b;
	m_col[3] = a;
}

ColorC::~ColorC()
{
	// empty
}

void
ColorC::convert_from_uint8( uint8 r, uint8 g, uint8 b, uint8 a )
{
	m_col[0] = (float)r / 255.0f;
	m_col[1] = (float)g / 255.0f;
	m_col[2] = (float)b / 255.0f;
	m_col[3] = (float)a / 255.0f;
}

bool
ColorC::operator==( const ColorC& a ) const
{
	return (m_col[0] == a.m_col[0] && m_col[1] == a.m_col[1] && m_col[2] == a.m_col[2] && m_col[3] == a.m_col[3]);
}

bool
ColorC::operator!=( const ColorC& a ) const
{
	return (m_col[0] != a.m_col[0] || m_col[1] != a.m_col[1] || m_col[2] != a.m_col[2] || m_col[3] != a.m_col[3]);
}
