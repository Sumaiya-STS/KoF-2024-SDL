#include "Character.h"
#include <queue>
#include <map>
#include <vector>
#include <algorithm>
#include <SDL2/SDL_mixer.h>
struct LeaderboardEntry{
    string winnerName;
    string loserName;
    int winTime;

    string getTime(){
        string time = to_string(winTime/1000.0);
        int pos = 0;
        while (time[pos]!='.')
        {
            pos++;
        }
        while (time.length() > pos+2)
        {
            time.pop_back();
        }
        return time;
    }
    string toString(int id){
        string time = to_string(winTime);
        int pos = 0;
        string rank = to_string(id);
        while (time[pos]!='.')
        {
            pos++;
        }
        while (time.length() > pos+2)
        {
            time.pop_back();
        }
        
        return rank + "          " +winnerName + "         " + loserName + "         " + time;
    }

    bool operator<(const LeaderboardEntry& other) const {
        return winTime < other.winTime;
    }

    
};

class GamePlayback
{
private:
    /* data */
    SDL_Renderer* gRenderer;
    int mode; // mode 0 for main menu, mode 1 for play, mode 2 for leaderboard, mode 3 for instruction
    int menuPointer;
    int playBackPointer ; // 0 for pre-game phrase, 1 for game running,2 showing video, 3 for game phrase end
    int countDown = 0;
    bool quit;
    string currentP1Name="";
    string currentP2Name="";
    string winnerName;
    string loserName;
    int matchStartTime;
    int lastBackground = -1;
    vector < LeaderboardEntry > leaderboardData; 
    void resetGame();
    void readData();
    void writeData();

public:
    LTexture gBackground;
    LTexture mBackground;
    LTexture gTextTexture;
    LTexture gNameTexture;
    LTexture gLeaderTextTexture;
    LTexture winnerImageTexture;
    LTexture winnerTextTexture;
    int winnerImagePosY = 0;
    Mix_Music *gMusic = NULL;
    Mix_Chunk *clap;
    //Character
    Dot player1 = Dot(00, &playBackPointer);
    Dot player2 = Dot(01, &playBackPointer);
    GamePlayback(SDL_Renderer* _gRenderer);
    bool loadMedia();
    ~GamePlayback();
    void run();
    void handleMenu(SDL_Event &e);
    void handlePlayback(SDL_Event &e);
    void renderPlayback();
    void renderMenu();
};

GamePlayback::GamePlayback(SDL_Renderer* _gRenderer)
{
    mode = 0 ;
    menuPointer = 0;
    playBackPointer = 0;
    gRenderer = _gRenderer;
    if( !loadMedia() )
    {
        printf( "Failed to load media!\n" );
    }
    Mix_PlayMusic( gMusic, -1 );
    readData();
}

GamePlayback::~GamePlayback()
{
    player1.gDotTexture.free();
	player2.gDotTexture.free();
}

bool GamePlayback::loadMedia()
{
    //Main loop flag
	//Loading success flag
	bool success = true;

	//Load dot texture
	if( !player1.gDotTexture.loadFromFile( gRenderer, "./Sprites/Iori.png" ) )
	{
		printf( "Failed to load dot texture!\n" );
		success = false;
	}
    if( !player1.gHealthTexture.loadFromFile( gRenderer, "./Player1/healthbar.png" ) )
	{
		printf( "Failed to Health dot texture!\n" );
		success = false;
	}
    if( !player2.gHealthTexture.loadFromFile( gRenderer, "./Player2/healthbar.png" ) )
	{
		printf( "Failed to Health dot texture!\n" );
		success = false;
	}
	if( !player2.gDotTexture.loadFromFile( gRenderer, "./Sprites/Iori.png" ) )
	{
		printf( "Failed to load dot texture!\n" );
		success = false;
	}

	if(!gBackground.loadFromFile(gRenderer,"./Backgrounds/3.png")){
		printf( "Failed to load Game background texture!\n" );
		success = false;

	}
    if(!mBackground.loadFromFile(gRenderer,"./Menu/menu1.png")){
		printf( "Failed to load Menu background texture!\n" );
		success = false;

	}
    if(!winnerImageTexture.loadFromFile(gRenderer,"./Menu/ko.png")){
		printf( "Failed to load KO background texture!\n" );
		success = false;

	}
    gTextTexture.gFont = TTF_OpenFont("./Config/font.otf",78);
	gNameTexture.gFont = TTF_OpenFont("./Config/font.otf",58);
    gLeaderTextTexture.gFont = TTF_OpenFont("./Config/franciosone.ttf",58);
    winnerTextTexture.gFont = TTF_OpenFont("./Config/font.otf",100);

    gMusic = Mix_LoadMUS( "Audio/intro.wav" );
    if( gMusic == NULL )
    {
        printf( "Failed to load beat music! SDL_mixer Error: %s\n", Mix_GetError() );
        success = false;
    }
    clap = Mix_LoadWAV("Audio/clap.wav");

	return success;
}

void GamePlayback::handleMenu(SDL_Event &e){
    if( e.type == SDL_KEYDOWN && e.key.repeat == 0 )
    {
        if(menuPointer>=0 and menuPointer <=5){
            if (e.key.keysym.sym == SDLK_DOWN) {
                menuPointer++;
                menuPointer%= 4;
            }
            if (e.key.keysym.sym == SDLK_UP) {
                menuPointer--;
                if(menuPointer < 0)
                    menuPointer+= 4;
            }
            if (e.key.keysym.sym == SDLK_RIGHT) {
                if(menuPointer==4)
                    menuPointer = 5;
            }
            if (e.key.keysym.sym == SDLK_LEFT) {
            
                if(menuPointer==5)
                    menuPointer = 4;
            }
        }
        if (e.key.keysym.sym == SDLK_RETURN) {
            if(menuPointer == 0){
                if(playBackPointer == 0)
                    menuPointer = 6;
                else{
                    Mix_HaltMusic();
                    mode = 1;
                }
                return;
            }
            if(menuPointer == 1){
                menuPointer = 8;
                return;
            }
            if (menuPointer == 2)
            {
                menuPointer = 10;
                return;
            }
            
            if(menuPointer == 3)
            {
                menuPointer = 4;
                return;
            }
            if(menuPointer == 4){
                quit = true;
            }
           if(menuPointer == 5){
                menuPointer = 0;
            }
            if(menuPointer == 6){
                if(currentP1Name.empty())
                    return;
                menuPointer = 7;
                return;
            }
            if(menuPointer == 7){
                if(currentP2Name.empty())
                    return;
                playBackPointer = 1;
                mode = 1;
                matchStartTime = SDL_GetTicks();
                return;
            }
        }
        if(e.key.keysym.sym == SDLK_ESCAPE){
            if(menuPointer == 6){
                menuPointer = 0;
            }
            if(menuPointer == 7)
            {
                menuPointer = 6;
            }
            if(menuPointer == 8 or menuPointer == 9){
                menuPointer = 1;
            }
            if(menuPointer == 10 or menuPointer == 11){
                menuPointer = 2;
            }

        }
        if(e.key.keysym.sym>='a' and e.key.keysym.sym<='z'){
            if(menuPointer == 6 and mode == 0){
                currentP1Name+=e.key.keysym.sym;
                if(currentP1Name.length()>15)
                    currentP1Name.pop_back();
            }
            if(menuPointer == 7 and mode == 0){
                currentP2Name+=e.key.keysym.sym;
                if(currentP2Name.length()>15)
                    currentP2Name.pop_back();
            }
        }
        if(e.key.keysym.sym == SDLK_BACKSPACE){
            if(menuPointer == 6 and mode == 0){
                if(!currentP1Name.empty())
                    currentP1Name.pop_back();
            }
            if(menuPointer == 7 and mode == 0){
                if(!currentP2Name.empty())
                    currentP2Name.pop_back();
            }
        }
        // cout << e.key.keysym.sym << endl;
    }


    if( e.type == SDL_MOUSEMOTION  )
    {
        //Get mouse position
        int x, y;
        SDL_GetMouseState( &x, &y );
        if(menuPointer >=0 and menuPointer <=3){
            if(x >= 46 and x<= 420){
                if(y >= 462 and y<= 548){
                    menuPointer = 0;
                    return;
                }
                if(y >= 584 and y<= 660){
                    menuPointer = 1;
                    return;
                }
                if(y >= 700 and y<= 786){
                    menuPointer = 2;
                    return;
                }
                if(y >= 820 and y<= 903){
                    menuPointer = 3;
                    return;
                }
            }
        }
        else
        if(menuPointer >=4 and menuPointer <=5){
            if(y>=493 and y <= 555){
                if(x>=714 and x<= 814){
                    menuPointer = 4;
                }
                if(x>=994 and x<= 1065){
                    menuPointer = 5;
                }
            }

        }
        if(menuPointer >=6 and menuPointer <=7){

        }
        if(menuPointer >=8 and menuPointer <=9){
            if(x>=105 and x <= 263 and y>=600 and y<= 690){
                menuPointer = 9;
            }
            else{
                menuPointer = 8;
            }
        }
        if(menuPointer >=10 and menuPointer <=11){
            if(x>=105 and x <= 263 and y>=600 and y<= 690){
                menuPointer = 11;
            }
            else{
                menuPointer = 10;
            }

        }
    }
    if(e.type == SDL_MOUSEBUTTONDOWN){
        if(menuPointer == 0){
            if(playBackPointer == 0)
                menuPointer = 6;
            else{
                Mix_HaltMusic();
                mode = 1;
            }
            return;
        }
        if(menuPointer == 1){
            
            menuPointer = 8;
            return;
        }
        if (menuPointer == 2)
        {
            menuPointer = 10;
            return;
        }
        
        if(menuPointer == 3)
        {
            menuPointer = 4;
            return;
        }
        if(menuPointer == 4){
            quit = true;
        }
        if(menuPointer == 5){
            menuPointer = 0;
        }
        if(menuPointer == 6){
            menuPointer = 7;
            return;
        }
        if(menuPointer == 7){
            playBackPointer = 1;
            mode = 1;
            matchStartTime = SDL_GetTicks();
            return;
        }
        if(menuPointer == 9){
            menuPointer = 1;
            return;
        }
        if(menuPointer == 11){
            menuPointer = 2;
            return;
        }

    }

}

void GamePlayback::handlePlayback(SDL_Event &e){
            //Move the dot
    if( e.type == SDL_KEYDOWN && e.key.repeat == 0 )
    {
        if (e.key.keysym.sym == SDLK_ESCAPE) {
            mode = 0;
            menuPointer = 0;
            return;
        }
    }
    player1.handleEvent( e );
    player2.handleEvent( e );
    int curTime = SDL_GetTicks();

    
    
       
}
void GamePlayback::renderMenu(){
     //Clear screen
    if(lastBackground!=menuPointer){
        string path = "./Menu/menu";
        path+= to_string(menuPointer);
        path+=".png";
        mBackground.loadFromFile(gRenderer, path);
        lastBackground = menuPointer;
    }
    SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
    SDL_RenderClear( gRenderer );
    mBackground.render(gRenderer, 0,0);
    SDL_Color textcolor= {200,200,200};
    if(menuPointer == 6){
        string text = "Enter Player 1 Name";
        gTextTexture.loadFromRenderedText(gRenderer,text,textcolor);
        gTextTexture.render(gRenderer, 400, 400);
        if(!currentP1Name.empty()){
            gNameTexture.loadFromRenderedText(gRenderer,currentP1Name,textcolor);
            gNameTexture.render(gRenderer, 760 - currentP1Name.length()*10, 560);
        }
    }
    if(menuPointer == 7){
        string text = "Enter Player 2 Name";
        gTextTexture.loadFromRenderedText(gRenderer,text,textcolor);
        gTextTexture.render(gRenderer, 400, 400);
        if(!currentP2Name.empty()){
            gNameTexture.loadFromRenderedText(gRenderer,currentP2Name,textcolor);
            gNameTexture.render(gRenderer, 760 - currentP2Name.length()*10, 560);
        }
    }

    if(menuPointer == 8 or menuPointer == 9){
        string text = "";
        int rank = 0;
        textcolor= {190,190,190};
        sort(leaderboardData.begin(), leaderboardData.end());
        for(auto it: leaderboardData){
            // text = it.toString(rank++); 
            gLeaderTextTexture.loadFromRenderedText(gRenderer, to_string(rank+1), textcolor);
            gLeaderTextTexture.render(gRenderer, 500 , 440 + rank*80);

            gLeaderTextTexture.loadFromRenderedText(gRenderer, it.winnerName, textcolor);
            gLeaderTextTexture.render(gRenderer, 780 , 440 + rank*80);

            gLeaderTextTexture.loadFromRenderedText(gRenderer, it.loserName, textcolor);
            gLeaderTextTexture.render(gRenderer, 1120 , 440 + rank*80);

            gLeaderTextTexture.loadFromRenderedText(gRenderer,it.getTime(), textcolor);
            gLeaderTextTexture.render(gRenderer, 1500 , 440 + rank*80);
            rank++;
            if(rank == 6)
                break;
        }
        
    }
    
    SDL_RenderPresent( gRenderer );


}
void GamePlayback::renderPlayback(){

    if(!player1.isAlive() and playBackPointer != 3){
        //change state to dead
        cout << "Player 1 is dead\n";
        player1.die();
        cout << "Showing animation\n";
        cout << "player 2 is winner\n";
        winnerName = currentP2Name;
        loserName = currentP1Name;
        player2.win();
        Mix_PlayChannel( -1, clap, 0 );
        playBackPointer = 3;
    }
    if(!player2.isAlive() and playBackPointer != 3){
        //change state to dead
        cout << "Player 2 is dead\n";
        player2.die();
        cout << "Showing animation\n";
        cout << "player 1 is winner\n";
        winnerName = currentP1Name;
        loserName = currentP2Name;
        player1.win();
        Mix_PlayChannel( -1, clap, 0 );
        playBackPointer = 3;
    }
    if(playBackPointer != 3){
        if(!q.empty()){
            Event fightingevent = q.front();
            q.pop();
            bool success = false;

            
            if(fightingevent.getMaster() == 1){
                if(fightingevent.getType()!=2){
                    fightingevent.posX = player2.getPerformingPosX(fightingevent);
                    fightingevent.posY = player2.getPerformingPosY(fightingevent);
                }
                if(fightingevent.getType()!=2 ){
              
                    success = player1.handleFightingEvent(fightingevent);
                }
                else{
                  //  if(powermap[{fightingevent.getMaster(), fightingevent.getTime()}]==0)
                        success = player1.handleFightingEvent(fightingevent);

                }
            }
            else{
                if(fightingevent.getType()!=2){
                    fightingevent.posX = player1.getPerformingPosX(fightingevent);
                    fightingevent.posY = player1.getPerformingPosY(fightingevent);
                }
               // if(fightingevent.getType()!=2 or powermap[{fightingevent.getMaster(), fightingevent.getTime()!=1}])
                    success = player2.handleFightingEvent(fightingevent);
            }
            if(fightingevent.getType()==2){
                if(success){
                    powermap[{fightingevent.getMaster(), fightingevent.getTime()}] = 1;
                    if(fightingevent.getMaster() == 1){
                        player2.erasePower(fightingevent.getTime());
                    }
                    else{
                        player1.erasePower(fightingevent.getTime());
                    }
                }
            }

            if(!success and fightingevent.getType()!=2){
                q.push(fightingevent);
            }
        }
        player1.move(player2.getPosX() - player2.Scaler*45);
        player2.move(player1.getPosX() + player1.DOT_WIDTH[player1.getState()] - 60);


    }
    //Clear screen
    SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
    SDL_RenderClear( gRenderer );
    gBackground.render(gRenderer, 0,0);
    if(playBackPointer == 3){
        countDown++;
        winnerImageTexture.render(gRenderer, 600, 100 + winnerImagePosY);
        winnerImagePosY+=4;
        winnerImagePosY = min(winnerImagePosY, 500);
        SDL_Color textColor = {200 , 200 ,200};
        winnerTextTexture.loadFromRenderedText(gRenderer,winnerName + " wins !", textColor);
        winnerTextTexture.render(gRenderer,600, 1000-winnerImagePosY);
        if(countDown == 300){
            countDown = 0 ;
            resetGame();
            mode = 0;
            playBackPointer = 0;
            menuPointer = 0;
            cout << "Game Over in " << SDL_GetTicks() - matchStartTime << endl;
            leaderboardData.push_back({winnerName, loserName,(int)SDL_GetTicks() - matchStartTime});
            writeData();
            return;
        }
    }
    
    //Render objects
    player1.render(gRenderer);
    player2.render(gRenderer);
    //Update screen
    SDL_RenderPresent( gRenderer );

}

void GamePlayback::run(){
    quit = false;
    //Event handler
    SDL_Event e;
    //The dot that will be moving around on the screen
    //While application is running
    while( !quit )
    {
        //Handle events on queue
        while( SDL_PollEvent( &e ) != 0 )
        {
            //User requests quit
            if( e.type == SDL_QUIT )
            {
                quit = true;
            }
            if(mode == 1){
                handlePlayback(e);
            }
            else{
                handleMenu(e);
            }
        }
        if(mode == 1){
            renderPlayback();
        }
        else{
            renderMenu();
        }

    }
}

void GamePlayback::resetGame(){
    powermap.clear();
    player1.reset();
    player2.reset();
    currentP1Name.clear();
    currentP2Name.clear();
    while (!q.empty())
    {
        q.pop();
    }
    
}

void GamePlayback::readData(){
    //reading leaderboard data if any
    ifstream fin("Config/leader.in",ios::in);
    int n;
    fin >> n;
    cout << n << endl;
    while(n--){
        string a,b;
        int t;
        fin >> a >> b >> t;
        leaderboardData.push_back({a,b,t});
    }
}

void GamePlayback::writeData(){
    //reading leaderboard data if any
    ofstream fout("Config/leader.in",ios::out);
    fout << leaderboardData.size() <<"\n";
    for(auto it: leaderboardData){
        fout << it.winnerName << " " << it.loserName << " " << it.winTime << "\n";
    }
}