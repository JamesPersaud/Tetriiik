#include <vector>
#include <string>
#include "D3DTetrisShape.h"
#include "D3DTetrisFragment.h"
using namespace std;

#define TETRISLEVELS 10

//TODO: Constructor to load default values from preferences struct.

//A tetris board is a 20x10 rectangle aligned vertically, which holds squares. Squares may be empty or they may have blocks in them.
//Blocks may be of different colours and may be alive or dead.
//Live blocks belong to a shape - a shape may be rotated by 90 degrees - if its requested new coordinates 
//are not occupied by dead blocks.
class D3DTetrisBoard
{
public:
	vector<D3DTetrisShape> ShapeCollection;
	vector<D3DTetrisFragment> Fragments;
	D3DTetrisBlock Blocks[200];
	D3DTetrisShape CurrentShape;
	D3DTetrisShape NextShape;
	int ShapeX;
	int ShapeY;
	float Gamespeed;
	int Lines;
	int Score;
	int Level;
	float FloorLevel;
	int Levels[TETRISLEVELS];
	unsigned long GameStartTick;
	bool dead;

	D3DTetrisBoard();

	void NewGame(unsigned long tick);

	D3DTetrisShape GetRandomShapeFromCollection();

	void LoadShapeIntoCollection(string shapeinfo,int colour);

	void InitializeShapes();

	void SetNextShape();

	void Tick(int ticknow);

	void CleanLives();

	void SetLives();

	void SetDeads();

	bool RequestMove(int move,int ticknow);

	void CleanLines(int ticknow);

	void Shuffle(int shuffley);

	string Spew();

	void LoadLevels(int levels[TETRISLEVELS]);
};