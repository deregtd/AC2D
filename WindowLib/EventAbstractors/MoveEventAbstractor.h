#ifndef MOVEEVENTABSTRACTOR_H
#define MOVEEVENTABSTRACTOR_H

#include "Interfaces/IMoveEvent.h"

template< class T >
class MoveEventAbstractor : public IMoveEvent
{
	public:
		virtual bool OnMove( IWindow &Window, float NewLeft, float NewTop ) = 0;
};

#endif