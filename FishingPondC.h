//-------------------------------------------------------------------------
//
// File:		FishingPondC.h
// Desc:		Fishing pond class (see also: WaterC).
// Author:		memon <memon@inside.org>
//
//-------------------------------------------------------------------------
//	Copyright (c) 2002 Moppi Productions. All Rights Reserved.
//  This file is part of Eskimo Fishing Trip game. For conditions of 
//  distribution and use, see the accompanying license.txt file.
//  http://moppi.inside.org/
//-------------------------------------------------------------------------

#ifndef __ESKIMO_FISHINGPONDC_H__
#define __ESKIMO_FISHINGPONDC_H__

// forward declaration
class FishingPondC;

#include "PajaTypes.h"
#include "Vector3C.h"
#include "FishC.h"
#include "FishingContextC.h"
#include "SceneC.h"
#include "FishMeshC.h"
#include "ColorC.h"


enum PondHitE {
	PONDHIT_NONE,		// no hit
	PONDHIT_WATER,		// hit pond water surface
	PONDHIT_LAND,		// hit the land surrounding the pond (or the island in the pond)
};


class FishingPondC
{
public:
	FishingPondC();
	~FishingPondC();

	PajaTypes::uint32			hit_test( const PajaTypes::Vector3C& rPos, PajaTypes::float32 f32PartRadius, PajaTypes::Vector3C& rForce );

	PajaTypes::uint32			get_fish_count() const;
	FishC*						get_fish( PajaTypes::uint32	ui32Idx );

	PajaTypes::uint32			get_alive_fish_count();

	void						set_pond_mesh( const char* szName );
	void						set_fish_mesh( PajaTypes::uint32 ui32Idx, const char* szName );		// idx max: 3

	void						set_bg_image( const char* szName );
	void						bind_bg_image();
	void						set_fog_start( PajaTypes::float32 f32Val );
	void						set_fog_end( PajaTypes::float32 f32Val );
	void						set_fog_color( const PajaTypes::ColorC& rCol );
	PajaTypes::float32			get_fog_start() const;
	PajaTypes::float32			get_fog_end() const;
	PajaTypes::ColorC			get_fog_color();


	// call these methods only _once_ before the init.
	void						set_fish_row_count( PajaTypes::uint32 ui32Rows );	// range 1 to 3
	void						set_fish_col_count( PajaTypes::uint32 ui32Cols );	// range 1 to 10
	void						set_fish_base_freq( PajaTypes::float32 f32Freq );			// min freq				(revolutions/"frame")
	void						set_fish_freq_variation( PajaTypes::float32 f32Freq );	// freq variation
	void						set_min_radius( PajaTypes::float32 f32Rad );
	void						set_max_radius( PajaTypes::float32 f32Rad );
	void						set_water_height( PajaTypes::float32 f32Val );
	void						set_land_height( PajaTypes::float32 f32Val );
	void						set_stream_current( PajaTypes::float32 f32Val );

	void						init( FishingContextC* pContext );
	void						update( FishingContextC* pContext );

	SceneC*						get_pond_scene() const;
	void						draw( FishingContextC* pContext );

protected:
	PajaTypes::float32	m_f32FogStart;
	PajaTypes::float32	m_f32FogEnd;
	PajaTypes::ColorC	m_rFogColor;

	PajaTypes::uint32	m_ui32Rows;
	PajaTypes::uint32	m_ui32Cols;
	PajaTypes::float32	m_f32FreqBase;
	PajaTypes::float32	m_f32FreqVar;
	PajaTypes::float32	m_f32MinRad;
	PajaTypes::float32	m_f32MaxRad;
	PajaTypes::float32	m_f32WaterHeight;
	PajaTypes::float32	m_f32LandHeight;
	PajaTypes::float32	m_f32StreamCurrent;
	std::vector<FishC*>	m_rFish;
	SceneC*				m_pPondScene;
	FishMeshC*			m_pFishMesh[3];
	TextureC*			m_pBgTexture;
};


#endif // __FISHINGPONDC_H__
