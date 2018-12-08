//-------------------------------------------------------------------------
//
// File:		ASELoaderC.cpp
// Desc:		ASCII scene .ASE loader
// Author:		memon <memon@inside.org>
//
//-------------------------------------------------------------------------
//	Copyright (c) 2002 Moppi Productions. All Rights Reserved.
//  This file is part of Eskimo Fishing Trip game. For conditions of 
//  distribution and use, see the accompanying license.txt file.
//  http://moppi.inside.org/
//-------------------------------------------------------------------------

// The loader is not complete. The person who designed the ASE file was either drunk,
// or dead tired, because the node TM and vertex transform stuff is plain stupid!
//

#ifdef _MSC_VER
#pragma warning( disable : 4786 )		// long names by STL
#define vsnprintf _vsnprintf
#endif

#include "ASELoaderC.h"
#ifdef WIN32
#include <windows.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "debuglog.h"

#include "ScenegraphItemI.h"
#include "MeshC.h"
#include "CameraC.h"
#include "LightC.h"
#include "DecomposeAffineC.h"

using namespace PajaTypes;
using namespace std;



static
void
TRACE( const char* szFormat, ...  )
{
	char	szMsg[256];
	va_list	rList;
	va_start( rList, szFormat );
	vsnprintf( szMsg, 255, szFormat, rList );
	va_end( rList );
	OutputDebugString( szMsg );
}



ASELoaderC::ASELoaderC()
{
}


ASELoaderC::~ASELoaderC()
{
}


void
ASELoaderC::remove_nl( char* buf )
{
	uint32	len = strlen( buf );
	if( buf[len - 1] == '\n' )
		buf[len - 1] = '\0';
}

char*
ASELoaderC::extract_string( char* szBuf )
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


static
int
fGets( char* pRow, int n, FILE* pStream )
{
	int		c;
	int		i = 0;
	
	do {
		c = fgetc( pStream );
		pRow[i] = c;
		i++;
		if( c == 0xd )
			break;
		if( i >= n )
			break;
	} while( !feof( pStream ) );
	pRow[i] = '\0';	// null terminate string
	
	return i;
}


void
ASELoaderC::read_row()
{
	if( !m_pStream )
		return;
	fGets( m_szRow, 1000, m_pStream );
	remove_nl( m_szRow );
}

bool
ASELoaderC::eof()
{
	return feof( m_pStream ) != 0;
}

bool
ASELoaderC::is_token( const char* szToken )
{
	sscanf( m_szRow, "%s", m_szWord );
	return (strcmp( m_szWord, szToken ) == 0 );
}

char*
ASELoaderC::get_row()
{
	return m_szRow;
}

bool
ASELoaderC::is_block()
{
	for( uint32 i = 0; i < strlen( m_szRow ); i++ )
		if( m_szRow[i] == '{' )
			return true;
	return false;
}


uint32
ASELoaderC::parse_dummy()
{
	do {
		read_row();
		if( is_block() )
			parse_dummy();
		else if( is_token( "}" ) )
			break;
	} while( !eof() );
  
	return 0;
}


bool
ASELoaderC::parse_look_at_node_tm( Vector3C& rPos )
{
	float32		f32X, f32Y, f32Z;
	bool		bIsTarget = false;

	do {
		read_row();

		if( is_token( "*TM_POS" ) ) {
			sscanf( get_row(), "%*s %f %f %f", &f32X, &f32Y, &f32Z );
			rPos[0] = f32X;
			rPos[1] = f32Z;
			rPos[2] = -f32Y;
		}
		else if( is_token( "*NODE_NAME" ) ) {
			if( strstr( get_row(), ".Target" ) )
				bIsTarget = true;
		}
		else if( is_block() )
			parse_dummy();
		else if( is_token( "}" ) )
			break;
	} while( !eof() );

	return bIsTarget;
}


//
// light
//


void
ASELoaderC::parse_light_settings( LightC* pLight )
{
	float32	f32Val, f32R, f32G, f32B;

	do {
		read_row();
		if( is_token( "*LIGHT_COLOR" ) ) {
			sscanf( get_row(), "%*s %f %f %f", &f32R, &f32G, &f32B );
			pLight->set_color( ColorC( f32R, f32G, f32B ) );
		}
		else if( is_token( "*LIGHT_INTENS" ) ) {
			sscanf( get_row(), "%*s %f", &f32Val );
			pLight->set_multiplier( f32Val );
		}
		else if( is_block() )
			parse_dummy();
		else if( is_token( "}" ) )
			break;
	} while( !eof() );
}

LightC*
ASELoaderC::parse_light()
{
	LightC*	pLight = new LightC;
	if( !pLight )
		return 0;

	Vector3C	rPos;

	do {
		read_row();

		if( is_token( "*NODE_NAME" ) ) {
			pLight->set_name( extract_string( get_row() ) );
		}
		else if( is_token( "*NODE_TM" ) ) {
			if( !parse_look_at_node_tm( rPos ) )
				pLight->set_position( rPos );
		}
		else if( is_token( "*LIGHT_SETTINGS" ) ) {
			parse_light_settings( pLight );
		}
		else if( is_block() )
			parse_dummy();
		else if( is_token( "}" ) )
			break;
	} while( !eof() );

	return pLight;
}


//
// camera
//

void
ASELoaderC::parse_camera_settings( CameraC* pCam )
{
	float32	f32Val;

	do {
		read_row();
		if( is_token( "*CAMERA_NEAR" ) ) {
			sscanf( get_row(), "%*s %f", &f32Val );
			pCam->set_near_plane( f32Val );
		}
		else if( is_token( "*CAMERA_FAR" ) ) {
			sscanf( get_row(), "%*s %f", &f32Val );
			pCam->set_far_plane( f32Val );
		}
		else if( is_token( "*CAMERA_FOV" ) ) {
			sscanf( get_row(), "%*s %f", &f32Val );
			pCam->set_fov( f32Val );
		}
		else if( is_block() )
			parse_dummy();
		else if( is_token( "}" ) )
			break;
	} while( !eof() );
}


CameraC*
ASELoaderC::parse_camera()
{
	CameraC*	pCam = new CameraC;
	if( !pCam )
		return 0;

	Vector3C	rPos;

	do {
		read_row();

		if( is_token( "*NODE_NAME" ) ) {
			pCam->set_name( extract_string( get_row() ) );
		}
		else if( is_token( "*NODE_TM" ) ) {
			if( parse_look_at_node_tm( rPos ) )
				pCam->set_target_position( rPos );
			else
				pCam->set_position( rPos );
		}
		else if( is_token( "*CAMERA_SETTINGS" ) ) {
			parse_camera_settings( pCam );
		}
		else if( is_block() )
			parse_dummy();
		else if( is_token( "}" ) )
			break;
	} while( !eof() );

	return pCam;
}


void
ASELoaderC::parse_mesh( MeshC* pMesh )
{
	uint32		ui32Vertices = 0, ui32Faces = 0;
	uint32		ui32TexVertices = 0, ui32TexFaces = 0;
	float32		f32X, f32Y, f32Z;
	uint32		ui32A, ui32B, ui32C, ui32Smooth, ui32Mtl;
	uint32		ui32VA, ui32VB, ui32VC;
	Matrix3C	rInvTM;

	rInvTM = pMesh->get_tm().inverse();

	ASEMeshS				rMesh;
	uint32					ui32MaxTexCh = 0;
	std::vector<Vector2C>	rCoords;

	do {
		read_row();

		if( is_token( "*MESH_NUMVERTEX" ) ) {
			sscanf( get_row(), "%*s %d", &ui32Vertices );
		}
		else if( is_token( "*MESH_NUMFACES" ) ) {
			sscanf( get_row(), "%*s %d", &ui32Faces );
		}
		else if( is_token( "*MESH_VERTEX_LIST" ) ) {
			for( uint32 i = 0; i < ui32Vertices; i++ ) {
				read_row();
				if( eof() )
					return;

				sscanf( get_row(), "%*s %*d %f %f %f", &f32X, &f32Y, &f32Z );

				Vector3C	rVec( f32X, f32Z, -f32Y );
				ASEVertexC	rVert;
				rVert.m_rVert = (rInvTM * rVec);
				rMesh.m_rVertices.push_back( rVert );
			}
			read_row();	// "}"  (end of block)
			if( eof() )
				return;
		}
		else if( is_token( "*MESH_FACE_LIST" ) ) {
			for( uint32 i = 0; i < ui32Faces; i++ ) {
				read_row();
				if( eof() )
					return;

				char	szSmooth[256] ="\0";
				//	*MESH_FACE  351:    A:  182 B:  186 C:  183 AB:    0 BC:    0 CA:    0	 *MESH_SMOOTHING 2 	*MESH_MTLID 0
				//	%*s         %*s     %*s %d %*s  %d  %*s %d  %*s  %*d %*s  %*d %*s  %*d   %*s             %s %*s        %d

//				sscanf( get_row(), "%*s %*s %*s %d %*s %d %*s %d %*s %*d %*s %*d %*s %*d %*s %s %*s %d", &ui32A, &ui32B, &ui32C, szSmooth, &ui32Mtl );


				char*  sPtr;
       
				ui32A = ui32B = ui32C = 0;
        
				sPtr = strstr( get_row(), "A:" );
				if( sPtr ) sscanf( sPtr, "%*s %d", &ui32A );

				sPtr = strstr( get_row(), "B:" );
				if( sPtr ) sscanf( sPtr, "%*s %d", &ui32B );

				sPtr = strstr( get_row(), "C:" );
				if( sPtr ) sscanf( sPtr, "%*s %d", &ui32C );

				sPtr = strstr( get_row(), "MESH_SM" );
				if( sPtr ) sscanf( sPtr, "%*s %s", szSmooth );

				sPtr = strstr( get_row(), "MESH_MT" );
				if( sPtr ) sscanf( sPtr, "%*s %d", &ui32Mtl );


				// Convert mesh smooth to a 32-bit integer
				// the smooth list is comma (,) separated list of numbers from 1 to 32
				// in this loop they are converted to bits in the integer.
				ui32Smooth = 0;

				if( strlen( szSmooth ) > 0 && szSmooth[0] != '*' ) {		// some times there's no smoothing group, so the next title is read.
					const char	szSep[] = ",";
					char*	szTok = strtok( szSmooth, szSep );
					int32	i32Bit;
					int32	i32MaxBits = 32;
					while( szTok && i32MaxBits ) {
						sscanf( szTok, "%d", &i32Bit );
						i32Bit--;	// make it zero based
						assert( i32Bit >= 0 && i32Bit < 32 );
						ui32Smooth |= 1 << i32Bit;
						szTok = strtok( NULL, szSep );
						i32MaxBits--;
					}
				}


				ASEFaceC	rFace;

				rFace.m_ui32VA = ui32A;
				rFace.m_ui32VB = ui32B;
				rFace.m_ui32VC = ui32C;
				rFace.m_ui32Smooth = ui32Smooth;
				rFace.m_ui32Mtl = ui32Mtl;

				rMesh.m_rFaces.push_back( rFace );

			}
			read_row();	// "}"  (end of block)
			if( eof() )
				return;
		}
		else if( is_token( "*MESH_NUMTVERTEX" ) ) {
			sscanf( get_row(), "%*s %d", &ui32TexVertices );
		}
		else if( is_token( "*MESH_TVERTLIST" ) ) {
			for( uint32 i = 0; i < ui32TexVertices; i++ ) {
				read_row();
				if( eof() )
					return;
				sscanf( get_row(), "%*s %*d %f %f", &f32X, &f32Y);
				rCoords.push_back( Vector2C( f32X, 1.0f - f32Y ) );
			}
			read_row();	// "}"  (end of block)
			if( eof() )
				return;
		}
		else if( is_token( "*MESH_NUMTVFACES" ) ) {
			sscanf( get_row(), "%*s %d", &ui32TexFaces );
		}
		else if( is_token( "*MESH_TFACELIST" ) ) {
			for( uint32 i = 0; i < ui32TexFaces; i++ ) {
				read_row();
				if( eof() )
					return;
				sscanf( get_row(), "%*s %*d %d %d %d", &ui32A, &ui32B, &ui32C );

				// add texture vertices to vertex buffers
				ui32VA = rMesh.m_rFaces[i].m_ui32VA;
				ui32VB = rMesh.m_rFaces[i].m_ui32VB;
				ui32VC = rMesh.m_rFaces[i].m_ui32VC;

				// Add coords to zero channel
				const uint32	ui32Channel = 0;
				ui32A = rMesh.m_rVertices[ui32VA].add_texcoord( rCoords[ui32A], ui32Channel );
				ui32B = rMesh.m_rVertices[ui32VB].add_texcoord( rCoords[ui32B], ui32Channel );
 				ui32C = rMesh.m_rVertices[ui32VC].add_texcoord( rCoords[ui32C], ui32Channel );

				rMesh.m_rFaces[i].set_texindex( ui32A, ui32B, ui32C, ui32Channel );

				ui32MaxTexCh++;
			}
			read_row();	// "}"  (end of block)
			if( eof() )
				return;
		}
		else if( is_block() )
			parse_dummy();
		else if( is_token( "}" ) )
			break;
	} while( !eof() );


	//
	// Convert the object
	//

	uint32	i, j, k;

	//
	// calculate normals
	//
	for( i = 0; i < rMesh.m_rFaces.size(); i++ ) {

		uint32	ui32VA = rMesh.m_rFaces[i].m_ui32VA;
		uint32	ui32VB = rMesh.m_rFaces[i].m_ui32VB;
		uint32	ui32VC = rMesh.m_rFaces[i].m_ui32VC;

		Vector3C	rA, rB, rNorm;
		rA = rMesh.m_rVertices[ui32VB].m_rVert - rMesh.m_rVertices[ui32VA].m_rVert;
		rB = rMesh.m_rVertices[ui32VC].m_rVert - rMesh.m_rVertices[ui32VA].m_rVert;
		rNorm = rA.cross( rB );
		rNorm = rNorm.normalize();

		rMesh.m_rFaces[i].m_rNorm = rNorm;

		rMesh.m_rFaces[i].m_ui32NA = rMesh.m_rVertices[ui32VA].add_norm( rNorm, rMesh.m_rFaces[i].m_ui32Smooth );
		rMesh.m_rFaces[i].m_ui32NB = rMesh.m_rVertices[ui32VB].add_norm( rNorm, rMesh.m_rFaces[i].m_ui32Smooth );
		rMesh.m_rFaces[i].m_ui32NC = rMesh.m_rVertices[ui32VC].add_norm( rNorm, rMesh.m_rFaces[i].m_ui32Smooth );
	}

	//
	// register vertex usage
	//
	for( i = 0; i < rMesh.m_rFaces.size(); i++ ) {
		uint32	ui32VA = rMesh.m_rFaces[i].m_ui32VA;
		uint32	ui32VB = rMesh.m_rFaces[i].m_ui32VB;
		uint32	ui32VC = rMesh.m_rFaces[i].m_ui32VC;

		uint32	ui32NA = rMesh.m_rFaces[i].m_ui32NA;
		uint32	ui32NB = rMesh.m_rFaces[i].m_ui32NB;
		uint32	ui32NC = rMesh.m_rFaces[i].m_ui32NC;

		std::vector<uint32>	rTexIdx;

		rTexIdx.clear();
		for( j = 0; j < rMesh.m_rVertices[ui32VA].m_rTexChannels.size(); j++ )
			rTexIdx.push_back( rMesh.m_rFaces[i].m_rTA[j] );
		rMesh.m_rFaces[i].m_ui32UA = rMesh.m_rVertices[ui32VA].add_usage( ui32NA, rTexIdx );

		rTexIdx.clear();
		for( j = 0; j < rMesh.m_rVertices[ui32VB].m_rTexChannels.size(); j++ )
			rTexIdx.push_back( rMesh.m_rFaces[i].m_rTB[j] );
		rMesh.m_rFaces[i].m_ui32UB = rMesh.m_rVertices[ui32VB].add_usage( ui32NB, rTexIdx );

		rTexIdx.clear();
		for( j = 0; j < rMesh.m_rVertices[ui32VC].m_rTexChannels.size(); j++ )
			rTexIdx.push_back( rMesh.m_rFaces[i].m_rTC[j] );
		rMesh.m_rFaces[i].m_ui32UC = rMesh.m_rVertices[ui32VC].add_usage( ui32NC, rTexIdx );
	}


	std::vector<ASETexChannelS>	rTexChannels;
	uint32						ui32BaseIdx = 0;

	rTexChannels.resize( ui32MaxTexCh );

	for( i = 0; i < rMesh.m_rVertices.size(); i++ ) {
		for( j = 0; j < rMesh.m_rVertices[i].m_rVertUsage.size(); j++ ) {
			uint32	ui32N = rMesh.m_rVertices[i].m_rVertUsage[j].m_ui32N;
			uint32	ui32T;

			pMesh->add_vert( rMesh.m_rVertices[i].m_rVert );
			pMesh->add_norm( rMesh.m_rVertices[i].m_rNorms[ui32N] );

			for( k = 0; k < ui32MaxTexCh; k++ ) {
				if( k < rMesh.m_rVertices[i].m_rTexChannels.size() && k < rMesh.m_rVertices[i].m_rVertUsage[j].m_rT.size() ) {
					ui32T = rMesh.m_rVertices[i].m_rVertUsage[j].m_rT[k];
					if( rMesh.m_rVertices[i].m_rTexChannels[k].m_rCoords.size() )
						rTexChannels[k].m_rCoords.push_back( rMesh.m_rVertices[i].m_rTexChannels[k].m_rCoords[ui32T] );
					else
						rTexChannels[k].m_rCoords.push_back( Vector2C( 0, 0 ) );
				}
				else
					rTexChannels[k].m_rCoords.push_back( Vector2C( 0, 0 ) );
			}
		}

		rMesh.m_rVertices[i].m_ui32BaseIdx = ui32BaseIdx;

		ui32BaseIdx += rMesh.m_rVertices[i].m_rVertUsage.size();
	}

	// put texture channels (only channel 0)
	if( rTexChannels.size() ) {
		for( i = 0; i < rTexChannels[0].m_rCoords.size(); i++ )
			pMesh->add_texcoord( rTexChannels[0].m_rCoords[i] );
	}

	//
	// build indices
	//
	for( i = 0; i < rMesh.m_rFaces.size(); i++ ) {
		uint32	ui32VA = rMesh.m_rFaces[i].m_ui32VA;
		uint32	ui32VB = rMesh.m_rFaces[i].m_ui32VB;
		uint32	ui32VC = rMesh.m_rFaces[i].m_ui32VC;

		pMesh->add_index( rMesh.m_rVertices[ui32VA].m_ui32BaseIdx + rMesh.m_rFaces[i].m_ui32UA );
		pMesh->add_index( rMesh.m_rVertices[ui32VB].m_ui32BaseIdx + rMesh.m_rFaces[i].m_ui32UB );
		pMesh->add_index( rMesh.m_rVertices[ui32VC].m_ui32BaseIdx + rMesh.m_rFaces[i].m_ui32UC );
	}


}

void
ASELoaderC::parse_mesh_node_tm( MeshC* pMesh )
{
	float32		f32X, f32Y, f32Z, f32A;

	Vector3C	rRotAxis;
	float32		f32RotAngle;
	Vector3C	rPos;
	Vector3C	rScale;
	Vector3C	rScaleAxis;
	float32		f32ScaleAngle;


	do {
		read_row();

		if( is_token( "*TM_POS" ) ) {
			sscanf( get_row(), "%*s %f %f %f", &f32X, &f32Y, &f32Z );
			rPos[0] = f32X;
			rPos[1] = f32Z;
			rPos[2] = -f32Y;
		}
		else if( is_token( "*TM_ROTAXIS" ) ) {
			sscanf( get_row(), "%*s %f %f %f", &f32X, &f32Y, &f32Z );
			rRotAxis[0] = f32X;
			rRotAxis[1] = f32Z;
			rRotAxis[2] = -f32Y;
		}
		else if( is_token( "*TM_ROTANGLE" ) ) {
			sscanf( get_row(), "%*s %f", &f32A );
			f32RotAngle = f32A;
		}
		else if( is_token( "*TM_SCALE" ) ) {
			sscanf( get_row(), "%*s %f %f %f", &f32X, &f32Y, &f32Z );
			rScale[0] = f32X;
			rScale[1] = f32Z;
			rScale[2] = f32Y;
		}
		else if( is_token( "*TM_SCALEAXIS" ) ) {
			sscanf( get_row(), "%*s %f %f %f", &f32X, &f32Y, &f32Z );
			rScaleAxis[0] = f32X;
			rScaleAxis[1] = f32Z;
			rScaleAxis[2] = -f32Y;
		}
		else if( is_token( "*TM_SCALEAXISANG" ) ) {
			sscanf( get_row(), "%*s %f", &f32A );
			f32ScaleAngle = f32A;
		}
		else if( is_block() )
			parse_dummy();
		else if( is_token( "}" ) )
			break;

	} while( !eof() );


	QuatC		rScaleRot;
	QuatC		rInvScaleRot;
	QuatC		rRot;

	rRot.from_axis_angle( rRotAxis, f32RotAngle );
	rScaleRot.from_axis_angle( rScaleAxis, f32ScaleAngle );

	pMesh->set_position( rPos );
	pMesh->set_rotation( rRot );
	pMesh->set_scale( rScale );
	pMesh->set_scale_rotation( rScaleRot );

	Matrix3C	rScaleMat;
	Matrix3C	rScaleRotMat;
	Matrix3C	rInvScaleRotMat;
	Matrix3C	rRotMat;
	Matrix3C	rPosMat;
	Matrix3C	rTM;

	// Strangle kludge. The animated rotations doesnt work correctly
	// if the angle is not negated when the quat is set to OpenGL.
	rRot[3] = -rRot[3];
	rScaleRot[3] = -rScaleRot[3];

	// scale
	rScaleMat.set_scale( rScale );
	// scale rot
	rScaleRotMat.set_rot( rScaleRot );
	// inv scale rot
	rInvScaleRotMat = rScaleRotMat.inverse();
	// rot
	rRotMat.set_rot( rRot );
	// pos
	rPosMat.set_trans( rPos );

	// Compose matrix
	rTM = rInvScaleRotMat * rScaleMat * rScaleRotMat * rRotMat * rPosMat;

	pMesh->set_tm( rTM );
}


MeshC*
ASELoaderC::parse_geomobject()
{
	uint32		ui32Mtl;
	MeshC*		pMesh = new MeshC;
	if( !pMesh )
		return 0;

	bool		bHasMtl = false;
	Matrix3C	rTM;

	do {
		read_row();

		if( is_token( "*NODE_NAME" ) ) {
			pMesh->set_name( extract_string( get_row() ) );
		}
		else if( is_token( "*NODE_PARENT" ) ) {
			pMesh->set_parent_name( extract_string( get_row() ) );
		}
		else if( is_token( "*NODE_TM" ) ) {
			parse_mesh_node_tm( pMesh );
		}
		else if( is_token( "*MESH" ) ) {
			parse_mesh( pMesh );
		}
		else if( is_token( "*MATERIAL_REF" ) ) {
			sscanf( get_row(), "%*s %d", &ui32Mtl );
			if( ui32Mtl < m_rMaterials.size() ) {
				pMesh->set_ambient( m_rMaterials[ui32Mtl].m_rAmbient );
				pMesh->set_diffuse( m_rMaterials[ui32Mtl].m_rDiffuse );
				pMesh->set_specular( m_rMaterials[ui32Mtl].m_rSpecular );
				pMesh->set_shininess( m_rMaterials[ui32Mtl].m_f32Shininess );
				pMesh->set_texture( get_texture( m_rMaterials[ui32Mtl].m_sTextureName ) );
				pMesh->set_two_sided( m_rMaterials[ui32Mtl].m_bTwoSided );
				bHasMtl = true;
			}
		}
		else if( is_token( "*WIREFRAME_COLOR" ) ) {
			if( !bHasMtl ) {
				float32	f32R, f32G, f32B;
				sscanf( get_row(), "%*s %f %f %f", &f32R, &f32G, &f32B );
				pMesh->set_diffuse( ColorC( f32R, f32G, f32B ) );
			}
      	}
		else if( is_block() )
			parse_dummy();
		else if( is_token( "}" ) )
			break;

	} while( !eof() );

	return pMesh;
}



MeshC*
ASELoaderC::parse_helperobject( string& sGroupName, bool& bIsGroup )
{
	Matrix3C	rTM;
	MeshC*		pMesh = new MeshC;

	if( !pMesh )
		return 0;

	do {
		read_row();

		if( is_token( "*NODE_NAME" ) ) {
			pMesh->set_name( extract_string( get_row() ) );
			if( sGroupName.compare( pMesh->get_name() ) == 0 )
				bIsGroup = true;
		}
		else if( is_token( "*NODE_PARENT" ) ) {
			pMesh->set_parent_name( extract_string( get_row() ) );
		}
		else if( is_token( "*NODE_TM" ) ) {
			parse_mesh_node_tm( pMesh );
		}
		else if( is_block() )
			parse_dummy();
		else if( is_token( "}" ) )
			break;
	} while( !eof() );

	return pMesh;
}


void
ASELoaderC::parse_map( string& sName )
{
	do {
		read_row();
    
		if( is_token( "*BITMAP" ) ) {
			sName = extract_string( get_row() );
		}
		else if( is_block() )
			parse_dummy();
		else if( is_token( "}" ) )
			break;
	} while( !eof() );
}


void
ASELoaderC::parse_material( ASEMaterialS* pMtl )
{
	float32	f32R, f32G, f32B, f32Val;

	read_row();

	pMtl->m_bTwoSided = false;
	
	do {
		read_row();

		if( is_token( "*MAP_DIFFUSE" ) ) {
			parse_map( pMtl->m_sTextureName );
		}
		else if( is_token( "*MATERIAL_SHINE" ) ) {
			sscanf( get_row(), "%*s %f", &f32Val );
			pMtl->m_f32Shininess = f32Val;
		}
		else if( is_token( "*MATERIAL_SHINESTRENGTH" ) ) {
			sscanf( get_row(), "%*s %f", &f32Val );
			if( f32Val > 1.0 )
				f32Val = 1.0;
			if( f32Val < 0 )
				f32Val = 0;
			pMtl->m_rSpecular *= f32Val;
		}
		else if( is_token( "*MATERIAL_TWOSIDED" ) ) {
			pMtl->m_bTwoSided = true;
		}
		else if( is_token( "*MATERIAL_AMBIENT" ) ) {
			sscanf( get_row(), "%*s %f %f %f", &f32R, &f32G, &f32B );
			pMtl->m_rAmbient = ColorC( f32R, f32G, f32B );
		}
		else if( is_token( "*MATERIAL_DIFFUSE" ) ) {
			sscanf( get_row(), "%*s %f %f %f", &f32R, &f32G, &f32B );
			pMtl->m_rDiffuse = ColorC( f32R, f32G, f32B );
		}
		else if( is_token( "*MATERIAL_SPECULAR" ) ) {
			sscanf( get_row(), "%*s %f %f %f", &f32R, &f32G, &f32B );
			pMtl->m_rSpecular = ColorC( f32R, f32G, f32B );
		}
		else if( is_block() )
			parse_dummy();
		else if( is_token( "}" ) )
			break;

	} while( !eof() );

}

void
ASELoaderC::parse_materials()
{
	uint32	ui32NumMaterials;

	do {
		read_row();

		if( is_token( "*MATERIAL_COUNT" ) ) {
			sscanf( get_row(), "%*s %d", &ui32NumMaterials );

			for( uint32 i = 0; i < ui32NumMaterials; i++ ) {
				ASEMaterialS rMtl;
				parse_material( &rMtl );
				m_rMaterials.push_back( rMtl );
			}
		}
		else if( is_block() )
			parse_dummy();
		else if( is_token( "}" ) )
			break;
	} while( !eof() );

}



void
ASELoaderC::get_time_parameters( int32& i32FPS, int32& i32TicksPerFrame, int32& i32FirstFrame, int32& i32LastFrame )
{
	i32FPS = m_i32FPS;
	i32TicksPerFrame = m_i32TicksPerFrame;
	i32FirstFrame = m_i32FirstFrame;
	i32LastFrame = m_i32LastFrame;
}


void
ASELoaderC::parse_scene()
{
	do {
		read_row();
		if( is_token( "*SCENE_FIRSTFRAME" ) ) {
			sscanf( get_row(), "%*s %d", &m_i32FirstFrame );
		}
		else if( is_token( "*SCENE_LASTFRAME" ) ) {
			sscanf( get_row(), "%*s %d", &m_i32LastFrame );
		}
		else if( is_token( "*SCENE_FRAMESPEED" ) ) {
			sscanf( get_row(), "%*s %d", &m_i32FPS );
		}
		else if( is_token( "*SCENE_TICKSPERFRAME" ) ) {
			sscanf( get_row(), "%*s %d", &m_i32TicksPerFrame );
		}
		else if( is_block() )
			parse_dummy();
		else if( is_token( "}" ) )
			break;
	} while( !eof() );
}


MeshC*
ASELoaderC::find_object( const string& sName, MeshC* pParent )
{
	if( !pParent ) {
		for( uint32 i = 0; i < m_rScenegraph.size(); i++ ) {
			MeshC*	pRetItem = find_object( sName, m_rScenegraph[i] );
			if( pRetItem )
				return pRetItem;
		}
	}
	else {
		if( sName.compare( pParent->get_name() ) == 0 )
			return pParent;
		for( uint32 i = 0; i < pParent->get_child_count(); i++ ) {
			MeshC*	pItem = (MeshC*)pParent->get_child( i );
			if( sName.compare( pItem->get_name() ) == 0 )
				return pItem;
			MeshC*	pRetItem = find_object( sName, pItem );
			if( pRetItem )
				return pRetItem;
		}
	}

	return 0;
}


void
ASELoaderC::fix_tm( MeshC* pMesh, MeshC* pParent )
{
	Matrix3C	rTM, rParentTM;

	rTM = pMesh->get_tm() * pParent->get_tm().inverse();

	DecomposeAffineC	rDecomp( rTM );

	QuatC	rRot = rDecomp.get_rotation();
	QuatC	rScaleRot = rDecomp.get_scale_rotation();

	// Strangle kludge. The animated rotations doesnt work correctly
	// if the angle is not negated when the quat is set to OpenGL.
	rRot[3] = -rRot[3];
	rScaleRot[3] = -rScaleRot[3];

	pMesh->set_position( rDecomp.get_translation() );
	pMesh->set_rotation( rRot );
	pMesh->set_scale( rDecomp.get_scale() );
	pMesh->set_scale_rotation( rScaleRot );
}

void
ASELoaderC::add_scenegraphitem( MeshC* pMesh, MeshC* pCurGroup )
{
	if( pMesh ) {
		MeshC*	pParent = 0;
		if( strlen( pMesh->get_parent_name() ) ) {
			if( pCurGroup && strcmp( pMesh->get_parent_name(), pCurGroup->get_name() ) == 0 )
				pParent = pCurGroup;
			else
				pParent = find_object( pMesh->get_parent_name() );
		}

		if( pParent ) {
			fix_tm( pMesh, pParent );
			pParent->add_child( pMesh );
		}
		else
			m_rScenegraph.push_back( pMesh );
	}	
}

MeshC*
ASELoaderC::parse_group( string& sGroupName, int32 i32Indent )
{

	MeshC*	pGroup = 0;

	do {
		read_row();

		if( is_token( "*3DSMAX_ASCIIEXPORT" ) ) {
			sscanf( get_row(), "%*s %d", &m_ui32Version );
		}
		else if( is_token( "*SCENE" ) ) {
			parse_scene();
		}
		else if( is_token( "*MATERIAL_LIST" ) ) {
			parse_materials();
		}
		else if( is_token( "*CAMERAOBJECT" ) ) {
			// Don't store cameras to scenegraph.
			CameraC*	pCam = parse_camera();
			if( pCam )
				m_rCameras.push_back( pCam );
		}
		else if( is_token( "*LIGHTOBJECT" ) ) {
			// Don't store lights to scenegraph.
			LightC*		pLight = parse_light();
			if( pLight )
				m_rLights.push_back( pLight );
		}
		else if( is_token( "*GEOMOBJECT" ) ) {
			MeshC*	pMesh = parse_geomobject();
			add_scenegraphitem( pMesh, pGroup );
		}
		else if( is_token( "*GROUP" ) ) {
			string	sName = extract_string( get_row() );
			MeshC*	pMesh = parse_group( sName, i32Indent + 1 );
			add_scenegraphitem( pMesh, pGroup );
		}
		else if( is_token( "*HELPEROBJECT" ) ) {
			// Kludge.
			// The group is actually a dummy (helper) object.
			// We check if the helper name is same as the group name.
			// If they are we use the helper as group.
			// Other helpers may affect the scenegraph, so we add them
			// to the scenegraph.
			bool	bIsGroup = false;
			MeshC*	pMesh = parse_helperobject( sGroupName, bIsGroup );
			if( pMesh ) {
				if( bIsGroup )
					pGroup = pMesh;
				else
					add_scenegraphitem( pMesh, pGroup );
			}
		}
		else if( is_block() ) {
			parse_dummy();
		}
		else if( is_token( "}" ) )
			break;

	} while( !eof() );

	return pGroup;
}


bool
ASELoaderC::load( const char* szName )
{
	if( (m_pStream = fopen( szName, "rb" )) == 0 )
		return false;

	string	dummy( "" );
	parse_group( dummy, 0 );

	fclose( m_pStream );

	return true;
}

TextureC*
ASELoaderC::get_texture( string& sOrigName )
{
	if( sOrigName.empty() )
		return 0;

	TextureC*	pTex = 0;

	// check if the texture is loade already
	
	for( uint32 i = 0; i < m_rTextures.size(); i++ ) {
		if( sOrigName.compare( m_rTextures[i]->get_name() ) == 0 ) {
			pTex = m_rTextures[i];
		}
	}

	if( !pTex ) { 
		pTex = new TextureC;
		if( !pTex->load( sOrigName.c_str() ) ) {
			// delete the texture if load failed.
			delete pTex;
			pTex = 0;
		}
		else {
			// if load succeed, add the texture to texture list.
			m_rTextures.push_back( pTex );
		}
	}

	return pTex;
}

void
ASELoaderC::get_scenegraph( std::vector<MeshC*>& rScenegraph )
{
	rScenegraph = m_rScenegraph;
}

void
ASELoaderC::get_lights( std::vector<LightC*>& rLights )
{
	rLights = m_rLights;
}

void
ASELoaderC::get_cameras( std::vector<CameraC*>& rCameras )
{
	rCameras = m_rCameras;
}

void
ASELoaderC::get_textures( std::vector<TextureC*>& rTextures )
{
	rTextures = m_rTextures;
}
