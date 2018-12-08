//-------------------------------------------------------------------------
//
// File:		FishingLineC.cpp
// Desc:		Fishing line class (see also FishingRodC).
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
#include <GL/gl.h>

#include "PajaTypes.h"
#include "Vector3C.h"
#include "FishingContextC.h"
#include "SplineCurveC.h"
#include "FishingLineC.h"
#include "FishingPondC.h"
#include "FishC.h"

using namespace PajaTypes;

FishingLineC::FishingLineC() :
	m_f32BaitMass( 1.0f ),
	m_ui32BaitIndex( 0 ),
	m_ui32BaitState( BAITSTATE_FREE ),
	m_pBaitScene( 0 ),
	m_pSpline( 0 ),
	m_i32BaitCount( 0 ),
	m_i32BaitFreshness( 0 ),
	m_f32ReloadDist( 0 )
{
	// empty
}

FishingLineC::~FishingLineC()
{
	delete m_pSpline;
	delete m_pBaitScene;
}

void
FishingLineC::set_bait_mesh( const char* szName )
{
	delete m_pBaitScene;
	m_pBaitScene = new SceneC;
	if( !m_pBaitScene->load( szName ) ) {
		delete m_pBaitScene;
		m_pBaitScene = 0;
	}
}

void
FishingLineC::set_bait_mass( float32 f32Mass )
{
	m_f32BaitMass = f32Mass;

	if( m_ui32BaitIndex ) {
		m_rParticles[m_ui32BaitIndex].m_f32Mass = f32Mass;
	}
}

void
FishingLineC::set_tip_pos( const Vector3C& rPos )
{
	m_rTipPos = rPos;

	if( m_rParticles.size() ) {
		m_rParticles[0].m_rPos = rPos;
	}
}

void
FishingLineC::set_bait_pos( const Vector3C& rPos )
{
	m_rBaitPos = rPos;

	if( m_ui32BaitIndex ) {
		m_rParticles[m_ui32BaitIndex].m_rPos = rPos;
	}
}

const Vector3C&
FishingLineC::get_tip_pos() const
{
	return m_rTipPos;
}


void
FishingLineC::set_bait_state( uint32 ui32State )
{
	m_ui32BaitState = ui32State;
}

uint32
FishingLineC::get_bait_state() const
{
	return m_ui32BaitState;
}

void
FishingLineC::set_extra_bait_force( const Vector3C& rF )
{
	m_rExtraBaitForce = rF;
}

const Vector3C&
FishingLineC::get_bait_pos() const
{
	return m_rBaitPos;
}

uint32
FishingLineC::get_particle_count() const
{
	return m_rParticles.size();
}

const Vector3C&
FishingLineC::get_particle_pos( uint32 ui32Idx ) const
{
	return m_rParticles[ui32Idx].m_rPos;
}

uint32
FishingLineC::get_fish_index() const
{
	return m_ui32FishIndex;
}

void
FishingLineC::init( FishingContextC* pContext )
{
	const uint32 ui32NumPart = 6;

	Vector3C	rDelta = (m_rBaitPos - m_rTipPos) / (float32)(ui32NumPart - 1);
	Vector3C	rPos = m_rTipPos;
	m_f32PartInitDist = rDelta.length();

	const float32	f32Mass = 0.1f;			// kg

	for( uint32 i = 0; i < ui32NumPart; i++ ) {
		ParticleS	rPart;
		rPart.m_rPos = rPos;
		rPart.m_f32Mass = i == (ui32NumPart - 1) ? m_f32BaitMass : f32Mass;
		m_rParticles.push_back( rPart );
		rPos += rDelta;
	}

	m_ui32BaitIndex = m_rParticles.size() - 1;

	delete m_pSpline;
	m_pSpline = new SplineCurveC( ui32NumPart );
}

void
FishingLineC::update( FishingContextC* pContext, float32 f32DeltaTime )
{
	// collision detection and simulation

	const float32	f32Damping = 0.90f;		// overall
	const float32	f32K = 10.0f;			// stiffness
	Vector3C		rGravity( 0, -3.3f, 0 );	// Grrravity

	int32	i, j;
	int32	i32SpringCount = (int32)m_rParticles.size() - 1;

	// Zero all forces
	for( i = 0; i < m_rParticles.size(); i++ )
		m_rParticles[i].m_rF = Vector3C( 0, 0, 0 );

	// Springs connecting the particles
	for( i = 0; i < i32SpringCount; i++ ) {

		Vector3C	rForce, rAB;
		float32		f32R, f32F;

		rAB = m_rParticles[i + 1].m_rPos - m_rParticles[i].m_rPos;
		f32R = rAB.length();
		f32F = f32K * (f32R - m_f32PartInitDist);
		if( f32F > 100 )
			f32F = 100.0f;
		rForce = (rAB.normalize()) * f32F;

		m_rParticles[i].m_rF += rForce;
		m_rParticles[i + 1].m_rF -= rForce;
	}


	// Gravity
	for( i = 0; i < m_rParticles.size(); i++ ) {
		Vector3C	rG = rGravity * m_rParticles[i].m_f32Mass;
		m_rParticles[i].m_rF += rG;
	}

	//
	// Collision detection
	//
	// do different collision detection for the bait and the rest of the particles
	//

	const float32	f32PartRad = 3.0f;
	Vector3C		rDistVec;

	FishingPondC*	pPond = pContext->get_pond();
	Vector3C		rForce;

	// Rest of the particles...
	for( i = 0; i < (m_rParticles.size() - 1); i++ ) {
		// Pond
		pPond->hit_test( m_rParticles[i].m_rPos, f32PartRad, rForce );
		m_rParticles[i].m_rF += rForce * m_rParticles[i].m_f32Mass;
		// Fish
		for( j = 0; j < pPond->get_fish_count(); j++ ) {
			FishC*	pFish = pPond->get_fish( j );
			if( pFish->get_state() == FISHSTATE_EATEN )
				continue;
			pFish->hit_test( m_rParticles[i].m_rPos, f32PartRad, FISHHITFLAGS_SKIN, rForce );
			m_rParticles[i].m_rF += rForce * m_rParticles[i].m_f32Mass;
		}
	}

	// The bate
	if( m_ui32BaitState == BAITSTATE_INSIDE_FISH ) {

		FishC*		pFish = pPond->get_fish( m_ui32FishIndex );
		Vector3C	rFishPos = pFish->get_pos();

		// Follow the fish position on pond
		m_rParticles[m_ui32BaitIndex].m_rPos[0] = rFishPos[0];
		m_rParticles[m_ui32BaitIndex].m_rPos[2] = rFishPos[2];

		// If the fish has got underwater, spit out the bait.
		if( pFish->get_mouth_angle() < 0.00001f ) {

			// set fish state (and animation)
			pFish->set_state( FISHSTATE_MISSED );

			pFish->set_draw_state( FISHDRAWSTATE_NORMAL );

			// Spit out
			m_rParticles[m_ui32BaitIndex].m_rF += Vector3C( 0, 15, 0 ) * m_rParticles[m_ui32BaitIndex].m_f32Mass;

			// Release bait
			m_ui32BaitState = BAITSTATE_FREE;
			m_ui32FishIndex = 0;

//			TRACE( "Fish missed (too late): mouth angle: %f\n", rFishes[m_ui32FishIndex].m_f32MouthAngle );

			// you loose one bait
			add_bait_damage( BAIT_MAX_FRESHNESS );

			pContext->play_sound( SOUND_FISH_MISSED );
		}
		else {
			// the fish will only follow, if it has eaten the bait.
			if( pFish->get_mouth_angle() < 0.25 ) {
				pFish->set_state( FISHSTATE_FOLLOWBAIT );
				pFish->set_follow_height( m_rParticles[m_ui32BaitIndex].m_rPos[1] );
			}

			// If the fisherman draws the rod back too early, he misses the fish.
			if( m_rParticles[m_ui32BaitIndex].m_rPos[1] > (pFish->get_radius() + rFishPos[1]) ) {

				// set fish state (and animation)
				pFish->set_state( FISHSTATE_MISSED );

				pFish->set_draw_state( FISHDRAWSTATE_NORMAL );

				// Spit out
				m_rParticles[m_ui32BaitIndex].m_rF += Vector3C( 0, 15, 0 ) * m_rParticles[m_ui32BaitIndex].m_f32Mass;

				// Release bait
				m_ui32BaitState = BAITSTATE_FREE;
				m_ui32FishIndex = 0;

				pContext->play_sound( SOUND_FISH_MISSED );

				// you loose one bait
				add_bait_damage( BAIT_MAX_FRESHNESS );

//				TRACE( "Fish missed (too early): mouth angle: %f\n", rFishes[m_ui32FishIndex].m_f32MouthAngle );
			}

			if( m_rParticles[m_ui32BaitIndex].m_rPos[1] > pContext->get_catch_height() ) {

//				TRACE( "Fish catch: mouth angle: %f\n", rFishes[m_ui32FishIndex].m_f32MouthAngle );

				// set fish state (and animation)
				pFish->set_state( FISHSTATE_FLYAWAY );

				pFish->set_draw_state( FISHDRAWSTATE_NORMAL );

				// Spit out
				m_rParticles[m_ui32BaitIndex].m_rF += Vector3C( 0, 15, 0 ) * m_rParticles[m_ui32BaitIndex].m_f32Mass;

				// Release bait
				m_ui32BaitState = BAITSTATE_FREE;
				m_ui32FishIndex = 0;

				pContext->fish_catch( m_ui32FishIndex );
				pContext->play_sound( SOUND_FISH_CATCH );

				// add score
				pContext->add_score( m_i32BaitFreshness );

				// you loose one bait
				add_bait_damage( BAIT_MAX_FRESHNESS );
			}
		}
	}
	else {

		uint32 ui32Hit;

		// add extra bate force
		m_rParticles[m_ui32BaitIndex].m_rF += m_rExtraBaitForce;
		m_rExtraBaitForce = Vector3C( 0, 0, 0 );

		// hit test the pond
		ui32Hit = pPond->hit_test( m_rParticles[m_ui32BaitIndex].m_rPos, f32PartRad, rForce );
		m_rParticles[m_ui32BaitIndex].m_rF += rForce * m_rParticles[m_ui32BaitIndex].m_f32Mass;

		if( ui32Hit == PONDHIT_WATER ) {
			// Hit water
			if( m_ui32BaitState == BAITSTATE_FREE || m_ui32BaitState == BAITSTATE_ON_LAND ) {
				pContext->play_sound( SOUND_BAIT_HIT_WATER );
			}

			if( m_ui32BaitState == BAITSTATE_ON_LAND || m_ui32BaitState == BAITSTATE_FREE ) {
				add_bait_damage( BAIT_WATER_DAMAGE );
			}

			m_ui32BaitState = BAITSTATE_ON_WATER;
		}
		else if( ui32Hit == PONDHIT_NONE ) {
			// no hit
			if( m_ui32BaitState == BAITSTATE_ON_WATER ) {
				pContext->play_sound( SOUND_BAIT_LEFT_WATER );
			}
			m_ui32BaitState = BAITSTATE_FREE;
		}
		else if( ui32Hit == PONDHIT_LAND ) {
			// hit land
			if( m_ui32BaitState == BAITSTATE_ON_WATER ) {
				pContext->play_sound( SOUND_BAIT_LEFT_WATER );
			}

			if( m_ui32BaitState == BAITSTATE_ON_WATER || m_ui32BaitState == BAITSTATE_FREE ) {
				add_bait_damage( BAIT_LAND_DAMAGE );
			}

			m_ui32BaitState = BAITSTATE_ON_LAND;
		}

		// Fish
		for( j = 0; j < pPond->get_fish_count(); j++ ) {
			FishC*	pFish = pPond->get_fish( j );

			if( pFish->get_state() == FISHSTATE_EATEN )
				continue;

			if( m_i32BaitFreshness > 0 )
				ui32Hit = pFish->hit_test( m_rParticles[m_ui32BaitIndex].m_rPos, f32PartRad, FISHHITFLAGS_SKIN_AND_MOUTH, rForce );
			else
				ui32Hit = pFish->hit_test( m_rParticles[m_ui32BaitIndex].m_rPos, f32PartRad, FISHHITFLAGS_SKIN, rForce );

			if( ui32Hit == FISHHIT_MOUTH ) {
				m_ui32BaitState = BAITSTATE_INSIDE_FISH;
				m_rParticles[m_ui32BaitIndex].m_rPos = pFish->get_pos();
				m_rParticles[m_ui32BaitIndex].m_rF = Vector3C( 0, 0, 0 );
				m_ui32FishIndex = j;
				pFish->set_draw_state( FISHDRAWSTATE_GLOW );
			}
			else {
				m_rParticles[m_ui32BaitIndex].m_rF += rForce * m_rParticles[m_ui32BaitIndex].m_f32Mass;
			}
		}
	}


	// Integration
	// The first particle is stationary, so dont handle it.
	for( i = 1; i < m_rParticles.size(); i++ ) {
		m_rParticles[i].m_rAcc = m_rParticles[i].m_rF * (1.0f / m_rParticles[i].m_f32Mass );
		m_rParticles[i].m_rVel += m_rParticles[i].m_rAcc * f32DeltaTime;
		m_rParticles[i].m_rPos += m_rParticles[i].m_rVel * f32DeltaTime;
		m_rParticles[i].m_rVel *= f32Damping;
	}

	m_rBaitPos = m_rParticles[m_ui32BaitIndex].m_rPos;

	// check if we can reload
	if( (m_rBaitPos[2] > m_f32ReloadDist) && (m_i32BaitFreshness < 1) ) {		// z goes negative forward
		if( m_i32BaitCount > 0 ) {
			m_i32BaitCount--;
			m_i32BaitFreshness = BAIT_MAX_FRESHNESS;
		}
	}
}

void
FishingLineC::draw( float32 f32RotY, FishingContextC* pContext )
{
	// Update fishing line points to spline and calc spline tangents
	for( int32 i = 0; i < m_rParticles.size(); i++ )
		m_pSpline->set_point( i, m_rParticles[i].m_rPos );
	m_pSpline->prepare();


	glDisable( GL_LIGHTING );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	// draw fishing line
	glBegin( GL_LINE_STRIP );

//		if( m_ui32BaitState == BAITSTATE_INSIDE_FISH )
//			glColor4ub( 255, 64, 16, 240 );
//		else
			glColor4ub( 128, 168, 255, 128 );
	
		float	f32T = 0;
		float	f32DT = 1.0f / (float32)((m_pSpline->get_point_count() * 8));
		for(int i = 0; i < (m_pSpline->get_point_count() * 8); i++ ) {
			Vector3C	rPos = m_pSpline->get_value( f32T );
			glVertex3f( rPos[0], rPos[1], rPos[2] );
			f32T += f32DT;
		}
	glEnd();

	glBlendFunc( GL_ONE, GL_ZERO );
	glEnable( GL_LIGHTING );


	// draw the bait
	if( m_i32BaitFreshness > 0 ) {
		if( m_pBaitScene ) {
			glPushMatrix();

			glTranslatef( m_rBaitPos[0], m_rBaitPos[1], m_rBaitPos[2] );
			glRotatef( f32RotY / M_PI * 180.0f, 0, 1, 0 );

			m_pBaitScene->draw();

			glPopMatrix();
		}
	}
}

void
FishingLineC::init_baits( int32 i32Count )
{
	m_i32BaitCount = i32Count;
	m_i32BaitFreshness = BAIT_MAX_FRESHNESS;
}

int32
FishingLineC::get_bait_count() const
{
	return m_i32BaitCount;
}

int32
FishingLineC::get_bait_freshness() const
{
	return m_i32BaitFreshness;
}

void
FishingLineC::add_bait_damage( int32 i32Count )
{
	m_i32BaitFreshness -= i32Count;
	if( m_i32BaitFreshness < 1 ) {
		m_i32BaitFreshness = 0;
	}
}

void
FishingLineC::set_reload_distance( float32 f32Dist )
{
	m_f32ReloadDist = f32Dist;
}
