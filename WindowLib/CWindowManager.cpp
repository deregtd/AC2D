#include "stdafx.h"
#include "CWindowManager.h"

CWindowManager::CWindowManager()
{
	FocusedWindow = NULL;
	CapturedWindow = NULL;
	LastMouseMove = NULL;
	
	FocusChangeCount = 0;

	Root.PrivateSetPosition( 0, 0 );
	Root.PrivateSetSize( 1, 1 );

	return;
}

CWindowManager::~CWindowManager()
{
	return;
}

bool CWindowManager::SetWidth( float NewWidth )
{
	return Root.PrivateSetWidth( NewWidth );
}

float CWindowManager::GetWidth()
{
	return Root.GetWidth();
}

bool CWindowManager::SetHeight( float NewHeight )
{
	return Root.PrivateSetHeight( NewHeight );
}

float CWindowManager::GetHeight()
{
	return Root.GetHeight();
}

bool CWindowManager::SetSize( float NewWidth, float NewHeight )
{
	return Root.PrivateSetSize( NewWidth, NewHeight );
}

bool CWindowManager::AddWindow( IWindow &Window )
{
	return Root.PrivateAddChild( Window );
}

bool CWindowManager::RemoveWindow( IWindow &Window )
{
	return Root.PrivateRemoveChild( Window );
}

IWindow *CWindowManager::SetFocusedWindow( IWindow * Window )
{
	bool FoundWindow = false; 
	bool MustResetFocusChangeCount = false;
	
	IWindow *TempWindow = NULL;
	IWindow *OldFocusedWindow = FocusedWindow;
	
	unsigned long MyFocusID = 0;
	
	std::list< IWindow *const >::iterator I;
	
	//IF THIS IS THE FIRST CALL TO SETFOCUSEDWINDOW IN A NESTED SET OF CALLS
	//THIS CALL MUST ALSO RESET THE FOCUSCHANGECOUNT TO 0
	if( FocusChangeCount == 0 )
	{
		MustResetFocusChangeCount = true;
	}
	
	//INCREMENT THE FOCUSCHANGECOUNT AND STORE THE LOCAL FOCUSID
	FocusChangeCount = FocusChangeCount + 1;
	MyFocusID = FocusChangeCount;

	//VERIFY THAT THE WINDOW THAT'S GOING TO RECEIVE FOCUS IS VALID
	//IE A NULL WINDOW OR ONE THAT'S IN THE WINDOW LIST
	if( Window != NULL )
	{
		TempWindow = Window;
		while( TempWindow->GetParent() != NULL )
		{
			TempWindow = TempWindow->GetParent();
		}

		FoundWindow = false;
		if( TempWindow == &Root )
		{
			FoundWindow = true;
		}

		if( FoundWindow == false )
		{
			if( MustResetFocusChangeCount == true )
			{
				FocusChangeCount = 0;
			}

			//RETURN UNSUCCESSFULLY
			return FocusedWindow;
		}
	}
	
	//UPDATE FOCUSEDWINDOW -- THE FOCUSEDWINDOW IS NULL DURING THE LOST AND GOTFOCUS EVENTS
	FocusedWindow = NULL;

	//IF THERE IS A CURRENT WINDOW WITH FOCUS CALL ITS ONLOSTFOCUS EVENT AND RETURN AN ERROR IF THE FOCUS DOES NOT END UP AS EXPECTED
	if( OldFocusedWindow != NULL )
	{
		OldFocusedWindow->FireLostFocus( Window );
		if( MyFocusID != FocusChangeCount )
		{
			if( MustResetFocusChangeCount == true )
			{
				FocusChangeCount = 0;
			}

			//RETURN UNSUCCESSFULLY
			return FocusedWindow;
		}
	}
	
	//IF THERE IS A NEW WINDOW TO RECEIVE FOCUS CALL ITS ONGOTFOCUS EVENT AND RETURN AN ERROR IF THE FOCUS DOES NOT END UP AS EXPECTED
	if( FocusedWindow != NULL )
	{
		FocusedWindow->FireGotFocus( OldFocusedWindow );
		if( MyFocusID != FocusChangeCount )
		{
			if( MustResetFocusChangeCount == true )
			{
				FocusChangeCount = 0;
			}

			//RETURN UNSUCCESSFULLY
			return FocusedWindow;
		}
	}
	
	//JUMP THE WINDOW AND ALL IT'S ANCESTORS TO THE TOP OF THE Z ORDER FOR VISUAL FOCUS
	FocusedWindow = Window;
	TempWindow = FocusedWindow;
	
	while( TempWindow != NULL )
	{
		TempWindow->JumpToFront();
		TempWindow = TempWindow->GetParent();
	}
	
	//RESET THE FOCUSCHANGECOUNT IF NECESSARY
	if( MustResetFocusChangeCount == true )
	{
		FocusChangeCount = 0;
	}

	//RETURN SUCCESSFULLY
	return FocusedWindow;
}

IWindow *CWindowManager::GetFocusedWindow()
{
	return FocusedWindow;
}

IWindow *CWindowManager::GetWindowFromXY( float X, float Y )
{
	std::list< IWindow * const >::const_reverse_iterator I;
	
	IWindow *Window = NULL;
	
	for( I = Root.GetChildren().rbegin(); I != Root.GetChildren().rend(); I++ )
	{
		if( (*I)->GetVisible() == true  )
		{
			Window = GetChildFromXY( *I, X, Y );
			if( Window != NULL )
			{
				return Window;
			}
		}
	}
	
	return NULL;	
}

void CWindowManager::OnRender( double TimeSlice )
{
	std::list< IWindow * const >::const_iterator I;

	for( I = Root.GetChildren().begin(); I != Root.GetChildren().end(); I++ )
	{
		if( (*I)->GetVisible() == true )
		{
			RenderWindow( *I, TimeSlice );
		}
	}
	
	return;
}

void CWindowManager::OnClick( float X, float Y, unsigned long Button )
{
	IWindow *TempWindow;

	TempWindow = GetWindowFromXY( X, Y );

	if( TempWindow != NULL )
	{
		TempWindow->FireClick( X - TempWindow->GetAbsoluteLeft() , Y - TempWindow->GetAbsoluteTop() , Button );
	}
	
	return;
}

void CWindowManager::OnDoubleClick( float X, float Y, unsigned long Button )
{
	IWindow *TempWindow;
	
	TempWindow = GetWindowFromXY( X, Y );

	if( TempWindow != NULL )
	{
		TempWindow->FireDoubleClick( X - TempWindow->GetAbsoluteLeft() , Y - TempWindow->GetAbsoluteTop() , Button );
	}
	
	return;
}

void CWindowManager::OnMouseDown( float X, float Y, unsigned long Button )
{
	IWindow *TempWindow;
	
	TempWindow = GetWindowFromXY( X, Y );
	CapturedWindow = TempWindow;

	SetFocusedWindow( TempWindow );

	if( TempWindow != NULL )
	{
		TempWindow->FireMouseDown( X - TempWindow->GetAbsoluteLeft() , Y - TempWindow->GetAbsoluteTop() , Button );
	}

	return;
}

void CWindowManager::OnMouseWheel( float X, float Y, unsigned long Button )
{
	IWindow *TempWindow;
	
	TempWindow = GetWindowFromXY( X, Y );

	if( TempWindow != NULL )
	{
		TempWindow->FireMouseWheel( X - TempWindow->GetAbsoluteLeft() , Y - TempWindow->GetAbsoluteTop() , Button );
	}

	return;
}

void CWindowManager::OnMouseUp( float X, float Y, unsigned long Button )
{
	IWindow *TempWindow;
	
	if( CapturedWindow == NULL )
	{
		TempWindow = GetWindowFromXY( X, Y );
	} else {
		TempWindow = CapturedWindow;
	}

	if( TempWindow != NULL )
	{
		TempWindow->FireMouseUp( X - TempWindow->GetAbsoluteLeft() , Y - TempWindow->GetAbsoluteTop() , Button );

		if( CapturedWindow == GetWindowFromXY( X, Y ) )
		{
			TempWindow->FireClick( X - TempWindow->GetAbsoluteLeft() , Y - TempWindow->GetAbsoluteTop() , Button );
		}
	}
	
	CapturedWindow = NULL;

	if( LastMouseMove != TempWindow )
	{
		if( LastMouseMove != NULL )
		{
			LastMouseMove->FireMouseExit( X, Y, Button );
		}

		if( TempWindow != NULL )
		{
			TempWindow->FireMouseEnter( X, Y, Button );
		}
	}

	LastMouseMove = TempWindow;

	return;
}

void CWindowManager::OnMouseMove( float X, float Y, unsigned long Button )
{
	IWindow *TempWindow;
	
	if( CapturedWindow == NULL )
	{
		TempWindow = GetWindowFromXY( X, Y );

		if( LastMouseMove != TempWindow )
		{
			if( LastMouseMove != NULL )
			{
				LastMouseMove->FireMouseExit( X, Y, Button );
			}

			if( TempWindow != NULL )
			{
				TempWindow->FireMouseEnter( X, Y, Button );
			}
		}

		LastMouseMove = TempWindow;
	} else {
		TempWindow = CapturedWindow;
	}

	if( TempWindow != NULL )
	{
		TempWindow->FireMouseMove( X - TempWindow->GetAbsoluteLeft() , Y - TempWindow->GetAbsoluteTop() , Button );
	}
	
	return;
}

void CWindowManager::OnMouseEnter( float X, float Y, unsigned long Button )
{
	return;
}

void CWindowManager::OnMouseExit( float X, float Y, unsigned long Button )
{
	return;
}

void CWindowManager::OnKeyPress( unsigned long KeyCode )
{
	std::list< IWindow * const >::const_iterator I;
	std::list< IWindow * const >::const_iterator NextI;
	
	if( FocusedWindow == NULL )
	{
		return;
	}
	
	//DEAL WITH FOCUS SWITCHING
	if( KeyCode == 9 )
	{
		if( FocusedWindow->GetParent() != NULL )
		{
			for( I = FocusedWindow->GetParent()->GetChildren().begin(); I != FocusedWindow->GetParent()->GetChildren().end(); I++ )
			{
				if( FocusedWindow == *I )
				{
					NextI = I++;
					if( NextI != FocusedWindow->GetParent()->GetChildren().end() )
					{
						FocusedWindow = *NextI;
					} else {
						FocusedWindow = *(FocusedWindow->GetParent()->GetChildren().begin() );
					}
					break;
				}
			}	
		}
	}
	
	FocusedWindow->FireKeyPress( KeyCode );
	
	return;
}

void CWindowManager::OnKeyDown( unsigned long KeyCode )
{
	if( FocusedWindow == NULL )
	{
		return;
	}
	
	FocusedWindow->FireKeyDown( KeyCode );
	
	return;
}

void CWindowManager::OnKeyUp( unsigned long KeyCode )
{
	if( FocusedWindow == NULL )
	{
		return;
	}
	
	FocusedWindow->FireKeyUp( KeyCode );
	
	return;
}

void CWindowManager::RenderWindow( IWindow * Window, double TimeSlice )
{
	std::list< IWindow *const >::const_iterator I;
	
	Window->FireRender( TimeSlice );
	
	for( I = Window->GetChildren().begin(); I != Window->GetChildren().end(); I++ )
	{
		if( (*I)->GetVisible() == true )
		{
			RenderWindow( *I, TimeSlice );
		}
	}
	
	return;
}

IWindow *CWindowManager::GetChildFromXY( IWindow * Window, float X, float Y )
{
	IWindow *TempWindow = NULL;

	std::list< IWindow *const >::const_reverse_iterator I;
	
	for( I = Window->GetChildren().rbegin(); I != Window->GetChildren().rend(); I++ )
	{
		if( (*I)->GetVisible() == true )
		{
			TempWindow = GetChildFromXY( *I, X, Y );
			if( TempWindow != NULL )
			{
				return TempWindow;
			}
		}
	}
	
	if( X >= Window->GetAbsoluteLeft() && X <= ( Window->GetAbsoluteLeft() + Window->GetWidth() ) && Y >= Window->GetAbsoluteTop() && Y <= ( Window->GetAbsoluteTop() + Window->GetHeight() ) )
	{
		return Window;
	}
	
	return NULL;
}