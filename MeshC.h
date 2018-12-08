//-------------------------------------------------------------------------
//
// File:		MeshC.h
// Desc:		mesh class.
// Author:		memon <memon@inside.org>
//
//-------------------------------------------------------------------------
//	Copyright (c) 2002 Moppi Productions. All Rights Reserved.
//  This file is part of Eskimo Fishing Trip game. For conditions of 
//  distribution and use, see the accompanying license.txt file.
//  http://moppi.inside.org/
//-------------------------------------------------------------------------

#ifndef __ESKIMO_MESHC_H__
#define __ESKIMO_MESHC_H__

#include "PajaTypes.h"
#include "ColorC.h"
#include "ScenegraphItemI.h"
#include "QuatC.h"
#include "Vector2C.h"
#include "Vector3C.h"

#include "TextureC.h"

const PajaTypes::int32	CGITEM_MESH = 0x1000;


class MeshC : public ScenegraphItemI
{
public:
	MeshC();
	virtual ~MeshC();

	virtual PajaTypes::int32			get_type();

	// transformation

	virtual void						eval_state( PajaTypes::int32 i32Time );

	virtual void						set_position( const PajaTypes::Vector3C& rPos );
	virtual const PajaTypes::Vector3C&	get_position();

	virtual void						set_scale( const PajaTypes::Vector3C& rScale );
	virtual const PajaTypes::Vector3C&	get_scale();

	virtual void						set_scale_rotation( const PajaTypes::QuatC& rRot );
	virtual const PajaTypes::QuatC&		get_scale_rotation();

	virtual void						set_rotation( const PajaTypes::QuatC& rRot );
	virtual const PajaTypes::QuatC&		get_rotation();

	// geometry

	virtual void						add_index( PajaTypes::uint32 ui32Index );		
	virtual void						add_vert( const PajaTypes::Vector3C& rVec );
	virtual void						add_norm( const PajaTypes::Vector3C& rNorm );
	virtual void						add_texcoord( const PajaTypes::Vector2C& rTex );

	virtual PajaTypes::uint32			get_index_count() const;
	virtual PajaTypes::uint32			get_vert_count() const;
	virtual PajaTypes::uint32			get_norm_count() const;
	virtual PajaTypes::uint32			get_texcoord_count() const;

	virtual PajaTypes::uint32			get_index( PajaTypes::uint32 ui32Index );
	virtual PajaTypes::Vector3C			get_vert( PajaTypes::uint32 ui32Index );
	virtual PajaTypes::Vector3C			get_norm( PajaTypes::uint32 ui32Index );
	virtual PajaTypes::Vector2C			get_texcoord( PajaTypes::uint32 ui32Index );

	virtual PajaTypes::uint32*			get_index_ptr();
	virtual PajaTypes::float32*			get_vert_ptr();
	virtual PajaTypes::float32*			get_norm_ptr();
	virtual PajaTypes::float32*			get_texcoord_ptr();

	// material
	virtual void						set_texture( TextureC* pTex );
	virtual TextureC*					get_texture() const;

	virtual void						set_ambient( const PajaTypes::ColorC &rAmbient );
	virtual void						set_diffuse( const PajaTypes::ColorC &rDiffuse );
	virtual void						set_specular( const PajaTypes::ColorC &rSpecular );
	virtual void						set_shininess( const PajaTypes::float32 f32Shininess );
	virtual void						set_two_sided( bool bState );

	virtual const PajaTypes::ColorC&	get_ambient() const;
	virtual const PajaTypes::ColorC&	get_diffuse() const;
	virtual const PajaTypes::ColorC&	get_specular() const;
	virtual PajaTypes::float32			get_shininess();
	virtual bool						get_two_sided() const;

private:

	PajaTypes::Vector3C			m_rPosition;
	PajaTypes::Vector3C			m_rScale;
	PajaTypes::QuatC			m_rScaleRot;
	PajaTypes::QuatC			m_rRotation;

	// Material
	PajaTypes::ColorC			m_rAmbient;
	PajaTypes::ColorC			m_rDiffuse;
	PajaTypes::ColorC			m_rSpecular;
	PajaTypes::float32			m_f32Shininess;
	bool						m_bTwoSided;
	TextureC*					m_pTexture;

	//Geometry
	std::vector<PajaTypes::uint32>		m_rIndices;
	std::vector<PajaTypes::float32>		m_rVertices;
	std::vector<PajaTypes::float32>		m_rNormals;
	std::vector<PajaTypes::float32>		m_rTexCoords;
};


#endif // __MESHC_H__