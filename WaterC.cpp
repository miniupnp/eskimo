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

#ifdef WIN32
#include <windows.h>
#endif
#include <GL/gl.h>
#include <GL/glu.h>
#include "PajaTypes.h"
#include "Vector3C.h"
#include "ColorC.h"
#include "TextureC.h"
#include "WaterC.h"
#include "CameraC.h"
#include "glextensions.h"


using namespace PajaTypes;


WaterC::WaterC() :
	m_ui32Slices( 0 ),
	m_ui32Stacks( 0 ),
	m_ui32HeightSource( 0 ),
	m_f32Radius1( 0 ),
	m_f32Radius2( 0 ),
	m_f32MaxAmp( 0 ),
	m_f32Time( 0 ),
	m_pEnvTexture( 0 ),
	m_ui32ReflTexWidth( 256 ),
	m_ui32ReflTexHeight( 256 ),
	m_ui32ReflTexId( 0 ),
	m_rDiffuse( 1, 1, 1, 1 ),
	m_bDrawReflections( true )
{
	// empty
}

WaterC::~WaterC()
{
	// empty
	delete m_pEnvTexture;
	if( m_ui32ReflTexId )
		glDeleteTextures( 1, &m_ui32ReflTexId );
}

// slice = angle increases
uint32
WaterC::get_slices() const
{
	return m_ui32Slices;
}

// stack = radius increases
uint32
WaterC::get_stacks() const
{
	return m_ui32Stacks;
}

void
WaterC::set_water_color1( const ColorC& rCol )
{
	m_rWaterColor1 = rCol;
}

void
WaterC::set_water_color2( const ColorC& rCol )
{
	m_rWaterColor2 = rCol;
}

ColorC
WaterC::get_water_color1() const
{
	return m_rWaterColor1;
}

ColorC
WaterC::get_water_color2() const
{
	return m_rWaterColor2;
}

void
WaterC::set_environment_texture( const char* szName )
{
	delete m_pEnvTexture;
	m_pEnvTexture = new TextureC;
	if( !m_pEnvTexture->load( szName ) ) {
		delete m_pEnvTexture;
		m_pEnvTexture = 0;
	}
}

void
WaterC::set_maximum_amplitude( float32 f32Amp )
{
	m_f32MaxAmp = f32Amp;
}

// call only once before initialisation
void
WaterC::set_min_radius( float32 f32Rad )
{
	m_f32Radius1 = f32Rad;
}

void
WaterC::set_max_radius( float32 f32Rad )
{
	m_f32Radius2 = f32Rad;
}

// call only once before initialisation
void
WaterC::set_grid_size( uint32 ui32Slices, uint32 ui32Stacks )
{
	m_ui32Slices = ui32Slices;
	m_ui32Stacks = ui32Stacks;
}

void
WaterC::set_diffuse( const ColorC& rCol )
{
	m_rDiffuse = rCol;
}

void
WaterC::set_specular( const ColorC& rCol )
{
	m_rSpecular = rCol;
}

void
WaterC::set_reflection_tex_size( uint32 ui32Width, uint32 ui32Height )
{
	m_ui32ReflTexWidth = ui32Width;
	m_ui32ReflTexHeight = ui32Height;
}

uint32
WaterC::get_reflection_tex_width() const
{
	return m_ui32ReflTexWidth;
}

uint32
WaterC::get_reflection_tex_height() const
{
	return m_ui32ReflTexHeight;
}

void
WaterC::set_draw_reflections( bool bState )
{
	m_bDrawReflections = bState;
}

bool
WaterC::get_draw_reflections() const
{
	return m_bDrawReflections;
}

void
WaterC::init()
{
	if( m_ui32Slices < 2 )
		m_ui32Slices = 2;
	if( m_ui32Stacks < 2 )
		m_ui32Stacks = 2;

	m_rPosition.resize( m_ui32Slices * m_ui32Stacks * 3 );
	m_rNormals.resize( m_ui32Slices * m_ui32Stacks * 3 );
	m_rHeight[0].resize( m_ui32Slices * m_ui32Stacks );
	m_rHeight[1].resize( m_ui32Slices * m_ui32Stacks );

	int32	i, j;

	m_f32Time = 0;

	// zero velocities
	m_ui32HeightSource = 0;
	for( i = 0; i < m_ui32Slices * m_ui32Stacks; i++ ) {
		m_rHeight[0][i] = 0.0f;
		m_rHeight[1][i] = 0.0f;
	}

	// calc points
	uint32	ui32Idx = 0;
	for( i = 0; i < m_ui32Stacks; i++ ) {
		float32	f32Rad = m_f32Radius1 + (m_f32Radius2 - m_f32Radius1) * ((float32)i / (float32)(m_ui32Stacks - 1));
		for( j = 0; j < m_ui32Slices; j++ ) {
			float32	f32Angle = ((float32)j / (float32)m_ui32Slices) * M_PI * 2.0f;
			m_rPosition[ui32Idx + 0] = cos( f32Angle ) * f32Rad;
			m_rPosition[ui32Idx + 1] = 0.0f;
			m_rPosition[ui32Idx + 2] = sin( f32Angle ) * f32Rad;
			ui32Idx += 3;
		}
	}

	// calc index list

	int32	i32IndexCount = ((int32)m_ui32Stacks - 1) * ((int32)(m_ui32Slices + 1) * 2);

	m_rIndices.resize( i32IndexCount );

	int32	i32Idx = 0;
	int32	i32CurStack, i32NextStack;
	int32	i32MaxSliceIdx = (int32)m_ui32Slices - 1;
	int32	i32MaxStackIdx = (int32)m_ui32Stacks - 1;

	for( i = 0; i < ((int32)m_ui32Stacks - 1); i++ ) {
		// calc stack indices
		i32CurStack = i * (int32)m_ui32Slices;
		i32NextStack = (i + 1) * (int32)m_ui32Slices;

		// wrap slice indices
		m_rIndices[i32Idx++] = i32MaxSliceIdx + i32NextStack;
		m_rIndices[i32Idx++] = i32MaxSliceIdx + i32CurStack;
		for( j = 0; j < (int32)m_ui32Slices; j++ ) {
			m_rIndices[i32Idx++] = j + i32NextStack;
			m_rIndices[i32Idx++] = j + i32CurStack;
		}
	}

	// create reflection texture
	glGenTextures( 1, &m_ui32ReflTexId );
	glBindTexture( GL_TEXTURE_2D, m_ui32ReflTexId );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );

	glCopyTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA8, 0, 0, m_ui32ReflTexWidth, m_ui32ReflTexHeight, 0 );
}


uint32
WaterC::get_point_count() const
{
	return m_rPosition.size();
}

float32*
WaterC::get_points_ptr()
{
	return &(m_rPosition[0]);
}

float32*
WaterC::get_normals_ptr()
{
	return &(m_rNormals[0]);
}

uint32
WaterC::get_index_count() const
{
	return m_rIndices.size();
}

uint32*
WaterC::get_indices_ptr()
{
	return &(m_rIndices[0]);
}

void
WaterC::impact( const Vector3C& rPoint, float32 f32Force )
{
	// check if the impact is needed at all
	if( rPoint[1] > 3 )
		return;

	Vector3C	rDir;
	rDir = rPoint;
	rDir[1] = 0;
	float32	f32Length = rDir.length();
	if( f32Length < m_f32Radius1 || f32Length > m_f32Radius2 )
		return;

	// Find angle
	float32	f32Angle = atan2( rDir[2], rDir[0] );

	float32	f32StackVal = ((f32Length - m_f32Radius1) / (m_f32Radius2 - m_f32Radius1)) * (float32)m_ui32Stacks;
	float32	f32SliceVal = f32Angle / (M_PI * 2.0f) * (float32)m_ui32Slices;

	while( f32SliceVal < 0.0f ) f32SliceVal += (float32)m_ui32Slices;
	while( f32SliceVal >= (float32)m_ui32Slices ) f32SliceVal += (float32)m_ui32Slices;
	if( f32StackVal < 0.0f ) f32StackVal = 0.0f;
	if( f32StackVal >= (float32)m_ui32Stacks ) f32StackVal = (float32)m_ui32Stacks;

	int32	i32Stack = (int32)floor( f32StackVal );
	int32	i32Slice = (int32)floor( f32SliceVal );

	assert( i32Stack >= 0 && i32Stack < (int32)m_ui32Stacks );
	assert( i32Slice >= 0 && i32Slice < (int32)m_ui32Slices );

	int32	i32NextStack = i32Stack + 1;
	int32	i32NextSlice = i32Slice + 1;
	if( i32NextStack >= (int32)m_ui32Stacks ) i32NextStack = (int32)m_ui32Stacks - 1;
	if( i32NextSlice >= (int32)m_ui32Slices ) i32NextSlice = 0;

	float32	f32AStack = (f32StackVal - (float32)i32Stack);
	float32	f32ASlice = (f32SliceVal - (float32)i32Slice);

	m_rHeight[m_ui32HeightSource][i32Slice + i32Stack * m_ui32Slices] += f32Force * (1.0f - f32ASlice) * (1.0f - f32AStack);
	m_rHeight[m_ui32HeightSource][i32NextSlice + i32Stack * m_ui32Slices] += f32Force * f32ASlice * (1.0f - f32AStack);
	m_rHeight[m_ui32HeightSource][i32Slice + i32NextStack * m_ui32Slices] += f32Force * (1.0f - f32ASlice) * f32AStack;
	m_rHeight[m_ui32HeightSource][i32NextSlice + i32NextStack * m_ui32Slices] += f32Force * f32ASlice * f32AStack;
}


void
WaterC::calc_normals()
{
	int32	i, j;

	int32	i32CurStack, i32CurSlice;
	int32	i32NextSlice, i32NextStack;
	int32	i32MaxStackIdx = (int32)m_ui32Stacks - 1;
	int32	i32MaxSliceIdx = (int32)m_ui32Slices - 1;

	// zero normals
	for( i = 0; i < m_rNormals.size(); i++ )
		m_rNormals[i] = 0;

	for( i = 0; i < ((int32)m_ui32Stacks - 1); i++ ) {
		// calc stack indices
		i32CurStack = i * (int32)m_ui32Slices;
		i32NextStack = (i + 1) * m_ui32Slices;

		for( j = 0; j < (int32)m_ui32Slices; j++ ) {
			// wrap slice indices
			i32CurSlice = j;
			i32NextSlice = (j == i32MaxSliceIdx) ? 0 : j + 1;

			// build triangles and calc normals
			Vector3C	rA, rB, rNorm;

			// Vertices
			int32	i32A = (i32CurSlice + i32CurStack) * 3;
			int32	i32B = (i32NextSlice + i32CurStack) * 3;
			int32	i32C = (i32NextSlice + i32NextStack) * 3;
			int32	i32D = (i32CurSlice + i32NextStack) * 3;

			// tri 1 (0, 1, 2)
			rA[0] = m_rPosition[i32B + 0] - m_rPosition[i32A + 0];
			rA[1] = m_rPosition[i32B + 1] - m_rPosition[i32A + 1];
			rA[2] = m_rPosition[i32B + 2] - m_rPosition[i32A + 2];
			rB[0] = m_rPosition[i32C + 0] - m_rPosition[i32A + 0];
			rB[1] = m_rPosition[i32C + 1] - m_rPosition[i32A + 1];
			rB[2] = m_rPosition[i32C + 2] - m_rPosition[i32A + 2];
			rNorm = rA.cross( rB );
			rNorm = rNorm.normalize();
			m_rNormals[i32A + 0] += rNorm[0];
			m_rNormals[i32A + 1] += rNorm[1];
			m_rNormals[i32A + 2] += rNorm[2];

			m_rNormals[i32B + 0] += rNorm[0];
			m_rNormals[i32B + 1] += rNorm[1];
			m_rNormals[i32B + 2] += rNorm[2];

			m_rNormals[i32C + 0] += rNorm[0];
			m_rNormals[i32C + 1] += rNorm[1];
			m_rNormals[i32C + 2] += rNorm[2];

			// tri 2 (0, 2, 3)

			rA[0] = m_rPosition[i32C + 0] - m_rPosition[i32A + 0];
			rA[1] = m_rPosition[i32C + 1] - m_rPosition[i32A + 1];
			rA[2] = m_rPosition[i32C + 2] - m_rPosition[i32A + 2];
			rB[0] = m_rPosition[i32D + 0] - m_rPosition[i32A + 0];
			rB[1] = m_rPosition[i32D + 1] - m_rPosition[i32A + 1];
			rB[2] = m_rPosition[i32D + 2] - m_rPosition[i32A + 2];
			rNorm = rA.cross( rB );
			rNorm = rNorm.normalize();
			m_rNormals[i32A + 0] += rNorm[0];
			m_rNormals[i32A + 1] += rNorm[1];
			m_rNormals[i32A + 2] += rNorm[2];

			m_rNormals[i32C + 0] += rNorm[0];
			m_rNormals[i32C + 1] += rNorm[1];
			m_rNormals[i32C + 2] += rNorm[2];
			
			m_rNormals[i32D + 0] += rNorm[0];
			m_rNormals[i32D + 1] += rNorm[1];
			m_rNormals[i32D + 2] += rNorm[2];
		}
	}

	// Normalize normals
	for( i = 0; i < m_rNormals.size(); i += 3 ) {

		float32  d = (m_rNormals[i + 0] * m_rNormals[i + 0]) + (m_rNormals[i + 1] * m_rNormals[i + 1]) + (m_rNormals[i + 2] * m_rNormals[i + 2]);
		if( d != 0.0 ) {
			float32	ood;
			ood = 1.0f / (float32)sqrt( d );
			m_rNormals[i + 0] *= ood;
			m_rNormals[i + 1] *= ood;
			m_rNormals[i + 2] *= ood;
		}
//			m_rNormals[i] = m_rNormals[i].normalize();
	}
}

void
WaterC::eval( float32 f32DeltaTime )
{
	int32	i, j;

	// Calc vertices
	for( i = 0; i < m_rPosition.size() / 3; i++ ) {
		float32	f32Val = m_rHeight[m_ui32HeightSource][i];
		if( f32Val < -1 ) f32Val = -1;
		if( f32Val > 1 ) f32Val = 1;
		m_rPosition[(i * 3) + 1] = f32Val * m_f32MaxAmp;
	}

	int32	i32CurStack, i32CurSlice;
	int32	i32NextStack, i32NextSlice;
	int32	i32PrevStack, i32PrevSlice;
	int32	i32MaxStackIdx = (int32)m_ui32Stacks - 1;
	int32	i32MaxSliceIdx = (int32)m_ui32Slices - 1;

	uint32	ui32From, ui32To;
	ui32From = m_ui32HeightSource;
	ui32To = m_ui32HeightSource ^ 1;

	for( i = 0; i < (int32)m_ui32Stacks; i++ ) {
		// calc stack indices
		i32CurStack = i * (int32)m_ui32Slices;
		i32PrevStack = ((i == 0) ? 0 : i - 1) * m_ui32Slices;
		i32NextStack = ((i == i32MaxStackIdx) ? i32MaxStackIdx : i + 1) * m_ui32Slices;

		i32PrevSlice = i32MaxSliceIdx;
		for( j = 0; j < (int32)m_ui32Slices; j++ ) {
			// wrap slice indices
			i32CurSlice = j;
			i32NextSlice = (j == i32MaxSliceIdx) ? 0 : (j + 1);
			
			// Vertices
			//    [B]
			// [D][A][E]
			//    [C]
			int32	i32A = i32CurSlice + i32CurStack;
			int32	i32B = i32CurSlice + i32PrevStack;
			int32	i32C = i32CurSlice  + i32NextStack;
			int32	i32D = i32PrevSlice + i32CurStack;
			int32	i32E = i32NextSlice + i32CurStack;

			float32	f32A = m_rHeight[ui32From][i32A] +
							m_rHeight[ui32From][i32B] +
							m_rHeight[ui32From][i32C] +
							m_rHeight[ui32From][i32E] +
							m_rHeight[ui32From][i32D];

			f32A *= 1.0f / 3.0f;				// overshoot
			f32A -= m_rHeight[ui32To][i32A];
			m_rHeight[ui32To][i32A] = f32A - f32A * 0.09f;
			i32PrevSlice = i32CurSlice;
		}
	}

	// Swap source heightbuffer
	m_ui32HeightSource ^= 1;

	m_f32Time += f32DeltaTime;
}

void
WaterC::draw( CameraC* pCam, float32 f32Aspect )
{
	// setup material
	float32	f32Amb[4];
	float32	f32Diff[4];
	float32	f32Spec[4];
	float32	f32Emission[4];

	f32Amb[0] = 0.0f; f32Amb[1] = 0.0f; f32Amb[2] = 0.0f; f32Amb[3] = 1;
	glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT, f32Amb );

	f32Diff[0] = m_rDiffuse[0];
	f32Diff[1] = m_rDiffuse[1];
	f32Diff[2] = m_rDiffuse[2];
	f32Diff[3] = 1.0f;
	glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, f32Diff );

	f32Spec[0] = m_rSpecular[0]; f32Spec[1] = m_rSpecular[1]; f32Spec[2] = m_rSpecular[2]; f32Spec[3] = 1;
	glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, f32Spec );

	f32Emission[0] = 0; f32Emission[1] = 0; f32Emission[2] = 0; f32Emission[3] = 1;
	glMaterialfv( GL_FRONT_AND_BACK, GL_EMISSION, f32Emission );

	glMateriali( GL_FRONT_AND_BACK, GL_SHININESS, 10 );


	// reflection texture
	if( m_bDrawReflections ) {
		glActiveTextureARB( GL_TEXTURE0_ARB );
		glClientActiveTextureARB( GL_TEXTURE0_ARB );
		glBindTexture( GL_TEXTURE_2D, m_ui32ReflTexId );
		glEnable( GL_TEXTURE_2D );
		glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
//			glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL );

		setup_texture_projection( pCam, f32Aspect );
	}


	// some spherical environment stuff (looks anisotrophic enough...)
	if( m_pEnvTexture ) {
		glActiveTextureARB( GL_TEXTURE1_ARB );
		glClientActiveTextureARB( GL_TEXTURE1_ARB );
		glEnable( GL_TEXTURE_2D );
		m_pEnvTexture->bind();
		glTexGeni( GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP );
		glTexGeni( GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP );
		glEnable( GL_TEXTURE_GEN_S );
		glEnable( GL_TEXTURE_GEN_T );

//			glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
		glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_ADD );
	}


	calc_normals();


	uint32*		pIndices = get_indices_ptr();
	int32		i32IdxCount = (m_ui32Slices + 1) * 2;

	glActiveTextureARB( GL_TEXTURE0_ARB );
	glClientActiveTextureARB( GL_TEXTURE0_ARB );

	glEnableClientState( GL_VERTEX_ARRAY );
	glEnableClientState( GL_NORMAL_ARRAY );

	glVertexPointer( 3, GL_FLOAT, 3 * sizeof( float32 ), get_points_ptr() );
	glNormalPointer( GL_FLOAT, 3 * sizeof( float32 ), get_normals_ptr() );

	if( m_bDrawReflections ) {
		glEnableClientState( GL_TEXTURE_COORD_ARRAY );
		glTexCoordPointer( 3, GL_FLOAT, 3 * sizeof( float32 ), get_points_ptr() );
	}

	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	for( int32 i = 0; i < ((int32)m_ui32Stacks - 1); i++ ) {
		glDrawElements( GL_TRIANGLE_STRIP, i32IdxCount, GL_UNSIGNED_INT, pIndices );
		pIndices += i32IdxCount;
	}

	glBlendFunc( GL_ONE, GL_ZERO );

	glDisableClientState( GL_VERTEX_ARRAY );
	glDisableClientState( GL_NORMAL_ARRAY );

	glDisableClientState( GL_TEXTURE_COORD_ARRAY );

	// disable second texture stage
	glActiveTextureARB( GL_TEXTURE1_ARB );
	glClientActiveTextureARB( GL_TEXTURE1_ARB );
	glDisable( GL_TEXTURE_GEN_S );
	glDisable( GL_TEXTURE_GEN_T );
	glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
	glDisable( GL_TEXTURE_2D );

	// Reset texture matrix
	glActiveTextureARB( GL_TEXTURE0_ARB );
	glClientActiveTextureARB( GL_TEXTURE0_ARB );
	glDisable( GL_TEXTURE_2D );
	restore_texture_projection();

	glActiveTextureARB( GL_TEXTURE0_ARB );
	glClientActiveTextureARB( GL_TEXTURE0_ARB );
}

void
WaterC::setup_texture_projection( CameraC* pCam, float32 f32Aspect )
{
	Vector3C	rPos = pCam->get_position();
	Vector3C	rTgt = pCam->get_target_position();
	float		f32FOV = pCam->get_fov();

	glMatrixMode( GL_TEXTURE );
	glLoadIdentity();

	glTranslatef( 0.5f, 0.5f, 0.0f );	// Center texture
	glScalef( 0.5f, 0.5f, 1.0f );		// Scale and bias

	gluPerspective( f32FOV / M_PI * 180.0f / f32Aspect, f32Aspect, 1, pCam->get_far_plane() );

	// Mirror around plane y = 0
	gluLookAt( rPos[0], -rPos[1], rPos[2], rTgt[0], -rTgt[1], rTgt[2], 0, -1, 0 );

	// scale the texture coords, so that the reflections are more lively.
	glScalef( 1, 2, 1 );

	glMatrixMode( GL_MODELVIEW );
}

void
WaterC::restore_texture_projection()
{
	glMatrixMode( GL_TEXTURE );
	glLoadIdentity();
	glMatrixMode( GL_MODELVIEW );
}

void
WaterC::begin_reflections()
{
	float64 f64ClipPlane[] = { 0, 1, 0, 3 };	// push the clipping plane a bit to disable the nasty
												// cracks of the clipped objects, when the water is deformed.
	glEnable( GL_CLIP_PLANE0 );
	glClipPlane( GL_CLIP_PLANE0, f64ClipPlane );
}

void
WaterC::end_reflections()
{
	glDisable( GL_CLIP_PLANE0 );

	// copy reflection to texture
	glBindTexture( GL_TEXTURE_2D, m_ui32ReflTexId );
	glCopyTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, 0, 0, m_ui32ReflTexWidth, m_ui32ReflTexHeight );

	glBindTexture( GL_TEXTURE_2D, 0 );
}
