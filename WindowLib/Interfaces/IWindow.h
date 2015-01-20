#ifndef IWINDOW_H
#define IWINDOW_H

class IWindow;

#include "IMoveEvent.h"
#include "IResizeEvent.h"
#include "IMouseEvents.h"
#include "IKeyboardEvents.h"
#include "IRenderEvent.h"
#include "IFocusEvents.h"
#include "IClipboardEvents.h"

#include <list>

class IWindowManager;

class IWindowEvents
{

};

class IWindow /* : public IMoveEvent, public IResizeEvent, public IMouseEvents, public IKeyboardEvents, public IRenderEvent, public IFocusEvents, public IClipboardEvents */
{
	public:
		virtual bool AddMoveEventHandler( IMoveEvent &MoveEvents ) = 0;
		virtual bool AddResizeEventHandler( IResizeEvent &ResizeEvent ) = 0;
		virtual bool AddMouseEventHandler( IMouseEvents &MouseEvents ) = 0;
		virtual bool AddKeyboardEventHandler( IKeyboardEvents &KeyboardEvents ) = 0;
		virtual bool AddRenderEventHandler( IRenderEvent &RenderEvent ) = 0;
		virtual bool AddFocusEventHandler( IFocusEvents &FocusEvents ) = 0;
		virtual bool AddClipboardEventHandler( IClipboardEvents &ClipboardEvents ) = 0;

		virtual bool AddWindowEventHandler( IWindowEvents &WindowEvents ) = 0;

		virtual bool SetWidth( float NewWidth ) = 0;
		virtual bool SetHeight( float NewHeight ) = 0;
		virtual bool SetSize( float NewWidth, float NewHeight ) = 0;

		virtual bool SetTop( float NewTop ) = 0;
		virtual bool SetLeft( float NewLeft ) = 0;
		virtual bool SetPosition( float NewLeft, float NewTop ) = 0;

		virtual float GetLeft() const = 0;
		virtual float GetTop() const = 0;

		virtual float GetWidth() const = 0;
		virtual float GetHeight() const = 0;

		virtual float GetAbsoluteLeft() const = 0;
		virtual float GetAbsoluteTop() const = 0;
		
		virtual bool SetAnchorTop( bool NewAnchorTop ) = 0;
		virtual bool SetAnchorLeft( bool NewAnchorLeft ) = 0;
		virtual bool SetAnchorBottom( bool NewAnchorBottom ) = 0;
		virtual bool SetAnchorRight( bool NewAnchorRight ) = 0;

		virtual bool GetAnchorTop() const = 0;
		virtual bool GetAnchorLeft() const = 0;
		virtual bool GetAnchorBottom() const = 0;
		virtual bool GetAnchorRight() const = 0;

		virtual bool SetVisible( bool NewVisible ) = 0;
		virtual bool GetVisible() const = 0;

		virtual bool JumpToFront() = 0;
		virtual bool JumpToBack() = 0;

		virtual bool AddChild( IWindow &Child, bool AddToBack = false ) = 0;
		virtual bool RemoveChild( IWindow &Child ) = 0;

		virtual bool SetParent( IWindow *NewParent ) = 0;
		virtual IWindow *GetParent() const = 0;

		virtual const std::list< IWindow * const > &GetChildren() const = 0;

		virtual bool FireRender( double TimeSlice ) = 0;

		virtual bool FireResize( float NewWidth, float NewHeight ) = 0;
		virtual bool FireResized() = 0;
		virtual bool FireMove( float NewLeft, float NewTop ) = 0;

		virtual bool FireClick( float X, float Y, unsigned long Button ) = 0;
		virtual bool FireDoubleClick( float X, float Y, unsigned long Button ) = 0;
		virtual bool FireMouseWheel( float X, float Y, unsigned long Button ) = 0;
		virtual bool FireMouseDown( float X, float Y, unsigned long Button ) = 0;
		virtual bool FireMouseUp( float X, float Y, unsigned long Button ) = 0;
		virtual bool FireMouseMove( float X, float Y, unsigned long Button ) = 0;
		virtual bool FireMouseEnter( float X, float Y, unsigned long Button ) = 0;
		virtual bool FireMouseExit( float X, float Y, unsigned long Button ) = 0;

		virtual bool FireKeyPress( unsigned long KeyCode ) = 0;
		virtual bool FireKeyDown( unsigned long KeyCode ) = 0;
		virtual bool FireKeyUp( unsigned long KeyCode ) = 0;

		virtual bool FireGotFocus( IWindow *FromWindow ) = 0;
		virtual bool FireLostFocus( IWindow *ToWindow ) = 0;

		virtual bool FireCut( IClipboard &Clipboard ) = 0;
		virtual bool FireCopy( IClipboard &Clipboard ) = 0;
		virtual bool FirePaste( const IClipboard &Clipboard ) = 0;
};

#endif