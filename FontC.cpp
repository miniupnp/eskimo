//-------------------------------------------------------------------------
//
// File:		FontC.cpp
// Desc:		Font class.
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
#include "PajaTypes.h"
#include "FontC.h"
#include "string.h"


using namespace PajaTypes;


FontC::FontC() :
	m_pData( 0 ),
	m_pWidths( 0 ),
	m_pCoords( 0 ),
	m_ui32TextureId( 0 )
{
}

FontC::~FontC()
{
}

inline
uint32
lowest_bit_mask( uint32 v )
{
	return (v & -v);
}

static
uint32
ceil_power2( uint32 ui32Num )
{
	uint32	i = lowest_bit_mask( ui32Num );
	while( i < ui32Num )
		i <<= 1;
	return i;
}

bool
FontC::load( const char* szName )
{
	FILE*		fp;

	if( (fp = fopen( szName, "rb" )) == 0 ) {
		return false;
	}

	uint8	ui8Header[4];
	uint8	ui8Tmp;
	uint16	ui16Tmp;

	// Header
	fread( ui8Header, sizeof( ui8Header ), 1, fp );
	if( ui8Header[0] != 'm' || ui8Header[1] != 'o' || ui8Header[2] != 'f' || ui8Header[3] != 'o' ) {
		return false;
	}

	// First char
	fread( &ui8Tmp, sizeof( ui8Tmp ), 1, fp );
	m_ui32FirstGlyph = ui8Tmp;

	// Last char
	fread( &ui8Tmp, sizeof( ui8Tmp ), 1, fp );
	m_ui32LastGlyph = ui8Tmp;

	m_ui32GlyphCount = m_ui32LastGlyph - m_ui32FirstGlyph;
	m_pWidths = new uint16[m_ui32GlyphCount];
	m_pCoords = new uint16[m_ui32GlyphCount * 4];

	// Font height
	fread( &ui16Tmp, sizeof( ui16Tmp ), 1, fp );
	m_ui32FontHeight = ui16Tmp;

	// Texture size
	fread( &ui16Tmp, sizeof( ui16Tmp ), 1, fp );
	m_ui32TexWidth = ui16Tmp;
	fread( &ui16Tmp, sizeof( ui16Tmp ), 1, fp );
	m_ui32TexHeight = ui16Tmp;

	// Widths
	fread( m_pWidths, m_ui32GlyphCount * sizeof( uint16 ), 1, fp );

	// Tex coords
	fread( m_pCoords, m_ui32GlyphCount * 4 * sizeof( uint16 ), 1, fp );

	uint32	ui32DataSize = m_ui32TexWidth * ceil_power2( m_ui32TexHeight );
	delete m_pData;
	m_pData = new uint8[ui32DataSize];
	if( !m_pData ) {
		fclose( fp );
		return false;
	}

	memset( m_pData, 0, ui32DataSize );
	fread( m_pData, m_ui32TexWidth * m_ui32TexHeight, 1, fp );

	fclose( fp );

	m_ui32TexHeight = ceil_power2( m_ui32TexHeight );

	return true;
}

int32
FontC::get_width()
{
	return m_ui32TexWidth;
}

int32
FontC::get_height()
{
	return m_ui32TexHeight;
}

uint16
FontC::get_font_height()
{
	return m_ui32FontHeight;
}

uint16
FontC::get_glyph_width( uint32 ui32Index )
{
	if( ui32Index >= m_ui32FirstGlyph && ui32Index < m_ui32LastGlyph ) {
		uint32	ui32Offset = (ui32Index - m_ui32FirstGlyph) * 4;
		return m_pCoords[ui32Offset + 2] - m_pCoords[ui32Offset + 0];
	}
	return 0;
}

uint16
FontC::get_glyph_advance( uint32 ui32Index )
{
	if( ui32Index >= m_ui32FirstGlyph && ui32Index < m_ui32LastGlyph )
		return m_pWidths[ui32Index - m_ui32FirstGlyph];
	return m_ui32FontHeight / 2;
}

void
FontC::get_glyph_coords( uint32 ui32Index, float32& f32U0, float32& f32V0, float32& f32U1, float32& f32V1 )
{
	if( ui32Index >= m_ui32FirstGlyph && ui32Index < m_ui32LastGlyph ) {
		float32	f32MultX = 1.0f / (float)m_ui32TexWidth;
		float32	f32MultY = 1.0f / (float)m_ui32TexHeight;
		uint32	ui32Offset = (ui32Index - m_ui32FirstGlyph) * 4;
		f32U0 =  (float32)m_pCoords[ui32Offset] * f32MultX;
		f32V0 =  (float32)m_pCoords[ui32Offset + 1] * f32MultY;
		f32U1 =  (float32)m_pCoords[ui32Offset + 2] * f32MultX;
		f32V1 =  (float32)m_pCoords[ui32Offset + 3] * f32MultY;
	}
	else
		f32U0 = f32V0 = f32U1 = f32V1 = 0;
}

void
FontC::upload_texture()
{
	glGenTextures( 1, &m_ui32TextureId );
	glBindTexture( GL_TEXTURE_2D, m_ui32TextureId );

//	gluBuild2DMipmaps( GL_TEXTURE_2D, GL_ALPHA8, m_ui32TexWidth, m_ui32TexHeight, GL_ALPHA, GL_UNSIGNED_BYTE, m_pData );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_ALPHA, m_ui32TexWidth, m_ui32TexHeight, 0, GL_ALPHA, GL_UNSIGNED_BYTE, m_pData );
}

void
FontC::bind()
{
	if( !m_ui32TextureId )
		upload_texture();
	else
		glBindTexture( GL_TEXTURE_2D, m_ui32TextureId );

	glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );

//	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
//	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
}

float32
FontC::get_text_length( const char* szText, float32 f32Scale )
{
	if( !szText || strlen( szText ) == 0 )
		return 0;

	float32	f32Width = 0;
	float32	f32MaxWidth = 0;

	int32	i32Len = strlen( szText );

	for( int32 i = 0; i < i32Len; i++ ) {
		if( szText[i] == 0xd ) {
			// next line
			if( f32Width > f32MaxWidth )
				f32MaxWidth = f32Width; 
			f32Width = 0;
		}
		f32Width += (float32)get_glyph_advance( szText[i] ) * f32Scale;
	}

	if( f32Width > f32MaxWidth )
		f32MaxWidth = f32Width; 

	return f32MaxWidth;
}

void
FontC::draw_text( float32 f32X, float32 f32Y, const char* szText, float32 f32Scale )
{
	if( !szText || strlen( szText ) == 0 )
		return;

	uint32	ui32Len = strlen( szText );
	float32	f32U0, f32V0, f32U1, f32V1;
	float32	f32GlyphWidth;
	float32	f32Height = (float32)get_font_height() * f32Scale;
	float32	f32StartX = f32X;

	glBegin( GL_QUADS );

	for( uint32 i = 0; i < ui32Len; i++ ) {

		if( szText[i] == '\n' ) {
			// next line
			f32X = f32StartX;
			f32Y -= m_ui32FontHeight * f32Scale;
			continue;
		}
		
		f32GlyphWidth = (float32)get_glyph_width( szText[i] ) * f32Scale;

		get_glyph_coords( (uint32)szText[i], f32U0, f32V0, f32U1, f32V1 );

		if( f32GlyphWidth > 0 ) {
			glTexCoord2f( f32U0, f32V1 );
			glVertex2f( f32X, f32Y );
			
			glTexCoord2f( f32U1, f32V1 );
			glVertex2f( f32X + f32GlyphWidth, f32Y );
			
			glTexCoord2f( f32U1, f32V0 );
			glVertex2f( f32X + f32GlyphWidth, f32Y + f32Height );

			glTexCoord2f( f32U0, f32V0 );
			glVertex2f( f32X, f32Y + f32Height );
		}

		f32X += (float32)get_glyph_advance( szText[i] ) * f32Scale;
	}
	
	glEnd();
}
