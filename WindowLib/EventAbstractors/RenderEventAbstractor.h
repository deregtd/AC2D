#ifndef RENDEREVENTABSTRACTOR_H
#define RENDEREVENTABSTRACTOR_H

#include "Interfaces/IRenderEvent.h"

template< class T >
class RenderEventAbstractor : public IRenderEvent
{
	public:
		virtual bool OnRender( IWindow &Window, double TimeSlice ) = 0;
};

#endif