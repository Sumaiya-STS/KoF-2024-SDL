#include "Character.h"
#include <queue>
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
    void resetGame();

public:
    LTexture gBackground;
    LTexture mBackground;
    LTexture gTextTexture;
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

	return success;
}

void GamePlayback::handleMenu(SDL_Event &e){
    if( e.type == SDL_KEYDOWN && e.key.repeat == 0 )
    {
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
        if (e.key.keysym.sym == SDLK_RETURN) {
            if(menuPointer == 0){
                mode = 1;
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
        }
        // cout << e.key.keysym.sym << endl;
    }

}

void GamePlayback::handlePlayback(SDL_Event &e){
            //Move the dot
    if( e.type == SDL_KEYDOWN && e.key.repeat == 0 )
    {
        if (e.key.keysym.sym == SDLK_ESCAPE) {
            mode = 0;
            return;
        }
    }
    player1.handleEvent( e );
    player2.handleEvent( e );
    int curTime = SDL_GetTicks();

    
    
       
}
void GamePlayback::renderMenu(){
     //Clear screen
    string path = "./Menu/menu1.png";
    path[11] = menuPointer  + '0';
    SDL_Color textcolor= {250,250,250};
    mBackground.loadFromFile(gRenderer, path);
    gTextTexture.loadFromRenderedText(gRenderer,path,textcolor);
    SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
    SDL_RenderClear( gRenderer );
    mBackground.render(gRenderer, 0,0);
    gTextTexture.render(gRenderer, 500, 500);
    SDL_RenderPresent( gRenderer );


}
void GamePlayback::renderPlayback(){

    if(!player1.isAlive() and playBackPointer != 3){
        //change state to dead
        cout << "Player 1 is dead\n";
        player1.die();
        cout << "Showing animation\n";
        cout << "player 2 is winner\n";
        player2.win();
        playBackPointer = 3;
    }
    if(!player2.isAlive() and playBackPointer != 3){
        //change state to dead
        cout << "Player 2 is dead\n";
        player2.die();
        cout << "Showing animation\n";
        cout << "player 1 is winner\n";
        player1.win();
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
                success = player1.handleFightingEvent(fightingevent);
            }
            else{
                if(fightingevent.getType()!=2){
                    fightingevent.posX = player1.getPerformingPosX(fightingevent);
                    fightingevent.posY = player1.getPerformingPosY(fightingevent);
                }
                success = player2.handleFightingEvent(fightingevent);
            }
            if(fightingevent.getType()==2){
                if(success){
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
    if(playBackPointer == 3){
        countDown++;
        if(countDown == 300){
            countDown = 0 ;
            resetGame();
            mode = 0;
            playBackPointer = 0;
            cout << "Game Over" << endl;
            
            return;
        }
    }
    //Clear screen
    SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
    SDL_RenderClear( gRenderer );
    gBackground.render(gRenderer, 0,0);
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
    player1.reset();
    player2.reset();
}
