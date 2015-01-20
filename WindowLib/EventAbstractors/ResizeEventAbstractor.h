#ifndef RESIZEEVENTABSTRACTOR_H
#define RESIZEEVENTABSTRACTOR_H

#include "Interfaces/IResizeEvent.h"

template< class T >
class ResizeEventAbstractor : public IResizeEvent
{
	public:
		virtual bool OnResize( IWindow &Window, float NewWidth, float NewHeight ) = 0;
		virtual bool OnResized( IWindow &Window ) = 0;
};

#endif