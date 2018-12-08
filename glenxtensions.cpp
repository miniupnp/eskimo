//-------------------------------------------------------------------------
//
// File:		glextensions.cpp
// Desc:		opengl extensions.
// Author:		memon <memon@inside.org>
//
//-------------------------------------------------------------------------
//	Copyright (c) 2002 Moppi Productions. All Rights Reserved.
//  This file is part of Eskimo Fishing Trip game. For conditions of 
//  distribution and use, see the accompanying license.txt file.
//  http://moppi.inside.org/
//-------------------------------------------------------------------------

#include "glextensions.h"


// Multitexture
PFNGLMULTITEXCOORD1FARBPROC		glMultiTexCoord1fARB		= NULL;
PFNGLMULTITEXCOORD2FARBPROC		glMultiTexCoord2fARB		= NULL;
PFNGLMULTITEXCOORD2FVARBPROC	glMultiTexCoord2fvARB		= NULL;
PFNGLMULTITEXCOORD3FARBPROC		glMultiTexCoord3fARB		= NULL;
PFNGLMULTITEXCOORD4FARBPROC		glMultiTexCoord4fARB		= NULL;
PFNGLACTIVETEXTUREARBPROC		glActiveTextureARB			= NULL;
PFNGLCLIENTACTIVETEXTUREARBPROC	glClientActiveTextureARB	= NULL;	

volatile bool					g_bMultiTexture = false;


bool
init_extensions()
{
	if( !g_bMultiTexture ) {

		char*	szExtensions;	
		szExtensions = (char*)glGetString( GL_EXTENSIONS );

		if( szExtensions && strstr( szExtensions, "GL_ARB_multitexture" ) != 0 &&
			strstr( szExtensions, "GL_EXT_texture_env_combine" ) != 0 ) {	

			glMultiTexCoord1fARB	 = (PFNGLMULTITEXCOORD1FARBPROC)wglGetProcAddress( "glMultiTexCoord1fARB" );
			glMultiTexCoord2fARB	 = (PFNGLMULTITEXCOORD2FARBPROC)wglGetProcAddress( "glMultiTexCoord2fARB" );
			glMultiTexCoord2fvARB	 = (PFNGLMULTITEXCOORD2FVARBPROC)wglGetProcAddress( "glMultiTexCoord2fvARB" );
			glMultiTexCoord3fARB	 = (PFNGLMULTITEXCOORD3FARBPROC)wglGetProcAddress( "glMultiTexCoord3fARB" );
			glMultiTexCoord4fARB	 = (PFNGLMULTITEXCOORD4FARBPROC)wglGetProcAddress( "glMultiTexCoord4fARB" );
			glActiveTextureARB		 = (PFNGLACTIVETEXTUREARBPROC)wglGetProcAddress( "glActiveTextureARB" );
			glClientActiveTextureARB = (PFNGLCLIENTACTIVETEXTUREARBPROC)wglGetProcAddress( "glClientActiveTextureARB" );
			g_bMultiTexture = true;
		}

	}

	return g_bMultiTexture;
}

