//-------------------------------------------------------------------------
//
// File:		FishingPondC.cpp
// Desc:		Fishing pond class (see also: WaterC).
// Author:		memon <memon@inside.org>
//
//-------------------------------------------------------------------------
//	Copyright (c) 2002 Moppi Productions. All Rights Reserved.
//  This file is part of Eskimo Fishing Trip game. For conditions of 
//  distribution and use, see the accompanying license.txt file.
//  http://moppi.inside.org/
//-------------------------------------------------------------------------

#ifdef WIN32
#include <windows.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include "PajaTypes.h"
#include "Vector3C.h"
#include "FishC.h"
#include "FishingContextC.h"
#include "FishingPondC.h"
#include "SceneC.h"
#include "FishMeshC.h"
#include "WaterC.h"


using namespace PajaTypes;


//
// Debug printf...
//
#if 0
static
void
TRACE( const char* szFormat, ...  )
{
	char	szMsg[256];
	va_list	rList;
	va_start( rList, szFormat );
	_vsnprintf( szMsg, 255, szFormat, rList );
	va_end( rList );
	OutputDebugString( szMsg );
}
#endif

FishingPondC::FishingPondC() :
	m_ui32Rows( 0 ),
	m_ui32Cols( 0 ),
	m_f32FreqBase( 0 ),
	m_f32FreqVar( 0 ),
	m_f32MinRad( 0 ),
	m_f32MaxRad( 0 ),
	m_pPondScene( 0 ),
	m_pBgTexture( 0 )
{
	m_pFishMesh[0] = 0;
	m_pFishMesh[1] = 0;
	m_pFishMesh[2] = 0;
}

FishingPondC::~FishingPondC()
{
	for( uint32 i = 0; i < m_rFish.size(); i++ )
		delete m_rFish[i];
	m_rFish.clear();

	delete m_pFishMesh[0];
	delete m_pFishMesh[1];
	delete m_pFishMesh[2];

	delete m_pPondScene;
	delete m_pBgTexture;
}

	
uint32
FishingPondC::hit_test( const Vector3C& rPos, float32 f32PartRadius, Vector3C& rForce )
{
	uint32	ui32Hit = PONDHIT_NONE;

	// zero force
	rForce = Vector3C( 0, 0, 0 );

	// calc distance in xz-plane
	Vector3C	rDistVec = rPos;
	rDistVec[1] = 0;
	float32		f32Dist = rDistVec.length();
	rDistVec = rDistVec.normalize();

	// Center cylinder
	if( f32Dist < m_f32MinRad ) {
		ui32Hit = PONDHIT_LAND;
		rForce += rDistVec * (m_f32MinRad - f32Dist);
	}

	if( f32Dist < m_f32MaxRad ) {
		// Pond
		if( rPos[1] < m_f32WaterHeight ) {
			ui32Hit = PONDHIT_WATER;

			// collisions force
			rForce += Vector3C( 0, (m_f32WaterHeight - rPos[1]) * 5.0f, 0 );

			// add the stream force
			Vector3C	rStreamForce;
			rStreamForce[0] = -rDistVec[2];
			rStreamForce[1] = 0;
			rStreamForce[2] = rDistVec[0];
			rForce += rStreamForce * m_f32StreamCurrent;
		}
	}
	else {
		// land
		if( rPos[1] < m_f32LandHeight ) {
			ui32Hit = PONDHIT_LAND;
			rForce += Vector3C( 0, (m_f32LandHeight - rPos[1]) * 5.0f, 0 );
		}
		// dont let the bate go further than plane z = 0 on land
		if( rPos[2] < 0 ) {
			rForce += Vector3C( 0, 0, (0.0f - rPos[2]) * 5.0f );
		}
	}

	return ui32Hit;
}


void
FishingPondC::set_pond_mesh( const char* szName )
{
	delete m_pPondScene;
	m_pPondScene = new SceneC;
	if( !m_pPondScene->load( szName ) ) {
		delete m_pPondScene;
		m_pPondScene = 0;
	}
}

void
FishingPondC::set_fish_mesh( uint32 ui32Idx, const char* szName )
{
	if( ui32Idx >= 3 )
		return;

	delete m_pFishMesh[ui32Idx];
	m_pFishMesh[ui32Idx] = new FishMeshC;
	if( !m_pFishMesh[ui32Idx]->load( szName ) ) {
		delete m_pFishMesh[ui32Idx];
		m_pFishMesh[ui32Idx] = 0;
	}
}

void
FishingPondC::set_bg_image( const char* szName )
{
	delete m_pBgTexture;
	m_pBgTexture = new TextureC;
	if( !m_pBgTexture->load( szName ) ) {
		delete m_pBgTexture;
		m_pBgTexture = 0;
	}
}

void
FishingPondC::bind_bg_image()
{
	if( m_pBgTexture )
		m_pBgTexture->bind();
}

void
FishingPondC::set_fog_start( float32 f32Val )
{
	m_f32FogStart = f32Val;
}

void
FishingPondC::set_fog_end( float32 f32Val )
{
	m_f32FogEnd = f32Val;
}

void
FishingPondC::set_fog_color( const ColorC& rCol )
{
	m_rFogColor = rCol;
}

float32
FishingPondC::get_fog_start() const
{
	return m_f32FogStart;
}

float32
FishingPondC::get_fog_end() const
{
	return m_f32FogEnd;
}

ColorC
FishingPondC::get_fog_color()
{
	return m_rFogColor;
}

uint32
FishingPondC::get_fish_count() const
{
	return m_rFish.size();
}

FishC*
FishingPondC::get_fish( uint32 ui32Idx )
{
	if( ui32Idx < m_rFish.size() )
		return m_rFish[ui32Idx];
	return 0;
}

uint32
FishingPondC::get_alive_fish_count()
{
	uint32 ui32Count = 0;
	for( uint32 i = 0; i < m_rFish.size(); i++ ) {
		if( m_rFish[i]->get_state() != FISHSTATE_EATEN )
			ui32Count++;
	}
	return ui32Count;
}


void
FishingPondC::set_fish_row_count( uint32 ui32Rows )
{
	if( ui32Rows < 1 )
		ui32Rows = 1;
	if( ui32Rows > 3 )
		ui32Rows = 3;

	m_ui32Rows = ui32Rows;
}

void
FishingPondC::set_fish_col_count( uint32 ui32Cols )
{
	if( ui32Cols < 1 )
		ui32Cols = 1;
	if( ui32Cols > 10 )
		ui32Cols = 10;

	m_ui32Cols = ui32Cols;
}

void
FishingPondC::set_fish_base_freq( float32 f32Freq )
{
	m_f32FreqBase = f32Freq;
}

void
FishingPondC::set_fish_freq_variation( float32 f32Freq )
{
	m_f32FreqVar = f32Freq;
}

void
FishingPondC::set_min_radius( float32 f32Rad )
{
	m_f32MinRad = f32Rad;
}

void
FishingPondC::set_max_radius( float32 f32Rad )
{
	m_f32MaxRad = f32Rad;
}

void
FishingPondC::set_water_height( float32 f32Val )
{
	m_f32WaterHeight = f32Val;
}

void
FishingPondC::set_land_height( float32 f32Val )
{
	m_f32LandHeight = f32Val;
}

void
FishingPondC::set_stream_current( float32 f32Val )
{
	m_f32StreamCurrent = f32Val;
}


static
float32
frand()
{
	return ((float32)rand() / (float32)RAND_MAX);
}

void
FishingPondC::init( FishingContextC* pContext )
{
	const float32	f32FishRad = 15.0f;

	float32	f32DeltaA = (1.0f / (float32)m_ui32Cols) * 2.0f * M_PI;
	float32	f32Space = (m_f32MaxRad - m_f32MinRad) / 4.0f;
	float32	f32InnerDist = m_f32MinRad + f32Space;
	float32	f32MiddleDist = m_f32MinRad + f32Space * 2.0f;
	float32	f32OuterDist = m_f32MinRad + f32Space * 3.0f;

	int32	i32MeshCount[3] = { 0, 0, 0 };
	int32	i32MeshIdx;
	int32	i32MaxMesh = 0;
	int32	i32TotalFish = m_ui32Cols * m_ui32Rows;

	if( m_pFishMesh[0] ) i32MaxMesh++;
	if( m_pFishMesh[1] ) i32MaxMesh++;
	if( m_pFishMesh[2] ) i32MaxMesh++;

	if( m_pFishMesh[0] )
		i32MeshCount[0] = (i32TotalFish / i32MaxMesh) + 1;
	if( m_pFishMesh[1] )
		i32MeshCount[1] = (i32TotalFish / i32MaxMesh) + 1;
	if( m_pFishMesh[2] )
		i32MeshCount[2] = (i32TotalFish / i32MaxMesh) + 1;

	for( uint32 i = 0; i < m_ui32Cols; i++ ) {
		float32 f32A = (float32)i * f32DeltaA;

		FishC*	pFish;

		if( m_ui32Rows == 2 || m_ui32Rows == 3 ) {

			i32MeshIdx = -1;
			while( i32MeshIdx == -1 ) {
				i32MeshIdx = rand() % 3;
				// make sure the mesh exists
				if( !m_pFishMesh[i32MeshIdx] ) {
					i32MeshIdx = -1;
					continue;
				}
				if( i32MeshCount[i32MeshIdx] == 0 ) {
					i32MeshIdx = -1;
					continue;
				}
				i32MeshCount[i32MeshIdx]--;
			}

			// inner round
			pFish = new FishC;
			pFish->init( f32FishRad, f32A, f32InnerDist, m_f32FreqBase + frand() * m_f32FreqVar, i32MeshIdx );
			m_rFish.push_back( pFish );
		}

		if( m_ui32Rows == 1 || m_ui32Rows == 3 ) {

			i32MeshIdx = -1;
			while( i32MeshIdx == -1 ) {
				i32MeshIdx = rand() % 3;
				// make sure the mesh exists
				if( !m_pFishMesh[i32MeshIdx] ) {
					i32MeshIdx = -1;
					continue;
				}
				if( i32MeshCount[i32MeshIdx] == 0 ) {
					i32MeshIdx = -1;
					continue;
				}
				i32MeshCount[i32MeshIdx]--;
			}

			// middle round
			pFish = new FishC;
			pFish->init( f32FishRad, f32A + 0.5f * f32DeltaA, f32MiddleDist, m_f32FreqBase + frand() * m_f32FreqVar, i32MeshIdx );
			m_rFish.push_back( pFish );
		}

		if( m_ui32Rows == 2 || m_ui32Rows == 3 ) {

			i32MeshIdx = -1;
			while( i32MeshIdx == -1 ) {
				i32MeshIdx = rand() % 3;
				// make sure the mesh exists
				if( !m_pFishMesh[i32MeshIdx] ) {
					i32MeshIdx = -1;
					continue;
				}
				if( i32MeshCount[i32MeshIdx] == 0 ) {
					i32MeshIdx = -1;
					continue;
				}
				i32MeshCount[i32MeshIdx]--;
			}

			// outer round
			pFish = new FishC;
			pFish->init( f32FishRad, f32A, f32OuterDist, m_f32FreqBase + frand() * m_f32FreqVar, i32MeshIdx );
			m_rFish.push_back( pFish );
		}
	}

}

void
FishingPondC::update( FishingContextC* pContext )
{
	int32	i;

	for( i = 0; i < m_rFish.size(); i++ ) {
		m_rFish[i]->update( pContext );
	}
}

SceneC*
FishingPondC::get_pond_scene() const
{
	return m_pPondScene;
}

void
FishingPondC::draw( FishingContextC* pContext )
{
	if( m_pPondScene )
		m_pPondScene->draw();

	// draw the fish
	if( m_pFishMesh ) {
		for( uint32 i = 0; i < m_rFish.size(); i++ ) {
			FishC*	pFish = m_rFish[i];
			if( pFish->get_state() == FISHSTATE_EATEN )
				continue;
			else {
				QuatC	rRot;
				rRot.from_axis_angle( 0, 1, 0, pFish->get_rot_angle() );

				FishMeshC*	pMesh = m_pFishMesh[pFish->get_mesh_idx()];

				bool	bGlow = false;
				if( pFish->get_draw_state() == FISHDRAWSTATE_GLOW )
					bGlow = true;

				if( pMesh ) {
					if( pFish->get_state() == FISHSTATE_FLYAWAY )
						pMesh->draw( pFish->get_mouth_angle(), pFish->get_pos(), pFish->get_tgt_pos(), rRot, true, bGlow );
					else
						pMesh->draw( pFish->get_mouth_angle(), pFish->get_pos(), pFish->get_tgt_pos(), rRot, false, bGlow );
				}
			}
		}
	}

}
