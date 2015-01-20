#ifndef IWINDOWMANAGER_H
#define IWINDOWMANAGER_H

class IWindowManager;

#include "IWindow.h"

class IWindowManager
{
	public:						
		virtual bool SetWidth( float NewWidth ) = 0;
		virtual float GetWidth() = 0;

		virtual bool SetHeight( float NewHeight ) = 0;
		virtual float GetHeight() = 0;

		virtual bool SetSize( float NewWidth, float NewHeight ) = 0;

		virtual bool AddWindow( IWindow & Window ) = 0;
		virtual bool RemoveWindow( IWindow & Window ) = 0;

		virtual IWindow * SetFocusedWindow( IWindow * Window ) = 0;
		virtual IWindow * GetFocusedWindow() = 0;
		
		virtual IWindow * GetWindowFromXY( float X, float Y ) = 0;

		virtual void OnRender( double TimeSlice ) = 0;
		
		virtual void OnClick( float X, float Y, unsigned long Button ) = 0;
		virtual void OnDoubleClick( float X, float Y, unsigned long Button ) = 0;
		virtual void OnMouseWheel( float X, float Y, unsigned long Button ) = 0;
		virtual void OnMouseDown( float X, float Y, unsigned long Button ) = 0;
		virtual void OnMouseUp( float X, float Y, unsigned long Button ) = 0;
		virtual void OnMouseMove( float X, float Y, unsigned long Button ) = 0;
		virtual void OnMouseEnter( float X, float Y, unsigned long Button ) = 0;
		virtual void OnMouseExit( float X, float Y, unsigned long Button ) = 0;
		
		virtual void OnKeyPress( unsigned long KeyCode ) = 0;
		virtual void OnKeyDown( unsigned long KeyCode ) = 0;
		virtual void OnKeyUp( unsigned long KeyCode ) = 0;
};

#endif