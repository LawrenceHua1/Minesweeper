
//
// Disclaimer:
// ----------
//
// This code will work only if you selected window, graphics and audio.
//
// Note that the "Run Script" build phase will copy the required frameworks
// or dylibs to your application bundle so you can execute it on any OS X
// computer.
//
// Your resource files (images, sounds, fonts, ...) are also copied to your
// application bundle. To get the path to these resources, use the helper
// function `resourcePath()` from ResourcePath.hpp
//

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include "ResourcePath.hpp"
using namespace std;


const int COUNTER_ID = 401;
const int RESTART_ID = 402;
const int DEBUG_ID = 403;
const int TEST1_ID = 404;
const int TEST2_ID = 405;

const int TILE_WIDTH = 32;
const int TILE_HEIGHT = 32;
const int MAX_TILES = 400;
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
const int BOARD_WIDTH = 800;
const int BOARD_HEIGHT = 512;
const int BUTTON_HEIGHT = 64;
const int BUTTON_WIDTH = 64;
const int TILES_PER_ROW = 25;
const int ROWS_PER_BOARD = 16;
const int RESTART_X = 368;
const int DEBUG_X = 496;
const int TEST1_X = 560;
const int TEST2_X = 624;

const int STATE_HIDDEN = 0;
const int STATE_REVEALED = 1;
const int STATE_FLAGGED = 2;

const int GAMEMODE_NORMAL = 0;
const int GAMEMODE_DEBUG = 1;
const int GAMEMODE_OVER = 2;
const int GAMEMODE_WIN = 3;

const int MAX_MINES = 50;

const int RC_OK = 0;
const int RC_FLAGGED = 1;
const int RC_ISAMINE = 2;
const int RC_ERROR = -1;

class Tile
{
public:
    int currentState;
    bool isAMine;
    int numberOfAdjMines;
};

class Board
{
public:
    Tile tiles[MAX_TILES];
    int gameMode;
    int mineCounter;
    int totalFlags;
    
    void ResetBoard();
    void InitializeMines();
    void LoadTestFile(const string &filename);
    void ToggleGameMode();
    int SetFlag(int tileNumber);
    int ClrFlag(int tileNumber);
    int TileClicked(int tileNumber);
    int AllTilesRevealed();
    void UpdateCounter(sf::RenderWindow &wnd);
};


//Resets game mode to default value
void Board::ResetBoard()
{
    std::cout << "ResetBoard..." << std::endl;

    for (int i=0; i<MAX_TILES; i++) {
        tiles[i].currentState = STATE_HIDDEN;
        tiles[i].isAMine = false;
        tiles[i].numberOfAdjMines = 0;
    }
    gameMode = GAMEMODE_NORMAL;
    totalFlags = 0;
    mineCounter = 0;
}
//Randomly places 50 mines on 400 tile board
void Board::InitializeMines()
{
    int mineLocation=0;
    mineCounter = 0;

    for (int i=0; i<MAX_MINES; i++) {
        mineLocation = rand() % MAX_TILES;
        tiles[mineLocation].isAMine = true;
        mineCounter++;
    }
    std::cout << "Initialize mines...Number of Mines= " << mineCounter << std::endl;

}


//Test
void Board::LoadTestFile(const string &filename)
{
    string line;
    ifstream file;
    int i;
    int tilenum;
    
    std::cout << "LoadTestFile: " << filename << std::endl;

    file.open(resourcePath() + filename); //Set your path here
    if(file.is_open())
    {
        tilenum = 0;
        while(std::getline(file, line))
        {
            // Getting every line
            // std::cout << line << std::endl;
            i = 0;
            while ((line[i] == '1') or (line[i] == '0'))
            {
                if (line[i] == '1')
                {
                    tiles[tilenum].isAMine = true;
                    mineCounter++;
                }
                else if (line[i] != '0')
                {
                    break;
                }
                i++;
                tilenum++;
                // std::cout << "i=" << i << " tilenum=" << tilenum << std::endl;
            }
        }
    }
}

//Show mines
void Board::ToggleGameMode()
{
    
    std::cout << "ToggleGameMode: " << gameMode << std::endl;

    if (gameMode == GAMEMODE_DEBUG)
        gameMode = GAMEMODE_NORMAL;
    else if (gameMode == GAMEMODE_NORMAL)
        gameMode = GAMEMODE_DEBUG;
    
}

int Board::AllTilesRevealed()
{
    int hiddenTiles = 0;
    int retcode = RC_ERROR;


    for (int i=0; i<MAX_TILES; i++)
    {
        if ((tiles[i].currentState == STATE_HIDDEN) &&
            (!tiles[i].isAMine))
        
            hiddenTiles++;
    }
    
    if (hiddenTiles == 0)
        retcode = RC_OK;

    std::cout << "AllTilesRevealed: hiddenTiles= " << hiddenTiles << std::endl;

    return retcode;
}

//Handle when a tile is clicked,
// returns RC_OK if it is not a mine
// or RC_ERRROR
// or RC_ISAMINE
// or RC_FLAGGED

int Board::TileClicked(int tileNumber)
{
    int retcode = RC_OK;
    int adjMines = 0;
    
    if (tileNumber > MAX_TILES)
        return RC_ERROR;
    
    if (tiles[tileNumber-1].isAMine)
    {
        retcode = RC_ISAMINE;
    }
    else
    {
        
        std::cout << "TileClicked: TileID= " << tileNumber << ", CS= " << tiles[tileNumber-1].currentState << std::endl;
        
        if (tiles[tileNumber-1].currentState == STATE_FLAGGED)
        {
            retcode = RC_FLAGGED;
        }
        else if (tiles[tileNumber-1].currentState == STATE_HIDDEN)
        {
            
            // calculate adj mines
            if ((tileNumber > 25) &&
                (tileNumber < 376))
            {
                // right edge tiles
                if ((tileNumber % TILES_PER_ROW) == 0)
                {
                    if (tiles[tileNumber-26-1].isAMine)
                        adjMines++;
                    if (tiles[tileNumber-25-1].isAMine)
                        adjMines++;
                    if (tiles[tileNumber+1-1].isAMine)
                        adjMines++;
                    if (tiles[tileNumber+24-1].isAMine)
                        adjMines++;
                    if (tiles[tileNumber+25-1].isAMine)
                        adjMines++;
                }
                // left edge tiles
                else if ((tileNumber % TILES_PER_ROW) == 1)
                {
                    if (tiles[tileNumber-25-1].isAMine)
                        adjMines++;
                    if (tiles[tileNumber-24-1].isAMine)
                        adjMines++;
                    if (tiles[tileNumber+1-1].isAMine)
                        adjMines++;
                    if (tiles[tileNumber+25-1].isAMine)
                        adjMines++;
                    if (tiles[tileNumber+26-1].isAMine)
                        adjMines++;
                }
                // inside tiles
                else
                {
                    
                    if (tiles[tileNumber-26-1].isAMine)
                        adjMines++;
                    if (tiles[tileNumber-25-1].isAMine)
                        adjMines++;
                    if (tiles[tileNumber-24-1].isAMine)
                        adjMines++;
                    if (tiles[tileNumber-1-1].isAMine)
                        adjMines++;
                    if (tiles[tileNumber+1-1].isAMine)
                        adjMines++;
                    if (tiles[tileNumber+24-1].isAMine)
                        adjMines++;
                    if (tiles[tileNumber+25-1].isAMine)
                        adjMines++;
                    if (tiles[tileNumber+26-1].isAMine)
                        adjMines++;
                 }
            }
            else if(tileNumber<=25)
            {
                switch(tileNumber){
                        
                    case 1:
                        if (tiles[tileNumber+1-1].isAMine)
                            adjMines++;
                        if (tiles[tileNumber+25-1].isAMine)
                            adjMines++;
                        if (tiles[tileNumber+26-1].isAMine)
                            adjMines++;
                        
 
                        break;
                    case 25:
                        if (tiles[tileNumber-1-1].isAMine)
                            adjMines++;
                        if (tiles[tileNumber+24-1].isAMine)
                            adjMines++;
                        if (tiles[tileNumber+25-1].isAMine)
                            adjMines++;
                        
                        break;
                        
                    default:
                        if (tiles[tileNumber-1-1].isAMine)
                            adjMines++;
                        if (tiles[tileNumber+1-1].isAMine)
                            adjMines++;
                        if (tiles[tileNumber+24-1].isAMine)
                            adjMines++;
                        if (tiles[tileNumber+25-1].isAMine)
                            adjMines++;
                        if (tiles[tileNumber+26-1].isAMine)
                            adjMines++;
 
                        
                        break;
                        
                }
                
                
            }
            else if(tileNumber>=376)
            {
                switch (tileNumber){
                    case 376:
                        if (tiles[tileNumber+1-1].isAMine)
                            adjMines++;
                        if (tiles[tileNumber-25-1].isAMine)
                            adjMines++;
                        if (tiles[tileNumber-24-1].isAMine)
                            adjMines++;
                        
   
                        break;
                    case 400:
                        if (tiles[tileNumber-1-1].isAMine)
                            adjMines++;
                        if (tiles[tileNumber-25-1].isAMine)
                            adjMines++;
                        if (tiles[tileNumber-26-1].isAMine)
                            adjMines++;
 
                        break;
                    default:
                        if (tiles[tileNumber+1-1].isAMine)
                            adjMines++;
                        if (tiles[tileNumber-1-1].isAMine)
                            adjMines++;
                        if (tiles[tileNumber-24-1].isAMine)
                            adjMines++;
                        if (tiles[tileNumber-25-1].isAMine)
                            adjMines++;
                        if (tiles[tileNumber-26-1].isAMine)
                            adjMines++;
                        
  
                        
                        break;
                }
            }
            tiles[tileNumber-1].numberOfAdjMines = adjMines;
            tiles[tileNumber-1].currentState = STATE_REVEALED;
            
            retcode = RC_OK;
            
     //        std::cout << "AdjMines: ID= " << tileNumber << ", CS= " << tiles[tileNumber-1].currentState << " #Mines= " << tiles[tileNumber-1].numberOfAdjMines << std::endl;
            
            if (adjMines==0)
            {
                if ((tileNumber > 25) &&
                    (tileNumber < 376))
                {
                    if ((tileNumber % TILES_PER_ROW) == 0)
                    {
                        if (TileClicked(tileNumber-26) != RC_OK)
                            return RC_ERROR;
                        if (TileClicked(tileNumber-25) != RC_OK)
                            return RC_ERROR;
                        if (TileClicked(tileNumber-1) != RC_OK)
                            return RC_ERROR;
                        if (TileClicked(tileNumber+24) != RC_OK)
                            return RC_ERROR;
                        if (TileClicked(tileNumber+25) != RC_OK)
                            return RC_ERROR;

                    }
                    // left edge tiles
                    else if ((tileNumber % TILES_PER_ROW) == 1)
                    {
                        if (TileClicked(tileNumber-25) != RC_OK)
                            return RC_ERROR;
                        if (TileClicked(tileNumber-24) != RC_OK)
                            return RC_ERROR;
                        if (TileClicked(tileNumber+1) != RC_OK)
                            return RC_ERROR;
                        if (TileClicked(tileNumber+25) != RC_OK)
                            return RC_ERROR;
                        if (TileClicked(tileNumber+26) != RC_OK)
                            return RC_ERROR;

                    }
                    // inside tiles
                    else
                    {
                        if (TileClicked(tileNumber-26) != RC_OK)
                            return RC_ERROR;
                        if (TileClicked(tileNumber-25) != RC_OK)
                            return RC_ERROR;
                        if (TileClicked(tileNumber-24) != RC_OK)
                            return RC_ERROR;
                        if (TileClicked(tileNumber-1) != RC_OK)
                            return RC_ERROR;
                        if (TileClicked(tileNumber+1) != RC_OK)
                            return RC_ERROR;
                        if (TileClicked(tileNumber+24) != RC_OK)
                            return RC_ERROR;
                        if (TileClicked(tileNumber+25) != RC_OK)
                            return RC_ERROR;
                        if (TileClicked(tileNumber+26) != RC_OK)
                            return RC_ERROR;
                    }

                }
                else if(tileNumber<=25)
                {
                    switch(tileNumber){
                            
                        case 1:
                            if (TileClicked(tileNumber+1) != RC_OK)
                                return RC_ERROR;
                            if (TileClicked(tileNumber+25) != RC_OK)
                                return RC_ERROR;
                            if (TileClicked(tileNumber+26) != RC_OK)
                                return RC_ERROR;
                           
                            break;
                        case 25:
                            if (TileClicked(tileNumber-1) != RC_OK)
                                return RC_ERROR;
                            if (TileClicked(tileNumber+24) != RC_OK)
                                return RC_ERROR;
                            if (TileClicked(tileNumber+25) != RC_OK)
                                return RC_ERROR;

                            break;
                            
                        default:
                            if (TileClicked(tileNumber-1) != RC_OK)
                                return RC_ERROR;
                            if (TileClicked(tileNumber+1) != RC_OK)
                                return RC_ERROR;
                            if (TileClicked(tileNumber+24) != RC_OK)
                                return RC_ERROR;
                            if (TileClicked(tileNumber+25) != RC_OK)
                                return RC_ERROR;
                            if (TileClicked(tileNumber+26) != RC_OK)
                                return RC_ERROR;

                            break;
                            
                    }
                    
                    
                }
                else if(tileNumber>=376)
                {
                    switch (tileNumber){
                        case 376:
              
                            if (TileClicked(tileNumber+1) != RC_OK)
                                return RC_ERROR;
                            if (TileClicked(tileNumber-25) != RC_OK)
                                return RC_ERROR;
                            if (TileClicked(tileNumber-24) != RC_OK)
                                return RC_ERROR;

                            break;
                        case 400:
                            if (TileClicked(tileNumber-1) != RC_OK)
                                return RC_ERROR;
                            if (TileClicked(tileNumber-25) != RC_OK)
                                return RC_ERROR;
                            if (TileClicked(tileNumber-26) != RC_OK)
                                return RC_ERROR;

                            break;
                        default:
               
                            if (TileClicked(tileNumber+1) != RC_OK)
                                return RC_ERROR;
                            if (TileClicked(tileNumber-1) != RC_OK)
                                return RC_ERROR;
                            if (TileClicked(tileNumber-24) != RC_OK)
                                return RC_ERROR;
                            if (TileClicked(tileNumber-25) != RC_OK)
                                return RC_ERROR;
                            if (TileClicked(tileNumber-26) != RC_OK)
                                return RC_ERROR;

                            
                            break;
                    }
                }
            }
        }
        
    }
    
    return retcode;
}


//sets flag for particular tile
int Board::SetFlag(int tileNumber)
{
    int retcode = RC_OK;


    if (tiles[tileNumber-1].currentState == STATE_HIDDEN) {
        tiles[tileNumber-1].currentState = STATE_FLAGGED;
        totalFlags++;
        mineCounter--;

        std::cout << "SetFlag: tileNumber= " << tileNumber << std::endl;

    }
    else
        retcode = RC_ERROR;
    return retcode;
}

//clear flag for particular tile
int Board::ClrFlag(int tileNumber)
{
    int retcode = RC_OK;
    
    if (tiles[tileNumber-1].currentState == STATE_FLAGGED) {
        tiles[tileNumber-1].currentState = STATE_HIDDEN;
        totalFlags--;
        mineCounter++;

        std::cout << "ClrFlag: tileNumber= " << tileNumber << std::endl;

    }
    else
        retcode = RC_ERROR;
    
    return retcode;
}

int GetButtonClicked (sf::Vector2i location)
// return button ID from 1 to 400
{
    
    int btnID = 0;
    int col,row = 0;
    
    if (location.y <= BOARD_HEIGHT)
    {
        // a tile is clicked
        row = location.y / TILE_HEIGHT;
        col = location.x / TILE_WIDTH;
        btnID = col + (row*TILES_PER_ROW) + 1;
    }
    // a button is clicked
    else if ((location.y > BOARD_HEIGHT) && (location.y <= (BOARD_HEIGHT + BUTTON_HEIGHT)))
    {
        if ((location.x >= TEST2_X) && (location.x < (TEST2_X+BUTTON_WIDTH)))
            btnID = TEST2_ID;
        else if ((location.x >= TEST1_X) && (location.x < (TEST1_X+BUTTON_WIDTH)))
            btnID = TEST1_ID;
        else if ((location.x >= DEBUG_X) && (location.x < (DEBUG_X+BUTTON_WIDTH)))
            btnID = DEBUG_ID;
        else if ((location.x >= RESTART_X) && (location.x <(RESTART_X+BUTTON_WIDTH)))
            btnID = RESTART_ID;
        else
            btnID=0;
    }
    else
        btnID=0;

    std::cout << "BtnClicked: ButtonID= " << btnID << std::endl;

    return btnID;
}

void Board::UpdateCounter(sf::RenderWindow &wnd)
{
    sf::IntRect counter;
    sf::Texture textureCounter;
    sf::Sprite spriteCounter;
    int d1, d2, d3;
    
    std::cout << "UpdateCounter: MineCounter= " << mineCounter << std::endl;

    if (!textureCounter.loadFromFile(resourcePath() + "digits.png")) {
        return EXIT_FAILURE;
    }
    spriteCounter.setTexture(textureCounter);
    
    int n = mineCounter;
    
    d1 = n / 100; // hundredth digit
    n = n - (d1 * 100);
    
    d2 = n / 10;  // tenth digit
    n = n - (d2 * 10);
    
    d3 = n;
    
    
    counter.top = 0;
    counter.height = 32;
    counter.width = 21;

    // draw d1
    counter.left = d1 * 21;
    spriteCounter.setTextureRect(counter);
    spriteCounter.setPosition(0,512);
    wnd.draw(spriteCounter);
    // draw d2
    counter.left = d2 * 21;
    spriteCounter.setTextureRect(counter);
    spriteCounter.setPosition(21,512);
    wnd.draw(spriteCounter);
    // draw d3
    counter.left = d3 * 21;
    spriteCounter.setTextureRect(counter);
    spriteCounter.setPosition(42,512);
    wnd.draw(spriteCounter);

}


int main(int, char const**)
{
    // Create the main window
    sf::RenderWindow window(sf::VideoMode(800, 600), "Minesweeper");
    
    //Declare sprite variables
    sf::Texture texture;
    sf::Texture textureDebug;
    sf::Texture textureTest1;
    sf::Texture textureTest2;
    sf::Texture textureRestart;
    sf::Texture textureGameOver;
    
    sf::Sprite spriteTile;
    sf::Sprite spriteDebug;
    sf::Sprite spriteTest1;
    sf::Sprite spriteTest2;
    sf::Sprite spriteRestart;
    sf::Sprite spriteGameOver;
    
    int buttonID;
    bool change;
    bool rightClick;
    Board gameBoard;

  
    sf::Font font;
    sf::Text text("Minesweepter", font , 50);
    text.setFillColor(sf::Color::Black);
    
    // initialization
    gameBoard.ResetBoard();
    gameBoard.InitializeMines();
    change = true;
    buttonID = 0;
    
    // Start the game loop
    while (window.isOpen())
    {
        // Process events
        sf::Event event;
        sf::Vector2i mouseLocation;
        
        while (window.pollEvent(event))
        {
            // Close window: exit
            switch (event.type) {
                    // close window
                case sf::Event::Closed:
                    window.close();
                    break;
                    
                case sf::Event::MouseButtonPressed:
                    if (event.mouseButton.button == sf::Mouse::Left)
                    {
                        mouseLocation = sf::Mouse::getPosition(window);
                        rightClick = false;
                        
                    }
                    else if (event.mouseButton.button == sf::Mouse::Right)
                    {
                        mouseLocation = sf::Mouse::getPosition(window);
                        rightClick = true;
                    }
                    buttonID = GetButtonClicked(mouseLocation);
                    // std::cout << "ID= " << buttonID << " RC=" << rightClick << std::endl;
                    
                    break;
                    
                default:
                    break;
            }
            
        }
        
        
        // Logic to handle button clicked
        switch (buttonID)
        {
            case DEBUG_ID:
                gameBoard.ToggleGameMode();
                change = true;
                buttonID = 0;
                break;
            case RESTART_ID:
                gameBoard.ResetBoard();
                gameBoard.InitializeMines();
                change = true;
                buttonID = 0;
                break;
            case TEST1_ID:
                gameBoard.ResetBoard();
                gameBoard.LoadTestFile("testboard.brd");
                change = true;
                buttonID = 0;
                break;
            case TEST2_ID:
                gameBoard.ResetBoard();
                gameBoard.LoadTestFile("testboard2.brd");
                change = true;
                buttonID = 0;
                break;
            case 1 ... MAX_TILES:
                if ((gameBoard.gameMode == GAMEMODE_NORMAL) ||
                    (gameBoard.gameMode == GAMEMODE_DEBUG)
                    )
                {
                    if (rightClick)
                    {
                        if (gameBoard.tiles[buttonID-1].currentState == STATE_FLAGGED)
                            gameBoard.ClrFlag(buttonID);
                        else
                            gameBoard.SetFlag(buttonID);
                        
                        rightClick = false;
                    }
                    else // left mouse button is clicked
                    {

                        if (gameBoard.TileClicked(buttonID) == RC_ISAMINE)
                        {
                            gameBoard.gameMode = GAMEMODE_OVER;
                        }
                        else if (gameBoard.AllTilesRevealed() == RC_OK)
                        {
                            gameBoard.gameMode = GAMEMODE_WIN;
                        }
                        else
                        {
                        
                        }
                    }
                    change = true;
                    buttonID = 0;
                }
                break;
                
            default:
                break;
        }
        
        
        if (change)
        {
            change = false;
            
            // Set a white background
            window.clear();
            
            //Redraw the board
            
            int tileID;
            for(int j = 0; j<16; j++)
            {
                for( int i=0; i<25; i++)
                {
                    spriteTile.setPosition(32*i, 32*j);
                    tileID = i + (j * TILES_PER_ROW);
                    
                    if ((gameBoard.gameMode == GAMEMODE_DEBUG) && (gameBoard.tiles[tileID].isAMine))
                    {
                        if (!texture.loadFromFile(resourcePath() + "mine.png")) {
                            return EXIT_FAILURE;
                        }
                    }
                    else if (gameBoard.tiles[tileID].currentState == STATE_FLAGGED)
                    {
                        if (!texture.loadFromFile(resourcePath() + "flag.png")) {
                            return EXIT_FAILURE;
                        }
                    }
                    else if ((gameBoard.gameMode == GAMEMODE_OVER) && (gameBoard.tiles[tileID].isAMine))
                    {
                        if (!texture.loadFromFile(resourcePath() + "mine.png")) {
                            return EXIT_FAILURE;
                        }
                    }
                    else if ((gameBoard.gameMode == GAMEMODE_WIN) && (gameBoard.tiles[tileID].isAMine))
                    {
                        if (!texture.loadFromFile(resourcePath() + "flag.png")) {
                            return EXIT_FAILURE;
                        }
                    }
                    else if (gameBoard.tiles[tileID].currentState == STATE_REVEALED)
                    {
                        switch (gameBoard.tiles[tileID].numberOfAdjMines)
                        {
                            case 0:
                                if (!texture.loadFromFile(resourcePath() + "tile_revealed.png")) {
                                    return EXIT_FAILURE;
                                }
                                break;
                                
                            case 1:
                                if (!texture.loadFromFile(resourcePath() + "number_1.png")) {
                                    return EXIT_FAILURE;
                                }
                                break;
                                
                            case 2:
                                if (!texture.loadFromFile(resourcePath() + "number_2.png")) {
                                    return EXIT_FAILURE;
                                }
                                break;
                                
                            case 3:
                                if (!texture.loadFromFile(resourcePath() + "number_3.png")) {
                                    return EXIT_FAILURE;
                                }
                                break;
                                
                            case 4:
                                if (!texture.loadFromFile(resourcePath() + "number_4.png")) {
                                    return EXIT_FAILURE;
                                }
                                break;
                                
                            case 5:
                                if (!texture.loadFromFile(resourcePath() + "number_5.png")) {
                                    return EXIT_FAILURE;
                                }
                                break;
                                
                            case 6:
                                if (!texture.loadFromFile(resourcePath() + "number_6.png")) {
                                    return EXIT_FAILURE;
                                }
                                break;
                                
                            case 7:
                                if (!texture.loadFromFile(resourcePath() + "number_7.png")) {
                                    return EXIT_FAILURE;
                                }
                                break;
                                
                            case 8:
                                if (!texture.loadFromFile(resourcePath() + "number_8.png")) {
                                    return EXIT_FAILURE;
                                }
                                break;
                                
                            default:
                                break;
                                
                        }
                        
                    }
                    else
                    {
                        if (!texture.loadFromFile(resourcePath() + "tile_hidden.png")) {
                            return EXIT_FAILURE;
                        }
                        
                    }

                    spriteTile.setTexture(texture);
                    window.draw(spriteTile);
                    
                } // for i loop
            } // for j loop
            
            // update the counter
        
            gameBoard.UpdateCounter(window);
            
            if (gameBoard.gameMode == GAMEMODE_OVER)
            {
                if (!textureGameOver.loadFromFile(resourcePath() + "face_lose.png"))
                {
                    return EXIT_FAILURE;
                }
                spriteGameOver.setPosition(368,512);
                spriteGameOver.setTexture(textureGameOver);
                window.draw(spriteGameOver);
                
            }
            else if (gameBoard.gameMode == GAMEMODE_WIN)
            {
                if (!textureGameOver.loadFromFile(resourcePath() + "face_win.png"))
                {
                    return EXIT_FAILURE;
                }
                spriteGameOver.setPosition(368,512);
                spriteGameOver.setTexture(textureGameOver);
                window.draw(spriteGameOver);
                
            }
            else
            {
                //draw the happy face
                if (!textureRestart.loadFromFile(resourcePath() + "face_happy.png")) {
                    return EXIT_FAILURE;
                }
                spriteRestart.setTexture(textureRestart);
                spriteRestart.setPosition(368,512);
                window.draw(spriteRestart);
            }
            
            //draw Debug
            if (!textureDebug.loadFromFile(resourcePath() + "debug.png")) {
                return EXIT_FAILURE;
            }
            spriteDebug.setTexture(textureDebug);
            spriteDebug.setPosition(496,512);
            window.draw(spriteDebug);
            //Test 1
            if (!textureTest1.loadFromFile(resourcePath() + "test_1.png")) {
                return EXIT_FAILURE;
            }
            spriteTest1.setTexture(textureTest1);
            spriteTest1.setPosition(560,512);
            window.draw(spriteTest1);
            //Test 2
            if (!textureTest2.loadFromFile(resourcePath() + "test_2.png")) {
                return EXIT_FAILURE;
            }
            spriteTest2.setTexture(textureTest2);
            spriteTest2.setPosition(624,512);
            window.draw(spriteTest2);
            
            
            // Update the window
            window.display();
            
        } // if change
        
    } //Window.isOpen()
    
    return EXIT_SUCCESS;
}
