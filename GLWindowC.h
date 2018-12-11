//-------------------------------------------------------------------------
//
// File:		GLWindowC.h
// Desc:		opengl window class.
// Author:		memon <memon@inside.org>
//
//-------------------------------------------------------------------------
//	Copyright (c) 2002 Moppi Productions. All Rights Reserved.
//  This file is part of Eskimo Fishing Trip game. For conditions of 
//  distribution and use, see the accompanying license.txt file.
//  http://moppi.inside.org/
//-------------------------------------------------------------------------

#ifndef __ESKIMO_GLWINDOWC_H__
#define __ESKIMO_GLWINDOWC_H__

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif
#ifdef USE_SDL2
#include <SDL.h>
#endif
#include <GL/gl.h>
#include "PajaTypes.h"
#include "ColorC.h"
#include "Vector2C.h"


#define DP_END_PREVIEW		(WM_APP + 110)

//! Graphics device creation flags.
enum GDeviceCreateFlagsE {
	GRAPHICSDEVICE_CREATE_WINDOWED = 1,			//!< Create a window, to be used in windowed preview.
	GRAPHICSDEVICE_CREATE_FULLSCREEN,		//!< Create a full screen window for preview.
};

//! Graphics device clear flags.
enum GDeviceClearFlagsE {
	GRAPHICSDEVICE_COLORBUFFER =	0x01,	//!< Clear color buffer.
	GRAPHICSDEVICE_DEPTHBUFFER =	0x02,	//!< Clear depth buffer.
	GRAPHICSDEVICE_STENCILBUFFER =	0x04,	//!< Clear stencil buffer.
	GRAPHICSDEVICE_ACCUMBUFFER =	0x08,	//!< Clear accum buffer.
	GRAPHICSDEVICE_ALLBUFFERS =		0x0f,	//!< Clear all buffers.
};


enum GDeviceButtonE {
	MOUSE_LBUTTON	= 0x01,
	MOUSE_RBUTTON	= 0x02,
	KEYB_ESC		= 0x04,
};


class GLWindowC
{
public:
	GLWindowC();
	~GLWindowC();

#ifdef USE_SDL2
	bool		init( PajaTypes::uint32 ui32Flags,
					  PajaTypes::uint32 ui32Width = 0, PajaTypes::uint32 ui32Height = 0,
					  PajaTypes::uint32 ui32BPP = 0 );
#elif defined(WIN32)
	bool		init( HINSTANCE hInstance, PajaTypes::uint32 ui32Flags,
					  PajaTypes::uint32 ui32Width = 0, PajaTypes::uint32 ui32Height = 0,
					  PajaTypes::uint32 ui32BPP = 0 );
#endif
	void		destroy();
	void		flush();

	void		clear_device( PajaTypes::uint32 ui32Flags, const PajaTypes::ColorC& rColor = PajaTypes::ColorC(),
								PajaTypes::float32 f32Depth = 1.0f, PajaTypes::int32 i32Stencil = 0 );

	void		set_size( PajaTypes::int32 int32X, PajaTypes::int32 int32Y,
							PajaTypes::int32 i32Width, PajaTypes::int32 i32Height );

#ifdef WIN32
	HWND		get_hwnd() const;
	HDC			get_hdc() const;
	HGLRC		get_glrc() const;
#endif

	// viewport
	PajaTypes::int32	get_pos_x() const;
	PajaTypes::int32	get_pos_y() const;
	PajaTypes::int32	get_width() const;
	PajaTypes::int32	get_height() const;

	// mouse
	PajaTypes::int32	get_mouse_x() const;
	PajaTypes::int32	get_mouse_y() const;
	PajaTypes::int32	get_mouse_button() const;

	bool				get_pressed( PajaTypes::int32 i32Flags );

protected:
#ifdef USE_SDL2
	SDL_Window *       m_window;
	SDL_GLContext      m_glcontext;
#elif defined(WIN32)
	static bool				set_pixelformat( HDC hDC, PajaTypes::int32 i32BPP );
	static LRESULT CALLBACK	stub_window_proc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );


	HDC					m_hDC;
	HGLRC				m_hGLRC;
	HWND				m_hWnd;
#endif
	bool				m_bResolutionChanged;
	PajaTypes::int32	m_i32WinX, m_i32WinY, m_i32WinWidth, m_i32WinHeight, m_i32BPP;

	PajaTypes::int32	m_i32MouseX, m_i32MouseY, m_i32MouseButton, m_i32Pressed;
};



#endif
