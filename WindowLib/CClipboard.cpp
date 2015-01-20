#include "stdafx.h"
#include "CClipboard.h"

CClipboard::CClipboard()
{

}

CClipboard::~CClipboard()
{

}

const std::string &CClipboard::GetData() const
{
	return Data;
}

void CClipboard::SetData( const std::string &NewData )
{
	Data = NewData;
}