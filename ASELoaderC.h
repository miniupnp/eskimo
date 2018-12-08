//-------------------------------------------------------------------------
//
// File:		ASELoaderC.h
// Desc:		ASCII scene .ASE loader
// Author:		memon <memon@inside.org>
//
//-------------------------------------------------------------------------
//	Copyright (c) 2002 Moppi Productions. All Rights Reserved.
//  This file is part of Eskimo Fishing Trip game. For conditions of 
//  distribution and use, see the accompanying license.txt file.
//  http://moppi.inside.org/
//-------------------------------------------------------------------------

#ifndef __ESKIMO_ASELOADERC_H__
#define __ESKIMO_ASELOADERC_H__

#include "PajaTypes.h"
#include "ColorC.h"
#include "ScenegraphItemI.h"
#include "CameraC.h"
#include "LightC.h"
#include "MeshC.h"

//#include "ContVector3C.h"
//#include "ContQuatC.h"
//#include "ContFloatC.h"


// stl
#include <vector>
#include <string>



class ASELoaderC
{
public:
	ASELoaderC();
	virtual ~ASELoaderC();

	bool				load( const char* szName );

	void				get_scenegraph( std::vector<MeshC*>& rScenegraph );
	void				get_lights( std::vector<LightC*>& rLights );
	void				get_cameras( std::vector<CameraC*>& rCameras );
	void				get_textures( std::vector<TextureC*>& rTextures );

	void				get_time_parameters( PajaTypes::int32& i32FPS, PajaTypes::int32& i32TicksPerFrame, PajaTypes::int32& i32FirstFrame, PajaTypes::int32& i32LastFrame );


private:


	//
	// mesh decomposition
	//

	struct ASETexChannelS {
		std::vector<PajaTypes::Vector2C>	m_rCoords;
	};


	class ASEFaceC {
	public:
		ASEFaceC() :
			m_ui32UA( 0 ), m_ui32UB( 0 ), m_ui32UC( 0 ),
			m_ui32VA( 0 ), m_ui32VB( 0 ), m_ui32VC( 0 ),
			m_ui32NA( 0 ), m_ui32NB( 0 ), m_ui32NC( 0 ),
			m_ui32Smooth( 0 ), m_ui32Mtl( 0 )
		{};
		virtual ~ASEFaceC() {};

		void	set_texindex( PajaTypes::uint32 ui32A, PajaTypes::uint32 ui32B, PajaTypes::uint32 ui32C, PajaTypes::uint32 ui32Ch )
		{
			// enlagre the arrays if they are too small.
			while( ui32Ch >= m_rTA.size() ) {
				m_rTA.push_back( 0 );
				m_rTB.push_back( 0 );
				m_rTC.push_back( 0 );
			}

			m_rTA[ui32Ch] = ui32A;
			m_rTB[ui32Ch] = ui32B;
			m_rTC[ui32Ch] = ui32C;
		}

		PajaTypes::Vector3C	m_rNorm;
		PajaTypes::uint32	m_ui32UA, m_ui32UB, m_ui32UC;
		PajaTypes::uint32	m_ui32VA, m_ui32VB, m_ui32VC;
		PajaTypes::uint32	m_ui32NA, m_ui32NB, m_ui32NC;
		PajaTypes::uint32	m_ui32Smooth, m_ui32Mtl;
		std::vector<PajaTypes::uint32>	m_rTA;
		std::vector<PajaTypes::uint32>	m_rTB;
		std::vector<PajaTypes::uint32>	m_rTC;
	};

	struct ASEVertUsageS {
		PajaTypes::uint32				m_ui32N;
		std::vector<PajaTypes::uint32>	m_rT;
	};

	class ASEVertexC {
	public:

		ASEVertexC() {};
		virtual ~ASEVertexC() {};

		PajaTypes::uint32 add_norm( const PajaTypes::Vector3C& v, PajaTypes::uint32 ui32Smooth )
		{
			for( PajaTypes::uint32 i = 0; i < m_rGroups.size(); i++ ) {
				if( m_rGroups[i] & ui32Smooth ) {
					m_rNorms[i] = m_rNorms[i] + v;
					m_rGroups[i] |= ui32Smooth;
					return i;
				}
			}

			m_rNorms.push_back( v );
			m_rGroups.push_back( ui32Smooth );
			return m_rNorms.size() - 1;
		}

		PajaTypes::uint32	add_texcoord( const PajaTypes::Vector2C& rCoord, PajaTypes::uint32 ui32Ch )
		{
			// enlarge the array if it is too small.
			while( ui32Ch >= m_rTexChannels.size() ) {
				ASETexChannelS	rCh;
				m_rTexChannels.push_back( rCh );
			}

			// kill duplicates
			for( PajaTypes::uint32 i = 0; i < m_rTexChannels[ui32Ch].m_rCoords.size(); i++ ) {
				if( m_rTexChannels[ui32Ch].m_rCoords[i] == rCoord )
					return i;
			}

			// no matching coord was found, add new one.
			m_rTexChannels[ui32Ch].m_rCoords.push_back( rCoord );
			return m_rTexChannels[ui32Ch].m_rCoords.size() - 1;
		}

		PajaTypes::uint32	add_usage( PajaTypes::uint32 ui32NIdx, std::vector<PajaTypes::uint32>& rTIdx )
		{
			PajaTypes::int32	i32Count = 0;

			// check if similar usage is found
			for( PajaTypes::uint32 i = 0; i < m_rVertUsage.size(); i++ ) {
				i32Count = 1 + rTIdx.size();

				if( ui32NIdx == m_rVertUsage[i].m_ui32N )
					i32Count--;

				for( PajaTypes::uint32 j = 0; j < rTIdx.size(); j++ )
					if( rTIdx[j] == m_rVertUsage[i].m_rT[j] )
						i32Count--;

				if( i32Count == 0 )
					return i;
			}

			// nope, add new usage
			ASEVertUsageS	rUsage;

			rUsage.m_ui32N = ui32NIdx;
			for( PajaTypes::uint32 j = 0; j < rTIdx.size(); j++ )
				rUsage.m_rT.push_back( rTIdx[j] );

			m_rVertUsage.push_back( rUsage );

			return m_rVertUsage.size() - 1;
		}

		PajaTypes::Vector3C					m_rVert;
		std::vector<PajaTypes::Vector3C>	m_rNorms;
		std::vector<PajaTypes::uint32>		m_rGroups;
		PajaTypes::uint32					m_ui32BaseIdx;
		std::vector<ASETexChannelS>			m_rTexChannels;
		std::vector<ASEVertUsageS>			m_rVertUsage;
	};

	struct ASEMeshS {
		std::vector<ASEFaceC>				m_rFaces;
		std::vector<ASEVertexC>				m_rVertices;
	};

	struct ASEMaterialS
	{
		std::string				m_sTextureName;
		PajaTypes::ColorC		m_rAmbient;
		PajaTypes::ColorC		m_rDiffuse;
		PajaTypes::ColorC		m_rSpecular;
		PajaTypes::float32		m_f32Shininess;
		bool					m_bTwoSided;
	};


	void					read_row();
	char*					get_row();
	void					remove_nl( char* buf );
	char*					extract_string( char* szBuf );
	bool					is_block();
	bool					is_token( const char* token );
	bool					eof();

	TextureC*				get_texture( std::string& name );
	MeshC*					find_object( const std::string& sName, MeshC* pParent = 0 );

	void					parse_scene();

	void					parse_map( std::string& sName );
	void					parse_materials();
	void					parse_material( ASEMaterialS* pMtl );

/*	void					parse_pos_track( ContVector3C* pCont );
	void					parse_sampled_pos_track( ContVector3C* pCont );
	void					parse_vector3_track( ContVector3C* pCont );
	void					parse_rot_track( ContQuatC* pCont );
	void					parse_sampled_rot_track( ContQuatC* pCont );
	void					parse_scale_track( ContVector3C* pContScale, ContQuatC* pContRot );
	void					parse_sampled_scale_track( ContVector3C* pContScale, ContQuatC* pContRot );
	void					parse_float_track( ContFloatC* pCont );*/

	void					parse_mesh_node_tm( MeshC* pMesh );

	void					fix_tm( MeshC* pMesh, MeshC* pParent );
	void					add_scenegraphitem( MeshC* pMesh, MeshC* pCurGroup );

	MeshC*					parse_geomobject();
	MeshC*					parse_helperobject( std::string& sGroupName, bool& bIsGroup );
	MeshC*					parse_group( std::string& sGroupName, PajaTypes::int32 i32Indent );
	PajaTypes::uint32		parse_dummy();
	void					parse_mesh( MeshC* pMesh );

	bool					parse_look_at_node_tm( PajaTypes::Vector3C& rPos );

	CameraC*				parse_camera();
//	void					parse_camera_tm_anim( CameraC* pCam );
	void					parse_camera_settings( CameraC* pCam );

//	void					parse_object_tm_anim( MeshC* pObj );
	void					parse_object_vis_track( MeshC* pObj );

	void					parse_light_settings( LightC* pLight );
//	void					parse_light_tm_anim( LightC* pLight );
	LightC*					parse_light();


	FILE*							m_pStream;
	char							m_szRow[1024];
	char							m_szWord[256];

	PajaTypes::uint32				m_ui32Version;
	std::vector<ASEMaterialS>		m_rMaterials;

	std::vector<CameraC*>			m_rCameras;
	std::vector<LightC*>			m_rLights;
	std::vector<MeshC*>				m_rScenegraph;
	std::vector<TextureC*>			m_rTextures;

	PajaTypes::int32				m_i32FPS;
	PajaTypes::int32				m_i32TicksPerFrame;
	PajaTypes::int32				m_i32FirstFrame;
	PajaTypes::int32				m_i32LastFrame;
};


#endif
