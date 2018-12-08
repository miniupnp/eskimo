//-------------------------------------------------------------------------
//
// File:		FishingContextC.h
// Desc:		Fishing context class.
// Author:		memon <memon@inside.org>
//
//-------------------------------------------------------------------------
//	Copyright (c) 2002 Moppi Productions. All Rights Reserved.
//  This file is part of Eskimo Fishing Trip game. For conditions of 
//  distribution and use, see the accompanying license.txt file.
//  http://moppi.inside.org/
//-------------------------------------------------------------------------

#ifndef __ESKIMO_FISHINGCONTEXTC_H__
#define __ESKIMO_FISHINGCONTEXTC_H__

// forward declaration
class FishingContextC;

#include "PajaTypes.h"
#include "Vector3C.h"
#include "FishingPondC.h"
#include "WaterC.h"
#include "FishingRodC.h"
#include "ColorC.h"
#include "fmod.h"
#include <vector>
#include <string>
#include "GLWindowC.h"

enum RenderingDetailE {
	RENDERINGDETAIL_LOW = 0,
	RENDERINGDETAIL_NORMAL,
	RENDERINGDETAIL_HIGH,
};


enum SoundsE {
	SOUND_BAIT_HIT_WATER = 0,
	SOUND_BAIT_LEFT_WATER,
	SOUND_FISH_CATCH,
	SOUND_FISH_MISSED,
	MAX_SOUNDS
};


class FishingContextC
{
public:
	FishingContextC();
	~FishingContextC();

	void				set_rendering_detail( PajaTypes::uint32 ui32Level );
	PajaTypes::uint32	get_rendering_detail() const;

	void				init_level();
	bool				load_level( const char* szName );

	PajaTypes::ColorC	get_level_brief_color();
	const char*			get_level_brief_name();
	const char*			get_level_brief_desc();

	void				set_render_reflections( bool bState );
	void				set_reflections_tex_size( PajaTypes::uint32 ui32W, PajaTypes::uint32 ui32H );

	PajaTypes::int32	get_timer();

	bool				load_sound( PajaTypes::uint32 ui32Sound, const char* szName );
	void				play_sound( PajaTypes::uint32 ui32Sound );

	FishingPondC*		get_pond();

	void				set_catch_height( PajaTypes::float32 f32Height );
	PajaTypes::float32	get_catch_height() const;

	void				fish_catch( PajaTypes::uint32 ui32Indx );

	PajaTypes::float32	get_aspect() const;
	void				set_aspect( PajaTypes::float32 f32Aspect );

	PajaTypes::float32	get_frame() const;
	void				set_frame( PajaTypes::float32 f32Frame );

	void				update();	// call 30 times/sec

	void				draw( GLWindowC* pWnd );

	void				set_rod( FishingRodC* pRod );
	
	void				add_score( PajaTypes::int32 i32Score );
	void				set_score( PajaTypes::int32 i32Score );
	PajaTypes::int32	get_score() const;

protected:
	WaterC*				m_pWater;
	FishingPondC*		m_pPond;
	PajaTypes::float32	m_f32CatchHeight;
	PajaTypes::float32	m_f32Frame;
	PajaTypes::float32	m_f32Aspect;
	FSOUND_SAMPLE*		m_rSoundFx[MAX_SOUNDS];
	FishingRodC*		m_pPlayer;

	PajaTypes::int32	m_i32Timer;
	PajaTypes::ColorC	m_rBriefColor;
	std::string			m_sBriefName;
	std::string			m_sBriefDesc;

	PajaTypes::int32	m_i32Score;
	PajaTypes::int32	m_i32MaxBaitCount;

	bool				m_bRenderReflections;
};



#endif // __FISHINGCONTEXTC_H__
