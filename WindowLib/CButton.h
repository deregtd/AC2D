#ifndef CBUTTON_H
#define CBUTTON_H

#include "CWindow.h"

class CButton : public CWindow
{
	public:
		virtual void OnButtonClick() = 0;
};

#endif