//-------------------------------------------------------------------------
//
// File:		SceneC.h
// Desc:		scene class.
// Author:		memon <memon@inside.org>
//
//-------------------------------------------------------------------------
//	Copyright (c) 2002 Moppi Productions. All Rights Reserved.
//  This file is part of Eskimo Fishing Trip game. For conditions of 
//  distribution and use, see the accompanying license.txt file.
//  http://moppi.inside.org/
//-------------------------------------------------------------------------

#ifndef __ESKIMO_SCENEC_H__
#define __ESKIMO_SCENEC_H__


#include "PajaTypes.h"
#include "MeshC.h"
#include "LightC.h"
#include "CameraC.h"
#include "TextureC.h"
#include "Vector3C.h"
#include "ColorC.h"

class SceneC
{
public:
	SceneC();
	~SceneC();

	virtual bool					load( const char* szName );

	virtual PajaTypes::uint32		get_scenegraphitem_count() const;
	virtual MeshC*					get_scenegraphitem( PajaTypes::uint32 ui32Index );

	virtual PajaTypes::uint32		get_camera_count() const;
	virtual CameraC*				get_camera( PajaTypes::uint32 ui32Index );

	virtual PajaTypes::uint32		get_light_count() const;
	virtual LightC*					get_light( PajaTypes::uint32 ui32Index );

	virtual PajaTypes::uint32		get_texture_count() const;
	virtual TextureC*				get_texture( PajaTypes::uint32 ui32Index );

	virtual void					get_time_parameters( PajaTypes::int32& i32FPS, PajaTypes::int32& i32TicksPerFrame, PajaTypes::int32& i32FirstFrame, PajaTypes::int32& i32LastFrame ) const;

	virtual void					setup_camera( PajaTypes::uint32 ui32Cam, PajaTypes::float32 f32Aspect, bool bReflection, PajaTypes::Vector3C rOffset = PajaTypes::Vector3C( 0, 0, 0 ) );
	virtual void					setup_lights();
	virtual void					draw();
protected:

	void							render_item( MeshC* pMesh, PajaTypes::int32 i32Frame );

	std::vector<TextureC*>		m_rTextures;
	std::vector<MeshC*>			m_rScenegraph;
	std::vector<CameraC*>		m_rCameras;
	std::vector<LightC*>		m_rLights;
	PajaTypes::int32			m_i32FPS;
	PajaTypes::int32			m_i32TicksPerFrame;
	PajaTypes::int32			m_i32FirstFrame;
	PajaTypes::int32			m_i32LastFrame;
};

#endif // __SCENEC_H__