#ifndef CCLIPBOARD_H
#define CCLIPBOARD_H

#include "Interfaces/IClipboard.h"

class CClipboard : public IClipboard
{
	public:
		CClipboard();
		virtual ~CClipboard();
		virtual const std::string &GetData() const;
		virtual void SetData( const std::string &NewData );

	private:
		std::string Data;
};

#endif