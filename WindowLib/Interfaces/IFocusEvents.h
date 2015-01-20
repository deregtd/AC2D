#ifndef IFOCUSEVENTS_H
#define IFOCUSEVENTS_H

class IWindow;

class IFocusEvents
{
	public:
		virtual bool OnGotFocus( IWindow &Window, IWindow *FromWindow ) = 0;
		virtual bool OnLostFocus( IWindow &Window, IWindow *ToWindow ) = 0;
};

#endif