//-------------------------------------------------------------------------
//
// File:		SceneC.cpp
// Desc:		scene class.
// Author:		memon <memon@inside.org>
//
//-------------------------------------------------------------------------
//	Copyright (c) 2002 Moppi Productions. All Rights Reserved.
//  This file is part of Eskimo Fishing Trip game. For conditions of 
//  distribution and use, see the accompanying license.txt file.
//  http://moppi.inside.org/
//-------------------------------------------------------------------------

#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include "PajaTypes.h"
#include "MeshC.h"
#include "LightC.h"
#include "CameraC.h"
#include "TextureC.h"
#include "SceneC.h"
#include "ASELoaderC.h"
#include "Vector3C.h"
#include "ColorC.h"


using namespace PajaTypes;

SceneC::SceneC() :
	m_i32FPS( 0 ),
	m_i32TicksPerFrame( 0 ),
	m_i32FirstFrame( 0 ),
	m_i32LastFrame( 0 )
{
	// empty
}

SceneC::~SceneC()
{
	uint32	i;

	for( i = 0; i < m_rTextures.size(); i++ )
		delete m_rTextures[i];
	m_rTextures.clear();

	for( i = 0; i < m_rLights.size(); i++ )
		delete m_rLights[i];
	m_rLights.clear();

	for( i = 0; i < m_rCameras.size(); i++ )
		delete m_rCameras[i];
	m_rCameras.clear();

	for( i = 0; i < m_rScenegraph.size(); i++ )
		delete m_rScenegraph[i];
	m_rScenegraph.clear();
}


bool
SceneC::load( const char* szName )
{
	ASELoaderC	rLoader;

	if( !rLoader.load( szName ) ) {
		return false;
	}

	rLoader.get_cameras( m_rCameras );
	rLoader.get_lights( m_rLights );
	rLoader.get_scenegraph( m_rScenegraph );
	rLoader.get_textures( m_rTextures );

	return true;
}

uint32
SceneC::get_scenegraphitem_count() const
{
	return m_rScenegraph.size();
}

MeshC*
SceneC::get_scenegraphitem( uint32 ui32Index )
{
	assert( ui32Index < m_rScenegraph.size() );
	return m_rScenegraph[ui32Index];
}

uint32
SceneC::get_camera_count() const
{
	return m_rCameras.size();
}

CameraC*
SceneC::get_camera( uint32 ui32Index )
{
	assert( ui32Index < m_rCameras.size() );
	return m_rCameras[ui32Index];
}

uint32
SceneC::get_light_count() const
{
	return m_rLights.size();
}

LightC*
SceneC::get_light( uint32 ui32Index )
{
	assert( ui32Index < m_rLights.size() );
	return m_rLights[ui32Index];
}

uint32
SceneC::get_texture_count() const
{
	return m_rTextures.size();
}

TextureC*
SceneC::get_texture( uint32 ui32Index )
{
	assert( ui32Index < m_rTextures.size() );
	return m_rTextures[ui32Index];
}

void
SceneC::get_time_parameters( int32& i32FPS, int32& i32TicksPerFrame, int32& i32FirstFrame, int32& i32LastFrame ) const
{
	i32FPS = m_i32FPS;
	i32TicksPerFrame = m_i32TicksPerFrame;
	i32FirstFrame = m_i32FirstFrame;
	i32LastFrame = m_i32LastFrame;
}

void
SceneC::draw()
{
	for( uint32 i = 0; i < m_rScenegraph.size(); i++ )
		render_item( m_rScenegraph[i], 0 );
}

void
SceneC::render_item( MeshC* pMesh, int32 i32Frame )
{
	if( !pMesh )
		return;

	pMesh->eval_state( i32Frame );

	glPushMatrix();

	float32		f32X, f32Y, f32Z, f32A;
	Vector3C	rPos = pMesh->get_position();
	QuatC		rRot = pMesh->get_rotation();
	Vector3C	rScale = pMesh->get_scale();
	QuatC		rScaleRot = pMesh->get_scale_rotation();
	QuatC		rInvScaleRot = rScaleRot.unit_inverse();


	// Strangle kludge. The animated rotations doesnt work correctly
	// if the angle is not negated when the quat is set to OpenGL.

	glTranslatef( rPos[0], rPos[1], rPos[2] );

	rRot.to_axis_angle( f32X, f32Y, f32Z, f32A );
	glRotatef( -f32A / (float32)M_PI * 180.0f, f32X, f32Y, f32Z );

	rScaleRot.to_axis_angle( f32X, f32Y, f32Z, f32A );
	glRotatef( -f32A / (float32)M_PI * 180.0f, f32X, f32Y, f32Z );

	glScalef( rScale[0], rScale[1], rScale[2] );

	rInvScaleRot.to_axis_angle( f32X, f32Y, f32Z, f32A );
	glRotatef( -f32A / (float32)M_PI * 180.0f, f32X, f32Y, f32Z );


	//
	// draw
	//

	float32	f32Amb[4];
	float32	f32Diff[4];
	float32	f32Spec[4];

	ColorC	rAmbient = pMesh->get_ambient();
	ColorC	rDiffuse = pMesh->get_diffuse();
	ColorC	rSpecular = pMesh->get_specular();

	f32Amb[0] = rAmbient[0] * 0.25;
	f32Amb[1] = rAmbient[1] * 0.25;
	f32Amb[2] = rAmbient[2] * 0.25;
	f32Amb[3] = 1;
	glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT, f32Amb );

	f32Diff[0] = rDiffuse[0];
	f32Diff[1] = rDiffuse[1];
	f32Diff[2] = rDiffuse[2];
	f32Diff[3] = 1;
	glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, f32Diff );

	f32Spec[0] = rSpecular[0];
	f32Spec[1] = rSpecular[1];
	f32Spec[2] = rSpecular[2];
	f32Spec[3] = 1;
	glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, f32Spec );

	glMateriali( GL_FRONT_AND_BACK, GL_SHININESS, 2 + (int32)(pMesh->get_shininess() * 126.0f) );
	
	// Texture stuff

	TextureC*	pTex = pMesh->get_texture();
	bool				bHasTex = false;

	if( pTex && pMesh->get_texcoord_count()  ) {
		glEnable( GL_TEXTURE_2D );
		pTex->bind();
		bHasTex = true;
	}
	else
		glDisable( GL_TEXTURE_2D );


	glDisableClientState( GL_VERTEX_ARRAY );
	glDisableClientState( GL_NORMAL_ARRAY );
	glDisableClientState( GL_COLOR_ARRAY );
	glDisableClientState( GL_EDGE_FLAG_ARRAY );
	glDisableClientState( GL_INDEX_ARRAY );
	glDisableClientState( GL_TEXTURE_COORD_ARRAY );

	glEnableClientState( GL_VERTEX_ARRAY );
	glEnableClientState( GL_NORMAL_ARRAY );

	glVertexPointer( 3, GL_FLOAT, 3 * sizeof( float32 ), pMesh->get_vert_ptr() );
	glNormalPointer( GL_FLOAT, 3 * sizeof( float32 ), pMesh->get_norm_ptr() );

	if( bHasTex ) {
		glEnableClientState( GL_TEXTURE_COORD_ARRAY );
		glTexCoordPointer( 2, GL_FLOAT, 2 * sizeof( float32 ), pMesh->get_texcoord_ptr() );
	}

	if( pMesh->get_two_sided() )
		glDisable( GL_CULL_FACE );

	glDrawElements( GL_TRIANGLES, pMesh->get_index_count(), GL_UNSIGNED_INT, pMesh->get_index_ptr() );

	if( pMesh->get_two_sided() )
		glEnable( GL_CULL_FACE );

	glDisable( GL_TEXTURE_2D );
	glDisableClientState( GL_VERTEX_ARRAY );
	glDisableClientState( GL_NORMAL_ARRAY );
	glDisableClientState( GL_TEXTURE_COORD_ARRAY );

	// draw childs
	for( uint32 i = 0; i < pMesh->get_child_count(); i++ )
		render_item( (MeshC*)pMesh->get_child( i ), i32Frame );

	glPopMatrix();

}

void
SceneC::setup_camera( uint32 ui32Cam, float32 f32Aspect, bool bReflection, Vector3C rOffset )
{
	CameraC*	pCam = get_camera( ui32Cam );
	if( !pCam )
		return;

	Vector3C	rPos = pCam->get_position() + rOffset;
	Vector3C	rTgt = pCam->get_target_position();
	float		f32FOV = pCam->get_fov();

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	gluPerspective( f32FOV / M_PI * 180.0f / f32Aspect, f32Aspect, 1.0f, pCam->get_far_plane() );

	if( bReflection ) {
		// Mirror around plane y = 0
		glMatrixMode( GL_MODELVIEW );
		glLoadIdentity();
		gluLookAt( rPos[0], -rPos[1], rPos[2], rTgt[0], -rTgt[1], rTgt[2], 0, -1, 0 );
	}
	else {
		glMatrixMode( GL_MODELVIEW );
		glLoadIdentity();
		gluLookAt( rPos[0], rPos[1], rPos[2], rTgt[0], rTgt[1], rTgt[2], 0, 1, 0 );
	}
}

void
SceneC::setup_lights()
{
	for( uint32 i = 0; i < m_rLights.size(); i++ ) {
		LightC*	pLight = m_rLights[i];
		if( !pLight )
			continue;

		Vector3C	rPos = pLight->get_position();
		ColorC		rColor = pLight->get_color();

		GLfloat	f32Diffuse[] = { 1, 1, 1, 1 };
		GLfloat	f32Pos[] = { 1, 1, 1, 1 };

		f32Diffuse[0] = rColor[0] * pLight->get_multiplier();
		f32Diffuse[1] = rColor[1] * pLight->get_multiplier();
		f32Diffuse[2] = rColor[2] * pLight->get_multiplier();

		f32Pos[0] = rPos[0];
		f32Pos[1] = rPos[1];
		f32Pos[2] = rPos[2];

		glEnable( GL_LIGHT0 + i );

		if( pLight->get_attenuate() ) {
			float32 f32Decay = 1.0f / pLight->get_decay_start();
			glLightf( GL_LIGHT0 + i, GL_CONSTANT_ATTENUATION, 0 );
			glLightf( GL_LIGHT0 + i, GL_LINEAR_ATTENUATION , f32Decay );
			glLightf( GL_LIGHT0 + i, GL_QUADRATIC_ATTENUATION , 0 );
		}
		else {
			glLightf( GL_LIGHT0 + i, GL_CONSTANT_ATTENUATION, 1 );
			glLightf( GL_LIGHT0 + i, GL_LINEAR_ATTENUATION , 0 );
			glLightf( GL_LIGHT0 + i, GL_QUADRATIC_ATTENUATION , 0 );
		}

		glLightfv( GL_LIGHT0 + i, GL_DIFFUSE, f32Diffuse );
		glLightfv( GL_LIGHT0 + i, GL_SPECULAR, f32Diffuse );
		glLightfv( GL_LIGHT0 + i, GL_POSITION , f32Pos );
	}
}
