//-------------------------------------------------------------------------
//
// File:		TextureC.cpp
// Desc:		texture class.
// Author:		memon <memon@inside.org>
//
//-------------------------------------------------------------------------
//	Copyright (c) 2002 Moppi Productions. All Rights Reserved.
//  This file is part of Eskimo Fishing Trip game. For conditions of 
//  distribution and use, see the accompanying license.txt file.
//  http://moppi.inside.org/
//-------------------------------------------------------------------------

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <windows.h>
#include <gl\gl.h>
#include <gl\glu.h>
#include <stdio.h>
#include "ijl.h"
#include "PajaTypes.h"
#include "TextureC.h"


using namespace PajaTypes;


TextureC::TextureC() :
	m_ui32TextureId( 0 ),
	m_pData( 0 ),
	m_i32Width( 0 ),
	m_i32Height( 0 ),
	m_i32BPP( 0 )
{
	// empty
}

TextureC::~TextureC()
{
	if( m_ui32TextureId )
		glDeleteTextures( 1, &m_ui32TextureId );
	delete [] m_pData;
	m_pData = 0;
}


bool
TextureC::load( const char* szName )
{
	FILE*	pStream;

	pStream = fopen( szName, "rb" );
	if( !pStream )
		return false;

	fseek( pStream, 0, SEEK_END );

	int32	i32CompressedSize = ftell( pStream );

	uint8*	pCompressedData = 0;
	pCompressedData = new uint8[i32CompressedSize];

	fseek( pStream, 0, SEEK_SET );

	if( fread( pCompressedData, i32CompressedSize, 1, pStream ) != 1 ) {
		fclose( pStream );
		delete [] pCompressedData;
		return false;
	}

	fclose( pStream );


	char szExt[_MAX_EXT];

	_splitpath( szName, NULL, NULL, NULL, szExt );


	if( _stricmp( ".jpg", szExt ) == 0 ) {
		if( !uncompress_jpeg( pCompressedData, i32CompressedSize ) ) {
			delete [] pCompressedData;
			return false;
		}
	}
	else {
		if( !uncompress_tga( pCompressedData, i32CompressedSize ) ) {
			delete [] pCompressedData;
			return false;
		}
	}

	delete [] pCompressedData;

	m_sName = szName;

	return true;
}

bool
TextureC::uncompress_jpeg( uint8* pCompData, int32 i32CompDataSize )
{
	delete [] m_pData;
	m_pData = 0;

    JPEG_CORE_PROPERTIES	image;
    memset( &image, 0, sizeof( JPEG_CORE_PROPERTIES ) );

    if( ijlInit( &image ) != IJL_OK ) {
        OutputDebugString( "Cannot initialize Intel JPEG library\n" );
        return false;
    }

	image.JPGBytes = pCompData;
	image.JPGSizeBytes = i32CompDataSize;
    
    if( ijlRead( &image, IJL_JBUFF_READPARAMS ) != IJL_OK )
    {
        OutputDebugString( "Cannot read JPEG file header file\n" );
		return false;
    }

    // !dudnik to fix bug [287A305B]
    // Set the JPG color space ... this will always be
    // somewhat of an educated guess at best because JPEG
    // is "color blind" (i.e., nothing in the bit stream
    // tells you what color space the data was encoded from).
    // However, in this example we assume that we are
    // reading JFIF files which means that 3 channel images
    // are in the YCbCr color space and 1 channel images are
    // in the Y color space.
    switch( image.JPGChannels )
    {
    case 1:
      image.JPGColor = IJL_G;
	  image.DIBColor = IJL_RGB;
	  image.DIBChannels = 3;
      break;

    case 3:
      image.JPGColor = IJL_YCBCR;
	  image.DIBColor = IJL_RGB;
	  image.DIBChannels = 3;
      break;

    default:
      // This catches everything else, but no
      // color twist will be performed by the IJL.
      image.DIBColor = (IJL_COLOR)IJL_OTHER;
      image.JPGColor = (IJL_COLOR)IJL_OTHER;
      break;
    }

    image.DIBWidth    = image.JPGWidth;
    image.DIBHeight   = image.JPGHeight;
    // !dudnik: It needs to be considered later...
    image.DIBPadBytes = 0;

    int32	i32ImageSize = image.DIBWidth * image.DIBChannels * image.DIBHeight;

	m_i32Width = image.DIBWidth;
	m_i32Height = image.DIBHeight;
	m_i32BPP = image.DIBChannels * 8;

    m_pData = new uint8[i32ImageSize];
    if( !m_pData ) {
        OutputDebugString( "Cannot allocate memory for image\n" );
        return false;
    }

    image.DIBBytes = m_pData;

    if( ijlRead( &image, IJL_JBUFF_READWHOLEIMAGE ) != IJL_OK )
    {
        OutputDebugString( "Cannot read image data from file\n" );
        delete[] m_pData;
		m_pData = 0;
        return false;
    }

    if( ijlFree( &image ) != IJL_OK ) {
        OutputDebugString( "Cannot free Intel(R) JPEG library" );
    }

	return true;
}


// Load an 8-bit gray TGA file
bool
TextureC::load_8bit_gray_tga( uint8* pCompData, int32 i32CompDataSize, TGAHeaderS& rHdr )
{
	m_pData = new uint8[m_i32Width * m_i32Height];
	m_i32BPP = 8;

	uint8* pDataSrc = pCompData;

	// skip header
	pDataSrc += sizeof( TGAHeaderS );

	//-- Bypass the image identification field, if present -------------------
	if( rHdr.idlen )
		pDataSrc += rHdr.idlen;

	//-- Bypass the color map, if present ------------------------------------
	if( rHdr.cmlen )
		pDataSrc += rHdr.cmlen * (long)((rHdr.cmes + 7) / 8);

	//-- Read Image File -----------------------------------------------------
	switch( rHdr.imgtype ) {

	//-- Uncompressed Gray -----------------------------
	case 3:
		{
			uint8*	pDest = m_pData;
			for( int32 int32Y = 0; int32Y < m_i32Height; int32Y++ ) {
				memcpy( pDest, pDataSrc, m_i32Width );
				pDataSrc += m_i32Width;
				pDest += m_i32Width;
			}
		}
		break;
	
	//-- Compressed Gray -------------------------------

	case 11:
		{
			int32	i32X = 0;
			int32	i32Y = 0;
			uint8	ui8Rle;
			uint8	ui8Pixel;
			uint8*	pDest = m_pData;
			
			while( 1 ) {
				
				ui8Rle = *pDataSrc;
				pDataSrc++;
				
				if( ui8Rle > 127 ) {
					//-- Compressed Block
					ui8Rle -= 127;

					ui8Pixel = *pDataSrc;
					pDataSrc++;

					for( int32 i = 0; i < ui8Rle; i++ ) {

						pDest[i32X++] = ui8Pixel;

						if( i32X >= m_i32Width ) {
							i32X = 0;
							i32Y++;
							if( i32Y >= m_i32Height )
								break;
							pDest += m_i32Width;
						}
					}
				}
				else {
					//-- Uncompressed Block
					ui8Rle++;

					for( int32 i = 0; i < ui8Rle; i++ ) {

						ui8Pixel = *pDataSrc;
						pDataSrc++;

						pDest[i32X++] = ui8Pixel;

						if( i32X >= m_i32Width ) {
							i32X = 0;
							i32Y++;
							if( i32Y >= m_i32Height )
								break;
							pDest += m_i32Width;
						}
					}
				}
			}
		}
		break;
	default:
		OutputDebugString( "weird 8bit tga\n" );
		break;
	}

	return true;
}

// Load an 24-bit TGA file
bool
TextureC::load_24bit_tga( uint8* pCompData, int32 i32CompDataSize, TGAHeaderS& rHdr )
{
	m_pData = new uint8[m_i32Width * m_i32Height * 3];
	m_i32BPP = 24;

	uint8* pDataSrc = pCompData;

	// skip header
	pDataSrc += sizeof( TGAHeaderS );

	//-- Bypass the image identification field, if present -------------------
	if( rHdr.idlen )
		pDataSrc += rHdr.idlen;

	//-- Bypass the color map, if present ------------------------------------
	if( rHdr.cmlen )
		pDataSrc += rHdr.cmlen * (long)((rHdr.cmes + 7) / 8);

	//-- Read Image File -----------------------------------------------------
	switch( rHdr.imgtype ) {

	//-- Uncompressed RGB -----------------------------
	case 2:
		{
			uint8*	pDest = m_pData;
			for( int32 int32Y = 0; int32Y < m_i32Height; int32Y++ ) {
				memcpy( pDest, pDataSrc, m_i32Width * 3 );
				pDataSrc += m_i32Width * 3;

				// swap r & b
				for( int32 i = 0; i < m_i32Width; i++ ) {
					uint8	ui8Tmp = pDest[i * 3];
					pDest[i * 3] = pDest[i * 3 + 2];
					pDest[i * 3 + 2] = ui8Tmp;
					pDest[i * 3 + 3] = pDest[i * 3 + 3];
				}

				pDest += m_i32Width * 3;
			}
		}
		break;
	
	//-- Compressed RGB -------------------------------

	case 10:
		{
			int32	i32X = 0;
			int32	i32Y = 0;
			uint8	ui8Rle;
			uint8	ui8Pixel[3];
			uint8*	pDest = m_pData;
			
			while( 1 ) {
				
				ui8Rle = *pDataSrc;
				pDataSrc++;
				
				if( ui8Rle > 127 ) {
					//-- Compressed Block
					ui8Rle -= 127;

					memcpy( ui8Pixel, pDataSrc, 3 );
					pDataSrc += 3;

					for( int32 i = 0; i < ui8Rle; i++ ) {
						pDest[i32X++] = ui8Pixel[2];
						pDest[i32X++] = ui8Pixel[1];
						pDest[i32X++] = ui8Pixel[0];
						if( i32X >= m_i32Width * 3 ) {
							i32X = 0;
							i32Y++;
							if( i32Y >= m_i32Height )
								break;
							pDest += m_i32Width * 3;
						}
					}
				}
				else {
					//-- Uncompressed Block
					ui8Rle++;

					for( int32 i = 0; i < ui8Rle; i++ ) {

						memcpy( ui8Pixel, pDataSrc, 3 );
						pDataSrc += 3;

						pDest[i32X++] = ui8Pixel[2];
						pDest[i32X++] = ui8Pixel[1];
						pDest[i32X++] = ui8Pixel[0];

						if( i32X >= m_i32Width * 3 ) {
							i32X = 0;
							i32Y++;
							if( i32Y >= m_i32Height )
								break;
							pDest += m_i32Width * 3;
						}
					}
				}
			}
		}
		break;
	default:
		OutputDebugString( "weird 24bit tga\n" );
		break;
	}

	return true;
}


// Load an 32-bit TGA file
bool
TextureC::load_32bit_tga( uint8* pCompData, int32 i32CompDataSize, TGAHeaderS& rHdr )
{
	m_pData = new uint8[m_i32Width * m_i32Height * 4];
	m_i32BPP = 32;

	uint8* pDataSrc = pCompData;

	// skip header
	pDataSrc += sizeof( TGAHeaderS );

	//-- Bypass the image identification field, if present -------------------
	if( rHdr.idlen )
		pDataSrc += rHdr.idlen;

	//-- Bypass the color map, if present ------------------------------------
	if( rHdr.cmlen )
		pDataSrc += rHdr.cmlen * (long)((rHdr.cmes + 7) / 8);

	//-- Read Image File -----------------------------------------------------
	switch( rHdr.imgtype ) {

	//-- Uncompressed RGB -----------------------------
	case 2:
		{
			uint8*	pDest = m_pData;
			for( int32 int32Y = 0; int32Y < m_i32Height; int32Y++ ) {
				memcpy( pDest, pDataSrc, m_i32Width * 4 );
				pDataSrc += m_i32Width * 4;

				// swap r & b
				for( int32 i = 0; i < m_i32Width; i++ ) {
					uint8	ui8Tmp = pDest[i * 4];
					pDest[i * 4] = pDest[i * 4 + 2];
					pDest[i * 4 + 2] = ui8Tmp;
					pDest[i * 4 + 3] = pDest[i * 4 + 3];
				}

				pDest += m_i32Width * 4;
			}
		}
		break;
	
	//-- Compressed RGB -------------------------------

	case 10:
		{
			int32	i32X = 0;
			int32	i32Y = 0;
			uint8	ui8Rle;
			uint8	ui8Pixel[4];
			uint8*	pDest = m_pData;
			
			while( 1 ) {
				
				ui8Rle = *pDataSrc;
				pDataSrc++;
				
				if( ui8Rle > 127 ) {
					//-- Compressed Block
					ui8Rle -= 127;

					memcpy( ui8Pixel, pDataSrc, 4 );
					pDataSrc += 4;

					for( int32 i = 0; i < ui8Rle; i++ ) {
						pDest[i32X++] = ui8Pixel[2];
						pDest[i32X++] = ui8Pixel[1];
						pDest[i32X++] = ui8Pixel[0];
						pDest[i32X++] = ui8Pixel[3];
						if( i32X >= m_i32Width * 4 ) {
							i32X = 0;
							i32Y++;
							if( i32Y >= m_i32Height )
								break;
							pDest += m_i32Width * 4;
						}
					}
				}
				else {
					//-- Uncompressed Block
					ui8Rle++;

					for( int32 i = 0; i < ui8Rle; i++ ) {

						memcpy( ui8Pixel, pDataSrc, 4 );
						pDataSrc += 4;

						pDest[i32X++] = ui8Pixel[2];
						pDest[i32X++] = ui8Pixel[1];
						pDest[i32X++] = ui8Pixel[0];
						pDest[i32X++] = ui8Pixel[3];

						if( i32X >= m_i32Width * 4 ) {
							i32X = 0;
							i32Y++;
							if( i32Y >= m_i32Height )
								break;
							pDest += m_i32Width * 4;
						}
					}
				}
			}
		}
		break;
	default:
		OutputDebugString( "weird 32bit tga\n" );
		break;
	}

	return true;
}


bool
TextureC::uncompress_tga( uint8* pCompData, int32 i32CompDataSize )
{
	TGAHeaderS	rHeader;

	memcpy( &rHeader, pCompData, sizeof( rHeader ) );

	bool	bHFlip = (rHeader.desc & 0x10) ? true : false;		// Need hflip
	bool	bVFlip = (rHeader.desc & 0x20) ? false : true;		// Need vflip
	rHeader.desc &= 0xcf;										// Mask off flip bits

	m_i32Width = rHeader.width;
	m_i32Height = rHeader.height;

	delete [] m_pData;
	m_pData = 0;

	switch( rHeader.pixsize ) {
	case 8:
		{
			if( rHeader.desc != 0 && rHeader.desc != 1 && rHeader.desc != 8 )
				return false;
			switch( rHeader.imgtype ) {
			case 3:
			case 11:
				load_8bit_gray_tga( pCompData, i32CompDataSize, rHeader );
				break;
			}
		}
		break;
	case 24:
		{
			if( rHeader.desc != 0 )
				return false;
			load_24bit_tga( pCompData, i32CompDataSize, rHeader );
		}
		break;
	case 32:
		{
			load_32bit_tga( pCompData, i32CompDataSize, rHeader );
		}
		break;
	default:
		return false;
	}

	//-- Perform clean-up operations!  
	if( m_pData ) {

		if( bHFlip ) {
			if( m_i32BPP == 8 ) {
				uint8*	pTmpLine = new uint8[m_i32Width];
				uint8*	pDest = m_pData;
				for( uint32 i = 0; i < m_i32Height; i++ ) {
					memcpy( pTmpLine, pDest, m_i32Width );
					for( uint32 j = 0; j < m_i32Width; j++ ) {
						pDest[j] = pTmpLine[((m_i32Width - 1) - j)];
					}
					pDest += m_i32Width;
				}
				delete pTmpLine;
			}
			else if( m_i32BPP == 24 ) {
				uint8*	pTmpLine = new uint8[m_i32Width * 3];
				uint8*	pDest = m_pData;
				for( uint32 i = 0; i < m_i32Height; i++ ) {
					memcpy( pTmpLine, pDest, m_i32Width * 3 );
					for( uint32 j = 0; j < m_i32Width; j++ ) {
						pDest[j * 3] = pTmpLine[((m_i32Width - 1) - j) * 3];
						pDest[j * 3 + 1] = pTmpLine[((m_i32Width - 1) - j) * 3 + 1];
						pDest[j * 3 + 2] = pTmpLine[((m_i32Width - 1) - j) * 3 + 2];
					}
					pDest += m_i32Width * 3;
				}
				delete pTmpLine;
			}
			else if( m_i32BPP == 32 ) {
				uint8*	pTmpLine = new uint8[m_i32Width * 4];
				uint8*	pDest = m_pData;
				for( uint32 i = 0; i < m_i32Height; i++ ) {
					memcpy( pTmpLine, pDest, m_i32Width * 4 );
					for( uint32 j = 0; j < m_i32Width; j++ ) {
						pDest[j * 4 + 0] = pTmpLine[((m_i32Width - 1) - j) * 4 + 0];
						pDest[j * 4 + 1] = pTmpLine[((m_i32Width - 1) - j) * 4 + 1];
						pDest[j * 4 + 2] = pTmpLine[((m_i32Width - 1) - j) * 4 + 2];
						pDest[j * 4 + 3] = pTmpLine[((m_i32Width - 1) - j) * 4 + 3];
					}
					pDest += m_i32Width * 4;
				}
				delete pTmpLine;
			}

		}

		if( bVFlip ) {
			if( m_i32BPP == 8 ) {
				uint8*	pTmpLineTop = new uint8[m_i32Width];
				uint8*	pTmpLineBottom = new uint8[m_i32Width];
				for( uint32 ui32YTop = 0, ui32YBot = m_i32Height - 1; ui32YTop < ui32YBot; ui32YTop++, ui32YBot-- ) {
					memcpy( pTmpLineTop, m_pData + (ui32YTop * m_i32Width), m_i32Width );
					memcpy( pTmpLineBottom, m_pData + (ui32YBot * m_i32Width), m_i32Width );
					memcpy( m_pData + (ui32YTop * m_i32Width), pTmpLineBottom, m_i32Width );
					memcpy( m_pData + (ui32YBot * m_i32Width), pTmpLineTop, m_i32Width );
				}
				delete pTmpLineTop;
				delete pTmpLineBottom;
			}
			else if( m_i32BPP == 24 ) {
				uint8*	pTmpLineTop = new uint8[m_i32Width * 3];
				uint8*	pTmpLineBottom = new uint8[m_i32Width * 3];
				for( uint32 ui32YTop = 0, ui32YBot = m_i32Height - 1; ui32YTop < ui32YBot; ui32YTop++, ui32YBot-- ) {
					memcpy( pTmpLineTop, m_pData + (ui32YTop * m_i32Width * 3), m_i32Width * 3 );
					memcpy( pTmpLineBottom, m_pData + (ui32YBot * m_i32Width * 3), m_i32Width * 3 );
					memcpy( m_pData + (ui32YTop * m_i32Width * 3), pTmpLineBottom, m_i32Width * 3 );
					memcpy( m_pData + (ui32YBot * m_i32Width * 3), pTmpLineTop, m_i32Width * 3 );
				}
				delete pTmpLineTop;
				delete pTmpLineBottom;
			}
			else if( m_i32BPP == 32 ) {
				uint8*	pTmpLineTop = new uint8[m_i32Width * 4];
				uint8*	pTmpLineBottom = new uint8[m_i32Width * 4];
				for( uint32 ui32YTop = 0, ui32YBot = m_i32Height - 1; ui32YTop < ui32YBot; ui32YTop++, ui32YBot-- ) {
					memcpy( pTmpLineTop, m_pData + (ui32YTop * m_i32Width * 4), m_i32Width * 4 );
					memcpy( pTmpLineBottom, m_pData + (ui32YBot * m_i32Width * 4), m_i32Width * 4 );
					memcpy( m_pData + (ui32YTop * m_i32Width * 4), pTmpLineBottom, m_i32Width * 4 );
					memcpy( m_pData + (ui32YBot * m_i32Width * 4), pTmpLineTop, m_i32Width * 4 );
				}
				delete pTmpLineTop;
				delete pTmpLineBottom;
			}
		}
	}

	return true;
}


void
TextureC::upload_texture()
{
	glGenTextures( 1, &m_ui32TextureId );
	glBindTexture( GL_TEXTURE_2D, m_ui32TextureId );

	if( m_i32BPP == 8 )
		gluBuild2DMipmaps( GL_TEXTURE_2D, GL_ALPHA8, m_i32Width, m_i32Height, GL_ALPHA, GL_UNSIGNED_BYTE, m_pData );
//		glTexImage2D( GL_TEXTURE_2D, 0, GL_ALPHA, m_i32Width, m_i32Height, 0, GL_ALPHA, GL_UNSIGNED_BYTE, m_pData );
	if( m_i32BPP == 24 )
		gluBuild2DMipmaps( GL_TEXTURE_2D, GL_RGB8, m_i32Width, m_i32Height, GL_RGB, GL_UNSIGNED_BYTE, m_pData );
//		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, m_i32Width, m_i32Height, 0, GL_RGB, GL_UNSIGNED_BYTE, m_pData );
	if( m_i32BPP == 32 )
		gluBuild2DMipmaps( GL_TEXTURE_2D, GL_RGBA8, m_i32Width, m_i32Height, GL_RGBA, GL_UNSIGNED_BYTE, m_pData );
//		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, m_i32Width, m_i32Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_pData );

}

void
TextureC::bind()
{
	if( !m_ui32TextureId )
		upload_texture();
	else
		glBindTexture( GL_TEXTURE_2D, m_ui32TextureId );

	glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );

	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR );

	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
}

int32
TextureC::get_width() const
{
	return m_i32Width;
}

int32
TextureC::get_height() const
{
	return m_i32Height;
}

uint8*
TextureC::get_data()
{
	return m_pData;
}

const char*
TextureC::get_name() const
{
	return m_sName.c_str();
}
