// File: GameWin.h
// Author: Ceejay DeGuzman
// Project: CS215 Project 3 Spring 2015
// Description of file contents:

// File: GameWin.h
// Author: Ceejay DeGuzman
// Project: CS215 Project 3 Fall 2018
// Description of file contents:

#include <afxwin.h>
#include "Game.h"

class GameWin : public CFrameWnd
{
	public:
		GameWin ();
		afx_msg void OnPaint ();
		afx_msg void OnLButtonDown( UINT nFlags, CPoint point );
		afx_msg void OnLButtonDblClk( UINT nFlags, CPoint point ); 
		afx_msg void OnTimer ( UINT nIDEvent );
	private:
		//Game myGame;
		Game * myGameP;
		bool firstTime;
		DECLARE_MESSAGE_MAP ()
};
