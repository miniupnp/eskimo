//-------------------------------------------------------------------------
//
// File:		FishingRodC.h
// Desc:		Fishing rod class (see also: FishingLineC).
// Author:		memon <memon@inside.org>
//
//-------------------------------------------------------------------------
//	Copyright (c) 2002 Moppi Productions. All Rights Reserved.
//  This file is part of Eskimo Fishing Trip game. For conditions of 
//  distribution and use, see the accompanying license.txt file.
//  http://moppi.inside.org/
//-------------------------------------------------------------------------

#ifndef __ESKIMO_FISHINGRODC_H__
#define __ESKIMO_FISHINGRODC_H__

#include "PajaTypes.h"
#include "Vector3C.h"
#include "FishingLineC.h"
#include "SceneC.h"


class FishingRodC
{
public:
	FishingRodC();
	~FishingRodC();

	void						set_pos( const PajaTypes::Vector3C& rPos );
	void						set_angle_x( PajaTypes::float32 f32Angle );
	void						set_angle_y( PajaTypes::float32 f32Angle );
	void						set_length( PajaTypes::float32 f32Len );
	void						set_line_length( PajaTypes::float32 f32Len );

	void						set_bait_force( PajaTypes::float32 f32Force );

	void						set_rod_mesh( const char* szName );
	void						set_bait_mesh( const char* szName );
	
	const PajaTypes::Vector3C&	get_bait_pos() const;
	const PajaTypes::Vector3C&	get_tip_pos() const;
	PajaTypes::uint32			get_particle_count() const;
	const PajaTypes::Vector3C&	get_particle_pos( PajaTypes::uint32 ui32Idx ) const;
	const PajaTypes::Vector3C&	get_dir() const;

	PajaTypes::uint32			get_bait_state() const;
	PajaTypes::uint32			get_fish_index() const;

	void						init_baits( PajaTypes::int32 i32Count );
	PajaTypes::int32			get_bait_count() const;
	PajaTypes::int32			get_bait_freshness() const;
	void						set_reload_distance( PajaTypes::float32 f32Dist );

	void						init( FishingContextC* pContext );
	void						update( FishingContextC* pContext );
	void						draw( FishingContextC* pContext );

protected:
	PajaTypes::float32		m_f32AngleX;
	PajaTypes::float32		m_f32AngleY;
	PajaTypes::float32		m_f32Length;
	PajaTypes::float32		m_f32LineLength;
	PajaTypes::Vector3C		m_rPos;
	PajaTypes::Vector3C		m_rDir;
	PajaTypes::Vector3C		m_rTipPos;
	PajaTypes::float32		m_f32ExtraBaitForce;
	FishingLineC*			m_pLine;
	SceneC*					m_pRodScene;
};


#endif // __FISHINGRODC_H__