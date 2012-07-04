#include "D3DTetrisFragment.h"

D3DTetrisFragment::D3DTetrisFragment(float x, float y, float z, float xv, float yv, float zv, float yaw, float pitch, float roll, float lifespan, int col, int birthday)
{
	this->X = x;
	this->Y = y;
	this->Z = z;
	this->XVelocity = xv;
	this->YVelocity = yv;
	this->ZVelocity = zv;
	this->Yaw = yaw;
	this->Pitch = pitch;
	this->Roll = roll;
	this->Lifespan = lifespan;
	this->Colour = col;
	this->Birthday = birthday;
}

void D3DTetrisFragment::Update(int now, float gravity)
{
	const float PI = 3.141592f;

	float currenty = this->Y;
	int ts = now - this->Birthday; //ts is total timespan in ticks
	float t = (float)ts / 1000.0f; // get t from conversion factor
	
	this->Y = (this->YVelocity * t) - (gravity /2) * (t * t);
	this->Z = this->ZVelocity * t;
	this->X = this->XVelocity * t;
	this->X = this->X + this->StartX;
	this->Y = this->Y + this->StartY;
	this->Z = this->Z + this->StartZ;

	this->Roll = this->X / PI * 0.4f;
	this->Pitch = this->Z / PI * 0.4f;
	
	this->falling = (this->Y <currenty);
}