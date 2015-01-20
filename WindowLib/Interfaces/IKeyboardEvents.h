#ifndef IKEYBOARDEVENTS_H
#define IKEYBOARDEVENTS_H

class IWindow;

class IKeyboardEvents
{
	public:
		virtual bool OnKeyPress( IWindow &Window, unsigned long KeyCode ) = 0;
		virtual bool OnKeyDown( IWindow &Window, unsigned long KeyCode ) = 0;
		virtual bool OnKeyUp( IWindow &Window, unsigned long KeyCode ) = 0;
};

#endif