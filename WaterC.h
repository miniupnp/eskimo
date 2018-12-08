//-------------------------------------------------------------------------
//
// File:		WaterC.h
// Desc:		Water class.
// Author:		memon <memon@inside.org>
//
//-------------------------------------------------------------------------
//	Copyright (c) 2002 Moppi Productions. All Rights Reserved.
//  This file is part of Eskimo Fishing Trip game. For conditions of 
//  distribution and use, see the accompanying license.txt file.
//  http://moppi.inside.org/
//-------------------------------------------------------------------------

#ifndef __ESKIMO_WATERC_H__
#define __ESKIMO_WATERC_H__

// forward declaration
class WaterC;

#include "PajaTypes.h"
#include "Vector3C.h"
#include "ColorC.h"
#include "TextureC.h"
#include "CameraC.h"


class WaterC
{
public:
	WaterC();
	~WaterC();

	PajaTypes::uint32	get_slices() const;
	PajaTypes::uint32	get_stacks() const;
	void				set_water_color1( const PajaTypes::ColorC& rCol );
	void				set_water_color2( const PajaTypes::ColorC& rCol );
	PajaTypes::ColorC	get_water_color1() const;
	PajaTypes::ColorC	get_water_color2() const;
	void				set_environment_texture( const char* szName );
	void				set_maximum_amplitude( PajaTypes::float32 f32Amp );
	void				set_min_radius( PajaTypes::float32 f32Rad );
	void				set_max_radius( PajaTypes::float32 f32Rad );
	void				set_grid_size( PajaTypes::uint32 ui32Slices, PajaTypes::uint32 ui32Stacks );
	void				set_diffuse( const PajaTypes::ColorC& rCol );
	void				set_specular( const PajaTypes::ColorC& rCol );
	void				set_reflection_tex_size( PajaTypes::uint32 ui32Width, PajaTypes::uint32 ui32Height );
	PajaTypes::uint32	get_reflection_tex_width() const;
	PajaTypes::uint32	get_reflection_tex_height() const;
	void				set_draw_reflections( bool bState );
	bool				get_draw_reflections() const;
	void				init();
	PajaTypes::uint32	get_point_count() const;
	PajaTypes::float32*	get_points_ptr();
	PajaTypes::float32*	get_normals_ptr();
	PajaTypes::uint32	get_index_count() const;
	PajaTypes::uint32*	get_indices_ptr();
	void				impact( const PajaTypes::Vector3C& rPoint, PajaTypes::float32 f32Force );
	void				calc_normals();
	void				eval( PajaTypes::float32 f32DeltaTime );
	void				draw( CameraC* pCam, PajaTypes::float32 f32Aspect );
	void				setup_texture_projection( CameraC* pCam, PajaTypes::float32 f32Aspect );
	void				restore_texture_projection();
	void				begin_reflections();
	void				end_reflections();


protected:
	bool								m_bDrawReflections;
	PajaTypes::ColorC					m_rWaterColor1;
	PajaTypes::ColorC					m_rWaterColor2;
	PajaTypes::uint32					m_ui32Slices, m_ui32Stacks;
	PajaTypes::uint32					m_ui32HeightSource;
	PajaTypes::float32					m_f32Radius1, m_f32Radius2;
	PajaTypes::float32					m_f32MaxAmp;
	PajaTypes::float32					m_f32Time;
	std::vector<PajaTypes::uint32>		m_rIndices;
	std::vector<PajaTypes::float32>		m_rPosition;
	std::vector<PajaTypes::float32>		m_rNormals;
	std::vector<PajaTypes::float32>		m_rHeight[2];
	TextureC*							m_pEnvTexture;
	PajaTypes::uint32					m_ui32ReflTexWidth, m_ui32ReflTexHeight;
	PajaTypes::uint32					m_ui32ReflTexId;
	PajaTypes::ColorC					m_rDiffuse, m_rSpecular;
};


#endif __WATERC_H__