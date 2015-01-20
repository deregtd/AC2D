#ifndef CMESSAGEBOX_H
#define CMESSAGEBOX_H

#include "CWindow.h"
#include "CButton.h"

class CMessageBox : public CWindow
{
	public:
		CMessageBox();
		~CMessageBox();

	public:
		class OkayButton : public CButton
		{
			public:
				virtual void OnButtonClick();
				virtual bool SetParent( CMessageBox *MyCMessageBox );

			private:
				CMessageBox *MyCMessageBox;
		};

		class CancelButton : public CButton
		{
			public:
				virtual void OnButtonClick();
				virtual bool SetParent( CMessageBox *MyCMessageBox );

			private:
				CMessageBox *MyCMessageBox;
		};

	public:
		OkayButton Okay;
		CancelButton Cancel;

	private:
		int HIJ;
};

#endif