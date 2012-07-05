#include "D3DTetrisBlock.h"
#include "D3DTetrisHelpers.h"

#define D3DTETRISSHAPEROTATIONS 4
#define D3DTETRISSHAPEROWS 4
#define D3DTETRISSHAPECOLUMNS 4
#define D3DTETRISSHAPELOADSIZE 127

using namespace std;

//Shapes are instructions to the tetris board on where and how to draw live blocks. Each tetris board has one live shape which
//has coordinates of its top left hand corner and a map of its blocks - a shape also has information on the different rotations of the
//block (4) and knows which rotation it is currently on.

class D3DTetrisShape
{
public:
	//Declarations
	D3DTetrisBlock Blocks[D3DTETRISSHAPEROTATIONS][D3DTETRISSHAPEROWS*D3DTETRISSHAPECOLUMNS];
	int CurrentRotation;

	//Constructors
	D3DTetrisShape();

	//Methods
	void Rotate();
	void LoadShape(string loadstring,int colour);
	void Kill();
	D3DTetrisShape Clone();
};