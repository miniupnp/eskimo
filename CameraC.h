//-------------------------------------------------------------------------
//
// File:		CameraC.h
// Desc:		Camera class.
// Author:		memon <memon@inside.org>
//
//-------------------------------------------------------------------------
//	Copyright (c) 2002 Moppi Productions. All Rights Reserved.
//  This file is part of Eskimo Fishing Trip game. For conditions of 
//  distribution and use, see the accompanying license.txt file.
//  http://moppi.inside.org/
//-------------------------------------------------------------------------

#ifndef __ESKIMO_CAMERAC_H__
#define __ESKIMO_CAMERAC_H__

#include "PajaTypes.h"
#include "Vector3C.h"
#include "ScenegraphItemI.h"


const PajaTypes::int32	CGITEM_CAMERA = 0x2000;


class CameraC : public ScenegraphItemI
{
public:
	CameraC();
	virtual ~CameraC();

	virtual PajaTypes::int32			get_type();

	virtual void						set_position( const PajaTypes::Vector3C& rPos );
	virtual const PajaTypes::Vector3C&	get_position();

	virtual void						set_target_position( const PajaTypes::Vector3C& rPos );
	virtual const PajaTypes::Vector3C&	get_target_position();

	virtual void						set_fov( const PajaTypes::float32 f32Val );
	virtual const PajaTypes::float32	get_fov();

	virtual void						set_near_plane( const PajaTypes::float32 f32Val );
	virtual const PajaTypes::float32	get_near_plane();

	virtual void						set_far_plane( const PajaTypes::float32 f32Val );
	virtual const PajaTypes::float32	get_far_plane();

	virtual void						eval_state( PajaTypes::int32 i32Time );

private:
	PajaTypes::Vector3C	m_rPos;
	PajaTypes::Vector3C	m_rTgt;
	PajaTypes::float32	m_f32Roll;
	PajaTypes::float32	m_f32FOV;
	PajaTypes::float32	m_f32NearPlane;
	PajaTypes::float32	m_f32FarPlane;
};


#endif // __CAMERAC_H__