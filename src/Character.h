#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <string>
#include "LTexture.h"
#include "Event.h"
#include <vector> 
#include <iostream>
#include <fstream>
#include <queue>
#define swidth 1920
#define sheight 1080
using namespace std;
queue < Event > q;

class Power{
    public:
		//The dimensions of the dot
        //Scene textures
        LTexture gPowerTexture;
        SDL_Renderer* gRenderer;
		int POWER_WIDTH = 150;
		int POWER_HEIGHT = 50;
        int mPosX, mPosY;
        int id ;
		//Maximum axis velocity of the dot
		static const int POWER_VEL = 20;

        // No of States
        static const int nStates = 1;
        
		//Initializes the variables
		Power(int pType, int _mPosX, int _mPosY, int _id);

	
		//Moves the power
		void move();

		//Shows the dot on the screen
		void render(SDL_Renderer* gRenderer, LTexture* gDotTexture);

    private:
		//The X and Y offsets of the dot
		

		//The velocity of the dot
		double mVelX, mVelY;

        // The gravity
        int state; 
        int speedUP = 7;
        int frames;
        int performingState = 0 ;
        
        int frameCounts[nStates];
        SDL_Rect gSpriteClips[ nStates][15];

};

Power::Power(int pType, int _mPosX, int _mPosY, int _id){
    id = _id;
    mPosX = _mPosX;
    mPosY = _mPosY;
    if(pType)
        mVelX = -POWER_VEL;
    else
        mVelX = POWER_VEL;
    state = 0 ;
    frames = 0;

    frameCounts[0] = 6 ;

    gSpriteClips[ 0 ][ 0 ].x =   620;
    gSpriteClips[ 0 ][ 0 ].y =   3235;
    gSpriteClips[ 0 ][ 0 ].w =  50;
    gSpriteClips[ 0 ][ 0 ].h =  80;
    int temp0[6] = {50, 65, 75, 95, 85,90} ;
     // state 0 rects
    for(int i = 1 ; i < frameCounts[0]; i++){
			gSpriteClips[0][i] = gSpriteClips[0][i-1];
            gSpriteClips[0][i].x+= gSpriteClips[0][i-1].w;
            gSpriteClips[0][i].w = temp0[i]; 
    }

    // for(int i = 0 ; i < frameCounts[0]; i++){
	// 		gSpriteClips[ 0 ][ i ].x =   88 * i;
	// 		gSpriteClips[ 0 ][ i ].y =   220;
	// 		gSpriteClips[ 0 ][ i ].w =  88;
	// 		gSpriteClips[ 0 ][ i ].h = 110;
    // }
    
}

void Power::move(){
    mPosX += mVelX;
    mPosY += mVelY;
    
}

void Power::render(SDL_Renderer* gRenderer, LTexture *gDotTexture)
{
    SDL_Rect* currentClip = &gSpriteClips[state][ frames / speedUP ];
    SDL_Rect dest;
    
    dest.x = mPosX;                              
    dest.y = mPosY;
    dest.w = 2*currentClip->w;
    dest.h = 2*currentClip->h;
    // cout << currentClip->x << " " << currentClip->w << " " << frames << endl; 
    SDL_RenderCopyEx(gRenderer, gDotTexture->getTexture(), currentClip,&dest, 0.0, NULL, SDL_FLIP_HORIZONTAL);

    // gDotTexture->render(gRenderer, mPosX, mPosY, currentClip,0.0,NULL,SDL_FLIP_NONE );
    //Update screen
   // SDL_RenderPresent( gRenderer );

    //Go to next frame
    ++frames;
    
    //Cycle animation
    if( frames / speedUP >= frameCounts[state] )
    {
        frames = 0;
    }
    //Show the dot
	//gDotTexture.render(gRenderer, mPosX, mPosY );
}

struct Life
{
    /* data */
    int health;
    Life(){
        health = 100;
    }
    bool isAlive(){
        return health > 0 ;
    }
    void decreasePunch(){
        health = max(health - 10, 0);
    }
    void decreaseKick(){
        health = max(health - 6, 0);
    }
    void decreasePower(){
        health = max(health - 8, 0);
    }
    int getHealth(){
        return health;
    }
    int getWidth(){
        return health*4.8;
    }
    int getHeight(){
        return 40;
    }
    int getGreen(){
        if(health > 50)
            return 177;
        return health*177.0/50.0;
    }
    int getRed(){
        if(health < 50)
            return 177;
        return (100.0-health)*177.0/50.0;
    }
};


struct PowerHealth
{
    /* data */
    int health;
    PowerHealth(){
        health = 99;
    }
    bool isAlive(){
        return health > 0 ;
    }
    void decrease(){
        health = max(health - 33, 0);
    }
    void increase(){
        health = min(health+33, 99);
    }
    int getHealth(){
        return health;
    }
    int getWidth(){
        return (health+1)*4.8;
    }
    int getHeight(){
        return 40;
    }
};



struct KeyProfile{
    int up,down,left,right;
    int p1,p2,p3;
    KeyProfile(){}
    KeyProfile(string path){
        ifstream fin(path,ios::in);
        fin >> up >> down >> left >> right >> p1 >> p2 >> p3;
    }
};

class Dot
{
    public:
		//The dimensions of the dot
        //Scene textures
        // No of States
        static const int nStates = 12;
        LTexture gDotTexture;
        LTexture gHealthTexture;
		int DOT_WIDTH[nStates];
		int DOT_HEIGHT[nStates];
        int powerId = 0;
        bool won = 0;
        bool lost = 0;
        int *playBackPointer;
        //Scaler
        static const int Scaler = 3;
		//Maximum axis velocity of the dot
		static const int DOT_VEL = 2;
        //Powers Container
        vector < Power > powerContainer;
		//Initializes the variables
		Dot(int pType, int *pBack);

		//Takes key presses and adjusts the dot's velocity
		void handleEvent( SDL_Event& e );

        //Handle Fighting events
        bool handleFightingEvent(Event e);

		//Moves the dot
		void move(int restrictionX);

		//Shows the dot on the screen
		void render(SDL_Renderer* gRenderer);

        //go to next frame for animation and also handle states
        
        void updateFrames();

        int getPosX(){return mPosX;}
        int getPosY(){return mPosY;}

        bool isAlive(){
            return life.isAlive();
        }

        bool collission(int mx, int my){
            if(mx < mPosX || mx > mPosX+DOT_WIDTH[state])
                return 0;
            if(my < mPosY || my > mPosY+DOT_HEIGHT[state])
                return 0;
            return 1;
        }

        int getPerformingPosX(Event e){
            if(e.getType() == 0){
                return getPunchPosX();
            }
            else{
                return getKickPosX();
            }
        }
        int getPerformingPosY(Event e){
            if(e.getType()==0){
                return getPunchPosY();
            }
            else{
                return getKickPosY();
            }
        }

        int getPunchPosX(){
            if(pType == 1)
                return mPosX + Scaler*20;
            else
                return mPosX + Scaler*90;
        }
        int getPunchPosY(){
            return mPosY + Scaler*20;
        }
        int getKickPosX(){
            if(pType == 1)
                return mPosX + Scaler*20;
            else
                return mPosX + Scaler*90;
        }
        int getKickPosY(){
            return mPosY + Scaler*50;
        }

        int getState(){
            return state;
        }
        void erasePower(int pid){
            for(int i = 0 ; i < powerContainer.size(); i++){
                if(powerContainer[i].id == pid){
                    powerContainer.erase(powerContainer.begin() + i);
                    break;
                }
            }
        }

        void die(){
            // frames = 0;
            // state = 10;
            lost = 1;
            speedUP = 8;
            if(pType == 0){
                mPosX-=200;
            }
            mPosY = 610;
        }

        void win(){
            // frames = 0 ;
            won = 1;
            speedUP = 8;
        }

        void reset(){
            life = Life();
            powerhealth = PowerHealth();
            mPosX = pType==0? 300 : 1300;
            mPosY = 0;
            state = 0;
            frames = 0;
            speedUP = 4;
            performingState = 0;
            damagingState = 0;
            mVelX = 0;
            mVelY = 0;
            won = 0;
            lost = 0;
        }


    private:

        // keyprofile to use
        KeyProfile keyprofile;
        Life life;
        PowerHealth powerhealth;
		//The X and Y offsets of the dot
		int mPosX, mPosY;

		//The velocity of the dot
		double mVelX, mVelY;
        // The gravity
        int pType = 0 ;
        double mGravY = Scaler*.3;
        int state; // 0 for resting, 1 for ducking, 2 walking forward, 3 walking backward, 4 jumping, 5 punching, 6 kicking, 7 power, 8 getting hit stand
        int speedUP = 4;
        int frames;
        int performingState;
        int damagingState;
        int frameCounts[nStates];
        SDL_Rect gSpriteClips[ nStates][15];
};


Dot::Dot(int _pType, int *pBack)
{

    playBackPointer = pBack;
    pType = _pType;
    if(pType== 0){
         
        keyprofile = KeyProfile("./Player1/keys.txt");
        mPosX = 300;
        mPosY = 0;
    }
    else{
        keyprofile = KeyProfile("./Player2/keys.txt");
        mPosX = 1300;
        mPosY = 0;
    }
    //Initialize the offsets
    life = Life();
    powerhealth = PowerHealth();    

    //Initialize the velocity
    mVelX = 0;
    mVelY = 0;

    // state
    state = 0 ;
    frames = 0;
    performingState = 0;
    damagingState = 0;

    DOT_HEIGHT[0] = Scaler*110;
    DOT_WIDTH[0]  = Scaler*88;

    DOT_HEIGHT[1] = Scaler*70;
    DOT_WIDTH[1]  = Scaler*80;

    DOT_HEIGHT[2] = Scaler*110;
    DOT_WIDTH[2]  = Scaler*84;

    DOT_HEIGHT[3] = Scaler*110;
    DOT_WIDTH[3]  = Scaler*84;
    
    DOT_HEIGHT[4] = Scaler*100;
    DOT_WIDTH[4]  = Scaler*78;

    DOT_HEIGHT[5] = Scaler*110;
    DOT_WIDTH[5]  = Scaler*105;

    DOT_HEIGHT[6] = Scaler*70;
    DOT_WIDTH[6]  = Scaler*115;

    DOT_HEIGHT[7] = Scaler*120;
    DOT_WIDTH[7]  = Scaler*80;

    DOT_HEIGHT[8] = Scaler*115;
    DOT_WIDTH[8]  = Scaler*85;

    DOT_HEIGHT[9] = Scaler*75;
    DOT_WIDTH[9]  = Scaler*110;





    frameCounts[0] = 9 ;
    frameCounts[1] = 6;
    frameCounts[2] = 10;
    frameCounts[3] = 10;
    frameCounts[4] = 9 ;
    frameCounts[5] = 5;
    frameCounts[6] = 7; 
    frameCounts[7] = 7;
    frameCounts[8] = 4;
    frameCounts[9] = 4;
    frameCounts[10]= 7;
    frameCounts[11]= 9;


    // state 0 rects
    for(int i = 0 ; i < frameCounts[0]; i++){
			gSpriteClips[ 0 ][ i ].x =   88 * i;
			gSpriteClips[ 0 ][ i ].y =   220;
			gSpriteClips[ 0 ][ i ].w =  88;
			gSpriteClips[ 0 ][ i ].h = 110;
    }
    // state 1 rects
    for(int i = 0 ; i < frameCounts[1] ; i++){
			gSpriteClips[ 1 ][ i ].x =   80 * (i+2);
			gSpriteClips[ 1 ][ i ].y =   370;
			gSpriteClips[ 1 ][ i ].w =  80;
			gSpriteClips[ 1 ][ i ].h = 70;

    }
    // state 2 rects
    gSpriteClips[ 2 ][ 0 ].x =   0;
    gSpriteClips[ 2 ][ 0 ].y =   450;
    gSpriteClips[ 2 ][ 0 ].w =  76;
    gSpriteClips[ 2 ][ 0 ].h = 110;
    gSpriteClips[ 2 ][ 1 ].x =   76;
    gSpriteClips[ 2 ][ 1 ].y =   450;
    gSpriteClips[ 2 ][ 1 ].w =  64;
    gSpriteClips[ 2 ][ 1 ].h = 110;
    gSpriteClips[ 2 ][ 2 ].x =   140;
    gSpriteClips[ 2 ][ 2 ].y =   450;
    gSpriteClips[ 2 ][ 2 ].w =  60;
    gSpriteClips[ 2 ][ 2 ].h = 110;
    gSpriteClips[ 2 ][ 3 ].x =   200;
    gSpriteClips[ 2 ][ 3 ].y =   450;
    gSpriteClips[ 2 ][ 3 ].w =  55;
    gSpriteClips[ 2 ][ 3 ].h = 110;
    gSpriteClips[ 2 ][ 4 ].x =   255;
    gSpriteClips[ 2 ][ 4 ].y =   450;
    gSpriteClips[ 2 ][ 4 ].w =  67;
    gSpriteClips[ 2 ][ 4 ].h = 110;
    gSpriteClips[ 2 ][ 5 ].x =   322;
    gSpriteClips[ 2 ][ 5 ].y =   450;
    gSpriteClips[ 2 ][ 5 ].w =  60;
    gSpriteClips[ 2 ][ 5 ].h = 110;
    gSpriteClips[ 2 ][ 6 ].x =   382;
    gSpriteClips[ 2 ][ 6 ].y =   450;
    gSpriteClips[ 2 ][ 6 ].w =  54;
    gSpriteClips[ 2 ][ 6 ].h = 110;
    gSpriteClips[ 2 ][ 7 ].x =   436;
    gSpriteClips[ 2 ][ 7 ].y =   450;
    gSpriteClips[ 2 ][ 7 ].w =  70;
    gSpriteClips[ 2 ][ 7 ].h = 110;
    gSpriteClips[ 2 ][ 8 ].x =   506;
    gSpriteClips[ 2 ][ 8 ].y =   450;
    gSpriteClips[ 2 ][ 8 ].w =  78;
    gSpriteClips[ 2 ][ 8 ].h = 110;
    gSpriteClips[ 2 ][ 9 ].x =   584;
    gSpriteClips[ 2 ][ 9 ].y =   450;
    gSpriteClips[ 2 ][ 9 ].w =  84;
    gSpriteClips[ 2 ][ 9 ].h = 110;
    //walking backward-state 3
    gSpriteClips[ 3 ][ 9].x =   0;
    gSpriteClips[ 3 ][ 9 ].y =   450;
    gSpriteClips[ 3 ][ 9 ].w =  76;
    gSpriteClips[ 3 ][ 9 ].h = 110;
    gSpriteClips[ 3 ][ 8 ].x =   76;
    gSpriteClips[ 3 ][ 8 ].y =   450;
    gSpriteClips[ 3 ][ 8 ].w =  64;
    gSpriteClips[ 3 ][ 8 ].h = 110;
    gSpriteClips[ 3 ][ 7 ].x =   140;
    gSpriteClips[ 3 ][ 7 ].y =   450;
    gSpriteClips[ 3 ][ 7 ].w =  60;
    gSpriteClips[ 3 ][ 7 ].h = 110;
    gSpriteClips[ 3 ][ 6 ].x =   200;
    gSpriteClips[ 3 ][ 6 ].y =   450;
    gSpriteClips[ 3 ][ 6 ].w =  55;
    gSpriteClips[ 3 ][ 6 ].h = 110;
    gSpriteClips[ 3 ][ 5 ].x =   255;
    gSpriteClips[ 3 ][ 5 ].y =   450;
    gSpriteClips[ 3 ][ 5 ].w =  67;
    gSpriteClips[ 3 ][ 5 ].h = 110;
    gSpriteClips[ 3 ][ 4 ].x =   322;
    gSpriteClips[ 3 ][ 4 ].y =   450;
    gSpriteClips[ 3 ][ 4 ].w =  60;
    gSpriteClips[ 3 ][ 4 ].h = 110;
    gSpriteClips[ 3 ][ 3 ].x =   382;
    gSpriteClips[ 3 ][ 3 ].y =   450;
    gSpriteClips[ 3 ][ 3 ].w =  54;
    gSpriteClips[ 3 ][ 3 ].h = 110;
    gSpriteClips[ 3 ][ 2 ].x =   436;
    gSpriteClips[ 3 ][ 2 ].y =   450;
    gSpriteClips[ 3 ][ 2 ].w =  70;
    gSpriteClips[ 3 ][ 2 ].h = 110;
    gSpriteClips[ 3 ][ 1 ].x =   506;
    gSpriteClips[ 3 ][ 1 ].y =   450;
    gSpriteClips[ 3 ][ 1 ].w =  78;
    gSpriteClips[ 3 ][ 1 ].h = 110;
    gSpriteClips[ 3 ][ 0 ].x =   584;
    gSpriteClips[ 3 ][ 0 ].y =   450;
    gSpriteClips[ 3 ][ 0 ].w =  84;
    gSpriteClips[ 3 ][ 0 ].h = 110;
    //state 4 jumping 
    gSpriteClips[ 4 ][ 0 ].x =   0;
    gSpriteClips[ 4 ][ 0 ].y =   970;
    gSpriteClips[ 4 ][ 0 ].w =  78;
    gSpriteClips[ 4 ][ 0 ].h = 100;
    gSpriteClips[ 4 ][ 1 ].x =   78;
    gSpriteClips[ 4 ][ 1 ].y =   900;
    gSpriteClips[ 4 ][ 1 ].w =  128-78;
    gSpriteClips[ 4 ][ 1 ].h = 1062-900;
    gSpriteClips[ 4 ][ 2 ].x =   128;
    gSpriteClips[ 4 ][ 2 ].y =   945;
    gSpriteClips[ 4 ][ 2 ].w =  194-128;
    gSpriteClips[ 4 ][ 2 ].h = 1065-945;
    gSpriteClips[ 4 ][ 3 ].x =   194;
    gSpriteClips[ 4 ][ 3 ].y =   970;
    gSpriteClips[ 4 ][ 3 ].w =  270-194;
    gSpriteClips[ 4 ][ 3 ].h = 1066-970;
    gSpriteClips[ 4 ][ 4 ].x =   270;
    gSpriteClips[ 4 ][ 4 ].y =   985;
    gSpriteClips[ 4 ][ 4 ].w =  342-270;
    gSpriteClips[ 4 ][ 4 ].h = 1065-985;
    gSpriteClips[ 4 ][ 5 ].x =   342;
    gSpriteClips[ 4 ][ 5 ].y =   970;
    gSpriteClips[ 4 ][ 5 ].w =  416-342;
    gSpriteClips[ 4 ][ 5 ].h = 1066-970;
    gSpriteClips[ 4 ][ 6 ].x =   416;
    gSpriteClips[ 4 ][ 6 ].y =   945;
    gSpriteClips[ 4 ][ 6 ].w =  490-416;
    gSpriteClips[ 4 ][ 6 ].h = 1065-945;
    gSpriteClips[ 4 ][ 7 ].x =   490;
    gSpriteClips[ 4 ][ 7 ].y =   945;
    gSpriteClips[ 4 ][ 7 ].w =  556-490;
    gSpriteClips[ 4 ][ 7 ].h = 1065-945;
    gSpriteClips[ 4 ][ 8 ].x =   556;
    gSpriteClips[ 4 ][ 8 ].y =   970;
    gSpriteClips[ 4 ][ 8 ].w =  630-556;
    gSpriteClips[ 4 ][ 8 ].h = 100;
    
    
    // punching state 5
    gSpriteClips[ 5 ][ 0 ].x =   0;
    gSpriteClips[ 5 ][ 0 ].y =   1300;
    gSpriteClips[ 5 ][ 0 ].w =  80;
    gSpriteClips[ 5 ][ 0 ].h = 110;
    gSpriteClips[ 5 ][ 1 ].x =   80;
    gSpriteClips[ 5 ][ 1 ].y =   1300;
    gSpriteClips[ 5 ][ 1 ].w =  80;
    gSpriteClips[ 5 ][ 1 ].h = 110;
    gSpriteClips[ 5 ][ 2 ].x =   160;
    gSpriteClips[ 5 ][ 2 ].y =   1300;
    gSpriteClips[ 5 ][ 2 ].w =  105;
    gSpriteClips[ 5 ][ 2 ].h = 110;
    gSpriteClips[ 5 ][ 3 ].x =   265;
    gSpriteClips[ 5 ][ 3 ].y =   1300;
    gSpriteClips[ 5 ][ 3 ].w =  105;
    gSpriteClips[ 5 ][ 3 ].h = 110;
    gSpriteClips[ 5 ][ 4 ].x =   370;
    gSpriteClips[ 5 ][ 4 ].y =   1300;
    gSpriteClips[ 5 ][ 4 ].w =  80;
    gSpriteClips[ 5 ][ 4 ].h = 110;
    //state 6
    gSpriteClips[ 6 ][ 0 ].x =   0;
    gSpriteClips[ 6 ][ 0 ].y =   2350;
    gSpriteClips[ 6 ][ 0 ].w =  85;
    gSpriteClips[ 6 ][ 0 ].h =  70;
    gSpriteClips[ 6 ][ 1 ].x =   85;
    gSpriteClips[ 6 ][ 1 ].y =   2350;
    gSpriteClips[ 6 ][ 1 ].w =  175-85;
    gSpriteClips[ 6 ][ 1 ].h =  70;
    gSpriteClips[ 6 ][ 2 ].x =   175;
    gSpriteClips[ 6 ][ 2 ].y =   2350;
    gSpriteClips[ 6 ][ 2 ].w =  290-175;
    gSpriteClips[ 6 ][ 2 ].h =  70;
    gSpriteClips[ 6 ][ 3 ].x =   290;
    gSpriteClips[ 6 ][ 3 ].y =   2350;
    gSpriteClips[ 6 ][ 3 ].w =  410-290;
    gSpriteClips[ 6 ][ 3 ].h =  70;
    gSpriteClips[ 6 ][ 4 ].x =   410;
    gSpriteClips[ 6 ][ 4 ].y =   2350;
    gSpriteClips[ 6 ][ 4 ].w =  525-410;
    gSpriteClips[ 6 ][ 4 ].h =  70;
    gSpriteClips[ 6 ][ 5 ].x =   525;
    gSpriteClips[ 6 ][ 5 ].y =   2350;
    gSpriteClips[ 6 ][ 5 ].w =  615-525;
    gSpriteClips[ 6 ][ 5 ].h =  70;
    gSpriteClips[ 6 ][ 6 ].x =   615;
    gSpriteClips[ 6 ][ 6 ].y =   2350;
    gSpriteClips[ 6 ][ 6 ].w =  695-615;
    gSpriteClips[ 6 ][ 6 ].h = 70;

    //state-7 power 
    int temp[7] = {80 , 80, 85, 85, 90, 85, 80};
    gSpriteClips[ 7 ][ 0 ].x =   0;
    gSpriteClips[ 7 ][ 0 ].y =   3105;
    gSpriteClips[ 7 ][ 0 ].w =  80;
    gSpriteClips[ 7 ][ 0 ].h = 120;
    for(int i = 1; i < frameCounts[7];i++){
        gSpriteClips[7][i] = gSpriteClips[7][i-1];
        gSpriteClips[7][i].x +=  gSpriteClips[7][i-1].w;
        gSpriteClips[7][i].w = temp[i];

    }

    //state-8 getting hit stand

    int temp8[5] = {380, 455, 535, 610, 690};
    gSpriteClips[ 8 ][ 0 ].x =   380;
    gSpriteClips[ 8 ][ 0 ].y =   8552;
    gSpriteClips[ 8 ][ 0 ].w =  455-380;
    gSpriteClips[ 8 ][ 0 ].h = 8668-8552;
    for(int i = 1; i < frameCounts[8];i++){
        gSpriteClips[8][i] = gSpriteClips[8][i-1];
        gSpriteClips[8][i].x +=  gSpriteClips[8][i-1].w;
        gSpriteClips[8][i].w = temp8[i+1]-temp8[i];

    }

    int temp9[5] = {600, 690, 815, 920, 1010};
    gSpriteClips[ 9 ][ 0 ].x =   600;
    gSpriteClips[ 9 ][ 0 ].y =   8340;
    gSpriteClips[ 9 ][ 0 ].w =  690-600;
    gSpriteClips[ 9 ][ 0 ].h = 8415-8340;
    for(int i = 1; i < frameCounts[8];i++){
        gSpriteClips[9][i] = gSpriteClips[9][i-1];
        gSpriteClips[9][i].x +=  gSpriteClips[9][i-1].w;
        gSpriteClips[9][i].w = temp9[i+1]-temp9[i];

    }

    int temp10[8] = {0, 87, 210, 334, 459, 597, 722,860};
    gSpriteClips[ 10 ][ 0 ].x =   0;
    gSpriteClips[ 10 ][ 0 ].y =   8928;
    gSpriteClips[ 10 ][ 0 ].w =  temp10[1];
    gSpriteClips[ 10 ][ 0 ].h = 9035 - 8928;
    for(int i = 1; i < frameCounts[10];i++){
        gSpriteClips[10][i] = gSpriteClips[10][i-1];
        gSpriteClips[10][i].x +=  gSpriteClips[10][i-1].w;
        gSpriteClips[10][i].w = temp10[i+1]-temp10[i];

    }

    int temp11[10] = {0, 80, 160, 234, 313, 391 , 471, 549, 628, 700};
    gSpriteClips[ 11 ][ 0 ].x =   0;
    gSpriteClips[ 11 ][ 0 ].y =   9547;
    gSpriteClips[ 11 ][ 0 ].w =  temp11[1];
    gSpriteClips[ 11 ][ 0 ].h = 9668 - 9547;
    for(int i = 1; i < frameCounts[11];i++){
        gSpriteClips[11][i] = gSpriteClips[11][i-1];
        gSpriteClips[11][i].x +=  gSpriteClips[11][i-1].w;
        gSpriteClips[11][i].w = temp11[i+1]-temp11[i];

    }


}

void Dot::handleEvent( SDL_Event& e )
{

    if(!isAlive()){
        return;
    }
    //If a key was pressed

	if( e.type == SDL_KEYDOWN && e.key.repeat == 0 )
    {
        //Adjust the velocity
        if (e.key.keysym.sym == keyprofile.up) {
            // if(damagingState == 1)
            //     return;
            if (performingState == 1)
                return;
            mVelY -= Scaler * 4;
            state = 4;
            frames = 0;
            performingState = 1;
        } else if (e.key.keysym.sym == keyprofile.down) {
            // if(damagingState == 1)
            //     return;
            // mVelY += 10;
            mPosY += DOT_HEIGHT[state] - DOT_HEIGHT[1];
            state = 1;
            frames = 0;
        } else if (e.key.keysym.sym == keyprofile.left) {
            // if(performingState)
            //     return;
            mVelX -= DOT_VEL;
            state = 3;
            frames = 0;
        } else if (e.key.keysym.sym == keyprofile.right) {
            // if(performingState)
            //     return;
            mVelX += DOT_VEL;
            state = 2;
            frames = 0;
        } else if (e.key.keysym.sym == keyprofile.p1) {
            if (performingState)
                return;
            if( state != 0)
                return;
            performingState = 1;
            q.push({0, pType, SDL_GetTicks()});
            state = 5;
            frames = 0;
            if(pType == 1)
                mPosX -= DOT_WIDTH[state] - DOT_WIDTH[0];
        } else if (e.key.keysym.sym == keyprofile.p2) {
            if (state != 1)
                return;
            performingState = 1;
            q.push({1, pType, SDL_GetTicks()});
            state = 6;
            frames = 0;
            mPosY += 5;
            if(pType == 1)
                mPosX -= DOT_WIDTH[state] - DOT_WIDTH[1];
        } else if (e.key.keysym.sym == keyprofile.p3) {
            if (state != 0)
                return;
            if(powerhealth.getHealth() == 0){
                return;
            }
            powerhealth.decrease();
            performingState = 1;
            state = 7;
            frames = 0;
            powerContainer.push_back(Power(pType,mPosX + 150, mPosY + 190, powerId++));
        }

        
    }
    //If a key was released

    else if( e.type == SDL_KEYUP && e.key.repeat == 0 )
    {
        //Adjust the velocity
        
        if (e.key.keysym.sym == keyprofile.up) {
            //  if(performingState == 1)
            //     break;
            // mVelY-=10;
            // state=4;
            // performingState = 1;
            // frames=0;
        } else if (e.key.keysym.sym == keyprofile.down) {
            // mVelY -= 10; 
            // if(performingState)
            // return;
            mPosY += DOT_HEIGHT[state] - DOT_HEIGHT[0];
            if(pType)
            mPosX += DOT_WIDTH[state] - DOT_WIDTH[1];
            state = 0;
            frames = 0;
        } else if (e.key.keysym.sym == keyprofile.left) {
            if(state!=3){
                mVelX += DOT_VEL;
                return;
            }
            // if(performingState)
            //     return;

            mVelX += DOT_VEL;
            // mVelX = 0;
            state = 0;
            frames = 0;
            performingState = 0;
        } else if (e.key.keysym.sym == keyprofile.right) {
            if(state!=2){
                mVelX -= DOT_VEL;
                return;
            }
            // if(performingState)
            //     return;
            mVelX -= DOT_VEL;
            state = 0;
            frames = 0;
            performingState  = 0;
        } else if (e.key.keysym.sym == keyprofile.p1) {
            // Additional handling for SDLK_j if needed

        } else if (e.key.keysym.sym == keyprofile.p2) {
            // Additional handling for SDLK_k if needed
        } else if (e.key.keysym.sym == keyprofile.p3) {
            // Additional handling for SDLK_l if needed
        }

    }
}

bool Dot::handleFightingEvent(Event e){
    int diff = SDL_GetTicks() - e.getTime();
    switch (e.getType())
    {
    case 0:
        
       
        if(diff < 20){
            return 0; 
        }
        //check collission
        if(collission(e.posX, e.posY)){
            cout << "Punch Collied" << endl;
            life.decreasePunch();
            if(state == 1 || state == 6){
                state = 9;
            }
            else
                state = 8;
            frames = 0;
            performingState = 0;
            damagingState = 1;
        }
        else{
            cout << "Punch not Collieded" << endl;
        }
        return 1;
        //deal damage
        break;
    case 1:
        if(diff < 50){
            return 0; 
        }
        //check collission
        if(collission(e.posX, e.posY)){
            cout << "Kick Collied" << endl;
            life.decreaseKick();
            if(state == 1 || state == 6){
                state = 9;
            }
            else
                state = 8;
            frames = 0;
            performingState = 0;
            damagingState = 1;
        }
        else{
            cout << "Kick not Collieded" << endl;
        }
        return 1;
    
        break;

    case 2:
        if(collission(e.posX, e.posY) and state!= 4){
            life.decreasePower();
            cout << "Power Collied" << endl;
            if(state == 1 || state == 6){
                state = 9;
            }
            else
                state = 8;
            frames = 0;
            performingState = 0;
            damagingState = 1;
            return 1;
        }
        else{
            cout << "Power not Collieded" << endl;
        }
        return 0;

        break;
    default:
        break;
    }
}

void Dot::move(int restrictionX)
{

    //Adjust gravity to velocity
    mVelY += mGravY;
    

    //Move the dot left or right
     if(pType == 0){
        // if(mPosX > restrictionX + 50)
        //     mPosX-= Scaler*50;
        if((mPosX + Scaler*mVelX <  restrictionX-10)||(mVelX < 0)){
            mPosX += Scaler*mVelX;
        }
    }
    else{
        // if(mPosX < restrictionX -100){
        //     mPosX+= Scaler*50;
        // }
        if((mPosX + Scaler*mVelX >  restrictionX - 50)||(mVelX > 0)){
            mPosX += Scaler*mVelX;
        }
    }


    //If the dot went too far to the left or right
    if( ( mPosX < 0 ) || ( mPosX + DOT_WIDTH[state] > 1920 ) )
    {
        //Move back
        mPosX -= Scaler*mVelX;
    }
    
   
    //Move the dot up or down
    mPosY += Scaler*mVelY;

    //If the dot went too far up or down
    if( ( mPosY < 0 ) || ( mPosY + DOT_HEIGHT[state]> 960 ) )
    {
        //Move back
        mPosY = 960 - DOT_HEIGHT[state];
        mVelY = 0;
    }

    for(int i = 0 ; i < powerContainer.size();){
        powerContainer[i].move();
        if(powerContainer[i].mPosX > swidth || powerContainer[i].mPosX < 0){
            powerContainer.erase(powerContainer.begin()+i);
        }
        else{
            q.push(Event(2, pType , powerContainer[i].id ,powerContainer[i].mPosX + powerContainer[i].POWER_WIDTH,powerContainer[i].mPosY));
            i++;
        }
    }
}
void Dot::updateFrames(){
    ++frames;

    //Cycle animation
    if( frames / speedUP >= frameCounts[state] )
    {
        frames = 0;
    }
    if(frames == 0 and won == 1){
        state = 11;
    }
    if(frames == 0 and lost == 1){
        state = 10;
    }
    if(frames == 0 and state == 10){
        frames = (frameCounts[state]-1)*speedUP ;
        return;
    }

    if(frames == 0 and state == 11){
        frames = (frameCounts[state]-3)*speedUP ;
        return;
    }
    // Handle punching and kicking return to initial state if finished punching 
    if(frames == 0 and performingState == 1){
        // If kicking
        if(state == 6)
        {
            // return to seating
            if(pType == 1)
                mPosX +=  DOT_WIDTH[6] - DOT_WIDTH[0];
            state = 1;
        }
        // If punchning or providing power or jumping 
        if(state == 5 || state == 7 || state == 4 ){
            if(pType == 1 && state!=4)
                mPosX += DOT_WIDTH[state] - DOT_WIDTH[0];
            state = 0;
            

        }
        performingState = 0;
    }
    if(frames == 0 and damagingState){
         if(state == 8){
            state = 0;
            damagingState = 0;
        }
        if(state == 9){
            state = 1;
            damagingState = 0;
        }
    }

}
void Dot::render(SDL_Renderer* gRenderer)
{
    SDL_Rect* currentClip = &gSpriteClips[state][ frames / speedUP ];
    // gDotTexture.render(gRenderer, mPosX, mPosY, currentClip,0.0,NULL,SDL_FLIP_NONE );
    SDL_Rect dest;
    
    dest.x = mPosX;                              
    dest.y = mPosY;
    dest.w = Scaler*currentClip->w;
    dest.h = Scaler*currentClip->h;
    // gDotTexture.setColor(255,255,255);
    // SDL_RenderCopy(gRenderer, gDotTexture.getTexture(), currentClip,&dest);
   // gDotTexture.setColor(255,255,0);
    if(pType){
        gDotTexture.setColor(100, 200, 100);
     
        SDL_RenderCopyEx(gRenderer, gDotTexture.getTexture(), currentClip,&dest, 0.0, NULL, SDL_FLIP_HORIZONTAL);
    }
    else{
        SDL_RenderCopyEx(gRenderer, gDotTexture.getTexture(), currentClip,&dest, 0.0, NULL, SDL_FLIP_NONE);
    }
    //Update screen
    //SDL_RenderPresent( gRenderer );

    //Go to next frame
    updateFrames();
    if(pType){
        gDotTexture.setColor(255, 50, 50);
    }
    for(auto &it: powerContainer){

        it.render(gRenderer, &gDotTexture);
    }

    //Show Life
    dest.x = 330;
    dest.y = 150;
    dest.w = life.getWidth();
    dest.h = life.getHeight();
    // gDotTexture.setColor(life.getRed(), life.getGreen(), 5);
    // gHealthTexture.loadFromFile(gRenderer,"./Player1/healthbar.png");
    // gHealthTexture.render(gRenderer, 100, 100);

    SDL_Rect pHealth;
    pHealth.x = 330;
    pHealth.y = 200;
    pHealth.w = powerhealth.getWidth();
    pHealth.h = powerhealth.getHeight();
    if(!pType){
        
        // SDL_RenderCopyEx(gRenderer, gDotTexture.getTexture(), &dest,&dest, 0.0, NULL, SDL_FLIP_NONE);
        SDL_SetRenderDrawColor( gRenderer, life.getRed(), life.getGreen(), 5, 0xFF ); 
        SDL_RenderFillRect( gRenderer, &dest );
        gHealthTexture.render(gRenderer, 150, 100);

        // power health 
        

        SDL_SetRenderDrawColor( gRenderer, 28, 46, 184, 0xFF ); 
        SDL_RenderFillRect( gRenderer, &pHealth );


    }
    else{
        
        dest.x = 1530- life.getWidth();
        
        SDL_SetRenderDrawColor( gRenderer, life.getRed(), life.getGreen(), 5, 0xFF ); 
        SDL_RenderFillRect( gRenderer, &dest );
        gHealthTexture.render(gRenderer, 1000, 100);
        // SDL_RenderCopyEx(gRenderer, gHealthTexture.getTexture(), &dest,&dest, 0.0, NULL, SDL_FLIP_HORIZONTAL);
        
        pHealth.x = 1530 - powerhealth.getWidth();

        SDL_SetRenderDrawColor( gRenderer, 28, 46, 184, 0xFF ); 
        SDL_RenderFillRect( gRenderer, &pHealth );
    }
    SDL_SetRenderDrawColor( gRenderer, 0x00, 0x00, 0x00, 0xFF ); 
    //Show the dot
	//gDotTexture.render(gRenderer, mPosX, mPosY );
}

