// File: GameApp.cpp
// Author: Ceejay DeGuzman
// Project: CS215 Project 3 Fall 2018
// Description of file contents:

#include <afxwin.h>
#include "GameApp.h"


BOOL GameApp::InitInstance ()
{
	// This function will:

	m_pMainWnd = new GameWin();
	m_pMainWnd->ShowWindow (m_nCmdShow);
	m_pMainWnd->UpdateWindow ();

	return TRUE;
}

GameApp GameApp;
