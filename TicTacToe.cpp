/*
Authors: Christopher Jacobs, Christopher Eberle
TicTacToe for Single User Against the Computer using MIN/MAX Algorithm
Winner is the first player to get three of their characters in a row
NOTE: Intended for 3x3 and 4x4 boards. Boards of other sizes will be displayed, but algorithm is not currently
equipped to handle them (mostly because as the board increases in size the number of nodes on the first computer's turn grows exponentially,
so efforts have been made to substantially limit the nodes, and these solutions only work for a 4x4. Removing these will let it handle any size,
but at the cost of computing times, which are still large[~1 min].)
*/

#include <iostream>
#include <vector>
#include <math.h>
#include <string>
#include <time.h>
using namespace std;

//the number of possilbe spaces. For a 3x3, 9, for a 4x4, 16. CHANGE ME!
#define SIZE 9

//node is the game state, represented by an array, where 0 means the space is unoccupied, and 1 or 2 is the computer or user
struct node {
	int state[SIZE];

	//states need to be able to be compared, so the operator overloading functions are below
	friend bool operator == (node x, node y) {
		for (int i = 0; i < SIZE; i++) {
			if (x.state[i] != y.state[i]) return false;
		}
		return true;
	}
	friend bool operator != (node x, node y) {
		for (int i = 0; i < SIZE; i++) {
			if (x.state[i] != y.state[i]) return true;
		}
		return false;
	}
	//initialize the start of the game by making all places 0
} gameStart = { 0 };

//initial char setting for computer and user
int comChar = 2; //1 = X, 2 = O
int userChar = 1;

//handles the user's turn, i.e., prints gameboard, instructions, takes user input and does error checking, and returns gamestate that user has selected
node userTurn(node option);

//handles the first part of the computer's turn. Input is the current gamestate, output is the computer's chosen gamestate
//acts as a maxPick (it wants the best outcome for itself) and starts the recursive calls to minpick/maxpick
node comTurn(node option);

//given a gamestate, returns an integer indicating kind of gamestate. 2 is incomplete, 0 stalemate, and 1 for com, -1 for user
int gameWon(node option);
//recursive function acting as oponent picking worst choice for computer, returns modified gameWon number depending on desirablility
double minPick(node option, double a, double b, double count);
//recursive function acting as computer picking the best choice
double maxPick(node option, double a, double b, double count);
//input the player's turn (com or user) and current gamestate, return vector of possible moves for that player
vector<node> getSucc(node option, int whosChoice);
//prints curent gameboard
void printGame(node option);
//prints reference board
void printGameNumbers();

void main() {
	//seeds random for later
	srand(time(0));
	//ironically, a test node that became gamestate
	node deleteme;
	//array storing strings used for declaring winners
	string winner[3] = { "User","Stalemate","Computer" };
	string stale;
	//char for user to choose to be x or o
	char XorO;
	//input from user for their char choice
	while (true) {
		cout << "Do you wanna be X or O? ";
		cin >> XorO;
		//clear input stream in case multiple characters are entered
		cin.clear();
		cin.ignore(numeric_limits<streamsize>::max(), '\n'); //jyggorath stackoverflow credited for numeric_limits<streamsize>::max(), though arbitrarily large number also works
															 //user chose o or O
		if (XorO == 79 || XorO == 111) {
			comChar = 1;
			userChar = 2;
			break;
		}
		//user chose x, which is default
		else if (XorO == 88 || XorO == 120) {
			break;
		}
		//continue the loop
		else {
			system("cls");
			cout << "Error. Invalid Character.\n";
		}

	}
	//start of game
	deleteme = gameStart;
	//loop until game is over. 2 means game is still not decided
	while (gameWon(deleteme) == 2) {
		//get new gamestate based on user's choice
		deleteme = userTurn(deleteme);
		//check if game is over
		if (gameWon(deleteme) < 2) break;
		//get new gamestate from computer's turn
		deleteme = comTurn(deleteme);
	}
	//game complete, print winners and gameboard
	printGame(deleteme);
	//if it was 0, then stalemate
	if (gameWon(deleteme) + 1 == 1)
		cout << "\nThe game was a stalemate.\n";
	else
		//since user winning is -1, add 1 to get 0 which is the start of the array for winer strings
		cout << "\nThe winner is the " << winner[gameWon(deleteme) + 1] << "!\n";
	system("pause");
	//end main
}


vector<node> getSucc(node option, int whosChoice) {
	//poschoice is a possible choice
	node posChoice = option;
	vector<node> returnMe;
	//for each position in the array, check if it is empty
	//if not, add current player's number, and save as a possible move
	for (int i = 0; i < SIZE; i++) {
		if (option.state[i] == 0) {
			posChoice.state[i] = whosChoice;
			returnMe.push_back(posChoice);
			//since the state was changed above as if the move was made, change it back before checking for the next one
			posChoice.state[i] = 0;
		}
	}
	return returnMe;
}

//return 1 if com wins, -1 if user wins, 0 if stalemate, 2 if incomplete
int gameWon(node option) {
	//since SIZE is total number of places in the board and the board is square, sqrt returns the number of columns/rows
	int boardSize = static_cast<int>(sqrt(SIZE));
	int place1, place2, place3;
	//check verticle
	//the number of checks is determined by board size. Works for 3x3 and 4x4, untested for above or below.
	//checks each possible location for a won gamestate
	for (int i = 0; i < boardSize*(boardSize - 2); i++) {
		//the array positions for all board sizes remain proportional to winners, allowing the following code to work for any size board
		//place# is the places of the characters
		place1 = option.state[i];
		place2 = option.state[i + boardSize];
		place3 = option.state[i + (2 * boardSize)];
		//the actual check. If they are not equal, then the places do not have the sae character, therefor the gamestate is not determined yet
		if (place1 != 0 && place1 == place2 && place2 == place3) {
			//if they are the same, return who won
			if (place1 == comChar)
				return 1;
			return -1;
		}
	}
	//check diagonal
	for (int i = 0; i < 2 + (boardSize - 3) * 6; i++) {
		//for diagonal down-left for board size 3
		if (boardSize < 4 && i > 0) i = 2;
		if (i % 4 < 2) {
			place1 = option.state[i];
			place2 = option.state[i + boardSize + 1];
			place3 = option.state[i + (2 * (boardSize + 1))];
		}
		else {
			place1 = option.state[i];
			place2 = option.state[i + boardSize - 1];
			place3 = option.state[i + (2 * (boardSize - 1))];
		}
		if (place1 != 0 && place1 == place2 && place2 == place3) {
			if (place1 == comChar)
				return 1;
			return -1;
		}
	}

	//check horizontal
	for (int i = 0; i < boardSize*(boardSize - 2); i++) {
		if (i < boardSize) {
			place1 = option.state[boardSize*i];
			place2 = option.state[boardSize*i + 1];
			place3 = option.state[boardSize*i + 2];
		}
		else {
			place1 = option.state[boardSize*(i % boardSize) + 1];
			place2 = option.state[boardSize*(i % boardSize) + 2];
			place3 = option.state[boardSize*(i % boardSize) + 3];
		}
		if (place1 != 0 && place1 == place2 && place2 == place3) {
			if (place1 == comChar)
				return 1;
			return -1;
		}
	}
	//if there are still 0's in the board, the game is not done
	for (int i = 0; i < SIZE; i++) {
		if (option.state[i] == 0)
			return 2;
	}
	//no 3 in a row, and no 0's left mean the game is over, no one won, so it is a stalemate
	return 0;
}

//prints gameboard one at a time
void printGame(node option) {
	int boardSize = static_cast<int>(sqrt(SIZE));
	char XorO = ' ', Ascii = '-', line = '|', plus = '+';
	for (int i = 0; i < boardSize; i++) {
		for (int j = 0; j < boardSize; j++) {
			if (option.state[i*boardSize + j] == 0) {
				XorO = ' ';
			}
			else XorO = (option.state[i*boardSize + j] == 1) ? 88 : 79;
			if (j%boardSize == boardSize - 1) {
				cout << XorO;
			}
			else {
				cout << XorO << line;
			}
		}
		if (i < boardSize - 1) {
			cout << "\n";
			for (int k = 0; k < boardSize - 1; k++) {
				cout << Ascii << plus;
			}
			cout << Ascii << endl;
		}
		else cout << endl << endl;
	}
}

void printGameNumbers() {
	int boardSize = static_cast<int>(sqrt(SIZE));
	//truenum is the char to display in the instruction board
	int trueNum = 0;
	//for hex characters 9 and above, array is used
	char alpha[7] = { '9','A','B','C','D','E','F' };
	char Ascii = '-', line = '|', plus = '+';
	for (int i = 0; i < boardSize; i++) {
		for (int j = 0; j < boardSize; j++) {
			trueNum = j + i * boardSize;
			if (j%boardSize == boardSize - 1) {
				//if greater than 8, use array index - 9 (so 10 -9 is index 1, which is A, and also why the array starts at 9)
				//array could have started at A, but since 3x3 goes from 0-8, might as well put 9 as index 0
				if (trueNum > 8) {
					cout << alpha[trueNum - 9];
				}
				else
					cout << trueNum;
			}
			else {
				if (trueNum > 8) {
					cout << alpha[trueNum - 9] << line;
				}
				else
					cout << trueNum << line;
			}
		}
		if (i < boardSize - 1) {
			cout << "\n";
			for (int k = 0; k < boardSize - 1; k++) {
				cout << Ascii << plus;
			}
			cout << Ascii << endl;
		}
		else cout << endl;
	}
}

node userTurn(node state) {
	printGame(state);
	int boardSize = static_cast<int>(sqrt(SIZE));
	char input;
	int modIn = -1;
	//loop while user has not made a valid choice
	while (true) {
		printGameNumbers();
		cout << "Please input the Hexidecimal char to place your " << ((userChar == 1) ? "X: " : "O: ");
		cin >> input;
		cin.clear();
		cin.ignore(numeric_limits<streamsize>::max(), '\n'); //jyggorath stackoverflow
															 //numbers between 0-9
		if (input >= 48 && input <= 57) {//boardsize 3 does not contain 9. throw error
			if (boardSize < 4 && input == 57) {
				system("cls");
				cout << "Error: The entered Character is invalid.\n";
				printGame(state);
				continue;
			}
			//get actual number from ascii
			modIn = input - 48;
		}
		else if (input >= 65 && input <= 70 && boardSize > 3) {
			modIn = input - 55; //-64 so A = 0, then +10 so A = 10
		}
		else if (input >= 97 && input <= 102 && boardSize > 3) {
			modIn = input - 87; //-96 so a = 0, then +10 so a = 10
		}
		else {
			system("cls");
			cout << "Error: The entered Character is invalid.\n";
			printGame(state);
			continue;
		}
		if (state.state[modIn] != 0) {
			system("cls");
			cout << "Error: The entered Location is invalid.\n";
			printGame(state);
			continue;
		}
		break;
	}
	//update the gamestate
	state.state[modIn] = userChar;
	printGame(state);
	system("pause");
	system("cls");
	return state;
}

node comTurn(node option) {
	double count = 0;
	cout << "\nComputer's turn\n";
	//get all possible next moves
	vector<node> children = getSucc(option, comChar);
	double maxGame = -100;
	bool randLoop = false;
	int randNum = 0;
	double temp;
	double a = -1; //alpha
	double b = 1;//beta
	node favChild;
	// It took 4 minutes to make the first move for boards larger than 3x3, so reduce work it has to do with below code
	//remove below if statement to line 447 (and all the code inside including else key word but not else code), and remove one bracket from 463
	if (SIZE > 9 && children.size() > 14)
	{
		int userPlace = 0;
		for (int i = 0; i < SIZE; i++) {
			if (option.state[i] == userChar) userPlace = i;
		}
		//there are 3 places a user can place a character for a 4x4: on a corner, on a side, or in the middle
		//for each case, manually choose the most pratical moves (to reduce the number of nodes in needs to look at)
		//note, for larger boards, game does not work
		//on the corner
		if (userPlace == 0 || userPlace == 3 || userPlace == 12 || userPlace == 15) {
			children.clear();
			favChild = gameStart;
			favChild.state[userPlace] = userChar;
			if (userPlace == 0) {
				favChild.state[1] = comChar;
				children.push_back(favChild);
				favChild.state[1] = 0;
				favChild.state[4] = comChar;
				children.push_back(favChild);
				favChild.state[4] = 0;
				favChild.state[5] = comChar;
				children.push_back(favChild);
			}
			else if (userPlace == 3) {
				favChild.state[2] = comChar;
				children.push_back(favChild);
				favChild.state[2] = 0;
				favChild.state[6] = comChar;
				children.push_back(favChild);
				favChild.state[6] = 0;
				favChild.state[7] = comChar;
				children.push_back(favChild);
			}
			else if (userPlace == 12) {
				favChild.state[8] = comChar;
				children.push_back(favChild);
				favChild.state[8] = 0;
				favChild.state[9] = comChar;
				children.push_back(favChild);
				favChild.state[9] = 0;
				favChild.state[13] = comChar;
				children.push_back(favChild);
			}
			else {
				favChild.state[10] = comChar;
				children.push_back(favChild);
				favChild.state[10] = 0;
				favChild.state[11] = comChar;
				children.push_back(favChild);
				favChild.state[11] = 0;
				favChild.state[14] = comChar;
				children.push_back(favChild);
			}

		}
		//if not the corners and not the middle
		else if (!(userPlace == 5 || userPlace == 6 || userPlace == 9 || userPlace == 10)) {
			children.clear();
			favChild = gameStart;
			favChild.state[userPlace] = userChar;
			if (userPlace % 4 == 0 || userPlace % 4 == 3) {
				favChild.state[userPlace + 4] = comChar;
				children.push_back(favChild);
				favChild.state[userPlace + 4] = 0;
				favChild.state[userPlace - 4] = comChar;
				children.push_back(favChild);
			}
			else {
				favChild = { comChar };
				favChild.state[userPlace + 1] = comChar;
				children.push_back(favChild);
				favChild.state[userPlace + 1] = 0;
				favChild.state[userPlace - 1] = comChar;
				children.push_back(favChild);
			}
		}
		//it's in the middle, there is no hope, just randomly pick
		else {
			randLoop = true;
		}
		if (randLoop == true) {
			favChild = option;
			while (randLoop) {
				randNum = rand() % 16;
				if (favChild.state[randNum] == 0) {
					favChild.state[randNum] = comChar;
					randLoop = false;
				}
			}
		}
		//now do minmax for 4x4
		else {
			for (int i = 0; i < children.size(); i++)
			{
				temp = minPick(children[i], a, b, count);
				if (temp >= maxGame) {
					favChild = children[i];
					maxGame = temp;
				}
				if (temp > b) return favChild;
				else (a < maxGame) ? maxGame : a;
			}
		}
	}
	//minmax for 3x3, or remove everything to here+1 to allow AI to calculate for more than 5x5, don't forget the } at 463
	else {
		//for each child node
		for (int i = 0; i < children.size(); i++)
		{
			//see what value would be if that child was chosen
			temp = minPick(children[i], a, b, count);
			//if that is the best pick so far, choose that one for further comparison
			if (temp >= maxGame) {
				favChild = children[i];
				maxGame = temp;
			}
			//if it is better than beta, then it has won, so choose it
			if (temp > b) return favChild;
			//update alpha as the user has won, so other loses will be compared to this
			else (a < maxGame) ? maxGame : a;
		}
	}
	return favChild;
}

double minPick(node option, double a, double b, double count) {
	count += 0.01;
	vector<node> children = getSucc(option, userChar);
	double minGame = 100;
	double temp;
	if (gameWon(option) == 2) {
		//assume that other player will try to maximize their turn from what you select, so use maxPick
		for (int i = 0; i < children.size(); i++) {
			temp = maxPick(children[i], a, b, count);
			if (minGame > temp) minGame = temp;
			if (temp <= a) return temp + count;
			else b = (b > minGame) ? minGame : b;
		}
		//penailze for how many turns it took to finish the game
		return minGame + count;
	}
	else {
		//if this causes the game to end, return penalized temp
		temp = gameWon(option);
		if (temp != 0)
			return temp;
		else
			return temp + count;
	}
}

double maxPick(node option, double a, double b, double count) {
	count += 0.01;
	vector<node> children = getSucc(option, comChar);
	double maxGame = -100;
	double temp;
	if (gameWon(option) == 2) {
		//assume other player will try to minimize your score based on this move
		for (int i = 0; i < children.size(); i++) {
			temp = minPick(children[i], a, b, count);
			if (temp > maxGame) maxGame = temp;
			if (temp >= b) return temp - count;
			else a = (a < maxGame) ? maxGame : a;
		}
		return maxGame - count;
	}
	else {
		temp = gameWon(option);
		if (temp != 0)
			return temp;
		else
			return temp - count;
	}
}