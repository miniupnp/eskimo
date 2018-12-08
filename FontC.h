//-------------------------------------------------------------------------
//
// File:		FontC.h
// Desc:		Font class.
// Author:		memon <memon@inside.org>
//
//-------------------------------------------------------------------------
//	Copyright (c) 2002 Moppi Productions. All Rights Reserved.
//  This file is part of Eskimo Fishing Trip game. For conditions of 
//  distribution and use, see the accompanying license.txt file.
//  http://moppi.inside.org/
//-------------------------------------------------------------------------

#ifndef __ESKIMO_FONT_H__
#define __ESKIMO_FONT_H__

#include "PajaTypes.h"


class FontC
{
public:
	FontC();
	~FontC();

	bool	load( const char* szName );

	PajaTypes::int32		get_width();
	PajaTypes::int32		get_height();

	PajaTypes::uint16		get_font_height();
	PajaTypes::uint16		get_glyph_width( PajaTypes::uint32 ui32Index );
	PajaTypes::uint16		get_glyph_advance( PajaTypes::uint32 ui32Index );
	void					get_glyph_coords( PajaTypes::uint32 ui32Index, PajaTypes::float32& f32U0, PajaTypes::float32& f32V0, PajaTypes::float32& f32U1, PajaTypes::float32& f32V1 );
	void					bind();

	PajaTypes::float32		get_text_length( const char* szText, PajaTypes::float32 f32Scale );
	void					draw_text( PajaTypes::float32 f32X, PajaTypes::float32 f32Y, const char* szText, PajaTypes::float32 f32Scale = 1.0f );

protected:
	void					upload_texture();

	PajaTypes::uint8*		m_pData;
	PajaTypes::uint16*		m_pWidths;
	PajaTypes::uint16*		m_pCoords;
	PajaTypes::uint32		m_ui32FirstGlyph;
	PajaTypes::uint32		m_ui32LastGlyph;
	PajaTypes::uint32		m_ui32FontHeight;
	PajaTypes::uint32		m_ui32GlyphCount;

	PajaTypes::uint32		m_ui32TexWidth;
	PajaTypes::uint32		m_ui32TexHeight;

	PajaTypes::uint32		m_ui32TextureId;
};


#endif