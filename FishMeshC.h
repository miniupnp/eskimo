//-------------------------------------------------------------------------
//
// File:		FishMeshC.h
// Desc:		Fishmesh class. A class to morph two meshes for fish animation.
// Author:		memon <memon@inside.org>
//
//-------------------------------------------------------------------------
//	Copyright (c) 2002 Moppi Productions. All Rights Reserved.
//  This file is part of Eskimo Fishing Trip game. For conditions of 
//  distribution and use, see the accompanying license.txt file.
//  http://moppi.inside.org/
//-------------------------------------------------------------------------

#ifndef __ESKIMO_FISHMESHC_H__
#define __ESKIMO_FISHMESHC_H__

#include "PajaTypes.h"
#include "Vector3C.h"
#include "QuatC.h"
#include "ColorC.h"
#include "TextureC.h"
#include <vector>

class FishMeshC
{
public:
	FishMeshC();
	~FishMeshC();

	bool						load( const char* szName );

	void						eval_state( PajaTypes::float32 f32State );

	PajaTypes::uint32			get_index_count() const;
	PajaTypes::uint32			get_vert_count() const;
	PajaTypes::uint32			get_norm_count() const;
	PajaTypes::uint32			get_texcoord_count() const;

	PajaTypes::uint32*			get_index_ptr();
	PajaTypes::float32*			get_vert_ptr();
	PajaTypes::float32*			get_norm_ptr();
	PajaTypes::float32*			get_texcoord_ptr();

	// material
	TextureC*					get_texture() const;

	const PajaTypes::ColorC&	get_ambient() const;
	const PajaTypes::ColorC&	get_diffuse() const;
	const PajaTypes::ColorC&	get_specular() const;
	PajaTypes::float32			get_shininess();

	void						draw( PajaTypes::float32 f32State, const PajaTypes::Vector3C& rPos, const PajaTypes::Vector3C& rTgt, const PajaTypes::QuatC& rRot, bool bUseTgt, bool bGlow );

protected:
	// Material
	PajaTypes::ColorC			m_rAmbient;
	PajaTypes::ColorC			m_rDiffuse;
	PajaTypes::ColorC			m_rSpecular;
	PajaTypes::float32			m_f32Shininess;
	TextureC*					m_pTexture;

	//Geometry
	std::vector<PajaTypes::uint32>		m_rIndices;
	std::vector<PajaTypes::float32>		m_rVertices;
	std::vector<PajaTypes::float32>		m_rVerticesSource;
	std::vector<PajaTypes::float32>		m_rVerticesTarget;
	std::vector<PajaTypes::float32>		m_rNormals;
	std::vector<PajaTypes::float32>		m_rNormalsSource;
	std::vector<PajaTypes::float32>		m_rNormalsTarget;
	std::vector<PajaTypes::float32>		m_rTexCoords;
};


#endif // __FISHMESHC_H__