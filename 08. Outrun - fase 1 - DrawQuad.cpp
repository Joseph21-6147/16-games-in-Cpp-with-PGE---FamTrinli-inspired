/* "Let's make 16 games in C++ - part 7: OUTRUN" - by FamTrinli
    ===========================================

   Youtube: https://youtu.be/N60lBZDEwJ8?list=PLB_ibvUSN7mzUffhiay5g5GUHyJRO4DYr
   This implementation version is upto: 0m24s in the video

   Implementation by Joseph21, october 5, 2022
   -------------------------------------------

   Fase 1 - drawing a quad (using decals)

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


// keep the screen dimensions constant and vary the resolution by adapting the pixel size
#define SCREEN_X   1024
#define SCREEN_Y    768
#define PIXEL_X       1
#define PIXEL_Y       1

class OutrunGame : public olc::PixelGameEngine {

public:
    OutrunGame() {
        sAppName = "Outrun Racing";
    }

private:

    void DrawQuad( olc::Decal *c, int x1, int y1, int w1, int x2, int y2, int w2 ) {

        // create vector of quad corner points from the parameters
		std::vector<olc::vf2d> points = {
		    { float( x1 - w1 ), float( y1 ) },
		    { float( x2 - w2 ), float( y2 ) },
		    { float( x2 + w2 ), float( y2 ) },
		    { float( x1 + w1 ), float( y1 ) }
		};
		// create vector of texture coordinates from vector of points (just normalize in [0.0f, 1.0f]
		std::vector<olc::vf2d> textures;
		for (auto elt : points) {
            olc::vf2d texture = { elt.x / float( SCREEN_X ), elt.y / float( SCREEN_Y ) };
            textures.push_back( texture );
		}
		// draw the quad using the decal ptr passed
        DrawPolygonDecal( c, points, textures );
    }

    olc::Decal *pDecal = nullptr;

public:
    bool OnUserCreate() override {

        // initialize a green decal
        auto create_coloured_decal = [=] ( olc::Pixel col ) {
            olc::Sprite *auxSpritePtr = new olc::Sprite( SCREEN_X, SCREEN_Y );
            SetDrawTarget( auxSpritePtr );
            Clear( col );
            SetDrawTarget( nullptr );
            olc::Decal *auxDecalPtr = new olc::Decal( auxSpritePtr );
            delete auxSpritePtr;
            return auxDecalPtr;
        };

        pDecal = create_coloured_decal( olc::GREEN );
        return true;
    }

    bool OnUserUpdate( float fElapsedTime ) override {

        // your update code per frame here
        Clear( olc::BLACK );
        DrawQuad( pDecal, 500, 500, 200, 500, 300, 100 );

        return true;
    }
};

int main()
{
	OutrunGame game;
	if (game.Construct( SCREEN_X / PIXEL_X, SCREEN_Y / PIXEL_Y, PIXEL_X, PIXEL_Y, false, true ))
		game.Start();

	return 0;
}
