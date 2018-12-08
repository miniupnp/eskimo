//-------------------------------------------------------------------------
//
// File:		ScenegraphItemI.h
// Desc:		scenegraph item interface.
// Author:		memon <memon@inside.org>
//
//-------------------------------------------------------------------------
//	Copyright (c) 2002 Moppi Productions. All Rights Reserved.
//  This file is part of Eskimo Fishing Trip game. For conditions of 
//  distribution and use, see the accompanying license.txt file.
//  http://moppi.inside.org/
//-------------------------------------------------------------------------

#ifndef __ESKIMO_SCENEGRAPHITEMI_H__
#define __ESKIMO_SCENEGRAPHITEMI_H__


#include "PajaTypes.h"
#include "Vector3C.h"
#include "Matrix3C.h"
#include <vector>
#include <string>


class ScenegraphItemI
{
public:
	ScenegraphItemI();
	virtual ~ScenegraphItemI();

	virtual PajaTypes::int32			get_type() = 0;
	virtual void						eval_state( PajaTypes::int32 i32Time ) = 0;

	virtual void						add_child( ScenegraphItemI* pChild );
	virtual PajaTypes::uint32			get_child_count();
	virtual ScenegraphItemI*			get_child( PajaTypes::uint32 ui32Index );
	virtual void						remove_child( PajaTypes::uint32 ui32Index );

	virtual void						set_name( const char* szName );
	virtual const char*					get_name();

	virtual void						set_parent_name( const char* szName );
	virtual const char*					get_parent_name();

	virtual void						set_tm( const PajaTypes::Matrix3C& rMat );
	virtual const PajaTypes::Matrix3C&	get_tm();

protected:
	std::vector<ScenegraphItemI*>	m_rChilds;
	std::string						m_sName;
	std::string						m_sParentName;
	PajaTypes::Matrix3C				m_rTM;
};


#endif // __SCENEGRAPHITEMI_H__