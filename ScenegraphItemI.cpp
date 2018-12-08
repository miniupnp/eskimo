//-------------------------------------------------------------------------
//
// File:		ScenegraphItemI.cpp
// Desc:		scenegraph item interface.
// Author:		memon <memon@inside.org>
//
//-------------------------------------------------------------------------
//	Copyright (c) 2002 Moppi Productions. All Rights Reserved.
//  This file is part of Eskimo Fishing Trip game. For conditions of 
//  distribution and use, see the accompanying license.txt file.
//  http://moppi.inside.org/
//-------------------------------------------------------------------------

#include "PajaTypes.h"
#include "Vector3C.h"
#include <vector>
#include "ScenegraphItemI.h"


using namespace PajaTypes;


ScenegraphItemI::ScenegraphItemI()
{
	// empty
}


ScenegraphItemI::~ScenegraphItemI()
{
	for( uint32 i = 0; i < m_rChilds.size(); i++ )
		delete m_rChilds[i];
}

void
ScenegraphItemI::add_child( ScenegraphItemI* pChild )
{
	if( pChild )
		m_rChilds.push_back( pChild );
}

uint32
ScenegraphItemI::get_child_count()
{
	return m_rChilds.size();
}

ScenegraphItemI*
ScenegraphItemI::get_child( uint32 ui32Index )
{
	if( ui32Index >= 0 && ui32Index < m_rChilds.size() )
		return m_rChilds[ui32Index];
	return 0;
}

void
ScenegraphItemI::remove_child( uint32 ui32Index )
{
	if( ui32Index >= 0 && ui32Index < m_rChilds.size() )
		m_rChilds.erase( m_rChilds.begin() + ui32Index );
}

void
ScenegraphItemI::set_name( const char* szName )
{
	m_sName = szName;
}

const char*
ScenegraphItemI::get_name()
{
	return m_sName.c_str();
}

void
ScenegraphItemI::set_parent_name( const char* szName )
{
	m_sParentName = szName;
}

const char*
ScenegraphItemI::get_parent_name()
{
	return m_sParentName.c_str();
}

void
ScenegraphItemI::set_tm( const Matrix3C& rMat )
{
	m_rTM = rMat;
}

const Matrix3C&
ScenegraphItemI::get_tm()
{
	return m_rTM;
}


