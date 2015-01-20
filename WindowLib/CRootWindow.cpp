#include "stdafx.h"
#include "CRootWindow.h"

CRootWindow::CRootWindow()
{
	/*
	AddMoveEventHandler( *this );
	AddResizeEventHandler( *this );
	AddMouseEventHandler( *this );
	AddKeyboardEventHandler( *this );
	AddRenderEventHandler( *this );
	AddFocusEventHandler( *this );
	AddClipboardEventHandler( *this );
	*/
}

CRootWindow::~CRootWindow()
{

}

bool CRootWindow::SetWidth( float NewWidth )
{
	return false;
}

bool CRootWindow::SetHeight( float NewHeight )
{
	return false;
}

bool CRootWindow::SetSize( float NewWidth, float NewHeight )
{
	return false;
}

bool CRootWindow::SetTop( float NewTop )
{
	return false;
}

bool CRootWindow::SetLeft( float NewLeft )
{
	return false;
}

bool CRootWindow::SetPosition( float NewLeft, float NewTop )
{
	return false;
}

float CRootWindow::GetLeft() const
{
	return 0;
}

float CRootWindow::GetTop() const
{
	return 0;
}

float CRootWindow::GetWidth() const
{
	return CWindow::GetWidth();
}

float CRootWindow::GetHeight() const
{
	return CWindow::GetHeight();
}

float CRootWindow::GetAbsoluteLeft() const
{
	return 0;
}

float CRootWindow::GetAbsoluteTop() const
{
	return 0;
}

bool CRootWindow::SetAnchorTop( bool NewAnchorTop )
{
	return false;
}

bool CRootWindow::SetAnchorLeft( bool NewAnchorLeft )
{
	return false;
}

bool CRootWindow::SetAnchorBottom( bool NewAnchorBottom )
{
	return false;
}

bool CRootWindow::SetAnchorRight( bool NewAnchorRight )
{
	return false;
}

bool CRootWindow::GetAnchorTop() const
{
	return true;
}

bool CRootWindow::GetAnchorLeft() const
{
	return true;
}

bool CRootWindow::GetAnchorBottom() const
{
	return false;
}

bool CRootWindow::GetAnchorRight() const
{
	return false;
}

bool CRootWindow::SetVisible( bool NewVisible )
{
	return false;
}

bool CRootWindow::GetVisible() const
{
	return true;
}

bool CRootWindow::JumpToFront()
{
	return false;
}
bool CRootWindow::JumpToBack()
{
	return false;
}

bool CRootWindow::AddChild( IWindow &Child, bool AddToBack )
{
	return false;
}

bool CRootWindow::RemoveChild( IWindow &Child )
{
	return false;
}

bool CRootWindow::SetParent( IWindow *NewParent )
{
	return false;
}
IWindow *CRootWindow::GetParent() const
{
	return NULL;
}

const std::list< IWindow * const > &CRootWindow::GetChildren() const
{
	return CWindow::GetChildren();
}

bool CRootWindow::PrivateSetWidth( float NewWidth )
{
	return CWindow::SetWidth( NewWidth );
}

bool CRootWindow::PrivateSetHeight( float NewHeight )
{
	return CWindow::SetHeight( NewHeight );
}

bool CRootWindow::PrivateSetSize( float NewWidth, float NewHeight )
{
	return CWindow::SetSize( NewWidth, NewHeight );
}

bool CRootWindow::PrivateSetTop( float NewTop )
{
	return CWindow::SetTop( NewTop );
}

bool CRootWindow::PrivateSetLeft( float NewLeft )
{
	return CWindow::SetLeft( NewLeft );
}

bool CRootWindow::PrivateSetPosition( float NewLeft, float NewTop )
{
	return CWindow::SetPosition( NewLeft, NewTop );
}

bool CRootWindow::PrivateAddChild( IWindow &Child )
{
	return CWindow::AddChild( Child );
}

bool CRootWindow::PrivateRemoveChild( IWindow &Child )
{
	return CWindow::RemoveChild( Child );
}