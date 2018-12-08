//-------------------------------------------------------------------------
//
// File:		FishingRodC.cpp
// Desc:		Fishing rod class (see also: FishingLineC).
// Author:		memon <memon@inside.org>
//
//-------------------------------------------------------------------------
//	Copyright (c) 2002 Moppi Productions. All Rights Reserved.
//  This file is part of Eskimo Fishing Trip game. For conditions of 
//  distribution and use, see the accompanying license.txt file.
//  http://moppi.inside.org/
//-------------------------------------------------------------------------

#include <windows.h>
#include <gl/gl.h>
#include "PajaTypes.h"
#include "Vector3C.h"
#include "FishingLineC.h"
#include "SceneC.h"
#include "FishingRodC.h"
#include <math.h>

using namespace PajaTypes;


FishingRodC::FishingRodC() :
	m_f32AngleX( 0 ),
	m_f32AngleY( 0 ),
	m_pRodScene( 0 ),
	m_f32ExtraBaitForce( 0 )
{
	m_pLine = new FishingLineC;
}

FishingRodC::~FishingRodC()
{
	delete m_pLine;
	delete m_pRodScene;
}

void
FishingRodC::set_pos( const Vector3C& rPos )
{
	m_rPos = rPos;
}

void
FishingRodC::set_angle_x( float32 f32Angle )
{
	m_f32AngleX = f32Angle;
}

void
FishingRodC::set_angle_y( float32 f32Angle )
{
	m_f32AngleY = f32Angle;
}

void
FishingRodC::set_length( float32 f32Length )
{
	m_f32Length = f32Length;
}

void
FishingRodC::set_line_length( float32 f32Length )
{
	m_f32LineLength = f32Length;
}

void
FishingRodC::set_bait_force( float32 f32Force )
{
	m_f32ExtraBaitForce = f32Force;
}

void
FishingRodC::set_rod_mesh( const char* szName )
{
	delete m_pRodScene;
	m_pRodScene = new SceneC;
	if( !m_pRodScene->load( szName ) ) {
		delete m_pRodScene;
		m_pRodScene = 0;
	}
}

void
FishingRodC::set_bait_mesh( const char* szName )
{
	m_pLine->set_bait_mesh( szName );
}

const Vector3C&
FishingRodC::get_bait_pos() const
{
	return m_pLine->get_bait_pos();
}

const Vector3C&
FishingRodC::get_tip_pos() const
{
	return m_pLine->get_tip_pos();
}

uint32
FishingRodC::get_particle_count() const
{
	return m_pLine->get_particle_count();
}

const Vector3C&
FishingRodC::get_particle_pos( uint32 ui32Idx ) const
{
	return m_pLine->get_particle_pos( ui32Idx );
}

const Vector3C&
FishingRodC::get_dir() const
{
	return m_rDir;
}

uint32
FishingRodC::get_bait_state() const
{
	return m_pLine->get_bait_state();
}

uint32
FishingRodC::get_fish_index() const
{
	return m_pLine->get_fish_index();
}
	
void
FishingRodC::init( FishingContextC* pContext )
{
	Vector3C	rAxisZ, rAxisY( 0, 1, 0 );

	// Well... i dont have any good mathematical proof for all this sin/cos twidling (the order and signs),
	// but this way it seems to work with opengl. RTFM I guess :)
	rAxisZ[0] = -sin( m_f32AngleY );
	rAxisZ[1] = 0.0f;
	rAxisZ[2] = -cos( m_f32AngleY );

	m_rDir = (rAxisZ * cos( m_f32AngleX )) + (rAxisY * sin( m_f32AngleX ));

	m_rDir = m_rDir.normalize();
	m_rDir *= m_f32Length;
	m_rTipPos = m_rPos + m_rDir;
	
	m_pLine->set_tip_pos( m_rTipPos );
	m_pLine->set_bait_pos( m_rTipPos + Vector3C( 0, -m_f32LineLength, 0 ) );
	m_pLine->set_bait_mass( 5.0f );
	m_pLine->init( pContext );

}

void
FishingRodC::update( FishingContextC* pContext )
{
	Vector3C	rAxisZ, rAxisY( 0, 1, 0 );

/*	rAxisZ[0] = cos( m_f32AngleY );
	rAxisZ[1] = 0.0f;
	rAxisZ[2] = sin( m_f32AngleY );
	rDir = (rAxisZ * cos( m_f32AngleX )) + (rAxisY * sin( m_f32AngleX ));
*/

	// Well... i dont have any good mathematical proof for all this sin/cos twidling (the order and signs),
	// but this way it seems to work with opengl. RTFM I guess :)
	rAxisZ[0] = -sin( m_f32AngleY );
	rAxisZ[1] = 0.0f;
	rAxisZ[2] = -cos( m_f32AngleY );

	m_rDir = (rAxisZ * cos( m_f32AngleX )) + (rAxisY * sin( m_f32AngleX ));

	m_rDir = m_rDir.normalize();
	m_rDir *= m_f32Length;
	m_rTipPos = m_rPos + m_rDir;
	
	m_pLine->set_tip_pos( m_rTipPos );

	Vector3C	rF;
	if( m_f32ExtraBaitForce < 0 ) {
		rF = m_rDir;
		rF[1] = -rF[1];
		rF *= m_f32ExtraBaitForce * 0.1f;
	}
	else {
		rF = m_rDir;
		rF *= m_f32ExtraBaitForce * 0.1f;
	}
	for( int32 i = 0; i < 10; i++ ) {
		m_pLine->set_extra_bait_force( rF );
		m_pLine->update( pContext, 0.1f );
	}
}

void
FishingRodC::draw( FishingContextC* pContext )
{
	if( m_pRodScene ) {
		glPushMatrix();

		glTranslatef( m_rPos[0], m_rPos[1], m_rPos[2] );

		glRotatef( m_f32AngleY / M_PI * 180.0f, 0, 1, 0 );
		glRotatef( m_f32AngleX / M_PI * 180.0f, 1, 0, 0 );

		m_pRodScene->draw();

		glPopMatrix();
	}

	m_pLine->draw( m_f32AngleY, pContext );
}


void
FishingRodC::init_baits( int32 i32Count )
{
	m_pLine->init_baits( i32Count );
}

int32
FishingRodC::get_bait_count() const
{
	return m_pLine->get_bait_count();
}

int32
FishingRodC::get_bait_freshness() const
{
	return m_pLine->get_bait_freshness();
}

void
FishingRodC::set_reload_distance( float32 f32Dist )
{
	m_pLine->set_reload_distance( f32Dist );
}
