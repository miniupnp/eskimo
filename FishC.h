//-------------------------------------------------------------------------
//
// File:		FishC.h
// Desc:		Fish class.
// Author:		memon <memon@inside.org>
//
//-------------------------------------------------------------------------
//	Copyright (c) 2002 Moppi Productions. All Rights Reserved.
//  This file is part of Eskimo Fishing Trip game. For conditions of 
//  distribution and use, see the accompanying license.txt file.
//  http://moppi.inside.org/
//-------------------------------------------------------------------------

#ifndef __ESKIMO_FISHC_H__
#define __ESKIMO_FISHC_H__

// forward declaration
class FishC;

#include "PajaTypes.h"
#include "Vector3C.h"
#include "SplineCurveC.h"
#include "FishingContextC.h"

// fish states
enum FishStateE {
	FISHSTATE_FREE = 0,
	FISHSTATE_FOLLOWBAIT,
	FISHSTATE_FLYAWAY,
	FISHSTATE_EATEN,
	FISHSTATE_MISSED,
};

enum FishDrawStateE {
	FISHDRAWSTATE_NORMAL = 0,
	FISHDRAWSTATE_GLOW,
};

enum FishHitTestE {
	FISHHIT_NONE = 0,			// no hit
	FISHHIT_MOUTH = 101,		// hit inside the mouth
	FISHHIT_SKIN = 102,			// hit on fish skin
};

// for animation, in frames
enum FishAnimE {
	FISHANIM_MISSED_COUNT = 10,
	FISHANIM_FLYAWAY_COUNT = 150,
};


enum FishHitFlagsE {
	FISHHITFLAGS_SKIN = 0,
	FISHHITFLAGS_SKIN_AND_MOUTH
};


class FishC
{
public:
	FishC();
	~FishC();

	void						init( PajaTypes::float32 f32Radius, PajaTypes::float32 f32Angle,
									PajaTypes::float32 f32Distance, PajaTypes::float32 f32Freq, PajaTypes::uint32 ui32FishMesh );

	void						update( FishingContextC* pContext );

	void						set_follow_height( PajaTypes::float32 f32Height );

	const PajaTypes::Vector3C&	get_pos() const;
	PajaTypes::float32			get_mouth_angle() const;
	PajaTypes::float32			get_radius() const;
	PajaTypes::float32			get_rot_angle() const;
	const PajaTypes::Vector3C&	get_tgt_pos() const;
	PajaTypes::uint32			get_mesh_idx() const;

	PajaTypes::uint32			hit_test( const PajaTypes::Vector3C& rPos, PajaTypes::float32 f32PartRadius, PajaTypes::uint32 ui32Flags, PajaTypes::Vector3C& rForce );

	void						set_state( PajaTypes::uint32 ui32State );
	PajaTypes::uint32			get_state() const;

	void						set_draw_state( PajaTypes::uint32 ui32State );
	PajaTypes::uint32			get_draw_state() const;

	void						draw( FishingContextC* pContext );

protected:

	void						set_animation( PajaTypes::uint32 ui32Type );

	PajaTypes::Vector3C			m_rPos;
	PajaTypes::Vector3C			m_rTgtPos;
	PajaTypes::Vector3C			m_rAnimPos;
	PajaTypes::float32			m_f32RotAngle;
	PajaTypes::float32			m_f32MouthAngle;
	PajaTypes::uint32			m_ui32FishMesh;

	PajaTypes::int32			m_i32AnimCount;
	
	PajaTypes::Vector3C			m_rMouthPlaneLeftNorm;
	PajaTypes::float32			m_f32MouthPlaneLeftDist;
	PajaTypes::Vector3C			m_rMouthPlaneRightNorm;
	PajaTypes::float32			m_f32MouthPlaneRightDist;

	PajaTypes::float32			m_f32Radius;			// radius of the fish
	PajaTypes::float32			m_f32OrigAngle;			// creation angle (in the pond circle)
	PajaTypes::float32			m_f32Distance;			// distance from the pond center
	PajaTypes::float32			m_f32Freq;				// frequency of the mouth angle anim

	PajaTypes::float32			m_f32FollowHeight;

	PajaTypes::uint32			m_ui32State;
	PajaTypes::uint32			m_ui32DrawState;

	SplineCurveC*				m_pSpline;
};


#endif // __FISHC_H__
