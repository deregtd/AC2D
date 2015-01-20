#ifndef ICLIPBOARDEVENTS_H
#define ICLIPBOARDEVENTS_H

#include "IClipboard.h"

class IWindow;

class IClipboardEvents
{
	public:
		virtual bool OnCut( IWindow &Window, IClipboard &Clipboard ) = 0;
		virtual bool OnCopy( IWindow &Window, IClipboard &Clipboard ) = 0;
		virtual bool OnPaste( IWindow &Window, const IClipboard &Clipboard ) = 0;
};

#endif