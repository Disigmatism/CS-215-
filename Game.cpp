// File: Game.cpp
// Author: Ceejay DeGuzman
// Project: CS215 Project 3 Fall 2018
// Description of file contents:

#include "Game.h"
#include <iomanip>
#include <cstdlib>

static CBitmap images [NUMIMAGES];
static COLORREF colors [NUMIMAGES];

Game::RCPair::RCPair ()
{
	row = -1;
	col = -1;
}

Game::RCPair::RCPair (int R, int C)
{
	row = R;
	col = C;
}

Game::Cell::Cell ()
{
	owner = NONE;
	//Change or flip the score and the moves
	scoreCheck = 50;
	flipt = flipl = flipr = flipb = 0;
	fliptl = 0;
	fliptr = 0;
	flipbl = 0;
	flipbr = 0;
}

void Game::Cell::Display(CDC * deviceContextP)
{
	deviceContextP->Rectangle(where);
	if (owner != NONE)	
	{
		CDC memDC;
		int res2 = memDC.CreateCompatibleDC(deviceContextP);
		memDC.SelectObject(&images[owner]);
		res2 = deviceContextP->TransparentBlt (where.left+1, where.top+1, where.Width()-2, where.Height()-2,
			&memDC, 0, 0, 290, 290, 1);
		DeleteDC (memDC);
	}
}

Game::Game (CFrameWnd * WP)
{
	windowP = WP;
	int res = bgImage.LoadBitmap(CString("BACKGROUND_BMP"));
	res = images[HUMAN].LoadBitmap(CString("HUMAN_GAME_PIECE_BMP"));
	res = images[COMPUTER].LoadBitmap(CString("COMPUTER_GAME_PIECE_BMP"));		
	colors[COMPUTER] = RGB (255, 255, 255);
	colors[HUMAN] = RGB(80, 123, 187);
	Init ();
}

void Game::Init ()
{
	Cell cell;
	int s = 0;
	int scores[] = { 99, 1 , 80, 1, 1, 35, 80, 35, 90 };
	for (int r = 1; r <= ROWS; r++)
		for (int c = 1; c <= COLS; c++)
			board[r][c] = cell;
	board[ROWS/2][COLS/2].owner = COMPUTER;
	board[ROWS/2][COLS/2+1].owner = HUMAN;
	board[ROWS/2+1][COLS/2].owner = HUMAN;
	board[ROWS/2+1][COLS/2+1].owner = COMPUTER;
	score[HUMAN] = 2;
	score[COMPUTER] = 2;
	score[HUMAN] = 2;
	for (int r = 1; r <= 3; r++)
	{
		for (int c = 1; c <= 3; c++)
		{
			board[r][c].scoreCheck = scores[s];
			s++;
		}
	}
	s = 0;
	board[3][4].scoreCheck = board[3][5].scoreCheck = board[4][3].scoreCheck = board[5][3].scoreCheck = 65;
	board[4][6].scoreCheck = board[5][6].scoreCheck = board[6][4].scoreCheck = board[6][5].scoreCheck = 65;
	for (int r = 1; r <= 3; r++)
	{
		for (int c = 8; c > 5; c--)
		{
			board[r][c].scoreCheck = scores[s];
			s++;
		}
	}
	s = 0;
	for (int r = 1; r > 5; r--)
	{
		for (int c = 1; c <= 3; c++) 
		{
			board[r][c].scoreCheck = scores[s];
			s++;
		}
	}
	s = 0;
	for (int r = 8; r > 5; r--)
	{
		for (int c = 8; c > 5; c--)
		{
			board[r][c].scoreCheck = scores[s];
			s++;
		}
	}

	whoseTurn = HUMAN;
	CheckBoard (HUMAN);
	CheckBoard (COMPUTER);
	currentRect = CRect();
	windowP->Invalidate(TRUE);
}

void Game::Instructions ()
{
	CString message = "Welcome to Reversi Nite at Pop's in Riverdale\n";

	message += "\nEnjoy a live song from Riverdale's main character's Archie and Veronica ft. Betty. Song name is Mad World\n";

	message += "\nHey Gang, Serpents by blood.\n";
	message += "\nSimple Instructions\n";
	message += "\nYou will be playing as the SouthSide Serpents challenging Pop’s to a\n";
	message += "game of reversi or othello. The objective is easy enough, use your\n";
	message += "SouthSide skills to flank Pop’s pieces and flipping more pieces than\n";
	message += "Pop’s. Once there there is no more valid moves, the winner of\n";
	message += "Riverdale’s Reversi Nite will be determined if they have no more\n";
	message += "valid moves and has more pieces than the opponent.\n";
	message += "\nIn case you haven’t noticed, I’m weird. I’m a weirdo. I don’t fit in, And I don’t want to fit in.\n";
	message += "-Jughead Jones (Serpent King)\n";
	message += "\nEnjoy helping Jughead and his Serpents fit in at Riverdale and beat Pop’s!!";
	windowP->MessageBox (message, CString ("Instructions"));
}

void Game::Click (CPoint point)
{
// This function will:
	if (whoseTurn != HUMAN)
		return;
	if (rulesRect.PtInRect(point))
		Instructions();
	else if (restartRect.PtInRect(point))
		Init();
	else if (gameRect.PtInRect(point))
	{
		int row = -1, col = -1;
		for (int r = 1; row == -1 && r <= ROWS; r++)
			for (int c = 1; col == -1 && c <= COLS; c++)
				if (board[r][c].where.PtInRect(point))
				{
					row = r;
					col = c;
				}
		if (ValidMove (whoseTurn, row, col))
			MakeMove (whoseTurn, row, col);
	}
}

void Game::Display () 
{
// This function will:
	CPaintDC dc (windowP);
	CRect rect;
	windowP->GetClientRect (&rect);
	if (rect != currentRect)
	{	
		SetUp (rect);
		currentRect = rect;
	}
	dc.SetBkMode(TRANSPARENT);			
	CDC memDC;
	int res = memDC.CreateCompatibleDC(&dc);
	memDC.SelectObject(&bgImage);
	dc.TransparentBlt (0, 0, rect.Width(), rect.Height(), &memDC, 0, 0, 1590, 795, SRCCOPY); 
	DeleteDC (memDC);
	CBrush * newBrush = new CBrush (RGB (255, 255, 255));
	CBrush * saveBrush = dc.SelectObject(newBrush);
	CPen * newPen = new CPen;
	newPen->CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
	CPen * savePen = dc.SelectObject(newPen);
	CFont font;
	CString fontName = "Arial";
	font.CreatePointFont(rect.Height() / 4, fontName, &dc);
	CFont* def_font = dc.SelectObject(&font);
	DisplayGameRect (&dc);
	DisplayScoreRect (&dc);
	DisplayButtonRect (&dc);
	dc.SelectObject(def_font);
	dc.SelectObject(saveBrush);
	delete newBrush;
	dc.SelectObject(savePen);
	delete newPen;
}

void Game::SetUp (CRect rect)
{
// This function will:

	int width = rect.Width();
	int height =  rect.Height();
	gameRect = CRect (4*width/7, height/4, 11*width/12, 8*height/9);
	scoreRect = CRect (width/12, 2*height/4, width/4, 8*height/9);
	height = scoreRect.Height();
	humanScoreRect = scoreRect;
	humanScoreRect.bottom -= height/2;
	computerScoreRect = scoreRect;
	computerScoreRect.top += height/2;
	buttonRect = CRect (width/12, height/2, width/4, 11*height/12);
	height = buttonRect.Height();
	restartRect = CRect (buttonRect.left, buttonRect.top, buttonRect.right, buttonRect.top + height/2);
	rulesRect = CRect (buttonRect.left, buttonRect.top+height/2, buttonRect.right, buttonRect.bottom);
	width = gameRect.Width() / ROWS;
	height = gameRect.Height() / COLS;	
	int tlx = gameRect.TopLeft().x + (gameRect.Width() - COLS*width) / 2;
	int tly = gameRect.TopLeft().y + (gameRect.Height() - ROWS*height) / 2;
	for (int r = 1; r <= ROWS; r++)
		for (int c = 1; c <= COLS; c++)
			board[r][c].where = CRect (tlx+(c-1)*width, tly+(r-1)*height, tlx+c*width, tly+r*height);
}

void Game::DisplayGameRect (CDC * deviceContextP)
{
// This function will:

	deviceContextP->Rectangle(gameRect);
	for (int r = 1; r <= ROWS; r++)
		for (int c = 1; c <= COLS; c++)
			board[r][c].Display (deviceContextP);
}

void Game::DisplayScoreRect (CDC * deviceContextP)
{ 
// This function will:

	deviceContextP->Rectangle(humanScoreRect);
	deviceContextP->Rectangle(computerScoreRect);

	int quarterH = scoreRect.Height() / 8;
	CRect text = humanScoreRect;
	text.bottom -= quarterH;
	char msgString [100] = "SouthSide Serpents";
	deviceContextP->DrawText(CString (msgString), text, DT_SINGLELINE | DT_VCENTER | DT_CENTER);
	text = humanScoreRect;
	text.top += quarterH;
	sprintf_s (msgString, "%d", score[HUMAN]);
	deviceContextP->DrawText(CString (msgString), text, DT_SINGLELINE | DT_VCENTER | DT_CENTER);

	text = computerScoreRect;
	text.bottom -= quarterH;
	strcpy_s (msgString, "Pop's");
	deviceContextP->DrawText(CString (msgString), text, DT_SINGLELINE | DT_VCENTER | DT_CENTER);
	text = computerScoreRect;
	text.top += quarterH;
	sprintf_s (msgString, "%d", score[COMPUTER]);
	deviceContextP->DrawText(CString (msgString), text, DT_SINGLELINE | DT_VCENTER | DT_CENTER);
}

void Game::DisplayButtonRect (CDC * deviceContextP)
{
	deviceContextP->Rectangle (rulesRect);
	deviceContextP->DrawText ("Rules", rulesRect, DT_SINGLELINE | DT_VCENTER | DT_CENTER);
	
	deviceContextP->Rectangle (restartRect);
	deviceContextP->DrawText ("Restart", restartRect, DT_SINGLELINE | DT_VCENTER | DT_CENTER);
}

bool Game::CheckBoard (playerType Who)
{
	
	for (int r = 1; r <= ROWS; r++)
	{
		for (int c = 1; c <= COLS; c++)
		{
			if (this->ValidMove(Who, r, c))
				return true;
		}
	}
	return false;
}

bool Game::ValidMove(playerType Who, int Row, int Col)
{ 
	playerType Other = Who == HUMAN ? COMPUTER : HUMAN;
	if (board[Row][Col].owner != NONE)
		return false;
	board[Row][Col].change = 0;
	bool whatisit = false; // Label if valid or unvalid

	// Set top,left, right, and bottom to false
	bool top = false;
	bool left = false;
	bool right = false;
	bool bottom = false;

	// Set top, left, right, bottom are true
	bool testtop = true;
	bool testleft = true;
	bool testright = true;
	bool testbottom = true;
	
	bool whatsleft = false; // Top Left portion
	bool testtl = true;
	bool bottomleft = false;
	bool testbl = true;

	bool whatsright = false; // Top right portion
	bool checktr = true;
	bool checkbr = true;
	bool bottomright = false;

	int addTop = 0;
	int addLeft = 0;
	int addRight = 0;
	int addBottom = 0;
	
	int addTopLeft = 0;
	int addTopRight = 0;

	int addBottomLeft = 0;
	int addBottomRight = 0;

	// Name the piece that is valid or unvalid 
	if ((board[Row - 1][Col - 1].owner != NONE) && (board[Row - 1][Col - 1].owner != Who)) 
	{
		whatsleft = true;
		addTopLeft++;
	}
	if ((board[Row - 1][Col].owner != NONE) && (board[Row - 1][Col].owner != Who)) 
	{
		top = true;
		addTop++;
	}
	if ((board[Row - 1][Col + 1].owner != NONE) && (board[Row - 1][Col + 1].owner != Who))
	{
		whatsright = true;
		addTopRight++;
	}
	if ((board[Row][Col - 1].owner != NONE) && (board[Row][Col - 1].owner != Who))
	{
		left = true;
		addLeft++;
	}
	if ((board[Row][Col + 1].owner != NONE) && (board[Row][Col + 1].owner != Who)) 
	{
		right = true;
		addRight++;
	}
	if ((board[Row + 1][Col - 1].owner != NONE) && (board[Row + 1][Col - 1].owner != Who)) 
	{
		bottomleft = true;
		addBottomLeft++;
	}
	if ((board[Row + 1][Col].owner != NONE) && (board[Row + 1][Col].owner != Who)) 
	{
		bottom = true;
		addBottom++;
	}
	if ((board[Row + 1][Col + 1].owner != NONE) && (board[Row + 1][Col + 1].owner != Who))
	{
		bottomright = true;
		addBottomRight++;
	}
	for (int i = 2; i < 10; i++)
	{
		if ((whatsleft) && (testtl))
		{
			if (board[Row - i][Col - i].owner == Who)
				testtl = false;
			else if (board[Row - i][Col - i].owner == NONE)
				whatsleft = false;
			else
				addTopLeft++;
		}
		if ((top) && (testtop))
		{
			if (board[Row - i][Col].owner == Who)
				testtop = false;
			else if (board[Row - i][Col].owner == NONE)
				top = false;
			else
				addTop++;
		}
		if ((whatsright) && (checktr))
		{
			if (board[Row - i][Col + i].owner == Who)
				checktr = false;
			else if (board[Row - i][Col + i].owner == NONE)
				whatsright = false;
			else
				addTopRight++;
		}
		if ((left) && (testleft))
		{
			if (board[Row][Col - i].owner == Who)
				testleft = false;
			else if (board[Row][Col - i].owner == NONE)
				left = false;
			else
				addLeft++;
		}
		if ((right) && (testright))
		{
			if (board[Row][Col + i].owner == Who)
				testright = false;
			else if (board[Row][Col + i].owner == NONE)
				right = false;
			else
				addRight++;
		}
		if ((bottomleft) && (testbl))
		{
			if (board[Row + i][Col - i].owner == Who)
				testbl = false;
			else if (board[Row + i][Col - i].owner == NONE)
				bottomleft = false;
			else
				addBottomLeft++;
		}
		if ((bottom) && (testbottom))
		{
			if (board[Row + i][Col].owner == Who)
				testbottom = false;
			else if (board[Row + i][Col].owner == NONE)
				bottom = false;
			else
				addBottom++;
		}
		if ((bottomright) && (checkbr))
		{
			if (board[Row + i][Col + i].owner == Who)
				checkbr = false;
			else if (board[Row + i][Col + i].owner == NONE)
				bottomright = false;
			else
				addBottomRight++;
		}
	}
	// Flip the game piece
	if (whatsleft) 
	{
		whatisit = true;
		board[Row][Col].fliptl = addTopLeft;
		board[Row][Col].change += addTopLeft;
	}
	if (whatsright)
	{
		whatisit = true;
		board[Row][Col].fliptr = addTopRight;
		board[Row][Col].change += addTopRight;
	}
	if (top) 
	{
		whatisit = true;
		board[Row][Col].flipt = addTop;
		board[Row][Col].change += addTop;
	}
	
	if (left) 
	{
		whatisit = true;
		board[Row][Col].flipl = addLeft;
		board[Row][Col].change += addLeft;
	}
	if (right)
	{
		whatisit = true;
		board[Row][Col].flipr = addRight;
		board[Row][Col].change += addRight;
	}
	if (bottom)
	{
		whatisit = true;
		board[Row][Col].flipb = addBottom;
		board[Row][Col].change += addBottom;
	}
	if (bottomleft) 
	{
		whatisit = true;
		board[Row][Col].flipbl = addBottomLeft;
		board[Row][Col].change += addBottomLeft;
	}
	
	if (bottomright) 
	{
		whatisit = true;
		board[Row][Col].changebottomr = addBottomRight;
		board[Row][Col].change += addBottomRight;
	}
	if (whatisit)
	{
		return true;
	}
	return false;
}

void Game::MakeMove(playerType Who, int Row, int Col)
{
	playerType Other = Who == HUMAN ? COMPUTER : HUMAN;
	// Create a RCPair object for each move - add to flips vector
	windowP->SetTimer(1, 50, NULL);
	flips.push_back(RCPair(Row, Col));
	for (int i = 1; i <= board[Row][Col].fliptl; i++)
	{
		flips.push_back(RCPair(Row - i, Col - i));
	}
	for (int i = 1; i <= board[Row][Col].flipt; i++)
	{
		flips.push_back(RCPair(Row - i, Col));
	}
	for (int i = 1; i <= board[Row][Col].fliptr; i++)
	{
		flips.push_back(RCPair(Row - i, Col + i));
	}
	for (int i = 1; i <= board[Row][Col].flipl; i++)
	{
		flips.push_back(RCPair(Row, Col - i));
	}
	for (int i = 1; i <= board[Row][Col].flipr; i++)
	{
		flips.push_back(RCPair(Row, Col + i));
	}
	for (int i = 1; i <= board[Row][Col].flipbl; i++)
	{
		flips.push_back(RCPair(Row + i, Col - i));
	}
	for (int i = 1; i <= board[Row][Col].flipb; i++)
	{
		flips.push_back(RCPair(Row + i, Col));
	}
	for (int i = 1; i <= board[Row][Col].flipbr; i++)
	{
		flips.push_back(RCPair(Row + i, Col + i));
	} 
	windowP->SetTimer(1, 50, NULL);
	score[Other]++;
}

int Game::FlipCells ()
{
	if (flips.size() > 0)
	{
		int r = flips[0].row;
		int c = flips[0].col;
		flips.erase(flips.begin());
		board[r][c].owner = whoseTurn;
		windowP->InvalidateRect (board[r][c].where, TRUE);
	}
	else
	{
		windowP->KillTimer(1);
		int r = moveRC.row, c = moveRC.col;
		windowP->InvalidateRect (board[r][c].where, TRUE);
		CalculateScores();
		windowP->InvalidateRect(scoreRect, TRUE);
		CheckBoard (HUMAN);
		CheckBoard (COMPUTER);
		if (Done())
		{
			whoseTurn = NONE; 
			Winner();
		}
		else if (whoseTurn == HUMAN)
			whoseTurn = COMPUTER;
		else if (whoseTurn == COMPUTER)
			whoseTurn = HUMAN;
		if (whoseTurn == COMPUTER)
		{
				int r = -1;
				int c = -1;
				ChooseMove (COMPUTER, r, c);
				MakeMove (COMPUTER, r, c);
		}
	}
	return 0;
}	

bool Game::ChooseMove (playerType Who, int & Row, int & Col)
{ 
	// AI determines desired location of move
	playerType Other = Who == HUMAN ? COMPUTER : HUMAN;
	int i = 0;
	int max = -10;
	int rows[32];
	int cols[32];
	for (int r = 1; r <= ROWS; r++) 
	{
		for (int c = 1; c <= COLS; c++)
		{
			if (ValidMove(Who, r, c))
			{
				if (((board[r][c].scoreCheck + board[r][c].change) > max))
				{
					max = board[r][c].scoreCheck + board[r][c].change;
				}
			}
		}
	}
	for (int r = 1; r <= ROWS; r++) 
	{
		for (int c = 1; c <= COLS; c++)
		{
			if (board[r][c].owner == NONE)
			{
				if (ValidMove(Who, r, c)) 
				{
					if ((board[r][c].scoreCheck + board[r][c].change) == max)
					{
						rows[i] = r;
						cols[i] = c;
						i++;
					}
				}
			}
		}
	}
	if (i == 0)
		return false;
	int play = (rand() % i);
	Row = rows[play];
	Col = cols[play];
	if (board[Row][Col].owner != NONE)
		return false;
	return true;
}

void Game::CalculateScores ()
{
	score[HUMAN] = 0;
	score[COMPUTER] = 0;
	for (int r = 1; r <= ROWS; r++)
		for (int c = 1; c <= COLS; c++)
			if (board[r][c].owner != NONE)
				score[board[r][c].owner]++;
}

bool Game::Done ()
{
	if (CheckBoard(HUMAN) & CheckBoard(COMPUTER))
		return false;
	return true;
}


playerType Game::Winner ()
{
	CString message = "The winner is....";
	playerType winner = NONE;
	if (score[HUMAN] > score[COMPUTER])
		winner = HUMAN;
	else if (score[COMPUTER] > score[HUMAN])
		winner = COMPUTER;
	else {}

	windowP->MessageBox(message, CString("Game Over"));
	Init();
	return winner; 
}
