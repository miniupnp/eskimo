//-------------------------------------------------------------------------
//
// File:		LightC.cpp
// Desc:		light class.
// Author:		memon <memon@inside.org>
//
//-------------------------------------------------------------------------
//	Copyright (c) 2002 Moppi Productions. All Rights Reserved.
//  This file is part of Eskimo Fishing Trip game. For conditions of 
//  distribution and use, see the accompanying license.txt file.
//  http://moppi.inside.org/
//-------------------------------------------------------------------------

#include "PajaTypes.h"
#include "Vector3C.h"
#include "ColorC.h"
#include "LightC.h"

using namespace PajaTypes;


LightC::LightC() :
	m_rColor( 1, 1, 1 ),
	m_f32Multiplier( 1 ),
	m_bAttenuate( false ),
	m_f32Decay( 0 )
{
	// empty
}

LightC::~LightC()
{
	// empty
}

int32
LightC::get_type()
{
	return CGITEM_LIGHT;
}

void
LightC::set_position( const Vector3C& rPos )
{
	m_rPos = rPos;
}

const
Vector3C&
LightC::get_position()
{
	return m_rPos;
}

void
LightC::set_color( const ColorC& rVal )
{
	m_rColor = rVal;
}

const
ColorC&
LightC::get_color()
{
	return m_rColor;
}

void
LightC::set_multiplier( float32 f32Val )
{
	m_f32Multiplier = f32Val;
}

float32
LightC::get_multiplier()
{
	return m_f32Multiplier;
}

void
LightC::eval_state( int32 i32Time )
{
}


void
LightC::set_attenuate( bool bState )
{
	m_bAttenuate = bState;
}

bool
LightC::get_attenuate() const
{
	return m_bAttenuate;
}

void
LightC::set_decay_start( float32 f32Val )
{
	m_f32Decay = f32Val;
}

float32
LightC::get_decay_start() const
{
	return m_f32Decay;
}
