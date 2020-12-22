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

#include "sys/win32/rc/Radiant_resource.h"
#include "sys/win32/rc/guied_resource.h"
#include "tools/common/ColorButton.h"
#include "tools/common/MaskEdit.h"

#include "GEApp.h"
#include "GEItemPropsDlg.h"
#include "GEPropertyPage.h"

#include "tools/radiant/GLWidget.h"
#include "tools/radiant/PreviewDlg.h"

enum
{
	RVITEMPROPS_GENERAL = 0,
	RVITEMPROPS_IMAGE,
	RVITEMPROPS_TEXT,
	RVITEMPROPS_KEYS,

	RVITEMPROPS_MAX
};

class rvGEItemPropsImagePage : public rvGEPropertyPage
{
public:

	rvGEItemPropsImagePage() : rvGEPropertyPage(), mDict( 0 ) {}
	rvGEItemPropsImagePage( idDict* dictValues );

	virtual bool	Apply( void );
	virtual bool	Init( void );
	virtual bool	SetActive( void );
	virtual bool	KillActive( void );
	virtual int		HandleMessage( UINT msg, WPARAM wParam, LPARAM lParam );

	void SetDict( idDict* dict )
	{
		mDict = dict;
	}
	void Enable( BOOL enable );

protected:

	void			UpdateCheckedStates( void );

	idDict*		mDict;
};

rvGEItemPropsImagePage::rvGEItemPropsImagePage( idDict* dict )
{
	mDict = dict;
}

bool rvGEItemPropsImagePage::Apply( void )
{
	//Ross T 1/6/2015 - could have this check here to only apply on the active page
	//but applying on all pages at once is more flexible and doesn't appear to cause any harm
	//if (gApp.GetOptions().GetLastOptionsPage() == RVITEMPROPS_IMAGE)
	//{
	//com_editors = EDITOR_RADIANT;
	gApp.ApplyProperties( mDict );
	//com_editors = EDITOR_GUI;
	return true;
	//}
	//return false;
}

/*
================
rvGEItemPropsImagePage::Init

Subclass the custom controls on the page
================
*/
bool rvGEItemPropsImagePage::Init( void )
{
	NumberEdit_Attach( GetDlgItem( mPage, IDC_GUIED_ITEMMATSCALEX ) );
	NumberEdit_Attach( GetDlgItem( mPage, IDC_GUIED_ITEMMATSCALEY ) );
	NumberEdit_Attach( GetDlgItem( mPage, IDC_GUIED_ITEMBORDERSIZE ) );

	return true;
}

/*
================
rvGEItemPropsImagePage::UpdateCheckedStates

Updates the enabled state of all the controls that are linked to a checkbox
================
*/
void rvGEItemPropsImagePage::UpdateCheckedStates( void )
{
	char temp[64];
	bool state;
	bool rstate;
	idStr result;
	bool  enable;

	enable = !IsExpression( mDict->GetString( "backcolor", "1,1,1,1" ) );
	EnableWindow( GetDlgItem( mPage, IDC_GUIED_USEBACKCOLOR ), enable );

	state = IsDlgButtonChecked( mPage, IDC_GUIED_USEBACKCOLOR ) && IsWindowEnabled( GetDlgItem( mPage, IDC_GUIED_USEBACKCOLOR ) );
	EnableWindow( GetDlgItem( mPage, IDC_GUIED_ITEMBACKCOLOR ), state );
	EnableWindow( GetDlgItem( mPage, IDC_GUIED_ITEMBACKCOLORALPHA ), state );

	state = IsDlgButtonChecked( mPage, IDC_GUIED_USEMATERIAL ) != 0;
	EnableWindow( GetDlgItem( mPage, IDC_GUIED_ITEMBACKGROUND ), state );
	EnableWindow( GetDlgItem( mPage, IDC_GUIED_ITEMVARIABLEBACKGROUND ), state );
	EnableWindow( GetDlgItem( mPage, IDC_GUIED_ITEMMATSCALEX ), state );
	EnableWindow( GetDlgItem( mPage, IDC_GUIED_ITEMMATSCALEY ), state );
	EnableWindow( GetDlgItem( mPage, IDC_GUIED_XSCALE_STATIC ), state );
	EnableWindow( GetDlgItem( mPage, IDC_GUIED_YSCALE_STATIC ), state );

	EnableWindow( GetDlgItem( mPage, ID_GUIED_CHOOSE_MATERIAL ), state );

	GetWindowText( GetDlgItem( mPage, IDC_GUIED_ITEMBORDERSIZE ), temp, 64 );
	enable = !IsExpression( mDict->GetString( "bordersize", "0" ) );
	EnableWindow( GetDlgItem( mPage, IDC_GUIED_ITEMBORDERSIZE ), enable );

	state = atol( temp ) ? true : false;
	rstate = IsDlgButtonChecked( mPage, IDC_GUIED_USEBORDERCOLOR ) != 0;
	enable = !IsExpression( mDict->GetString( "bordercolor", "1,1,1,1" ) );
	EnableWindow( GetDlgItem( mPage, IDC_GUIED_USEBORDERCOLOR ), state && enable );
	EnableWindow( GetDlgItem( mPage, IDC_GUIED_USEBORDERMATERIAL ), state && enable );
	EnableWindow( GetDlgItem( mPage, IDC_GUIED_ITEMBORDERCOLOR ), rstate && state && enable );
	EnableWindow( GetDlgItem( mPage, IDC_GUIED_ITEMBORDERCOLORALPHA ), rstate && state && enable );
	EnableWindow( GetDlgItem( mPage, IDC_GUIED_ITEMBORDERMATERIAL ), !rstate && state );

	state = ( state && !rstate ) || IsDlgButtonChecked( mPage, IDC_GUIED_USEMATERIAL );
	enable = !IsExpression( mDict->GetString( "matcolor", "1,1,1,1" ) );
	EnableWindow( GetDlgItem( mPage, IDC_GUIED_ITEMMATCOLORSTATIC ), state && enable );
	EnableWindow( GetDlgItem( mPage, IDC_GUIED_ITEMMATCOLOR ), state && enable );
	EnableWindow( GetDlgItem( mPage, IDC_GUIED_ITEMMATCOLORALPHA ), state && enable );
}

/*
================
rvGEItemPropsImagePage::HandleMessage

Handles messages for the text item properties page
================
*/
int rvGEItemPropsImagePage::HandleMessage( UINT msg, WPARAM wParam, LPARAM lParam )
{
	switch( msg )
	{
		case WM_DRAWITEM:
			ColorButton_DrawItem( GetDlgItem( mPage, wParam ), ( LPDRAWITEMSTRUCT )lParam );
			return TRUE;
		case WM_COMMAND:
			switch( LOWORD( wParam ) )
			{
				case IDC_GUIED_ITEMBORDERSIZE:
					if( HIWORD( wParam ) == EN_CHANGE )
					{
						UpdateCheckedStates( );
						SendMessage( GetParent( mPage ), PSM_CHANGED, ( WPARAM )mPage, 0 );
					}
					break;
				case IDC_GUIED_ITEMMATSCALEX:
				case IDC_GUIED_ITEMMATSCALEY:
					if( HIWORD( wParam ) == EN_CHANGE )
					{
						SendMessage( GetParent( mPage ), PSM_CHANGED, ( WPARAM )mPage, 0 );
					}
					break;
				case IDC_GUIED_USEBORDERCOLOR:
				case IDC_GUIED_USEBORDERMATERIAL:
				case IDC_GUIED_USEBACKCOLOR:
				case IDC_GUIED_USEMATERIAL:
					UpdateCheckedStates( );
					break;

				case IDC_GUIED_ITEMBACKCOLORALPHA:
				case IDC_GUIED_ITEMMATCOLORALPHA:
				case IDC_GUIED_ITEMBORDERCOLORALPHA:
				{
					AlphaButton_OpenPopup( GetDlgItem( mPage, LOWORD( wParam ) ) );
					SendMessage( GetParent( mPage ), PSM_CHANGED, ( WPARAM )mPage, 0 );
					break;
				}

				case IDC_GUIED_ITEMBORDERCOLOR:
				case IDC_GUIED_ITEMBACKCOLOR:
				case IDC_GUIED_ITEMMATCOLOR:
				{
					CHOOSECOLOR col;
					ZeroMemory( &col, sizeof( col ) );
					col.lStructSize = sizeof( col );
					col.lpCustColors = gApp.GetOptions().GetCustomColors( );
					col.hwndOwner = mPage;
					col.hInstance = NULL;
					col.Flags = CC_RGBINIT;
					col.rgbResult = ColorButton_GetColor( GetDlgItem( mPage, LOWORD( wParam ) ) );
					if( ChooseColor( &col ) )
					{
						ColorButton_SetColor( GetDlgItem( mPage, LOWORD( wParam ) ), col.rgbResult );
						SendMessage( GetParent( mPage ), PSM_CHANGED, ( WPARAM )mPage, 0 );
					}
					break;
				}
				case ID_GUIED_CHOOSE_MATERIAL:
				{
					CPreviewDlg matDlg( CWnd::FromHandle( mPage ) );
					matDlg.SetOnlyFilter( true, idStr( "gui/" ) );
					matDlg.SetMode( CPreviewDlg::MATERIALS, "gui" );

					com_editors = EDITOR_RADIANT;
					INT_PTR res = matDlg.DoModal();
					com_editors = EDITOR_GUI;
					if( res == IDOK )
					{
						idStr matName = matDlg.mediaName;
						SetWindowText( GetDlgItem( mPage, IDC_GUIED_ITEMBACKGROUND ), matName.StripQuotes() );
						idStr	s;
						s = "\"";
						s.Append( matName );
						s.Append( "\"" );
						mDict->Set( "background", s );
						SendMessage( GetParent( mPage ), PSM_CHANGED, ( WPARAM )mPage, 0 );
					}
					break;
				}
			}
			break;
	}

	return rvGEPropertyPage::HandleMessage( msg, wParam, lParam );
}

void rvGEItemPropsImagePage::Enable( BOOL enable )
{
	EnableWindow( GetDlgItem( mPage, IDC_GUIED_ITEMBORDERSIZE ), enable );
	EnableWindow( GetDlgItem( mPage, IDC_GUIED_ITEMMATSCALEX ), enable );
	EnableWindow( GetDlgItem( mPage, IDC_GUIED_ITEMMATSCALEY ), enable );
	EnableWindow( GetDlgItem( mPage, IDC_GUIED_USEBORDERCOLOR ), enable );
	EnableWindow( GetDlgItem( mPage, IDC_GUIED_USEBORDERMATERIAL ), enable );
	EnableWindow( GetDlgItem( mPage, IDC_GUIED_USEBACKCOLOR ), enable );
	EnableWindow( GetDlgItem( mPage, IDC_GUIED_USEMATERIAL ), enable );
	EnableWindow( GetDlgItem( mPage, IDC_GUIED_ITEMBACKCOLORALPHA ), enable );
	EnableWindow( GetDlgItem( mPage, IDC_GUIED_ITEMMATCOLORALPHA ), enable );
	EnableWindow( GetDlgItem( mPage, IDC_GUIED_ITEMBORDERCOLORALPHA ), enable );
	EnableWindow( GetDlgItem( mPage, IDC_GUIED_ITEMBORDERCOLOR ), enable );
	EnableWindow( GetDlgItem( mPage, IDC_GUIED_ITEMBACKCOLOR ), enable );
	EnableWindow( GetDlgItem( mPage, IDC_GUIED_ITEMMATCOLOR ), enable );
	EnableWindow( GetDlgItem( mPage, IDC_GUIED_USEBACKCOLOR ), enable );
	EnableWindow( GetDlgItem( mPage, IDC_GUIED_ITEMVARIABLEBACKGROUND ), enable );
}

/*
================
rvGEItemPropsImagePage::SetActive

Initializes the text properties page by copying data from the attached
window into the controls
================
*/
bool rvGEItemPropsImagePage::SetActive( void )
{
	gApp.GetOptions().SetLastOptionsPage( RVITEMPROPS_IMAGE );

	/*static bool loadedOnce = false;

	if (loadedOnce)
	{
		if (gApp.GetActiveWorkspace() == 0)
		{
			Enable(FALSE);
			return false;
		}

		if (gApp.GetActiveWorkspace()->GetSelectionMgr().Num() == 0)
		{
			Enable(FALSE);
			return false;
		}

		loadedOnce = true;
	}*/

	if( mDict == NULL )
	{
		return false;
	}

	ColorButton_SetColor( GetDlgItem( mPage, IDC_GUIED_ITEMBACKCOLOR ), mDict->GetString( "backcolor", "1,1,1,1" ) );
	AlphaButton_SetColor( GetDlgItem( mPage, IDC_GUIED_ITEMBACKCOLORALPHA ), mDict->GetString( "backcolor", "1,1,1,1" ) );

	ColorButton_SetColor( GetDlgItem( mPage, IDC_GUIED_ITEMMATCOLOR ), mDict->GetString( "matcolor", "1,1,1,1" ) );
	AlphaButton_SetColor( GetDlgItem( mPage, IDC_GUIED_ITEMMATCOLORALPHA ), mDict->GetString( "matcolor", "1,1,1,1" ) );

	ColorButton_SetColor( GetDlgItem( mPage, IDC_GUIED_ITEMBORDERCOLOR ), mDict->GetString( "bordercolor", "0,0,0,1" ) );
	AlphaButton_SetColor( GetDlgItem( mPage, IDC_GUIED_ITEMBORDERCOLORALPHA ), mDict->GetString( "bordercolor", "0,0,0,1" ) );

	SetWindowText( GetDlgItem( mPage, IDC_GUIED_ITEMBACKGROUND ), idStr( mDict->GetString( "background", "" ) ).StripQuotes( ) );
	SetWindowText( GetDlgItem( mPage, IDC_GUIED_ITEMBORDERMATERIAL ), idStr( mDict->GetString( "borderShader", "" ) ).StripQuotes( ) );
	SetWindowText( GetDlgItem( mPage, IDC_GUIED_ITEMBORDERSIZE ), idStr( mDict->GetString( "bordersize", "0" ) ).StripQuotes( ) );

	SetWindowText( GetDlgItem( mPage, IDC_GUIED_ITEMMATSCALEX ), idStr( mDict->GetString( "matscalex", "1" ) ).StripQuotes( ) );
	SetWindowText( GetDlgItem( mPage, IDC_GUIED_ITEMMATSCALEY ), idStr( mDict->GetString( "matscaley", "1" ) ).StripQuotes( ) );

	CheckDlgButton( mPage, IDC_GUIED_ITEMVARIABLEBACKGROUND, mDict->GetBool( "variablebackground", "0" ) );

	CheckDlgButton( mPage, IDC_GUIED_USEMATERIAL, idStr( mDict->GetString( "background", "" ) ).StripQuotes( ).Length() ? BST_CHECKED : BST_UNCHECKED );
	CheckDlgButton( mPage, IDC_GUIED_USEBACKCOLOR, idStr( mDict->GetString( "backcolor", "" ) ).StripQuotes( ).Length() ? BST_CHECKED : BST_UNCHECKED );

	CheckRadioButton( mPage, IDC_GUIED_USEBORDERCOLOR, IDC_GUIED_USEBORDERMATERIAL,
					  idStr( mDict->GetString( "borderShader", "" ) ).Length() ? IDC_GUIED_USEBORDERMATERIAL : IDC_GUIED_USEBORDERCOLOR );

	UpdateCheckedStates( );

	return true;
}

/*
================
rvGEItemPropsImagePage::KillActive

Applys the settings currently stored in the property page back into the attached window
================
*/
bool rvGEItemPropsImagePage::KillActive( void )
{
	/*static bool loadedOnce = false;

	if (loadedOnce)
	{
		if (gApp.GetActiveWorkspace() == 0)
		{
			Enable(FALSE);
			return false;
		}

		if (gApp.GetActiveWorkspace()->GetSelectionMgr().Num() == 0)
		{
			Enable(FALSE);
			return false;
		}

		loadedOnce = true;
	}*/

	if( mDict == NULL )
	{
		return false;
	}

	char	temp[1024];
	bool	matcolor = false;
	idStr	s;

	if( IsDlgButtonChecked( mPage, IDC_GUIED_USEMATERIAL ) )
	{
		float val;

		GetWindowText( GetDlgItem( mPage, IDC_GUIED_ITEMBACKGROUND ), temp, 1024 );
		s = "\"";
		s.Append( temp );
		s.Append( "\"" );
		mDict->Set( "background", s );
		matcolor = true;

		GetWindowText( GetDlgItem( mPage, IDC_GUIED_ITEMMATSCALEX ), temp, 1024 );
		val = atof( temp );
		if( val >= 0.999f && val <= 1.001f )
		{
			mDict->Delete( "matscalex" );
		}
		else
		{
			mDict->Set( "matscalex", idStr::FloatArrayToString( &val, 1, 8 ) );
		}

		GetWindowText( GetDlgItem( mPage, IDC_GUIED_ITEMMATSCALEY ), temp, 1024 );
		val = atof( temp );
		if( val >= 0.999f && val <= 1.001f )
		{
			mDict->Delete( "matscaley" );
		}
		else
		{
			mDict->Set( "matscaley", idStr::FloatArrayToString( &val, 1, 8 ) );
		}

		if( IsDlgButtonChecked( mPage, IDC_GUIED_ITEMVARIABLEBACKGROUND ) )
		{
			mDict->Set( "variablebackground", "1" );
		}
		else
		{
			mDict->Delete( "variablebackground" );
		}
	}
	else
	{
		mDict->Delete( "background" );
		mDict->Delete( "variablebackground" );
		mDict->Delete( "matscalex" );
		mDict->Delete( "matscaley" );
	}

	if( IsDlgButtonChecked( mPage, IDC_GUIED_USEBACKCOLOR ) )
	{
		if( IsWindowEnabled( GetDlgItem( mPage, IDC_GUIED_ITEMBACKCOLOR ) ) )
		{
			COLORREF color = ColorButton_GetColor( GetDlgItem( mPage, IDC_GUIED_ITEMBACKCOLOR ) );
			COLORREF alpha = ColorButton_GetColor( GetDlgItem( mPage, IDC_GUIED_ITEMBACKCOLORALPHA ) );
			mDict->Set( "backcolor", StringFromVec4( idVec4( ( float )GetRValue( color ) / 255.0f, ( float )GetGValue( color ) / 255.0f, ( float )GetBValue( color ) / 255.0f, ( float )GetRValue( alpha ) / 255.0f ) ) );
		}
	}
	else
	{
		mDict->Delete( "backcolor" );
	}

	GetWindowText( GetDlgItem( mPage, IDC_GUIED_ITEMBORDERSIZE ), temp, 1024 );
	if( atoi( temp ) )
	{
		if( IsWindowEnabled( GetDlgItem( mPage, IDC_GUIED_ITEMBORDERSIZE ) ) )
		{
			mDict->Set( "bordersize", va( "%d", atoi( temp ) ) );
		}

		if( IsDlgButtonChecked( mPage, IDC_GUIED_USEBORDERCOLOR ) )
		{
			if( IsWindowEnabled( GetDlgItem( mPage, IDC_GUIED_ITEMBORDERCOLOR ) ) )
			{
				COLORREF color = ColorButton_GetColor( GetDlgItem( mPage, IDC_GUIED_ITEMBORDERCOLOR ) );
				COLORREF alpha = ColorButton_GetColor( GetDlgItem( mPage, IDC_GUIED_ITEMBORDERCOLORALPHA ) );
				mDict->Set( "bordercolor", StringFromVec4( idVec4( ( float )GetRValue( color ) / 255.0f, ( float )GetGValue( color ) / 255.0f, ( float )GetBValue( color ) / 255.0f, ( float )GetRValue( alpha ) / 255.0f ) ) );
			}
			mDict->Delete( "borderShader" );
		}
		else
		{
			GetWindowText( GetDlgItem( mPage, IDC_GUIED_ITEMBORDERMATERIAL ), temp, 1024 );
			s = "\"";
			s.Append( temp );
			s.Append( "\"" );
			mDict->Set( "borderShader", s );
			mDict->Delete( "bordercolor" );
			matcolor = true;
		}
	}
	else
	{
		mDict->Delete( "bordersize" );
		mDict->Delete( "bordercolor" );
		mDict->Delete( "borderShader" );
	}

	if( matcolor )
	{
		if( IsWindowEnabled( GetDlgItem( mPage, IDC_GUIED_ITEMMATCOLOR ) ) )
		{
			COLORREF color = ColorButton_GetColor( GetDlgItem( mPage, IDC_GUIED_ITEMMATCOLOR ) );
			COLORREF alpha = ColorButton_GetColor( GetDlgItem( mPage, IDC_GUIED_ITEMMATCOLORALPHA ) );
			mDict->Set( "matcolor", StringFromVec4( idVec4( ( float )GetRValue( color ) / 255.0f, ( float )GetGValue( color ) / 255.0f, ( float )GetBValue( color ) / 255.0f, ( float )GetRValue( alpha ) / 255.0f ) ) );
		}
	}
	else
	{
		mDict->Delete( "matcolor" );
	}

	SendMessage( GetParent( mPage ), PSM_CHANGED, ( WPARAM )mPage, 0 );

	return true;
}

class rvGEItemPropsTextPage : public rvGEPropertyPage
{
public:

	rvGEItemPropsTextPage() : rvGEPropertyPage(), mDict( 0 ) {}
	rvGEItemPropsTextPage( idDict* dictValues );

	virtual bool	Apply( void );
	virtual bool	Init( void );
	virtual bool	SetActive( void );
	virtual bool	KillActive( void );
	virtual int		HandleMessage( UINT msg, WPARAM wParam, LPARAM lParam );

	void SetDict( idDict* dict )
	{
		mDict = dict;
	}
	void Enable( BOOL enable );

protected:

	void			UpdateCheckedStates( void );

	idDict*		mDict;
};

rvGEItemPropsTextPage::rvGEItemPropsTextPage( idDict* dict )
{
	mDict = dict;
}

bool rvGEItemPropsTextPage::Apply( void )
{
	//Ross T 1/6/2015 - could have this check here to only apply on the active page
	//but applying on all pages at once is more flexible and doesn't appear to cause any harm
	//if (gApp.GetOptions().GetLastOptionsPage() == RVITEMPROPS_TEXT)
	//{
	return gApp.ApplyProperties( mDict );
	//}

	return false;
}

/*
================
rvGEItemPropsTextPage::Init

Subclass the custom controls on the page
================
*/
bool rvGEItemPropsTextPage::Init( void )
{
	NumberEdit_Attach( GetDlgItem( mPage, IDC_GUIED_ITEMTEXTSCALE ) );
	NumberEdit_Attach( GetDlgItem( mPage, IDC_GUIED_ITEMTEXTALIGNX ) );
	NumberEdit_Attach( GetDlgItem( mPage, IDC_GUIED_ITEMTEXTALIGNY ) );

	SendMessage( GetDlgItem( mPage, IDC_GUIED_ITEMTEXTALIGN ), CB_ADDSTRING, 0, ( LPARAM )"Left" );
	SendMessage( GetDlgItem( mPage, IDC_GUIED_ITEMTEXTALIGN ), CB_ADDSTRING, 0, ( LPARAM )"Center" );
	SendMessage( GetDlgItem( mPage, IDC_GUIED_ITEMTEXTALIGN ), CB_ADDSTRING, 0, ( LPARAM )"Right" );

	SendMessage( GetDlgItem( mPage, IDC_GUIED_ITEMTEXTFONT ), CB_ADDSTRING, 0, ( LONG )"<default>" );

	idFileList* folders;
	int		  i;

	folders = fileSystem->ListFiles( "fonts", "/" );

	for( i = 0; i < folders->GetNumFiles(); i++ )
	{
		if( folders->GetFile( i )[0] == '.' )
		{
			continue;
		}

		SendMessage( GetDlgItem( mPage, IDC_GUIED_ITEMTEXTFONT ), CB_ADDSTRING, 0, ( LONG )folders->GetFile( i ) );
	}

	fileSystem->FreeFileList( folders );

	return true;
}

/*
================
rvGEItemPropsTextPage::UpdateCheckedStates

Updates the enabled state of all the controls that are linked to a checkbox
================
*/
void rvGEItemPropsTextPage::UpdateCheckedStates( void )
{
	bool	state;
	idStr	result;
	bool	enable;

	state = IsDlgButtonChecked( mPage, IDC_GUIED_USETEXT ) != 0;
	enable = !IsExpression( mDict->GetString( "forecolor", "1,1,1,1" ) );

	EnableWindow( GetDlgItem( mPage, IDC_GUIED_ITEMFORECOLOR ), state && enable );
	EnableWindow( GetDlgItem( mPage, IDC_GUIED_ITEMFORECOLORALPHA ), state && enable );

	EnableWindow( GetDlgItem( mPage, IDC_GUIED_ITEMTEXT ), state );
	EnableWindow( GetDlgItem( mPage, IDC_GUIED_ITEMTEXTSCALE ), state );
	EnableWindow( GetDlgItem( mPage, IDC_GUIED_ITEMTEXTSCALE ), state );
	EnableWindow( GetDlgItem( mPage, IDC_GUIED_ITEMTEXTALIGN ), state );
	EnableWindow( GetDlgItem( mPage, IDC_GUIED_ITEMTEXTALIGNX ), state );
	EnableWindow( GetDlgItem( mPage, IDC_GUIED_ITEMTEXTALIGNY ), state );
	EnableWindow( GetDlgItem( mPage, IDC_GUIED_ITEMTEXTFONT ), state );
	EnableWindow( GetDlgItem( mPage, IDC_GUIED_ITEMTEXTNOWRAP ), state );

	EnableWindow( GetDlgItem( mPage, IDC_GUIED_STATIC_ALIGNMENT ), state );
	EnableWindow( GetDlgItem( mPage, IDC_GUIED_STATIC_X ), state );
	EnableWindow( GetDlgItem( mPage, IDC_GUIED_STATIC_Y ), state );
	EnableWindow( GetDlgItem( mPage, IDC_GUIED_STATIC_SCALE ), state );
	EnableWindow( GetDlgItem( mPage, IDC_GUIED_STATIC_COLOR ), state );
	EnableWindow( GetDlgItem( mPage, IDC_GUIED_STATIC_FONT ), state );
}

void rvGEItemPropsTextPage::Enable( BOOL enable )
{
	EnableWindow( GetDlgItem( mPage, IDC_GUIED_ITEMFORECOLOR ), enable );
	EnableWindow( GetDlgItem( mPage, IDC_GUIED_ITEMFORECOLORALPHA ), enable );

	EnableWindow( GetDlgItem( mPage, IDC_GUIED_USETEXT ), enable );

	EnableWindow( GetDlgItem( mPage, IDC_GUIED_ITEMTEXT ), enable );
	EnableWindow( GetDlgItem( mPage, IDC_GUIED_ITEMTEXTSCALE ), enable );
	EnableWindow( GetDlgItem( mPage, IDC_GUIED_ITEMTEXTSCALE ), enable );
	EnableWindow( GetDlgItem( mPage, IDC_GUIED_ITEMTEXTALIGN ), enable );
	EnableWindow( GetDlgItem( mPage, IDC_GUIED_ITEMTEXTALIGNX ), enable );
	EnableWindow( GetDlgItem( mPage, IDC_GUIED_ITEMTEXTALIGNY ), enable );
	EnableWindow( GetDlgItem( mPage, IDC_GUIED_ITEMTEXTFONT ), enable );
	EnableWindow( GetDlgItem( mPage, IDC_GUIED_ITEMTEXTNOWRAP ), enable );

	EnableWindow( GetDlgItem( mPage, IDC_GUIED_STATIC_ALIGNMENT ), enable );
	EnableWindow( GetDlgItem( mPage, IDC_GUIED_STATIC_X ), enable );
	EnableWindow( GetDlgItem( mPage, IDC_GUIED_STATIC_Y ), enable );
	EnableWindow( GetDlgItem( mPage, IDC_GUIED_STATIC_SCALE ), enable );
	EnableWindow( GetDlgItem( mPage, IDC_GUIED_STATIC_COLOR ), enable );
	EnableWindow( GetDlgItem( mPage, IDC_GUIED_STATIC_FONT ), enable );
}

/*
================
rvGEItemPropsTextPage::HandleMessage

Handles messages for the text item properties page
================
*/
int rvGEItemPropsTextPage::HandleMessage( UINT msg, WPARAM wParam, LPARAM lParam )
{
	switch( msg )
	{
		case WM_DRAWITEM:
			ColorButton_DrawItem( GetDlgItem( mPage, wParam ), ( LPDRAWITEMSTRUCT )lParam );
			return TRUE;

		case WM_COMMAND:
			switch( LOWORD( wParam ) )
			{
				case IDC_GUIED_ITEMTEXTFONT:
					if( HIWORD( wParam ) == EN_SELCHANGE )
					{
						SendMessage( GetParent( mPage ), PSM_CHANGED, ( WPARAM )mPage, 0 );
					}
					break;

				case IDC_GUIED_USETEXT:
					UpdateCheckedStates( );
					SendMessage( GetParent( mPage ), PSM_CHANGED, ( WPARAM )mPage, 0 );
					break;

				case IDC_GUIED_ITEMFORECOLORALPHA:
					AlphaButton_OpenPopup( GetDlgItem( mPage, LOWORD( wParam ) ) );
					SendMessage( GetParent( mPage ), PSM_CHANGED, ( WPARAM )mPage, 0 );
					break;

				case IDC_GUIED_ITEMFORECOLOR:
				{
					CHOOSECOLOR col;
					ZeroMemory( &col, sizeof( col ) );
					col.lStructSize = sizeof( col );
					col.lpCustColors = gApp.GetOptions().GetCustomColors( );
					col.hwndOwner = mPage;
					col.hInstance = NULL;
					col.Flags = CC_RGBINIT;
					col.rgbResult = ColorButton_GetColor( GetDlgItem( mPage, LOWORD( wParam ) ) );
					if( ChooseColor( &col ) )
					{
						ColorButton_SetColor( GetDlgItem( mPage, LOWORD( wParam ) ), col.rgbResult );
						SendMessage( GetParent( mPage ), PSM_CHANGED, ( WPARAM )mPage, 0 );
					}
					break;
				}

				case IDC_GUIED_ITEMTEXT:
				{
					if( HIWORD( wParam ) == EN_CHANGE )
					{
						SendMessage( GetParent( mPage ), PSM_CHANGED, ( WPARAM )mPage, 0 );
					}
					break;
				}
			}

			break;
	}

	return rvGEPropertyPage::HandleMessage( msg, wParam, lParam );
}

/*
================
rvGEItemPropsTextPage::SetActive

Initializes the text properties page by copying data from the attached
window into the controls
================
*/
bool rvGEItemPropsTextPage::SetActive( void )
{
	gApp.GetOptions().SetLastOptionsPage( RVITEMPROPS_TEXT );

	/*static bool loadedOnce = false;

	if (loadedOnce)
	{
		if (gApp.GetActiveWorkspace() == 0)
		{
			Enable(FALSE);
			return false;
		}

		if (gApp.GetActiveWorkspace()->GetSelectionMgr().Num() == 0)
		{
			Enable(FALSE);
			return false;
		}

		loadedOnce = true;
	}*/

	if( mDict == NULL )
	{
		return false;
	}

	ColorButton_SetColor( GetDlgItem( mPage, IDC_GUIED_ITEMFORECOLOR ), mDict->GetString( "forecolor", "1,1,1,1" ) );
	AlphaButton_SetColor( GetDlgItem( mPage, IDC_GUIED_ITEMFORECOLORALPHA ), mDict->GetString( "forecolor", "1,1,1,1" ) );

	SetWindowText( GetDlgItem( mPage, IDC_GUIED_ITEMTEXT ), idStr( mDict->GetString( "text", "" ) ).StripQuotes( ) );
	SetWindowText( GetDlgItem( mPage, IDC_GUIED_ITEMTEXTSCALE ), mDict->GetString( "textscale", "1.0" ) );
	SetWindowText( GetDlgItem( mPage, IDC_GUIED_ITEMTEXTALIGNX ), mDict->GetString( "textalignx", "0" ) );
	SetWindowText( GetDlgItem( mPage, IDC_GUIED_ITEMTEXTALIGNY ), mDict->GetString( "textaligny", "0" ) );

	SendMessage( GetDlgItem( mPage, IDC_GUIED_ITEMTEXTALIGN ), CB_SETCURSEL, atoi( mDict->GetString( "textalign", "0" ) ), 0 );

	// Figure out which font to select
	idStr font = mDict->GetString( "font", "" );
	int   fontSel;
	font.StripQuotes( );
	font.StripPath( );
	fontSel = SendMessage( GetDlgItem( mPage, IDC_GUIED_ITEMTEXTFONT ), CB_FINDSTRING, -1, ( LONG )font.c_str() );
	SendMessage( GetDlgItem( mPage, IDC_GUIED_ITEMTEXTFONT ), CB_SETCURSEL, 0, 0 );
	SendMessage( GetDlgItem( mPage, IDC_GUIED_ITEMTEXTFONT ), CB_SETCURSEL, fontSel == -1 ? 0 : fontSel, 0 );

	CheckDlgButton( mPage, IDC_GUIED_USETEXT, idStr( mDict->GetString( "text", "" ) ).Length() ? BST_CHECKED : BST_UNCHECKED );
	CheckDlgButton( mPage, IDC_GUIED_ITEMTEXTNOWRAP, atoi( idStr( mDict->GetString( "nowrap", "0" ) ).StripQuotes() ) ? BST_CHECKED : BST_UNCHECKED );

	UpdateCheckedStates( );

	return true;
}

/*
================
rvGEItemPropsTextPage::KillActive

Applys the settings currently stored in the property page back into the attached window
================
*/
bool rvGEItemPropsTextPage::KillActive( void )
{
	/*static bool loadedOnce = false;

	if (loadedOnce)
	{
		if (gApp.GetActiveWorkspace() == 0)
		{
			Enable(FALSE);
			return false;
		}

		if (gApp.GetActiveWorkspace()->GetSelectionMgr().Num() == 0)
		{
			Enable(FALSE);
			return false;
		}

		loadedOnce = true;
	}*/

	if( mDict == NULL )
	{
		return false;
	}

	idStr	s;
	char	temp[1024];
	int		i;
	float	f;

	if( IsDlgButtonChecked( mPage, IDC_GUIED_USETEXT ) )
	{
		if( IsWindowEnabled( GetDlgItem( mPage, IDC_GUIED_ITEMFORECOLOR ) ) )
		{
			COLORREF color = ColorButton_GetColor( GetDlgItem( mPage, IDC_GUIED_ITEMFORECOLOR ) );
			COLORREF alpha = ColorButton_GetColor( GetDlgItem( mPage, IDC_GUIED_ITEMFORECOLORALPHA ) );
			mDict->Set( "forecolor", StringFromVec4( idVec4( ( float )GetRValue( color ) / 255.0f, ( float )GetGValue( color ) / 255.0f, ( float )GetBValue( color ) / 255.0f, ( float )GetRValue( alpha ) / 255.0f ) ) );
		}

		GetWindowText( GetDlgItem( mPage, IDC_GUIED_ITEMTEXT ), temp, 1024 );
		s = "\"";
		s.Append( temp );
		s.Append( "\"" );
		mDict->Set( "text", s );

		GetWindowText( GetDlgItem( mPage, IDC_GUIED_ITEMTEXTSCALE ), temp, 1024 );
		f = atof( temp );
		mDict->Set( "textscale", idStr::FloatArrayToString( &f, 1, 8 ) );

		i = SendMessage( GetDlgItem( mPage, IDC_GUIED_ITEMTEXTALIGN ), CB_GETCURSEL, 0, 0 );
		s = va( "%d", i );
		if( i != 0 )
		{
			mDict->Set( "textalign", s );
		}
		else
		{
			mDict->Delete( "textalign" );
		}

		GetWindowText( GetDlgItem( mPage, IDC_GUIED_ITEMTEXTALIGNX ), temp, 1024 );
		i = atoi( temp );
		if( i )
		{
			mDict->Set( "textalignx", va( "%d", i ) );
		}
		else
		{
			mDict->Delete( "textalignx" );
		}

		GetWindowText( GetDlgItem( mPage, IDC_GUIED_ITEMTEXTALIGNY ), temp, 1024 );
		i = atoi( temp );
		if( i )
		{
			mDict->Set( "textaligny", va( "%d", i ) );
		}
		else
		{
			mDict->Delete( "textaligny" );
		}

		int fontSel = SendMessage( GetDlgItem( mPage, IDC_GUIED_ITEMTEXTFONT ), CB_GETCURSEL, 0, 0 );
		if( fontSel == 0 )
		{
			mDict->Delete( "font" );
		}
		else
		{
			char fontName[MAX_PATH];
			SendMessage( GetDlgItem( mPage, IDC_GUIED_ITEMTEXTFONT ), CB_GETLBTEXT, fontSel, ( LONG )fontName );
			mDict->Set( "font", idStr( "\"fonts/" ) + idStr( fontName ) + idStr( "\"" ) );
		}

		if( IsDlgButtonChecked( mPage, IDC_GUIED_ITEMTEXTNOWRAP ) )
		{
			mDict->Set( "nowrap", "1" );
		}
		else
		{
			mDict->Delete( "nowrap" );
		}
	}
	else
	{
		mDict->Delete( "text" );
		mDict->Delete( "textscale" );
		mDict->Delete( "textalign" );
		mDict->Delete( "textalignx" );
		mDict->Delete( "textaligny" );
		mDict->Delete( "forecolor" );
		mDict->Delete( "font" );
	}

	SendMessage( GetParent( mPage ), PSM_CHANGED, ( WPARAM )mPage, 0 );

	return true;
}

class rvGEItemPropsKeysPage : public rvGEPropertyPage
{
public:

	rvGEItemPropsKeysPage() : rvGEPropertyPage(), mDict( 0 ), mWrapper( 0 ) {}
	rvGEItemPropsKeysPage( idDict* dictValues, rvGEWindowWrapper* wrapper );

	virtual bool	Apply( void );
	virtual bool	Init( void );
	virtual bool	SetActive( void );
	virtual int		HandleMessage( UINT msg, WPARAM wParam, LPARAM lParam );

	void SetDict( idDict* dict )
	{
		mDict = dict;
	}
	void SetWrapper( rvGEWindowWrapper* wrapper )
	{
		mWrapper = wrapper;
	}
	void Enable( BOOL enable );

protected:

	idDict*				mDict;
	rvGEWindowWrapper*	mWrapper;
};

rvGEItemPropsKeysPage::rvGEItemPropsKeysPage( idDict* dict, rvGEWindowWrapper* wrapper )
{
	mDict = dict;
	mWrapper = wrapper;
}

INT_PTR CALLBACK ModifyItemKeyDlg_WndProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	switch( msg )
	{
		case WM_INITDIALOG:
		{
			const idKeyValue* keyValue = ( const idKeyValue* ) lParam;
			MaskEdit_Attach( GetDlgItem( hwnd, IDC_GUIED_ITEMKEY ), " \t\r\n" );
			SetWindowText( GetDlgItem( hwnd, IDC_GUIED_ITEMVALUE ), idStr( keyValue->GetValue() ).StripQuotes( ) );
			if( idStr::Icmp( keyValue->GetKey(), "guied_temp" ) )
			{
				if( !idStr::Icmp( keyValue->GetKey(), "name" ) || !idStr::Icmp( keyValue->GetKey(), "rect" ) )
				{
					// Dont allow editing the name keyname
					EnableWindow( GetDlgItem( hwnd, IDC_GUIED_ITEMKEY ), FALSE );
				}

				SetWindowText( GetDlgItem( hwnd, IDC_GUIED_ITEMKEY ), keyValue->GetKey() );
				SetFocus( GetDlgItem( hwnd, IDC_GUIED_ITEMVALUE ) );
				SendMessage( GetDlgItem( hwnd, IDC_GUIED_ITEMVALUE ), EM_SETSEL, 0, -1 );

				SetWindowText( hwnd, "New Item Key" );
			}

			SetWindowLongPtr( hwnd, GWLP_USERDATA, lParam );
			return FALSE;
		}

		case WM_COMMAND:
			switch( LOWORD( wParam ) )
			{
				case IDOK:
				{
					char key[1024];
					char value[1024];

					const idKeyValue* keyValue = ( const idKeyValue* ) GetWindowLongPtr( hwnd, GWLP_USERDATA );

					GetWindowText( GetDlgItem( hwnd, IDC_GUIED_ITEMKEY ), key, 1024 );
					GetWindowText( GetDlgItem( hwnd, IDC_GUIED_ITEMVALUE ), value, 1024 );

					if( strlen( key ) < 1 )
					{
						gApp.MessageBox( va( "Invalid key name '%s'", key ), MB_OK | MB_ICONERROR );
					}
					else
					{
						const_cast<idStr&>( keyValue->GetKey() ) = key;
						const_cast<idStr&>( keyValue->GetValue() ) = value;

						// FIXME: MrE may never change key value pairs directly!
						//keyValue->GetKey() = key;
						//keyValue->GetValue = value;

						EndDialog( hwnd, 1 );
					}

					break;
				}

				case IDCANCEL:
					EndDialog( hwnd, 0 );
					break;
			}
			break;
	}

	return FALSE;
}

bool rvGEItemPropsKeysPage::Apply( void )
{
	//Ross T 1/6/2015 - could have this check here to only apply on the active page
	//but applying on all pages at once is more flexible and doesn't appear to cause any harm
	//if (gApp.GetOptions().GetLastOptionsPage() == RVITEMPROPS_KEYS)
	//{
	return gApp.ApplyProperties( mDict );
	//}

	return false;
}


int rvGEItemPropsKeysPage::HandleMessage( UINT msg, WPARAM wParam, LPARAM lParam )
{
	switch( msg )
	{
		case WM_NOTIFY:
		{
			NMHDR* nm = ( NMHDR* ) lParam;

			// There are a few protected keys that need to dissallow deleting so
			// check to see if the newly selected key is one of them and if so then
			// disable the delete button
			if( nm->code == LVN_ITEMCHANGED )
			{
				NMLISTVIEW* nmlv = ( NMLISTVIEW* ) nm;
				if( nmlv->uNewState & LVIS_SELECTED )
				{
					const idKeyValue* keyValue = ( idKeyValue* ) nmlv->lParam;
					assert( keyValue );

					if( !idStr::Icmp( keyValue->GetKey(), "name" ) || !idStr::Icmp( keyValue->GetKey(), "rect" ) )
					{
						EnableWindow( GetDlgItem( mPage, IDC_GUIED_DELETEKEY ), FALSE );
					}
					else
					{
						EnableWindow( GetDlgItem( mPage, IDC_GUIED_DELETEKEY ), TRUE );
					}
				}
			}
			break;
		}

		case WM_COMMAND:
			switch( LOWORD( wParam ) )
			{
				case IDC_GUIED_ADDKEY:
				{
					HWND list = GetDlgItem( mPage, IDC_GUIED_ITEMKEYS );
					mDict->Set( "guied_temp", "" );
					const idKeyValue* key = mDict->FindKey( "guied_temp" );

					while( 1 )
					{
						if( DialogBoxParam( gApp.GetInstance(), MAKEINTRESOURCE( IDD_GUIED_ITEMKEY ), mPage, ModifyItemKeyDlg_WndProc, ( LPARAM )key ) )
						{
							idStr finalValue;

							finalValue = key->GetValue();
							if( !mWrapper->VerfiyStateKey( key->GetKey(), finalValue ) )
							{
								finalValue = "\"";
								finalValue += key->GetValue();
								finalValue += "\"";
								if( !mWrapper->VerfiyStateKey( key->GetKey(), finalValue ) )
								{
									gApp.MessageBox( va( "Invalid property value '%s' for property '%s'", key->GetValue().c_str(), key->GetKey().c_str() ), MB_OK | MB_ICONERROR );
									continue;
								}
							}

							//Ross T 1/6/2015 - commented this out since it gets set
							//within the dialog's callback now (so it can show up right away on the item props dialog)
							//mDict->Set(key->GetKey(), finalValue);

							LVITEM		item;
							ZeroMemory( &item, sizeof( item ) );
							item.mask = LVIF_TEXT | LVIF_PARAM;
							item.iItem = ListView_GetItemCount( list );
							item.pszText = ( LPSTR )key->GetKey().c_str( );
							item.lParam = ( LONG ) key;
							int index = ListView_InsertItem( list, &item );

							finalValue.StripQuotes( );
							ListView_SetItemText( list, index, 1, ( LPSTR )finalValue.c_str( ) );

							break;
						}
						else
						{
							mDict->Delete( "guied_temp" );
							break;
						}
					}

					SendMessage( GetParent( mPage ), PSM_CHANGED, ( WPARAM )mPage, 0 );

					break;
				}

				case IDC_GUIED_EDITKEY:
				{
					HWND list = GetDlgItem( mPage, IDC_GUIED_ITEMKEYS );
					int index = ListView_GetNextItem( list, -1, LVNI_SELECTED );
					if( index != -1 )
					{
						LVITEM item;
						item.iItem = index;
						item.mask = LVIF_PARAM;
						ListView_GetItem( list, &item );
						const idKeyValue* key = ( const idKeyValue* )item.lParam;
						assert( key );

						while( 1 )
						{
							if( DialogBoxParam( gApp.GetInstance(), MAKEINTRESOURCE( IDD_GUIED_ITEMKEY ), mPage, ModifyItemKeyDlg_WndProc, ( LPARAM )key ) )
							{
								idStr finalValue;

								finalValue = key->GetValue();
								if( !mWrapper->VerfiyStateKey( key->GetKey(), finalValue ) )
								{
									finalValue = "\"";
									finalValue += key->GetValue();
									finalValue += "\"";
									if( !mWrapper->VerfiyStateKey( key->GetKey(), finalValue ) )
									{
										gApp.MessageBox( va( "Invalid property value '%s' for property '%s'", key->GetValue().c_str(), key->GetKey().c_str() ), MB_OK | MB_ICONERROR );
										continue;
									}
								}

								// FIXME: MrE is this the right thing todo?
								mDict->Set( key->GetKey(), finalValue );
								//key->GetValue() = finalValue;

								ListView_SetItemText( list, index, 0, ( LPSTR )key->GetKey().c_str() );

								finalValue.StripQuotes( );
								ListView_SetItemText( list, index, 1, ( LPSTR )finalValue.c_str() );
								break;
							}
						}

						SendMessage( GetParent( mPage ), PSM_CHANGED, ( WPARAM )mPage, 0 );
					}
					break;
				}

				case IDC_GUIED_DELETEKEY:
				{
					HWND list = GetDlgItem( mPage, IDC_GUIED_ITEMKEYS );
					int index = ListView_GetNextItem( list, -1, LVNI_SELECTED );
					if( index != -1 )
					{
						LVITEM item;
						item.iItem = index;
						item.mask = LVIF_PARAM;
						ListView_GetItem( list, &item );
						const idKeyValue* key = ( const idKeyValue* )item.lParam;
						assert( key );

						mDict->Delete( key->GetKey() );

						ListView_DeleteItem( list, index );

						SendMessage( GetParent( mPage ), PSM_CHANGED, ( WPARAM )mPage, 0 );
					}
					break;
				}
			}
			break;
	}

	return rvGEPropertyPage::HandleMessage( msg, wParam, lParam );
}

void rvGEItemPropsKeysPage::Enable( BOOL enable )
{
	EnableWindow( GetDlgItem( mPage, IDC_GUIED_ITEMKEYS ), enable );
	EnableWindow( GetDlgItem( mPage, IDC_GUIED_ADDKEY ), enable );
	EnableWindow( GetDlgItem( mPage, IDC_GUIED_DELETEKEY ), enable );
	EnableWindow( GetDlgItem( mPage, IDC_GUIED_EDITKEY ), enable );
}

/*
================
rvGEItemPropsKeysPage::Init

Called when the advanced page is first initialized.  Setup the extended
control styles and add the key/value columns
================
*/
bool rvGEItemPropsKeysPage::Init( void )
{
	HWND	list;
	RECT	rWindow;

	// Get the list control
	list = GetDlgItem( mPage, IDC_GUIED_ITEMKEYS );
	assert( list );

	GetClientRect( list, &rWindow );

	// Set the full row select for better visual appearance
	ListView_SetExtendedListViewStyle( list, LVS_EX_FULLROWSELECT );

	// Add the key column
	LVCOLUMN col;
	col.mask = LVCF_TEXT | LVCF_WIDTH;
	col.cx = .3 * ( rWindow.right - rWindow.left );
	col.pszText = "Key";
	ListView_InsertColumn( list, 0, &col );

	// Add the value column
	col.pszText = "Value";
	col.cx = ( rWindow.right - rWindow.left ) - col.cx;
	ListView_InsertColumn( list, 1, &col );

	return true;
}

/*
================
rvGEItemPropsKeysPage::SetActive

Called when the advanced page is made active and will add an entry to
the keys list view for each key in the properties dictionary
================
*/
bool rvGEItemPropsKeysPage::SetActive( void )
{
	int		i;
	HWND	list;

	gApp.GetOptions().SetLastOptionsPage( RVITEMPROPS_KEYS );

	/*static bool loadedOnce = false;

	if (loadedOnce)
	{
		if (gApp.GetActiveWorkspace() == 0)
		{
			Enable(FALSE);
			return false;
		}

		if (gApp.GetActiveWorkspace()->GetSelectionMgr().Num() == 0)
		{
			Enable(FALSE);
			return false;
		}

		loadedOnce = true;
	}*/

	// Get listview control
	list = GetDlgItem( mPage, IDC_GUIED_ITEMKEYS );
	assert( list );

	// Delete anything already in there
	ListView_DeleteAllItems( list );

	if( mDict != NULL )
	{
		// Add each key in the properties dictionary
		for( i = 0; i < mDict->GetNumKeyVals(); i++ )
		{
			const idKeyValue* key = mDict->GetKeyVal( i );
			assert( key );

			// Add the item
			LVITEM item;
			ZeroMemory( &item, sizeof( item ) );
			item.mask = LVIF_TEXT | LVIF_PARAM;
			item.iItem = ListView_GetItemCount( list );
			item.pszText = ( LPSTR )key->GetKey().c_str();
			item.lParam = ( LONG )key;
			int index = ListView_InsertItem( list, &item );

			idStr value;
			value = key->GetValue();
			value.StripQuotes();
			ListView_SetItemText( list, index, 1, ( LPSTR )value.c_str() );
		}
	}



	return true;
}

class rvGEItemPropsGeneralPage : public rvGEPropertyPage
{
public:

	rvGEItemPropsGeneralPage() : rvGEPropertyPage(), mDict( 0 ) {}

	rvGEItemPropsGeneralPage( idDict* dict, rvGEWindowWrapper::EWindowType type );

	virtual bool	Apply( void );
	virtual bool	SetActive( void );
	virtual	bool	KillActive( void );
	virtual bool	Init( void );
	virtual int		HandleMessage( UINT msg, WPARAM wParam, LPARAM lParam );

	void SetDict( idDict* dict )
	{
		mDict = dict;
	}
	void SetWrapperType( const idStr& type )
	{
		mType = type;
	}
	void Enable( BOOL enable );

protected:

	void			UpdateCheckedStates( void );

	idDict*	mDict;
	idStr	mType;
};

rvGEItemPropsGeneralPage::rvGEItemPropsGeneralPage( idDict* dict, rvGEWindowWrapper::EWindowType type )
{
	mDict = dict;
	mType = rvGEWindowWrapper::WindowTypeToString( type );
}

bool rvGEItemPropsGeneralPage::Apply( void )
{
	//Ross T 1/6/2015 - could have this check here to only apply on the active page
	//but applying on all pages at once is more flexible and doesn't appear to cause any harm
	//if (gApp.GetOptions().GetLastOptionsPage() == RVITEMPROPS_GENERAL)
	//{
	return gApp.ApplyProperties( mDict );
	//}

	return false;
}

void rvGEItemPropsGeneralPage::Enable( BOOL enable )
{
	EnableWindow( GetDlgItem( mPage, IDC_GUIED_ITEMNAME ), enable );
	EnableWindow( GetDlgItem( mPage, IDC_GUIED_ITEMVISIBLE ), enable );
	EnableWindow( GetDlgItem( mPage, IDC_GUIED_ITEMNOTIME ), enable );
	EnableWindow( GetDlgItem( mPage, IDC_GUIED_ITEMNOEVENTS ), enable );
	EnableWindow( GetDlgItem( mPage, IDC_GUIED_ITEMNOCURSOR ), enable );
	EnableWindow( GetDlgItem( mPage, IDC_GUIED_TYPE ), enable );
}

/*
================
rvGEItemPropsGeneralPage::UpdateCheckedStates

Updates the enabled state of all the controls that are linked to a checkbox
================
*/
void rvGEItemPropsGeneralPage::UpdateCheckedStates( void )
{
}

/*
================
rvGEItemPropsGeneralPage::HandleMessage

Handles messages for the general item properties page
================
*/
int rvGEItemPropsGeneralPage::HandleMessage( UINT msg, WPARAM wParam, LPARAM lParam )
{
	switch( msg )
	{
		case WM_COMMAND:
		{
			switch( LOWORD( wParam ) )
			{
				case IDC_GUIED_ITEMNAME:
				{
					if( HIWORD( wParam ) == EN_CHANGE )
					{
						SendMessage( GetParent( mPage ), PSM_CHANGED, ( WPARAM )mPage, 0 );
					}
					break;
				}
			}
		}
	}

	return rvGEPropertyPage::HandleMessage( msg, wParam, lParam );
}

/*
================
rvGEItemPropsGeneralPage::Init

Subclass the custom controls on the page
================
*/
bool rvGEItemPropsGeneralPage::Init( void )
{
	MaskEdit_Attach( GetDlgItem( mPage, IDC_GUIED_ITEMNAME ), " \t\n\r" );

	return true;
}

/*
================
rvGEItemPropsGeneralPage::SetActive

Initializes the general properties page by copying data from the attached
window into the controls
================
*/
bool rvGEItemPropsGeneralPage::SetActive( void )
{
	bool  enable;
	idStr result;

	gApp.GetOptions().SetLastOptionsPage( RVITEMPROPS_GENERAL );

	/*static bool loadedOnce = false;

	if (loadedOnce)
	{
		if (gApp.GetActiveWorkspace() == 0)
		{
			Enable(FALSE);
			return false;
		}

		if (gApp.GetActiveWorkspace()->GetSelectionMgr().Num() == 0)
		{
			Enable(FALSE);
			return false;
		}

		loadedOnce = true;
	}*/

	if( mDict == NULL )
	{
		return false;
	}

	SetWindowText( GetDlgItem( mPage, IDC_GUIED_ITEMNAME ), idStr( mDict->GetString( "name", "unnamed" ) ).StripQuotes( ) );

	enable = !IsExpression( mDict->GetString( "visible", "1" ) );
	CheckDlgButton( mPage, IDC_GUIED_ITEMVISIBLE, atol( idStr( mDict->GetString( "visible", "1" ) ).StripQuotes( ) ) ? BST_CHECKED : BST_UNCHECKED );
	EnableWindow( GetDlgItem( mPage, IDC_GUIED_ITEMVISIBLE ), enable );

	enable = !IsExpression( mDict->GetString( "notime", "0" ) );
	CheckDlgButton( mPage, IDC_GUIED_ITEMNOTIME, atol( idStr( mDict->GetString( "notime", "0" ) ).StripQuotes( ) ) ? BST_CHECKED : BST_UNCHECKED );
	EnableWindow( GetDlgItem( mPage, IDC_GUIED_ITEMNOTIME ), enable );

	enable = !IsExpression( mDict->GetString( "noevents", "0" ) );
	CheckDlgButton( mPage, IDC_GUIED_ITEMNOEVENTS, atol( idStr( mDict->GetString( "noevents", "0" ) ).StripQuotes( ) ) ? BST_CHECKED : BST_UNCHECKED );
	EnableWindow( GetDlgItem( mPage, IDC_GUIED_ITEMNOEVENTS ), enable );

	enable = !IsExpression( mDict->GetString( "noclip", "0" ) );
	CheckDlgButton( mPage, IDC_GUIED_ITEMNOCLIP, atol( idStr( mDict->GetString( "noclip", "0" ) ).StripQuotes( ) ) ? BST_CHECKED : BST_UNCHECKED );
	EnableWindow( GetDlgItem( mPage, IDC_GUIED_ITEMNOCLIP ), enable );

	enable = !IsExpression( mDict->GetString( "nocursor", "0" ) );
	EnableWindow( GetDlgItem( mPage, IDC_GUIED_ITEMNOCURSOR ), enable );
	CheckDlgButton( mPage, IDC_GUIED_ITEMNOCURSOR, atol( idStr( mDict->GetString( "nocursor", "0" ) ).StripQuotes( ) ) ? BST_CHECKED : BST_UNCHECKED );

	SetWindowText( GetDlgItem( mPage, IDC_GUIED_TYPE ), mType );

	UpdateCheckedStates( );

	return true;
}


/*
================
rvGEItemPropsGeneralPage::KillActive

Applys the settings currently stored in the property page back into the attached window
================
*/
bool rvGEItemPropsGeneralPage::KillActive( void )
{
	/*static bool loadedOnce = false;

	if (loadedOnce)
	{
		if (gApp.GetActiveWorkspace() == 0)
		{
			Enable(FALSE);
			return false;
		}

		if (gApp.GetActiveWorkspace()->GetSelectionMgr().Num() == 0)
		{
			Enable(FALSE);
			return false;
		}

		loadedOnce = true;
	}*/

	if( mDict == NULL )
	{
		return false;
	}

	char temp[1024];

	GetWindowText( GetDlgItem( mPage, IDC_GUIED_ITEMNAME ), temp, 1024 );

	const idKeyValue* kv = mDict->FindKey( "name" );
	if( kv )
	{
		if( kv->GetValue().Icmp( temp ) != 0 )
		{
			mDict->Set( "name", temp );
			SendMessage( GetParent( mPage ), PSM_CHANGED, ( WPARAM )mPage, 0 );
		}
	}
	else
	{
		mDict->Set( "name", temp );
		SendMessage( GetParent( mPage ), PSM_CHANGED, ( WPARAM )mPage, 0 );
	}

	if( IsWindowEnabled( GetDlgItem( mPage, IDC_GUIED_ITEMVISIBLE ) ) )
	{
		if( IsDlgButtonChecked( mPage, IDC_GUIED_ITEMVISIBLE ) )
		{
			mDict->Set( "visible" , "1" );
		}
		else
		{
			mDict->Set( "visible" , "0" );
		}

		SendMessage( GetParent( mPage ), PSM_CHANGED, ( WPARAM )mPage, 0 );
	}

	if( IsWindowEnabled( GetDlgItem( mPage, IDC_GUIED_ITEMNOTIME ) ) )
	{
		if( IsDlgButtonChecked( mPage, IDC_GUIED_ITEMNOTIME ) )
		{
			mDict->Set( "notime" , "1" );
		}
		else
		{
			mDict->Delete( "notime" );
		}
	}

	if( IsWindowEnabled( GetDlgItem( mPage, IDC_GUIED_ITEMNOEVENTS ) ) )
	{
		if( IsDlgButtonChecked( mPage, IDC_GUIED_ITEMNOEVENTS ) )
		{
			mDict->Set( "noevents" , "1" );
		}
		else
		{
			mDict->Delete( "noevents" );
		}

		SendMessage( GetParent( mPage ), PSM_CHANGED, ( WPARAM )mPage, 0 );
	}

	if( IsWindowEnabled( GetDlgItem( mPage, IDC_GUIED_ITEMNOCLIP ) ) )
	{
		if( IsDlgButtonChecked( mPage, IDC_GUIED_ITEMNOCLIP ) )
		{
			mDict->Set( "noclip" , "1" );
		}
		else
		{
			mDict->Delete( "noclip" );
		}

		SendMessage( GetParent( mPage ), PSM_CHANGED, ( WPARAM )mPage, 0 );
	}

	if( IsWindowEnabled( GetDlgItem( mPage, IDC_GUIED_ITEMNOCURSOR ) ) )
	{
		if( IsDlgButtonChecked( mPage, IDC_GUIED_ITEMNOCURSOR ) )
		{
			mDict->Set( "nocursor" , "1" );
		}
		else
		{
			mDict->Delete( "nocursor" );
		}

		SendMessage( GetParent( mPage ), PSM_CHANGED, ( WPARAM )mPage, 0 );
	}

	return true;
}


/*
================
rvGEItemProps::rvGEItemProps

constructor
================
*/
rvGEItemProps::rvGEItemProps( void )
{
	mWrapper = NULL;
	mWnd = NULL;
	mDlg = NULL;
	mWorkspace = NULL;
}

rvGEItemProps::~rvGEItemProps()
{
	delete( rvGEItemPropsGeneralPage* )propsp[RVITEMPROPS_GENERAL].lParam;
	delete( rvGEItemPropsImagePage* )propsp[RVITEMPROPS_IMAGE].lParam;
	delete( rvGEItemPropsTextPage* )propsp[RVITEMPROPS_TEXT].lParam;
	delete( rvGEItemPropsKeysPage* )propsp[RVITEMPROPS_KEYS].lParam;
}

bool rvGEItemProps::Create( HWND parent, bool visible )
{
	propsp[RVITEMPROPS_GENERAL].dwSize = sizeof( PROPSHEETPAGE );
	propsp[RVITEMPROPS_GENERAL].dwFlags = PSP_USETITLE;
	propsp[RVITEMPROPS_GENERAL].hInstance = win32.hInstance;
	propsp[RVITEMPROPS_GENERAL].pszTemplate = MAKEINTRESOURCE( IDD_GUIED_ITEMPROPS_GENERAL );
	propsp[RVITEMPROPS_GENERAL].pfnDlgProc = rvGEPropertyPage::WndProc;
	propsp[RVITEMPROPS_GENERAL].pszTitle = "General";
	propsp[RVITEMPROPS_GENERAL].lParam = ( LONG )new rvGEItemPropsGeneralPage();

	propsp[RVITEMPROPS_IMAGE].dwSize = sizeof( PROPSHEETPAGE );
	propsp[RVITEMPROPS_IMAGE].dwFlags = PSP_USETITLE;
	propsp[RVITEMPROPS_IMAGE].hInstance = win32.hInstance;
	propsp[RVITEMPROPS_IMAGE].pszTemplate = MAKEINTRESOURCE( IDD_GUIED_ITEMPROPS_IMAGE );
	propsp[RVITEMPROPS_IMAGE].pfnDlgProc = rvGEPropertyPage::WndProc;
	propsp[RVITEMPROPS_IMAGE].pszTitle = "Image";
	propsp[RVITEMPROPS_IMAGE].lParam = ( LONG )new rvGEItemPropsImagePage();

	propsp[RVITEMPROPS_TEXT].dwSize = sizeof( PROPSHEETPAGE );
	propsp[RVITEMPROPS_TEXT].dwFlags = PSP_USETITLE;
	propsp[RVITEMPROPS_TEXT].hInstance = win32.hInstance;
	propsp[RVITEMPROPS_TEXT].pszTemplate = MAKEINTRESOURCE( IDD_GUIED_ITEMPROPS_TEXT );
	propsp[RVITEMPROPS_TEXT].pfnDlgProc = rvGEPropertyPage::WndProc;
	propsp[RVITEMPROPS_TEXT].pszTitle = "Text";
	propsp[RVITEMPROPS_TEXT].lParam = ( LONG )new rvGEItemPropsTextPage();

	propsp[RVITEMPROPS_KEYS].dwSize = sizeof( PROPSHEETPAGE );
	propsp[RVITEMPROPS_KEYS].dwFlags = PSP_USETITLE;
	propsp[RVITEMPROPS_KEYS].hInstance = win32.hInstance;
	propsp[RVITEMPROPS_KEYS].pszTemplate = MAKEINTRESOURCE( IDD_GUIED_ITEMPROPS_KEYS );
	propsp[RVITEMPROPS_KEYS].pfnDlgProc = rvGEPropertyPage::WndProc;
	propsp[RVITEMPROPS_KEYS].pszTitle = "Keys";
	propsp[RVITEMPROPS_KEYS].lParam = ( LONG )new rvGEItemPropsKeysPage();

	propsh.dwSize = sizeof( PROPSHEETHEADER );
	propsh.nStartPage = gApp.GetOptions().GetLastOptionsPage();
	propsh.dwFlags = PSH_PROPSHEETPAGE | PSH_NOCONTEXTHELP | PSH_MODELESS | PSH_USECALLBACK;
	propsh.hwndParent = parent;
	propsh.pszCaption = "Item Properties";
	propsh.nPages = RVITEMPROPS_MAX;
	propsh.ppsp = ( LPCPROPSHEETPAGE )&propsp;
	propsh.pfnCallback = ( PFNPROPSHEETCALLBACK )rvGEItemProps::WndProc;

	// Bring up the item properties dialog now
	INT_PTR result = PropertySheet( &propsh );
	mWnd = ( HWND )result;

	LONG st = GetWindowLong( mWnd, GWL_STYLE );
	SetWindowLong( mWnd, GWL_STYLE, st & ~WS_SYSMENU );
	//these four lines make sure the individual pages get initiated..
	SendMessage( mWnd, PSM_SETCURSEL, RVITEMPROPS_GENERAL, 0 );
	SendMessage( mWnd, PSM_SETCURSEL, RVITEMPROPS_IMAGE, 0 );
	SendMessage( mWnd, PSM_SETCURSEL, RVITEMPROPS_TEXT, 0 );
	SendMessage( mWnd, PSM_SETCURSEL, RVITEMPROPS_KEYS, 0 );

	SendMessage( mWnd, PSM_SETCURSEL, gApp.GetOptions().GetLastOptionsPage(), 0 );

	if( !gApp.GetOptions().GetWindowPlacement( "itemProperties", mWnd ) )
	{
		RECT rParent;
		RECT rTrans;

		GetWindowRect( GetParent( mWnd ), &rParent );
		GetWindowRect( mWnd, &rTrans );
		SetWindowPos( mWnd, NULL,
					  rParent.right - 10 - ( rTrans.right - rTrans.left ),
					  rParent.bottom - 10 - ( rTrans.bottom - rTrans.top ),
					  0, 0,
					  SWP_NOZORDER | SWP_NOSIZE );
	}

	ShowWindow( GetDlgItem( mWnd, IDOK ), SW_HIDE );
	ShowWindow( GetDlgItem( mWnd, IDCANCEL ), SW_HIDE );
	return true;
}

void rvGEItemProps::Show( bool visible )
{
	gApp.GetOptions().SetItemPropertiesVisible( visible );
	ShowWindow( mWnd, visible ? SW_SHOW : SW_HIDE );
}

void rvGEItemProps::Update( void )
{
	rvGEWorkspace* workspace = gApp.GetActiveWorkspace();
	if( workspace )
	{
		if( workspace->GetSelectionMgr().Num() > 0 )
		{
			rvGEWindowWrapper*	wrapper = rvGEWindowWrapper::GetWrapper( workspace->GetSelectionMgr()[0] );
			assert( wrapper );

			// Start the destination dictionary with the values in the window dictionary
			mDict.Clear();
			mDict.Copy( wrapper->GetStateDict() );

			( ( rvGEItemPropsGeneralPage* )propsp[RVITEMPROPS_GENERAL].lParam )->Enable( TRUE );
			( ( rvGEItemPropsTextPage* )propsp[RVITEMPROPS_TEXT].lParam )->Enable( TRUE );
			( ( rvGEItemPropsImagePage* )propsp[RVITEMPROPS_IMAGE].lParam )->Enable( TRUE );
			( ( rvGEItemPropsKeysPage* )propsp[RVITEMPROPS_KEYS].lParam )->Enable( TRUE );

			( ( rvGEItemPropsKeysPage* )propsp[RVITEMPROPS_KEYS].lParam )->SetDict( &mDict );
			( ( rvGEItemPropsKeysPage* )propsp[RVITEMPROPS_KEYS].lParam )->SetWrapper( wrapper );

			( ( rvGEItemPropsImagePage* )propsp[RVITEMPROPS_IMAGE].lParam )->SetDict( &mDict );

			( ( rvGEItemPropsTextPage* )propsp[RVITEMPROPS_TEXT].lParam )->SetDict( &mDict );

			( ( rvGEItemPropsGeneralPage* )propsp[RVITEMPROPS_GENERAL].lParam )->SetDict( &mDict );
			( ( rvGEItemPropsGeneralPage* )propsp[RVITEMPROPS_GENERAL].lParam )->SetWrapperType( rvGEWindowWrapper::WindowTypeToString( wrapper->GetWindowType() ) );

			( ( rvGEItemPropsGeneralPage* )propsp[gApp.GetOptions().GetLastOptionsPage()].lParam )->SetActive();
		}
		else
		{
			//disable controls...
			mDict.Clear();
			( ( rvGEItemPropsGeneralPage* )propsp[RVITEMPROPS_GENERAL].lParam )->Enable( FALSE );
			( ( rvGEItemPropsTextPage* )propsp[RVITEMPROPS_TEXT].lParam )->Enable( FALSE );
			( ( rvGEItemPropsImagePage* )propsp[RVITEMPROPS_IMAGE].lParam )->Enable( FALSE );
			( ( rvGEItemPropsKeysPage* )propsp[RVITEMPROPS_KEYS].lParam )->Enable( FALSE );
		}
	}
	else
	{
		mDict.Clear();
		( ( rvGEItemPropsGeneralPage* )propsp[RVITEMPROPS_GENERAL].lParam )->Enable( FALSE );
		( ( rvGEItemPropsTextPage* )propsp[RVITEMPROPS_TEXT].lParam )->Enable( FALSE );
		( ( rvGEItemPropsImagePage* )propsp[RVITEMPROPS_IMAGE].lParam )->Enable( FALSE );
		( ( rvGEItemPropsKeysPage* )propsp[RVITEMPROPS_KEYS].lParam )->Enable( FALSE );
	}
}


void rvGEItemProps::SetWorkspace( rvGEWorkspace* workspace )
{
	mWorkspace = workspace;
	if( mWorkspace == NULL )
	{
		mDict.Clear();
		( ( rvGEItemPropsGeneralPage* )propsp[RVITEMPROPS_GENERAL].lParam )->Enable( FALSE );
		( ( rvGEItemPropsTextPage* )propsp[RVITEMPROPS_TEXT].lParam )->Enable( FALSE );
		( ( rvGEItemPropsImagePage* )propsp[RVITEMPROPS_IMAGE].lParam )->Enable( FALSE );
		( ( rvGEItemPropsKeysPage* )propsp[RVITEMPROPS_KEYS].lParam )->Enable( FALSE );
	}
	else
	{
		Update();
	}
}

int CALLBACK rvGEItemProps::WndProc( HWND hWnd, UINT msg, LPARAM lParam )
{
	switch( msg )
	{
		case PSCB_BUTTONPRESSED:
		{
			if( lParam == PSBTN_FINISH )
			{
				ShowWindow( hWnd, FALSE );
			}
			else if( lParam == PSBTN_CANCEL )
			{
				ShowWindow( hWnd, FALSE );
			}
			break;
		}
	}

	return 0;
}