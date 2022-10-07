// Series: Let's make 16 games in C++ and SFML (by FamTrinli)
// Game  : Tron
//
// Youtube: https://youtu.be/bwsyFVqWoz8?list=PLB_ibvUSN7mzUffhiay5g5GUHyJRO4DYr
//
// Implemented by: Joseph21
// Date: 2022-05-31
//
// final version

/* "Let's make 16 games in C++ - part 13: TRON" - by FamTrinli
    ==========================================

   Youtube: https://youtu.be/bwsyFVqWoz8?list=PLB_ibvUSN7mzUffhiay5g5GUHyJRO4DYr
   This implementation version is upto the end of the video (final version)

   Implementation by Joseph21, october 7, 2022
   -------------------------------------------

   Fase 3 - improvised shading

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

// player colours
#define COLOUR1    olc::GREEN
#define COLOUR2    olc::BLUE

// defines the width of the player beams on the screen
#define BEAM_RADIUS 2

const int W = SCREEN_X;
const int H = SCREEN_Y;

class TronGame : public olc::PixelGameEngine {

public:
    TronGame() {
        sAppName = "The Tron Game";
    }

private:
    // put your class variables here
    int speed = 1;                  // must be 4, scaled down for testing
    bool field[W][H] = { 0 };       // playing field definition and init

    struct player {
        int x, y, dir;
        olc::Pixel color;

        player( olc::Pixel c ) {
            x = rand() % W;         // set random location and direction, and passed colour
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
    };

    olc::Sprite *sBackground = nullptr;
    // pointer to aux sprite
    olc::Sprite *sprite = nullptr;

    player p1 = player( COLOUR1 );
    player p2 = player( COLOUR2 );

    bool Game = true;
    int nHasWon = 0;

public:
    bool OnUserCreate() override {

        // initialize your assets here
        srand( time( 0 ));

        // load background sprite
        std::string sPath = "files/";
        sBackground = new olc::Sprite( sPath + "background.png" );

        // create an auxiliary sprite
        sprite = new olc::Sprite( W, H );
        // draw the background to it
        SetDrawTarget( sprite );
        DrawSprite( 0, 0, sBackground );
        SetDrawTarget( nullptr );

        return true;
    }

    bool OnUserUpdate( float fElapsedTime ) override {

        // user controls

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

        // code for testing the radius and brightness values
//        if (GetKey( olc::Key::INS   ).bPressed) radius += 1;
//        if (GetKey( olc::Key::DEL   ).bPressed) radius -= 1;
//        if (GetKey( olc::Key::NP_ADD ).bHeld) brightness += 0.0001f;
//        if (GetKey( olc::Key::NP_SUB ).bHeld) brightness -= 0.0001f;

//		void SetPixelMode(std::function<olc::Pixel(const int x, const int y, const olc::Pixel& pSource, const olc::Pixel& pDest)> pixelMode);
		// Write a lambda that plots the pixel, and also additively draws to the neighbours
		auto CustomShaderTrial = [&] ( const int x, const int y, const olc::Pixel &pSrc, const olc::Pixel& pDst) -> olc::Pixel {

            int radius = 4;
            float brightness = 0.004f;

		    int strt_x = std::max(                  0, x - radius );
		    int stop_x = std::min( sprite->width  - 1, x + radius );
		    int strt_y = std::max(                  0, y - radius );
		    int stop_y = std::min( sprite->height - 1, y + radius );

		    for (int i = strt_x; i <= stop_x; i++) {
                for (int j = strt_y; j <= stop_y; j++) {
                    if (i != x || j != y) {
                        olc::Pixel aux = sprite->GetPixel( i, j );
                        aux += (pSrc * brightness);
                        sprite->SetPixel( i, j, aux );
                    }
                }
		    }
            return pSrc;
		};

        SetDrawTarget( sprite );

        if (Game) {

            SetPixelMode( olc::Pixel::CUSTOM );
            SetPixelMode( CustomShaderTrial );
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
                FillCircle( p1.x, p1.y, BEAM_RADIUS, p1.color );
                FillCircle( p2.x, p2.y, BEAM_RADIUS, p2.color );
            }
            SetPixelMode( olc::Pixel::NORMAL );

        } else {
            olc::Pixel textColour;
            switch (nHasWon) {
                case 1: textColour = p1.color; break;
                case 2: textColour = p2.color; break;
                default: textColour = olc::YELLOW;
            }
            DrawString( W / 2 - 80, 20, "YOU WIN!", textColour, 4 );
        }
        SetDrawTarget( nullptr );

        // draw
        DrawSprite( 0, 0, sprite );

//        DrawString( 0,  0, "radius: " + std::to_string( radius ));
//        DrawString( 0, 10, "brightness: " + std::to_string( brightness ));

        return true;
    }

    bool OnUserDestroy() override {

        // your cleanup code here
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

