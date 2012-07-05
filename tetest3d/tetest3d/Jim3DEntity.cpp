/*
	Besic Implementation of JIM3D entity - see Jim3D_Entity.h for descriptions
*/

#include "JIM3D_Entity.h"

using namespace std;

//What?? Can't nest/inherit constructors in C++?? Primitive, much?

JIM3DEntity::JIM3DEntity()
{

}

JIM3DEntity::JIM3DEntity(LPDIRECT3DDEVICE9 d3dp)
{	
	this->TheDevice = d3dp;
	this->EntityID = -1;
	this->EntityName = "<noname>";
	this->LocationVector = D3DXVECTOR3(0.0f,0.0f,0.0f);	
	this->NumMaterials =0;
	this->Pitch = 0.0f;
	this->Roll = 0.0f;
	this->Yaw = 0.0f;
	this->Scale = 1.0f;
	this->ScalingFactor = D3DXVECTOR3(1.0f,1.0f,1.0f);	
	this->InPlay = false;
}

JIM3DEntity::JIM3DEntity(LPDIRECT3DDEVICE9 d3dp,DWORD id, std::string name)
{	
	this->TheDevice = d3dp;
	this->EntityID = id;
	this->EntityName = name;
	this->LocationVector = D3DXVECTOR3(0.0f,0.0f,0.0f);		
	this->NumMaterials =0;
	this->Pitch = 0.0f;
	this->Roll = 0.0f;
	this->Yaw = 0.0f;
	this->Scale = 1.0f;
	this->ScalingFactor = D3DXVECTOR3(1.0f,1.0f,1.0f);	
	this->InPlay = false;
}

JIM3DEntity::JIM3DEntity(LPDIRECT3DDEVICE9 d3dp, DWORD id, std::string name, std::string meshfile_name, D3DXVECTOR3 location, float yaw, float pitch, float roll, float scale)
{	
	this->TheDevice = d3dp;
	this->EntityID = id;
	this->EntityName = name;
	this->LocationVector = location;
	this->Pitch = pitch;
	this->Roll = roll;
	this->Yaw = yaw;
	this->Scale = scale;	
	this->ScalingFactor = D3DXVECTOR3(1.0f,1.0f,1.0f);	
	this->InPlay = false;

	this->LoadMeshFromXFile(meshfile_name);
}

void JIM3DEntity::SetLocationAndAttitude(D3DXVECTOR3 location, float yaw, float pitch, float roll, float scale)
{
	this->LocationVector = location;
	this->Yaw = yaw;
	this->Pitch = pitch;
	this->Roll = roll;
	this->Scale = scale;
}

/*
*	Load the mesh/texture/material from the specified .x mesh
*/
void JIM3DEntity::LoadMeshFromXFile(std::string meshfile_name)
{
	LPD3DXBUFFER bufMaterial;	

    D3DXLoadMeshFromXA(meshfile_name.c_str(),   // load this file
                      D3DXMESH_SYSTEMMEM,		// load the mesh into system memory
                      this->TheDevice,			// the Direct3D Device
                      NULL,						// we aren't using adjacency
                      &bufMaterial,				// put the materials here
                      NULL,						// we aren't using effect instances
                      &this->NumMaterials,		// the number of materials in this model
                      &this->Mesh);				// put the mesh here

    // retrieve the pointer to the buffer containing the material information
    D3DXMATERIAL* tempMaterials = (D3DXMATERIAL*)bufMaterial->GetBufferPointer();

    // create a new material buffer and texture for each material in the mesh
    this->Material = new D3DMATERIAL9[this->NumMaterials];
    this->Texture = new LPDIRECT3DTEXTURE9[this->NumMaterials];

    for(DWORD i = 0; i < this->NumMaterials; i++)    // for each material...
    {
        this->Material[i] = tempMaterials[i].MatD3D;    // get the material info
        this->Material[i].Ambient = this->Material[i].Diffuse;    // make ambient the same as diffuse
        // if there is a texture to load, load it
        if(FAILED(D3DXCreateTextureFromFileA(this->TheDevice,
                                             tempMaterials[i].pTextureFilename,
                                             &this->Texture[i])))
        this->Texture[i] = NULL;    // if there is no texture, set the texture to NULL
      }

    return;
}

void JIM3DEntity::Render()
{
	//Declare Matrices
	D3DXMATRIX matTransform = D3DMATRIX();
	D3DXMATRIX matScale = D3DMATRIX();
	D3DXMATRIX matRotateX = D3DMATRIX();
	D3DXMATRIX matRotateY = D3DMATRIX();	
	D3DXMATRIX matRotateZ = D3DMATRIX();	
	D3DXMATRIX matTranslate = D3DMATRIX();		

	//Define Identity
	D3DXMatrixIdentity(&matTransform);	
	//Define Scaling
	D3DXMatrixScaling(&matScale,
		this->Scale * this->ScalingFactor.x,
		this->Scale * this->ScalingFactor.y,
		this->Scale * this->ScalingFactor.z);
	//Define Rotation
	D3DXMatrixRotationX(&matRotateX,this->Yaw);
	D3DXMatrixRotationY(&matRotateY,this->Pitch);
	D3DXMatrixRotationZ(&matRotateZ,this->Roll);
	//Define Translation
	D3DXMatrixTranslation(&matTranslate,this->LocationVector.x,this->LocationVector.y,this->LocationVector.z);		
	
	//Apply Scaling
	D3DXMatrixMultiply(&matTransform,&matTransform,&matScale);
	//Apply Rotation
	D3DXMatrixMultiply(&matTransform,&matTransform,&matRotateX);
	D3DXMatrixMultiply(&matTransform,&matTransform,&matRotateY);
	D3DXMatrixMultiply(&matTransform,&matTransform,&matRotateZ);
	//Apply Translation
	D3DXMatrixMultiply(&matTransform,&matTransform,&matTranslate);

	//Transform
	this->WorldMatrix = matTransform;
	this->TheDevice->SetTransform(D3DTS_WORLD, &(matTransform));

	for(DWORD i = 0; i < this->NumMaterials; i++)    
	{
		this->TheDevice->SetMaterial(&this->Material[i]);    // set the material
		if(this->Texture[i] != NULL)    
			this->TheDevice->SetTexture(0, this->Texture[i]);    //  set the texture
		this->Mesh->DrawSubset(i);    //Render
	}

	this->InPlay = true;

	return;
}

void JIM3DEntity::Render(LPDIRECT3DDEVICE9 d3ddevice)
{
	this->TheDevice = d3ddevice;
	this->Render();

	return;
}

void JIM3DEntity::Render(LPDIRECT3DDEVICE9 d3ddevice, D3DXVECTOR3 location)
{
	this->LocationVector = location;
	this->Render(d3ddevice);

	return;
}

