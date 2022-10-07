/* "Let's make 16 games in C++ - part 13: TRON" - by FamTrinli
    ==========================================

   Youtube: https://youtu.be/bwsyFVqWoz8?list=PLB_ibvUSN7mzUffhiay5g5GUHyJRO4DYr
   This implementation version is upto 0m54s into the video

   Implementation by Joseph21, october 7, 2022
   -------------------------------------------

   Fase 2 - you win text

   This implementation file is part of a series of progressive implementation files. These files follow the compilation
   instances from the video. The best way to study this is to follow the video, look at the associated implementation file and
   check on differences between two subsequent implementation files using some difference analysis tool (for instance
   WinMerge).

   Dependencies:
     * FamTrinli uses SFML, I implemented this game using olcPixelGameEngine.h - (olc::PixelGameEngine header file) by JavidX9
       (see: https://github.com/OneLoneCoder/olcPixelGameEngine)
     * In the header of the Youtube video you can find the location where any resources can be found, as well as the original
       code by FamTrinli

     Have fun!
 */

#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#include "vector_types.h"


// screen dimensions and pixel size
#define SCREEN_X    600
#define SCREEN_Y    480
#define PIXEL_X       1
#define PIXEL_Y       1

const int W = SCREEN_X;
const int H = SCREEN_Y;


class TronGame : public olc::PixelGameEngine {

public:
    TronGame() {
        sAppName = "The Tron Game";
    }

private:
    // put your class variables here
    int speed = 1;                 // must be 4, scaled down for testing
    bool field[W][H] = { 0 };      // playing field definition and init

    struct player {
        int x, y, dir;
        olc::Pixel color;

        player( olc::Pixel c ) {
            x = rand() % W;    // set random location and direction, and passed colour
            y = rand() % H;
            dir = rand() % 4;
            color = c;
        }

        void tick() {
            // this could also be a switch statement
            if (dir == 0) y += 1;   // south
            if (dir == 1) x -= 1;   // west
            if (dir == 2) x += 1;   // east
            if (dir == 3) y -= 1;   // north

            // wrap around the borders of the field
            if (x >= W) x =     0;
            if (x <  0) x = W - 1;
            if (y >= H) y =     0;
            if (y <  0) y = H - 1;
        }

        vf3d getColor() {
            return vf3d( color.r, color.g, color.b );
        }
    };

    olc::Sprite *sBackground = nullptr;
    olc::Decal  *dBackground = nullptr;
    player p1 = player( olc::RED ), p2 = player( olc::GREEN );

    bool Game = true;
    int nHasWon = 0;

    olc::Sprite *sprite = nullptr;

public:
    bool OnUserCreate() override {

        // initialize your assets here
        srand( time( 0 ));

        // This lambda function loads the sprite from sFileName, and creates a decal from it.
        // The decal pointer is returned, the sprite pointer is passed as parameter.
        // Upon failure, both spritePtr and returned pointer will be nullptr, and an error is given to std::cout
        auto load_decal_from_file = [=]( std::string sFileName, olc::Sprite *spritePtr ) {
            spritePtr = new olc::Sprite( sFileName );
            olc::Decal *auxDecalPtr = new olc::Decal( spritePtr );
            if (spritePtr->width == 0 || spritePtr->height == 0) {
                std::cout << "ERROR: OnUserCreate() --> couldn't load: " << sFileName << std::endl;
                delete spritePtr  ; spritePtr   = nullptr;
                delete auxDecalPtr; auxDecalPtr = nullptr;
            }
            return auxDecalPtr;
        };
        // load background sprite and decal
        dBackground = load_decal_from_file( "files/background.png", sBackground );

        sprite = new olc::Sprite( W, H );
        SetDrawTarget( sprite );
        Clear( olc::BLACK );
        DrawSprite( 0, 0, sBackground );
        SetDrawTarget( nullptr );

        return true;
    }

    bool OnUserUpdate( float fElapsedTime ) override {

        // your update code per frame here

        // get direction for player 1
        if (GetKey( olc::Key::LEFT  ).bPressed) if (p1.dir != 2) p1.dir = 1;
        if (GetKey( olc::Key::RIGHT ).bPressed) if (p1.dir != 1) p1.dir = 2;
        if (GetKey( olc::Key::UP    ).bPressed) if (p1.dir != 0) p1.dir = 3;
        if (GetKey( olc::Key::DOWN  ).bPressed) if (p1.dir != 3) p1.dir = 0;

        // get direction for player 2
        if (GetKey( olc::Key::A     ).bPressed) if (p2.dir != 2) p2.dir = 1;
        if (GetKey( olc::Key::D     ).bPressed) if (p2.dir != 1) p2.dir = 2;
        if (GetKey( olc::Key::W     ).bPressed) if (p2.dir != 0) p2.dir = 3;
        if (GetKey( olc::Key::S     ).bPressed) if (p2.dir != 3) p2.dir = 0;

        if (Game) {
            SetDrawTarget( sprite );
            for (int i = 0; i < speed; i++) {
                // move players
                p1.tick();
                p2.tick();
                // check on death by collision on some trail
                if (field[p1.x][p1.y] == 1) { Game = false; nHasWon = 2; }
                if (field[p2.x][p2.y] == 1) { Game = false; nHasWon = 1; }
                // set the trails
                field[p1.x][p1.y] = 1;
                field[p2.x][p2.y] = 1;
                // display as little circles with the right colour
                FillCircle( p1.x, p1.y, 3, p1.color );
                FillCircle( p2.x, p2.y, 3, p2.color );
            }
            SetDrawTarget( nullptr );
        } else {
            olc::Pixel textColour;
            switch (nHasWon) {
                case 1: textColour = p1.color; break;
                case 2: textColour = p2.color; break;
                default: textColour = olc::YELLOW;
            }
            SetDrawTarget( sprite );
            DrawString( W / 2 - 80, 20, "YOU WIN!", textColour, 4 );
            SetDrawTarget( nullptr );
        }

        // draw
        Clear( olc::BLACK );
        DrawSprite( 0, 0, sprite );


        return true;
    }

    bool OnUserDestroy() override {

        // your cleanup code here
        delete dBackground; dBackground = nullptr;
        delete sBackground; sBackground = nullptr;
        delete sprite     ; sprite      = nullptr;

        return true;
    }
};

int main()
{
	TronGame game;
	if (game.Construct( SCREEN_X / PIXEL_X, SCREEN_Y / PIXEL_Y, PIXEL_X, PIXEL_Y, false, true ))
		game.Start();

	return 0;
}

