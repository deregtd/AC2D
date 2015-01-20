#ifndef CWINDOW_H
#define CWINDOW_H

#pragma warning( disable : 4584 )

#include <list>
#include "Interfaces/IWindow.h"

#include "EventAbstractors/ClipboardEventsAbstractor.h"
#include "EventAbstractors/FocusEventsAbstractor.h"
#include "EventAbstractors/KeyboardEventsAbstractor.h"
#include "EventAbstractors/MouseEventsAbstractor.h"
#include "EventAbstractors/MoveEventAbstractor.h"
#include "EventAbstractors/RenderEventAbstractor.h"
#include "EventAbstractors/ResizeEventAbstractor.h"

class CWindow : public IWindow, private MoveEventAbstractor< CWindow >, private ResizeEventAbstractor< CWindow >, private MouseEventsAbstractor< CWindow >, private KeyboardEventsAbstractor< CWindow >, private RenderEventAbstractor< CWindow >, private FocusEventsAbstractor< CWindow >, private ClipboardEventsAbstractor< CWindow >
{
	public:
		CWindow();
		virtual ~CWindow();

		virtual bool AddMoveEventHandler( IMoveEvent & MoveEvents );
		virtual bool AddResizeEventHandler( IResizeEvent & ResizeEvent );
		virtual bool AddMouseEventHandler( IMouseEvents & MouseEvents );
		virtual bool AddKeyboardEventHandler( IKeyboardEvents & KeyboardEvents );
		virtual bool AddRenderEventHandler( IRenderEvent & RenderEvent );
		virtual bool AddFocusEventHandler( IFocusEvents & FocusEvents );
		virtual bool AddClipboardEventHandler( IClipboardEvents & ClipboardEvents );

		virtual bool AddWindowEventHandler( IWindowEvents & WindowEvents );

		virtual bool SetWidth( float NewWidth );
		virtual bool SetHeight( float NewHeight );
		virtual bool SetSize( float NewWidth, float NewHeight );

		virtual bool SetTop( float NewTop );
		virtual bool SetLeft( float NewLeft );
		virtual bool SetPosition( float NewLeft, float NewTop );

		virtual float GetLeft() const;
		virtual float GetTop() const;

		virtual float GetWidth() const;
		virtual float GetHeight() const;

		virtual float GetAbsoluteLeft() const;
		virtual float GetAbsoluteTop() const;

		virtual bool SetAnchorTop( bool NewAnchorTop );
		virtual bool SetAnchorLeft( bool NewAnchorLeft );
		virtual bool SetAnchorBottom( bool NewAnchorBottom );
		virtual bool SetAnchorRight( bool NewAnchorRight );

		virtual bool GetAnchorTop() const;
		virtual bool GetAnchorLeft() const;
		virtual bool GetAnchorBottom() const;
		virtual bool GetAnchorRight() const;

		virtual bool SetVisible( bool NewVisible );
		virtual bool GetVisible() const;

		virtual bool JumpToFront();
		virtual bool JumpToBack();

		virtual bool AddChild( IWindow &Child, bool AddToBack = false );
		virtual bool RemoveChild( IWindow &Child );

		virtual bool SetParent( IWindow *NewParent );
		virtual IWindow *GetParent() const;

		virtual const std::list< IWindow * const > &GetChildren() const;

	private:
		virtual bool OnRender( IWindow & Window, double TimeSlice );

		virtual bool OnResize( IWindow &Window, float NewWidth, float NewHeight );
		virtual bool OnResized( IWindow &Window );

		virtual bool OnMove( IWindow &Window, float NewLeft, float NewTop );

		virtual bool OnClick( IWindow &Window, float X, float Y, unsigned long Button );
		virtual bool OnDoubleClick( IWindow &Window, float X, float Y, unsigned long Button );
		virtual bool OnMouseWheel( IWindow &Window, float X, float Y, unsigned long Button );
		virtual bool OnMouseDown( IWindow &Window, float X, float Y, unsigned long Button );
		virtual bool OnMouseUp( IWindow &Window, float X, float Y, unsigned long Button );
		virtual bool OnMouseMove( IWindow &Window, float X, float Y, unsigned long Button );
		virtual bool OnMouseEnter( IWindow &Window, float X, float Y, unsigned long Button );
		virtual bool OnMouseExit( IWindow &Window, float X, float Y, unsigned long Button );

		virtual bool OnKeyPress( IWindow &Window, unsigned long KeyCode );
		virtual bool OnKeyDown( IWindow &Window, unsigned long KeyCode );
		virtual bool OnKeyUp( IWindow &Window, unsigned long KeyCode );

		virtual bool OnGotFocus( IWindow &Window, IWindow *FromWindow );
		virtual bool OnLostFocus( IWindow &Window, IWindow *ToWindow );

		virtual bool OnCut( IWindow &Window, IClipboard &Clipboard );
		virtual bool OnCopy( IWindow &Window, IClipboard &Clipboard );
		virtual bool OnPaste( IWindow &Window, const IClipboard &Clipboard );

	public:
		virtual bool FireRender( double TimeSlice );

		virtual bool FireResize( float NewWidth, float NewHeight );
		virtual bool FireResized();
		virtual bool FireMove( float NewLeft, float NewTop );

		virtual bool FireClick( float X, float Y, unsigned long Button );
		virtual bool FireDoubleClick( float X, float Y, unsigned long Button );
		virtual bool FireMouseWheel( float X, float Y, unsigned long Button );
		virtual bool FireMouseDown( float X, float Y, unsigned long Button );
		virtual bool FireMouseUp( float X, float Y, unsigned long Button );
		virtual bool FireMouseMove( float X, float Y, unsigned long Button );
		virtual bool FireMouseEnter( float X, float Y, unsigned long Button );
		virtual bool FireMouseExit( float X, float Y, unsigned long Button );

		virtual bool FireKeyPress( unsigned long KeyCode );
		virtual bool FireKeyDown( unsigned long KeyCode );
		virtual bool FireKeyUp( unsigned long KeyCode );

		virtual bool FireGotFocus( IWindow *FromWindow );
		virtual bool FireLostFocus( IWindow *ToWindow );

		virtual bool FireCut( IClipboard &Clipboard );
		virtual bool FireCopy( IClipboard &Clipboard );
		virtual bool FirePaste( const IClipboard &Clipboard );

	private:
		float Top;
		float Left;
		
		float Width;
		float Height;

		bool AnchorTop;
		bool AnchorLeft;
		bool AnchorBottom;
		bool AnchorRight;

		std::list< IWindow * const > Children;
		IWindow *Parent;

		bool Updating;
		
		bool Visible;

		std::list< IMoveEvent * const >			MoveEventHandlers;
		std::list< IResizeEvent * const >		ResizeEventHandlers;
		std::list< IMouseEvents * const >		MouseEventHandlers;
		std::list< IKeyboardEvents * const >	KeyboardEventHandlers;
		std::list< IRenderEvent * const >		RenderEventHandlers;
		std::list< IFocusEvents * const >		FocusEventHandlers;
		std::list< IClipboardEvents * const >	ClipboardEventHandlers;

		std::list< IWindowEvents * const >		WindowEventHandlers;
};

#endif