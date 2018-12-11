//-------------------------------------------------------------------------
//
// File:		fisupeli.cpp
// Desc:		the main game file...
// Author:		memon <memon@inside.org>
//
//-------------------------------------------------------------------------
//	Copyright (c) 2002 Moppi Productions. All Rights Reserved.
//  This file is part of Eskimo Fishing Trip game. For conditions of 
//  distribution and use, see the accompanying license.txt file.
//  http://moppi.inside.org/
//-------------------------------------------------------------------------

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <time.h>
#endif
#if USE_SDL2
#include <SDL.h>
#endif
#include <GL/gl.h>
#include <GL/glu.h>
#include "glextensions.h"
#include <stdio.h>
#include <string>
#include "PajaTypes.h"
#include "Vector3C.h"
#include <vector>
#include <string>
#include <assert.h>
#include "debuglog.h"

#include "GLWindowC.h"
#include "TextureC.h"
#include "SplineCurveC.h"
#include "SceneC.h"
#include "FishMeshC.h"
#include "FontC.h"

#ifdef WIN32
#include "fmod.h"
#include "fmod_errors.h"
#endif

#include "FishingContextC.h"
#include "FishC.h"
#include "FishingPondC.h"
#include "FishingRodC.h"
#include "WaterC.h"

#include "res/resource.h"

#ifdef _MSC_VER
#define snprintf _snprintf
#endif

using namespace PajaTypes;




//
// Debug printf...
//

static
void
TRACE( const char* szFormat, ...  )
{
#if 0
	char	szMsg[256];
	va_list	rList;
	va_start( rList, szFormat );
	_vsnprintf( szMsg, 255, szFormat, rList );
	va_end( rList );
	OutputDebugString( szMsg );
#endif
}


#if 0
static
GL_ERROR( const char* szName )
{
	GLenum	eError = glGetError();
	if( eError ) {
		OutputDebugString( szName );
		OutputDebugString( ": " );
		OutputDebugString( (const char*)gluErrorString( eError ) );
		OutputDebugString( "\n" );
	}
}
#endif



#if 0
static
float32
frand()
{
	return ((float32)rand() / (float32)RAND_MAX);
}
#endif

static
int32
get_time()
{
#ifdef WIN32
	LARGE_INTEGER	d;
	double			i, j;
	QueryPerformanceCounter( &d );
	i = (double)d.QuadPart;
	QueryPerformanceFrequency( &d );
	j = (double)d.QuadPart;
	return (int32)((i / j) * 1000.0);
#else
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
#endif
}




// Globals.
bool			g_bFullscreen = true;
int32			g_i32Resolution = 4;
int32			g_i32Detail = 1;


// Init dialog proc.
#ifdef WIN32
BOOL CALLBACK
InitDlgProc( HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch( uMsg ) {

	case WM_INITDIALOG:
		{
			// fullscreen
			if( g_bFullscreen )
				CheckDlgButton( hwndDlg, IDC_CHECK_FULLSCREEN, BST_CHECKED );

			int32	i;

			// resolution display
			char	szItems[6][64] = {
			"640 x 480 x 16Bit",
			"800 x 600 x 16Bit",
			"1024 x 768 x 16Bit",
			"640 x 480 x 32Bit",
			"800 x 600 x 32Bit",
			"1024 x 768 x 32Bit" };

			for( i = 0; i < 6; i++ )
				SendDlgItemMessage( hwndDlg, IDC_COMBO_RESOLUTION, CB_INSERTSTRING, i, (LPARAM)(LPCTSTR)szItems[i] );

			SendDlgItemMessage( hwndDlg, IDC_COMBO_RESOLUTION, CB_SETCURSEL, g_i32Resolution, 0 );

			// detail
			char	szItemsDetail[6][64] = {
			"Excellent",
			"Good",
			"Medium",
			"Poor" };
			for( i = 0; i < 4; i++ )
				SendDlgItemMessage( hwndDlg, IDC_COMBO_DETAIL, CB_INSERTSTRING, i, (LPARAM)(LPCTSTR)szItemsDetail[i] );

			SendDlgItemMessage( hwndDlg, IDC_COMBO_DETAIL, CB_SETCURSEL, g_i32Detail, 0 );

			return TRUE;
		}
		break;

	case WM_COMMAND:
		{
			WORD	wNotifyCode = HIWORD(wParam);	// notification code 
			WORD	wID = LOWORD(wParam);			// item, control, or accelerator identifier 
			HWND	hwndCtl = (HWND)lParam;			// handle of control 

			if( wNotifyCode == BN_CLICKED ) {
				if( wID == IDOK ) {

					g_bFullscreen = (IsDlgButtonChecked( hwndDlg, IDC_CHECK_FULLSCREEN ) == BST_CHECKED) ? true : false;
					g_i32Resolution = SendDlgItemMessage( hwndDlg, IDC_COMBO_RESOLUTION, CB_GETCURSEL, 0, 0 );
					g_i32Detail = SendDlgItemMessage( hwndDlg, IDC_COMBO_DETAIL, CB_GETCURSEL, 0, 0 );

					EndDialog( hwndDlg, IDOK );
				}
				else if( wID == IDCANCEL )
					EndDialog( hwndDlg, IDCANCEL );
			}
		}

	default:
		return FALSE;
		break;
	}
}
#endif


#ifdef WIN32
int APIENTRY WinMain( HINSTANCE hInstance,
                      HINSTANCE hPrevInstance,
                      LPSTR     lpCmdLine,
                      int       nCmdShow )
#else
int main(int argc, char * * argv)
#endif
{

	GLWindowC	rWnd;


	// Show init dialog.
#ifdef WIN32
	if( DialogBox( hInstance, MAKEINTRESOURCE( IDD_INITDIALOG ), NULL, InitDlgProc ) != IDOK ) {
		return 0;
	}
#endif

	int32	i32Fullscreen, i32Width, i32Height, i32BPP;
	int32	i32ReflTexWidth = 0, i32ReflTexHeight = 0;
	bool	bReflections = false;

	i32Fullscreen = g_bFullscreen ? GRAPHICSDEVICE_CREATE_FULLSCREEN : GRAPHICSDEVICE_CREATE_WINDOWED;

	switch( g_i32Resolution ) {
	case 0: i32Width = 640; i32Height = 480; i32BPP = 16; break;
	case 1: i32Width = 800; i32Height = 600; i32BPP = 16; break;
	case 2: i32Width = 1024; i32Height = 768; i32BPP = 16; break;
	case 3: i32Width = 640; i32Height = 480; i32BPP = 32; break;
	case 4: i32Width = 800; i32Height = 600; i32BPP = 32; break;
	case 5: i32Width = 1024; i32Height = 768; i32BPP = 32; break;
	default: TRACE( "UGH!\n" ); break;
	};

	switch( g_i32Detail ) {
	case 0: i32ReflTexWidth = 512; i32ReflTexHeight = 512; bReflections = true; break;		// Excellent
	case 1: i32ReflTexWidth = 512; i32ReflTexHeight = 256; bReflections = true; break;		// good
	case 2: i32ReflTexWidth = 256; i32ReflTexHeight = 256; bReflections = true; break;		// med
	case 3: i32ReflTexWidth = 0; i32ReflTexHeight = 0; bReflections = false; break;			// poor
	}

	if( i32ReflTexHeight > i32Height )
		i32ReflTexHeight /= 2;

#if USE_SDL2
	if(SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO) < 0) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to init SDL2 : %s\n", SDL_GetError());
		return -1;
	}
	rWnd.init(i32Fullscreen, i32Width, i32Height, i32BPP );
#elif defined(WIN32)
	rWnd.init( hInstance, i32Fullscreen, i32Width, i32Height, i32BPP );
#endif

	if( !init_extensions() ) {
		rWnd.destroy();
#ifdef WIN32
		::MessageBox( NULL, "3D hardware is not present or\ndoes not support the required attributes (needs at least OpenGL 1.2 support)" , "Error", MB_OK | MB_ICONERROR );
#endif
		return -1;
	}


	//
	// Init FSOUND
	//

#ifdef FMOD_VERSION
	FSOUND_SetOutput( FSOUND_OUTPUT_DSOUND );
	if( !FSOUND_Init( 44100, 32, 0 ) ) {
		rWnd.destroy();
		::MessageBox( NULL, FMOD_ErrorString( FSOUND_GetError() ), "FMOD Error", MB_OK | MB_ICONERROR );
		return -1;
	}

	FSOUND_STREAM*	pWaterStream = 0;
	pWaterStream = FSOUND_Stream_OpenFile( "soundfx/water.mp3", FSOUND_LOOP_NORMAL, 0 );
	if( !pWaterStream ) {
		rWnd.destroy();
		::MessageBox( NULL, "Loading the soundfx failed." , "Error", MB_OK | MB_ICONERROR );
		FSOUND_Close();
		return -1;
	}


	FSOUND_STREAM*	pAmbientStream = 0;
	pAmbientStream = FSOUND_Stream_OpenFile( "soundfx/ambient1.mp3", FSOUND_LOOP_NORMAL, 0 );
	if( !pAmbientStream ) {
		rWnd.destroy();
		::MessageBox( NULL, "Loading the soundfx failed." , "Error", MB_OK | MB_ICONERROR );
		FSOUND_Close();
		return -1;
	}
#endif

#ifdef WIN32
	MSG	rMsg;
#endif

	TextureC*	pEskimoImage = new TextureC;
	if( !pEskimoImage->load( "maps/eskimoimage.tga" ) )
		OutputDebugString( "cannot load eskimo image texture\n" );

	TextureC*	pMenuImage = new TextureC;
	if( !pMenuImage->load( "maps/menu2.tga" ) )
		OutputDebugString( "cannot load menu image texture\n" );

	TextureC*	pLogoImage = new TextureC;
	if( !pLogoImage->load( "maps/kylpyamme_logo_small.tga" ) )
		OutputDebugString( "cannot load logo image texture\n" );

	TextureC*	pBaitMeterImage = new TextureC;
	if( !pBaitMeterImage->load( "maps/bait_meter_base.tga" ) )
		OutputDebugString( "cannot load logo image texture\n" );

	TextureC*	pBaitMeterScaleImage = new TextureC;
	if( !pBaitMeterScaleImage->load( "maps/bait_meter_scale.tga" ) )
		OutputDebugString( "cannot load bait meter scale image texture\n" );


	FontC*	pFont = new FontC;
	if( !pFont->load( "data/Futura Hv BT.mofo" ) )
		OutputDebugString( "cannot load font\n" );

	FontC*	pFontBlur = new FontC;
	if( !pFontBlur->load( "data/Futura Hv BT blur.mofo" ) )
		OutputDebugString( "cannot load font\n" );



	FishingContextC*	pCtx = new FishingContextC;


	pCtx->set_render_reflections( bReflections );
	pCtx->set_reflections_tex_size( i32ReflTexWidth, i32ReflTexHeight );

	pCtx->set_catch_height( 45 );
	pCtx->load_sound( SOUND_BAIT_HIT_WATER, "soundfx/blop1.wav" );
	pCtx->load_sound( SOUND_BAIT_LEFT_WATER, "soundfx/blop2.wav" );
	pCtx->load_sound( SOUND_FISH_CATCH, "soundfx/catch.mp3" );
	pCtx->load_sound( SOUND_FISH_MISSED, "soundfx/blop2.wav" );


	FishingRodC*	pRod = new FishingRodC;

	pRod->set_angle_x( 45 );
	pRod->set_angle_y( 0 );
	
	pRod->set_bait_mesh( "data/bait.ase" );
	pRod->set_line_length( 70 );

	pRod->set_pos( Vector3C( 30, 10, 250 ) );
	pRod->set_rod_mesh( "data/rod.ase" );
	pRod->set_length( 170.0f );

	pRod->init( pCtx );

	pRod->init_baits( 20 );
	pRod->set_reload_distance( 200 );


	int32	i32Time;
	int32	i32TimeStep = 1000 / 30;	// 30 fps
	int32	i32MaxIter = 30;
	int32	i32Iter;
	float32	f32Frame = 0;
	int32	i32LastTime = get_time();


	enum TimerTypeE {
		TIMERTYPE_ELAPSED,
		TIMERTYPE_COUNTDOWN,
	};
	int32	i32TimerType;
	int32	i32TimerStart;
	int32	i32TimerDiff;

	if( pCtx->get_timer() ) {
		i32TimerType = TIMERTYPE_COUNTDOWN;
		i32TimerStart = get_time() + pCtx->get_timer();
	}
	else {
		i32TimerType = TIMERTYPE_ELAPSED;
		i32TimerStart = get_time();
	}


	enum GameStateE {
		GAMESTATE_MAIN_MENU,
		GAMESTATE_RESUME_MENU,
		GAMESTATE_HIGH_SCORES,
		GAMESTATE_LEVEL_BRIEFING,
		GAMESTATE_LEVEL_GAMEOVER,
		GAMESTATE_LEVEL_PLAYING,
		GAMESTATE_LEVEL_PAUSED,
		GAMESTATE_LEVEL_COMPLETE,
	};


	enum GameLevelsE {
		GAMELEVEL_MENU = 0,
		GAMELEVEL_STAGE_11,
		GAMELEVEL_STAGE_12,
		GAMELEVEL_STAGE_21,
		GAMELEVEL_STAGE_22,
		GAMELEVEL_STAGE_31,
		GAMELEVEL_STAGE_32,
		GAMELEVEL_STAGE_41,
		GAMELEVEL_STAGE_42,
		GAME_MAXLEVELS,
	};

	char	szLevels[GAME_MAXLEVELS][128] = {
		"levels/menu.lev",
		"levels/desert.lev",
		"levels/desert_night.lev",
		"levels/summer.lev",
		"levels/autumn.lev",
		"levels/cave1.lev",
		"levels/cave2.lev",
		"levels/winter.lev",
		"levels/final.lev",
	};

	int32	i32GameState = GAMESTATE_MAIN_MENU;
	bool	bLoadLevel = true;
	int32	i32Level = GAMELEVEL_MENU;
	bool	bQuit = false;




	glShadeModel( GL_SMOOTH );
	glEnable( GL_NORMALIZE  );
	glEnable( GL_LIGHTING );
	glCullFace( GL_BACK );
	glEnable( GL_CULL_FACE );
	glLightModeli( GL_LIGHT_MODEL_TWO_SIDE , 0 );
	glEnable( GL_BLEND );
	glBlendFunc( GL_ONE, GL_ZERO );

	glEnable( GL_DEPTH_TEST );
	glDepthFunc( GL_LEQUAL );


	float32	f32BaitForce = 0;


	int32	i32MaxLevel = 0;

	{
		FILE*	pStream = fopen( "config.dat", "r" );
		if( pStream ) {
			fread( &i32MaxLevel, sizeof( uint32 ), 1, pStream );
			i32MaxLevel ^= 0xdeadbeef;
			fclose( pStream );
		}

		if( i32MaxLevel < 0 )
			i32MaxLevel = 0;
		if( i32MaxLevel > GAMELEVEL_STAGE_42 )
			i32MaxLevel = GAMELEVEL_STAGE_42;
	}


#ifdef FMOD_VERSION
	int32	i32WaterCh = FSOUND_Stream_Play( FSOUND_FREE, pWaterStream );
	int32	i32AmbCh = FSOUND_Stream_Play( FSOUND_FREE, pAmbientStream );
	FSOUND_SetVolume( i32AmbCh, 150 );
#endif

	do {
#ifdef USE_SDL2
		SDL_Event event;
		if (SDL_PollEvent(&event)) {
			if (event.window.event == SDL_WINDOWEVENT_CLOSE)
				bQuit = true;
		}
#elif defined(WIN32)
		if( PeekMessage( &rMsg , rWnd.get_hwnd() , 0 , 0 , PM_REMOVE ) ) {
			TranslateMessage( &rMsg ) ;
			DispatchMessage( &rMsg );
		}
		else
#endif
		{


			//
			// Load level if it's not loaded
			//

			if( bLoadLevel ) {

				glViewport( 0, 0, rWnd.get_width(), rWnd.get_height() );
				glEnable( GL_SCISSOR_TEST );
				glScissor( 0, 0, rWnd.get_width(), rWnd.get_height() );
				rWnd.clear_device( GRAPHICSDEVICE_ALLBUFFERS );

				// UI
				glMatrixMode( GL_PROJECTION );
				glLoadIdentity();
				gluOrtho2D( 0, rWnd.get_width(), 0, rWnd.get_height() );

				glMatrixMode( GL_MODELVIEW );
				glLoadIdentity();

				glDisable( GL_LIGHTING );
				glDisable( GL_CULL_FACE );
				glDisable( GL_DEPTH_TEST );

				glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

				glEnable( GL_TEXTURE_2D );
				glColor4ub( 255, 255, 255, 255 );

				pFont->bind();
				pFont->draw_text( 50, 40, "loading...", 0.5f );

				glDisable( GL_TEXTURE_2D );
				glBlendFunc( GL_ONE, GL_ZERO );
				glEnable( GL_LIGHTING );
				glEnable( GL_CULL_FACE );
				glEnable( GL_DEPTH_TEST );

				rWnd.flush();


				if( i32Level == GAMELEVEL_MENU )
					pCtx->set_rod( 0 );
				else
					pCtx->set_rod( pRod );

				// load and init level
				pCtx->load_level( szLevels[i32Level] );
				pCtx->init_level();

				bLoadLevel = false;
			}




			//
			// Do user input, animation and simulation
			//


			int32	i32MouseX, i32MouseY;
			i32MouseX = rWnd.get_mouse_x();
			i32MouseY = (rWnd.get_height() - 1) - rWnd.get_mouse_y();

			i32Time = get_time();
			i32Iter = 0;

			float32	f32AngleX, f32AngleY;

			f32BaitForce = 0;

			if( i32GameState == GAMESTATE_LEVEL_PLAYING ) {
				f32AngleY = (((1.0f - (float32)rWnd.get_mouse_x() / (float32)rWnd.get_width()) * 120.0f - 60.0f) / 180.0f) * M_PI;
				f32AngleX = (((1.0f - (float32)rWnd.get_mouse_y() / (float32)rWnd.get_height()) * 80.0f + 5.0f) / 180.0f) * M_PI;

				// Electronic bate control
				if( rWnd.get_mouse_button() == MOUSE_LBUTTON ) {
					f32BaitForce = 1.0f;
				}
				else if( rWnd.get_mouse_button() == MOUSE_RBUTTON ) {
					f32BaitForce = -1.0f;
				}

				rWnd.get_pressed( MOUSE_LBUTTON | MOUSE_RBUTTON );	// clear states
			}
			else {
				f32AngleY = 0;
				f32AngleX = (float32)(45.0 / 180.0 * M_PI);
			}


			pRod->set_angle_x( f32AngleX );
			pRod->set_angle_y( f32AngleY );

			// Do game simulation 30 fps
			while( (i32Time >= i32LastTime) && (i32Iter < i32MaxIter) ) {

				pRod->set_bait_force( f32BaitForce );

				pCtx->set_frame( f32Frame );
				pCtx->update();

				f32Frame += 0.005f;

				i32LastTime += i32TimeStep;
				i32Iter++;

				// damp bait force
/*				if( f32BaitForce > 0 )
					f32BaitForce -= 1.0f;
				if( f32BaitForce < 0 )
						f32BaitForce = 0;*/
			}


			//
			// Draw stuff
			//

			pCtx->draw( &rWnd );



			// UI
			glMatrixMode( GL_PROJECTION );
			glLoadIdentity();
			gluOrtho2D( 0, rWnd.get_width(), 0, rWnd.get_height() );

			glMatrixMode( GL_MODELVIEW );
			glLoadIdentity();

			glDisable( GL_LIGHTING );
			glDisable( GL_CULL_FACE );
			glDisable( GL_DEPTH_TEST );

			// Game UI

			glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

			glEnable( GL_TEXTURE_2D );
			glColor4ub( 255, 255, 255, 255 );



			char	szMsg[256];
			float32	f32TxtLen;


			if( !bLoadLevel && i32GameState == GAMESTATE_LEVEL_PAUSED ) {
				//
				// Level paused display
				//

				// stall time
				i32TimerStart = get_time() - i32TimerDiff;

				enum PauseMenuE {
					PAUSEMENU_CONTINUE = 1,
					PAUSEMENU_QUIT,
				};
				int32	i32Sel = 0;

				// Level complete text
				f32TxtLen = pFont->get_text_length( "Pause", 1.0f );

				f32TxtLen /= 2.0f;

				glColor4ub( 0, 0, 0, 128 );
				pFontBlur->bind();
				pFontBlur->draw_text( rWnd.get_width() / 2 - f32TxtLen + 3, rWnd.get_height() / 2 - 3 + 40, "Pause", 1.0f );

				glColor3ub( 233, 76, 7 );
				pFont->bind();
				pFont->draw_text( rWnd.get_width() / 2 - f32TxtLen, rWnd.get_height() / 2 + 40, "Pause", 1.0f );

				// continue
				f32TxtLen = pFont->get_text_length( "Continue", 0.6f );

				f32TxtLen /= 2.0f;

				glColor4ub( 0, 0, 0, 128 );
				pFontBlur->bind();
				pFontBlur->draw_text( rWnd.get_width() / 2 - f32TxtLen + 3, rWnd.get_height() / 2 - 3, "Continue", 0.6f );

				if( i32MouseX >= (rWnd.get_width() / 2 - f32TxtLen) && i32MouseX < (rWnd.get_width() / 2 + f32TxtLen) &&
					i32MouseY >= (rWnd.get_height() / 2 - 5) && i32MouseY < (rWnd.get_height() / 2 + 20) ) {
					glColor3ub( 233, 76, 7 );
					i32Sel = PAUSEMENU_CONTINUE;
				}
				else
					glColor3ub( 255, 255, 255 );
				pFont->bind();
				pFont->draw_text( rWnd.get_width() / 2 - f32TxtLen, rWnd.get_height() / 2, "Continue", 0.6f );


				// main menu

				glColor4ub( 0, 0, 0, 128 );
				pFontBlur->bind();
				pFontBlur->draw_text( rWnd.get_width() / 2 - f32TxtLen + 3, rWnd.get_height() / 2 - 30 - 3, "Main Menu", 0.6f );

				if( i32MouseX >= (rWnd.get_width() / 2 - f32TxtLen) && i32MouseX < (rWnd.get_width() / 2 + f32TxtLen) &&
					i32MouseY >= (rWnd.get_height() / 2 - 30) && i32MouseY < (rWnd.get_height() / 2 - 30 + 20) ) {
					glColor3ub( 233, 76, 7 );
					i32Sel = PAUSEMENU_QUIT;
				}
				else
					glColor3ub( 255, 255, 255 );
				pFont->bind();
				pFont->draw_text( rWnd.get_width() / 2 - f32TxtLen, rWnd.get_height() / 2 - 30, "Main Menu", 0.6f );



				if( rWnd.get_pressed( MOUSE_LBUTTON ) ) {
					if( i32Sel == PAUSEMENU_QUIT ) {
						i32Level = GAMELEVEL_MENU;
						bLoadLevel = true;
						i32GameState = GAMESTATE_MAIN_MENU;
					}
					else if( i32Sel == PAUSEMENU_CONTINUE ) {
						i32GameState = GAMESTATE_LEVEL_PLAYING;
					}
					else if( i32MouseX == 0 && i32MouseY == 0 )
						i32GameState = GAMESTATE_LEVEL_COMPLETE;
				}

				// continue to play if mouse is pressed			
				if( rWnd.get_pressed( MOUSE_LBUTTON ) ) {
					i32GameState = GAMESTATE_LEVEL_PLAYING;
				}
			}

			if( !bLoadLevel && i32GameState == GAMESTATE_LEVEL_COMPLETE ) {
				//
				// Level complete display
				//


				// Level complete text

				char*	szTxtComplete;
				char	szLevelComplete[] = "Level Complete";
				char	szGameComplete[] = "Game Complete";

				if( i32Level == GAMELEVEL_STAGE_42 )
					szTxtComplete = szGameComplete;
				else
					szTxtComplete = szLevelComplete;

				f32TxtLen = pFont->get_text_length( szTxtComplete, 1.0f );

				f32TxtLen /= 2.0f;

				glColor4ub( 0, 0, 0, 128 );
				pFontBlur->bind();
				pFontBlur->draw_text( rWnd.get_width() / 2 - f32TxtLen + 3, rWnd.get_height() / 2 - 3 + 40, szTxtComplete, 1.0f );

				glColor3ub( 233, 76, 7 );
				pFont->bind();
				pFont->draw_text( rWnd.get_width() / 2 - f32TxtLen, rWnd.get_height() / 2 + 40, szTxtComplete, 1.0f );


				// score
				snprintf( szMsg, 255, "%d", pCtx->get_score() );
				f32TxtLen = pFont->get_text_length( szMsg, 1 );

				f32TxtLen /= 2.0f;

				glColor4ub( 0, 0, 0, 128 );
				pFontBlur->bind();
				pFontBlur->draw_text( rWnd.get_width() / 2 - f32TxtLen + 3, rWnd.get_height() / 2 - 3, szMsg );

				glColor3ub( 255, 255, 255 );
				pFont->bind();
				pFont->draw_text( rWnd.get_width() / 2 - f32TxtLen, rWnd.get_height() / 2, szMsg );


				f32TxtLen += 10;
				f32TxtLen += pFont->get_text_length( "SCORE", 0.4f );

				glColor4ub( 0, 0, 0, 128 );
				pFontBlur->bind();
				pFontBlur->draw_text( rWnd.get_width() / 2 - f32TxtLen + 1, rWnd.get_height() / 2 + 10 - 2, "SCORE", 0.4f );

				glColor3ub( 179, 218, 220 );	// light greyish blue
				pFont->bind();
				pFont->draw_text( rWnd.get_width() / 2 - f32TxtLen, rWnd.get_height() / 2 + 10, "SCORE", 0.4f );


				// continue to play if mouse is pressed			
				if( rWnd.get_pressed( MOUSE_LBUTTON ) ) {
					i32Level++;
					if( i32Level > GAMELEVEL_STAGE_42 ) {
						i32Level = GAMELEVEL_STAGE_42;
						i32GameState = GAMESTATE_LEVEL_GAMEOVER;
					}
					else {
						if( i32Level > i32MaxLevel )
							i32MaxLevel = i32Level;
						i32GameState = GAMESTATE_LEVEL_BRIEFING;
						bLoadLevel = true;
					}

				}
			}

			if( !bLoadLevel && i32GameState == GAMESTATE_LEVEL_GAMEOVER ) {
				//
				// Game over
				//

/*				f32TxtLen = pFont->get_text_length( "Game Over", 1 );

				f32TxtLen /= 2.0f;

				glColor4ub( 0, 0, 0, 128 );
				pFontBlur->bind();
				pFontBlur->draw_text( rWnd.get_width() / 2 - f32TxtLen + 3, rWnd.get_height() / 2 - 3, "Game Over" );

				glColor3ub( 233, 76, 7 );		// orangeish red
				pFont->bind();
				pFont->draw_text( rWnd.get_width() / 2 - f32TxtLen, rWnd.get_height() / 2, "Game Over" );
*/


				// Game over text
				f32TxtLen = pFont->get_text_length( "Game Over", 1.0f );

				f32TxtLen /= 2.0f;

				glColor4ub( 0, 0, 0, 128 );
				pFontBlur->bind();
				pFontBlur->draw_text( rWnd.get_width() / 2 - f32TxtLen + 3, rWnd.get_height() / 2 - 3 + 40, "Game Over", 1.0f );

				glColor3ub( 233, 76, 7 );		// orangeish red
				pFont->bind();
				pFont->draw_text( rWnd.get_width() / 2 - f32TxtLen, rWnd.get_height() / 2 + 40, "Game Over", 1.0f );


				// score
				snprintf( szMsg, 255, "%d", pCtx->get_score() );
				f32TxtLen = pFont->get_text_length( szMsg, 1 );

				f32TxtLen /= 2.0f;

				glColor4ub( 0, 0, 0, 128 );
				pFontBlur->bind();
				pFontBlur->draw_text( rWnd.get_width() / 2 - f32TxtLen + 3, rWnd.get_height() / 2 - 3, szMsg );

				glColor3ub( 255, 255, 255 );
				pFont->bind();
				pFont->draw_text( rWnd.get_width() / 2 - f32TxtLen, rWnd.get_height() / 2, szMsg );


				f32TxtLen += 10;
				f32TxtLen += pFont->get_text_length( "SCORE", 0.4f );

				glColor4ub( 0, 0, 0, 128 );
				pFontBlur->bind();
				pFontBlur->draw_text( rWnd.get_width() / 2 - f32TxtLen + 1, rWnd.get_height() / 2 + 10 - 2, "SCORE", 0.4f );

				glColor3ub( 179, 218, 220 );	// light greyish blue
				pFont->bind();
				pFont->draw_text( rWnd.get_width() / 2 - f32TxtLen, rWnd.get_height() / 2 + 10, "SCORE", 0.4f );


				// continue to play if mouse is pressed			
				if( rWnd.get_pressed( MOUSE_LBUTTON ) ) {
					i32Level = GAMELEVEL_MENU;
					bLoadLevel = true;
					i32GameState = GAMESTATE_MAIN_MENU;
				}
			}

			if( !bLoadLevel && i32GameState == GAMESTATE_LEVEL_BRIEFING ) {
				//
				// Level brief display
				//

				// dim the background
				glDisable( GL_TEXTURE_2D );
				glColor4fv( pCtx->get_level_brief_color() );
				glBegin( GL_QUADS );
					glVertex2f( 0, rWnd.get_height() );
					glVertex2f( rWnd.get_width(), rWnd.get_height() );
					glVertex2f( rWnd.get_width(), 0 );
					glVertex2f( 0, 0 );
				glEnd();
				glEnable( GL_TEXTURE_2D );


				// draw the eskimo picture
				pEskimoImage->bind();
				glColor4ub( 255, 255, 255, 255 );
				glBegin( GL_QUADS );
					glTexCoord2f( 0, 1 );
					glVertex2f( 30, 200 );

					glTexCoord2f( 1, 1 );
					glVertex2f( 20 + pEskimoImage->get_width(), 200 );

					glTexCoord2f( 1, 0 );
					glVertex2f( 20 + pEskimoImage->get_width(), 200 + pEskimoImage->get_height() );

					glTexCoord2f( 0, 0 );
					glVertex2f( 20,  200 + pEskimoImage->get_height() );
				glEnd();


				glColor4ub( 0, 0, 0, 128 );
				pFontBlur->bind();
				pFontBlur->draw_text( 290 + 2, 390 - 4, pCtx->get_level_brief_name(), 1.0f );

				glColor3ub( 179, 218, 220 );	// light greyish blue
				pFont->bind();
				pFont->draw_text( 290, 390, pCtx->get_level_brief_name(), 1.0f );

				glColor4ub( 255, 255, 255, 255 );
				pFont->draw_text( 300, 350, pCtx->get_level_brief_desc(), 0.4f );

				// continue to play if mouse is pressed			
				if( rWnd.get_pressed( MOUSE_LBUTTON ) ) {
					i32GameState = GAMESTATE_LEVEL_PLAYING;
				}
			}

			if( !bLoadLevel && i32GameState == GAMESTATE_LEVEL_PLAYING ) {
				//
				//
				// On game display
				//
				//

				// Time

				int32	i32Timer;

				if( i32TimerType == TIMERTYPE_COUNTDOWN )
					i32Timer = i32TimerStart - get_time();		// count down timer
				else
					i32Timer = get_time() - i32TimerStart;		//  elapsed
				
				int32	i32Sec = i32Timer / 1000;
				int32	i32Min = 0;
				while( i32Sec >= 60 ) {
					i32Min++;
					i32Sec -= 60;
				}
				snprintf( szMsg, 255, "%d:%02d", i32Min, i32Sec );
				f32TxtLen = pFont->get_text_length( szMsg, 1 );

				glColor4ub( 0, 0, 0, 128 );
				pFontBlur->bind();
				pFontBlur->draw_text( rWnd.get_width() / 2 - f32TxtLen / 2.0f + 3, rWnd.get_height() - 60 - 3, szMsg );

				if( i32TimerType == TIMERTYPE_COUNTDOWN ) {
					if( i32Min == 0 && i32Sec < 30 )
						glColor3ub( 233, 76, 7 );		// orangeish red
					else
						glColor3ub( 255, 180, 0 );		// orange
				}
				else {
					glColor3ub( 255, 240, 0 );	// warm yellow
				}
				
				pFont->bind();
				pFont->draw_text( rWnd.get_width() / 2 - f32TxtLen / 2.0f, rWnd.get_height() - 60, szMsg );


				// fish count

				//glColor3ub( 179, 218, 220 );	// light greyish blue
				//glColor3ub( 255, 240, 0 );	// warm yellow
				//glColor3ub( 233, 76, 7 );		// orangeish red

				f32TxtLen = pFont->get_text_length( "CATCH", 0.3f );

				glColor4ub( 0, 0, 0, 128 );
				pFontBlur->bind();
				pFontBlur->draw_text( 70 - f32TxtLen + 1, rWnd.get_height() - 45 - 1, "CATCH", 0.3f );

				glColor3ub( 179, 218, 220 );	// light greyish blue
				pFont->bind();
				pFont->draw_text( 70 - f32TxtLen, rWnd.get_height() - 45, "CATCH", 0.3f );


				snprintf( szMsg, 255, "%d/%02d", pCtx->get_pond()->get_fish_count() - pCtx->get_pond()->get_alive_fish_count(), pCtx->get_pond()->get_fish_count() );
				f32TxtLen = pFont->get_text_length( szMsg, 0.7f );

				glColor4ub( 0, 0, 0, 128 );
				pFontBlur->bind();
				pFontBlur->draw_text( 75 + 2, rWnd.get_height() - 50 - 2, szMsg, 0.7f );

				glColor3ub( 255, 255, 255 );
				pFont->bind();
				pFont->draw_text( 75, rWnd.get_height() - 50, szMsg, 0.7f );


				// bait count

				//glColor3ub( 179, 218, 220 );	// light greyish blue
				//glColor3ub( 255, 240, 0 );	// warm yellow
				//glColor3ub( 233, 76, 7 );		// orangeish red

				f32TxtLen = pFont->get_text_length( "BAITS LEFT", 0.3f );

				glColor4ub( 0, 0, 0, 128 );
				pFontBlur->bind();
				pFontBlur->draw_text( 70 - f32TxtLen + 1, rWnd.get_height() - 75 - 1, "BAITS LEFT", 0.3f );

				glColor3ub( 179, 218, 220 );	// light greyish blue
				pFont->bind();
				pFont->draw_text( 70 - f32TxtLen, rWnd.get_height() - 75, "BAITS LEFT", 0.3f );


				snprintf( szMsg, 255, "%d", pRod->get_bait_count() );
				f32TxtLen = pFont->get_text_length( szMsg, 0.5f );

				glColor4ub( 0, 0, 0, 128 );
				pFontBlur->bind();
				pFontBlur->draw_text( 78 + 2, rWnd.get_height() - 78 - 2, szMsg, 0.5f );

				glColor3ub( 255, 255, 255 );
				pFont->bind();
				pFont->draw_text( 78, rWnd.get_height() - 78, szMsg, 0.5f );


				f32TxtLen = pFont->get_text_length( "DAMAGE", 0.3f );

				glColor4ub( 0, 0, 0, 128 );
				pFontBlur->bind();
				pFontBlur->draw_text( 70 - f32TxtLen + 1, rWnd.get_height() - 105 - 1, "FRESHNESS", 0.3f );

				glColor3ub( 179, 218, 220 );	// light greyish blue
				pFont->bind();
				pFont->draw_text( 70 - f32TxtLen, rWnd.get_height() - 105, "FRESHNESS", 0.3f );


				pBaitMeterImage->bind();
				glColor4ub( 255, 255, 255, 255 );
				glBegin( GL_QUADS );
					glTexCoord2f( 0, 1 );
					glVertex2f( 75, rWnd.get_height() - 90 - pBaitMeterImage->get_height() );

					glTexCoord2f( 1, 1 );
					glVertex2f( 75 + pBaitMeterImage->get_width(), rWnd.get_height() - 90 - pBaitMeterImage->get_height() );

					glTexCoord2f( 1, 0 );
					glVertex2f( 75 + pBaitMeterImage->get_width(), rWnd.get_height() - 90 );

					glTexCoord2f( 0, 0 );
					glVertex2f( 75, rWnd.get_height() - 90 );
				glEnd();

				pBaitMeterScaleImage->bind();
				glColor4ub( 255, 255, 255, 255 );
				float32	f32U = (4 + 50 - pRod->get_bait_freshness() / 2) / (float32)pBaitMeterScaleImage->get_width();
				glBegin( GL_QUADS );
					glTexCoord2f( 0, 1 );
					glVertex2f( 75, rWnd.get_height() - 90 - pBaitMeterScaleImage->get_height() );

					glTexCoord2f( f32U, 1 );
					glVertex2f( 75 + 4 + 50 - (int32)(pRod->get_bait_freshness() / 2), rWnd.get_height() - 90 - pBaitMeterScaleImage->get_height() );

					glTexCoord2f( f32U, 0 );
					glVertex2f( 75 + 4 + 50 - (int32)(pRod->get_bait_freshness() / 2), rWnd.get_height() - 90 );

					glTexCoord2f( 0, 0 );
					glVertex2f( 75, rWnd.get_height() - 90 );
				glEnd();


				// score
				f32TxtLen = pFont->get_text_length( "SCORE", 0.3f );

				glColor4ub( 0, 0, 0, 128 );
				pFontBlur->bind();
				pFontBlur->draw_text( rWnd.get_width() - 150 - f32TxtLen + 1, rWnd.get_height() - 45 - 1, "SCORE", 0.3f );

				glColor3ub( 179, 218, 220 );	// light greyish blue
				pFont->bind();
				pFont->draw_text( rWnd.get_width() - 150 - f32TxtLen, rWnd.get_height() - 45, "SCORE", 0.3f );


				snprintf( szMsg, 255, "%d", pCtx->get_score() );
				f32TxtLen = pFont->get_text_length( szMsg, 0.7f );

				glColor4ub( 0, 0, 0, 128 );
				pFontBlur->bind();
				pFontBlur->draw_text( rWnd.get_width() - 145 + 2, rWnd.get_height() - 50 - 2, szMsg, 0.7f );

				glColor3ub( 255, 255, 255 );
				pFont->bind();
				pFont->draw_text( rWnd.get_width() - 145, rWnd.get_height() - 50, szMsg, 0.7f );


				// if all the fish is eaten, goto next level
				if( pCtx->get_pond()->get_alive_fish_count() == 0 ) {
					i32GameState = GAMESTATE_LEVEL_COMPLETE;
				}

				// if all the baits are used, game over
				if( pRod->get_bait_count() < 0 ) {
					i32GameState = GAMESTATE_LEVEL_GAMEOVER;
				}

				// if elapsed time is zero, game over
				if( i32TimerType == TIMERTYPE_COUNTDOWN && i32Timer < 1 ) {
					i32GameState = GAMESTATE_LEVEL_GAMEOVER;
				}


				// if ESC is hit, goto main menu
				if( rWnd.get_pressed( KEYB_ESC ) ) {
//					i32GameState = GAMESTATE_LEVEL_COMPLETE;
//					i32GameState = GAMESTATE_LEVEL_GAMEOVER;
					i32GameState = GAMESTATE_LEVEL_PAUSED;
					i32TimerDiff = get_time() - i32TimerStart;
/*					i32Level = GAMELEVEL_MENU;
					bLoadLevel = true;
					i32GameState = GAMESTATE_MAIN_MENU;*/
				}

			}


			int32	i32CenterX, i32CenterY;

			if( !bLoadLevel && (i32GameState == GAMESTATE_MAIN_MENU ||
				i32GameState == GAMESTATE_RESUME_MENU || i32GameState == GAMESTATE_HIGH_SCORES) ) {

				//
				//
				// Menu
				//
				//

				// dim the background
				glDisable( GL_TEXTURE_2D );
				glColor4ub( 180, 212, 216, 128 );
				glBegin( GL_QUADS );
					glVertex2f( 0, rWnd.get_height() );
					glVertex2f( rWnd.get_width(), rWnd.get_height() );
					glVertex2f( rWnd.get_width(), 0 );
					glVertex2f( 0, 0 );
				glEnd();
				glEnable( GL_TEXTURE_2D );

				// draw the kylpyamme logo
				pLogoImage->bind();
				glColor4ub( 255, 255, 255, 255 );
				glBegin( GL_QUADS );
					glTexCoord2f( 0, 1 );
					glVertex2f( rWnd.get_width() - pLogoImage->get_width() - 15, -10 );

					glTexCoord2f( 1, 1 );
					glVertex2f( rWnd.get_width() - pLogoImage->get_width() - 15 + pLogoImage->get_width(), -10 );

					glTexCoord2f( 1, 0 );
					glVertex2f( rWnd.get_width() - pLogoImage->get_width() - 15 + pLogoImage->get_width(), -10 + pLogoImage->get_height() );

					glTexCoord2f( 0, 0 );
					glVertex2f( rWnd.get_width() - pLogoImage->get_width() - 15, -10 + pLogoImage->get_height() );
				glEnd();


				// draw the eskimo picture
				pMenuImage->bind();
				glColor4ub( 255, 255, 255, 255 );

				i32CenterX = (rWnd.get_width() / 2) - (pMenuImage->get_width() / 2);
				i32CenterY = (rWnd.get_height() / 2) - (pMenuImage->get_height() / 2);

				glBegin( GL_QUADS );
					glTexCoord2f( 0, 1 );
					glVertex2f( i32CenterX, i32CenterY );

					glTexCoord2f( 1, 1 );
					glVertex2f( i32CenterX + pMenuImage->get_width(), i32CenterY );

					glTexCoord2f( 1, 0 );
					glVertex2f( i32CenterX + pMenuImage->get_width(), i32CenterY + pMenuImage->get_height() );

					glTexCoord2f( 0, 0 );
					glVertex2f( i32CenterX, i32CenterY + pMenuImage->get_height() );
				glEnd();
			}


			if( !bLoadLevel && i32GameState == GAMESTATE_HIGH_SCORES ) {
				//
				// High Scores
				//

				char	szNames[10][6] = { "Memon", "Gurmo", "MMMMM", "37337", "Funga", "Memon", "Gurmo", "MMMMM", "37337", "Funga" };

				for( uint32 i = 0; i < 10; i++ ) {

					// Rank
					snprintf( szMsg, 255, "%d.", i + 1 );
					glColor3ub( 0, 0, 0 );
					pFont->bind();
					pFont->draw_text( i32CenterX + 270, i32CenterY + 250 - i * 20 + 2, szMsg, 0.3f );

					// Name
					glColor4ub( 0, 0, 0, 128 );
					pFontBlur->bind();
					pFontBlur->draw_text( i32CenterX + 270 + 23 + 1, i32CenterY + 250 - i * 20 - 3, szNames[i], 0.5f );

					if( i < 3 )
						glColor3ub( 255, 240, 0 );	// warm yellow
					else
						glColor3ub( 179, 218, 220 );	// light greyish blue
					pFont->bind();
					pFont->draw_text( i32CenterX + 270 + 23, i32CenterY + 250 - i * 20, szNames[i], 0.5f );

					// Score
					snprintf( szMsg, 255, "%d", (10 - i) * 3133 );
					f32TxtLen = pFont->get_text_length( szMsg, 0.5f );

					glColor4ub( 0, 0, 0, 128 );
					pFontBlur->bind();
					pFontBlur->draw_text( i32CenterX + 270 + 190 - f32TxtLen + 1, i32CenterY + 250 - i * 20 - 3, szMsg, 0.5f );

					glColor3ub( 255, 255, 255 );
					pFont->bind();
					pFont->draw_text( i32CenterX + 270 + 190 - f32TxtLen, i32CenterY + 250 - i * 20, szMsg, 0.5f );

				}

				if( rWnd.get_pressed( KEYB_ESC ) || rWnd.get_pressed( MOUSE_LBUTTON ) ) {
					i32GameState = GAMESTATE_MAIN_MENU;
				}
			}

			if( !bLoadLevel && i32GameState == GAMESTATE_RESUME_MENU ) {
				//
				// resume game
				//

				int32	i32Sel = 0;

				for( uint32 i = 0; i < 8; i++ ) {

					snprintf( szMsg, 255, "Stage %d - %d", (i / 2) + 1, (i & 1) + 1 );

					if( i < i32MaxLevel ) {		// max level is one based
						glColor4ub( 0, 0, 0, 128 );
						pFontBlur->bind();
						pFontBlur->draw_text( i32CenterX + 270 + 23 + 1, i32CenterY + 250 - i * 20 - 3, szMsg, 0.5f );

						if( i32MouseX >= (i32CenterX + 270) && i32MouseX < (i32CenterX + 512) &&
							i32MouseY > (i32CenterY + 250 - (i * 20) - 3) && i32MouseY < (i32CenterY + 250 - (i * 20) + 20 - 3) ) {
							glColor3ub( 233, 76, 7 );
							i32Sel = i + 1;
						}
						else {
								glColor3ub( 179, 218, 220 );	// light greyish blue
						}
					}
					else {
						glColor4ub( 179, 218, 220, 128 );	// light greyish blue
					}

					pFont->bind();
					pFont->draw_text( i32CenterX + 270 + 23, i32CenterY + 250 - i * 20, szMsg, 0.5f );
				}


				if( rWnd.get_pressed( MOUSE_LBUTTON ) ) {
					if( i32Sel ) {
						i32Level = i32Sel;
						bLoadLevel = true;
						i32GameState = GAMESTATE_LEVEL_BRIEFING;

						// reset score
						pCtx->set_score( 0 );
					}
					else
						i32GameState = GAMESTATE_MAIN_MENU;
				}

				if( rWnd.get_pressed( KEYB_ESC ) )
					i32GameState = GAMESTATE_MAIN_MENU;
			}

			if( !bLoadLevel && i32GameState == GAMESTATE_MAIN_MENU ) {

				//
				// Main Menu
				//

				enum MainMenuSelectionE {
					MAINMENU_NONE = 0,
					MAINMENU_NEW_GAME,
					MAINMENU_RESUME_GAME,
					MAINMENU_OPTIONS,
					MAINMENU_HIGH_SCORES,
					MAINMENU_QUIT,
				};
				int32	i32Selection = MAINMENU_NONE;

				// new game
				glColor4ub( 0, 0, 0, 128 );
				pFontBlur->bind();
				pFontBlur->draw_text( i32CenterX + 270 + 2, i32CenterY + 245 - 3, "NEW GAME", 0.5f );

				if( i32MouseX >= (i32CenterX + 270) && i32MouseX < (i32CenterX + 512) &&
					i32MouseY > (i32CenterY + 245 + 5) && i32MouseY < (i32CenterY + 245 + 30) ) {
					glColor3ub( 233, 76, 7 );
					i32Selection = MAINMENU_NEW_GAME;
				}
				else
					glColor3ub( 255, 255, 255 );
				pFont->bind();
				pFont->draw_text( i32CenterX + 270, i32CenterY + 245, "NEW GAME", 0.5f );

				// resume game
				glColor4ub( 0, 0, 0, 128 );
				pFontBlur->bind();
				pFontBlur->draw_text( i32CenterX + 270 + 2, i32CenterY + 245 - 30 - 3, "RESUME GAME", 0.5f );

				if( i32MouseX >= (i32CenterX + 270) && i32MouseX < (i32CenterX + 512) &&
					i32MouseY >= (i32CenterY + 245 - 30 + 5) && i32MouseY < (i32CenterY + 245 - 30 + 30) ) {
					glColor3ub( 233, 76, 7 );
					i32Selection = MAINMENU_RESUME_GAME;
				}
				else
					glColor3ub( 255, 255, 255 );
				pFont->bind();
				pFont->draw_text( i32CenterX + 270, i32CenterY + 245 - 30, "RESUME GAME", 0.5f );

/*				// resumer game
				glColor4ub( 0, 0, 0, 128 );
				pFontBlur->bind();
				pFontBlur->draw_text( i32CenterX + 270 + 2, i32CenterY + 245 - 60 - 3, "HIGH SCORES", 0.5f );

				if( i32MouseX >= (i32CenterX + 270) && i32MouseX < (i32CenterX + 512) &&
					i32MouseY >= (i32CenterY + 245 - 60 + 5) && i32MouseY < (i32CenterY + 245 - 60 + 30) ) {
					glColor3ub( 233, 76, 7 );
					i32Selection = MAINMENU_HIGH_SCORES;
				}
				else
					glColor3ub( 255, 255, 255 );
				pFont->bind();
				pFont->draw_text( i32CenterX + 270, i32CenterY + 245 - 60, "HIGH SCORES", 0.5f );*/

				// quit
				glColor4ub( 0, 0, 0, 128 );
				pFontBlur->bind();
				pFontBlur->draw_text( i32CenterX + 270 + 2, i32CenterY + 245 - 60 - 3, "QUIT", 0.5f );

				if( i32MouseX >= (i32CenterX + 270) && i32MouseX < (i32CenterX + 512) &&
					i32MouseY >= (i32CenterY + 245 - 60 + 5) && i32MouseY < (i32CenterY + 245 - 60 + 30) ) {
					glColor3ub( 233, 76, 7 );
					i32Selection = MAINMENU_QUIT;
				}
				else
					glColor3ub( 255, 255, 255 );
				pFont->bind();
				pFont->draw_text( i32CenterX + 270, i32CenterY + 245 - 60, "QUIT", 0.5f );

				if( rWnd.get_pressed( MOUSE_LBUTTON ) ) {
					if( i32Selection == MAINMENU_QUIT ) {
						bQuit = true;
					}
					else if( i32Selection == MAINMENU_RESUME_GAME ) {
						i32GameState = GAMESTATE_RESUME_MENU;
					}
//					else if( i32Selection == MAINMENU_HIGH_SCORES ) {
//						i32GameState = GAMESTATE_HIGH_SCORES;
//					}
					else if( i32Selection == MAINMENU_NEW_GAME ) {
						i32Level = GAMELEVEL_STAGE_11;
						bLoadLevel = true;
						i32GameState = GAMESTATE_LEVEL_BRIEFING;

						// reset score
						pCtx->set_score( 0 );
					}
				}
			}

  
			glDisable( GL_TEXTURE_2D );
			glBlendFunc( GL_ONE, GL_ZERO );
			glEnable( GL_LIGHTING );
			glEnable( GL_CULL_FACE );
			glEnable( GL_DEPTH_TEST );


			rWnd.flush();
		}
	} while(
#ifdef WIN32
		 rMsg.message != DP_END_PREVIEW &&
#endif
		 !bQuit );


	// Delete textures
	delete pEskimoImage;
	delete pMenuImage;
	delete pLogoImage;
	delete pBaitMeterImage;
	delete pBaitMeterScaleImage;

	delete pCtx;
	delete pRod;

	delete pFont;
	delete pFontBlur;


	rWnd.destroy();

#ifdef FMOD_VERSION
	for( uint32 i = 0; i < 256; i++ ) {
		FSOUND_SetSFXMasterVolume( 255 - i );
		Sleep( 2 );
	}

	FSOUND_Stream_Close( pWaterStream );
	FSOUND_Stream_Close( pAmbientStream );

	// close music sys
	FSOUND_Close();
#endif


	{
		i32MaxLevel ^= 0xdeadbeef;

		FILE*	pStream = fopen( "config.dat", "w" );
		if( pStream ) {
			fwrite( &i32MaxLevel, sizeof( uint32 ), 1, pStream );
			fclose( pStream );
		}
	}


	return 0;
}
