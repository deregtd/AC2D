#ifndef IMOVEEVENT_H
#define IMOVEEVENT_H

class IWindow;

class IMoveEvent
{
	public:
		virtual bool OnMove( IWindow &Window, float NewLeft, float NewTop ) = 0;
};

#endif