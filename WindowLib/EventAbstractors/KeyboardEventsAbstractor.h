#ifndef KEYBOARDEVENTSABSTRACTOR_H
#define KEYBOARDEVENTSABSTRACTOR_H

#include "Interfaces/IKeyboardEvents.h"

template< class T >
class KeyboardEventsAbstractor : public IKeyboardEvents
{
	public:
		virtual bool OnKeyPress( IWindow &Window, unsigned long KeyCode ) = 0;
		virtual bool OnKeyDown( IWindow &Window, unsigned long KeyCode ) = 0;
		virtual bool OnKeyUp( IWindow &Window, unsigned long KeyCode ) = 0;

};

#endif