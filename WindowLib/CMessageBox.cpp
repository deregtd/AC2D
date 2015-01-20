#include "stdafx.h"
#include "CMessageBox.h"

#include "Windows.h"

CMessageBox::CMessageBox()
{
	MessageBox( NULL, "CMessageBox Constructor Called", "CMessageBox::CMessageBox()", MB_OK );

	HIJ = 0;
	Okay.SetParent( this );
	Cancel.SetParent( this );

	return;
}

CMessageBox::~CMessageBox()
{
	MessageBox( NULL, "CMessageBox Destructor Called", "CMessageBox::~CMessageBox()", MB_OK );
	return;
}

bool CMessageBox::OkayButton::SetParent( CMessageBox *MyNewCMessageBox )
{
	if( CButton::SetParent( MyNewCMessageBox ) == true )
	{
		MyCMessageBox = MyNewCMessageBox;
		return true;
	}

	return false;
}

void CMessageBox::OkayButton::OnButtonClick()
{
	MyCMessageBox->HIJ = 1;
	MessageBox( NULL, "OkayButton Clicked", "CMessageBox::OkayButton::OnButtonClick()", MB_OK );
	return;
}

bool CMessageBox::CancelButton::SetParent( CMessageBox *MyNewCMessageBox )
{
	if( CButton::SetParent( MyNewCMessageBox ) == true )
	{
		MyCMessageBox = MyNewCMessageBox;
		return true;
	}

	return false;
}

void CMessageBox::CancelButton::OnButtonClick()
{
	MyCMessageBox->HIJ = 2;
	MessageBox( NULL, "CancelButton Clicked", "CMessageBox::CancelButton::OnButtonClick()", MB_OK );
	return;
}