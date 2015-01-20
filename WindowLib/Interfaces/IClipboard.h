#ifndef ICLIPBOARD_H
#define ICLIPBOARD_H

#include <string>

class IClipboard
{
	public:
		virtual const std::string &GetData() const = 0;
		virtual void SetData( const std::string &NewData ) = 0;
};

#endif