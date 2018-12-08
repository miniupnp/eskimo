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

#include <windows.h>
#include <stdio.h>
#include <string.h>
#include "glextensions.h"
#include <gl/gl.h>
#include <gl/glu.h>
#include "PajaTypes.h"
#include "Vector3C.h"
#include "FishingPondC.h"
#include "ColorC.h"
#include "FishingContextC.h"
#include "fmod.h"
#include "GLWindowC.h"

using namespace PajaTypes;


//
// Debug printf...
//
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



FishingContextC::FishingContextC() :
	m_f32Frame( 0 ),
	m_f32Aspect( 1.0f ),
	m_i32Timer( 0 ),
	m_pPlayer( 0 ),
	m_bRenderReflections( true ),
	m_i32Score( 0 ),
	m_i32MaxBaitCount( 0 )
{
	m_pPond = new FishingPondC;
	m_pWater = new WaterC;

	for( uint32 i = 0; i < MAX_SOUNDS; i++ )
		m_rSoundFx[i] = 0;
}

FishingContextC::~FishingContextC()
{
	delete m_pPond;
	delete m_pWater;
}


void
FishingContextC::set_rendering_detail( uint32 ui32Level )
{
}

uint32
FishingContextC::get_rendering_detail() const
{
	return 0;
}


char*
extract_string( char* szBuf )
{
	if( !szBuf || !*szBuf )
		return 0;

	static char		szWord[512];

	char*	szSrc = szBuf;
	char*	szDst = szWord;

	// find first delimit
	while( *szSrc && *szSrc != '\"' )
		szSrc++;

	// step over the first delimiter
	szSrc++;

	// find second delimit and copy the between to szWord
	while( *szSrc && *szSrc != '\"' )
		*szDst++ = *szSrc++;

	// terminate the string
	*szDst = '\0';

	return szWord;
}

ColorC
FishingContextC::get_level_brief_color()
{
	return m_rBriefColor;
}

const char*
FishingContextC::get_level_brief_name()
{
	return m_sBriefName.c_str();
}
const char*
FishingContextC::get_level_brief_desc()
{
	return m_sBriefDesc.c_str();
}

int32
FishingContextC::get_timer()
{
	return m_i32Timer;
}

void
FishingContextC::init_level()
{
	if( m_pPlayer )
		m_pPlayer->init_baits( m_i32MaxBaitCount );
}

bool
FishingContextC::load_level( const char* szName )
{
	char	szRow[512];

	FILE*	pStream = fopen( szName, "r" );
	if( !pStream ) {
		TRACE( "cannot open scene file\n" );
		return false;
	}


	// delete old data
	delete m_pPond;
	m_pPond = new FishingPondC;

	delete m_pWater;
	m_pWater = new WaterC;

	// clear brief
	m_sBriefName = "";
	m_sBriefDesc = "";
	m_rBriefColor = ColorC( 0, 0, 0, 0 );

	m_i32MaxBaitCount = 0;

	while( !feof( pStream ) ) {
		fgets( szRow, 511, pStream );

		if( feof( pStream ) )
			break;

		if( szRow[0] == '\0' || szRow[0] == '#' )	// Skip comments and empty rows
			continue;

		float32	f32Val;
		uint32	ui32R, ui32G, ui32B, ui32A;
		int32	i32Val;

		if( strncmp( "scene", szRow, 5 ) == 0 ) {
			m_pPond->set_pond_mesh( extract_string( szRow ) );
		}
		else if( strncmp( "brief_name", szRow, 10 ) == 0 ) {
			m_sBriefName = extract_string( szRow );
		}
		else if( strncmp( "brief_desc", szRow, 10 ) == 0 ) {
			m_sBriefDesc += extract_string( szRow );
			m_sBriefDesc += "\n";
		}
		else if( strncmp( "brief_color", szRow, 11 ) == 0 ) {
			sscanf( szRow, "%*s %d %d %d %d", &ui32R, &ui32G, &ui32B, &ui32A );
			m_rBriefColor.convert_from_uint8( ui32R, ui32G, ui32B, ui32A );
		}
		else if( strncmp( "fish3", szRow, 5 ) == 0 ) {
			m_pPond->set_fish_mesh( 2, extract_string( szRow ) );
		}
		else if( strncmp( "fish2", szRow, 5 ) == 0 ) {
			m_pPond->set_fish_mesh( 1, extract_string( szRow ) );
		}
		else if( strncmp( "fish", szRow, 4 ) == 0 ) {
			m_pPond->set_fish_mesh( 0, extract_string( szRow ) );
		}
		else if( strncmp( "bg_image", szRow, 8 ) == 0 ) {
			m_pPond->set_bg_image( extract_string( szRow ) );
		}
		else if( strncmp( "fog_start", szRow, 9 ) == 0 ) {
			sscanf( szRow, "%*s %f", &f32Val );
			m_pPond->set_fog_start( f32Val );
		}
		else if( strncmp( "fog_end", szRow, 7 ) == 0 ) {
			sscanf( szRow, "%*s %f", &f32Val );
			m_pPond->set_fog_end( f32Val );
		}
		else if( strncmp( "fog_color", szRow, 9 ) == 0 ) {
			sscanf( szRow, "%*s %d %d %d", &ui32R, &ui32G, &ui32B );
			ColorC	rCol;
			rCol.convert_from_uint8( ui32R, ui32G, ui32B );
			m_pPond->set_fog_color( rCol );
		}
		else if( strncmp( "water_color1", szRow, 12 ) == 0 ) {
			sscanf( szRow, "%*s %d %d %d", &ui32R, &ui32G, &ui32B );
			ColorC	rCol;
			rCol.convert_from_uint8( ui32R, ui32G, ui32B );
			m_pWater->set_water_color1( rCol );
		}
		else if( strncmp( "water_color2", szRow, 12 ) == 0 ) {
			sscanf( szRow, "%*s %d %d %d %d", &ui32R, &ui32G, &ui32B, &ui32A );
			ColorC	rCol;
			rCol.convert_from_uint8( ui32R, ui32G, ui32B, ui32A );
			m_pWater->set_water_color2( rCol );
		}
		else if( strncmp( "water_refl_image", szRow, 16 ) == 0 ) {
			m_pWater->set_environment_texture( extract_string( szRow ) );
		}
		else if( strncmp( "pond_land_height", szRow, 16 ) == 0 ) {
			sscanf( szRow, "%*s %f", &f32Val );
			m_pPond->set_land_height( f32Val );
		}
		else if( strncmp( "pond_inner", szRow, 10 ) == 0 ) {
			sscanf( szRow, "%*s %f", &f32Val );
			m_pPond->set_min_radius( f32Val );
			m_pWater->set_min_radius( f32Val );
		}
		else if( strncmp( "pond_outer", szRow, 10 ) == 0 ) {
			sscanf( szRow, "%*s %f", &f32Val );
			m_pPond->set_max_radius( f32Val );
			m_pWater->set_max_radius( f32Val );
		}
		else if( strncmp( "pond_row_count", szRow, 14 ) == 0 ) {
			sscanf( szRow, "%*s %d", &i32Val );
			m_pPond->set_fish_row_count( i32Val );
		}
		else if( strncmp( "pond_col_count", szRow, 14 ) == 0 ) {
			sscanf( szRow, "%*s %d", &i32Val );
			m_pPond->set_fish_col_count( i32Val );
		}
		else if( strncmp( "pond_base_freq", szRow, 14 ) == 0 ) {
			sscanf( szRow, "%*s %f", &f32Val );
			m_pPond->set_fish_base_freq( f32Val );
		}
		else if( strncmp( "pond_freq_var", szRow, 13 ) == 0 ) {
			sscanf( szRow, "%*s %f", &f32Val );
			m_pPond->set_fish_freq_variation( f32Val );
		}
		else if( strncmp( "timer", szRow, 5 ) == 0 ) {
			sscanf( szRow, "%*s %d", &m_i32Timer );
		}
		else if( strncmp( "bait_count", szRow, 10 ) == 0 ) {
			sscanf( szRow, "%*s %d", &m_i32MaxBaitCount );
		}
	}


	m_pWater->set_grid_size( 128, 16 );
	m_pWater->set_maximum_amplitude( 10.0f );
	m_pWater->set_diffuse( ColorC( 1.0f, 1.0f, 1.0f ) );
	m_pWater->set_specular( ColorC( 0.2f, 0.2f, 0.2f ) );
	m_pWater->set_reflection_tex_size( 512, 256 );
	m_pWater->init();

	m_pPond->set_water_height( 0 );
	m_pPond->set_stream_current( 0.5f );
	m_pPond->init( this );

	return true;
}


bool
FishingContextC::load_sound( uint32 ui32Sound, const char* szName )
{
	m_rSoundFx[ui32Sound] = FSOUND_Sample_Load( FSOUND_FREE, szName, FSOUND_NORMAL, 0 );
	return m_rSoundFx[ui32Sound] != 0;
}

void
FishingContextC::play_sound( uint32 ui32Sound )
{
//	TRACE( "FishingContextC::play_sound( %d )\n", ui32Sound );
	if( ui32Sound < MAX_SOUNDS && m_rSoundFx[ui32Sound] )
		FSOUND_PlaySound( FSOUND_FREE, m_rSoundFx[ui32Sound] );
}

FishingPondC*
FishingContextC::get_pond()
{
	return m_pPond;
}

void
FishingContextC::set_catch_height( float32 f32Height )
{
	m_f32CatchHeight = f32Height;
}

float32
FishingContextC::get_catch_height() const
{
	return m_f32CatchHeight;
}

void
FishingContextC::fish_catch( uint32 ui32Indx )
{
	TRACE( "FishingContextC::fish_catch( %d );\n", ui32Indx );
}

float32
FishingContextC::get_frame() const
{
	return m_f32Frame;
}

void
FishingContextC::set_frame( float32 f32Frame )
{
	m_f32Frame = f32Frame;
}


float32
FishingContextC::get_aspect() const
{
	return m_f32Aspect;
}

void
FishingContextC::set_aspect( float32 f32Aspect )
{
	m_f32Aspect = f32Aspect;
}


void
FishingContextC::update()
{
	if( m_pPond )
		m_pPond->update( this );

	int32	i;

	if( m_pPlayer )
		m_pPlayer->update( this );

	if( m_pWater ) {

		if( m_pPlayer ) {
			for( i = 0; i < (int32)m_pPlayer->get_particle_count() - 1; i++ )
				m_pWater->impact( m_pPlayer->get_particle_pos( i ), -0.5f );
			m_pWater->impact( m_pPlayer->get_bait_pos(), -1.5f );
		}

		for( i = 0; i < m_pPond->get_fish_count(); i++ ) {
			FishC*	pFish = m_pPond->get_fish( i );
			if( pFish->get_state() == FISHSTATE_EATEN )
				continue;

			Vector3C	rPos = pFish->get_pos();
			rPos[1] = 0;

			if( pFish->get_state() == FISHSTATE_FLYAWAY )
				m_pWater->impact( rPos, 1.5f );
			else if( pFish->get_mouth_angle() > 0.4f  )
				m_pWater->impact( rPos, pFish->get_mouth_angle() * 3.0f );
		}
		
		// evaluate water physics
		m_pWater->eval( 1.0f );
	}

	if( m_pPlayer && m_pPond && m_pPond->get_pond_scene() ) {
		SceneC*	pScene = m_pPond->get_pond_scene();
		for( i = 0; i < pScene->get_light_count(); i++ ) {
			LightC*	pLight = pScene->get_light( i );
			if( strcmp( "follow", pLight->get_name() ) == 0 ) {
				pLight->set_position( m_pPlayer->get_tip_pos() );
				pLight->set_attenuate( true );
				pLight->set_decay_start( 150 );
			}
		}
	}
}

void
FishingContextC::draw( GLWindowC* pWnd )
{
	if( !m_pPond || !m_pWater )
		return;

	glEnable( GL_FOG );
	glFogi( GL_FOG_MODE, GL_LINEAR );

	glFogf( GL_FOG_START, m_pPond->get_fog_start() );
	glFogf( GL_FOG_END, m_pPond->get_fog_end() );
	glFogfv( GL_FOG_COLOR, m_pPond->get_fog_color() );

	//
	// First draw reflections
	//

	if( m_bRenderReflections ) {

		glViewport( 0, 0, m_pWater->get_reflection_tex_width(), m_pWater->get_reflection_tex_height() );
		glEnable( GL_SCISSOR_TEST );
		glScissor( 0, 0, m_pWater->get_reflection_tex_width(), m_pWater->get_reflection_tex_height() );
		pWnd->clear_device( GRAPHICSDEVICE_ALLBUFFERS );


		// Draw background
		glMatrixMode( GL_PROJECTION );
		glLoadIdentity();
		gluOrtho2D( 0, 1, 0, 1 );

		glMatrixMode( GL_MODELVIEW );
		glLoadIdentity();

		glDisable( GL_LIGHTING );
		glDisable( GL_FOG );
		glDepthMask( GL_FALSE );
		glDisable( GL_DEPTH_TEST );
		glEnable( GL_TEXTURE_2D );
		m_pPond->bind_bg_image();
		glColor3ub( 255, 255, 255 );

		glBegin( GL_QUADS );
			glTexCoord2f( 0, 0 );
			glVertex2f( 0, 0 );

			glTexCoord2f( 1, 0 );
			glVertex2f( 1, 0 );

			glTexCoord2f( 1, 1 );
			glVertex2f( 1, 1 );

			glTexCoord2f( 0, 1 );
			glVertex2f( 0, 1 );
		glEnd();

		glDepthMask( GL_TRUE );
		glDisable( GL_TEXTURE_2D );
		glEnable( GL_DEPTH_TEST );
		glEnable( GL_LIGHTING );
		glEnable( GL_FOG );


		m_pPond->get_pond_scene()->setup_camera( 0, (float32)pWnd->get_width() / (float32)pWnd->get_height(), true );
		m_pPond->get_pond_scene()->setup_lights();

		m_pWater->begin_reflections();

		// draw environment and the fish
		m_pPond->draw( this );

		// draw the fishing rod
		if( m_pPlayer )
			m_pPlayer->draw( this );

		// Draw background
		glMatrixMode( GL_PROJECTION );
		glLoadIdentity();
		gluOrtho2D( 0, 1, 0, 1 );

		glMatrixMode( GL_MODELVIEW );
		glLoadIdentity();

		//
		// make the image bluish
		//
		glDisable( GL_CLIP_PLANE0 );
		glDisable( GL_LIGHTING );
		glDisable( GL_FOG );
		glDepthMask( GL_FALSE );
		glDisable( GL_DEPTH_TEST );

		glBlendFunc( GL_DST_COLOR, GL_ZERO );
		glColor3fv( m_pWater->get_water_color1() );
		glBegin( GL_QUADS );
			glVertex2f( 0, 0 );
			glVertex2f( 1, 0 );
			glVertex2f( 1, 1 );
			glVertex2f( 0, 1 );
		glEnd();

		glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
		glColor4fv( m_pWater->get_water_color2() );
		glBegin( GL_QUADS );
			glVertex2f( 0, 0 );
			glVertex2f( 1, 0 );
			glVertex2f( 1, 1 );
			glVertex2f( 0, 1 );
		glEnd();

		glBlendFunc( GL_ONE, GL_ZERO );

		glEnable( GL_CLIP_PLANE0 );
		glDepthMask( GL_TRUE );
		glEnable( GL_DEPTH_TEST );
		glEnable( GL_LIGHTING );
		glEnable( GL_FOG );

		m_pWater->end_reflections();
	}

	//
	// Draw the rest
	//

	glViewport( 0, 0, pWnd->get_width(), pWnd->get_height() );
	glEnable( GL_SCISSOR_TEST );
	glScissor( 0, 0, pWnd->get_width(), pWnd->get_height() );
	pWnd->clear_device( GRAPHICSDEVICE_ALLBUFFERS );


	// Draw background
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	gluOrtho2D( 0, 1, 0, 1 );

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

	glDisable( GL_LIGHTING );
	glDisable( GL_FOG );
	glDepthMask( GL_FALSE );
	glColor3ub( 255, 255, 255 );
	glEnable( GL_TEXTURE_2D );
	m_pPond->bind_bg_image();

	glBegin( GL_QUADS );
		glTexCoord2f( 0, 1 );
		glVertex2f( 0, 0 );

		glTexCoord2f( 1, 1 );
		glVertex2f( 1, 0 );

		glTexCoord2f( 1, 0 );
		glVertex2f( 1, 1 );

		glTexCoord2f( 0, 0 );
		glVertex2f( 0, 1 );
	glEnd();

	glDisable( GL_TEXTURE_2D );
	glDepthMask( GL_TRUE );
	glEnable( GL_LIGHTING );
	glEnable( GL_FOG );

	// setup scene
	m_pPond->get_pond_scene()->setup_camera( 0, (float32)pWnd->get_width() / (float32)pWnd->get_height(), false );
	m_pPond->get_pond_scene()->setup_lights();

	// draw pond surface
	if( m_bRenderReflections ) {
		m_pWater->set_diffuse( ColorC( 1, 1, 1, 1 ) );
		m_pWater->set_draw_reflections( true );
	}
	else {
		ColorC	rCol;
		ColorC	rCol1 = m_pWater->get_water_color1();
		ColorC	rCol2 = m_pWater->get_water_color2();
		float32	f32Alpha = rCol2[3];
		rCol1[3] = 1;	// make sure alpha is one
		rCol2[3] = 1;	// make sure alpha is one
		rCol = f32Alpha * rCol2 + (1 - f32Alpha) * rCol1;
		m_pWater->set_diffuse( rCol );
		m_pWater->set_draw_reflections( false );
	}
	CameraC*	pCam = m_pPond->get_pond_scene()->get_camera( 0 );
	m_pWater->draw( pCam, (float32)pWnd->get_width() / (float32)pWnd->get_height() );

	// draw environment and the fish
	m_pPond->draw( this );

	// draw the fishing rod
	if( m_pPlayer )
		m_pPlayer->draw( this );
}

void
FishingContextC::set_render_reflections( bool bState )
{
	m_bRenderReflections = bState;
}

void
FishingContextC::set_reflections_tex_size( uint32 ui32W, uint32 ui32H )
{
	m_pWater->set_reflection_tex_size( ui32W, ui32H );
}

void
FishingContextC::set_rod( FishingRodC* pRod )
{
	m_pPlayer = pRod;
}

void
FishingContextC::add_score( int32 i32Score )
{
	m_i32Score += i32Score;
}

void
FishingContextC::set_score( int32 i32Score )
{
	m_i32Score = i32Score;
}

int32
FishingContextC::get_score() const
{
	return m_i32Score;
}
