// File: GameWin.cpp
// Author: Ceejay DeGuzman
// Project: CS215 Project 3 Fall 2018
// Description of file contents:

#include <afxwin.h>
#include "GameWin.h"
//#include <mmsystem.h>

//BOOL soundPlayed = PlaySound(CString("SOUND_WAV"), GetModuleHandle(NULL), SND_RESOURCE | SND_ASYNC | SND_LOOP);

GameWin::GameWin ()
{
	// This function will:

	CString WindowTitle = "Welcome to my Reversi Game!";
	Create (NULL, WindowTitle);
	myGameP = new Game (this);
	firstTime = true;
}

afx_msg void GameWin::OnPaint ()
{
	// This function will:

	myGameP->Display ();
	if (firstTime)
	{
		myGameP->Instructions();
//		myGameP->Init();		
//		Invalidate (TRUE);
		firstTime = false;
	}
}

afx_msg void GameWin::OnLButtonDown( UINT nFlags, CPoint point )
{
	// This function will:

	myGameP->Click (point);
}

afx_msg void GameWin::OnLButtonDblClk( UINT nFlags, CPoint point )
{	
	// This function will:

	OnLButtonDown (nFlags, point);
}

afx_msg void GameWin::OnTimer (UINT nIDEvent)
{	
	// This function will: Handle timer events

	if (nIDEvent == 1)
		myGameP->FlipCells (); 
}

BEGIN_MESSAGE_MAP (GameWin, CFrameWnd)
	ON_WM_PAINT ()
	ON_WM_LBUTTONDOWN ()
	ON_WM_LBUTTONDBLCLK ()
	ON_WM_TIMER ()
END_MESSAGE_MAP ()
