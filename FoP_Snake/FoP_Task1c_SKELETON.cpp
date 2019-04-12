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
const char MONGOOSE('M');
//defining the command letters to move the spot on the maze
const int  UP(72);			//up arrow
const int  DOWN(80); 		//down arrow
const int  RIGHT(77);		//right arrow
const int  LEFT(75);		//left arrow
const int  PLAYER2UP(87);			//up arrow
const int  PLAYER2DOWN(83); 		//down arrow
const int  PLAYER2RIGHT(68);		//right arrow
const int  PLAYER2LEFT(65);		//left arrow
//defining the other command letters
const char QUIT('Q');		//to end the game
const char MAXMICE = 10;

struct Item {
	int x, y;
	char symbol;
};
struct Player {
	int id;
	string name;
	vector<Item> snake;
	int bestScore;
	int mouseEaten = 0;
	int key;
	int invincibleKeysPressed = 0;
	int previousDirection = -1;
	bool invincible = false;
	bool dead = false;

	void MoveSnake(const int dx, const int dy)
	{
		for (int t = snake.size() - 1; t > 0; t--)
		{
			snake[t].x = snake[t - 1].x;
			snake[t].y = snake[t - 1].y;
		}
		snake[0].y += dy;	//go in that Y direction
		snake[0].x += dx;	//go in that X direction
	}
};
struct GameData {
	vector<Player> players;
	Item pill = { 0,0,PILL };
	Item mongoose = { 0,0, MONGOOSE };
	Item mongoosePrevious = { 0,0, MONGOOSE };
	Item mouse = { 0, 0, MOUSE };		//mouse's position and symbol.

	int timeLeft = 200;
	int keysPressed = 0;
	int pillKeysPressed = 0;
	int sleepTime = 300;
	bool isPillPlaced = false;
	bool isMongoosePlaced = false;
	bool multiplayer;
	bool cheatMode = false;
	bool hasCheated = false;

	int calculateMouseEaten()
	{
		int mouseEaten = 0;
		for (Player& player : players)
		{
			mouseEaten += player.mouseEaten;
		}
		return mouseEaten;
	}
	bool checkIfAllPlayersAreDead()
	{
		bool allDead = true;
		for (Player& player : players)
		{
			if (player.dead == false)
			{
				allDead = false;
			}
		}
		return allDead;
	}
	bool checkCollision(Item& item1, Item& item2)
	{
		if (item1.x == item2.x && item1.y == item2.y)
			return true;
		return false;
	}
};
//---------------------------------------------------------------------------
//----- run game
//---------------------------------------------------------------------------

int main()
{
	//function declarations (prototypes)
	void initialiseGame(char g[][SIZEX], char m[][SIZEX], GameData& gameData);
	void renderGame(const char g[][SIZEX], const string& mess, GameData& gameData);
	void updateGame(char g[][SIZEX], const char m[][SIZEX], string& message, GameData& gameData);
	bool wantsToQuit(const int key);
	bool wantsToCheat(const int key);
	bool wantsToSlowDown(const int key);
	bool isArrowKey(const int k, const bool multiplayer);
	int  getKeyPress(int& keypress);
	void endProgram(GameData& gameData);
	void showMessage(const WORD backColour, const WORD textColour, int x, int y, const string& message);
	void toggleCheat(GameData& gameData);
	int getBestScore(string& name);
	string tostring(int x);

	GameData gameData;
	Player player1;
	gameData.players.push_back(player1);
	gameData.players[0].id = gameData.players.size();
	//local variable declarations 
	char grid[SIZEY][SIZEX];			//grid for display
	char maze[SIZEY][SIZEX];			//structure of the maze
	string message("LET'S START...");	//current message to player

	//action...
	seed();								//seed the random number generator
	SetConsoleTitle("FoP 2018-19 - Task 1c - Game Skeleton");

	//Ask if Multiplayer
	char type;
	do {
		system("cls");
		showMessage(clBlack, clYellow, 10, 9, ("DO YOU WANT TO PLAY MULTIPLAYER (Y/N): "));
		cin >> type;
	} while (toupper(type) != 'N' && toupper(type) != 'Y');
	if (toupper(type) == 'N')
		gameData.multiplayer = false;
	else
	{
		gameData.multiplayer = true;
		Player player2;
		gameData.players.push_back(player2);
		gameData.players[1].id = gameData.players.size();
	}
	//Get Player 1 name
	for (Player& player : gameData.players)
	{
		do {
			system("cls");
			showMessage(clBlack, clYellow, 10, 9, ("ENTER PLAYER" + tostring(player.id) + " NAME (MAX 20 CHARACTERS): "));
			cin >> player.name;
		} while (player.name.size() > 20);
	}
	//Load Player's score
	for (Player& player : gameData.players)
	{
		player.bestScore = getBestScore(player.name);
	}

	system("cls");
	initialiseGame(grid, maze, gameData);	//initialise grid (incl. walls and spot)
	int initTime = time(NULL);
	do {
		int currentTime = time(NULL);
		gameData.timeLeft = 300 - (currentTime - initTime);
		renderGame(grid, message, gameData);			//display game info, modified grid and messages
		Sleep(gameData.sleepTime);
		for (Player& player : gameData.players)
		{
			if (_kbhit() || player.previousDirection == -1)
			{
				player.key = toupper(getKeyPress(player.previousDirection)); 	//read in  selected key: arrow or letter commands
			}
			else
			{
				player.key = player.previousDirection;
			}
		}
		for (Player& player : gameData.players)
		{
			if (isArrowKey(player.key, gameData.multiplayer))
				updateGame(grid, maze, message, gameData);
			else
				if (wantsToCheat(player.key))
				{
					toggleCheat(gameData);
				}
				else if (wantsToSlowDown(player.key))
				{
					gameData.sleepTime = 300;
				}
				else
					message = "INVALID KEY!";  //set 'Invalid key' message
		}

	} while (!wantsToQuit(gameData.players[0].key) && gameData.players[0].mouseEaten < MAXMICE && (!gameData.checkIfAllPlayersAreDead()) && gameData.timeLeft > 0);		//while user does not want to quit
	renderGame(grid, message, gameData);			//display game info, modified grid and messages
	endProgram(gameData);						//display final message

	return 0;
}


//---------------------------------------------------------------------------
//----- initialise game state
//---------------------------------------------------------------------------

void initialiseGame(char grid[][SIZEX], char maze[][SIZEX], GameData& gameData)
{ //initialise grid and place spot in middle
	void setInitialMazeStructure(char maze[][SIZEX]);
	void setItemInitialCoordinates(Item&, const char maze[][SIZEX]);
	void updateGrid(char g[][SIZEX], const char m[][SIZEX], GameData& gameData);

	setInitialMazeStructure(maze);		//initialise maze

	for (int x = 0; x < gameData.players.size(); x++)
	{
		gameData.players[x].snake.push_back(Item{ 0,0,SPOT });
		setItemInitialCoordinates(gameData.players[x].snake[0], maze);
		for (int y = 0; y < 3; y++)
		{
			Item item = gameData.players[x].snake[0];
			item.symbol = TAIL;
			gameData.players[x].snake.push_back(item);
		}
	}
	setItemInitialCoordinates(gameData.mouse, maze);
	bool checkAgain = true;
	while (checkAgain)
	{
		for (Player& player : gameData.players)
		{
			if (gameData.mouse.x == player.snake[0].x && gameData.mouse.y == player.snake[0].y)
			{
				setItemInitialCoordinates(gameData.mouse, maze);
				checkAgain = true;
			}
			else
				checkAgain = false;
		}
	}
	updateGrid(grid, maze, gameData);//prepare grid
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

void updateGame(char grid[][SIZEX], const char maze[][SIZEX], string& message, GameData& gameData)
{ //update game
	void updateGameData(const char g[][SIZEX], string& message, GameData& gameData);
	void updateGrid(char g[][SIZEX], const char m[][SIZEX], GameData& gameData);

	updateGameData(grid, message, gameData);		//move spot in required direction
	updateGrid(grid, maze, gameData);			//update grid information
}
void updateGameData(const char g[][SIZEX], string& mess, GameData& gameData)
{ //move spot in required direction
	bool isArrowKey(const int k, const bool multiplayer);
	void setKeyDirection(int k, int& dx, int& dy, int& id, const bool multiplayer);
	bool wantsToQuit(const int key);
	for (Player& player : gameData.players)
		assert(isArrowKey(player.key, gameData.multiplayer));

	//reset message to blank
	mess = "";
	gameData.keysPressed++;
	//move mongoose
	if (gameData.isMongoosePlaced)
	{
		int my, mx;
		do {
			my = (rand() % 3) - 1;
			mx = (rand() % 3) - 1;
		} while (g[gameData.mongoose.y + my][gameData.mongoose.x + mx] != TUNNEL && g[gameData.mongoose.y + my][gameData.mongoose.x + mx] != SPOT);

		gameData.mongoose.x += mx;
		gameData.mongoose.y += my;
		gameData.mongoosePrevious = gameData.mongoose;

		for (Player& player : gameData.players)
			if (gameData.checkCollision(gameData.mongoose, player.snake[0]))
				player.dead = true;
	}
	//check new target position in grid and update game data (incl. spot coordinates) if move is possible
	for (Player& player : gameData.players)
	{
		//calculate direction of movement for given key
		int dx(0), dy(0);
		setKeyDirection(player.key, dx, dy, player.id, gameData.multiplayer);

		switch (g[player.snake[0].y + dy][player.snake[0].x + dx])
		{			//...depending on what's on the target position in grid...

		case TUNNEL:		//can move
			player.MoveSnake(dx, dy);
			break;

		case MONGOOSE:
		case TAIL:
			if (!player.invincible)
				player.dead = true;
			break;

		case WALL:
			if (player.invincible)
			{
				int x = player.snake[0].x + dx;
				int y = player.snake[0].y + dy;
				if (x == 0)
				{
					player.snake[0].x = SIZEX - 1;
				}
				if (x == SIZEX - 1)
				{
					player.snake[0].x = 1;
				}
				if (y == 0)
				{
					player.snake[0].y = SIZEY - 1;
				}
				if (y == SIZEY - 1)
				{
					player.snake[0].y = 0;
				}
			}
			else
				player.dead = true;
			break;

		case MOUSE:			//Hit a mouse and eat it
		{
			setItemInitialCoordinates(gameData.mouse, g);
			//Update the movement
			player.MoveSnake(dx, dy);
			//Add the 2 new nails
			if (!gameData.cheatMode)
			{
				for (int x = 0; x < 2; x++)
				{
					Item item = player.snake[0];
					item.symbol = TAIL;
					item.x = player.snake.back().x;
					item.y = player.snake.back().y;
					player.snake.push_back(item);
				}
			}
			player.mouseEaten += 1;

			if (gameData.calculateMouseEaten() % 2 == 0 && gameData.calculateMouseEaten() != 0 && gameData.isPillPlaced == false)
			{
				setItemInitialCoordinates(gameData.pill, g);
				gameData.isPillPlaced = true;
				gameData.pillKeysPressed = 10;
			}
			if (gameData.calculateMouseEaten() == 3 && gameData.isMongoosePlaced == false)
			{
				setItemInitialCoordinates(gameData.mongoose, g);
				gameData.isMongoosePlaced = true;
			}
			gameData.sleepTime -= 20;
		}
		break;

		case PILL:
			while (player.snake.size() > 4)
			{
				player.snake.pop_back();
			}
			gameData.pill.x = -1;
			gameData.pill.y = -1;
			gameData.isPillPlaced = false;
			player.MoveSnake(dx, dy);
			player.invincible = true;
			player.invincibleKeysPressed = 20;
			break;

		case SPOT:
			Player currentPlayer = player;
			for (Player& player : gameData.players)
				if (gameData.checkCollision(currentPlayer.snake[0], player.snake[0]) && currentPlayer.id != player.id)
					player.dead == true;
			break;
		}
		if (player.invincible)
		{
			player.invincibleKeysPressed--;
			if (player.invincibleKeysPressed < 0)
				player.invincible = false;
		}
	}
	if (gameData.isPillPlaced)
	{
		gameData.pillKeysPressed--;
		if (gameData.pillKeysPressed < 0)
		{
			gameData.pill.x = -1;
			gameData.pill.y = -1;
			gameData.isPillPlaced = false;
		}
	}
}
void updateGrid(char grid[][SIZEX], const char maze[][SIZEX], GameData& gameData)
{ //update grid configuration after each move
	void placeMaze(char g[][SIZEX], const char b[][SIZEX]);
	void placeItem(char g[][SIZEX], const Item&);
	void resetItem(char g[][SIZEX], const Item& item);

	placeMaze(grid, maze);	//reset the empty maze configuration into grid
	for (int x = 0; x < gameData.players.size(); x++)
	{
		for (int y = gameData.players[x].snake.size(); y > 0; y--)
		{
			placeItem(grid, gameData.players[x].snake[y - 1]);
		}
	}
	placeItem(grid, gameData.mouse);	//set mouse in grid

	if (gameData.calculateMouseEaten() == 0 && gameData.calculateMouseEaten() != 0 && gameData.isPillPlaced == false || gameData.isPillPlaced == true)
	{
		placeItem(grid, gameData.pill);
	}
	if (gameData.calculateMouseEaten() == 3 && gameData.isMongoosePlaced == false || gameData.isMongoosePlaced == true)
	{
		resetItem(grid, gameData.mongoosePrevious);
		placeItem(grid, gameData.mongoose);
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
void resetItem(char g[][SIZEX], const Item& item)
{
	g[item.y][item.x] = TUNNEL;
}
//---------------------------------------------------------------------------
//----- process key
//---------------------------------------------------------------------------
void setKeyDirection(const int key, int& dx, int& dy, int& id, const bool multiplayer)
{ //calculate direction indicated by key
	bool isArrowKey(const int k, const bool multiplayer);
	assert(isArrowKey(key, multiplayer));

	if (id == 1)
	{
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
	if (id == 2)
	{
		switch (key)	//...depending on the selected key...
		{
		case PLAYER2LEFT:
			dx = -1;	//decrease the X coordinate
			dy = 0;
			break;
		case PLAYER2RIGHT:
			dx = +1;	//increase the X coordinate
			dy = 0;
			break;
		case PLAYER2UP:
			dx = 0;	//decrease the Y coordinate
			dy = -1;
			break;
		case PLAYER2DOWN:
			dx = 0;	//increase the Y coordinate
			dy = +1;
			break;
		}
	}
}
int getKeyPress(int& previousKey)
{ //get key or command selected by user
  //KEEP THIS FUNCTION AS GIVEN
	int keyPressed;
	keyPressed = _getch();			//read in the selected arrow key or command letter
	while (keyPressed == 224) 		//ignore symbol following cursor key
		keyPressed = _getch();
	if (keyPressed == DOWN || keyPressed == UP || keyPressed == LEFT || keyPressed == RIGHT || keyPressed == PLAYER2DOWN || keyPressed == PLAYER2UP || keyPressed == PLAYER2LEFT || keyPressed == PLAYER2RIGHT)
		previousKey = keyPressed;
	return keyPressed;
}

bool isArrowKey(const int key, const bool multiplayer)
{	//check if the key pressed is an arrow key (also accept 'K', 'M', 'H' and 'P')
	return (key == LEFT) || (key == RIGHT) || (key == UP) || (key == DOWN) || ((key == PLAYER2LEFT) && multiplayer) || ((key == PLAYER2RIGHT) && multiplayer) || ((key == PLAYER2UP) && multiplayer) || ((key == PLAYER2DOWN) && multiplayer); //Add KMHP for movement >:(
}
bool wantsToQuit(const int key)
{	//check if the user wants to quit (when key is 'Q' or 'q')
	return (key == QUIT) || (key == tolower(QUIT));
}
bool wantsToCheat(const int key)
{
	return (toupper(key) == ('C'));
}
bool wantsToSlowDown(const int key)
{
	return (toupper(key) == ('Z'));
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
void renderGame(const char g[][SIZEX], const string& mess, GameData& gameData)
{ //display game title, messages, maze, spot and other items on screen
	string tostring(char x);
	string tostring(int x);
	void showMessage(const WORD backColour, const WORD textColour, int x, int y, const string& message);
	void paintGrid(const char g[][SIZEX], const GameData& gameData);
	//display game title
	showMessage(clBlack, clGreen, 0, 0, "___GAME___");
	showMessage(clWhite, clBlack, 40, 0, "Fop Task 1C " + getDate() + " " + getTime());
	showMessage(clWhite, clDarkMagenta, 40, 1, "b8014500 Dylan Lake GROUP 1-9 ");
	showMessage(clWhite, clDarkMagenta, 40, 2, "b8043407 Jarrod Back GROUP 1-9");
	//display menu options available
	showMessage(clRed, clYellow, 40, 3, "TO MOVE  - USE KEYBOARD ARROWS ");

	if (gameData.cheatMode == false)
		showMessage(clRed, clYellow, 40, 4, "TO CHEAT - ENTER 'C'           ");
	else
		showMessage(clRed, clYellow, 40, 4, "CHEAT MODE: ON |TO DEACTIVATE - ENTER C");

	showMessage(clRed, clYellow, 40, 5, "TO QUIT  - ENTER 'Q'           ");
	showMessage(clBlack, clWhite, 40, 6, ("TIME LEFT: " + tostring(gameData.timeLeft)));
	showMessage(clBlack, clWhite, 40, 7, ("NAME: " + gameData.players[0].name));
	showMessage(clBlack, clWhite, 40, 8, ("BEST SCORE: " + tostring(gameData.players[0].bestScore)));
	showMessage(clBlack, clWhite, 40, 9, ("MICE EATEN: " + tostring(gameData.players[0].mouseEaten)) + "/10");
	showMessage(clBlack, clWhite, 40, 10, ("SCORE: " + tostring(gameData.keysPressed)));
	if (gameData.players[0].invincible)
		showMessage(clRed, clYellow, 40, 14, ("SNAKE 1 IS INVINCIBLE"));
	else
		showMessage(clBlack, clWhite, 40, 14, "");
	if (gameData.multiplayer && gameData.players[1].invincible)
		showMessage(clRed, clYellow, 40, 16, ("SNAKE 2 IS INVINCIBLE"));
	else
		showMessage(clBlack, clWhite, 40, 16, "");
	if (gameData.multiplayer)
	{
		showMessage(clBlack, clWhite, 40, 7, "");
		showMessage(clBlack, clWhite, 40, 8, "");
		showMessage(clBlack, clWhite, 40, 9, "");

		showMessage(clRed, clYellow, 80, 0, ("[MULTIPLAYER OPTIONS]"));
		showMessage(clBlack, clGreen, 80, 1, ("[PLAYER1]"));
		showMessage(clBlack, clWhite, 80, 2, ("Name: " + gameData.players[0].name));
		showMessage(clBlack, clWhite, 80, 3, ("Movement: Arrow Keys"));
		showMessage(clBlack, clWhite, 80, 4, ("Best Score: " + tostring(gameData.players[0].bestScore)));
		showMessage(clBlack, clWhite, 80, 5, ("Mice Eaten: " + tostring(gameData.players[0].mouseEaten)));

		showMessage(clBlack, clBlue, 80, 6, ("[PLAYER2]"));
		showMessage(clBlack, clWhite, 80, 7, ("Name: " + gameData.players[1].name));
		showMessage(clBlack, clWhite, 80, 8, ("Movement: WASD"));
		showMessage(clBlack, clWhite, 80, 9, ("Best Score: " + tostring(gameData.players[1].bestScore)));
		showMessage(clBlack, clWhite, 80, 10, ("Mice Eaten: " + tostring(gameData.players[1].mouseEaten)));
	}


	//print auxiliary messages if any
	showMessage(clBlack, clWhite, 40, 15, mess);	//display current message
	//display grid contents
	paintGrid(g, gameData);
}

void paintGrid(const char g[][SIZEX], const GameData& gameData)
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
				if (row == gameData.players[0].snake[0].y && col == gameData.players[0].snake[0].x)
				{

					if (gameData.players[0].invincible)
						selectTextColour(clRed);
					else
						selectTextColour(clGreen);
				}
				if (gameData.multiplayer && row == gameData.players[1].snake[0].y && col == gameData.players[1].snake[0].x)
				{
					if (gameData.players[0].invincible)
						selectTextColour(clRed);
					else
						selectTextColour(clBlue);
				}
			}

			if (g[row][col] == MOUSE)
			{
				selectTextColour(clYellow);
			}
			if (g[row][col] == TAIL)
			{
				for (int x = 1; x < gameData.players[0].snake.size(); x++)
				{
					if (row == gameData.players[0].snake[x].y && col == gameData.players[0].snake[x].x)
					{
						if (gameData.players[0].invincible)
							selectTextColour(clRed);
						else
							selectTextColour(clGreen);
					}
				}
				if (gameData.multiplayer)
				{
					for (int x = 1; x < gameData.players[1].snake.size(); x++)
					{
						if (row == gameData.players[1].snake[x].y && col == gameData.players[1].snake[x].x)
						{
							if (gameData.players[1].invincible)
								selectTextColour(clRed);
							else
								selectTextColour(clBlue);
						}
					}
				}
			}
			if (g[row][col] == PILL)
			{
				selectTextColour(clMagenta);
			}
			if (g[row][col] == MONGOOSE)
			{
				selectTextColour(clDarkBlue);
			}
			cout << g[row][col];	//output cell content
		}
		cout << endl;
	}
}

void endProgram(GameData& gameData)
{
	void showMessage(const WORD backColour, const WORD textColour, int x, int y, const string& message);

	for (Player& player : gameData.players)
	{
		if (player.mouseEaten >= MAXMICE)
		{
			showMessage(clRed, clYellow, 40, 12, "YOU WON");
			//Save score if not cheated
			if (!gameData.hasCheated)
			{
				if (gameData.keysPressed > player.bestScore || player.bestScore == 500)
				{
					ofstream scoreFile;
					scoreFile.open(player.name + ".txt");
					scoreFile << gameData.keysPressed;
					scoreFile.close();
				}
			}
		}

		if (player.dead)
			showMessage(clRed, clYellow, 40, 12, "PLAYER " + tostring(player.id) + " DIED");
	}
	if (gameData.timeLeft == 0)
		showMessage(clRed, clYellow, 40, 12, "YOU RAN OUT OF TIME");

	showMessage(clRed, clYellow, 40, 12, "QUITTING GAME");
	system("pause");	//hold output screen until a keyboard key is hit

}

int getBestScore(string& name)
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

void toggleCheat(GameData& gameData)
{
	if (gameData.cheatMode == false)
	{
		gameData.cheatMode = true;
		gameData.hasCheated = true;
		for (int x = 0; x < 3; x++)
		{
			cout << '\a';
			Sleep(1000);
		}
		for (Player& player : gameData.players)
			while (player.snake.size() > 4)
				player.snake.pop_back();
	}
	else
		gameData.cheatMode = false;
}
