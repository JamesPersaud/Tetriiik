// include the basic windows header files and the Direct3D header file
#include <windows.h>
#include <windowsx.h>
#include <time.h>
#include <iostream>
#include <sstream>
#include "D3DTetrisBoard.h"
#include "TetrisEnums.h"
#include "HighScores.h"

//Include directX/Jim3d stuff	
#include "JIM3D.h"		// Direct3D | DirectInput

//STD includes
#include <queue> //to handle input

using namespace std;

// define the screen resolution and keyboard macros
#define SCREEN_WIDTH 2560
#define SCREEN_HEIGHT 1600
#define KEY_DOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)
#define KEY_UP(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 0 : 1)

// global declarations
LPDIRECT3D9 d3d;    // the pointer to our Direct3D interface
LPDIRECT3DDEVICE9 d3ddev;    // the pointer to the device class
LPDIRECT3DVERTEXBUFFER9 t_buffer = NULL;    // the pointer to the vertex buffer
LPDIRECT3DSURFACE9 z_buffer = NULL;    // the pointer to the z-buffer
LPD3DXFONT dxfont;    // the pointer to the font object
LPD3DXFONT dxfontBIG;

D3DPRESENT_PARAMETERS d3dpp;

//Highscores
vector<HighScorePair> Tetris_HighScores;

//sprites
LPD3DXSPRITE d3dspt;
LPDIRECT3DTEXTURE9 mouse_sprite;    // the pointer to the sprite
LPDIRECT3DTEXTURE9 panel32_sprite[9];
LPDIRECT3DTEXTURE9 logo;

long mouseX =0;
long mouseY =0;
long mouseZ =0;

long mouseCurrentX = (long)SCREEN_WIDTH/2;
long mouseCurrentY = (long)SCREEN_HEIGHT/2;
long mouseCurrentZ =500;

bool mouselooking = false;

//camera and position
D3DXVECTOR3 vector_camera;
D3DXVECTOR3 vector_position;

//Grid Gobals
#define GRIDFVF (D3DFVF_XYZ | D3DFVF_DIFFUSE)
LPDIRECT3DVERTEXBUFFER9 g_buffer = NULL;
struct GRIDVERTEX {D3DXVECTOR3 position; DWORD color;};

//Direct Input pointers
LPDIRECTINPUT8 din;
LPDIRECTINPUTDEVICE8 dinkeyboard;
LPDIRECTINPUTDEVICE8 dinmouse;    // the pointer to the mouse device

// mesh declarations
LPD3DXMESH meshSpaceship;    // define the mesh pointer
D3DMATERIAL9* material;    // define the material object
LPDIRECT3DTEXTURE9* texture;    // a pointer to a texture
DWORD numMaterials;    // stores the number of materials in the mesh

//Picking*************************
D3DXVECTOR3 mouserayOrigin,mouserayDir;
string hits = "";
bool doNewPickTest = true;
bool pickbuttondown = false;

//Tetris**************************
D3DTetrisBoard CurrentTetrisBoard;
float Gravity = -60.0f;

//JIM3D********************************
JIM3DEntity ColouredBricks[7];
JIM3DEntity BoardSide;
JIM3DEntity BoardTop;
//*************************************

//Game state
int GAMESTATE;
enum GAMESTATEENUM {PLAY,PAUSE,GAMEOVER,QUIT};

//Keyboard input
queue<int> KeyQueue;
int lastkeypressed = -1;
bool lastkeyreleased = true;

//TIME
DWORD timesincelastkeychange;
DWORD start_time;
DWORD last_frame;
int frame_count;
DWORD GameTicks =0;
DWORD GameLastTick =0;
DWORD lastpaused=0;


//DEBUG
string debugstring= (string)"";
bool debugoutput = false;

// function prototypes
void initD3D(HWND hWnd);    // sets up and initializes Direct3D
void render_frame(void);    // renders a single frame
void cleanD3D(void);    // closes Direct3D and releases memory
void init_graphics(void);    // 3D declarations
void init_light(void);    // sets up the light and the material

void initDInput(HINSTANCE hInstance, HWND hWnd);
void detect_keys(DWORD ticknow);
void cleanDInput(void);
void detect_mousepos(void);

void render_grid(void);
void set_mouse_cursor(bool looking);

void third_person_look(float x, float y, float z,float zoom,bool rotatebutton);

void mouseray_test(void);
bool picktest(JIM3DEntity m_ent);

void IncrementTimer();
DWORD GetGameTick();
void TogglePause();

void ResetDevice();

//TestUI
void LoadTestPanelSprites();
void RenderTestPanel(int x,int y, int width, int height);
void RenderHeaderPanel(int x,int y, int width, int height,string header);
int UI_X(float percent);
int UI_Y(float percent);



//DEBUG
void WriteDebugToFile(string debugmessage);
void WriteDebugToString(string debugmessage);


//tostring func
template <class T> inline string to_string (const T& t)
{
	std::stringstream ss;
	ss << t;
	return ss.str();
}

void draw_simple_text(string buffer, int x,int y);

// the WindowProc function prototype
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);


// the entry point for any Windows program
int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine,
                   int nCmdShow)
{
    HWND hWnd;
    WNDCLASSEX wc;

    ZeroMemory(&wc, sizeof(WNDCLASSEX));

    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = (WNDPROC)WindowProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = L"TetriikWindowClass";

    RegisterClassEx(&wc);

    hWnd = CreateWindowEx(NULL,
                          L"TetriikWindowClass",
                          L"TetriiiK 0.01",
                          WS_EX_TOPMOST | WS_POPUP,
                          0, 0,
                          SCREEN_WIDTH, SCREEN_HEIGHT,
                          NULL,
                          NULL,
                          hInstance,
                          NULL);

    ShowWindow(hWnd, nCmdShow);

	// set up and initialize DirectInput
    initDInput(hInstance, hWnd);

    // set up and initialize Direct3D
    initD3D(hWnd);

    // enter the main loop:

    MSG msg;

	//Randomise

	srand(time(NULL));
	
	float testf;
	int testi;

	testf = GetRandomFloat(0.0f,10.0f); // discard the first number
	testi = GetRandomInt(0,10);

	//Set Up Tetris
	CurrentTetrisBoard = D3DTetrisBoard();
	int iar[10] = {10,20,30,40,50,60,70,80,90,100};
	CurrentTetrisBoard.LoadLevels(iar);
	CurrentTetrisBoard.LoadShapeIntoCollection("1,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0;0,1,1,0,0,1,0,0,0,1,0,0,0,0,0,0;0,0,0,0,1,1,1,0,0,0,1,0,0,0,0,0;0,1,0,0,0,1,0,0,1,1,0,0,0,0,0,0",BLUE);
	CurrentTetrisBoard.LoadShapeIntoCollection("0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0;0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0;0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0;0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0",CYAN);
	CurrentTetrisBoard.LoadShapeIntoCollection("0,1,1,0,1,1,0,0,0,0,0,0,0,0,0,0;1,0,0,0,1,1,0,0,0,1,0,0,0,0,0,0;0,1,1,0,1,1,0,0,0,0,0,0,0,0,0,0;1,0,0,0,1,1,0,0,0,1,0,0,0,0,0,0",GREEN);
	CurrentTetrisBoard.LoadShapeIntoCollection("0,0,0,1,0,1,1,1,0,0,0,0,0,0,0,0;0,0,1,0,0,0,1,0,0,0,1,1,0,0,0,0;0,0,0,0,0,1,1,1,0,1,0,0,0,0,0,0;0,1,1,0,0,0,1,0,0,0,1,0,0,0,0,0",ORANGE);
	CurrentTetrisBoard.LoadShapeIntoCollection("0,1,0,0,1,1,1,0,0,0,0,0,0,0,0,0;0,1,0,0,0,1,1,0,0,1,0,0,0,0,0,0;0,0,0,0,1,1,1,0,0,1,0,0,0,0,0,0;0,1,0,0,1,1,0,0,0,1,0,0,0,0,0,0",PINK);
	CurrentTetrisBoard.LoadShapeIntoCollection("1,1,0,0,0,1,1,0,0,0,0,0,0,0,0,0;0,1,0,0,1,1,0,0,1,0,0,0,0,0,0,0;1,1,0,0,0,1,1,0,0,0,0,0,0,0,0,0;0,1,0,0,1,1,0,0,1,0,0,0,0,0,0,0",RED);
	CurrentTetrisBoard.LoadShapeIntoCollection("1,1,0,0,1,1,0,0,0,0,0,0,0,0,0,0;1,1,0,0,1,1,0,0,0,0,0,0,0,0,0,0;1,1,0,0,1,1,0,0,0,0,0,0,0,0,0,0;1,1,0,0,1,1,0,0,0,0,0,0,0,0,0,0",YELLOW);			

		/*
					PINK, 
					YELLOW,	
					RED, 
					GREEN, 
					CYAN, 
					BLUE, 
					ORANGE*/

	//set up tetris coloured bricks	
	ColouredBricks[PINK] = JIM3DEntity(d3ddev,PINK,"Pink Brick","BOX_PINK.x",D3DXVECTOR3(0.0f,0.0f,0.0f),0.0f,0.0f,0.0f,1.0f);
	ColouredBricks[YELLOW] = JIM3DEntity(d3ddev,YELLOW,"Yellow Brick","BOX_YELLOW.x",D3DXVECTOR3(0.0f,0.0f,0.0f),0.0f,0.0f,0.0f,1.0f);
	ColouredBricks[RED] = JIM3DEntity(d3ddev,RED,"Red Brick","BOX_RED.x",D3DXVECTOR3(0.0f,0.0f,0.0f),0.0f,0.0f,0.0f,1.0f);
	ColouredBricks[GREEN] = JIM3DEntity(d3ddev,GREEN,"Green Brick","BOX_GREEN.x",D3DXVECTOR3(0.0f,0.0f,0.0f),0.0f,0.0f,0.0f,1.0f);
	ColouredBricks[CYAN] = JIM3DEntity(d3ddev,CYAN,"Cyan Brick","BOX_CYAN.x",D3DXVECTOR3(0.0f,0.0f,0.0f),0.0f,0.0f,0.0f,1.0f);
	ColouredBricks[BLUE] = JIM3DEntity(d3ddev,BLUE,"Blue Brick","BOX_BLUE.x",D3DXVECTOR3(0.0f,0.0f,0.0f),0.0f,0.0f,0.0f,1.0f);
	ColouredBricks[ORANGE] = JIM3DEntity(d3ddev,ORANGE,"Orange Brick","BOX_ORANGE.x",D3DXVECTOR3(0.0f,0.0f,0.0f),0.0f,0.0f,0.0f,1.0f);

	//board sides
	BoardSide = JIM3DEntity(d3ddev,99,"Board Side","tetris_side_1.x",D3DXVECTOR3(0.0f,0.0f,0.0f),0.0f,0.0f,0.0f,1.0f);	
	BoardTop = JIM3DEntity(d3ddev,99,"Board Side","tetris_side_1.x",D3DXVECTOR3(0.0f,0.0f,0.0f),0.0f,0.0f,0.0f,1.0f);	
		
	//Load highscores
	Tetris_HighScores = LoadSc((string)""); //Assume default filename
	SortHighScorePair(&Tetris_HighScores);

	//1 second is 1000 ticks	

	//Init timer
	GameLastTick = GetTickCount();	

	DWORD thistick = GetGameTick();

	last_frame = thistick;
	DWORD last_tetris_tick = thistick;
	DWORD last_input_process = GameLastTick;
	DWORD start_time = thistick;
	frame_count = 0;
	lastpaused = 0;
	
	GAMESTATE = PLAY;	

	CurrentTetrisBoard.NewGame(thistick);

	float floorlevel = 22.0f;

    while(GAMESTATE != QUIT)
    {        
		if(GAMESTATE != PAUSE)
		{			
			IncrementTimer();
		}

        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
                break;

            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }				

		//mouse
		detect_mousepos();

		// experiment
		render_frame();				//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ RENDER

		//Only render frame 40 times/second   //////////////// <-- Do stuff in here every frame
		if((GetGameTick() - last_frame) >= 15)
		{			
			frame_count++;
			doNewPickTest = false; // do not require unless mouse has been pressed this frame
			last_frame = GetGameTick();

			//Update and Clean fragments
			for(int i=0;i<CurrentTetrisBoard.Fragments.size();i++)
			{
				if(CurrentTetrisBoard.Fragments[i].Y >= floorlevel && CurrentTetrisBoard.Fragments[i].falling)
				{					
					CurrentTetrisBoard.Fragments[i].Y = floorlevel;
					CurrentTetrisBoard.Fragments[i].XVelocity *= 0.9;
					CurrentTetrisBoard.Fragments[i].YVelocity *= 0.7;
					CurrentTetrisBoard.Fragments[i].ZVelocity *= 0.9;
					CurrentTetrisBoard.Fragments[i].StartX = CurrentTetrisBoard.Fragments[i].X;
					CurrentTetrisBoard.Fragments[i].StartY = CurrentTetrisBoard.Fragments[i].Y;
					CurrentTetrisBoard.Fragments[i].StartZ = CurrentTetrisBoard.Fragments[i].Z;
					CurrentTetrisBoard.Fragments[i].Lifespan -= last_frame - CurrentTetrisBoard.Fragments[i].Birthday;
					CurrentTetrisBoard.Fragments[i].Birthday = last_frame;
					CurrentTetrisBoard.Fragments[i].Update(last_frame,Gravity);
				}
				else
				{
					CurrentTetrisBoard.Fragments[i].Update(last_frame,Gravity);					
				}
				if(CurrentTetrisBoard.Fragments[i].Birthday + CurrentTetrisBoard.Fragments[i].Lifespan < last_frame)
						CurrentTetrisBoard.Fragments.erase(CurrentTetrisBoard.Fragments.begin() + i);
			}
		}		

		//Tick tetris every second*speed factor

		if(CurrentTetrisBoard.dead)
		{
			if(GAMESTATE != GAMEOVER)
			{
				Tetris_HighScores.push_back(NewHighScorePair("Jim",CurrentTetrisBoard.Score));
				SaveSc(Tetris_HighScores,(string)"");
				GAMESTATE = GAMEOVER;
			}
		}

		if(GAMESTATE == PLAY && (GetGameTick() - last_tetris_tick) >= (CurrentTetrisBoard.Gamespeed*800))		
		{
			last_tetris_tick = GetGameTick();
			CurrentTetrisBoard.Tick(last_tetris_tick);
		}				

		detect_keys(GetGameTick());

		//process key queue every frame
		if(((GetGameTick() - last_input_process) >= 15) || (GAMESTATE ==PAUSE))		
		{						
			last_input_process = GetGameTick();
			if(!KeyQueue.empty())
			{
				int nextkeypress = KeyQueue.front();

				switch(nextkeypress)
				{
				case ROTATE:
					if(GAMESTATE==PLAY)CurrentTetrisBoard.RequestMove(ROTATE,last_input_process);break;
				case LEFT:
					if(GAMESTATE==PLAY)CurrentTetrisBoard.RequestMove(LEFT,last_input_process);break;
				case DOWN:
					if(GAMESTATE==PLAY)CurrentTetrisBoard.RequestMove(DOWN,last_input_process);break;
				case RIGHT:
					if(GAMESTATE==PLAY)CurrentTetrisBoard.RequestMove(RIGHT,last_input_process);break;
				case DROP:
					if(GAMESTATE==PLAY)CurrentTetrisBoard.RequestMove(DROP,last_input_process);break;
				case PAUSEKEY:
					TogglePause();
				}

				KeyQueue.pop();
			}
		}

        // check the 'escape' key
        if(KEY_DOWN(VK_ESCAPE))
		{
			WriteDebugToString((string)"Posting WM_DESTROY message");
            PostMessage(hWnd, WM_DESTROY, 0, 0);			
		}
    }

	cleanDInput();

    // clean up DirectX and COM
    cleanD3D();

    return msg.wParam;
}


// this is the main message handler for the program
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch(message)
    {		
		case WM_SETCURSOR:
			// Turn off window cursor. 			
			SetCursor( NULL );
			d3ddev->ShowCursor( TRUE );
			return TRUE; // Prevent Windows from setting cursor to window class cursor.			
			break;			
        case WM_DESTROY:
            {				
				//do once!
				WriteDebugToFile(debugstring);
                PostQuitMessage(0);
                return 0;
            } break;
		case WM_ACTIVATEAPP:
            {
				WriteDebugToString((string)"Handling WM_ACTIVATEAPP message");
                if(wParam)
				{
					WriteDebugToString((string)"Calling ResetDevice");
                    ResetDevice();					
				}
				return 0;
            } break;
    }

    return DefWindowProc (hWnd, message, wParam, lParam);
}


// this function initializes and prepares Direct3D for use
void initD3D(HWND hWnd)
{
    d3d = Direct3DCreate9(D3D_SDK_VERSION);    

    ZeroMemory(&d3dpp, sizeof(d3dpp));
    d3dpp.Windowed = FALSE;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.hDeviceWindow = hWnd;
    d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
    d3dpp.BackBufferWidth = SCREEN_WIDTH;
    d3dpp.BackBufferHeight = SCREEN_HEIGHT;
    d3dpp.EnableAutoDepthStencil = TRUE;    // automatically run the z-buffer for us
    d3dpp.AutoDepthStencilFormat = D3DFMT_D16;    // 16-bit pixel format for the z-buffer

    // create a device class using this information and the info from the d3dpp stuct
    d3d->CreateDevice(D3DADAPTER_DEFAULT,
                      D3DDEVTYPE_HAL,
                      hWnd,
                      D3DCREATE_HARDWARE_VERTEXPROCESSING,
                      &d3dpp,
                      &d3ddev);

    // create the z-buffer
    d3ddev->CreateDepthStencilSurface(SCREEN_WIDTH,
                                      SCREEN_HEIGHT,
                                      D3DFMT_D16,
                                      D3DMULTISAMPLE_NONE,
                                      0,
                                      TRUE,
                                      &z_buffer,
                                      NULL);

    init_graphics();    // call the function to initialize the triangle
    init_light();    // call the function to initialize the light and material

    d3ddev->SetRenderState(D3DRS_LIGHTING, TRUE);    // turn on the 3D lighting
    d3ddev->SetRenderState(D3DRS_ZENABLE, TRUE);    // turn on the z-buffer
    d3ddev->SetRenderState(D3DRS_AMBIENT, D3DCOLOR_XRGB(50, 50, 50));    // ambient light

	D3DXCreateFont(d3ddev,    // the D3D Device
                   30,    // font height of 30
                   0,    // default font width
                   FW_NORMAL,    // font weight
                   1,    // not using MipLevels
                   true,    // italic font
                   DEFAULT_CHARSET,    // default character set
                   OUT_DEFAULT_PRECIS,    // default OutputPrecision,
                   DEFAULT_QUALITY,    // default Quality
                   DEFAULT_PITCH | FF_DONTCARE,    // default pitch and family
                   L"Courier New",    // use Facename Arial
                   &dxfont);    // the font object

	D3DXCreateFont(d3ddev,    // the D3D Device
                   90,    // font height of 30
                   0,    // default font width
                   FW_NORMAL,    // font weight
                   1,    // not using MipLevels
                   true,    // italic font
                   DEFAULT_CHARSET,    // default character set
                   OUT_DEFAULT_PRECIS,    // default OutputPrecision,
                   DEFAULT_QUALITY,    // default Quality
                   DEFAULT_PITCH | FF_DONTCARE,    // default pitch and family
                   L"Courier New",    // use Facename Arial
                   &dxfontBIG);    // the font object

	D3DXCreateSprite(d3ddev, &d3dspt);    // create the Direct3D Sprite object

    D3DXCreateTextureFromFile(d3ddev, L"mouse.png", &mouse_sprite);	

	LoadTestPanelSprites();


	vector_camera = D3DXVECTOR3(0.0f, 0.0f, 70.0f);
	vector_position = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	

    return;
}

// this is the function that initializes DirectInput
void initDInput(HINSTANCE hInstance, HWND hWnd)
{
    // create the DirectInput interface
    DirectInput8Create(hInstance,    // the handle to the application
                       DIRECTINPUT_VERSION,    // the compatible version
                       IID_IDirectInput8,    // the DirectInput interface version
                       (void**)&din,    // the pointer to the interface
                       NULL);    // COM stuff, so we'll set it to NULL

    // create the keyboard device
    din->CreateDevice(GUID_SysKeyboard,    // the default keyboard ID being used
                      &dinkeyboard,    // the pointer to the device interface
                      NULL);    // COM stuff, so we'll set it to NULL

	// create the mouse device
    din->CreateDevice(GUID_SysMouse,    // the default mouse ID being used
                      &dinmouse,    // the pointer to the device interface
                      NULL);    // COM stuff, so we'll set it to NULL


    dinkeyboard->SetDataFormat(&c_dfDIKeyboard); // set the data format to keyboard format
	dinmouse->SetDataFormat(&c_dfDIMouse);    // set the data format to mouse format

    // set the control you will have over the keyboard
    dinkeyboard->SetCooperativeLevel(hWnd,
                                     DISCL_NONEXCLUSIVE | DISCL_BACKGROUND);

	dinmouse->SetCooperativeLevel(hWnd,
                                  DISCL_NONEXCLUSIVE | DISCL_BACKGROUND);

    dinmouse->Acquire();    // Acquire the mouse only once


    return;    // return to WinMain()
}

// update key buffer
void detect_keys(DWORD ticknow)
{
    static BYTE keystate[256];    // create a static storage for the key-states

    dinkeyboard->Acquire();    // get access if we don't have it already

    dinkeyboard->GetDeviceState(256, (LPVOID)keystate);    // fill keystate with values

	int keypressed =-1;

	if(keystate[DIK_W] & 0x80) // TODO: redefine keys!!!!!!!!!!
		keypressed = ROTATE;
	else if (lastkeypressed == ROTATE)
		lastkeyreleased = true;

	if(keystate[DIK_A] & 0x80)
		keypressed = LEFT;
	else if (lastkeypressed == LEFT)
		lastkeyreleased = true;

	if(keystate[DIK_S] & 0x80) 
		keypressed = DOWN;
	else if (lastkeypressed == DOWN)
		lastkeyreleased = true;

	if(keystate[DIK_D] & 0x80)
		keypressed = RIGHT;
	else if (lastkeypressed == RIGHT)
		lastkeyreleased = true;

	if(keystate[DIK_SPACE] & 0x80)
		keypressed = DROP;	
	else if (lastkeypressed == DROP)
		lastkeyreleased = true;

	if(keystate[DIK_P] & 0x80)
		keypressed = PAUSEKEY;	
	else if (lastkeypressed == PAUSEKEY)
		lastkeyreleased = true;		

	if(KeyQueue.size() <20 && keypressed != lastkeypressed)	
	{
		KeyQueue.push(keypressed);		
		timesincelastkeychange = ticknow;
		lastkeyreleased = false;
	}			

	if(KeyQueue.size() <1 && !lastkeyreleased && (ticknow - timesincelastkeychange >=500))
	{
		KeyQueue.push(keypressed);
	}

	lastkeypressed = keypressed;

    //if(keystate[DIK_A] & 0x80)    // if the 'A' key was pressed...

    //    // then inform the user of this very important message:
    //    MessageBox(NULL, L"You pressed the 'A' key!", L"IMPORTANT MESSAGE!", MB_OK);

    return;
}

// this is the function that closes DirectInput
void cleanDInput(void)
{
    dinkeyboard->Unacquire();    // make sure the keyboard is unacquired
	dinmouse->Unacquire();    // make sure the mouse is unacquired
    din->Release();    // close DirectInput before exiting

    return;
}

//Draw simple text with default font
void draw_simple_text(string buffer, int x,int y)
{
	// create a RECT to contain the text
    static RECT textbox; 
	SetRect(&textbox, x, y, SCREEN_WIDTH, SCREEN_HEIGHT); 	

	dxfont->DrawTextA(NULL,
                      buffer.c_str(),
                      buffer.size(),
                      &textbox,
                      DT_LEFT | DT_TOP,
                      D3DCOLOR_ARGB(255, 150, 250, 150));
}


// this is the function used to render a single frame
void render_frame(void)
{
    d3ddev->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 15), 1.0f, 0);
    d3ddev->Clear(0, NULL, D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 15), 1.0f, 0);

    d3ddev->BeginScene();	

	DWORD currenttick = GetGameTick();		
			
    // SET UP THE TRANSFORMS

    D3DXMATRIX matView;    // the view transform matrix
    D3DXMatrixLookAtLH(&matView,
    &vector_camera,    // the camera position
    &vector_position,    // the look-at position
    &D3DXVECTOR3 (0.0f, 1.0f, 0.0f));    // the up direction
    d3ddev->SetTransform(D3DTS_VIEW, &matView);    // set the view transform to matView 

    D3DXMATRIX matProjection;    // the projection transform matrix
    D3DXMatrixPerspectiveFovLH(&matProjection,
                               D3DXToRadian(45),    // the horizontal field of view
                               //SCREEN_WIDTH / SCREEN_HEIGHT,    // the aspect ratio
							   (float)SCREEN_WIDTH/(float)SCREEN_HEIGHT,
                               1.0f,    // the near view-plane
                               500.0f);    // the far view-plane
    d3ddev->SetTransform(D3DTS_PROJECTION, &matProjection);    // set the projection  

	//TETRIS-----------------*********---------------	
	
	D3DXMATRIX matTranslate;	

	//Render Tetris Board
	for(int y=0;y<20;y++)
    {
        for (int x = 0; x < 10; x++)
        {
            if(CurrentTetrisBoard.Blocks[x + y * 10].GetStatus() == ALIVE)			
			{				
				ColouredBricks[CurrentTetrisBoard.Blocks[x + y * 10].GetColour()].Render(d3ddev,D3DXVECTOR3((-x*1.48f)+8,(-y*1.40f)+11,0.0f));
			}
			else if(CurrentTetrisBoard.Blocks[x + y * 10].GetStatus() == DEAD)	
			{			
				ColouredBricks[CurrentTetrisBoard.Blocks[x + y * 10].GetColour()].Render(d3ddev,D3DXVECTOR3((-x*1.48f)+8,(-y*1.40f)+11,0.0f));
			}				
			//else if(CurrentTetrisBoard.Blocks[x + y * 10].GetStatus() == EMPTY)	
				
        }        
    }    

	//Render NEXT shape	
	for(int y =0;y<4;y++)
	{
		for(int x =0;x<4;x++)
		{
			if(CurrentTetrisBoard.NextShape.Blocks[0][x+ y*4].GetStatus() != EMPTY)
			{
				ColouredBricks[CurrentTetrisBoard.NextShape.Blocks[0][x+y*4].GetColour()].Render(d3ddev,D3DXVECTOR3((-x*1.48f)-14,(-y*1.40f)+18,0.0f));
				if(doNewPickTest)				
					picktest(ColouredBricks[CurrentTetrisBoard.NextShape.Blocks[0][x+y*4].GetColour()]);									
			}
		}
	}

	//Render fragments
	for(int i =0;i<CurrentTetrisBoard.Fragments.size();i++)
	{
		ColouredBricks[CurrentTetrisBoard.Fragments[i].Colour].Scale = 0.5f;
		ColouredBricks[CurrentTetrisBoard.Fragments[i].Colour].Yaw = CurrentTetrisBoard.Fragments[i].Yaw;
		ColouredBricks[CurrentTetrisBoard.Fragments[i].Colour].Pitch = CurrentTetrisBoard.Fragments[i].Pitch;
		ColouredBricks[CurrentTetrisBoard.Fragments[i].Colour].Roll = CurrentTetrisBoard.Fragments[i].Roll;

		ColouredBricks[CurrentTetrisBoard.Fragments[i].Colour].Render(d3ddev,
			D3DXVECTOR3((-CurrentTetrisBoard.Fragments[i].X*1.48f)+8,
			(-CurrentTetrisBoard.Fragments[i].Y*1.40)+11,
			CurrentTetrisBoard.Fragments[i].Z));

		ColouredBricks[CurrentTetrisBoard.Fragments[i].Colour].Scale = 1.0f;
		ColouredBricks[CurrentTetrisBoard.Fragments[i].Colour].Yaw = 0.0f;
		ColouredBricks[CurrentTetrisBoard.Fragments[i].Colour].Pitch = 0.0f;
		ColouredBricks[CurrentTetrisBoard.Fragments[i].Colour].Roll = 0.0f;
	}	

	//Render board Sides
	//BoardSide.ScalingFactor.x = 0.75f;
	//BoardSide.Scale = 0.55f;
	//BoardSide.ScalingFactor.y = 1.2f;
	//BoardSide.Render(d3ddev,D3DXVECTOR3(10.0f,-2.0f,0.0f));
	//BoardSide.Render(d3ddev,D3DXVECTOR3(-7.35f,-2.0f,0.0f));
	////Top and Bottom
	//BoardTop.ScalingFactor.x = 0.75f;
	//BoardTop.ScalingFactor.y = 0.7f;
	//BoardTop.Scale = 0.55f;
	//BoardTop.Roll = 1.570f;
	//BoardTop.Render(d3ddev,D3DXVECTOR3(1.55f,12.5f,0.0f));
	//BoardTop.Render(d3ddev,D3DXVECTOR3(1.55f,-17.0f,0.0f));

	//TETRIS OVER

	//JIM3D****************

	//grid	
	d3ddev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	d3ddev->SetRenderState(D3DRS_LIGHTING, FALSE);    
    render_grid();
	d3ddev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	d3ddev->SetRenderState(D3DRS_LIGHTING, TRUE);   


	//*********************

	//SPRITES		

	d3dspt->Begin(D3DXSPRITE_ALPHABLEND);    
	d3dspt->Begin(NULL);    // begin sprite drawing

	//Panel
	RenderTestPanel(UI_X(0)
				   ,UI_Y(0)
				   ,UI_X(20)
				   ,UI_Y(97));

	d3dspt->Draw(logo, NULL, &D3DXVECTOR3(0.0,0.0,0.0), &D3DXVECTOR3((float)SCREEN_WIDTH - 320.0,0.0,0.0) , D3DCOLOR_ARGB(255, 255, 255, 255));

	

	//RenderTestPanel(UI_X(45)
	//			   ,UI_Y(45)
	//			   ,UI_X(10)
	//			   ,UI_Y(10));	

	// TEXT

	//Display the game time

	unsigned long hours =0;
	unsigned long minutes =0;
	unsigned long seconds =0;

	hours = (currenttick - CurrentTetrisBoard.GameStartTick)/1000/60/60;
	minutes = (currenttick - CurrentTetrisBoard.GameStartTick)/1000/60 - 60*hours;
	seconds = (currenttick - CurrentTetrisBoard.GameStartTick)/1000 - 60*minutes;
	
	string buffer = (string)"Time   ";
	if(hours>9) buffer+= to_string(hours); else buffer += "0" + to_string(hours);
	if(minutes>9) buffer += ":" +to_string(minutes); else buffer += ":0" +to_string(minutes);
	if(seconds>9) buffer += ":" +to_string(seconds); else buffer += ":0" +to_string(seconds);		

	draw_simple_text(buffer,75,75);

	buffer = (string)"Level  " + to_string(CurrentTetrisBoard.Level);		
	draw_simple_text(buffer,75,100);

	buffer = (string)"Lines  " + to_string(CurrentTetrisBoard.Lines);		
	draw_simple_text(buffer,75,125);

	buffer = (string)"Score  " + to_string(CurrentTetrisBoard.Score);		
	draw_simple_text(buffer,75,175);

	buffer = (string)"High Scores";		
	draw_simple_text(buffer,75,225);

	//DEBUG highscores
	SortHighScorePair(&Tetris_HighScores);
	for(int scoreloop = 0;scoreloop < Tetris_HighScores.size() && scoreloop < 10;scoreloop++)
	{
		buffer = to_string(scoreloop+1) + (string)" " + Tetris_HighScores[scoreloop].n + (string)" " + to_string(Tetris_HighScores[scoreloop].s);
		draw_simple_text(buffer,75,250+(scoreloop*25));
	}

	//buffer = (string)"DEBUG: live fragments:  " + to_string(CurrentTetrisBoard.Fragments.size());		
	//draw_simple_text(buffer,75,250);
	//

	////FPS
	//float elapsed = (float)currenttick - (float)last_frame;
	//elapsed = elapsed /1000;
	//float fps = 1 / elapsed;

	//buffer = (string)"DEBUG: FPS:  " + to_string(fps);		
	//draw_simple_text(buffer,75,300);

	//DEBUG MOUSE POS
	//buffer = (string)"DEBUG: MouseX:  " + to_string(mouseCurrentX);		
	//draw_simple_text(buffer,75,700);
	//buffer = (string)"DEBUG: MouseY:  " + to_string(mouseCurrentY);		
	//draw_simple_text(buffer,75,725);

	////DEBUG PICKING
	//buffer = (string)"DEBUG: MouseRayOriginX:  " + to_string(mouserayOrigin.x);		
	//draw_simple_text(buffer,75,775);
	//buffer = (string)"DEBUG: MouseRayOriginY:  " + to_string(mouserayOrigin.y);		
	//draw_simple_text(buffer,75,800);
	//buffer = (string)"DEBUG: MouseRayOriginZ:  " + to_string(mouserayOrigin.z);		
	//draw_simple_text(buffer,75,825);

	//buffer = (string)"DEBUG: MouseRayDirectionX:  " + to_string(mouserayDir.x);		
	//draw_simple_text(buffer,75,875);
	//buffer = (string)"DEBUG: MouseRayDirectionY:  " + to_string(mouserayDir.y);		
	//draw_simple_text(buffer,75,900);
	//buffer = (string)"DEBUG: MouseRayDirectionZ:  " + to_string(mouserayDir.z);		
	//draw_simple_text(buffer,75,925);

	if(hits.length() >0)
	{
		buffer = hits;		//HITS
		draw_simple_text(buffer,75,975);
	}

	//DEBUG TIMER
	//buffer = (string)"DEBUG: Ticks:  " + to_string(GameTicks);		
	//draw_simple_text(buffer,75,775);	

	//buffer = (string)"DEBUG: MouseZ:  " + to_string(mouseCurrentZ);		
	//draw_simple_text(buffer,75,750);

	//buffer = (string)"DEBUG: Xdiff:  " + to_string(mouseX);		
	//draw_simple_text(buffer,75,800);
	//buffer = (string)"DEBUG: Ydiff:  " + to_string(mouseY);		
	//draw_simple_text(buffer,75,825);
	//buffer = (string)"DEBUG: Zdiff:  " + to_string(mouseZ);		
	//draw_simple_text(buffer,75,850);

	//DEBUG CAMERA

	//buffer = (string)"DEBUG: CameraX: " + to_string(vector_camera.x);
	//draw_simple_text(buffer,75,900);
	//buffer = (string)"DEBUG: CameraY: " + to_string(vector_camera.y);
	//draw_simple_text(buffer,75,925);
	//buffer = (string)"DEBUG: CameraZ: " + to_string(vector_camera.z);
	//draw_simple_text(buffer,75,950);

	//buffer = (string)"DEBUG: LookX: " + to_string(vector_position.x);
	//draw_simple_text(buffer,75,1000);
	//buffer = (string)"DEBUG: LookY: " + to_string(vector_position.y);
	//draw_simple_text(buffer,75,1025);
	//buffer = (string)"DEBUG: LookZ: " + to_string(vector_position.z);
	//draw_simple_text(buffer,75,1050);	


	if(GAMESTATE == GAMEOVER)
	{	
		buffer = (string)"(press ESC to quit)";
		draw_simple_text(buffer,275,475);
	}

	if(GAMESTATE == GAMEOVER)
	{
		buffer = (string)"GAME OVER :(";
		static RECT textbox; 
		SetRect(&textbox, 270, 400, SCREEN_WIDTH, 500); 	

		dxfontBIG->DrawTextA(NULL,
						  buffer.c_str(),
						  buffer.size(),
						  &textbox,
						  DT_LEFT | DT_TOP,
						  D3DCOLOR_ARGB(255, 255, 255, 255));
		
	}	

	/*if(CurrentTetrisBoard.Fragments.size() >1)
	{
		buffer = (string)"DEBUG: frag[0]:  " + 
			(string)"X:" + to_string((int)CurrentTetrisBoard.Fragments[0].X) + " " +
			(string)"Y:" + to_string((int)CurrentTetrisBoard.Fragments[0].Y) + " " +
			(string)"Z:" + to_string((int)CurrentTetrisBoard.Fragments[0].Z) + " " +
			(string)"D:" + to_string((int)CurrentTetrisBoard.Fragments[0].falling);			
			
		draw_simple_text(buffer,75,275);
	}*/

	buffer = (string)"Next Shape";		
	draw_simple_text(buffer,SCREEN_WIDTH-925,175);

	// END OF TEXT

	//mouse cursor AFTER TEXT
    D3DXVECTOR3 center(0.0f, 0.0f, 0.0f);    // center at the upper-left corner
    D3DXVECTOR3 position((float)mouseCurrentX, (float)mouseCurrentY, 0.0f);    
    d3dspt->Draw(mouse_sprite, NULL, &center, &position, D3DCOLOR_ARGB(255, 255, 255, 255));	



    d3dspt->End();    // end sprite drawing

    d3ddev->EndScene(); 
    
	d3ddev->Present(NULL, NULL, NULL, NULL);


    return;

}


// this is the function that cleans up Direct3D and COM
void cleanD3D(void)
{
    meshSpaceship->Release();    // close and release the spaceship mesh
	mouse_sprite->Release();
//	logo->Release();
	panel32_sprite[0]->Release();
	panel32_sprite[1]->Release();
	panel32_sprite[2]->Release();
	panel32_sprite[3]->Release();
	panel32_sprite[4]->Release();
	panel32_sprite[5]->Release();
	panel32_sprite[6]->Release();
	panel32_sprite[7]->Release();
	panel32_sprite[8]->Release();
    d3ddev->Release();    // close and release the 3D device
    d3d->Release();    // close and release Direct3D

    return;
}


// this is the function that puts the 3D models into video RAM
void init_graphics(void)
{
    LPD3DXBUFFER bufShipMaterial;

    D3DXLoadMeshFromX(L"BOX_ORANGE.x",    // load this file
                      D3DXMESH_SYSTEMMEM,    // load the mesh into system memory
                      d3ddev,    // the Direct3D Device
                      NULL,    // we aren't using adjacency
                      &bufShipMaterial,    // put the materials here
                      NULL,    // we aren't using effect instances
                      &numMaterials,    // the number of materials in this model
                      &meshSpaceship);    // put the mesh here

    // retrieve the pointer to the buffer containing the material information
    D3DXMATERIAL* tempMaterials = (D3DXMATERIAL*)bufShipMaterial->GetBufferPointer();

    // create a new material buffer and texture for each material in the mesh
    material = new D3DMATERIAL9[numMaterials];
    texture = new LPDIRECT3DTEXTURE9[numMaterials];

    for(DWORD i = 0; i < numMaterials; i++)    // for each material...
    {
        material[i] = tempMaterials[i].MatD3D;    // get the material info
        material[i].Ambient = material[i].Diffuse;    // make ambient the same as diffuse
        // if there is a texture to load, load it
        if(FAILED(D3DXCreateTextureFromFileA(d3ddev,
                                             tempMaterials[i].pTextureFilename,
                                             &texture[i])))
        texture[i] = NULL;    // if there is no texture, set the texture to NULL
      }

    return;
}


// this is the function that sets up the lights and materials
void init_light(void)
{
    D3DLIGHT9 light;    // create the light struct

    ZeroMemory(&light, sizeof(light));    // clear out the struct for use
    light.Type = D3DLIGHT_DIRECTIONAL;    // make the light type 'directional light'
    light.Diffuse.r = 0.5f;    // .5 red
    light.Diffuse.g = 0.5f;    // .5 green
    light.Diffuse.b = 0.5f;    // .5 blue
    light.Diffuse.a = 1.0f;    // full alpha (we'll get to that soon)

    D3DVECTOR vecDirection = {-1.0f, -0.3f, -1.0f};    // the direction of the light
    light.Direction = vecDirection;    // set the direction

    d3ddev->SetLight(0, &light);    // send the light struct properties to light #0
    d3ddev->LightEnable(0, TRUE);    // turn on light #0

	D3DLIGHT9 light2;    // create the light struct

    ZeroMemory(&light2, sizeof(light2));    // clear out the struct for use
    light2.Type = D3DLIGHT_POINT;    // make the light type 'point light'
    light2.Diffuse.r = 0.5f;    // .5 red
    light2.Diffuse.g = 0.5f;    // .5 green
    light2.Diffuse.b = 0.5f;    // .5 blue
    light2.Diffuse.a = 1.0f;    // full alpha (we'll get to that soon)
    light2.Range = 100.0f;    // a range of 100
    light2.Attenuation0 = 0.0f;    // no constant inverse attenuation
    light2.Attenuation1 = 0.125f;    // only .125 inverse attenuation
    light2.Attenuation2 = 0.0f;    // no square inverse attenuation

    D3DVECTOR vecPosition = {0.0f, 0.0f, 25.0f};    // the position of the light
    light2.Position = vecPosition;    // set the position

    d3ddev->SetLight(1, &light2);    // send the light struct properties to light #0
    d3ddev->LightEnable(1, TRUE);    // turn on light #0

    return;
}

//Render a grid
void render_grid()
{
    static bool InitNeeded = true;
    static int GridSize = 130;
    static D3DXMATRIX matIdentity;

    d3ddev->SetFVF(GRIDFVF);

    if(InitNeeded)
    {
        d3ddev->CreateVertexBuffer(sizeof(GRIDVERTEX) * (GridSize * 9 + 64),
                                   0,
                                   GRIDFVF,
                                   D3DPOOL_MANAGED,
                                   &g_buffer,
                                   0);

        GRIDVERTEX* pGridData = 0;
        g_buffer->Lock(0, 0, (void**)&pGridData, 0);

        int index = 0;
        for(; index <= GridSize * 4 + 1; index++)
        {
            float x = (index % 2) ? GridSize : -GridSize;
            float y = -22.0f;
            float z = index / 2 - GridSize;

            pGridData[index].position = D3DXVECTOR3(x, y, z);
            pGridData[index].color = D3DXCOLOR(0.2f, 0.2f, 0.2f, 1.0f);
        }

        for(; index <= GridSize * 8 + 4; index++)
        {
            static int half = index;
            float x = (index - half) / 2 - GridSize;
            float y = -22.0f;
            float z = (index % 2) ? -GridSize : GridSize;

            pGridData[index].position = D3DXVECTOR3(x, y, z);
            pGridData[index].color = D3DXCOLOR(0.2f, 0.2f, 0.2f, 1.0f);
        }	

		pGridData[index].position = D3DXVECTOR3(pGridData[index-1].position.x, pGridData[index-1].position.y, pGridData[index-1].position.z);
        pGridData[index].color = D3DXCOLOR(0.4f, 0.4f, 0.4f, 1.0f);

		//tetrisgrid
		for(int y =0;y<21;y++)
		{
			index++;
			pGridData[index].position = D3DXVECTOR3(8.79f,(-y*1.40f)+11.88,0.0f);		
			pGridData[index].color = D3DXCOLOR(0.3f, 0.6f/(21.0f-(float)y), 0.2f, 1.0f);
			index++;
			pGridData[index].position = D3DXVECTOR3((-10*1.48f)+8.79f,(-y*1.40f)+11.88,0.0f);			
			pGridData[index].color = D3DXCOLOR(0.3f, 0.6f, 0.2f, 1.0f);
		}
		for(int x=0;x<11;x++)
		{
			index++;
			pGridData[index].position = D3DXVECTOR3((-x*1.48f)+8.79,11.88,0.0f);
			pGridData[index].color = D3DXCOLOR(0.3f, 0.6f/(11.0f-(float)x), 0.2f, 1.0f);
			index++;
			pGridData[index].position = D3DXVECTOR3((-x*1.48f)+8.79,(-20*1.40f)+11.88,0.0f);			
			pGridData[index].color = D3DXCOLOR(0.3f, 0.6f, 0.2f, 1.0f);
		}		

        g_buffer->Unlock();

        D3DXMatrixIdentity(&matIdentity);
        InitNeeded = false;
    }

    d3ddev->SetTransform(D3DTS_WORLD, &matIdentity);
    d3ddev->SetStreamSource(0, g_buffer, 0, sizeof(GRIDVERTEX));
    d3ddev->SetTexture(0, NULL);
    d3ddev->DrawPrimitive(D3DPT_LINELIST, 0, GridSize * 4 + 2 + 34);

    return;
}

// this is the function that detets mouse movements and mouse buttons
void detect_mousepos(void)
{
    static DIMOUSESTATE mousestate;    // create a static storage for the mouse-states

    dinmouse->Acquire();    // get access if we don't have it already

    // fill the mousestate with values
    dinmouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&mousestate);

	long x=0,y=0,z=0;
	x = mousestate.lX;
	y = mousestate.lY;
	z = -mousestate.lZ;

	if(x != 0)
		mouseX = x;
	if(y != 0)
		mouseY = y;
	if(z != 0)
		mouseZ = z;		

	if (mousestate.rgbButtons[0] & 0x80 || z !=0)
	{		
		set_mouse_cursor(true);		
		third_person_look((float)x/2,(float)y/2,(float)z,(float)mouseCurrentZ,(mousestate.rgbButtons[0] & 0x80));		
	}
	else
	{
		set_mouse_cursor(false);
		if(mouseCurrentX + x >0 && mouseCurrentX + x < SCREEN_WIDTH)
			mouseCurrentX += x;
		if(mouseCurrentY + y >0 && mouseCurrentY + y < SCREEN_HEIGHT)
			mouseCurrentY += y;			
	}

	if(mouseCurrentZ + z >0 && mouseCurrentZ +z <1000)
			mouseCurrentZ += z;	

	if(mousestate.rgbButtons[1] & 0x80)
	{		
		if(!pickbuttondown)
			mouseray_test();

		pickbuttondown = true;
	}
	else
	{
		pickbuttondown = false;
	}

    return;
} 

void set_mouse_cursor(bool looking)
{
	if(mouselooking && !looking)
	{
		D3DXCreateTextureFromFile(d3ddev, L"mouse.png", &mouse_sprite);
	}		
	if(!mouselooking && looking)
	{
		D3DXCreateTextureFromFile(d3ddev, L"mouse_looking.png", &mouse_sprite);
	}

	mouselooking = looking;
}

void third_person_look(float x, float y, float z,float zoom,bool rotatebutton)
{
	//code from c#
	/*
			_target = _target - _position;
            _target.TransformCoordinate(Matrix.RotationX(rotx * ((float)Math.PI / 180)));
            horizontalaxis = _target;
            horizontalaxis.TransformCoordinate(Matrix.RotationX(90 * ((float)Math.PI / 180)));
            horizontalaxis.X = 0;
            _target.TransformCoordinate(Matrix.RotationAxis(horizontalaxis, roty * ((float)Math.PI / 180)));
            _target = _target + _position;
	*/	

	// z is the change in Z
	// zoom is the current Z

	float zratio =0.0f;

	if((z < -1 || z > 1))
	{		
		if( (z+zoom < -1 || z+zoom > 1) && (zoom+z >0 && zoom+z <1000))	
		{
			//translate to origin
			vector_camera = vector_camera - vector_position;

			zratio = (z/zoom);		
			vector_camera.x += vector_camera.x * zratio;
			vector_camera.y += vector_camera.y * zratio;
			vector_camera.z += vector_camera.z * zratio;

			//translate to world
			vector_camera = vector_camera + vector_position;
		}		
	}

	if((x!=0 || y!=0) && rotatebutton)
	{
		//translate to origin
		vector_camera = vector_camera - vector_position;

		D3DXVECTOR3 newXaxis = D3DXVECTOR3();
		D3DXMATRIX matTransform = D3DMATRIX();
		D3DXMATRIX matScale = D3DMATRIX();
		D3DXMATRIX matRotateY = D3DMATRIX();
		D3DXMATRIX matRotateX = D3DMATRIX();
		D3DXMATRIX matRotateOnYBy90 = D3DMATRIX();

		//Define Identity
		D3DXMatrixIdentity(&matTransform);	
		//Define Scaling
		D3DXMatrixScaling(&matScale,1.0f,1.0f,1.0f);
		//Define RotationY : x movement rotates around the Y axis	
		D3DXMatrixRotationY(&matRotateY,x * (MATHS_PI /180));		

		//Apply Scaling
		D3DXMatrixMultiply(&matTransform,&matTransform,&matScale);
		//Apply Rotation
		D3DXMatrixMultiply(&matTransform,&matTransform,&matRotateY);	
				
		//Apply transform
		D3DXVec3TransformCoord(&vector_camera,&vector_camera,&matTransform);		

		// NOW THE NEW X AXIS

		//Define Identity
		matTransform = D3DMATRIX();
		D3DXMatrixIdentity(&matTransform);	

		//Get the new x axis
		newXaxis = vector_camera;
		D3DXMatrixRotationY(&matRotateOnYBy90,90 * (MATHS_PI /180));	
		D3DXMatrixMultiply(&matRotateOnYBy90,&matScale,&matRotateOnYBy90);
		D3DXVec3TransformCoord(&newXaxis,&newXaxis,&matRotateOnYBy90);
		newXaxis.y =0; //y of target
		
		//Define RotationX : y movement rotates around the *NEW* X axis	
		D3DXMatrixRotationAxis(&matRotateX,&newXaxis,-y * (MATHS_PI /180));

		//Apply Scaling
		D3DXMatrixMultiply(&matTransform,&matTransform,&matScale);
		//Apply Rotation	
		D3DXMatrixMultiply(&matTransform,&matTransform,&matRotateX);

		//Apply transform
		D3DXVec3TransformCoord(&vector_camera,&vector_camera,&matTransform);

		//translate to world
		vector_camera = vector_camera + vector_position;
	}				
}

void mouseray_test()
{
	float w;
	float h;
	float sx = (float)mouseCurrentX;
	float sy = (float)mouseCurrentY;
	D3DXMATRIX matProj = D3DMATRIX();
	D3DXMATRIX matView = D3DMATRIX();
	D3DVIEWPORT9 m_mainViewport = D3DVIEWPORT9();

	d3ddev->GetTransform(D3DTS_PROJECTION,&matProj);
	d3ddev->GetTransform(D3DTS_VIEW,&matView);
	d3ddev->GetViewport(&m_mainViewport);

	w = (float)m_mainViewport.Width;
	h = (float)m_mainViewport.Height;

	D3DXVECTOR3 v;
	v.x =  ( ( ( 2.0f * sx ) / w  ) - 1 ) / matProj._11;
	v.y = -( ( ( 2.0f * sy ) / h ) - 1 ) / matProj._22;
	v.z =  1.0f;

	D3DXMATRIX m;	

	D3DXMatrixInverse( &m, NULL, &matView );

	// Transform the screen space pick ray into 3D space
	mouserayDir.x  = v.x*m._11 + v.y*m._21 + v.z*m._31;
	mouserayDir.y  = v.x*m._12 + v.y*m._22 + v.z*m._32;
	mouserayDir.z  = v.x*m._13 + v.y*m._23 + v.z*m._33;
	mouserayOrigin.x = m._41;
	mouserayOrigin.y = m._42;
	mouserayOrigin.z = m._43;

	hits = "";
	doNewPickTest = true;
}

bool picktest(JIM3DEntity m_ent)
{	
	// Use inverse of matrix
	D3DXMATRIX matInverse;
	D3DXMatrixInverse(&matInverse,NULL,&m_ent.WorldMatrix);

	// Transform ray origin and direction by inv matrix
	D3DXVECTOR3 rayObjOrigin,rayObjDirection;

	D3DXVec3TransformCoord(&rayObjOrigin,&mouserayOrigin,&matInverse);
	D3DXVec3TransformNormal(&rayObjDirection,&mouserayDir,&matInverse);
	D3DXVec3Normalize(&rayObjDirection,&rayObjDirection);

	BOOL hasHit;
	float distanceToCollision;

	D3DXIntersect((LPD3DXBASEMESH)m_ent.Mesh, &rayObjOrigin, &rayObjDirection, &hasHit, NULL, NULL, NULL, &distanceToCollision, NULL, NULL);

	if(hasHit)
		hits += "Picked:" + m_ent.EntityName + " AT:" + 
			to_string(m_ent.LocationVector.x)+":"+
			to_string(m_ent.LocationVector.y)+":"+		
			to_string(m_ent.LocationVector.z)+" Dist.:"+
			to_string(distanceToCollision)+"\n";

	return hasHit;
}

void IncrementTimer()
{
	DWORD now = GetTickCount();
	DWORD inc = now - GameLastTick;
	GameTicks += inc;
	GameLastTick = now;
}

DWORD GetGameTick()
{
	return GameTicks;
}

void TogglePause()
{
	DWORD now = GetTickCount();
	if(now-lastpaused >= 250)
	{
		if(GAMESTATE == PAUSE)
		{
			GameLastTick = now;
			GAMESTATE = PLAY;
		}
		else if (GAMESTATE == PLAY)
			GAMESTATE = PAUSE;

		lastpaused = now;
	}
}

void ResetDevice()
{
	WriteDebugToString((string)"Executing ResetDevice");
    if(d3ddev)
	{
		WriteDebugToString((string)"LostSprite");
		d3dspt->OnLostDevice();
		WriteDebugToString((string)"LostFont");
        dxfont->OnLostDevice();

		WriteDebugToString((string)"ResetDevice");
        d3ddev->Reset(&d3dpp);

		WriteDebugToString((string)"ResetSprite");
        d3dspt->OnResetDevice();
		WriteDebugToString((string)"ResetFont");
        dxfont->OnResetDevice();
	}

	return;
}

void WriteDebugToString(string debugmessage)
{
	if(!debugoutput)	
		debugstring += "  \r\n:  " + debugmessage;	
}

void WriteDebugToFile(string debugmessage)
{
	if(!debugoutput)
	{
		FILE*	SaveFile;	

		SaveFile = fopen("DEBUG.TXT", "wb"); 	
		
		fwrite (debugmessage.c_str(), sizeof(const char), debugmessage.size(), SaveFile);	

		fclose(SaveFile);
		debugoutput = true;
	}
}

void RenderHeaderPanel(int x,int y, int width, int height,string header)
{
//	RenderTestPanel( x, y+32,  width,  height-1);
//	RenderTestPanel(x+width/3,int y, int width/3, 1);
}

void RenderTestPanel(int x,int y, int width, int height)
{
	//0 = back, 1 = left, 2 = top, 3 = right, 4 = bottom, 5 = topleft, 6 =topright, 7 = bottomright 8= bottomleft	

	//transparent back	
    D3DXVECTOR3 panelposition(x,y, 0.5f);
	D3DXVECTOR3 center(0.0f,0.0f,0.0f);

	width+=2;	// account for bounding
	height+=2;

	int panely =0;
	int panelx=0;
	for(int o = 0;o<width*height;o++)
	{		
		panely = o/width;		
		panelx = o-(panely*width);
		panelposition.x = x*32 + (32*panelx);
		panelposition.y = y*32 + (32*panely);		

		//topleft
		if(o==0)
			d3dspt->Draw(panel32_sprite[5], NULL, &center, &panelposition, D3DCOLOR_ARGB(255, 255, 255, 255));
		//top
		if(panely==0 && panelx>0 && panelx<width-1)
			d3dspt->Draw(panel32_sprite[2], NULL, &center, &panelposition, D3DCOLOR_ARGB(255, 255, 255, 255));
		//topright
		if(panely==0 && panelx==width-1)
			d3dspt->Draw(panel32_sprite[6], NULL, &center, &panelposition, D3DCOLOR_ARGB(255, 255, 255, 255));
		//left
		if(panely>0 && panely<height-1 && panelx ==0)
			d3dspt->Draw(panel32_sprite[1], NULL, &center, &panelposition, D3DCOLOR_ARGB(255, 255, 255, 255));
		//right
		if(panely>0 && panely<height-1 && panelx ==width-1)
			d3dspt->Draw(panel32_sprite[3], NULL, &center, &panelposition, D3DCOLOR_ARGB(255, 255, 255, 255));
		//bottomleft
		if(panely==height-1 && panelx ==0)
			d3dspt->Draw(panel32_sprite[8], NULL, &center, &panelposition, D3DCOLOR_ARGB(255, 255, 255, 255));
		//bottom
		if(panely==height-1 && panelx>0 && panelx<width-1)
			d3dspt->Draw(panel32_sprite[4], NULL, &center, &panelposition, D3DCOLOR_ARGB(255, 255, 255, 255));
		//bottomright
		if(panely==height-1 && panelx ==width-1)
			d3dspt->Draw(panel32_sprite[7], NULL, &center, &panelposition, D3DCOLOR_ARGB(255, 255, 255, 255));
		//back
		if(panely != 0 && panelx != 0 && panely != height-1 && panelx != width-1)
			d3dspt->Draw(panel32_sprite[0], NULL, &center, &panelposition, D3DCOLOR_ARGB(127, 255, 255, 255));
	}
}

void LoadTestPanelSprites()
{
	D3DXCreateTextureFromFileEx(d3ddev,L"panel32.png",D3DX_DEFAULT,D3DX_DEFAULT,D3DX_DEFAULT,NULL,D3DFMT_A8R8G8B8,D3DPOOL_MANAGED,D3DX_DEFAULT,D3DX_DEFAULT,D3DCOLOR_XRGB(255, 0, 255),    
                                NULL,NULL,&panel32_sprite[0]);    
	D3DXCreateTextureFromFileEx(d3ddev,L"panel32_leftend.png",D3DX_DEFAULT,D3DX_DEFAULT,D3DX_DEFAULT,NULL,D3DFMT_A8R8G8B8,D3DPOOL_MANAGED,D3DX_DEFAULT,D3DX_DEFAULT,D3DCOLOR_XRGB(255, 0, 255),    
                                NULL,NULL,&panel32_sprite[1]);
	D3DXCreateTextureFromFileEx(d3ddev,L"panel32_topend.png",D3DX_DEFAULT,D3DX_DEFAULT,D3DX_DEFAULT,NULL,D3DFMT_A8R8G8B8,D3DPOOL_MANAGED,D3DX_DEFAULT,D3DX_DEFAULT,D3DCOLOR_XRGB(255, 0, 255),    
                                NULL,NULL,&panel32_sprite[2]);
	D3DXCreateTextureFromFileEx(d3ddev,L"panel32_rightend.png",D3DX_DEFAULT,D3DX_DEFAULT,D3DX_DEFAULT,NULL,D3DFMT_A8R8G8B8,D3DPOOL_MANAGED,D3DX_DEFAULT,D3DX_DEFAULT,D3DCOLOR_XRGB(255, 0, 255),    
                                NULL,NULL,&panel32_sprite[3]);
	D3DXCreateTextureFromFileEx(d3ddev,L"panel32_bottomend.png",D3DX_DEFAULT,D3DX_DEFAULT,D3DX_DEFAULT,NULL,D3DFMT_A8R8G8B8,D3DPOOL_MANAGED,D3DX_DEFAULT,D3DX_DEFAULT,D3DCOLOR_XRGB(255, 0, 255),    
                                NULL,NULL,&panel32_sprite[4]);
	D3DXCreateTextureFromFileEx(d3ddev,L"panel32_topleft.png",D3DX_DEFAULT,D3DX_DEFAULT,D3DX_DEFAULT,NULL,D3DFMT_A8R8G8B8,D3DPOOL_MANAGED,D3DX_DEFAULT,D3DX_DEFAULT,D3DCOLOR_XRGB(255, 0, 255),    
                                NULL,NULL,&panel32_sprite[5]);
	D3DXCreateTextureFromFileEx(d3ddev,L"panel32_topright.png",D3DX_DEFAULT,D3DX_DEFAULT,D3DX_DEFAULT,NULL,D3DFMT_A8R8G8B8,D3DPOOL_MANAGED,D3DX_DEFAULT,D3DX_DEFAULT,D3DCOLOR_XRGB(255, 0, 255),    
                                NULL,NULL,&panel32_sprite[6]);
	D3DXCreateTextureFromFileEx(d3ddev,L"panel32_bottomright.png",D3DX_DEFAULT,D3DX_DEFAULT,D3DX_DEFAULT,NULL,D3DFMT_A8R8G8B8,D3DPOOL_MANAGED,D3DX_DEFAULT,D3DX_DEFAULT,D3DCOLOR_XRGB(255, 0, 255),    
                                NULL,NULL,&panel32_sprite[7]);
	D3DXCreateTextureFromFileEx(d3ddev,L"panel32_bottomleft.png",D3DX_DEFAULT,D3DX_DEFAULT,D3DX_DEFAULT,NULL,D3DFMT_A8R8G8B8,D3DPOOL_MANAGED,D3DX_DEFAULT,D3DX_DEFAULT,D3DCOLOR_XRGB(255, 0, 255),    
                                NULL,NULL,&panel32_sprite[8]);

	//Load Logo
	D3DXCreateTextureFromFileEx(d3ddev,L"tetriiik.png",320,128,D3DX_DEFAULT,NULL,D3DFMT_A8R8G8B8,D3DPOOL_MANAGED,D3DX_DEFAULT,D3DX_DEFAULT,D3DCOLOR_XRGB(255, 255, 255),    
                                NULL,NULL,&logo);
}

int UI_X(float percentage)
{
	return ((SCREEN_WIDTH*(percentage/100))/32);
}

int UI_Y(float percentage)
{
	return ((SCREEN_HEIGHT*(percentage/100))/32);
}

