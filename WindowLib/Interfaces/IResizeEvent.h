#ifndef IRESIZEEVENT_H
#define IRESIZEEVENT_H

class IWindow;

class IResizeEvent
{
	public:
		virtual bool OnResize( IWindow &Window, float NewWidth, float NewHeight ) = 0;
		virtual bool OnResized( IWindow &Window ) = 0;
};

#endif