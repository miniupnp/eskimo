
#ifndef __GAMEINPUT_H__
#define __GAMEINPUT_H__

HRESULT InitDirectInput( HWND hWnd );
VOID    FreeDirectInput();
HRESULT	UpdateInputState( int* pX, int* pY, int* pButton );


#endif