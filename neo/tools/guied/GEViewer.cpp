/*
===========================================================================

Doom 3 BFG Edition GPL Source Code
Copyright (C) 1993-2012 id Software LLC, a ZeniMax Media company.
Copyright (C) 2014-2016 Robert Beckebans
Copyright (C) 2014-2016 Kot in Action Creative Artel

This file is part of the Doom 3 BFG Edition GPL Source Code ("Doom 3 BFG Edition Source Code").

Doom 3 BFG Edition Source Code is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Doom 3 BFG Edition Source Code is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Doom 3 BFG Edition Source Code.  If not, see <http://www.gnu.org/licenses/>.

In addition, the Doom 3 BFG Edition Source Code is also subject to certain additional terms. You should have received a copy of these additional terms immediately following the terms and conditions of the GNU General Public License which accompanied the Doom 3 BFG Edition Source Code.  If not, please request a copy in writing from id Software at the address below.

If you have questions concerning this license or the applicable additional terms, you may contact in writing id Software LLC, c/o ZeniMax Media Inc., Suite 120, Rockville, Maryland 20850 USA.

===========================================================================
*/
#include <afxwin.h>
#pragma hdrstop
#include "precompiled.h"
//#include "renderer/qgl.h"
#include "sys/win32/rc/guied_resource.h"
#include "renderer/tr_local.h"

#include "GEApp.h"
#include "GEViewer.h"

rvGEViewer::rvGEViewer( )
{
	mInterface = NULL;
	mPaused    = true;
	mTime	   = 0;
}

bool rvGEViewer::Create( HWND parent )
{
	WNDCLASSEX	wndClass;
	
	// Make sure the alpha slider window class is registered
	memset( &wndClass, 0, sizeof( wndClass ) );
	wndClass.cbSize			= sizeof( WNDCLASSEX );
	wndClass.lpszClassName	= "GUIED_VIEWER";
	wndClass.lpfnWndProc	= rvGEViewer::WndProc;
	wndClass.hInstance		= gApp.GetInstance( );
	wndClass.style			= CS_OWNDC | CS_BYTEALIGNWINDOW | CS_VREDRAW | CS_HREDRAW;
	wndClass.hbrBackground	= ( HBRUSH )( COLOR_3DFACE + 1 );
	RegisterClassEx( &wndClass );
	
	mWnd = CreateWindowEx( WS_EX_TOOLWINDOW, "GUIED_VIEWER", "GUI Viewer",
						   WS_SYSMENU | WS_THICKFRAME | WS_CAPTION | WS_POPUP | WS_OVERLAPPED | WS_BORDER | WS_CLIPSIBLINGS | WS_CHILD,
						   CW_USEDEFAULT, CW_USEDEFAULT, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2,
						   parent, NULL, gApp.GetInstance(), this );
						   
	gApp.GetOptions().GetWindowPlacement( "viewer", mWnd );
	
	
	ShowWindow( mWnd, SW_SHOW );
	UpdateWindow( mWnd );
	
	return true;
}

void rvGEViewer::Play( void )
{
	if( !mPaused )
	{
		return;
	}
	
	mLastTime = eventLoop->Milliseconds();
	mPaused   = false;
	
	TBBUTTONINFO tbinfo;
	tbinfo.cbSize = sizeof( TBBUTTONINFO );
	tbinfo.dwMask = TBIF_COMMAND | TBIF_IMAGE;
	tbinfo.iImage = 1;
	tbinfo.idCommand = ID_GUIED_VIEWER_PAUSE;
	SendMessage( mToolbar, TB_SETBUTTONINFO, ID_GUIED_VIEWER_PLAY, ( LPARAM )&tbinfo );
}

void rvGEViewer::Pause( void )
{
	if( mPaused )
	{
		return;
	}
	
	mPaused = true;
	
	TBBUTTONINFO tbinfo;
	tbinfo.cbSize = sizeof( TBBUTTONINFO );
	tbinfo.dwMask = TBIF_COMMAND | TBIF_IMAGE;
	tbinfo.iImage = 0;
	tbinfo.idCommand = ID_GUIED_VIEWER_PLAY;
	SendMessage( mToolbar, TB_SETBUTTONINFO, ID_GUIED_VIEWER_PAUSE, ( LPARAM )&tbinfo );
}


bool rvGEViewer::Destroy( void )
{
	gApp.GetOptions().SetWindowPlacement( "viewer", mWnd );
	
	DestroyWindow( mWnd );
	return true;
}

bool rvGEViewer::OpenFile( const char* filename )
{
	idStr tempfile;
	idStr ospath;
	
	delete mInterface;
	
	tempfile = filename;
	tempfile.StripPath();
	tempfile.StripFileExtension( );
	// foresthale 2014-05-19: fix /analyze warning about unused format parameter
	//tempfile = va("guis/temp.guied", tempfile.c_str() );
	tempfile = "guis/temp.guied";
	//ospath = fileSystem->RelativePathToOSPath ( tempfile, "fs_basepath" );
	ospath = fileSystem->RelativePathToOSPath( tempfile, "fs_savepath" );
	
	// Make sure the gui directory exists
	idStr createDir = ospath;
	createDir.StripFilename( );
	CreateDirectory( createDir, NULL );
	
	SetFileAttributes( ospath, FILE_ATTRIBUTE_NORMAL );
	DeleteFile( ospath );
	CopyFile( filename, ospath, FALSE );
	SetFileAttributes( ospath, FILE_ATTRIBUTE_NORMAL );
	
	mInterface = reinterpret_cast< idUserInterfaceLocal* >( uiManager->FindGui( tempfile, true, true ) );
	
	mInterface->SetStateString( "guied_item_0", "guied 1" );
	mInterface->SetStateString( "guied_item_1", "guied 2" );
	mInterface->SetStateString( "guied_item_2", "guied 3" );
	
	mTime = 0;
	
	mInterface->Activate( true, mTime );
	
	DeleteFile( ospath );
	
	Play( );
	
	return true;
}

/*
=======
MapKey

Map from windows to Doom keynums
=======
*/
static int MapKey( int key )
{
	int result;
	int modified;
	bool is_extended;
	
	modified = ( key >> 16 ) & 255;
	
	if( modified > 127 )
		return 0;
		
	if( key & ( 1 << 24 ) )
	{
		is_extended = true;
	}
	else
	{
		is_extended = false;
	}
	
	const unsigned char* scanToKey = Sys_GetScanTable();
	result = scanToKey[modified];
	
	// common->Printf( "Key: 0x%08x Modified: 0x%02x Extended: %s Result: 0x%02x\n", key, modified, (is_extended?"Y":"N"), result);
	
	if( is_extended )
	{
		switch( result )
		{
			case K_PAUSE:
				return K_NUMLOCK;
			case 0x0D:
				return K_KP_ENTER;
			case 0x2F:
				return K_KP_SLASH;
			case 0xAF:
				return K_KP_PLUS;
		}
	}
	else
	{
		switch( result )
		{
			case K_HOME:
				return K_KP_7;
			case K_UPARROW:
				return K_KP_8;
			case K_PGUP:
				return K_KP_9;
			case K_LEFTARROW:
				return K_KP_4;
			case K_RIGHTARROW:
				return K_KP_6;
			case K_END:
				return K_KP_1;
			case K_DOWNARROW:
				return K_KP_2;
			case K_PGDN:
				return K_KP_3;
			case K_INS:
				return K_KP_0;
			case K_DEL:
				return K_KP_DOT;
		}
	}
	
	return result;
}

LRESULT CALLBACK rvGEViewer::WndProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	rvGEViewer* viewer = ( rvGEViewer* ) GetWindowLongPtr( hwnd, GWLP_USERDATA );
	
	switch( msg )
	{
	
		case WM_COMMAND:
			switch( LOWORD( wParam ) )
			{
				case ID_GUIED_VIEWER_PLAY:
					viewer->Play( );
					break;
					
				case ID_GUIED_VIEWER_PAUSE:
					viewer->Pause( );
					break;
			}
			break;
			
		case WM_SIZE:
		{
			RECT rToolbar;
			SendMessage( viewer->mToolbar, TB_AUTOSIZE, 0, 0 );
			GetWindowRect( viewer->mToolbar, &rToolbar );
			viewer->mToolbarHeight = rToolbar.bottom - rToolbar.top;
			break;
		}
		
		case WM_ACTIVATE:
			common->ActivateTool( LOWORD( wParam ) != WA_INACTIVE );
			break;
			
		case WM_ERASEBKGND:
			return TRUE;
			
		case WM_PAINT:
			assert( viewer );
			viewer->HandlePaint( wParam, lParam );
			break;
			
		case WM_LBUTTONDOWN:
			if( viewer->mInterface )
			{
				sysEvent_t event;
				bool       visuals;
				ZeroMemory( &event, sizeof( event ) ) ;
				event.evType = SE_KEY;
				event.evValue = K_MOUSE1;
				event.evValue2 = true;
				viewer->mInterface->HandleEvent( &event, viewer->mTime, &visuals );
			}
			break;
			
		case WM_LBUTTONUP:
			if( viewer->mInterface )
			{
				sysEvent_t event;
				bool       visuals;
				ZeroMemory( &event, sizeof( event ) ) ;
				event.evType = SE_KEY;
				event.evValue = K_MOUSE1;
				event.evValue2 = false;
				viewer->mInterface->HandleEvent( &event, viewer->mTime, &visuals );
			}
			break;
			
		case WM_KEYDOWN:
			if( viewer->mInterface )
			{
				sysEvent_t event;
				bool       visuals;
				ZeroMemory( &event, sizeof( event ) ) ;
				event.evType = SE_KEY;
				event.evValue = MapKey( lParam );
				event.evValue2 = true;
				viewer->mInterface->HandleEvent( &event, viewer->mTime, &visuals );
			}
			break;
			
		case WM_SYSKEYUP:
		case WM_KEYUP:
			if( viewer->mInterface )
			{
				sysEvent_t event;
				bool       visuals;
				ZeroMemory( &event, sizeof( event ) ) ;
				event.evType = SE_KEY;
				event.evValue = MapKey( lParam );
				event.evValue2 = false;
				viewer->mInterface->HandleEvent( &event, viewer->mTime, &visuals );
			}
			break;
			
		case WM_CHAR:
		
			if( wParam == VK_ESCAPE )
			{
				SendMessage( hwnd, WM_CLOSE, 0, 0 );
				break;
			}
			
			if( viewer->mInterface )
			{
				sysEvent_t event;
				bool       visuals;
				ZeroMemory( &event, sizeof( event ) ) ;
				event.evType = SE_CHAR;
				event.evValue = wParam;
				event.evValue2 = false;
				viewer->mInterface->HandleEvent( &event, viewer->mTime, &visuals );
			}
			break;
			
		case WM_MOUSEMOVE:
			if( viewer->mInterface )
			{
				float x = ( float )( LOWORD( lParam ) ) / ( float )viewer->mWindowWidth * SCREEN_WIDTH;
				float y = ( float )( HIWORD( lParam ) ) / ( float )( viewer->mWindowHeight - viewer->mToolbarHeight ) * SCREEN_HEIGHT;
				sysEvent_t event;
				bool       visuals;
				
				ZeroMemory( &event, sizeof( event ) ) ;
				event.evType = SE_MOUSE;
				event.evValue = ( int )x - viewer->mInterface->CursorX();
				event.evValue2 = ( int )y - viewer->mInterface->CursorY();
				viewer->mInterface->HandleEvent( &event, viewer->mTime, &visuals );
			}
			break;
			
		case WM_CLOSE:
			viewer->mInterface = NULL;
			gApp.CloseViewer( );
			return 0;
			
		case WM_CREATE:
		{
			CREATESTRUCT* cs = ( CREATESTRUCT* ) lParam;
			SetWindowLongPtr( hwnd, GWLP_USERDATA, ( LONG_PTR )cs->lpCreateParams );
			
			viewer = ( rvGEViewer* )cs->lpCreateParams;
			viewer->mWnd = hwnd;
			viewer->SetupPixelFormat( );
			
			viewer->mToolbar = CreateWindowEx( 0, TOOLBARCLASSNAME, "", CCS_BOTTOM | WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, hwnd, ( HMENU )IDR_GUIED_VIEWERTOOLBAR, gApp.GetInstance(), NULL );
			
			// Send the TB_BUTTONSTRUCTSIZE message, which is required for backward compatibility.
			SendMessage( viewer->mToolbar, TB_BUTTONSTRUCTSIZE, ( WPARAM )sizeof( TBBUTTON ), 0 );
			
			SendMessage( viewer->mToolbar, TB_SETBUTTONSIZE, 0, MAKELONG( 16, 15 ) );
			
			SendMessage( viewer->mToolbar, TB_SETSTYLE, 0, SendMessage( viewer->mToolbar, TB_GETSTYLE, 0, 0 ) | TBSTYLE_FLAT );
			
			TBMETRICS tbmet;
			tbmet.cbSize = sizeof( TBMETRICS );
			SendMessage( viewer->mToolbar, TB_GETMETRICS, 0, ( LPARAM )&tbmet );
			tbmet.cyPad = 0;
			tbmet.cyBarPad = 0;
			SendMessage( viewer->mToolbar, TB_SETMETRICS, 0, ( LPARAM )&tbmet );
			
			// Add the bitmap containing button images to the toolbar.
			TBADDBITMAP	tbab;
			tbab.hInst = win32.hInstance;
			tbab.nID = IDR_GUIED_VIEWERTOOLBAR;
			SendMessage( viewer->mToolbar, TB_ADDBITMAP, ( WPARAM )4, ( LPARAM ) &tbab );
			
			TBBUTTON tbb[4];
			tbb[0].fsStyle = BTNS_SEP;
			tbb[0].fsState = 0;
			
			tbb[1].idCommand = ID_GUIED_VIEWER_START;
			tbb[1].iBitmap = 2;
			tbb[1].fsState = 0;
			tbb[1].fsStyle = BTNS_BUTTON;
			tbb[1].dwData = 0;
			tbb[1].iString = -1;
			
			tbb[2].idCommand = ID_GUIED_VIEWER_PAUSE;
			tbb[2].iBitmap = 1;
			tbb[2].fsState = TBSTATE_ENABLED;
			tbb[2].fsStyle = BTNS_BUTTON;
			tbb[2].dwData = 0;
			tbb[2].iString = -1;
			
			tbb[3].fsStyle = BTNS_SEP;
			tbb[3].fsState = 0;
			
			SendMessage( viewer->mToolbar, TB_ADDBUTTONS, ( WPARAM )4, ( LPARAM ) &tbb );
			
			break;
		}
		
		case WM_SETCURSOR:
			SetCursor( NULL );
			break;
	}
	
	return DefWindowProc( hwnd, msg, wParam, lParam );
}

LRESULT rvGEViewer::HandlePaint( WPARAM wParam, LPARAM lParam )
{
	HDC			dc;
	PAINTSTRUCT ps;
	
	dc = BeginPaint( mWnd, &ps );
	
	Render( dc );
	
	EndPaint( mWnd, &ps );
	
	return 1;
}

/*
================
rvGEViewer::SetupPixelFormat

Setup the pixel format for the opengl context
================
*/
bool rvGEViewer::SetupPixelFormat( void )
{
	HDC	 hDC    = GetDC( mWnd );
	bool result = true;
	
	int pixelFormat = ChoosePixelFormat( hDC, &win32.pfd );
	if( pixelFormat > 0 )
	{
		if( SetPixelFormat( hDC, pixelFormat, &win32.pfd ) == NULL )
		{
			result = false;
		}
	}
	else
	{
		result = false;
	}
	
	ReleaseDC( mWnd, hDC );
	
	return result;
}

void rvGEViewer::Render( HDC dc )
{
	// Switch GL contexts to our dc
	if( !qwglMakeCurrent( dc, win32.hGLRC ) )
	{
		common->Printf( "ERROR: wglMakeCurrent failed.. Error:%i\n", qglGetError() );
		common->Printf( "Please restart SS2Ed if the Map view is not working\n" );
		return;
	}
	
	// foresthale 2014-05-19: set up familiar state for editors before we draw anything
	tr.Editor_SetupState();
	
	if( !mPaused )
	{
		mTime += eventLoop->Milliseconds() - mLastTime;
		mLastTime = eventLoop->Milliseconds();
	}
	
	RECT rClient;
	RECT rToolbar;
	GetClientRect( mWnd, &rClient );
	GetClientRect( mToolbar, &rToolbar );
	mWindowWidth = rClient.right - rClient.left;
	mWindowHeight = rClient.bottom - rClient.top;
	
	qglViewport( 0, 0, mWindowWidth, mWindowHeight );
	qglScissor( 0, 0, mWindowWidth, mWindowHeight );
	qglClearColor( 0, 0, 0, 0 );
	
	qglDisable( GL_DEPTH_TEST );
	qglDisable( GL_CULL_FACE );
	qglClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	
	// Render the workspace below
	qglMatrixMode( GL_PROJECTION );
	qglLoadIdentity();
	qglOrtho( 0, mWindowWidth, mWindowHeight, 0, -1, 1 );
	qglMatrixMode( GL_MODELVIEW );
	qglLoadIdentity();
	
	if( mInterface )
	{
		viewDef_t viewDef;
		memset( &viewDef, 0, sizeof( viewDef ) );
		tr.viewDef = &viewDef;
		viewDef.renderView.x = 0;
		viewDef.renderView.y = mToolbarHeight;
		viewDef.renderView.width = mWindowWidth;
		viewDef.renderView.height = mWindowHeight - mToolbarHeight;
		viewDef.scissor.x1 = 0;
		viewDef.scissor.y1 = 0; // (rToolbar.bottom-rToolbar.top);
		viewDef.scissor.x2 = mWindowWidth;
		viewDef.scissor.y2 = mWindowHeight;
		viewDef.isEditor = true;
		
		// Prepare the renderSystem view to draw the GUI in
#if 1 //FIX_RADIANT
		// foresthale 2014-05-19: cleaned up editor begin/end rendering code
		int originalNativeWidth, originalNativeHeight;
		tr.Editor_BeginView( mWindowWidth, mWindowHeight, originalNativeWidth, originalNativeHeight );
		// Draw the gui
		mInterface->Redraw( mTime, false );
		tr.Editor_EndView( originalNativeWidth, originalNativeHeight );
#endif
	}
	
	qglFinish( );
	qwglSwapBuffers( dc );
}

void rvGEViewer::RunFrame( void )
{
	if( !mPaused )
	{
		HDC hDC = GetDC( mWnd );
		Render( hDC );
		ReleaseDC( mWnd, hDC );
	}
}
