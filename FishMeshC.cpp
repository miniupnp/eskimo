//-------------------------------------------------------------------------
//
// File:		FishMeshC.cpp
// Desc:		Fishmesh class. A class to morph two meshes for fish animation.
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
#include <stdio.h>
#include <string.h>
#include "debuglog.h"
#include "PajaTypes.h"
#include "Vector3C.h"
#include "QuatC.h"
#include "ColorC.h"
#include "FishMeshC.h"
#include "TextureC.h"
#include <vector>
#include <string>

#include "ASELoaderC.h"


using namespace PajaTypes;


FishMeshC::FishMeshC() :
	m_pTexture( 0 ),
	m_f32Shininess( 1 )
{
}

FishMeshC::~FishMeshC()
{
	delete m_pTexture;
}

bool
FishMeshC::load( const char* szName )
{
	ASELoaderC	rLoader;

	if( !rLoader.load( szName ) ) {
		OutputDebugString( "FishMeshC::load: load failed\n" );
		return false;
	}

	int32	i;
	std::vector<TextureC*>		rTextures;
	std::vector<MeshC*>			rScenegraph;
	std::vector<CameraC*>		rCameras;
	std::vector<LightC*>		rLights;

	rLoader.get_cameras( rCameras );
	rLoader.get_lights( rLights );
	rLoader.get_scenegraph( rScenegraph );
	rLoader.get_textures( rTextures );

	// make sure the loaded file meets our needs
	if( rTextures.size() != 1 || rScenegraph.size() != 2 || rCameras.size() != 0 || rLights.size() != 0 ) {

		OutputDebugString( "FishMeshC::load: mismatch array sizes\n" );

		for( i = 0; i < rTextures.size(); i++ )
			delete rTextures[i];
		rTextures.clear();

		for( i = 0; i < rLights.size(); i++ )
			delete rLights[i];
		rLights.clear();

		for( i = 0; i < rCameras.size(); i++ )
			delete rCameras[i];
		rCameras.clear();

		for( i = 0; i < rScenegraph.size(); i++ )
			delete rScenegraph[i];
		rScenegraph.clear();

		return false;
	}

	MeshC*	pSrcMesh = 0;
	MeshC*	pTgtMesh = 0;

	std::string	sOpen( "open" );
	std::string	sClosed( "closed" );

	// the source mesh is called "open"
	for( i = 0; i < rScenegraph.size(); i++ ) {
		if( sOpen.compare( rScenegraph[i]->get_name() ) == 0 )
			pTgtMesh = rScenegraph[i];
		if( sClosed.compare( rScenegraph[i]->get_name() ) == 0 )
			pSrcMesh = rScenegraph[i];
	}

	// make sure the meshes are compatible for morphing
	if( !pSrcMesh || !pTgtMesh || pSrcMesh->get_vert_count() != pTgtMesh->get_vert_count() ) {
		for( i = 0; i < rTextures.size(); i++ )
			delete rTextures[i];
		rTextures.clear();
		for( i = 0; i < rScenegraph.size(); i++ )
			delete rScenegraph[i];
		rScenegraph.clear();

		OutputDebugString( "FishMeshC::load: mismatch meshes\n" );

		return false;
	}

	// Save the texture
	if( rTextures.size() )
		m_pTexture = rTextures[0];


	// Copy meshes

	float32*	pSrc;
	float32*	pTgt;
	uint32*		pIndices;
	
	m_rIndices.resize( pSrcMesh->get_index_count() );
	pIndices = pSrcMesh->get_index_ptr();
	for( i = 0; i < m_rIndices.size(); i++ )
		m_rIndices[i] = *pIndices++;

	m_rVertices.resize( pSrcMesh->get_vert_count() * 3 );
	m_rVerticesSource.resize( pSrcMesh->get_vert_count() * 3 );
	m_rVerticesTarget.resize( pSrcMesh->get_vert_count() * 3 );
	
	pSrc = pSrcMesh->get_vert_ptr();
	pTgt = pTgtMesh->get_vert_ptr();

	for( i = 0; i < m_rVerticesSource.size(); i++ ) {
		m_rVerticesSource[i] = *pSrc++;
		m_rVerticesTarget[i] = *pTgt++;
	}

	m_rNormals.resize( pSrcMesh->get_norm_count() * 3 );
	m_rNormalsSource.resize( pSrcMesh->get_norm_count() * 3 );
	m_rNormalsTarget.resize( pSrcMesh->get_norm_count() * 3 );

	pSrc = pSrcMesh->get_norm_ptr();
	pTgt = pTgtMesh->get_norm_ptr();

	for( i = 0; i < m_rNormalsSource.size(); i++ ) {
		m_rNormalsSource[i] = *pSrc++;
		m_rNormalsTarget[i] = *pTgt++;
	}

	m_rTexCoords.resize( pSrcMesh->get_texcoord_count() * 2 );
	pSrc = pSrcMesh->get_texcoord_ptr();
	for( i = 0; i < m_rTexCoords.size(); i++ ) {
		m_rTexCoords[i] = *pSrc++;
	}

	// copy material
	m_rAmbient = pSrcMesh->get_ambient();
	m_rDiffuse = pSrcMesh->get_diffuse();
	m_rSpecular = pSrcMesh->get_specular();
	m_f32Shininess = pSrcMesh->get_shininess();

	return true;
}

void
FishMeshC::eval_state( float32 f32State )
{
	// Linear interpolation from source mesh to destination

	// optimize simple cases
	if( f32State == 0.0f ) {
		memcpy( &(m_rVertices[0]), &(m_rVerticesSource[0]), m_rVertices.size() * sizeof( float32 ) );
		memcpy( &(m_rNormals[0]), &(m_rNormalsSource[0]), m_rNormals.size() * sizeof( float32 ) );
	}
	else if( f32State == 1.0f ) {
		memcpy( &(m_rVertices[0]), &(m_rVerticesTarget[0]), m_rVertices.size() * sizeof( float32 ) );
		memcpy( &(m_rNormals[0]), &(m_rNormalsTarget[0]), m_rNormals.size() * sizeof( float32 ) );
	}
	else {
		float32	f32InvState = 1.0f - f32State;
		for( int32 i = 0; i < m_rVertices.size(); i++ ) {
			m_rVertices[i] = m_rVerticesSource[i] * f32InvState + m_rVerticesTarget[i] * f32State;
			m_rNormals[i] = m_rNormalsSource[i] * f32InvState + m_rNormalsTarget[i] * f32State;
		}
	}
}

uint32
FishMeshC::get_index_count() const
{
	return m_rIndices.size();
}

uint32
FishMeshC::get_vert_count() const
{
	return m_rVertices.size();
}

uint32
FishMeshC::get_norm_count() const
{
	return m_rNormals.size();
}

uint32
FishMeshC::get_texcoord_count() const
{
	return m_rTexCoords.size();
}

uint32*
FishMeshC::get_index_ptr()
{
	return &(m_rIndices[0]);
}

float32*
FishMeshC::get_vert_ptr()
{
	return &(m_rVertices[0]);
}

float32*
FishMeshC::get_norm_ptr()
{
	return &(m_rNormals[0]);
}

float32*
FishMeshC::get_texcoord_ptr()
{
	return &(m_rTexCoords[0]);
}

TextureC*
FishMeshC::get_texture() const
{
	return m_pTexture;
}

const ColorC&
FishMeshC::get_ambient() const
{
	return m_rAmbient;
}

const ColorC&
FishMeshC::get_diffuse() const
{
	return m_rDiffuse;
}

const ColorC&
FishMeshC::get_specular() const
{
	return m_rSpecular;
}

float32
FishMeshC::get_shininess()
{
	return m_f32Shininess;
}

void
FishMeshC::draw( float32 f32State, const Vector3C& rPos, const Vector3C& rTgt, const QuatC& rRot, bool bUseTgt, bool bGlow )
{

	eval_state( f32State );

	glPushMatrix();

	float32		f32X, f32Y, f32Z, f32A;


	// Strangle kludge. The animated rotations doesnt work correctly
	// if the angle is not negated when the quat is set to OpenGL.

	if( !bUseTgt ) {
		glTranslatef( rPos[0], rPos[1], rPos[2] );
		rRot.to_axis_angle( f32X, f32Y, f32Z, f32A );
		glRotatef( -f32A / (float32)M_PI * 180.0f, f32X, f32Y, f32Z );
	}
	else {

		glTranslatef( rPos[0], rPos[1], rPos[2] );

		GLdouble eyex = rPos[0];
		GLdouble eyey = rPos[1];
		GLdouble eyez = rPos[2];

		GLdouble centerx = rTgt[0];
		GLdouble centery = rTgt[1];
		GLdouble centerz = rTgt[2];

		GLdouble upx = 0;
		GLdouble upy = 1;
		GLdouble upz = 0;

		{
		   GLdouble m[16];
		   GLdouble x[3], y[3], z[3];
		   GLdouble mag;

		   /* Make rotation matrix */

		   /* Z vector */
		   z[0] = centerx - eyex;
		   z[1] = centery - eyey;
		   z[2] = centerz - eyez;
		   mag = sqrt( z[0]*z[0] + z[1]*z[1] + z[2]*z[2] );
		   if (mag) {  /* mpichler, 19950515 */
			  z[0] /= mag;
			  z[1] /= mag;
			  z[2] /= mag;
		   }

		   /* Y vector */
		   y[0] = upx;
		   y[1] = upy;
		   y[2] = upz;

		   /* X vector = Y cross Z */
		   x[0] =  y[1]*z[2] - y[2]*z[1];
		   x[1] = -y[0]*z[2] + y[2]*z[0];
		   x[2] =  y[0]*z[1] - y[1]*z[0];

		   /* Recompute Y = Z cross X */
		   y[0] =  z[1]*x[2] - z[2]*x[1];
		   y[1] = -z[0]*x[2] + z[2]*x[0];
		   y[2] =  z[0]*x[1] - z[1]*x[0];

		   /* mpichler, 19950515 */
		   /* cross product gives area of parallelogram, which is < 1.0 for
			* non-perpendicular unit-length vectors; so normalize x, y here
			*/

		   mag = sqrt( x[0]*x[0] + x[1]*x[1] + x[2]*x[2] );
		   if (mag) {
			  x[0] /= mag;
			  x[1] /= mag;
			  x[2] /= mag;
		   }

		   mag = sqrt( y[0]*y[0] + y[1]*y[1] + y[2]*y[2] );
		   if (mag) {
			  y[0] /= mag;
			  y[1] /= mag;
			  y[2] /= mag;
		   }

		   // store the matrix in transposed, and 90degree rotate around x-axis.
		#define M(row,col)  m[col*4+row]
		   M(0,0) = x[0];  M(0,1) = z[0];  M(0,2) = -y[0];  M(0,3) = 0.0;
		   M(1,0) = x[1];  M(1,1) = z[1];  M(1,2) = -y[1];  M(1,3) = 0.0;
		   M(2,0) = x[2];  M(2,1) = z[2];  M(2,2) = -y[2];  M(2,3) = 0.0;
		   M(3,0) = 0.0;   M(3,1) = 0.0;   M(3,2) = 0.0;   M(3,3) = 1.0;
		#undef M
		   glMultMatrixd( m );

		}

		rRot.to_axis_angle( f32X, f32Y, f32Z, f32A );
		glRotatef( -f32A / (float32)M_PI * 180.0f, f32X, f32Y, f32Z );
	}

	//
	// draw
	//

	float32	f32Amb[4];
	float32	f32Diff[4];
	float32	f32Spec[4];
	float32	f32Emission[4];

	f32Amb[0] = m_rAmbient[0] * 0.25;
	f32Amb[1] = m_rAmbient[1] * 0.25;
	f32Amb[2] = m_rAmbient[2] * 0.25;
	f32Amb[3] = 1;

	f32Diff[0] = m_rDiffuse[0];
	f32Diff[1] = m_rDiffuse[1];
	f32Diff[2] = m_rDiffuse[2];
	f32Diff[3] = 1;

	f32Emission[0] = 0;
	f32Emission[1] = 0;
	f32Emission[2] = 0;
	f32Emission[3] = 1;

	f32Spec[0] = m_rSpecular[0];
	f32Spec[1] = m_rSpecular[1];
	f32Spec[2] = m_rSpecular[2];
	f32Spec[3] = 1;

	if( bGlow ) {
		f32Diff[0] = 1;
		f32Diff[1] = 0;
		f32Diff[2] = 0;
		f32Diff[3] = 1;

		f32Emission[0] = 0.5;
		f32Emission[1] = 0;
		f32Emission[2] = 0;
		f32Emission[3] = 1;
	}

	glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT, f32Amb );
	glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, f32Diff );
	glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, f32Spec );
	glMaterialfv( GL_FRONT_AND_BACK, GL_EMISSION, f32Emission );
	glMateriali( GL_FRONT_AND_BACK, GL_SHININESS, 2 + (int32)(m_f32Shininess * 126.0f) );
	
	// Texture stuff
	bool	bHasTex = false;

	if( m_pTexture && get_texcoord_count()  ) {
		glEnable( GL_TEXTURE_2D );
		m_pTexture->bind();
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

	glVertexPointer( 3, GL_FLOAT, 3 * sizeof( float32 ), get_vert_ptr() );
	glNormalPointer( GL_FLOAT, 3 * sizeof( float32 ), get_norm_ptr() );

	if( bHasTex ) {
		glEnableClientState( GL_TEXTURE_COORD_ARRAY );
		glTexCoordPointer( 2, GL_FLOAT, 2 * sizeof( float32 ), get_texcoord_ptr() );
	}

	glDrawElements( GL_TRIANGLES, get_index_count(), GL_UNSIGNED_INT, get_index_ptr() );

	glDisable( GL_TEXTURE_2D );
	glDisableClientState( GL_VERTEX_ARRAY );
	glDisableClientState( GL_NORMAL_ARRAY );
	glDisableClientState( GL_TEXTURE_COORD_ARRAY );

	glPopMatrix();
}
