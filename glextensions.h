#ifndef __GLEXTENSIONS_H__
#define __GLEXTENSIONS_H__

#include <windows.h>
#include <gl/gl.h>
#include "glext.h"


extern PFNGLMULTITEXCOORD1FARBPROC		glMultiTexCoord1fARB;
extern PFNGLMULTITEXCOORD2FARBPROC		glMultiTexCoord2fARB;
extern PFNGLMULTITEXCOORD2FVARBPROC		glMultiTexCoord2fvARB;
extern PFNGLMULTITEXCOORD3FARBPROC		glMultiTexCoord3fARB;
extern PFNGLMULTITEXCOORD4FARBPROC		glMultiTexCoord4fARB;
extern PFNGLACTIVETEXTUREARBPROC		glActiveTextureARB;
extern PFNGLCLIENTACTIVETEXTUREARBPROC	glClientActiveTextureARB;

bool	init_extensions();

#endif