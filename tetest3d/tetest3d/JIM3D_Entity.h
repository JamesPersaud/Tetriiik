/*

	Test header file for basic 3D engine implementation.

	Jim3D_Entity.h - Describes a gameworld entity

	Important concepts:
		
		Mesh
		Material
		Texture
		Location info
			X coord
			Y coord
			Z coord
			yaw
			pitch
			roll
			scaling factor???? (not really important, or is it?)
*/

#include "JIM3D_Core_essentials.h"

using namespace std;

class JIM3DEntity 
{
public:

	//The pointer to the D3D device (up yours managed DX!)
	LPDIRECT3DDEVICE9 TheDevice;

	//Constructors / Destructors
	JIM3DEntity();
	JIM3DEntity(LPDIRECT3DDEVICE9 d3dp);
	JIM3DEntity(LPDIRECT3DDEVICE9 d3dp,DWORD id, string name);
	JIM3DEntity(LPDIRECT3DDEVICE9 d3dp,DWORD id, string name, string meshfile_name, D3DXVECTOR3 location, float yaw, float pitch, float roll, float scale);

	//Members

	//Identification Members
	DWORD EntityID;
	string EntityName;

	//Location Members
	D3DXMATRIX WorldMatrix;				// The last world matrix the entity was rendered with.
	D3DXVECTOR3 LocationVector;			// A vector to hold the location of the entity (XYZ)
	D3DXVECTOR3 ScalingFactor;
	float Yaw;
	float Pitch;
	float Roll;
	float Scale;
	bool InPlay;

	//Rendering Members
	LPD3DXMESH Mesh;					// a pointer to the entity's mesh
	D3DMATERIAL9* Material;				// a pointer to the entity's material
	LPDIRECT3DTEXTURE9* Texture;		// a pointer to the entity's texture
	DWORD NumMaterials;					// Number of materials in the entity.

	//Methods
	void LoadMeshFromXFile(string meshfile_name);
	void SetLocationAndAttitude(D3DXVECTOR3 location, float yaw, float pitch, float roll, float scale);
	
	//Render Methods
	void Render();
	void Render(LPDIRECT3DDEVICE9 d3ddevice);
	void Render(LPDIRECT3DDEVICE9 d3ddevice, D3DXVECTOR3 location);
};
