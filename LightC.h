//-------------------------------------------------------------------------
//
// File:		LightC.h
// Desc:		light class.
// Author:		memon <memon@inside.org>
//
//-------------------------------------------------------------------------
//	Copyright (c) 2002 Moppi Productions. All Rights Reserved.
//  This file is part of Eskimo Fishing Trip game. For conditions of 
//  distribution and use, see the accompanying license.txt file.
//  http://moppi.inside.org/
//-------------------------------------------------------------------------

#ifndef __ESKIMO_LIGHTC_H__
#define __ESKIMO_LIGHTC_H__

#include "PajaTypes.h"
#include "Vector3C.h"
#include "ColorC.h"
#include "ScenegraphItemI.h"


const PajaTypes::int32	CGITEM_LIGHT = 0x3000;


class LightC : public ScenegraphItemI
{
public:
	LightC();
	virtual ~LightC();

	virtual PajaTypes::int32			get_type();

	virtual void						set_position( const PajaTypes::Vector3C& rPos );
	virtual const PajaTypes::Vector3C&	get_position();

	virtual void						set_color( const PajaTypes::ColorC& rVal );
	virtual const PajaTypes::ColorC&	get_color();

	virtual void						set_attenuate( bool bState );
	virtual bool						get_attenuate() const;

	virtual void						set_decay_start( PajaTypes::float32 f32Val );
	virtual PajaTypes::float32			get_decay_start() const;

	virtual void						set_multiplier( PajaTypes::float32 f32Val );
	virtual PajaTypes::float32			get_multiplier();

	virtual void						eval_state( PajaTypes::int32 i32Time );

private:
	PajaTypes::Vector3C	m_rPos;
	PajaTypes::ColorC	m_rColor;
	PajaTypes::float32	m_f32Multiplier;
	PajaTypes::float32	m_f32Decay;
	bool				m_bAttenuate;
};


#endif // LIGHT_H