//-------------------------------------------------------------------------
//
// File:		GLWindowC.cpp
// Desc:		opengl window class.
// Author:		memon <memon@inside.org>
//
//-------------------------------------------------------------------------
//	Copyright (c) 2002 Moppi Productions. All Rights Reserved.
//  This file is part of Eskimo Fishing Trip game. For conditions of 
//  distribution and use, see the accompanying license.txt file.
//  http://moppi.inside.org/
//-------------------------------------------------------------------------

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <gl\gl.h>
#include <gl\glu.h>
#include <stdio.h>
#include <string>
#include "GLWindowC.h"


using namespace PajaTypes;


GLWindowC::GLWindowC() :
	m_hWnd( 0 ),
	m_hDC( 0 ),
	m_hGLRC( 0 ),
	m_bResolutionChanged( false ),
	m_i32WinX( 0 ),
	m_i32WinY( 0 ),
	m_i32WinWidth( 0 ),
	m_i32WinHeight( 0 ),
	m_i32BPP( 0 ),
	m_i32MouseX( 0 ),
	m_i32MouseY( 0 ),
	m_i32MouseButton( 0 ),
	m_i32Pressed( 0 )
{
}

GLWindowC::~GLWindowC()
{
	if( m_hWnd )
		destroy();
}

LRESULT CALLBACK
GLWindowC::stub_window_proc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	GLWindowC*		pWnd = reinterpret_cast<GLWindowC*>(GetWindowLong( hWnd, GWL_USERDATA ));
	PAINTSTRUCT		rPS;

	// the usual window procedure...

	switch( uMsg ) {
	case WM_CREATE:
		{
			SetWindowLong( hWnd, GWL_USERDATA, (LONG)((LPCREATESTRUCT)lParam)->lpCreateParams );

			pWnd = reinterpret_cast<GLWindowC*>(((LPCREATESTRUCT)lParam)->lpCreateParams);

            pWnd->m_hDC = GetDC( hWnd );

			if( !set_pixelformat( pWnd->m_hDC, pWnd->m_i32BPP ) ) {
				OutputDebugString( "set pixel format failed." );
				return -1;
			}
            if ( (pWnd->m_hGLRC = wglCreateContext( pWnd->m_hDC )) == 0 ) {
				OutputDebugString( "wglCreateContext failed." );
				return -1;
			}

			wglMakeCurrent( NULL, NULL );

            if( !wglMakeCurrent( pWnd->m_hDC, pWnd->m_hGLRC ) ) {
				OutputDebugString( "wglMakeCurrent failed" );
				return -1;
			}
		}
		OutputDebugString( "Device Create\n" );
		return 0;

	case WM_DESTROY:
		{
			wglMakeCurrent( NULL, NULL );
			wglDeleteContext( pWnd->m_hGLRC );
			ReleaseDC( hWnd, pWnd->m_hDC );
			pWnd->m_hWnd = 0;
			OutputDebugString( "Device Destroy\n" );
		}
		return 0;

	case WM_ERASEBKGND:
		return -1;	// we clear out mess ourselfs.

	case WM_PAINT:
		{
			BeginPaint( hWnd, &rPS );
			EndPaint( hWnd, &rPS );
		}
		return 0;

	case WM_CLOSE:
		PostMessage( hWnd, DP_END_PREVIEW, 0, 0);
		return 0;

	case WM_SIZE:
		{
			RECT	rRect;
			GetClientRect( hWnd, &rRect );
			uint32	ui32Width = rRect.right; //LOWORD( lParam );
			uint32	ui32Height = rRect.bottom; //HIWORD( lParam );
			pWnd->set_size( 0, 0, ui32Width, ui32Height );
		}
		return 0;

	case WM_CHAR:
		{
			if( wParam == 27 ) {
				pWnd->m_i32Pressed |= (int32)KEYB_ESC;
				return 0;
			}
			break;
		}

	case WM_LBUTTONDOWN:
		{
			pWnd->m_i32MouseButton = 0;
			if( wParam & MK_LBUTTON )
				pWnd->m_i32MouseButton |= MOUSE_LBUTTON;
			if( wParam & MK_RBUTTON )
				pWnd->m_i32MouseButton |= MOUSE_RBUTTON;
			pWnd->m_i32MouseX = LOWORD( lParam );
			pWnd->m_i32MouseY = HIWORD( lParam );

			pWnd->m_i32Pressed |= (int32)MOUSE_LBUTTON;
		}
		return 0;
	case WM_RBUTTONDOWN:
		{
			pWnd->m_i32MouseButton = 0;
			if( wParam & MK_LBUTTON )
				pWnd->m_i32MouseButton |= MOUSE_LBUTTON;
			if( wParam & MK_RBUTTON )
				pWnd->m_i32MouseButton |= MOUSE_RBUTTON;
			pWnd->m_i32MouseX = LOWORD( lParam );
			pWnd->m_i32MouseY = HIWORD( lParam );

			pWnd->m_i32Pressed |= (int32)MOUSE_RBUTTON;
		}
		return 0;

	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MOUSEMOVE:
		{
			pWnd->m_i32MouseButton = 0;
			if( wParam & MK_LBUTTON )
				pWnd->m_i32MouseButton |= MOUSE_LBUTTON;
			if( wParam & MK_RBUTTON )
				pWnd->m_i32MouseButton |= MOUSE_RBUTTON;
			pWnd->m_i32MouseX = LOWORD( lParam );
			pWnd->m_i32MouseY = HIWORD( lParam );
		}
		return 0;

	case DP_END_PREVIEW:
		// make sure out message wont overflow to the system.
		return 0;
	}

	return DefWindowProc( hWnd, uMsg, wParam, lParam );
}


bool
GLWindowC::set_pixelformat( HDC hDC, int32 i32BPP )
{
	PIXELFORMATDESCRIPTOR pfd = {
		sizeof( PIXELFORMATDESCRIPTOR ),// size of this pfd
		1,                              // version number
		PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,	
		PFD_TYPE_RGBA,                  // RGBA type
		32,								// color depth
		0, 0, 0, 0, 0, 0,               // color bits ignored
		0,                              // no alpha buffer
		0,                              // shift bit ignored
		0,                              // no accumulation buffer
		0, 0, 0, 0,                     // accum bits ignored
		24,								// 16-bit z-buffer
		0,                              // no stencil buffer
		0,                              // no auxiliary buffer
		PFD_MAIN_PLANE,                 // main layer
		0,                              // reserved
		0, 0, 0                         // layer masks ignored
	};

	if( i32BPP == 16 )
		pfd.cColorBits = 16;

	int	iPixelformat;

	if( !(iPixelformat = ChoosePixelFormat( hDC, &pfd )) )
		return false;

	if( !SetPixelFormat( hDC, iPixelformat, &pfd ) )
		return false;

	return true;
}


bool
GLWindowC::init( HINSTANCE hInstance, uint32 ui32Flags, uint32 ui32Width, uint32 ui32Height, uint32 ui32BPP )
{

	//
	// Register window class.
	//
	WNDCLASS	rWndClass;

	// Set window infos
	memset( &rWndClass, 0, sizeof( rWndClass ) );
	rWndClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	rWndClass.lpfnWndProc = &stub_window_proc;
	rWndClass.cbClsExtra = 0;
	rWndClass.cbWndExtra = 0;
	rWndClass.hInstance = hInstance;
	rWndClass.hIcon = LoadIcon( NULL, IDI_APPLICATION );
	rWndClass.hCursor = LoadCursor( NULL, IDC_ARROW );
	rWndClass.hbrBackground = NULL;
	rWndClass.lpszMenuName = NULL;
	rWndClass.lpszClassName = "OpenGLWindow";

	// register window
	if( !RegisterClass( &rWndClass ) )
		return false;

	OutputDebugString( "class ok\n" );

	if( ui32Flags == GRAPHICSDEVICE_CREATE_WINDOWED ) {

		uint32	ui32Caption = GetSystemMetrics( SM_CYCAPTION );
		uint32	ui32BorderX = GetSystemMetrics( SM_CXBORDER );
		uint32	ui32BorderY = GetSystemMetrics( SM_CXBORDER );

		// Create popup window
		m_hWnd = CreateWindowEx( 0, "OpenGLWindow", "The ESKIMO Fishing Trip",
								WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
								0, 0, ui32Width + 2 * ui32BorderX, ui32Height + ui32Caption + ui32BorderY * 2,
								NULL, NULL, hInstance, (LPVOID)this );

		ShowWindow( m_hWnd, SW_SHOW );

		set_size( 0, 0, ui32Width, ui32Height );
	}
	else if( ui32Flags == GRAPHICSDEVICE_CREATE_FULLSCREEN ) {
		// Create fullscreen window

		m_hWnd = CreateWindowEx( WS_EX_TOPMOST, "OpenGLWindow", "The ESKIMO Fishing Trip",
								WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
								0, 0, ui32Width, ui32Height,
								NULL, NULL, hInstance, (LPVOID)this );

		if( ui32BPP == 0 ) {
			// Get desktop BPP
			HDC dc = GetDC( 0 ); // desktop dc
			ui32BPP = GetDeviceCaps( dc, BITSPIXEL );
			ReleaseDC( 0, dc );
		}

		DEVMODE	rMode;
		bool	bFoundMode = false;
		uint32	ui32ScreenWidth = ui32Width;
		uint32	ui32ScreenHeight = ui32Height;
		uint32	i = 1;

		memset( &rMode, 0, sizeof( DEVMODE ) );
		rMode.dmSize = sizeof( DEVMODE );

		// init cache
		EnumDisplaySettings( NULL, 0, &rMode );

		// loop all modes
		while( EnumDisplaySettings( NULL, i, &rMode ) != FALSE ) {
			if( rMode.dmBitsPerPel == ui32BPP && rMode.dmPelsWidth >= ui32Width && rMode.dmPelsHeight >= ui32Height ) {
				ui32ScreenWidth = rMode.dmPelsWidth;
				ui32ScreenHeight = rMode.dmPelsHeight;
				bFoundMode = true;
				break;
			}
			i++;
		}

		if( !bFoundMode ) {
			MessageBox( NULL, "Could not find proper display settigs for fullscreen display.", "Error!", MB_OK );
			SendMessage( m_hWnd, WM_DESTROY, 0, 0);
			return false;
		}

		// full screen
		DEVMODE dm;
		memset( &dm, 0, sizeof( dm ) );
		dm.dmSize = sizeof( dm );
		dm.dmPelsWidth = ui32ScreenWidth;
		dm.dmPelsHeight = ui32ScreenHeight;
		dm.dmBitsPerPel = ui32BPP;
		dm.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL;
		if( ChangeDisplaySettings( &dm, CDS_FULLSCREEN ) != DISP_CHANGE_SUCCESSFUL ) {
			MessageBox( NULL, "Cannot change to requested fullscreen display mode.", "Error!", MB_OK );
			SendMessage( m_hWnd, WM_DESTROY, 0, 0);
			return false;
		}

		ShowWindow( m_hWnd, SW_MAXIMIZE );
		SetFocus( m_hWnd );
		BringWindowToTop( m_hWnd );
		SetForegroundWindow( m_hWnd );
		UpdateWindow( m_hWnd );

		m_i32WinX = 0;
		m_i32WinY = 0;
		m_i32WinWidth = (int32)ui32Width;
		m_i32WinHeight = (int32)ui32Height;

		m_bResolutionChanged = true;
	}

	m_i32BPP = (int32)ui32BPP;

	return true;
}

void
GLWindowC::destroy()
{
	if( m_bResolutionChanged ) {
		// restore display mode
		ChangeDisplaySettings( NULL, 0 );
		m_bResolutionChanged = false;
	}

	DestroyWindow( m_hWnd );
}

void
GLWindowC::flush()
{
	SwapBuffers( m_hDC );
}

HWND
GLWindowC::get_hwnd() const
{
	return m_hWnd;
}

HDC
GLWindowC::get_hdc() const
{
	return m_hDC;
}

HGLRC
GLWindowC::get_glrc() const
{
	return m_hGLRC;
}

int32
GLWindowC::get_pos_x() const
{
	return m_i32WinX;
}

int32
GLWindowC::get_pos_y() const
{
	return m_i32WinY;
}

int32
GLWindowC::get_width() const
{
	return m_i32WinWidth;
}

int32
GLWindowC::get_height() const
{
	return m_i32WinHeight;
}

void
GLWindowC::clear_device( uint32 ui32Flags, const ColorC& rColor, float32 f32Depth, int32 i32Stencil )
{
	GLint	iClearBits = 0;

	if( ui32Flags & GRAPHICSDEVICE_COLORBUFFER ) {
		glClearColor( rColor[0], rColor[1], rColor[2], rColor[3] );
		iClearBits |= GL_COLOR_BUFFER_BIT;
	}

	if( ui32Flags & GRAPHICSDEVICE_DEPTHBUFFER ) {
		glClearDepth( f32Depth );
		iClearBits |= GL_DEPTH_BUFFER_BIT;
	}

	if( ui32Flags & GRAPHICSDEVICE_STENCILBUFFER ) {
		glClearStencil( i32Stencil );
		iClearBits |= GL_STENCIL_BUFFER_BIT;
	}

	glClear( iClearBits );
}

void
GLWindowC::set_size( int32 i32X, int32 i32Y, int32 i32Width, int32 i32Height )
{
	glViewport( i32X, i32Y, i32Width, i32Height );
	m_i32WinX = i32X;
	m_i32WinY = i32Y;
	m_i32WinWidth = i32Width;
	m_i32WinHeight = i32Height;
}


int32
GLWindowC::get_mouse_x() const
{
	return m_i32MouseX;
}

int32
GLWindowC::get_mouse_y() const
{
	return m_i32MouseY;
}

int32
GLWindowC::get_mouse_button() const
{
	return m_i32MouseButton;
}

bool
GLWindowC::get_pressed( int32 i32Flags )
{
	if( m_i32Pressed & i32Flags ) {
		m_i32Pressed &= ~i32Flags;	// clear flags
		return true;
	}
	return false;
}
