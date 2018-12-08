//-------------------------------------------------------------------------
//
// File:		TextureC.h
// Desc:		texture class.
// Author:		memon <memon@inside.org>
//
//-------------------------------------------------------------------------
//	Copyright (c) 2002 Moppi Productions. All Rights Reserved.
//  This file is part of Eskimo Fishing Trip game. For conditions of 
//  distribution and use, see the accompanying license.txt file.
//  http://moppi.inside.org/
//-------------------------------------------------------------------------

#ifndef __ESKIMO_TEXTUREC_H__
#define __ESKIMO_TEXTUREC_H__

#include "PajaTypes.h"
#include <string>


// .TGA file header

#pragma pack(1)     // Gotta pack these structures!

struct TGAHeaderS {
	unsigned char	idlen;
	unsigned char	cmtype;
	unsigned char	imgtype;

	unsigned short	cmorg;
	unsigned short	cmlen;
	unsigned char	cmes;

	short			xorg;
	short			yorg;
	short			width;
	short			height;
	unsigned char	pixsize;
	unsigned char	desc;
};

#pragma pack()


class TextureC
{
public:
	TextureC();
	~TextureC();

	bool	load( const char* szName );
	void	bind();

	const char*			get_name() const;

	PajaTypes::int32	get_width() const;
	PajaTypes::int32	get_height() const;
	PajaTypes::int32	get_bpp() const;
	PajaTypes::uint8*	get_data();

protected:
	bool				uncompress_jpeg( PajaTypes::uint8* pCompData, PajaTypes::int32 i32CompDataSize );
	bool				uncompress_tga( PajaTypes::uint8* pCompData, PajaTypes::int32 i32CompDataSize );
	void				upload_texture();
	bool				load_8bit_gray_tga( PajaTypes::uint8* pCompData, PajaTypes::int32 i32CompDataSize, TGAHeaderS& rHdr );
	bool				load_24bit_tga( PajaTypes::uint8* pCompData, PajaTypes::int32 i32CompDataSize, TGAHeaderS& rHdr );
	bool				load_32bit_tga( PajaTypes::uint8* pCompData, PajaTypes::int32 i32CompDataSize, TGAHeaderS& rHdr );

	PajaTypes::uint32		m_ui32TextureId;
	PajaTypes::uint8*		m_pData;
	PajaTypes::int32		m_i32Width, m_i32Height, m_i32BPP;
	std::string				m_sName;
};


#endif // __TEXTUREC_H__