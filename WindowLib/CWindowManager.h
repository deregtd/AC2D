#ifndef CWINDOWMANAGER_H
#define CWINDOWMANAGER_H

#include "Interfaces/IWindowManager.h"
#include "CWindow.h"
#include "CRootWindow.h"
#include "CClipboard.h"

class CWindowManager : public IWindowManager
{
	public:
		CWindowManager();
		virtual ~CWindowManager();
		
		virtual bool SetWidth( float NewWidth );
		virtual float GetWidth();

		virtual bool SetHeight( float NewHeight );
		virtual float GetHeight();

		virtual bool SetSize( float NewWidth, float NewHeight );

		virtual bool AddWindow( IWindow & Window );
		virtual bool RemoveWindow( IWindow & Window );
		
		virtual IWindow * SetFocusedWindow( IWindow * Window );
		virtual IWindow * GetFocusedWindow();

		virtual IWindow * GetWindowFromXY( float X, float Y );
		
		virtual void OnRender( double TimeSlice );
		
		virtual void OnClick( float X, float Y, unsigned long Button );
		virtual void OnDoubleClick( float X, float Y, unsigned long Button );
		virtual void OnMouseWheel( float X, float Y, unsigned long Button );
		virtual void OnMouseDown( float X, float Y, unsigned long Button );
		virtual void OnMouseUp( float X, float Y, unsigned long Button );
		virtual void OnMouseMove( float X, float Y, unsigned long Button );
		virtual void OnMouseEnter( float X, float Y, unsigned long Button );
		virtual void OnMouseExit( float X, float Y, unsigned long Button );
		
		virtual void OnKeyPress( unsigned long KeyCode );
		virtual void OnKeyDown( unsigned long KeyCode );
		virtual void OnKeyUp( unsigned long KeyCode );
		
	private:
		void RenderWindow( IWindow * Window, double TimeSlice );
		IWindow *GetChildFromXY( IWindow * Window, float X, float Y );
		
		CRootWindow Root;
		CClipboard Clipboard;
		
		unsigned long FocusChangeCount;
		
		IWindow * FocusedWindow;
		IWindow * CapturedWindow;
		IWindow * LastMouseMove;
};

#endif