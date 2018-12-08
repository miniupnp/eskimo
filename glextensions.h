#ifndef __GLEXTENSIONS_H__
#define __GLEXTENSIONS_H__

#ifdef WIN32
#include <windows.h>
#endif
#include <GL/gl.h>
#include "Glext.h"


#if 0
extern PFNGLMULTITEXCOORD1FARBPROC		glMultiTexCoord1fARB;
extern PFNGLMULTITEXCOORD2FARBPROC		glMultiTexCoord2fARB;
extern PFNGLMULTITEXCOORD2FVARBPROC		glMultiTexCoord2fvARB;
extern PFNGLMULTITEXCOORD3FARBPROC		glMultiTexCoord3fARB;
extern PFNGLMULTITEXCOORD4FARBPROC		glMultiTexCoord4fARB;
extern PFNGLACTIVETEXTUREARBPROC		glActiveTextureARB;
extern PFNGLCLIENTACTIVETEXTUREARBPROC	glClientActiveTextureARB;
#endif

bool	init_extensions();

#endif
