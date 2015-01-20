#ifndef FOCUSEVENTSABSTRACTOR_H
#define FOCUSEVENTSABSTRACTOR_H

#include "Interfaces/IFocusEvents.h"

template< class T >
class FocusEventsAbstractor : public IFocusEvents
{
	public:
		virtual bool OnGotFocus( IWindow &Window, IWindow *FromWindow ) = 0;
		virtual bool OnLostFocus( IWindow &Window, IWindow *ToWindow ) = 0;

};

#endif