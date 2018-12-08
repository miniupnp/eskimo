//-------------------------------------------------------------------------
//
// File:		CameraC.cpp
// Desc:		Camera class.
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
#include "CameraC.h"

using namespace PajaTypes;


CameraC::CameraC() :
	m_rPos( 0, 0, 0 ),
	m_rTgt( 0, 0, 1 ),
	m_f32FOV( 60 / 180 * M_PI  ),
	m_f32NearPlane( 0 ),
	m_f32FarPlane( 1 )
{
	// empty
}

CameraC::~CameraC()
{
	// empty
}

int32
CameraC::get_type()
{
	return CGITEM_CAMERA;
}

void
CameraC::set_position( const Vector3C& rPos )
{
	m_rPos = rPos;
}

const
Vector3C&
CameraC::get_position()
{
	return m_rPos;
}

void
CameraC::set_target_position( const Vector3C& rPos )
{
	m_rTgt = rPos;
}

const
Vector3C&
CameraC::get_target_position()
{
	return m_rTgt;
}

void
CameraC::set_fov( const float32 f32Val )
{
	m_f32FOV = f32Val;
}

const
float32
CameraC::get_fov()
{
	return m_f32FOV;
}

void
CameraC::set_near_plane( const float32 f32Val )
{
	m_f32NearPlane = f32Val;
}

const
float32
CameraC::get_near_plane()
{
	return m_f32NearPlane;
}

void
CameraC::set_far_plane( const float32 f32Val )
{
	m_f32FarPlane = f32Val;
}

const
float32
CameraC::get_far_plane()
{
	return m_f32FarPlane;
}

void
CameraC::eval_state( int32 i32Time )
{
}
