#include "D3DTetrisBoard.h"
#include "TetrisEnums.h"
using namespace std;

D3DTetrisBoard::D3DTetrisBoard()
{	
	for(int i =0;i<200;i++)
	{
		this->Blocks[i] = D3DTetrisBlock(EMPTY,EMPTYBLOCK);
	}

	this->CurrentShape = D3DTetrisShape();
	this->NextShape = D3DTetrisShape();
	this->ShapeX = 4;
	this->ShapeY = 0;
	this->Gamespeed = 1.5f;
	this->Level = 0;
	this->Score = 0;
	this->FloorLevel = 0.0f;
	this->dead = false;
}

void D3DTetrisBoard::NewGame(unsigned long tick)
{
	this->GameStartTick = tick;
	this->Lines =0;
	this->Level =1;

	for(int i =0;i<200;i++)
	{
		this->Blocks[i] = D3DTetrisBlock(EMPTY,EMPTYBLOCK);
	}

	this->InitializeShapes();
	this->SetLives();
}

D3DTetrisShape D3DTetrisBoard::GetRandomShapeFromCollection()
{	
	int collectionsize = this->ShapeCollection.size();
	int randi = GetRandomInt(0,(collectionsize));

	return this->ShapeCollection[randi].Clone();
}

void D3DTetrisBoard::LoadShapeIntoCollection(std::string shapeinfo, int colour)
{
	D3DTetrisShape s = D3DTetrisShape();
	s.LoadShape(shapeinfo,colour);
	this->ShapeCollection.push_back(s);
}

void D3DTetrisBoard::InitializeShapes()
{
	this->CurrentShape = GetRandomShapeFromCollection();
	this->NextShape = GetRandomShapeFromCollection();
}

void D3DTetrisBoard::SetNextShape()
{
	this->CurrentShape = this->NextShape.Clone();
	this->NextShape = this->GetRandomShapeFromCollection();
}

void D3DTetrisBoard::Tick(int ticknow)
{
	this->RequestMove(DOWN, ticknow);
}

void D3DTetrisBoard::CleanLives()
{
	for(int i=0;i<200;i++)
	{
		if(this->Blocks[i].GetStatus() == ALIVE)
		{
			this->Blocks[i].SetColour(EMPTYBLOCK);
			this->Blocks[i].SetStatus(EMPTY);
		}
	}
}

void D3DTetrisBoard::SetLives()
{
	for (int y = 0; y < D3DTETRISSHAPEROWS; y++)
    {
        for (int x = 0; x < D3DTETRISSHAPECOLUMNS; x++)
        {
            if (this->CurrentShape.Blocks[this->CurrentShape.CurrentRotation][(x + (y * 4))].GetStatus() == ALIVE)
            {
                this->Blocks[(x + ShapeX) + ((y + ShapeY) * 10)].SetStatus(ALIVE);
                this->Blocks[(x + ShapeX) + ((y + ShapeY) * 10)].SetColour(this->CurrentShape.Blocks[this->CurrentShape.CurrentRotation][(x + (y * 4))].GetColour());
            }
        }
    }    
}

void D3DTetrisBoard::SetDeads()
{
	for (int y = 0; y < D3DTETRISSHAPEROWS; y++)
    {
        for (int x = 0; x < D3DTETRISSHAPECOLUMNS; x++)
        {
            if (this->CurrentShape.Blocks[this->CurrentShape.CurrentRotation][(x + (y * 4))].GetStatus() == DEAD)
            {
                this->Blocks[(x + ShapeX) + ((y + ShapeY) * 10)].SetStatus(DEAD);
                this->Blocks[(x + ShapeX) + ((y + ShapeY) * 10)].SetColour(this->CurrentShape.Blocks[this->CurrentShape.CurrentRotation][(x + (y * 4))].GetColour());
            }
        }
    }    
}

bool D3DTetrisBoard::RequestMove(int move,int ticknow)
{	
	int requestx = this->ShapeX;
	int requesty = this->ShapeY;

	D3DTetrisShape requestshape = this->CurrentShape.Clone();

	bool allowed = true;

	switch(move)
	{
	case LEFT:
		requestx = ShapeX-1;
        requesty = ShapeY;break;
	case RIGHT:
		requestx = ShapeX+1;
        requesty = ShapeY;
		break;
	case DOWN:
		requestx = ShapeX;
        requesty = ShapeY + 1;
        break;
	case DROPDOWN:
		requestx = ShapeX;
        requesty = ShapeY + 1;
        break;
	case ROTATE:
		requestshape.Rotate();
        break;
	case DROP:
		bool b=true;
        do{b = RequestMove(DROPDOWN, ticknow);} while (b); return true;break;
	}

	for (int y = 0; y < D3DTETRISSHAPEROWS; y++)
    {
        for (int x = 0; x < D3DTETRISSHAPECOLUMNS; x++)
        {
            if (requestshape.Blocks[requestshape.CurrentRotation][(x + (y * D3DTETRISSHAPECOLUMNS))].GetStatus() == ALIVE)
            {
                if (x + requestx < 0 || x + requestx > 9)
                {
                    allowed = false; break;
                }
                if (y + requesty < 0 || y + requesty > 19)
                {
                    allowed = false; break;
                }
                if (Blocks[((requesty + y) * 10) + x + requestx].GetStatus() == DEAD)
                {
                    allowed = false; break;
                }
            }
        }
        if (!allowed) { break; }
	}

	if (!allowed && move == DOWN)
    {
		if(ShapeY == 0)
		{
			this->dead = true;			
		}
		
		CurrentShape.Kill();                
		SetDeads();
		CleanLives();
		SetNextShape();
		ShapeX = 4;
		ShapeY = 0;
		CleanLines(ticknow);
		SetLives();		
    }

    if (allowed)
    {
        CurrentShape = requestshape.Clone();
        
        ShapeX = requestx;
        ShapeY = requesty;                

        CleanLives();
        SetLives();
    }            

    return allowed;    
}

void D3DTetrisBoard::CleanLines(int ticknow)
{
	int deadcount =0;

	for(int y =0;y<20;y++)
	{
		deadcount =0;
		for(int x=0;x<10;x++)
		{
			if(this->Blocks[(y*10)+x].GetStatus() == DEAD)
			{
				deadcount++;
			}
		}
		if(deadcount == 10)
		{
			this->dead = false;
			this->Lines++;
			this->Gamespeed = 1.5f;
			for(int l =0;l< TETRISLEVELS;l++)
			{
				if (Lines >= this->Levels[l])
                {
                    this->Gamespeed = this->Gamespeed * 0.75f;
                    Level = l+1;
                }
			}

			//Add Fragments
            D3DTetrisFragment* currentFragment;
            for (int x = 0; x < 10; x++)
            {                        
                currentFragment = &D3DTetrisFragment(x, y, 0, GetRandomFloat(-19.0, 19.0), GetRandomFloat(-80.0, -15.0), GetRandomFloat(-19.0, 19.0), 0, 0, 0, 10000, Blocks[(y * 10) + x].GetColour(),ticknow);                       
                Fragments.push_back(*currentFragment);
                currentFragment = &D3DTetrisFragment(x + 0.5f, y, 0, GetRandomFloat(-19.0, 19.0), GetRandomFloat(-80.0, -15.0), GetRandomFloat(-19.0, 19.0), 0, 0, 0, 10000, Blocks[(y * 10) + x].GetColour(),ticknow);                       
                Fragments.push_back(*currentFragment);
                currentFragment = &D3DTetrisFragment(x, y + 0.5f, 0, GetRandomFloat(-19.0, 19.0), GetRandomFloat(-80.0, -15.0), GetRandomFloat(-19.0, 19.0), 0, 0, 0, 10000, Blocks[(y * 10) + x].GetColour(),ticknow);                                            
                Fragments.push_back(*currentFragment);
                currentFragment = &D3DTetrisFragment(x + 0.5f, y + 0.5f, 0, GetRandomFloat(-19.0, 19.0), GetRandomFloat(-80.0, -15.0), GetRandomFloat(-19.0, 19.0), 0, 0, 0, 10000, Blocks[(y * 10) + x].GetColour(),ticknow);                                           
                Fragments.push_back(*currentFragment); 
            }

            this->Shuffle(y);    
		}	
	}
}

void D3DTetrisBoard::Shuffle(int shuffley)
{
	this->Score = Score + (shuffley * Level);

    int x=0;

    for (x = 0; x < 10; x++)
    {
        Blocks[(shuffley * 10) + x].SetStatus(EMPTY);
        Blocks[(shuffley * 10) + x].SetColour(EMPTYBLOCK);
    }

    for (int y = shuffley; y > 0; y--)
    {
        for (x = 0; x < 10; x++)
        {
            Blocks[(y * 10) + x].SetStatus(Blocks[((y - 1) * 10) + x].GetStatus());
            Blocks[(y * 10) + x].SetColour(Blocks[((y - 1) * 10) + x].GetColour());
        }
    }
}

string D3DTetrisBoard::Spew()
{	
	string s = "";
    for(int y=0;y<20;y++)
    {
        for (int x = 0; x < 10; x++)
        {
            if(Blocks[x + y * 10].GetStatus() == ALIVE)			
				s.append("1");
			else if(Blocks[x + y * 10].GetStatus() == DEAD)	
				s.append("2");
			else if(Blocks[x + y * 10].GetStatus() == EMPTY)	
				s.append("0");
        }
        s.append("\n");
    }    
    return s;        
}

void D3DTetrisBoard::LoadLevels(int levels[TETRISLEVELS])
{
	for(int i=0;i<TETRISLEVELS;i++)
		this->Levels[i] = levels[i];
}