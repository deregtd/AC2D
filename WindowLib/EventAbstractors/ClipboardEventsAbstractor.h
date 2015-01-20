#ifndef CLIPBOARDEVENTSABSTRACTOR_H
#define CLIPBOARDEVENTSABSTRACTOR_H

#include "Interfaces/IClipboardEvents.h"

template< class T >
class ClipboardEventsAbstractor : public IClipboardEvents
{
	public:
		virtual bool OnCut( IWindow &Window, IClipboard &Clipboard ) = 0;
		virtual bool OnCopy( IWindow &Window, IClipboard &Clipboard ) = 0;
		virtual bool OnPaste( IWindow &Window, const IClipboard &Clipboard ) = 0;
};

#endif