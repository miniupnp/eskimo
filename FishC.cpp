//-------------------------------------------------------------------------
//
// File:		FishC.cpp
// Desc:		Fish class.
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
#include "SplineCurveC.h"
#include "FishingContextC.h"
#include "FishC.h"
#include <math.h>


using namespace PajaTypes;


FishC::FishC() :
	m_i32AnimCount( 0 ),
	m_f32MouthPlaneLeftDist( 0 ),
	m_f32MouthPlaneRightDist( 0 ),
	m_f32Radius( 0 ),
	m_f32OrigAngle( 0 ),
	m_f32Distance( 0 ),
	m_f32Freq( 0 ),
	m_ui32State( FISHSTATE_FREE ),
	m_ui32DrawState( FISHDRAWSTATE_NORMAL ),
	m_pSpline( 0 ),
	m_ui32FishMesh( 0 )
{
	// empty
}

FishC::~FishC()
{
	delete m_pSpline;
}

void
FishC::init( float32 f32Radius, float32 f32Angle, float32 f32Distance, float32 f32Freq, uint32 ui32FishMesh )
{
	m_f32Radius = f32Radius;
	m_f32OrigAngle = f32Angle;
	m_f32Distance = f32Distance;
	m_f32Freq = f32Freq;
	m_ui32FishMesh = ui32FishMesh;
}

void
FishC::update( FishingContextC* pContext )
{
	if( m_ui32State == FISHSTATE_EATEN )
		return;

	float32	f32Frame = pContext->get_frame();

	float32	f32B = sin( f32Frame * m_f32Freq ) * 0.5f + 0.5f;

	m_f32MouthAngle = f32B * f32B;
	m_f32RotAngle = m_f32OrigAngle + f32Frame;

	m_rPos[0] = cos( m_f32RotAngle ) * m_f32Distance;
	m_rPos[1] = -18.0f + f32B * 30.0f;
	m_rPos[2] = sin( m_f32RotAngle ) * m_f32Distance;

	if( m_ui32State == FISHSTATE_FOLLOWBAIT )
		m_rPos[1] = m_f32FollowHeight;

	if( m_ui32State == FISHSTATE_MISSED ) {
		float32	f32Alpha = (float32)m_i32AnimCount / (float32)FISHANIM_MISSED_COUNT;

		m_rPos = (f32Alpha * m_rAnimPos) + ((1.0f - f32Alpha ) * m_rPos);

		m_i32AnimCount--;
		if( m_i32AnimCount < 1 )
			m_ui32State = FISHSTATE_FREE;
	}
	else if( m_ui32State == FISHSTATE_FLYAWAY ) {
		float32	f32Alpha = 1.0f - (float32)m_i32AnimCount / (float32)FISHANIM_FLYAWAY_COUNT;

		m_rPos = m_pSpline->get_value( f32Alpha );
		m_rTgtPos = m_pSpline->get_value( f32Alpha + (1.0f / (float32)FISHANIM_FLYAWAY_COUNT) );

		// rotate to normal in 1/4th of the frames
		m_f32RotAngle = fmod( m_f32RotAngle, M_PI * 2.0f );

		if( (FISHANIM_FLYAWAY_COUNT - m_i32AnimCount) < FISHANIM_FLYAWAY_COUNT / 4 )
			m_f32RotAngle *= 1.0f - (float32)(FISHANIM_FLYAWAY_COUNT - m_i32AnimCount) / (float32)(FISHANIM_FLYAWAY_COUNT / 4);
		else
			m_f32RotAngle = 0;

		m_i32AnimCount--;
		if( m_i32AnimCount < 1 ) {
			delete m_pSpline;
			m_pSpline = 0;
			m_ui32State = FISHSTATE_EATEN;
		}
	}
	
	// Calc fish mouth collision planes
	Vector3C	rDirY( 0, 1, 0 );
	Vector3C	rDirZ;

	rDirZ[0] = sin( m_f32RotAngle );
	rDirZ[1] = 0;
	rDirZ[2] = -cos( m_f32RotAngle );

	m_rMouthPlaneLeftNorm = sin( (m_f32MouthAngle * (60.0f / 180.0f) * M_PI) + M_PI / 2 ) * rDirZ;
	m_rMouthPlaneLeftNorm += -cos( (m_f32MouthAngle * (60.0f / 180.0f) * M_PI) + M_PI / 2 ) * rDirY;
	m_f32MouthPlaneLeftDist = -(m_rMouthPlaneLeftNorm.dot( m_rPos ));

	m_rMouthPlaneRightNorm = -sin( -(m_f32MouthAngle * (20.0f / 180.0f) * M_PI) + M_PI / 2 ) * rDirZ;
	m_rMouthPlaneRightNorm += cos( -(m_f32MouthAngle * (20.0f / 180.0f) * M_PI) + M_PI / 2 ) * rDirY;
	m_f32MouthPlaneRightDist = -(m_rMouthPlaneRightNorm.dot( m_rPos ));
}

void
FishC::set_follow_height( float32 f32Height )
{
	m_f32FollowHeight = f32Height;
}

const Vector3C&
FishC::get_pos() const
{
	return m_rPos;
}

float32
FishC::get_mouth_angle() const
{
	return m_f32MouthAngle;
}

float32
FishC::get_radius() const
{
	return m_f32Radius;
}

float32
FishC::get_rot_angle() const
{
	return m_f32RotAngle;
}

const Vector3C&
FishC::get_tgt_pos() const
{
	return m_rTgtPos;
}

uint32
FishC::get_mesh_idx() const
{
	return m_ui32FishMesh;
}

uint32
FishC::hit_test( const Vector3C& rPartPos, float32 f32PartRadius, uint32 ui32Flags, Vector3C& rForce )
{
	if( m_ui32State != FISHSTATE_FREE )
		return FISHHIT_NONE;

	// distance between particle and the fish
	Vector3C	rDistVec = rPartPos - m_rPos;
	float32		f32Dist = rDistVec.length();

	// Zero force and hit result
	rForce = Vector3C( 0, 0, 0 );
	uint32	ui32Hit = FISHHIT_NONE;

	// check if the particle penetrates the fish.
	if( f32Dist < (f32PartRadius + m_f32Radius) ) {

		// If the point is inside the mouth planes, use them to collision detection
		float32	f32Dist1, f32Dist2;

		f32Dist1 = rPartPos.dot( m_rMouthPlaneLeftNorm ) + m_f32MouthPlaneLeftDist;
		f32Dist2 = rPartPos.dot( m_rMouthPlaneRightNorm ) + m_f32MouthPlaneRightDist;

		if( ui32Flags == FISHHITFLAGS_SKIN_AND_MOUTH && f32Dist1 > 0 && f32Dist2 > 0 ) {

			// if we get far inside the mouth, catch it.
			if( f32Dist < (m_f32Radius - f32PartRadius) )
				return FISHHIT_MOUTH;

			// slippery lips...
			if( f32Dist1 < f32PartRadius ) {
				rForce = (f32Dist1 - f32PartRadius) * m_rMouthPlaneLeftNorm;
				ui32Hit = FISHHIT_SKIN;
			}
			if( f32Dist2 < f32PartRadius ) {
				rForce = (f32Dist2 - f32PartRadius) * m_rMouthPlaneRightNorm;
				ui32Hit = FISHHIT_SKIN;
			}
		}
		else {
			// Hit test the fish body.
			rDistVec = rDistVec.normalize();
			rDistVec *= ((f32PartRadius + m_f32Radius) - f32Dist);
			rForce = rDistVec;
			ui32Hit = FISHHIT_SKIN;
		}
	}

	return ui32Hit;
}

void
FishC::set_animation( uint32 ui32Type )
{
	m_rAnimPos = m_rPos;

	if( ui32Type == FISHSTATE_MISSED ) {
		m_i32AnimCount = FISHANIM_MISSED_COUNT;
	}
	else if( ui32Type == FISHSTATE_FLYAWAY ) {

		m_i32AnimCount = FISHANIM_FLYAWAY_COUNT;

		delete m_pSpline;
//		m_pSpline = new SplineCurveC( 7 );
		m_pSpline = new SplineCurveC( 10 );

		// Find angle
		Vector3C	rVec = m_rAnimPos;
		rVec[1] = 0;
		float32		f32Angle = atan2( rVec[2], rVec[0] );
		float32		f32Dist = rVec.length();

		float32		f32AngleDest = atan2( -155.0f, 200.0f );

		while( f32AngleDest < (f32Angle + 2.0f * M_PI) )
			f32AngleDest += 2.0f * (float32)M_PI;

		m_pSpline->set_point( 0, m_rAnimPos );
		f32Angle += (float32)M_PI / 6;

		rVec[0] = cos( f32Angle ) * f32Dist;
		rVec[1] = m_rAnimPos[1] + 60;
		rVec[2] = sin( f32Angle ) * f32Dist;
		m_pSpline->set_point( 1, rVec );
		f32Angle += (float32)M_PI / 6;

		// from now on, take equal steps towards the destination angle
		float32	f32DeltaAngle = (f32AngleDest - (f32Angle + M_PI / 2)) / 5.0f;

		rVec[0] = cos( f32Angle  ) * f32Dist;
		rVec[1] = /*m_rAnimPos[1] +*/ 30;
		rVec[2] = sin( f32Angle ) * f32Dist;
		m_pSpline->set_point( 2, rVec );

		f32Angle += f32DeltaAngle;

		rVec[0] = cos( f32Angle ) * f32Dist;
		rVec[1] = /*m_rAnimPos[1] +*/ 35;
		rVec[2] = sin( f32Angle ) * f32Dist;
		m_pSpline->set_point( 3, rVec );

		f32Angle += f32DeltaAngle;

		rVec[0] = cos( f32Angle ) * f32Dist;
		rVec[1] = /*m_rAnimPos[1] +*/ 25;
		rVec[2] = sin( f32Angle ) * f32Dist;
		m_pSpline->set_point( 4, rVec );

		f32Angle += f32DeltaAngle;

		rVec[0] = cos( f32Angle ) * f32Dist;
		rVec[1] = /*m_rAnimPos[1] +*/ 45;
		rVec[2] = sin( f32Angle ) * f32Dist;
		m_pSpline->set_point( 5, rVec );

		f32Angle += f32DeltaAngle;

		rVec[0] = cos( f32Angle ) * f32Dist;
		rVec[1] = /*m_rAnimPos[1] +*/ 35;
		rVec[2] = sin( f32Angle ) * f32Dist;
		m_pSpline->set_point( 6, rVec );

		f32Angle += f32DeltaAngle;

		rVec[0] = cos( f32Angle ) * f32Dist;
		rVec[1] = /*m_rAnimPos[1] +*/ 55;
		rVec[2] = sin( f32Angle ) * f32Dist;
		m_pSpline->set_point( 7, rVec );


		// the two last poits are always fixed
		m_pSpline->set_point( 8, Vector3C( 200, 90, -155 ) );
		m_pSpline->set_point( 9, Vector3C( 1160, 370, -450 ) );


/*		rVec[0] = cos( f32Angle +  M_PI / 6  ) * f32Dist;
		rVec[1] = m_rAnimPos[1] + 60;
		rVec[2] = sin( f32Angle +  M_PI / 6 ) * f32Dist;
		m_pSpline->set_point( 1, rVec );

		rVec[0] = cos( f32Angle + M_PI / 2 ) * f32Dist;
		rVec[1] = m_rAnimPos[1] + 25;
		rVec[2] = sin( f32Angle + M_PI / 2 ) * f32Dist;
		m_pSpline->set_point( 2, rVec );

		f32Dist += 10.0f;

		rVec[0] = cos( f32Angle + M_PI ) * f32Dist;
		rVec[1] = m_rAnimPos[1] + 50;
		rVec[2] = sin( f32Angle + M_PI ) * f32Dist;
		m_pSpline->set_point( 3, rVec );

		f32Dist += 10.0f;

		rVec[0] = cos( f32Angle + M_PI * 1.5f ) * f32Dist;
		rVec[1] = m_rAnimPos[1] + 60;
		rVec[2] = sin( f32Angle + M_PI * 1.5f ) * f32Dist;
		m_pSpline->set_point( 4, rVec );

		f32Dist += 10.0f;

		rVec[0] = cos( f32Angle + M_PI * 2 ) * f32Dist;
		rVec[1] = m_rAnimPos[1] + 80;
		rVec[2] = sin( f32Angle + M_PI * 2 ) * f32Dist;
		m_pSpline->set_point( 5, rVec );

		f32Dist += 300.0f;

		rVec[0] = cos( f32Angle + M_PI * 2.5f ) * f32Dist;
		rVec[1] = m_rAnimPos[1] + 200;
		rVec[2] = sin( f32Angle + M_PI * 2.5f ) * f32Dist;
		m_pSpline->set_point( 6, rVec );*/

		m_pSpline->prepare();
	}
}

void
FishC::set_state( uint32 ui32State )
{
	m_ui32State = ui32State;

	if( m_ui32State == FISHSTATE_MISSED )
		set_animation( FISHSTATE_MISSED );
	else if( m_ui32State == FISHSTATE_FLYAWAY )
		set_animation( FISHSTATE_FLYAWAY );
}

uint32
FishC::get_state() const
{
	return m_ui32State;
}

void
FishC::set_draw_state( uint32 ui32State )
{
	m_ui32DrawState = ui32State;
}

uint32
FishC::get_draw_state() const
{
	return m_ui32DrawState;
}

void
FishC::draw( FishingContextC* pContext )
{
	if( m_ui32State == FISHSTATE_EATEN )
		return;
}
