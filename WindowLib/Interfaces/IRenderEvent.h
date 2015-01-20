#ifndef IRENDEREVENT_H
#define IRENDEREVENT_H

class IWindow;

class IRenderEvent
{
	public:
		virtual bool OnRender( IWindow &Window, double TimeSlice ) = 0;
};

#endif