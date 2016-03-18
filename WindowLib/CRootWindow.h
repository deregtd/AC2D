#ifndef CROOTWINDOW_H
#define CROOTWINDOW_H

#include "CWindow.h"

class CRootWindow : public CWindow
{
	public:
		CRootWindow();
		~CRootWindow();

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

		virtual const std::list< IWindow * > &GetChildren() const;

		virtual bool PrivateSetWidth( float NewWidth );
		virtual bool PrivateSetHeight( float NewHeight );
		virtual bool PrivateSetSize( float NewWidth, float NewHeight );

		virtual bool PrivateSetTop( float NewTop );
		virtual bool PrivateSetLeft( float NewLeft );
		virtual bool PrivateSetPosition( float NewLeft, float NewTop );

		virtual bool PrivateAddChild( IWindow &Child );
		virtual bool PrivateRemoveChild( IWindow &Child );
};

#endif