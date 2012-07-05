#include "TetrisEnums.h"
#include "D3DTetrisBlock.h"

//Constructors
	D3DTetrisBlock::D3DTetrisBlock(int status, int colour)
	{
		this->Status = status;
		this->Colour = colour;
	}

//Properties

	//Colour
	int D3DTetrisBlock::GetColour(){return this->Colour;}
	void D3DTetrisBlock::SetColour(int colour){this->Colour = colour;}

	//Status
	int D3DTetrisBlock::GetStatus(){return this->Status;}
	void D3DTetrisBlock::SetStatus(int status){this->Status = status;}

//Methods

	void D3DTetrisBlock::Kill()
	{
		if (this->Status == ALIVE)
			this->Status = DEAD;
	}
	

