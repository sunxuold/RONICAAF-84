/* AF-84 game simulator, code file for SDL 1.2 and 2.0
 *
 * Copyright (C) 2021 Sunxu <xusun521@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <stdbool.h>
#include <string.h>

#if defined SDL_1
#  define SDL_VER_STR "1.2"
#  include "SDL/SDL.h"
#  define SDL_SCREEN_TYPE SDL_Surface*
#  define SDL_RASTER_TYPE SDL_Surface*
#elif defined SDL_2
#  define SDL_VER_STR "2.0"
#  include "SDL2/SDL.h"
#  define SDL_SCREEN_TYPE SDL_Window*
//#  define SDL_RASTER_TYPE SDL_Texture*
#else
#  error "neither SDL_1 nor SDL_2 is defined"
#endif

//#include "SDL_ttf.h"

SDL_Surface* Screen;
#ifndef SDL_1
//SDL_Renderer* Renderer;
//SDL_Haptic* HapticDevice;
SDL_Window* mainwindows;
#else
//Shake_Effect HapticEffect;	
//Shake_Device *HapticDevice;
int id;
#endif

SDL_Surface* RGBSurface = NULL;

/* - - - DATA DEFINITIONS - - - */

bool debugcheck = false;

//处理普通飞机移动模式
bool handlePlan();

//处理小船逻辑
bool handleShip();

//初始化游戏数据
void initMainGame( bool isintinal);

//飞机图片索引，从左67.5度开始计算
SDL_Surface* planmap1 = NULL; //67.5
SDL_Surface* planmap2 = NULL; //45
SDL_Surface* planmap3 = NULL; //22.5
SDL_Surface* planmap4 = NULL; //0
SDL_Surface* planmap5 = NULL; //-22.5
SDL_Surface* planmap6 = NULL; //-45
SDL_Surface* planmap7 = NULL; //-67.5
SDL_Surface* planshoot = NULL;//击中飞机
SDL_Surface* planboom = NULL; //飞机爆炸

SDL_Surface* helicopter = NULL;//营救直升机

SDL_Surface* scoremap = NULL; // SCORE显示
SDL_Surface* levelmap = NULL; // LEVEL显示

SDL_Surface* batterymap = NULL;  // 炮台
SDL_Surface* batteryshoot = NULL; // 炮台发射

SDL_Surface* shipmap = NULL;   //小船
SDL_Surface* shipboom = NULL;  //小船爆炸

SDL_Surface* backgroundmap = NULL; //大背景图

SDL_Surface* digitialvalue = NULL; //五位数字显示

SDL_Surface* emptyImgae = NULL; //空白图

SDL_Surface* LifeCountImage = NULL; //生命计数图

SDL_Surface* GameOverImage = NULL; //GameOver图

SDL_Surface* SuspendImage = NULL; //暂停游戏图
SDL_Surface* MuteImage = NULL; //静音标志图
SDL_Surface* Input1Image = NULL; //输入1图
SDL_Surface* Input0Image = NULL; //输入0图


//int plan1Table[1,2,3,4];//第一列飞机图像索引，0表示不显示
//int plan2Table[2,3,4,1];//第二列飞机图像索引，0表示不显示
//int plan3Table[2,5,6,3];//第三列飞机图像索引，0表示不显示
//int freeplanTable[1,2,3,4,5,6,7];//自由飞机图像索引，0表示不显示
SDL_Surface* DigtialNumber[11]={NULL};

#include "SDL_image.h"
//加载图像文件到surface
void LoadSurfacefromFile(SDL_Surface** desSurface, const char* filename)
{	
	SDL_Surface* tmppngsurf = NULL;
	
	SDL_ClearError();
	
	tmppngsurf = IMG_Load(filename);	
	
	if(tmppngsurf)
	{		
#if defined SDL_1
		(*desSurface)=SDL_ConvertSurface(tmppngsurf, Screen->format, SDL_SRCCOLORKEY|SDL_RLEACCEL);			
		Uint32 colorkey = *((Uint32*)(*desSurface)->pixels);		
		SDL_SetColorKey((*desSurface), SDL_SRCCOLORKEY|SDL_RLEACCEL, colorkey);
#elif defined SDL_2		
		(*desSurface) = SDL_ConvertSurfaceFormat(tmppngsurf, Screen->format->format, 0);	
		Uint32 colorkey = *((Uint32*)(*desSurface)->pixels);
		SDL_SetColorKey((*desSurface), 1, colorkey);	
#else
#  error "neither SDL_1 nor SDL_2 is defined"
#endif						
		SDL_FreeSurface(tmppngsurf);	
		
		if(debugcheck) printf("LoadSurfacefromFile OK load :%s, res:%dx%d  error:%s \n", filename, (*desSurface)->w, (*desSurface)->h, SDL_GetError());
	}
	else
		printf("Couldn't LoadSurfacefromFile:%s, error:%s\n", filename, SDL_GetError());				
}

//初始化图像内容
void initVideoSurface()
{
	LoadSurfacefromFile(&planmap1, "Plan1.png");
	LoadSurfacefromFile(&planmap2, "Plan2.png");
	LoadSurfacefromFile(&planmap3, "Plan3.png");
	LoadSurfacefromFile(&planmap4, "Plan4.png");
	LoadSurfacefromFile(&planmap5, "Plan5.png");
	LoadSurfacefromFile(&planmap6, "Plan6.png");
	LoadSurfacefromFile(&planmap7, "Plan7.png");
	LoadSurfacefromFile(&planshoot, "PlanShoot.png");
	LoadSurfacefromFile(&planboom, "PlanBoom.png");
	
	LoadSurfacefromFile(&helicopter, "Helicopter.png");
	
	LoadSurfacefromFile(&scoremap, "Score.png");
	LoadSurfacefromFile(&levelmap, "Level.png");
	
	LoadSurfacefromFile(&batterymap, "Battery.png");
	LoadSurfacefromFile(&batteryshoot, "BatteryShoot.png");
	
	
	LoadSurfacefromFile(&shipmap, "Ship.png");
	LoadSurfacefromFile(&shipboom, "ShipBoom.png");
		
	LoadSurfacefromFile(&digitialvalue, "Digtal.png");		
	
	
	LoadSurfacefromFile(&backgroundmap, "MainBack.png");

	LoadSurfacefromFile(&LifeCountImage, "Life.png");

	LoadSurfacefromFile(&GameOverImage, "GameOver.png");
	//SDL_Surface* tmppngsurf = IMG_Load("MainBack.png");
	//backgroundmap = SDL_ConvertSurface(tmppngsurf, Screen->format, SDL_RLEACCEL);
	//SDL_SetAlpha(backgroundmap, 0, 255);	
	//SDL_FreeSurface(tmppngsurf);	
	char DigName[16];
	for(int i= 0;i<10;i++)
	{
		sprintf(DigName,"%d.png",i);
		LoadSurfacefromFile(&(DigtialNumber[i]), DigName);
	}
	LoadSurfacefromFile(&(DigtialNumber[10]), "Heng.png");
	
	LoadSurfacefromFile(&SuspendImage, "Suspend.png");
	LoadSurfacefromFile(&MuteImage, "Mute.png");
	LoadSurfacefromFile(&Input1Image, "Input1.png");
	LoadSurfacefromFile(&Input0Image, "Input0.png");
	
}



//卸载图片资源
void unloadVideoSurface()
{
	if(planmap1)  SDL_FreeSurface(planmap1);
	if(planmap2)  SDL_FreeSurface(planmap2);
	if(planmap3)  SDL_FreeSurface(planmap3);
	if(planmap4)  SDL_FreeSurface(planmap4);
	if(planmap5)  SDL_FreeSurface(planmap5);
	if(planmap6)  SDL_FreeSurface(planmap6);
	if(planmap7)  SDL_FreeSurface(planmap7);
	if(planshoot)  SDL_FreeSurface(planshoot);
	if(planboom)  SDL_FreeSurface(planboom);
	
	if(helicopter)  SDL_FreeSurface(helicopter);
	
	if(scoremap)  SDL_FreeSurface(scoremap);
	if(levelmap)  SDL_FreeSurface(levelmap);
	
	if(batterymap)  SDL_FreeSurface(batterymap);
	if(batteryshoot)  SDL_FreeSurface(batteryshoot);
	
	if(shipmap)  SDL_FreeSurface(shipmap);
	if(shipboom)  SDL_FreeSurface(shipboom);
	
	if(backgroundmap)  SDL_FreeSurface(backgroundmap);
	
	if(digitialvalue)  SDL_FreeSurface(digitialvalue);
	
	if(LifeCountImage)  SDL_FreeSurface(LifeCountImage);
	
	if(GameOverImage)  SDL_FreeSurface(GameOverImage);
			
	for(int i= 0;i<11;i++)
	{
		if(DigtialNumber[i]) SDL_FreeSurface(DigtialNumber[i]);
	}		
	if(SuspendImage)  SDL_FreeSurface(SuspendImage);
	if(MuteImage)  SDL_FreeSurface(MuteImage);	
	if(Input1Image)  SDL_FreeSurface(Input1Image);
	if(Input0Image)  SDL_FreeSurface(Input0Image);
	
	if(Screen)  SDL_FreeSurface(Screen);	
}


//原点坐标
int startposX = 0, startposY = 0;
// x,y 要加载到表面的左上角坐标
int BlitSurface(SDL_Surface* des, SDL_Surface* src, int x, int y)
{
	
	if(des == NULL || src ==NULL ) 
	{
		//if(debugcheck) printf("Couldn't BlitSurface with NULL surface: %s\n", SDL_GetError());
		return 1;
	}
	
	SDL_Rect destRec;	
	destRec.x=x+startposX;
	destRec.y=y+startposY;
	destRec.w=src->w;
	destRec.h=src->h;		
	SDL_BlitSurface(src, NULL, des, &destRec);	
	
	return 0;
}


//------------------Voice define------------------------
#include "SDL_mixer.h"
enum GamePlayVoice{
	START,     //游戏开始
	BEEP,      //飞机下降
	FIRE,      //开火
	FIREPLAN,  //击中飞机
	PLANBOOM,  //飞机爆炸
	SHIPBOOM,  //小船爆炸 
	GAMEOVER,  //游戏结束
	LEVELPASS  //关卡完成
};

#define WAVNUM 8
static Mix_Chunk *WavChunk[WAVNUM];   //播放音效数据，可以同时播放几个，因此用数组
int g_EnableSound = 0;
int g_SoundVolume = 128;

//初始化声音资源
void initAudio()
{
	int r;
	r=SDL_InitSubSystem(SDL_INIT_AUDIO);
    if(r<0)
	{
        g_EnableSound=0;
		printf("Couldn't SDL_INIT_AUDIO: %s\n", SDL_GetError());	
	}
	else		
	{
		
		for(int i=0;i<WAVNUM;i++) WavChunk[i]=NULL;
		r = Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 4096);		
		
		if( r < 0 ) {
			printf("Couldn't initialize SDL_Mixer: %s\n", SDL_GetError());		
			g_EnableSound=0;						
		}
		else
		{
			g_EnableSound=1;
			WavChunk[START]= Mix_LoadWAV("START.wav");
			if(WavChunk[START]) Mix_VolumeChunk(WavChunk[START],g_SoundVolume);
			
			WavChunk[BEEP]= Mix_LoadWAV("BEEP.wav");
			if(WavChunk[BEEP]) Mix_VolumeChunk(WavChunk[BEEP],g_SoundVolume);
			
			WavChunk[FIRE]= Mix_LoadWAV("FIRE.wav");
			if(WavChunk[FIRE]) Mix_VolumeChunk(WavChunk[FIRE],g_SoundVolume);
			
			WavChunk[FIREPLAN]= Mix_LoadWAV("FIREPLAN.wav");
			if(WavChunk[FIREPLAN]) Mix_VolumeChunk(WavChunk[FIREPLAN],g_SoundVolume);						

			WavChunk[PLANBOOM]= Mix_LoadWAV("PLANBOOM.wav");
			if(WavChunk[PLANBOOM]) Mix_VolumeChunk(WavChunk[PLANBOOM],g_SoundVolume);	

			WavChunk[SHIPBOOM]= Mix_LoadWAV("SHIPBOOM.wav");
			if(WavChunk[SHIPBOOM]) Mix_VolumeChunk(WavChunk[SHIPBOOM],g_SoundVolume);	

			WavChunk[GAMEOVER]= Mix_LoadWAV("GAMEOVER.wav");
			if(WavChunk[GAMEOVER]) Mix_VolumeChunk(WavChunk[GAMEOVER],g_SoundVolume);	
			
			WavChunk[LEVELPASS]= Mix_LoadWAV("LEVELPASS.wav");
			if(WavChunk[LEVELPASS])	Mix_VolumeChunk(WavChunk[LEVELPASS],g_SoundVolume);
			
			printf("Couldn't Mix_VolumeChunk: %s\n", SDL_GetError());	
		}
	}
}

//卸载音效资源
void unloadAudio()
{
	for(int i=0;i<WAVNUM;i++){
		if(WavChunk[i]){
             Mix_FreeChunk(WavChunk[i]);
		     WavChunk[i]=NULL;
		}
	}
	Mix_CloseAudio();
}

bool audioSwitch = true;	 

//播放音效
int PlayWAVAudio(int  pos, int len)
{
	if(!audioSwitch) return 0;
	
    if(g_EnableSound==0 || pos >= WAVNUM || WavChunk[pos] == NULL)
	{
		printf("Couldn't MPlayWAVAudio \n");
		return 1;          	
	}
	if(len<=0)	
		Mix_PlayChannel(-1, WavChunk[pos], 0);  //播放音效
	else
		Mix_PlayChannelTimed(-1, WavChunk[pos], 0, len);
	return 0;	
}


//播放声音优先级从低到高：BBEP   FIRE FIREPLAN  PLANBOOM  SHIPBOOM 
//void PlayAudioLogic()
//{
	
	//PlayWAVAudio(START);	
		
	//PlayWAVAudio(SHIPBOOM);
	//PlayWAVAudio(PLANBOOM);
	//PlayWAVAudio(FIREPLAN);
	//PlayWAVAudio(FIRE);
	//PlayWAVAudio(BEEP);
				
	//PlayWAVAudio(GAMEOVER);
	
	
//}
//------------------Voice define End----------------------------------------



/* Binary elements (pressed or not) that are shown on the display. */
enum Element {
	ELEMENT_DPAD_UP,
	ELEMENT_DPAD_DOWN,
	ELEMENT_DPAD_LEFT,
	ELEMENT_DPAD_RIGHT,
	ELEMENT_Y,
	ELEMENT_B,
	ELEMENT_X,
	ELEMENT_A,
	ELEMENT_SELECT,
	ELEMENT_START,
	ELEMENT_L1,
	ELEMENT_R1,
	ELEMENT_L2,
	ELEMENT_R2,
	ELEMENT_L3,
	ELEMENT_R3,
	ELEMENT_POWER,
#ifndef SDL_1
	ELEMENT_VOLDOWN,
	ELEMENT_VOLUP,
#endif	
	ELEMENT_COUNT
};

/* These define the keys that are not covered by JS 0, or used when the
 * GCW Zero's buttons are not being mapped to JS 0. */
#ifdef SDL_1
SDLKey KeysHavingElements[] = {
	SDLK_LEFT,
	SDLK_RIGHT,
	SDLK_UP,
	SDLK_DOWN,
	SDLK_LCTRL,
	SDLK_LALT,
	SDLK_SPACE,
	SDLK_LSHIFT,
	SDLK_TAB,
	SDLK_BACKSPACE,
	SDLK_PAGEUP,
	SDLK_PAGEDOWN,
	SDLK_KP_DIVIDE, 
	SDLK_KP_PERIOD,   //小键盘的.
	SDLK_ESCAPE,
	SDLK_RETURN,
	SDLK_HOME,
	/*
	SDLK_VOLUMEDOWN,
	SDLK_VOLUMEUP,
	*/
};
#else
SDL_Scancode KeysHavingElements[] = {
	SDL_SCANCODE_LEFT,
	SDL_SCANCODE_RIGHT,
	SDL_SCANCODE_UP,
	SDL_SCANCODE_DOWN,
	SDL_SCANCODE_LCTRL,
	SDL_SCANCODE_LALT,
	SDL_SCANCODE_SPACE,
	SDL_SCANCODE_LSHIFT,
	SDL_SCANCODE_TAB,	//R1
	SDL_SCANCODE_BACKSPACE,
	SDL_SCANCODE_PAGEUP,	//R2
	SDL_SCANCODE_PAGEDOWN,
	SDL_SCANCODE_KP_DIVIDE,
	SDL_SCANCODE_KP_PERIOD,
	SDL_SCANCODE_ESCAPE,
	SDL_SCANCODE_RETURN,
	SDL_SCANCODE_HOME,
	SDL_SCANCODE_VOLUMEDOWN,
	SDL_SCANCODE_VOLUMEUP
};
#endif

/* These define the elements that should be lit up by pressing the above keys,
 * ordered in the same way as in KeysHavingElements. */
enum Element KeysToElements[] = {
	ELEMENT_DPAD_LEFT,
	ELEMENT_DPAD_RIGHT,
	ELEMENT_DPAD_UP,
	ELEMENT_DPAD_DOWN,
	ELEMENT_A,
	ELEMENT_B,
	ELEMENT_X,
	ELEMENT_Y,
	ELEMENT_L1,
	ELEMENT_R1,
	ELEMENT_L2,
	ELEMENT_R2,
	ELEMENT_L3,
	ELEMENT_R3,
	ELEMENT_SELECT,
	ELEMENT_START,
	ELEMENT_POWER,
#ifndef SDL_1
	ELEMENT_VOLDOWN,
	ELEMENT_VOLUP,
#endif
};

/* These define the elements that should be lit up by pressing JS 0's
 * buttons. */
enum Element JoyButtonsToElements[] = {
	ELEMENT_B,
	ELEMENT_A,
	ELEMENT_Y,
	ELEMENT_X,
	ELEMENT_L1,
	ELEMENT_R1,
	ELEMENT_L2,
	ELEMENT_R2,
	ELEMENT_DPAD_UP,
	ELEMENT_DPAD_DOWN,	
	ELEMENT_DPAD_LEFT,
	ELEMENT_DPAD_RIGHT,	
	ELEMENT_L3,
	ELEMENT_R3,
	ELEMENT_SELECT,
	ELEMENT_START,
};

const char* ElementNames[] = {
	"D-pad Up",
	"D-pad Down",
	"D-pad Left",
	"D-pad Right",
	"Y",
	"B",
	"X",
	"A",
	"Select",
	"Start",
	"L1",
	"R1",
	"L2",
	"R2",
	"L3",
	"R3",
	"Power",
#ifndef SDL_1	
	"Vol-Down",
	"Vol-Up",
#endif
};

/* Last readings for all the elements and axes. */
bool ElementPressed[ELEMENT_COUNT];
bool ElementEverPressed[ELEMENT_COUNT];

bool DPadOppositeEverPressed = false;

int16_t BuiltInJS_X = 0;
int16_t BuiltInJS_Y = 0;
int16_t BuiltInJS_X2 = 0;
int16_t BuiltInJS_Y2 = 0;

struct DrawnElement {
	      SDL_Rect   Rect;
	const SDL_Color* ColorPressed;
	const SDL_Color* ColorEverPressed;
};



bool HapticActive = false;


/* - - - CUSTOMISATION - - - */

#define FONT_FILE        "font.ttf"
#define FONT_SIZE         12

#define SCREEN_WIDTH     640
#define SCREEN_HEIGHT    480

#define INNER_SCREEN_X    84
#define INNER_SCREEN_Y    18
#define INNER_SCREEN_W   154
#define INNER_SCREEN_H   117

#define GCW_ZERO_PIC_Y    43

#define TEXT_CROSS_LX      4
#define TEXT_CROSS_Y      4

#define TEXT_ANALOG_CX   160
#define TEXT_ANALOG_Y      4

#define TEXT_GRAVITY_CX  160
#define TEXT_GRAVITY_Y    20

#define TEXT_OTHERS_RX   316
#define TEXT_OTHERS_Y      4

#define TEXT_FACE_RX     316
#define TEXT_FACE_Y       4

#define TEXT_CROSS_ERR_LX  4
#define TEXT_CROSS_ERR_Y 188

#define TEXT_EXIT_RX     316
#define TEXT_EXIT_Y      220

#define TEXT_RUMBLE_RX   316
#define TEXT_RUMBLE_Y    204

                                   //  R    G    B    A
const SDL_Color ColorBackground   = {   0,   0,   0, 255 };
const SDL_Color ColorEmptycolor   = {   0,   0,   0, 0 };

const SDL_Color ColorBorder       = { 255, 255, 255, 255 };
const SDL_Color ColorInnerBorder  = { 128, 128, 128, 255 };
const SDL_Color ColorError        = { 255,  32,  32, 255 };
const SDL_Color ColorPrompt       = { 255, 255, 255, 255 };
const SDL_Color ColorNeverPressed = {  32,  32,  32, 255 };

const SDL_Color ColorCross        = { 255, 255,  32, 255 };
const SDL_Color ColorAnalog       = {  32, 255,  32, 255 };
const SDL_Color ColorAnalog2      = { 255, 128,  32, 255 };
const SDL_Color ColorFace         = {  32,  32, 255, 255 };
const SDL_Color ColorOthers       = { 255,  32, 255, 255 };

const SDL_Color ColorEverCross    = {  64,  64,  32, 255 };
const SDL_Color ColorEverAnalog   = {  32,  64,  32, 255 };
const SDL_Color ColorEverAnalog2  = {  64,  48,  32, 255 };
const SDL_Color ColorEverFace     = {  32,  32,  64, 255 };
const SDL_Color ColorEverOthers   = {  64,  32,  64, 255 };

bool MustExit(void)
{
	// Start+Select allows exiting this application.
	return ElementPressed[ELEMENT_SELECT] && ElementPressed[ELEMENT_START];
}

#ifdef SDL_1
#  define JOYSTICK_NAME(Index) SDL_JoystickName(Index)
#  define JOYSTICK_INDEX(Joystick) SDL_JoystickIndex(Joystick)
#  define SDL_COLOR(Source) SDL_MapRGB(Screen->format, (Source).r, (Source).g, (Source).b)
#  define MAKE_RASTER(Surface) Surface
#  define PRESENT() SDL_Flip(Screen); 
#else
#  define JOYSTICK_NAME(Index) SDL_JoystickNameForIndex(Index)
#  define JOYSTICK_INDEX(Joystick) SDL_JoystickInstanceID(Joystick)
#  define SDL_COLOR(Source) (Source).r, (Source).g, (Source).b, (Source).a
#  define PRESENT()  SDL_UpdateWindowSurface(mainwindows);
#endif



//-----------------------------------------GameLogic---------------------------------------------

//当前模式 0-初始化  1-游戏中炮台模式  2-游戏中直升机模式 
int currentmode = 0;

//输入模式 0-标准方向模式，1- 经典模式
int inputmode= 0; 

//定义每关的延迟周期每秒60个周期，暂定9关
int dashDelay[]=      {0,110,100,90, 80, 70, 60, 50,40,30};//飞机下降延迟
int newPlanDelay[]=   {0,90 ,80 ,70, 60, 50, 40, 30,25,20};//飞机出现延迟
int shipDelay[]=      {0,360,300,240,200,160,120,90,60,40};//小船前进延迟
int levelPlanCount[] ={0,20, 22, 25, 28, 32, 36, 44,55,72}; //每阶段总飞机数量
int shootPlanDelay = 40;//被击中飞机在屏幕上显示延迟
int demoDelay = 200;//演示在屏幕上显示延迟
int fireDely = 30; //
int currentLife = 3;  //生命数



int SCORE = 0; //当前分数
int HiSCORE = 0; //当前最高分数

int currentlevel = 1; //关卡级别
int HiLevel= 1; //当前最高关卡级别

int currentstage = 1; //关卡阶段
int HiStage = 1; //当前最高关卡阶段

int currentTick = 300; //当前计数周期

int planbound = 10; //击中飞机基本奖励，最终奖励乘以关卡级别
int shipbound = 20; //每推动小船一步算一次的最终奖励乘以关卡级别

int plancount;//每阶段剩余未击落飞机数量


bool suspentGame = false; //暂停游戏

//所有元素定义结构
struct ElementInfo {
	int Pos; //当前标识0-表明没有使用	
	SDL_Surface** imageinfo; //元素图片
	int posX, posY;	//贴图位置
	int delaytime; //当前持续周期，变为0就需要移动或者消失了
    
};


//飞机元素第0行不处理，Pos说明：0 不显示 每行代表一列,只有最后一行需要第6个位置 其中第四个位置固定45度图(planmap2)， 第五个位置固定爆炸图
struct ElementInfo MainPlanInfo[5][6]={{{0,NULL,0,0,0}, {0,NULL,0,0,0},{0,NULL,0,0,0},{0,NULL,0,0,0},{0,NULL,0,0,0},{0,NULL,0,0,0}},
									   {{0,&planmap5,220,0,0}, {0,&planmap2,224,100,0},{0,&planmap2,140,180,0},{0,&planmap3,100,260,0},{0,&planboom,122,300,0},{0,&emptyImgae,0,0,0}},
									   {{0,&planmap2,340,0,0}, {0,&planmap4,332,100,0},{0,&planmap2,320,180,0},{0,&planmap3,260,260,0},{0,&planboom,280,300,0},{0,&emptyImgae,0,0,0}},
									   {{0,&planmap2,470,0,0}, {0,&planmap3,430,100,0},{0,&planmap5,460,180,0},{0,&planmap3,400,260,0},{0,&planboom,420,300,0},{0,&emptyImgae,0,0,0}},
									   {{0,&planmap2,570,0,0}, {0,&planmap6,530,100,0},{0,&planmap2,570,180,0},{0,&planmap3,530,260,0},{0,&planmap5,560,330,0},{0,&planmap6,586,400,0}}};
//自由飞机
//struct ElementInfo MainFreePlanInfo[]={{0,NULL,0}, {0,NULL,0},{0,NULL,0},{0,NULL,0},{0,NULL,0},{0,NULL,0}};

struct surfacePos{
int posX, posY;
};


struct surfacePos  digitalPos = {10,40};
struct surfacePos  scoremapPos = {10, 108};
struct surfacePos  levelmapPos = {148,108};


//小船Pos说明：0 不显示
struct ElementInfo Mainshipinfo = {0,&shipmap,0,0,0};
struct surfacePos  MainShipPos[]={{0,0},{80,420},{230,430},{380,430},{530,430}};
int shipflashValue = 15; //小船闪烁时间间隔

//炮台Pos说明：0-不显示  1 2 3 3 代表1234位置, 具体图片由逻辑进行提换
struct ElementInfo MainBatteryInfo={0,&batterymap,0,0,0};
struct surfacePos  MainBatteryPos[]={{0,0},{70,292},{230,292},{370,292},{500,292}};

//直升机Pos说明：0-不显示
struct ElementInfo MainHelicopterInfo={0,&helicopter,0,0,0};
struct surfacePos  MainHelicopterPos[]={{0,0},{80,380},{230,380},{380,380},{530,380}};

int priviousPlan=0; //上次新飞机位置 下面的表用来防止连续同一个位置出飞机
int planPosMap33[5][2]={{0,0},{2,3},{3,1},{1,2},{1,2}}; 
int planPosMap44[5][3]={{0,0},{2,3,4},{3,4,1},{4,1,2},{1,2,3}};  

bool showScore = false;

bool needrefresh = false;

//播放飞机下降音乐
bool PlayBEEP = false;
bool PlayFire = false;
bool PlayFirePlan = false;
//没有移动过救船不加分
bool batteryMoved = false;

#include <time.h>
//演示刷新
void demorefresh()
{
	if(currentTick>demoDelay)
	{
		//srand((unsigned)time(NULL));				
		for(int i = 1; i<5; i++)
			for(int j=0; j<6; j++)
			{
				MainPlanInfo[i][j].Pos = rand()%2;
			}
		
		Mainshipinfo.Pos = rand()%5;
		if(Mainshipinfo.Pos==1)
			Mainshipinfo.imageinfo = &shipboom;
		else
			Mainshipinfo.imageinfo = &shipmap;
		
		MainBatteryInfo.Pos = rand()%5;
		
		MainHelicopterInfo.Pos = rand()%5;	
		
		showScore = rand()%2;	
		
		currentTick = 0;
		
		needrefresh = true;
		
		PlayWAVAudio(LEVELPASS, 0);
		
		if(debugcheck) printf("demorefresh waite game start \n");
	}
	currentTick++;
}

void createValueSurface(int pre, int post)
{
	//digitialvalue  DigtialNumber
	//char value[5];
	int value=10000;
	int realv =0;
	if(post == 0) //输出数字
	{
		if(pre>99999) pre = 99999;
		
		for (int i = 0; i<5; i++)
		{	
			realv = pre/value;
			pre = pre - realv*value;
			value /=10;
			BlitSurface(digitialvalue, DigtialNumber[realv], i*40, 0);
		}
		
	}
	else //输出关卡
	{
		if(pre>99) pre =99;
		if(post>99) post = 99;
		value = pre/10;
		realv = post/10;
		BlitSurface(digitialvalue, DigtialNumber[value], 0, 0);
		BlitSurface(digitialvalue, DigtialNumber[pre-value*10], 40, 0);
		BlitSurface(digitialvalue, DigtialNumber[10], 80, 0);
		BlitSurface(digitialvalue, DigtialNumber[realv], 120, 0);
		BlitSurface(digitialvalue, DigtialNumber[post-realv*10], 160, 0);
		
	}
	
}

//刷新整个屏幕
void mainRefresh()
{
	if(!needrefresh) return;
	
	if(debugcheck) printf("mainRefresh start \n");

	//准备大背景图	
	//SDL_FillRect (Screen, NULL, SDL_COLOR(ColorEmptycolor));
	SDL_BlitSurface(backgroundmap , NULL, RGBSurface, NULL);	
	

	int i, j;
	
	//刷新飞机
	for(i = 1; i<5; i++)
		for(j=0; j<6; j++)
		{
			if(MainPlanInfo[i][j].Pos)
			{				
				if(BlitSurface(RGBSurface, *(MainPlanInfo[i][j].imageinfo), MainPlanInfo[i][j].posX, MainPlanInfo[i][j].posY))
				{
					if(debugcheck) printf("Couldn't BlitSurface MainPlanInfo[%d][%d]\n",i, j);
				}
			}
		}
	
	//小船
	if(Mainshipinfo.Pos)
	{		
		if(BlitSurface(RGBSurface, *(Mainshipinfo.imageinfo), MainShipPos[Mainshipinfo.Pos].posX, MainShipPos[Mainshipinfo.Pos].posY))
			if(debugcheck) printf("Couldn't BlitSurface Mainshipinfo at %d\n",Mainshipinfo.Pos);
	}
	
	//炮台
	if(MainBatteryInfo.Pos)	
	{
		if(BlitSurface(RGBSurface, *(MainBatteryInfo.imageinfo), MainBatteryPos[MainBatteryInfo.Pos].posX, MainBatteryPos[MainBatteryInfo.Pos].posY))
			if(debugcheck) printf("Couldn't BlitSurface MainBatteryInfo at %d\n",MainBatteryInfo.Pos);
	}
	
	//直升机
	if(MainHelicopterInfo.Pos)	
	{
		if(BlitSurface(RGBSurface, *(MainHelicopterInfo.imageinfo), MainHelicopterPos[MainHelicopterInfo.Pos].posX, MainHelicopterPos[MainHelicopterInfo.Pos].posY))
			if(debugcheck) printf("Couldn't BlitSurface MainHelicopterInfo at %d\n",MainHelicopterInfo.Pos);
		
	}
		
	//标志
	if(showScore) 
	{		
		BlitSurface(RGBSurface, scoremap, scoremapPos.posX, scoremapPos.posY);
		if(currentmode >0)
			createValueSurface(SCORE,0);	
		else
			createValueSurface(HiSCORE, 0);	
	}
	else
	{
		BlitSurface(RGBSurface, levelmap, levelmapPos.posX, levelmapPos.posY);
		
		if(currentmode >0)
			createValueSurface(currentlevel,currentstage);		
		else
			createValueSurface(HiLevel,HiStage);					 
	}
	
	BlitSurface(RGBSurface, digitialvalue, digitalPos.posX, digitalPos.posY);
	
	//生命数、GAMEOVER显示	
	j = 24;
	if(currentLife <3 && currentmode>0)
	{
		for(i =3; i>currentLife; i--)
		{			
			BlitSurface(RGBSurface, LifeCountImage, j, 192);
			j+=28;
		}
		
		if(currentLife==0)BlitSurface(RGBSurface, GameOverImage, 40, 136);
		
	}
	
	if(!audioSwitch)//静音标志
	{
		BlitSurface(RGBSurface, MuteImage, 100, 4);
	}
		
	if(suspentGame)//暂停标志
	{
		BlitSurface(RGBSurface, SuspendImage, 100, 180);
	}
	
	if(inputmode==0)//输入标志
	{
		BlitSurface(RGBSurface, Input0Image, 10, 380);
	}
	else if(inputmode==1)
	{
		BlitSurface(RGBSurface, Input1Image, 10, 380);
	}
	//SDL_UnlockSurface(Screen);
/*	
	Uint32* src = (Uint32*)RGBSurface->pixels; 
	Uint32* des = (Uint32*)Screen->pixels;
	
	
	for(i = 0; i<240; i++)
	{
		for(j=0; j<320; j++)
		{
			*des = (((*src)&0xFCFCFCFC)>>2) + (((*(src+1))&0xFCFCFCFC)>>2) + (((*(src+640))&0xFCFCFCFC)>>2) + (((*(src+641))&0xFCFCFCFC)>>2);
			//*des = *src; //((*src)&0xFF)+(((*src)&0xFCFCFCFC)>>2 + ((*(src+1))&0xFCFCFCFC)>>2 + ((*(src+640))&0xFCFCFCFC)>>2 + ((*(src+641))&0xFCFCFCFC)>>2);
								 //+((((*src)&0x3030300 + (*(src+1))&0x3030300  + (*(src+640))&0x3030300 + (*(src+641))&0x3030300)&0xFCFCFC00)>>2);			
			des++;
			src+=2;
		}
		src+=640;
	}
*/
	PRESENT();	
	//SDL_LockSurface(Screen);
}

int  inputdefine[2][10]={
	{	
		ELEMENT_DPAD_UP,
		ELEMENT_DPAD_DOWN,
		ELEMENT_DPAD_LEFT,
		ELEMENT_DPAD_RIGHT,
		ELEMENT_Y,
		ELEMENT_B,
		ELEMENT_X,
		ELEMENT_A,
		ELEMENT_SELECT,
		ELEMENT_START,	
	},
	
	{
		ELEMENT_X,//ELEMENT_DPAD_UP,
		ELEMENT_B,//ELEMENT_DPAD_DOWN,
		ELEMENT_DPAD_DOWN,//ELEMENT_DPAD_LEFT,
		ELEMENT_A,//ELEMENT_DPAD_RIGHT,
		ELEMENT_Y,
		ELEMENT_DPAD_UP,//ELEMENT_B,
		ELEMENT_X,
		ELEMENT_DPAD_UP,//ELEMENT_A,
		ELEMENT_SELECT,
		ELEMENT_START,		
		}
	
	};


//处理用户输入
void handleUserInput()
{
	needrefresh = false;
	
	if(ElementPressed[ELEMENT_SELECT]) //设置静音开关	
	{
		audioSwitch = !audioSwitch;	
		needrefresh = true;
		if(debugcheck) printf("handleUserInput change  audioSwitch\n");
	}
	
	if(ElementPressed[ELEMENT_Y]) //设置输入模式	
	{
		if(inputmode==0 ) 
			inputmode =1;
		else
			inputmode = 0;		
		
		needrefresh = true;		
		if(debugcheck) printf("handleUserInput change  Inputtmode\n");
	}
	
	
	//演示状态 随机展示图片即可
	if(currentmode ==0)
	{
		if(ElementPressed[ELEMENT_START]) //进入游戏
		{	
			needrefresh = true;
			if(debugcheck) printf("handleUserInput Start Game\n");
			
			if(currentTick<180) //等demo音乐完成
				SDL_Delay((180-currentTick) *15);
				
			//播放游戏开始音乐
			PlayWAVAudio(START, 0);
			
			//初始化状态
			initMainGame(true);		
			currentmode =1;
				
			//刷新屏幕
			showScore = false;
			mainRefresh();			
			showScore = true;
			//延迟3s开始游戏
			SDL_Delay(3000);
			//createValueSurface(SCORE,0);
		}
		else
		{			
			//随机展示图片
			demorefresh();			
			//刷新屏幕
			mainRefresh();			
			
		}					
		return;
	}
	else
	{
		if(ElementPressed[ELEMENT_START]) //暂停游戏
		{
			suspentGame = !suspentGame;
			if(suspentGame) 
			{
				needrefresh = true;
				mainRefresh();//暂停前需要刷新屏幕
				return;
			}
		}
		else
		{
			if(suspentGame) return;
		}
		
		
		if(plancount ==0)//开始新关卡
		{
			//播放游戏过关音乐
			PlayWAVAudio(LEVELPASS, 0);
			mainRefresh();		
			
			if(currentstage ==3)
			{
				currentlevel++; //关卡级别
				currentstage = 1;
				if(HiLevel< currentlevel)	
				{
					HiLevel = currentlevel;
					HiStage = 1;
				}
			}
			else
			{
				currentstage ++;
				
				if(HiLevel< currentlevel)	
				{
					HiLevel = currentlevel;
					HiStage = currentstage;
				}
				else if(HiLevel == currentlevel)
				{
					if(HiStage < currentstage) 
						HiStage = currentstage;
				}				
			}
			
			SDL_Delay(2000);
			initMainGame(false);
			//刷新屏幕
			showScore = false;
			mainRefresh();		
			showScore = true;	
			//延迟2s开始游戏
			SDL_Delay(1000);
			//createValueSurface(SCORE,0);	
			return;
		}
	}
		
	
		//先处理各个元素位置
		if(!handlePlan()) return;	
	
		if(!handleShip()) return;
	

		
		//判断用户有效输入
		if (currentmode==2 && ElementPressed[inputdefine[inputmode][ELEMENT_DPAD_UP]] ) //直升机变炮台
		{
			MainHelicopterInfo.Pos = 0;
			MainBatteryInfo.Pos = 1;
			MainBatteryInfo.imageinfo = &batterymap;			
			currentmode = 1;
			needrefresh = true;
			if(debugcheck) printf("handleUserInput change to  Battery\n");
		}
		else if (Mainshipinfo.Pos && currentmode== 1 && ElementPressed[inputdefine[inputmode][ELEMENT_DPAD_DOWN]] ) //炮台变直升机,小船有的情况才有
		{				
			MainBatteryInfo.Pos = 0;
			MainHelicopterInfo.Pos = Mainshipinfo.Pos;
			currentmode = 2;
			
			if(batteryMoved) SCORE+=shipbound*currentlevel; //加分			
			batteryMoved = false;
			
			//createValueSurface(SCORE,0);
			Mainshipinfo.delaytime = shipDelay[currentlevel]; //增加延迟
			
			needrefresh = true;
			if(debugcheck) printf("handleUserInput change to  Helicopter\n");	
		}
				
		if ( ElementPressed[inputdefine[inputmode][ELEMENT_DPAD_LEFT]])//向左移动炮台
		{
			if(currentmode == 1 && MainBatteryInfo.Pos>1 )
			{
			   MainBatteryInfo.Pos--;				   
			   MainBatteryInfo.imageinfo = &batterymap; //恢复成未发射状态			   
			   needrefresh = true;
			   if(debugcheck) printf("handleUserInput Move Battery to: %d\n", MainBatteryInfo.Pos);
			   batteryMoved = true;
			}
		}
		else if(ElementPressed[inputdefine[inputmode][ELEMENT_DPAD_RIGHT]])//向右移动炮台或者直升机
		{
			if(currentmode == 1)
			{
				if(MainBatteryInfo.Pos<4)
				{
					MainBatteryInfo.Pos++;					
					batteryMoved = true;
					needrefresh = true;
					if(debugcheck) printf("handleUserInput  Move Battery to: %d\n", MainBatteryInfo.Pos);
				}
				if(MainBatteryInfo.imageinfo != &batterymap)
				{									
					MainBatteryInfo.imageinfo = &batterymap;//恢复成未发射状态
					needrefresh = true;
					if(debugcheck) printf("handleUserInput change Battery to: batterymap\n");	
				}
			}
			else if(currentmode == 2)
			{			
				if(Mainshipinfo.Pos < 4) 
				{
					MainHelicopterInfo.Pos++;
					Mainshipinfo.Pos++;
					SCORE+=shipbound*currentlevel; //加分，放在这里防止频繁加分
					//createValueSurface(SCORE,0);
					needrefresh = true;
					if(debugcheck) printf("handleUserInput  Move HelicopterInfo to: %d\n", MainHelicopterInfo.Pos);
					batteryMoved = true;
				}				
				Mainshipinfo.delaytime = shipDelay[currentlevel]; //增加延迟
			}
		}
		
		PlayFire = false;
		PlayFirePlan = false;
		
		if(ElementPressed[inputdefine[inputmode][ELEMENT_A]]|| ElementPressed[inputdefine[inputmode][ELEMENT_B]]) //攻击设置
		{
			if(MainBatteryInfo.imageinfo != &batteryshoot)
			{
				MainBatteryInfo.imageinfo = &batteryshoot; 				
				
				//播放发射音乐				
				PlayFire = true;
				needrefresh = true;
				fireDely = 30;
				if(debugcheck) printf("handleUserInput  Battery Fire at %d\n", MainBatteryInfo.Pos);
			}
		}		
	
	
	
	//不能简单的还原飞机图片，这里还要考虑被击中的图片需要延迟一段时间
	//MainPlanInfo[0][3].imageinfo = MainPlanInfo[1][3].imageinfo = MainPlanInfo[2][3].imageinfo = MainPlanInfo[3][3].imageinfo = planmap2;
	

		
	//攻击判定
	if( MainBatteryInfo.imageinfo == (&batteryshoot) && currentmode == 1)
	{
		if(MainPlanInfo[MainBatteryInfo.Pos][3].Pos && (MainPlanInfo[MainBatteryInfo.Pos][3].imageinfo !=  &planshoot)) //炮台位置上的飞机是否存在
		{
			//设置图片
			MainPlanInfo[MainBatteryInfo.Pos][3].imageinfo = &planshoot;
			
			//播放击中音乐
			PlayFirePlan = true;
			
			
			//刷新爆炸图片的延迟显示时间
			MainPlanInfo[MainBatteryInfo.Pos][3].delaytime = shootPlanDelay;	
			
			//加分
			SCORE += planbound*currentlevel;
			//createValueSurface(SCORE,0);
			plancount--;
			
			needrefresh = true;
			fireDely = 0;
			if(debugcheck) printf("handleUserInput  Shot Plan at:%d\n", MainBatteryInfo.Pos);
		}

	}	
	if(fireDely==0){
		MainBatteryInfo.imageinfo = &batterymap;//恢复成未发射状态
	}
	
	fireDely--;
	
	//播放飞机相关声音	
	if(PlayFirePlan)PlayWAVAudio(FIREPLAN, 0);
	else if (PlayFire) PlayWAVAudio(FIRE, 0);	
	else if(PlayBEEP)PlayWAVAudio(BEEP, 0);
	
	//刷新屏幕
	mainRefresh();
}

//处理普通飞机移动模式

bool handlePlan()
{
	PlayBEEP = false;
	int i,j;
	currentTick++;
		
	if(!priviousPlan) priviousPlan = (rand()%3)+1;		
	
	//增加新飞机
	if(currentTick>newPlanDelay[currentlevel])
	{
		
		//srand((unsigned)time(NULL));				
		currentTick = rand();
		
		if(currentstage < 2) //第一阶段没有自由飞机
			currentTick = planPosMap33[priviousPlan][currentTick%2];
		else
			currentTick = planPosMap44[priviousPlan][currentTick%3];					
		
		priviousPlan = currentTick;
		
		//如果该位置占用了相当于只刷新了延迟时间
		MainPlanInfo[currentTick][0].Pos = 1;		
		MainPlanInfo[currentTick][0].delaytime = dashDelay[currentlevel];						
		PlayBEEP = true;					
				
		needrefresh = true;
		if(debugcheck) printf("handlePlan new paln add %d-%d at %d \n", currentlevel, currentstage, currentTick );	
		
		currentTick = 0;		
		
	}
	
	//处理普通飞机1、2、3列	
	for(i =1; i<4;i++)		
	{
		//碰撞判断
		if(MainPlanInfo[i][3].Pos)
		{
			MainPlanInfo[i][3].delaytime--;
			
			if(MainPlanInfo[i][3].delaytime<0) 
			{
				needrefresh = true;
				if(MainPlanInfo[i][3].imageinfo == (&planshoot)) //被攻击图消失
				{
					MainPlanInfo[i][3].Pos = 0;
					MainPlanInfo[i][3].imageinfo = &planmap2;
					
					if(debugcheck) printf("handlePlan removed Fired Plan at:%d \n", i);	
					
				}
				else//碰撞发生
				{
					MainPlanInfo[i][3].Pos = 0;
					MainPlanInfo[i][4].Pos = 1;
					currentLife--;
					if(currentLife>0) 
					{					
						//本关重新开始
						PlayWAVAudio(PLANBOOM, 0);	
						mainRefresh();		
						initMainGame(false);
						if(debugcheck) printf("handlePlan %d fail restart %d-%d \n", i, currentlevel, currentstage);		
					}
					else //gameOver
					{
						
						
						//播放GameOver音乐
						PlayWAVAudio(GAMEOVER, 0);
						if(HiSCORE< SCORE)HiSCORE = SCORE;
						mainRefresh();	
						
						//游戏结束标志
						currentmode =0;												
						initMainGame(true);						
						if(debugcheck) printf("handlePlan %d game over restart %d-%d \n", i, currentlevel, currentstage);
						SDL_Delay(1000);	
					}

									
					//延迟2s开始游戏
					SDL_Delay(2000);
					showScore = false;
					mainRefresh();
					showScore = true;
				
					//延迟2s开始游戏					
					SDL_Delay(1000);
					//createValueSurface(SCORE,0);

					return false;
				}
			}
				
		}
				
		for (int j = 2; j>=0; j--) //反向处理飞机
		{
			if(MainPlanInfo[i][j].Pos)
			{
				MainPlanInfo[i][j].delaytime--;
				
				if(MainPlanInfo[i][j].delaytime<0) //需要下移了
				{
					needrefresh = true;
					if(debugcheck) printf("handlePlan moved Plan(%d) from: %d \n", i, j);
					
					MainPlanInfo[i][j].Pos = 0;
					MainPlanInfo[i][j+1].Pos = 1;
					MainPlanInfo[i][j+1].delaytime = dashDelay[currentlevel];
					//设置图片
					if(j==2) MainPlanInfo[i][3].imageinfo = &planmap2;
					
					PlayBEEP = true;
				}				
			}
		}
	}
	
	//自由飞机第四列
	if(MainPlanInfo[4][5].Pos) //最后位置消失处理
	{
		MainPlanInfo[4][5].delaytime--;
		if(MainPlanInfo[4][5].delaytime<0)
		{
			MainPlanInfo[4][5].Pos = 0;
			PlayBEEP = true;
			needrefresh = true;
		}
	}
	
	if( MainPlanInfo[4][3].Pos && MainPlanInfo[4][3].imageinfo == (&planshoot)) //被攻击图消失
	{
		MainPlanInfo[4][3].delaytime--;
		if(MainPlanInfo[4][3].delaytime < 0)
		{
			MainPlanInfo[4][3].Pos = 0;
			MainPlanInfo[4][3].imageinfo = &planmap2;
			needrefresh = true;
			if(debugcheck) printf("handlePlan removed Fired Plan at:4\n");	
		}
	}
				
	for(i =4; i>=0;i--)	//前面几个位置
	{
		if(MainPlanInfo[4][i].Pos)
		{
			MainPlanInfo[4][i].delaytime--;
			if(MainPlanInfo[4][i].delaytime<0)
			{
				MainPlanInfo[4][i].Pos = 0;
				MainPlanInfo[4][i+1].Pos = 1;
				MainPlanInfo[4][i+1].delaytime = dashDelay[currentlevel];
				
				//设置图片
				if(i==2) MainPlanInfo[4][3].imageinfo = &planmap2;
				
				PlayBEEP = true;
				
				needrefresh = true;
				if(debugcheck) printf("handlePlan moved Plan(4) from: %d \n", i);
			}			
		}
	}			

	return true;	
}


//处理小船逻辑
bool handleShip()
{
	
	Mainshipinfo.imageinfo = &shipmap;
	
	//直升机模式下不减时间
	if(currentmode ==2) return true;
	//Mainshipinfo
	if(Mainshipinfo.Pos)
	{
		if(Mainshipinfo.Pos==2)
		{
			if(Mainshipinfo.posX>shipflashValue) //不显示图标 闪烁
			{
				Mainshipinfo.posX = 0;
				Mainshipinfo.imageinfo = &emptyImgae;
				needrefresh = true;
			}
			else
				Mainshipinfo.posX++;			 
		}
		else
		{
			Mainshipinfo.posX = 0;
		}
		
		Mainshipinfo.delaytime--;
		if(Mainshipinfo.delaytime<0)
		{
			needrefresh = true;
			Mainshipinfo.Pos--;
			if(Mainshipinfo.Pos <= 1) //爆炸了
			{
				Mainshipinfo.imageinfo = &shipboom;
				currentLife--;
				if(currentLife>0) 
				{					
					//本关重新开始
					PlayWAVAudio(SHIPBOOM, 0);	
					mainRefresh();	
					initMainGame(false);
					if(debugcheck) printf("handleShip fail restart %d-%d \n", currentlevel, currentstage);	
						
				}
				else //gameOver
				{

					//播放GameOver音乐
					PlayWAVAudio(GAMEOVER, 0);
					mainRefresh();
					//游戏结束
					currentmode =0;					
					if(HiSCORE< SCORE)HiSCORE = SCORE;
					
					initMainGame(true);
					if(debugcheck) printf("handleShip game over restart %d-%d \n", currentlevel, currentstage);	
					SDL_Delay(1000);
				}
				
				//刷新页面
				
				
				//延迟2s开始游戏
				SDL_Delay(2000);
				showScore = false;
				mainRefresh();
				SDL_Delay(1000);
				showScore = true;
				
				
				//createValueSurface(SCORE,0);				
				return false;
			}
			else
			{
				Mainshipinfo.delaytime = shipDelay[currentlevel]; //增加延迟
			}
			
		}
		
	}	
	return true;
}


//初始化，分为整体初始化或者每关初始化
void initMainGame( bool isintinal)
{
	if(isintinal)
	{
		SCORE = 0; //当前分数		
		currentLife = 3;  //生命数
		currentlevel = 1; //关卡级别
		currentstage = 1; //关卡阶段		
		//MainPlanInfo[0][4].imageinfo = MainPlanInfo[1][4].imageinfo = MainPlanInfo[2][4].imageinfo = MainPlanInfo[3][4].imageinfo = &planboom;
	}
	plancount = levelPlanCount[currentlevel]; //每阶段累计飞机数量	
	
	batteryMoved = false;
	
	currentTick = 0;
	//createValueSurface(currentlevel,currentstage);
	needrefresh = true;
	
	if(currentstage == 1)
	{
		Mainshipinfo.Pos = 0;
	}
	else
	{
		Mainshipinfo.Pos = 4;
		Mainshipinfo.delaytime = shipDelay[currentlevel];
	}
	Mainshipinfo.imageinfo = &shipmap;
	
	
	MainPlanInfo[0][0].Pos = MainPlanInfo[0][1].Pos = MainPlanInfo[0][2].Pos = MainPlanInfo[0][3].Pos = MainPlanInfo[0][4].Pos = MainPlanInfo[0][5].Pos= 0;
	MainPlanInfo[1][0].Pos = MainPlanInfo[1][1].Pos = MainPlanInfo[1][2].Pos = MainPlanInfo[1][3].Pos = MainPlanInfo[1][4].Pos = MainPlanInfo[1][5].Pos= 0;
	MainPlanInfo[2][0].Pos = MainPlanInfo[2][1].Pos = MainPlanInfo[2][2].Pos = MainPlanInfo[2][3].Pos = MainPlanInfo[2][4].Pos = MainPlanInfo[2][5].Pos= 0;
	MainPlanInfo[3][0].Pos = MainPlanInfo[3][1].Pos = MainPlanInfo[3][2].Pos = MainPlanInfo[3][3].Pos = MainPlanInfo[3][4].Pos = MainPlanInfo[3][5].Pos= 0;
	MainPlanInfo[4][0].Pos = MainPlanInfo[4][1].Pos = MainPlanInfo[4][2].Pos = MainPlanInfo[4][3].Pos = MainPlanInfo[4][4].Pos = MainPlanInfo[4][5].Pos= 0;
	
	//MainFreePlanInfo[0].Pos = MainFreePlanInfo[1].Pos = MainFreePlanInfo[2].Pos = MainFreePlanInfo[3].Pos = MainFreePlanInfo[4].Pos = MainFreePlanInfo[5].Pos = 0;
	
	MainBatteryInfo.Pos = 1;
	MainBatteryInfo.imageinfo = &batterymap;
	
	MainHelicopterInfo.Pos = 0;
	
}
//----------------------------Game Logic End-----------------------------------------------------

int main(int argc, char** argv)
{
	//FILE *fp = NULL;
	//fp = fopen("/media/roms/app/test.log", "w"); 
	//char strlog[128];

	unsigned int i;
	bool Error = false;
	
	if(argc >1) debugcheck = true;
	
	
	printf("SDL " SDL_VER_STR " AF-84 Game starting\n");

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) < 0)
	{
		printf("SDL initialisation failed: %s\n", SDL_GetError());
		Error = true;
		goto end;
	}

	SDL_ShowCursor(SDL_DISABLE);	

#ifdef SDL_1
	Screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, 32, SDL_SWSURFACE);
	if (Screen == NULL)
	{
		printf("SDL_SetVideoMode failed: %s\n", SDL_GetError());
		Error = true;
		goto cleanup_font;
	}
	RGBSurface =  Screen;//SDL_CreateRGBSurface(SDL_SWSURFACE,640,480, Screen->format->BitsPerPixel,
	                      //Screen->format->Rmask,Screen->format->Gmask,Screen->format->Bmask,0);
#else
	
	SDL_SetHintWithPriority(SDL_HINT_RENDER_SCALE_QUALITY, "2", SDL_HINT_OVERRIDE);		
	mainwindows = SDL_CreateWindow("Input test",
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL);
	
	Screen  = SDL_GetWindowSurface(mainwindows);
	
	if (Screen == NULL)
	{
		printf("SDL_CreateWindow failed: %s\n", SDL_GetError());
		Error = true;
		goto cleanup_font;
	}
	RGBSurface = Screen;//

//	RGBSurface = SDL_CreateRGBSurface(0,720,480, Screen->format->BitsPerPixel,
//	                    Screen->format->Rmask,Screen->format->Gmask,Screen->format->Bmask,0);
#endif
	 

#ifdef SDL_1
	// Make sure we don't get key repeating.
	SDL_EnableKeyRepeat(0, 0);
#endif
	// Initialise joystick input.
	SDL_JoystickEventState(SDL_ENABLE);
	SDL_Joystick* BuiltInJS = NULL;

	BuiltInJS = SDL_JoystickOpen(0);
	

	bool Exit = false;
	printf("entry main while\n");
	
	initAudio();
	initVideoSurface();
	
	//初始化随机数
	srand((unsigned)time(NULL));
	
	//准备大背景图
	//SDL_ClearError();
	//SDL_BlitSurface(backgroundmap, NULL, RGBSurface, NULL);	
	//PRESENT();	

	if(debugcheck) printf("First Main frame Blit error is: %s\n", SDL_GetError());
	
	while (!Exit)
	{
		int delaytime, endtime; //延迟时间
		
		delaytime = 15 + SDL_GetTicks(); //15ms循环一轮
		
		SDL_Event Event;

		while(SDL_PollEvent(&Event))
		{
			//printf("SDL_PollEvent come\n");
			switch (Event.type)
			{
				case SDL_JOYBUTTONDOWN:
				case SDL_JOYBUTTONUP:
					if (BuiltInJS != NULL
					 && Event.jbutton.which == JOYSTICK_INDEX(BuiltInJS))
					{
						i = JoyButtonsToElements[Event.jbutton.button];
						//if (ElementPressed[i] && Event.type == SDL_JOYBUTTONDOWN)
						//	printf("Received SDL_JOYBUTTONDOWN for already-pressed button %s (joystick %d button %d)\n", ElementNames[i], Event.jbutton.which, Event.jbutton.button);
						//else if (!ElementPressed[i] && Event.type == SDL_JOYBUTTONUP)
						//	printf("Received SDL_JOYBUTTONUP for already-released button %s (joystick %d button %d)\n", ElementNames[i], Event.jbutton.which, Event.jbutton.button);
						ElementPressed[i] = (Event.type == SDL_JOYBUTTONDOWN);
						ElementEverPressed[i] |= ElementPressed[i];
					}
					break;
				case SDL_KEYDOWN:
				case SDL_KEYUP:
					for (i = 0; i < sizeof(KeysHavingElements) / sizeof(KeysHavingElements[0]); i++)
					{
#ifdef SDL_1
						if (Event.key.keysym.sym == KeysHavingElements[i])
#else
						if (Event.key.keysym.scancode == KeysHavingElements[i] && Event.key.repeat == 0)
#endif
						{
							i = KeysToElements[i];							
							ElementPressed[i] = (Event.type == SDL_KEYDOWN);
							ElementEverPressed[i] = true;
							//printf("Received SDL_KEY button %s (keyboard %s)\n", ElementNames[i], SDL_GetKeyName(Event.key.keysym.sym));
							//fwrite(strlog, sizeof(strlog) , 1, fp );	
							break;
						}
						else
						{
							//printf("Received SDL_KEY button %s\n", SDL_GetKeyName(Event.key.keysym.sym));
							
							 //fwrite(strlog, sizeof(strlog) , 1, fp );
						}
					}
					break;
				case SDL_QUIT:
					Exit = true;
					break;
					
			} // switch (Event.type)
		} // while (SDL_PollEvent(&Event))
				
		
		handleUserInput();	
	
		Exit |= MustExit();
		
		for (i = 0; i < ELEMENT_COUNT; i++)
		{
			ElementPressed[i] = 0;
		}
						
		endtime = SDL_GetTicks();				

		if(delaytime > endtime ) SDL_Delay(delaytime-endtime);
		
	} // while (!Exit)
		
	unloadAudio();
	unloadVideoSurface();
	
	//if(fp)
	//{
	//	fseek(fp,0,SEEK_END); 
	//	fclose(fp);	
	//}
	
	

	if (BuiltInJS != NULL)
		SDL_JoystickClose(BuiltInJS);


#ifdef SDL_2
	SDL_DestroyWindow(mainwindows);	
#endif
  //if(RGBSurface)  SDL_FreeSurface(RGBSurface); 

cleanup_font:

	SDL_Quit();

end:
	return Error ? 2 : 0;
}
