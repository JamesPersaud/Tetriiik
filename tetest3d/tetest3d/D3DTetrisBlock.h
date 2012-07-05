//A single block on the Tetris Board.
class D3DTetrisBlock
{
private:
	//Private declarations
	int Status;
	int Colour;

public:
	//Properties
	int GetColour(void);
	int GetStatus(void);
	void SetStatus(int);
	void SetColour(int);

	//Constructors
	D3DTetrisBlock(int status, int colour);
	D3DTetrisBlock(){;}
	
	//Methods
	void Kill(void);
};