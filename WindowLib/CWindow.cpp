#include "stdafx.h"
#include "CWindow.h"

CWindow::CWindow()
{
	Top = 0;
	Left = 0;

	Width = 10000;
	Height = 10000;

	AnchorLeft = true;
	AnchorTop = true;
	AnchorRight = false;
	AnchorBottom = false;

	Visible = false;

	Updating = false;
	
	Parent = NULL;

	AddMoveEventHandler( *(MoveEventAbstractor< CWindow > *)this );
	AddResizeEventHandler( *(ResizeEventAbstractor< CWindow > *)this );
	AddMouseEventHandler( *(MouseEventsAbstractor< CWindow > *)this );
	AddKeyboardEventHandler( *(KeyboardEventsAbstractor< CWindow > *)this );
	AddRenderEventHandler( *(RenderEventAbstractor< CWindow > *)this );
	AddFocusEventHandler( *(FocusEventsAbstractor< CWindow > *)this );
	AddClipboardEventHandler( *(ClipboardEventsAbstractor< CWindow > *)this );

	return;
}

CWindow::~CWindow()
{
	if (GetParent())
		GetParent()->RemoveChild(*this);
	return;
}

bool CWindow::AddMoveEventHandler( IMoveEvent &MoveEvents )
{
	for( std::list< IMoveEvent * const >::iterator I = MoveEventHandlers.begin(); I != MoveEventHandlers.end(); I++ )
	{
		if( *I == &MoveEvents )
		{
			return false;
		}
	}

	MoveEventHandlers.push_back( &MoveEvents );

	return true;
}

bool CWindow::AddResizeEventHandler( IResizeEvent &ResizeEvent )
{
	for( std::list< IResizeEvent * const >::iterator I = ResizeEventHandlers.begin(); I != ResizeEventHandlers.end(); I++ )
	{
		if( *I == &ResizeEvent )
		{
			return false;
		}
	}

	ResizeEventHandlers.push_back( &ResizeEvent );

	return true;
}

bool CWindow::AddMouseEventHandler( IMouseEvents &MouseEvents )
{
	for( std::list< IMouseEvents * const >::iterator I = MouseEventHandlers.begin(); I != MouseEventHandlers.end(); I++ )
	{
		if( *I == &MouseEvents )
		{
			return false;
		}
	}

	MouseEventHandlers.push_back( &MouseEvents );

	return true;
}

bool CWindow::AddKeyboardEventHandler( IKeyboardEvents &KeyboardEvents )
{
	for( std::list< IKeyboardEvents * const >::iterator I = KeyboardEventHandlers.begin(); I != KeyboardEventHandlers.end(); I++ )
	{
		if( *I == &KeyboardEvents )
		{
			return false;
		}
	}

	KeyboardEventHandlers.push_back( &KeyboardEvents );

	return true;
}

bool CWindow::AddRenderEventHandler( IRenderEvent &RenderEvent )
{
	for( std::list< IRenderEvent * const >::iterator I = RenderEventHandlers.begin(); I != RenderEventHandlers.end(); I++ )
	{
		if( *I == &RenderEvent )
		{
			return false;
		}
	}

	RenderEventHandlers.push_back( &RenderEvent );

	return true;
}

bool CWindow::AddFocusEventHandler( IFocusEvents &FocusEvents )
{
	for( std::list< IFocusEvents * const >::iterator I = FocusEventHandlers.begin(); I != FocusEventHandlers.end(); I++ )
	{
		if( *I == &FocusEvents )
		{
			return false;
		}
	}

	FocusEventHandlers.push_back( &FocusEvents );

	return true;
}

bool CWindow::AddClipboardEventHandler( IClipboardEvents &ClipboardEvents )
{
	for( std::list< IClipboardEvents * const >::iterator I = ClipboardEventHandlers.begin(); I != ClipboardEventHandlers.end(); I++ )
	{
		if( *I == &ClipboardEvents )
		{
			return false;
		}
	}

	ClipboardEventHandlers.push_back( &ClipboardEvents );

	return true;
}

bool CWindow::AddWindowEventHandler( IWindowEvents &WindowEvents )
{
	for( std::list< IWindowEvents * const >::iterator I = WindowEventHandlers.begin(); I != WindowEventHandlers.end(); I++ )
	{
		if( *I == &WindowEvents )
		{
			return false;
		}
	}

	WindowEventHandlers.push_back( &WindowEvents );

	return true;
}

bool CWindow::SetWidth( float NewWidth )
{
	float LeftMargin = 0;
	float RightMargin = 0;

	bool AnchoredLeft = false;
	bool AnchoredRight = false;

	std::list< IWindow *const >::iterator I;

	if( NewWidth < 0 )
	{
		return false;
	}

	if( FireResize( NewWidth, Height ) == true )
	{
		for( I = Children.begin(); I != Children.end(); I++ )
		{
			AnchoredLeft = (*I)->GetAnchorLeft();
			AnchoredRight = (*I)->GetAnchorRight();

			if( AnchoredLeft == true && AnchoredRight == false )
			{

			}
			else if( AnchoredLeft == false && AnchoredRight == true )
			{
				RightMargin = Width - (*I)->GetLeft() - (*I)->GetWidth();
				(*I)->SetLeft( NewWidth - RightMargin - (*I)->GetWidth() );
			}
			else if( AnchoredLeft == true && AnchoredRight == true )
			{
				LeftMargin = (*I)->GetLeft();
				RightMargin = Width - (*I)->GetLeft() - (*I)->GetWidth();

				if( NewWidth - LeftMargin - RightMargin > 0 ) 
				{
					(*I)->SetWidth( NewWidth - LeftMargin - RightMargin );
				}
			}
			else if( AnchoredLeft == false && AnchoredRight == false )
			{
				(*I)->SetLeft( (*I)->GetLeft() + ( NewWidth - Width ) / 2 );
			}
		}

		Width = NewWidth;
		FireResized();
	} else {
		return false;
	}

	return true;
}

bool CWindow::SetHeight( float NewHeight )
{
	float TopMargin = 0;
	float BottomMargin = 0;

	bool AnchoredTop = false;
	bool AnchoredBottom = false;

	std::list< IWindow *const >::iterator I;

	if( NewHeight < 0 )
	{
		return false;
	}

	if( FireResize( Width, NewHeight ) == true )
	{
		for( I = Children.begin(); I != Children.end(); I++ )
		{
			AnchoredTop = (*I)->GetAnchorTop();
			AnchoredBottom = (*I)->GetAnchorBottom();

			if( AnchoredTop == true && AnchoredBottom == false )
			{

			}
			else if( AnchoredTop == false && AnchoredBottom == true )
			{
				BottomMargin = Height - (*I)->GetTop() - (*I)->GetHeight();
				(*I)->SetTop( NewHeight - BottomMargin - (*I)->GetHeight() );
			}
			else if( AnchoredTop == true && AnchoredBottom == true )
			{
				TopMargin = (*I)->GetTop();
				BottomMargin = Height - (*I)->GetTop() - (*I)->GetHeight();

				if( NewHeight - TopMargin - BottomMargin > 0 ) 
				{
					(*I)->SetHeight( NewHeight - TopMargin - BottomMargin );
				}
			}
			else if( AnchoredTop == false && AnchoredBottom == false )
			{
				(*I)->SetTop( (*I)->GetTop() + ( NewHeight - Height ) / 2 );
			}
		}

		Height = NewHeight;
		FireResized();
	} else {
		return false;
	}

	return true;
}

bool CWindow::SetSize( float NewWidth, float NewHeight )
{
	float LeftMargin = 0;
	float TopMargin = 0;
	float RightMargin = 0;
	float BottomMargin = 0;
	
	bool AnchoredLeft = false;
	bool AnchoredTop = false;
	bool AnchoredRight = false;
	bool AnchoredBottom = false;
	
	std::list< IWindow *const >::iterator I;
	
	if( NewWidth < 0 || NewHeight < 0 )
	{
		return false;
	}
	
	if( FireResize( NewWidth, NewHeight ) == true )
	{
		for( I = Children.begin(); I != Children.end(); I++ )
		{
			AnchoredLeft = (*I)->GetAnchorLeft();
			AnchoredTop = (*I)->GetAnchorTop();
			AnchoredRight = (*I)->GetAnchorRight();
			AnchoredBottom = (*I)->GetAnchorBottom();
			
			//LEFT AND TOP ARE ANCHORED
			if( AnchoredLeft == true && AnchoredRight == false && AnchoredTop == true && AnchoredBottom == false )
			{

			}
			//LEFT, TOP, RIGHT AND BOTTOM ARE ANCHORED
			else if( AnchoredLeft == true && AnchoredRight == true && AnchoredTop == true && AnchoredBottom == true )
			{
				LeftMargin = (*I)->GetLeft();
				TopMargin = (*I)->GetTop();
				RightMargin = Width - (*I)->GetLeft() - (*I)->GetWidth();
				BottomMargin = Height - (*I)->GetTop() - (*I)->GetHeight();

				if( NewHeight - TopMargin - BottomMargin > 0 && NewWidth - LeftMargin - RightMargin > 0 )
				{
					(*I)->SetSize( NewWidth - LeftMargin - RightMargin, NewHeight - TopMargin - BottomMargin );
				}
			}
			//TOP AND RIGHT ARE ANCHORED
			else if( AnchoredLeft == false && AnchoredRight == true && AnchoredTop == true && AnchoredBottom == false )
			{
				RightMargin = Width - (*I)->GetLeft() - (*I)->GetWidth();
				(*I)->SetLeft( NewWidth - RightMargin - (*I)->GetWidth() );
			}
			//RIGHT AND BOTTOM ARE ANCHORED
			else if( AnchoredLeft == false && AnchoredRight == true && AnchoredTop == false && AnchoredBottom == true )
			{
				RightMargin = Width - (*I)->GetLeft() - (*I)->GetWidth();
				BottomMargin = Height - (*I)->GetTop() - (*I)->GetHeight();

				(*I)->SetPosition( NewWidth - RightMargin - (*I)->GetWidth(), NewHeight - BottomMargin - (*I)->GetHeight() );
			}
			//LEFT AND BOTTOM ARE ANCHORED
			else if( AnchoredLeft == true && AnchoredRight == false && AnchoredTop == false && AnchoredBottom == true )
			{
				BottomMargin = Height - (*I)->GetTop() - (*I)->GetHeight();
				(*I)->SetTop( NewHeight - BottomMargin - (*I)->GetHeight() );
			}
			//LEFT, TOP AND BOTTOM ARE ANCHORED
			else if( AnchoredLeft == true && AnchoredRight == false && AnchoredTop == true && AnchoredBottom == true )
			{
				TopMargin = (*I)->GetTop();
				BottomMargin = Height - (*I)->GetTop() - (*I)->GetHeight();

				if( NewHeight - TopMargin - BottomMargin > 0 ) 
				{
					(*I)->SetHeight( NewHeight - TopMargin - BottomMargin );
				}
			}
			//TOP, RIGHT AND BOTTOM ARE ANCHORED
			else if( AnchoredLeft == false && AnchoredRight == true && AnchoredTop == true && AnchoredBottom == true )
			{
				TopMargin = (*I)->GetTop();
				RightMargin = Width - (*I)->GetLeft() - (*I)->GetWidth();
				BottomMargin = Height - (*I)->GetTop() - (*I)->GetHeight();

				(*I)->SetLeft( NewWidth - RightMargin - (*I)->GetWidth() );

				if( NewHeight - TopMargin - BottomMargin > 0 ) 
				{
					(*I)->SetHeight( NewHeight - TopMargin - BottomMargin );
				}
			}
			//LEFT, TOP AND RIGHT ARE ANCHORED
			else if( AnchoredLeft == true && AnchoredRight == true && AnchoredTop == true && AnchoredBottom == false )
			{
				LeftMargin = (*I)->GetLeft();
				RightMargin = Width - (*I)->GetLeft() - (*I)->GetWidth();

				if( NewWidth - LeftMargin - RightMargin > 0 ) 
				{
					(*I)->SetWidth( NewWidth - LeftMargin - RightMargin );
				}
			}
			//LEFT, RIGHT AND BOTTOM ARE ANCHORED
			else if( AnchoredLeft == true && AnchoredRight == true && AnchoredTop == false && AnchoredBottom == true )
			{			
				LeftMargin = (*I)->GetLeft();
				RightMargin = Width - (*I)->GetLeft() - (*I)->GetWidth();
				BottomMargin = Height - (*I)->GetTop() - (*I)->GetHeight();

				(*I)->SetTop( NewHeight - BottomMargin - (*I)->GetHeight() );

				if( NewWidth - LeftMargin - RightMargin > 0 ) 
				{
					(*I)->SetWidth( NewWidth - LeftMargin - RightMargin );
				}
			}
			//LEFT AND RIGHT ARE ANCHORED
			else if( AnchoredLeft == true && AnchoredRight == true && AnchoredTop == false && AnchoredBottom == false )
			{
				LeftMargin = (*I)->GetLeft();
				RightMargin = Width - (*I)->GetLeft() - (*I)->GetWidth();
				
				(*I)->SetTop( (*I)->GetTop() + ( NewHeight - Height ) / 2 );

				if( NewWidth - LeftMargin - RightMargin > 0 ) 
				{
					(*I)->SetWidth( NewWidth - LeftMargin - RightMargin );
				}
			}
			//TOP AND BOTTOM ARE ANCHORED
			else if( AnchoredLeft == false && AnchoredRight == false && AnchoredTop == true && AnchoredBottom == true )
			{
				TopMargin = (*I)->GetTop();
				BottomMargin = Height - (*I)->GetTop() - (*I)->GetHeight();

				(*I)->SetLeft( (*I)->GetLeft() + ( NewWidth - Width ) / 2 );

				if( NewHeight - TopMargin - BottomMargin > 0 ) 
				{
					(*I)->SetHeight( NewHeight - TopMargin - BottomMargin );
				}
			}
			//LEFT IS ANCHORED
			else if( AnchoredLeft == true && AnchoredRight == false && AnchoredTop == false && AnchoredBottom == false )
			{
				(*I)->SetTop( (*I)->GetTop() + ( NewHeight - Height ) / 2 );
			}
			//TOP IS ANCHORED
			else if( AnchoredLeft == false && AnchoredRight == false && AnchoredTop == true && AnchoredBottom == false )
			{
				(*I)->SetLeft( (*I)->GetLeft() + ( NewWidth - Width ) / 2 );
			}
			//RIGHT IS ANCHORED
			else if( AnchoredLeft == false && AnchoredRight == true && AnchoredTop == false && AnchoredBottom == false )
			{
				RightMargin = Width - (*I)->GetLeft() - (*I)->GetWidth();
				(*I)->SetPosition( NewWidth - RightMargin - (*I)->GetWidth(), (*I)->GetTop() + ( NewHeight - Height ) / 2 );
			}
			//BOTTOM IS ANCHORED
			else if( AnchoredLeft == false && AnchoredRight == false && AnchoredTop == false && AnchoredBottom == true )
			{
				BottomMargin = Height - (*I)->GetTop() - (*I)->GetHeight();

				(*I)->SetPosition( (*I)->GetLeft() + ( NewWidth - Width ) / 2, NewHeight - BottomMargin - (*I)->GetHeight() );
			}
			//NOTHING IS ANCHORED
			else if( AnchoredLeft == false && AnchoredRight == false && AnchoredTop == false && AnchoredBottom == false )
			{
				(*I)->SetPosition( (*I)->GetLeft() + ( NewWidth - Width ) / 2, (*I)->GetTop() + ( NewHeight - Height ) / 2 );
			}
		}

		Width = NewWidth;
		Height = NewHeight;

		FireResized();
	} else {
		return false;
	}

	return true;
}

bool CWindow::SetTop( float NewTop )
{
	if( FireMove( Left, NewTop ) == true )
	{
		Top = NewTop;
	} else {
		return false;
	}

	return true;
}

bool CWindow::SetLeft( float NewLeft )
{
	if( FireMove( NewLeft, Top ) == true )
	{
		Left = NewLeft;
	} else {
		return false;
	}

	return true;
}

bool CWindow::SetPosition( float NewLeft, float NewTop )
{
	if( FireMove( NewLeft, NewTop ) == true )
	{
		Left = NewLeft;
		Top = NewTop;
	} else {
		return false;
	}

	return true;
}

float CWindow::GetLeft() const
{
	return Left;
}

float CWindow::GetTop() const
{
	return Top;
}

float CWindow::GetWidth() const
{
	return Width;
}

float CWindow::GetHeight() const
{
	return Height;
}

float CWindow::GetAbsoluteLeft() const
{
	return Left + Parent->GetAbsoluteLeft();
}

float CWindow::GetAbsoluteTop() const
{
	return Top + Parent->GetAbsoluteTop();
}

bool CWindow::SetAnchorTop( bool NewAnchorTop )
{
	AnchorTop = NewAnchorTop;

	return true;
}

bool CWindow::SetAnchorLeft( bool NewAnchorLeft )
{
	AnchorLeft = NewAnchorLeft;
	
	return true;
}

bool CWindow::SetAnchorBottom( bool NewAnchorBottom )
{
	AnchorBottom = NewAnchorBottom;
	
	return true;
}

bool CWindow::SetAnchorRight( bool NewAnchorRight )
{
	AnchorRight = NewAnchorRight;

	return true;
}

bool CWindow::GetAnchorTop() const
{
	return AnchorTop;
}

bool CWindow::GetAnchorLeft() const
{
	return AnchorLeft;
}

bool CWindow::GetAnchorBottom() const
{
	return AnchorBottom;
}

bool CWindow::GetAnchorRight() const
{
	return AnchorRight;
}

bool CWindow::SetVisible( bool NewVisible )
{
	Visible = NewVisible;

	return true;
}

bool CWindow::GetVisible() const
{
	return Visible;
}

bool CWindow::JumpToFront()
{
	if( Parent == NULL )
	{
		return true;
	}

	try
	{
		IWindow *OldParent = Parent;
		Parent->RemoveChild( *this );
		OldParent->AddChild( *this, true );
		//Parent->GetChildren().remove( (IWindow *)this );
		//Parent->GetChildren().push_back( this );
	} catch( ... ) {
		return false;
	}

	return true;
}

bool CWindow::JumpToBack()
{
	if( Parent == NULL )
	{
		return true;
	}

	try
	{
		IWindow *OldParent = Parent;
		Parent->RemoveChild( *this );
		OldParent->AddChild( *this );
	} catch( ... ) {
		return false;
	}

	return true;
}

bool CWindow::AddChild( IWindow &Child, bool AddToBack )
{
	//THIS SHOULD BE CLEANED UP WHEN A CUSTOM LIST TAKES THE PLACE OF STD::LIST
	//ALSO, THIS UPDATING FLAG IS ASS, BUT I CAN'T THINK OF A BETTER FIX AT THE MOMENT
	
	if( &Child == this )
	{
		return false;
	}

	if( Updating == true )
	{
		return true;
	}
	
	Updating = true;
	
	if( Child.GetParent() != NULL )
	{
		Child.GetParent()->RemoveChild( Child );
	}

	try
	{	
		if( AddToBack == false )
		{
			Children.push_front( &Child );
		}
		else
		{
			Children.push_back( &Child );
		}
	} catch( ... ) {
		Updating = false;
		return false;
	}

	Child.SetParent( this );

	Updating = false;
	
	return true;
}

bool CWindow::RemoveChild( IWindow &Child )
{
	//THIS SHOULD BE CLEANED UP WHEN A CUSTOM LIST TAKES THE PLACE OF STD::LIST
	//ALSO, THIS UPDATING FLAG IS ASS, BUT I CAN'T THINK OF A BETTER FIX AT THE MOMENT

	if( Updating == true )
	{
		return true;
	}

	Updating = true;

	try
	{
		Children.remove( &Child );
	} catch( ... ) {
		Updating = false;
		return false;
	}

	Child.SetParent( NULL );
	
	Updating = false;
	
	return true;
}

bool CWindow::SetParent( IWindow *NewParent )
{
	//THIS UPDATING FLAG IS ASS, BUT I CAN'T THINK OF A BETTER FIX AT THE MOMENT

	if( NewParent == this )
	{
		return false;
	}

	if( Updating == true )
	{
		return true;
	}

	Updating = true;

	if( Parent != NULL )
	{
		Parent->RemoveChild( *this );
	}

	Parent = NewParent;

	if( Parent != NULL )
	{
		Parent->AddChild( *this );
	}

	Updating = false;

	return true;
}

IWindow *CWindow::GetParent() const
{
	return Parent;
}

const std::list< IWindow * const > &CWindow::GetChildren() const
{
	return Children;
}

bool CWindow::OnRender( IWindow & Window, double TimeSlice )
{
	return false;
}

bool CWindow::OnResize( IWindow & Window, float NewWidth, float NewHeight )
{
	return true;
}

bool CWindow::OnResized( IWindow & Window )
{
	return false;
}

bool CWindow::OnMove( IWindow & Window, float NewLeft, float NewTop )
{
	return true;
}

bool CWindow::OnClick( IWindow & Window, float X, float Y, unsigned long Button )
{
	return false;
}

bool CWindow::OnDoubleClick( IWindow & Window, float X, float Y, unsigned long Button )
{
	return false;
}

bool CWindow::OnMouseWheel( IWindow & Window, float X, float Y, unsigned long Button )
{
	return false;
}

bool CWindow::OnMouseDown( IWindow & Window, float X, float Y, unsigned long Button )
{
	return false;
}

bool CWindow::OnMouseUp( IWindow & Window, float X, float Y, unsigned long Button )
{
	return false;
}

bool CWindow::OnMouseMove( IWindow & Window, float X, float Y, unsigned long Button )
{
	return false;
}

bool CWindow::OnMouseEnter( IWindow & Window, float X, float Y, unsigned long Button )
{
	return false;
}

bool CWindow::OnMouseExit( IWindow & Window, float X, float Y, unsigned long Button )
{
	return false;
}

bool CWindow::OnKeyPress( IWindow & Window, unsigned long KeyCode )
{
	return false;
}

bool CWindow::OnKeyDown( IWindow & Window, unsigned long KeyCode )
{
	return false;
}

bool CWindow::OnKeyUp( IWindow & Window, unsigned long KeyCode )
{	
	return false;
}

bool CWindow::OnGotFocus( IWindow & Window, IWindow *FromWindow )
{
	return false;
}

bool CWindow::OnLostFocus( IWindow & Window, IWindow *ToWindow )
{
	return false;
}

bool CWindow::OnCut( IWindow & Window, IClipboard &Clipboard )
{
	return false;
}

bool CWindow::OnCopy( IWindow & Window, IClipboard &Clipboard )
{
	return false;
}

bool CWindow::OnPaste( IWindow & Window, const IClipboard &Clipboard )
{
	return false;
}

bool CWindow::FireRender( double TimeSlice )
{
	bool Temp = false;
	bool RetVal = false;

	for( std::list< IRenderEvent * const >::iterator I = RenderEventHandlers.begin(); I != RenderEventHandlers.end(); I++ )
	{
		Temp = (*I)->OnRender( *this, TimeSlice );

		if( Temp == true )
		{
			RetVal = true;
		}
	}

	return RetVal;
}

bool CWindow::FireResize( float NewWidth, float NewHeight )
{
	bool Temp = false;
	bool RetVal = false;

	for( std::list< IResizeEvent * const >::iterator I = ResizeEventHandlers.begin(); I != ResizeEventHandlers.end(); I++ )
	{
		IResizeEvent * const foo = *I;
		Temp = (*I)->OnResize( *this, NewWidth, NewHeight );

		if( Temp == true )
		{
			RetVal = true;
		}
	}

	return RetVal;
}

bool CWindow::FireResized()
{
	bool Temp = false;
	bool RetVal = false;

	for( std::list< IResizeEvent * const >::iterator I = ResizeEventHandlers.begin(); I != ResizeEventHandlers.end(); I++ )
	{
		Temp = (*I)->OnResized( *this );

		if( Temp == true )
		{
			RetVal = true;
		}
	}

	return RetVal;
}

bool CWindow::FireMove( float NewLeft, float NewTop )
{
	bool Temp = false;
	bool RetVal = false;

	for( std::list< IMoveEvent * const >::iterator I = MoveEventHandlers.begin(); I != MoveEventHandlers.end(); I++ )
	{
		Temp = (*I)->OnMove( *this, NewLeft, NewTop );

		if( Temp == true )
		{
			RetVal = true;
		}
	}

	return RetVal;
}

bool CWindow::FireClick( float X, float Y, unsigned long Button )
{
	bool Temp = false;
	bool RetVal = false;

	for( std::list< IMouseEvents * const >::iterator I = MouseEventHandlers.begin(); I != MouseEventHandlers.end(); I++ )
	{
		Temp = (*I)->OnClick( *this, X, Y, Button );

		if( Temp == true )
		{
			RetVal = true;
		}
	}

	return RetVal;
}

bool CWindow::FireDoubleClick( float X, float Y, unsigned long Button )
{
	bool Temp = false;
	bool RetVal = false;

	for( std::list< IMouseEvents * const >::iterator I = MouseEventHandlers.begin(); I != MouseEventHandlers.end(); I++ )
	{
		Temp = (*I)->OnDoubleClick( *this, X, Y, Button );

		if( Temp == true )
		{
			RetVal = true;
		}
	}

	return RetVal;
}

bool CWindow::FireMouseWheel( float X, float Y, unsigned long Button )
{
	bool Temp = false;
	bool RetVal = false;

	for( std::list< IMouseEvents * const >::iterator I = MouseEventHandlers.begin(); I != MouseEventHandlers.end(); I++ )
	{
		Temp = (*I)->OnMouseWheel( *this, X, Y, Button );

		if( Temp == true )
		{
			RetVal = true;
		}
	}

	return RetVal;
}

bool CWindow::FireMouseDown( float X, float Y, unsigned long Button )
{
	bool Temp = false;
	bool RetVal = false;

	for( std::list< IMouseEvents * const >::iterator I = MouseEventHandlers.begin(); I != MouseEventHandlers.end(); I++ )
	{
		Temp = (*I)->OnMouseDown( *this, X, Y, Button );

		if( Temp == true )
		{
			RetVal = true;
		}
	}

	return RetVal;
}

bool CWindow::FireMouseUp( float X, float Y, unsigned long Button )
{
	bool Temp = false;
	bool RetVal = false;

	for( std::list< IMouseEvents * const >::iterator I = MouseEventHandlers.begin(); I != MouseEventHandlers.end(); I++ )
	{
		Temp = (*I)->OnMouseUp( *this, X, Y, Button );

		if( Temp == true )
		{
			RetVal = true;
		}
	}

	return RetVal;
}

bool CWindow::FireMouseMove( float X, float Y, unsigned long Button )
{
	bool Temp = false;
	bool RetVal = false;

	for( std::list< IMouseEvents * const >::iterator I = MouseEventHandlers.begin(); I != MouseEventHandlers.end(); I++ )
	{
		Temp = (*I)->OnMouseMove( *this, X, Y, Button );

		if( Temp == true )
		{
			RetVal = true;
		}
	}

	return RetVal;
}

bool CWindow::FireMouseEnter( float X, float Y, unsigned long Button )
{
	bool Temp = false;
	bool RetVal = false;

	for( std::list< IMouseEvents * const >::iterator I = MouseEventHandlers.begin(); I != MouseEventHandlers.end(); I++ )
	{
		Temp = (*I)->OnMouseEnter( *this, X, Y, Button );

		if( Temp == true )
		{
			RetVal = true;
		}
	}

	return RetVal;
}

bool CWindow::FireMouseExit( float X, float Y, unsigned long Button )
{
	bool Temp = false;
	bool RetVal = false;

	for( std::list< IMouseEvents * const >::iterator I = MouseEventHandlers.begin(); I != MouseEventHandlers.end(); I++ )
	{
		Temp = (*I)->OnMouseExit( *this, X, Y, Button );

		if( Temp == true )
		{
			RetVal = true;
		}
	}

	return RetVal;
}

bool CWindow::FireKeyPress( unsigned long KeyCode )
{
	bool Temp = false;
	bool RetVal = false;

	for( std::list< IKeyboardEvents * const >::iterator I = KeyboardEventHandlers.begin(); I != KeyboardEventHandlers.end(); I++ )
	{
		Temp = (*I)->OnKeyPress( *this, KeyCode );

		if( Temp == true )
		{
			RetVal = true;
		}
	}

	return RetVal;
}
bool CWindow::FireKeyDown( unsigned long KeyCode )
{
	bool Temp = false;
	bool RetVal = false;

	for( std::list< IKeyboardEvents * const >::iterator I = KeyboardEventHandlers.begin(); I != KeyboardEventHandlers.end(); I++ )
	{
		Temp = (*I)->OnKeyDown( *this, KeyCode );

		if( Temp == true )
		{
			RetVal = true;
		}
	}

	return RetVal;
}
bool CWindow::FireKeyUp( unsigned long KeyCode )
{
	bool Temp = false;
	bool RetVal = false;

	for( std::list< IKeyboardEvents * const >::iterator I = KeyboardEventHandlers.begin(); I != KeyboardEventHandlers.end(); I++ )
	{
		Temp = (*I)->OnKeyUp( *this, KeyCode );

		if( Temp == true )
		{
			RetVal = true;
		}
	}

	return RetVal;
}

bool CWindow::FireGotFocus( IWindow *FromWindow )
{
	bool Temp = false;
	bool RetVal = false;

	for( std::list< IFocusEvents * const >::iterator I = FocusEventHandlers.begin(); I != FocusEventHandlers.end(); I++ )
	{
		Temp = (*I)->OnGotFocus( *this, FromWindow );

		if( Temp == true )
		{
			RetVal = true;
		}
	}

	return RetVal;
}

bool CWindow::FireLostFocus( IWindow *ToWindow )
{
	bool Temp = false;
	bool RetVal = false;

	for( std::list< IFocusEvents * const >::iterator I = FocusEventHandlers.begin(); I != FocusEventHandlers.end(); I++ )
	{
		Temp = (*I)->OnLostFocus( *this, ToWindow );

		if( Temp == true )
		{
			RetVal = true;
		}
	}

	return RetVal;
}

bool CWindow::FireCut( IClipboard &Clipboard )
{
	bool Temp = false;
	bool RetVal = false;

	for( std::list< IClipboardEvents * const >::iterator I = ClipboardEventHandlers.begin(); I != ClipboardEventHandlers.end(); I++ )
	{
		Temp = (*I)->OnCut( *this, Clipboard );

		if( Temp == true )
		{
			RetVal = true;
		}
	}

	return RetVal;
}

bool CWindow::FireCopy( IClipboard &Clipboard )
{
	bool Temp = false;
	bool RetVal = false;

	for( std::list< IClipboardEvents * const >::iterator I = ClipboardEventHandlers.begin(); I != ClipboardEventHandlers.end(); I++ )
	{
		Temp = (*I)->OnCopy( *this, Clipboard );

		if( Temp == true )
		{
			RetVal = true;
		}
	}

	return RetVal;
}

bool CWindow::FirePaste( const IClipboard &Clipboard )
{
	bool Temp = false;
	bool RetVal = false;

	for( std::list< IClipboardEvents * const >::iterator I = ClipboardEventHandlers.begin(); I != ClipboardEventHandlers.end(); I++ )
	{
		Temp = (*I)->OnPaste( *this, Clipboard );

		if( Temp == true )
		{
			RetVal = true;
		}
	}

	return RetVal;
}