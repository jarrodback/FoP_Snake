//---------------------------------------------------------------------------
//Program: Skeleton for Task 1c – group assignment
//Author: Pascale Vacher
//Last updated: 23 February 2018
//---------------------------------------------------------------------------

//Go to 'View > Task List' menu to open the 'Task List' pane listing the initial amendments needed to this program

//---------------------------------------------------------------------------
//----- include libraries
//---------------------------------------------------------------------------

//include standard libraries
#include <iostream>	
#include <iomanip> 
#include <conio.h> 
#include <cassert> 
#include <string>
#include <sstream>
#include <vector>
#include <fstream>
using namespace std;

//include our own libraries
#include "RandomUtils.h"    //for seed, random
#include "ConsoleUtils.h"	//for clrscr, gotoxy, etc.
#include "TimeUtils.h"		//for getSystemTime, timeToString, etc.

//---------------------------------------------------------------------------
//----- define constants
//---------------------------------------------------------------------------
//defining the size of the grid
const int  SIZEX(12);    	//horizontal dimension
const int  SIZEY(10);		//vertical dimension
//defining symbols used for display of the grid and content
const char SPOT('@');   	//spot
const char TAIL('o');
const char TUNNEL(' ');    	//tunnel
const char WALL('#');    	//border
const char MOUSE('*');		//mouse
const char PILL('+');
//defining the command letters to move the spot on the maze
const int  UP(72);			//up arrow
const int  DOWN(80); 		//down arrow
const int  RIGHT(77);		//right arrow
const int  LEFT(75);		//left arrow
//defining the other command letters
const char QUIT('Q');		//to end the game
const char MAXMICE = 10;

struct Item {
	int x, y;
	char symbol;
};

struct Variables {
	int mouseEaten = 0;
	bool dead = false;
	int keysPressed = 0;
	int pillKeysPressed;
	bool isPillPlaced = false;
	bool cheatMode = false;
	bool hasCheated = false;
	int bestScore;
	vector<Item> snake;
	string name;
	Item pill = { 0,0,PILL };
};

//---------------------------------------------------------------------------
//----- run game
//---------------------------------------------------------------------------

int main()
{
	Variables vars;

	//function declarations (prototypes)
	void initialiseGame(char g[][SIZEX], char m[][SIZEX], Item& spot, Item& mouse, Variables& vars);
	void renderGame(const char g[][SIZEX], const string& mess, Variables& vars);
	void updateGame(char g[][SIZEX], const char m[][SIZEX], Item& s, const int kc, string& mess, Item& mouse, Variables& vars);
	bool wantsToQuit(const int key);
	bool wantsToCheat(const int key);
	bool isArrowKey(const int k);
	int  getKeyPress();
	void endProgram(Variables& vars);
	void showMessage(const WORD backColour, const WORD textColour, int x, int y, const string& message);
	void toggleCheat(Variables& vars);
	int openFile(string& name);

	//local variable declarations 
	char grid[SIZEY][SIZEX];			//grid for display
	char maze[SIZEY][SIZEX];			//structure of the maze
	Item spot = { 0, 0, SPOT }; 		//spot's position and symbol
	Item mouse = { 0, 0, MOUSE };		//mouse's position and symbol.
	string message("LET'S START...");	//current message to player

	//action...
	seed();								//seed the random number generator
	SetConsoleTitle("FoP 2018-19 - Task 1c - Game Skeleton");

	//Get Name
	do {
		system("cls");
		showMessage(clBlack, clYellow, 10, 9, ("ENTER PLAYER NAME (MAX 20 CHARACTERS): "));
		cin >> vars.name;
	} while (vars.name.size() > 20);
	//Load Player's score
	vars.bestScore = openFile(vars.name);

	system("cls");
	initialiseGame(grid, maze, spot, mouse, vars);	//initialise grid (incl. walls and spot)
	int key;							//current key selected by player
	do {
		renderGame(grid, message, vars);			//display game info, modified grid and messages
		key = toupper(getKeyPress()); 	//read in  selected key: arrow or letter commands
		if (isArrowKey(key))
			updateGame(grid, maze, spot, key, message, mouse, vars);
		else
			if (wantsToCheat(key))
			{
				toggleCheat(vars);
			}
			else
				message = "INVALID KEY!";  //set 'Invalid key' message
	} while (!wantsToQuit(key) && vars.mouseEaten < MAXMICE && !vars.dead);		//while user does not want to quit
	renderGame(grid, message, vars);			//display game info, modified grid and messages
	endProgram(vars);						//display final message
	return 0;
}


//---------------------------------------------------------------------------
//----- initialise game state
//---------------------------------------------------------------------------

void initialiseGame(char grid[][SIZEX], char maze[][SIZEX], Item& spot, Item& mouse, Variables& vars)
{ //initialise grid and place spot in middle
	void setInitialMazeStructure(char maze[][SIZEX]);
	void setItemInitialCoordinates(Item&, const char maze[][SIZEX]);
	void updateGrid(char g[][SIZEX], const char m[][SIZEX], const Item& i, const Item& mouse, Variables& vars);

	setInitialMazeStructure(maze);		//initialise maze
	setItemInitialCoordinates(spot, maze);
	vars.snake.push_back(spot);
	for (int x = 0; x < 3; x++)
	{
		Item item = spot;
		item.symbol = TAIL;
		vars.snake.push_back(item);
	}
	setItemInitialCoordinates(mouse, maze);
	while (mouse.x == spot.x && mouse.y == spot.y)
	{
		setItemInitialCoordinates(mouse, maze);
	}

	updateGrid(grid, maze, spot, mouse, vars);//prepare grid
}

void setItemInitialCoordinates(Item& item, const char maze[][SIZEX])
{ //set spot coordinates inside the grid at random at beginning of game
	do
	{
		item.y = random(SIZEY - 2);      //vertical coordinate in range [1..(SIZEY - 2)]
		item.x = random(SIZEX - 2);      //horizontal coordinate in range [1..(SIZEX - 2)]
	} while (maze[item.y][item.x] != TUNNEL);
}


void setInitialMazeStructure(char maze[][SIZEX])
{ //set the position of the walls in the maze
//TODO: Amend initial maze configuration (change size changed and inner walls)
  //initialise maze configuration
	char initialMaze[SIZEY][SIZEX] 	//local array to store the maze structure
		= { { '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#' },
		   { '#', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '#' },
		   { '#', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '#' },
		   { '#', ' ', ' ', '#', ' ', ' ', ' ', '#', '#', ' ', ' ', '#' },
		   { '#', ' ', ' ', '#', ' ', ' ', ' ', ' ', '#', ' ', ' ', '#' },
		   { '#', ' ', ' ', '#', '#', ' ', ' ', ' ', ' ', ' ', ' ', '#' },
		   { '#', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '#' },
		   { '#', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '#' },
		   { '#', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '#' },
		   { '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#' } };
	//with '#' for wall, ' ' for tunnel, etc. 
	//copy into maze structure with appropriate symbols
	for (int row(0); row < SIZEY; ++row)
		for (int col(0); col < SIZEX; ++col)
			switch (initialMaze[row][col])
			{
				//not a direct copy, in case the symbols used are changed
			case '#': maze[row][col] = WALL; break;
			case ' ': maze[row][col] = TUNNEL; break;
			}
}

//---------------------------------------------------------------------------
//----- Update Game
//---------------------------------------------------------------------------

void updateGame(char grid[][SIZEX], const char maze[][SIZEX], Item& spot, const int keyCode, string& mess, Item& mouse, Variables& vars)
{ //update game
	void updateGameData(const char g[][SIZEX], Item& spot, const int keyCode, string& mess, Item& mouse, Variables& vars);
	void updateGrid(char g[][SIZEX], const char maze[][SIZEX], const Item& s, const Item& mouse, Variables& vars);
	vars.keysPressed++;

	updateGameData(grid, vars.snake[0], keyCode, mess, mouse, vars);		//move spot in required direction
	updateGrid(grid, maze, vars.snake[0], mouse, vars);			//update grid information
}
void updateGameData(const char g[][SIZEX], Item& spot, const int key, string& mess, Item& mouse, Variables& vars)
{ //move spot in required direction
	bool isArrowKey(const int k);
	void setKeyDirection(int k, int& dx, int& dy);
	bool wantsToQuit(const int key);
	assert(isArrowKey(key));

	//reset message to blank
	mess = "";

	//calculate direction of movement for given key
	int dx(0), dy(0);
	setKeyDirection(key, dx, dy);

	//check new target position in grid and update game data (incl. spot coordinates) if move is possible
	switch (g[spot.y + dy][spot.x + dx])
	{			//...depending on what's on the target position in grid...
	case TUNNEL:		//can move
		for (int t = vars.snake.size() - 1; t > 0; t--)
		{
			vars.snake[t].x = vars.snake[t - 1].x;
			vars.snake[t].y = vars.snake[t - 1].y;
		}
		vars.snake[0].y += dy;	//go in that Y direction
		vars.snake[0].x += dx;	//go in that X direction
		break;
	case TAIL:
	case WALL:
		vars.dead = true;
		break;
	case MOUSE:			//Hit a mouse and eat it
		setItemInitialCoordinates(mouse, g);
		//Update the movement
		for (int t = vars.snake.size() - 1; t > 0; t--)
		{
			vars.snake[t].x = vars.snake[t - 1].x;
			vars.snake[t].y = vars.snake[t - 1].y;
		}
		//Add the 2 new nails
		if (!vars.cheatMode)
		{
			for (int x = 0; x < 2; x++)
			{
				Item item = spot;
				item.symbol = TAIL;
				item.x = vars.snake.back().x;
				item.y = vars.snake.back().y;
				vars.snake.push_back(item);
			}
		}
		//Move the snake head
		vars.snake[0].y += dy;	//go in that Y direction
		vars.snake[0].x += dx;	//go in that X direction;
		vars.mouseEaten += 1;
		if (vars.mouseEaten % 2 == 0 && vars.mouseEaten != 0 && vars.isPillPlaced == false)
		{
			setItemInitialCoordinates(vars.pill, g);
			vars.isPillPlaced = true;
			vars.pillKeysPressed = 10;
		}
		break;
	case PILL:
		while (vars.snake.size() > 4)
		{
			vars.snake.pop_back();
		}
		vars.pill.x = -1;
		vars.pill.y = -1;
		vars.isPillPlaced = false;
		for (int t = vars.snake.size() - 1; t > 0; t--)
		{
			vars.snake[t].x = vars.snake[t - 1].x;
			vars.snake[t].y = vars.snake[t - 1].y;
		}
		vars.snake[0].y += dy;	//go in that Y direction
		vars.snake[0].x += dx;	//go in that X direction
		break;
	}
	if (vars.isPillPlaced == true)
	{
		vars.pillKeysPressed--;
		if (vars.pillKeysPressed < 0)
		{
			vars.pill.x = -1;
			vars.pill.y = -1;
			vars.isPillPlaced = false;
		}
	}
}
void updateGrid(char grid[][SIZEX], const char maze[][SIZEX], const Item& spot, const Item& mouse, Variables& vars)
{ //update grid configuration after each move
	void placeMaze(char g[][SIZEX], const char b[][SIZEX]);
	void placeItem(char g[][SIZEX], const Item&);

	placeMaze(grid, maze);	//reset the empty maze configuration into grid
	for each (Item item in (vars.snake))
	{
		placeItem(grid, item);
	}
	placeItem(grid, spot);	//set spot in grid
	placeItem(grid, mouse);	//set mouse in grid
	if (vars.mouseEaten % 2 == 0 && vars.mouseEaten != 0 && vars.isPillPlaced == false || vars.isPillPlaced == true)
	{
		placeItem(grid, vars.pill);
	}
}

void placeMaze(char grid[][SIZEX], const char maze[][SIZEX])
{ //reset the empty/fixed maze configuration into grid
	for (int row(0); row < SIZEY; ++row)
		for (int col(0); col < SIZEX; ++col)
			grid[row][col] = maze[row][col];
}

void placeItem(char g[][SIZEX], const Item& item)
{ //place item at its new position in grid
	g[item.y][item.x] = item.symbol;
}
//---------------------------------------------------------------------------
//----- process key
//---------------------------------------------------------------------------
void setKeyDirection(const int key, int& dx, int& dy)
{ //calculate direction indicated by key
	bool isArrowKey(const int k);
	assert(isArrowKey(key));
	switch (key)	//...depending on the selected key...
	{
	case LEFT:  	//when LEFT arrow pressed...
		dx = -1;	//decrease the X coordinate
		dy = 0;
		break;
	case RIGHT: 	//when RIGHT arrow pressed...
		dx = +1;	//increase the X coordinate
		dy = 0;
		break;
	case UP:  	//when UP arrow pressed...
		dx = 0;	//decrease the Y coordinate
		dy = -1;
		break;
	case DOWN: 	//when DOWN arrow pressed...
		dx = 0;	//increase the Y coordinate
		dy = +1;
		break;
	}
}

int getKeyPress()
{ //get key or command selected by user
  //KEEP THIS FUNCTION AS GIVEN
	int keyPressed;
	keyPressed = _getch();			//read in the selected arrow key or command letter
	if (keyPressed == ' ')
		return keyPressed;
	while (keyPressed == 224) 		//ignore symbol following cursor key
		keyPressed = _getch();

	return keyPressed;
}

bool isArrowKey(const int key)
{	//check if the key pressed is an arrow key (also accept 'K', 'M', 'H' and 'P')
	return (key == LEFT) || (key == RIGHT) || (key == UP) || (key == DOWN) || (key == 'K') || (key == 'M') || (key == 'H') || (key == 'P'); //Add KMHP for movement >:(
}
bool wantsToQuit(const int key)
{	//check if the user wants to quit (when key is 'Q' or 'q')
	return (key == QUIT) || (key == tolower(QUIT));
}
bool wantsToCheat(const int key)
{
	return (toupper(key) == ('C'));
}

//---------------------------------------------------------------------------
//----- display info on screen
//---------------------------------------------------------------------------

string tostring(int x)
{	//convert an integer to a string
	std::ostringstream os;
	os << x;
	return os.str();
}
string tostring(char x)
{	//convert a char to a string
	std::ostringstream os;
	os << x;
	return os.str();
}
void showMessage(const WORD backColour, const WORD textColour, int x, int y, const string& message)
{	//display a string using specified colour at a given position 
	gotoxy(x, y);
	selectBackColour(backColour);
	selectTextColour(textColour);
	cout << message + string(40 - message.length(), ' ');
}
void renderGame(const char g[][SIZEX], const string& mess, Variables& vars)
{ //display game title, messages, maze, spot and other items on screen
	string tostring(char x);
	string tostring(int x);
	void showMessage(const WORD backColour, const WORD textColour, int x, int y, const string& message);
	void paintGrid(const char g[][SIZEX]);
	//display game title
	showMessage(clBlack, clGreen, 0, 0, "___GAME___");
	showMessage(clWhite, clBlack, 40, 0, "Fop Task 1C " + getDate() + " " + getTime());
	showMessage(clWhite, clDarkMagenta, 40, 1, "b8014500 Dylan Lake GROUP 1-9 ");
	showMessage(clWhite, clDarkMagenta, 40, 2, "b8043407 Jarrod Back GROUP 1-9");
	//display menu options available
	showMessage(clRed, clYellow, 40, 3, "TO MOVE  - USE KEYBOARD ARROWS ");
	if (vars.cheatMode == false)
		showMessage(clRed, clYellow, 40, 4, "TO CHEAT - ENTER 'C'           ");
	else
		showMessage(clRed, clYellow, 40, 4, "CHEAT MODE: ON |TO DEACTIVATE - ENTER C");
	showMessage(clRed, clYellow, 40, 5, "TO QUIT  - ENTER 'Q'           ");

	showMessage(clBlack, clWhite, 40, 7, ("NAME: " + vars.name));
	showMessage(clBlack, clWhite, 40, 8, ("BEST SCORE: " + tostring(vars.bestScore)));
	showMessage(clBlack, clWhite, 40, 9, ("MICE EATEN: " + tostring(vars.mouseEaten)) + "/10");
	showMessage(clBlack, clWhite, 40, 10, ("SCORE: " + tostring(vars.keysPressed)));

	//print auxiliary messages if any
	showMessage(clBlack, clWhite, 40, 15, mess);	//display current message
	//display grid contents
	paintGrid(g);
}

void paintGrid(const char g[][SIZEX])
{ //display grid content on screen
	selectBackColour(clBlack);
	selectTextColour(clWhite);
	gotoxy(0, 2);
	for (int row(0); row < SIZEY; ++row)
	{
		for (int col(0); col < SIZEX; ++col)
		{
			selectTextColour(clWhite);
			if (g[row][col] == SPOT)
			{
				selectTextColour(clGreen);
			}
			if (g[row][col] == MOUSE)
			{
				selectTextColour(clYellow);
			}
			if (g[row][col] == TAIL)
			{
				selectTextColour(clGreen);
			}
			if (g[row][col] == PILL)
			{
				selectTextColour(clMagenta);
			}
			cout << g[row][col];	//output cell content
		}
		cout << endl;
	}
}

void endProgram(Variables& vars)
{
	void showMessage(const WORD backColour, const WORD textColour, int x, int y, const string& message);
	showMessage(clRed, clYellow, 40, 8, "QUITTING GAME");
	if (vars.mouseEaten >= MAXMICE)
	{
		showMessage(clRed, clYellow, 40, 7, "YOU WON");
		//Save score if not cheated
		if (!vars.hasCheated)
		{
			if (vars.keysPressed > vars.bestScore || vars.bestScore == 500)
			{
				ofstream scoreFile;
				scoreFile.open(vars.name + ".txt");
				scoreFile << vars.keysPressed;
				scoreFile.close();
			}
		}
	}
	if (vars.dead)
		showMessage(clRed, clYellow, 40, 7, "YOU DIED");
	system("pause");	//hold output screen until a keyboard key is hit
}

int openFile(string& name)
{
	string strBestScore = "";
	int bestScore;
	ifstream scoreFile;
	scoreFile.open(name + ".txt", ios::in);
	if (scoreFile.is_open())
	{
		getline(scoreFile, strBestScore);
		//scoreFile >> strBestScore;  //Either work
		scoreFile.close();
		bestScore = stoi(strBestScore);
	}
	else
		bestScore = 500;
	return bestScore;
}

void toggleCheat(Variables& vars)
{
	if (vars.cheatMode == false)
	{
		vars.cheatMode = true;
		vars.hasCheated = true;
		for (int x = 0; x < 3; x++)
		{
			cout << '\a';
			Sleep(1000);
		}
		while (vars.snake.size() > 4)
		{
			vars.snake.pop_back();
		}
	}
	else
		vars.cheatMode = false;
}