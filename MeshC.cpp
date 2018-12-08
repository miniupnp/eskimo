//-------------------------------------------------------------------------
//
// File:		MeshC.cpp
// Desc:		mesh class.
// Author:		memon <memon@inside.org>
//
//-------------------------------------------------------------------------
//	Copyright (c) 2002 Moppi Productions. All Rights Reserved.
//  This file is part of Eskimo Fishing Trip game. For conditions of 
//  distribution and use, see the accompanying license.txt file.
//  http://moppi.inside.org/
//-------------------------------------------------------------------------

#include <assert.h>
#include "ScenegraphItemI.h"
#include "PajaTypes.h"
#include "MeshC.h"

using namespace PajaTypes;


MeshC::MeshC() :
	m_pTexture( 0 ),
	m_bTwoSided( false )
{
	// empty
}

MeshC::~MeshC()
{
	// empty
}

int32
MeshC::get_type()
{
	return CGITEM_MESH;
}

void
MeshC::set_position( const Vector3C& rPos )
{
	m_rPosition = rPos;
}


const
Vector3C&
MeshC::get_position()
{
	return m_rPosition;
}

void
MeshC::set_scale( const Vector3C& rScale )
{
	m_rScale = rScale;
}

const
Vector3C&
MeshC::get_scale()
{
	return m_rScale;
}

void
MeshC::set_rotation( const QuatC& rRot )
{
	m_rRotation = rRot;
}

const
QuatC&
MeshC::get_rotation()
{
	return m_rRotation;
}

void
MeshC::set_scale_rotation( const QuatC& rRot )
{
	m_rScaleRot = rRot;
}

const
QuatC&
MeshC::get_scale_rotation()
{
	return m_rScaleRot;
}


void
MeshC::eval_state( int32 i32Time )
{
}


void
MeshC::add_index( uint32 ui32Index )
{
	m_rIndices.push_back( ui32Index );
}

void
MeshC::add_vert( const Vector3C& rVec )
{
	m_rVertices.push_back( rVec[0] );
	m_rVertices.push_back( rVec[1] );
	m_rVertices.push_back( rVec[2] );
}

void
MeshC::add_norm( const Vector3C& rNorm )
{
	m_rNormals.push_back( rNorm[0] );
	m_rNormals.push_back( rNorm[1] );
	m_rNormals.push_back( rNorm[2] );
}

void
MeshC::add_texcoord( const Vector2C& rTex )
{
	m_rTexCoords.push_back( rTex[0] );
	m_rTexCoords.push_back( rTex[1] );
}

uint32
MeshC::get_index_count() const
{
	return m_rIndices.size();
}

uint32
MeshC::get_vert_count() const
{
	return m_rVertices.size() / 3;
}

uint32
MeshC::get_norm_count() const
{
	return m_rNormals.size() / 3;
}

uint32
MeshC::get_texcoord_count() const
{
	return m_rTexCoords.size() / 2;
}

uint32
MeshC::get_index( uint32 ui32Index )
{
	assert( ui32Index < m_rIndices.size() );
	return m_rIndices[ui32Index];
}

Vector3C
MeshC::get_vert( uint32 ui32Index )
{
	ui32Index *= 3;
	assert( ui32Index < m_rVertices.size() );
	return Vector3C( m_rVertices[ui32Index], m_rVertices[ui32Index + 1], m_rVertices[ui32Index + 2] );
}

Vector3C
MeshC::get_norm( uint32 ui32Index )
{
	ui32Index *= 3;
	assert( ui32Index < m_rNormals.size() );
	return Vector3C( m_rNormals[ui32Index], m_rNormals[ui32Index + 1], m_rNormals[ui32Index + 2] );
}

Vector2C
MeshC::get_texcoord( uint32 ui32Index )
{
	ui32Index *= 2;
	assert( ui32Index < m_rTexCoords.size() );
	return Vector2C( m_rTexCoords[ui32Index], m_rTexCoords[ui32Index + 1] );
}

uint32*
MeshC::get_index_ptr()
{
	return &(m_rIndices[0]);
}

float32*
MeshC::get_vert_ptr()
{
	return &(m_rVertices[0]);
}

float32*
MeshC::get_norm_ptr()
{
	return &(m_rNormals[0]);
}

float32*
MeshC::get_texcoord_ptr()
{
	return &(m_rTexCoords[0]);
}



// material
void
MeshC::set_texture( TextureC* pTex )
{
	m_pTexture = pTex;
}

TextureC*
MeshC::get_texture() const
{
	return m_pTexture;
}

void
MeshC::set_ambient( const ColorC &rAmbient )
{
	m_rAmbient = rAmbient;
}

void
MeshC::set_diffuse( const ColorC &rDiffuse )
{
	m_rDiffuse = rDiffuse;
}

void
MeshC::set_specular( const ColorC &rSpecular )
{
	m_rSpecular = rSpecular;
}

void
MeshC::set_shininess( const float32 f32Shininess )
{
	m_f32Shininess = f32Shininess;
}

void
MeshC::set_two_sided( bool bState )
{
	m_bTwoSided = bState;
}

const ColorC&
MeshC::get_ambient() const
{
	return m_rAmbient;
}

const ColorC&
MeshC::get_diffuse() const
{
	return m_rDiffuse;
}

const ColorC&
MeshC::get_specular() const
{
	return m_rSpecular;
}

float32
MeshC::get_shininess()
{
	return m_f32Shininess;
}

bool
MeshC::get_two_sided() const
{
	return m_bTwoSided;
}

