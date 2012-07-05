/*

	This is meant to include all the directX stuff necessary for rendering entities and anything else the Jim3D libs end up doing.

*/

//GlobalDefines
#define MATHS_PI 3.14159265		//pi
#define MATHS_RAD 57.3			//degrees / MATHS_RAD = radians

//Include the direct 3D headers
#include <string>
#include <d3d9.h>
#include <d3dx9.h>

//Include the Direct Input headers
#include <dinput.h>

// include the Direct3D Library files
#pragma comment (lib, "d3d9.lib")
#pragma comment (lib, "d3dx9.lib")

// Include Direct Input libs
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

