//-------------------------------------------------------------------------
//
// File:		FishingLineC.h
// Desc:		Fishing line class (see also FishingRodC).
// Author:		memon <memon@inside.org>
//
//-------------------------------------------------------------------------
//	Copyright (c) 2002 Moppi Productions. All Rights Reserved.
//  This file is part of Eskimo Fishing Trip game. For conditions of 
//  distribution and use, see the accompanying license.txt file.
//  http://moppi.inside.org/
//-------------------------------------------------------------------------

#ifndef __ESKIMO_FISHINGLINEC_H__
#define __ESKIMO_FISHINGLINEC_H__

// forward declaration
class FishingLineC;

#include "PajaTypes.h"
#include "Vector3C.h"
#include "FishingContextC.h"
#include "SplineCurveC.h"
#include <vector>
#include "SceneC.h"


enum BaitStateE {
	BAITSTATE_FREE,
	BAITSTATE_ON_WATER,
	BAITSTATE_ON_LAND,
	BAITSTATE_INSIDE_FISH,
};


#define		BAIT_MAX_FRESHNESS	100
#define		BAIT_WATER_DAMAGE	10
#define		BAIT_LAND_DAMAGE	15


class FishingLineC
{
public:
	FishingLineC();
	~FishingLineC();

	void						set_bait_mesh( const char* szName );

	void						set_bait_mass( PajaTypes::float32 f32Mass );
	void						set_tip_pos( const PajaTypes::Vector3C& rPos );
	const PajaTypes::Vector3C&	get_tip_pos() const;

	void						set_bait_pos( const PajaTypes::Vector3C& rPos );
	const PajaTypes::Vector3C&	get_bait_pos() const;

	void						set_extra_bait_force( const PajaTypes::Vector3C& rF );

	void						set_bait_state( PajaTypes::uint32 ui32State );
	PajaTypes::uint32			get_bait_state() const;

	void						init_baits( PajaTypes::int32 i32Count );
	PajaTypes::int32			get_bait_count() const;
	PajaTypes::int32			get_bait_freshness() const;

	PajaTypes::uint32			get_particle_count() const;
	const PajaTypes::Vector3C&	get_particle_pos( PajaTypes::uint32 ui32Idx ) const;

	void						set_reload_distance( PajaTypes::float32 f32Dist );

	PajaTypes::uint32			get_fish_index() const;

	void						init( FishingContextC* pContext );

	void						update( FishingContextC* pContext, PajaTypes::float32 f32DeltaTime );

	void						draw( PajaTypes::float32 f32RotY, FishingContextC* pContext );

protected:

	void	add_bait_damage( PajaTypes::int32 i32Count );

	struct ParticleS
	{
		PajaTypes::Vector3C	m_rPos;
		PajaTypes::Vector3C	m_rVel;
		PajaTypes::Vector3C	m_rAcc;
		PajaTypes::Vector3C	m_rF;
		PajaTypes::float32	m_f32Mass;
	};

	std::vector<ParticleS>	m_rParticles;			// particles which forms the line
	PajaTypes::float32		m_f32PartInitDist;		// initial distance between each particle
	PajaTypes::float32		m_f32BaitMass;			// bait mass
	PajaTypes::Vector3C		m_rTipPos;				// tip position
	PajaTypes::Vector3C		m_rBaitPos;				// bait position
	PajaTypes::Vector3C		m_rExtraBaitForce;		// extra force added to bait at update (cleared after update)
	SplineCurveC*			m_pSpline;				// spline used to draw the line
	PajaTypes::uint32		m_ui32BaitIndex;		// bait index within the particles array
	PajaTypes::uint32		m_ui32BaitState;		// current bait state
	PajaTypes::uint32		m_ui32FishIndex;		// the index of the fish the bait is in

	SceneC*					m_pBaitScene;

	PajaTypes::int32		m_i32BaitFreshness;
	PajaTypes::int32		m_i32BaitCount;
	PajaTypes::float32		m_f32ReloadDist;
};



#endif // __FISHINGLINEC_H__
