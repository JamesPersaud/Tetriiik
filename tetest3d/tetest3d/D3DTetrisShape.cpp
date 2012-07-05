#include "D3DTetrisShape.h"
#include "TetrisEnums.h"

using namespace std;

//Constructors

	D3DTetrisShape::D3DTetrisShape()
	{
		this->CurrentRotation = 0;
		for(int i = 0;i<D3DTETRISSHAPEROTATIONS;i++)
		{
			for(int j =0;j<D3DTETRISSHAPEROWS*D3DTETRISSHAPECOLUMNS;j++)
			{
				this->Blocks[i][j] = D3DTetrisBlock(EMPTYBLOCK,EMPTYBLOCK);
			}
		}
	}

//Methods

	//Rotate the shape
	void D3DTetrisShape::Rotate()
	{
		if(this->CurrentRotation <3)
			this->CurrentRotation++;
		else
			this->CurrentRotation =0;
	}

	//Load tetris shape from a character array
	void D3DTetrisShape::LoadShape(string
		loadstring, int colour)
	{				
		vector<string> tokens = tokenize(loadstring,";");
		vector<string> innertokens;

		for(int i =0;i<tokens.size();i++)
		{			
			innertokens = tokenize(tokens[i],",");
			for(int j=0;j<innertokens.size();j++)
			{
				if(innertokens[j] == "1")				
					Blocks[i][j] = D3DTetrisBlock(ALIVE,colour);				
				else
					Blocks[i][j] = D3DTetrisBlock(EMPTY,colour);	
			}
		}		
	}

	//Mark all as dead
	void D3DTetrisShape::Kill()
	{		
		for(int j =0;j<D3DTETRISSHAPEROWS*D3DTETRISSHAPECOLUMNS;j++)
		{
			this->Blocks[this->CurrentRotation][j].Kill();
		}		
	}

	//Clone object
	D3DTetrisShape D3DTetrisShape::Clone()
	{
		D3DTetrisShape newshape;

		for(int i = 0;i<D3DTETRISSHAPEROTATIONS;i++)
		{
			for(int j =0;j<D3DTETRISSHAPEROWS*D3DTETRISSHAPECOLUMNS;j++)
			{
				newshape.Blocks[i][j].SetColour(this->Blocks[i][j].GetColour());
				newshape.Blocks[i][j].SetStatus(this->Blocks[i][j].GetStatus());
			}
		}

		newshape.CurrentRotation = this->CurrentRotation;
		return newshape;
	}