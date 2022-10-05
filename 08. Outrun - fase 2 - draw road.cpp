/* "Let's make 16 games in C++ - part 7: OUTRUN" - by FamTrinli
    ===========================================

   Youtube: https://youtu.be/N60lBZDEwJ8?list=PLB_ibvUSN7mzUffhiay5g5GUHyJRO4DYr
   This implementation version is upto: 1m12s in the video

   Implementation by Joseph21, october 5, 2022
   -------------------------------------------

   Fase 2 - road drawing

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


int width  = SCREEN_X;
int height = SCREEN_Y;

int roadW = 2000;    // road width
int segL  =  200;    // segment length
float camD = 0.84f;  // camera depth

class OutrunGame : public olc::PixelGameEngine {

public:
    OutrunGame() {
        sAppName = "Outrun Racing";
    }

private:
    struct Line {
        float x = 0.0f, y = 0.0f, z = 0.0f;    // 3d center of line
        float X = 0.0f, Y = 0.0f, W = 0.0f;    // screen coord
        float scale = 1.0f;

        Line() { x = y = z = 0.0f; }

        // from world to screen coordinates
        void project( int camX, int camY, int camZ ) {
            scale = camD / (z - camZ);
            X = (1 + scale * (x - camX)) * width  / 2;
            Y = (1 - scale * (y - camY)) * height / 2;
            W = scale * roadW * width / 2;
        }
    };

    void DrawQuad( olc::Decal *c, int x1, int y1, int w1, int x2, int y2, int w2 ) {

        // clamp w values to not become negative
        w1 = std::max( 0, w1 );
        w2 = std::max( 0, w2 );

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

    std::vector<Line> lines;
    int N;

// colours for grass, rumble and road
#define  GRASS_COL1    olc::Pixel(  16, 200,  16 )    // 2 shades of green
#define  GRASS_COL2    olc::Pixel(   0, 154,   0 )
#define RUMBLE_COL1    olc::Pixel( 255, 255, 255 )    // black and white blocks
#define RUMBLE_COL2    olc::Pixel(   0,   0,   0 )
#define   ROAD_COL1    olc::Pixel( 107, 107, 107 )    // two nuances of grey
#define   ROAD_COL2    olc::Pixel( 105, 105, 105 )

    // for each colour a decal is created to draw quads for that colour
    // maybe I could just create 1 white decal, and tint it with the right colour...
    olc::Decal *pDecalGrass1  = nullptr;
    olc::Decal *pDecalGrass2  = nullptr;
    olc::Decal *pDecalRumble1 = nullptr;
    olc::Decal *pDecalRumble2 = nullptr;
    olc::Decal *pDecalRoad1   = nullptr;
    olc::Decal *pDecalRoad2   = nullptr;

public:
    bool OnUserCreate() override {

        // initialize the different coloured decals
        auto create_coloured_decal = [=] ( olc::Pixel col ) {
            olc::Sprite *auxSpritePtr = new olc::Sprite( SCREEN_X, SCREEN_Y );
            SetDrawTarget( auxSpritePtr );
            Clear( col );
            SetDrawTarget( nullptr );
            olc::Decal *auxDecalPtr = new olc::Decal( auxSpritePtr );
            delete auxSpritePtr;
            return auxDecalPtr;
        };

        pDecalGrass1  = create_coloured_decal(  GRASS_COL1 );
        pDecalGrass2  = create_coloured_decal(  GRASS_COL2 );
        pDecalRumble1 = create_coloured_decal( RUMBLE_COL1 );
        pDecalRumble2 = create_coloured_decal( RUMBLE_COL2 );
        pDecalRoad1   = create_coloured_decal(   ROAD_COL1 );
        pDecalRoad2   = create_coloured_decal(   ROAD_COL2 );

        // fill the screen lines with the distance info
        for (int i = 0; i < 1600; i++) {
            Line line;
            line.z = i * segL;
            lines.push_back( line );
        }
        N = lines.size();

        return true;
    }

    bool OnUserUpdate( float fElapsedTime ) override {

        // your update code per frame here
        Clear( olc::BLACK );

        // draw road
        for (int n = 0; n < 300; n++) {
            Line &l = lines[ n % N ];   // whence the & here?
            l.project( 0, 1500, 0 );

            olc::Decal *pGrass  = (n / 3) % 2 ? pDecalGrass1  : pDecalGrass2 ;
            olc::Decal *pRumble = (n / 3) % 2 ? pDecalRumble1 : pDecalRumble2;
            olc::Decal *pRoad   = (n / 3) % 2 ? pDecalRoad1   : pDecalRoad2  ;

            Line p = lines[ (n - 1) % N ];  // previous line

            DrawQuad( pGrass ,   0, p.Y, width, 0, l.Y, width );
            DrawQuad( pRumble, p.X, p.Y, p.W * 1.2f, l.X, l.Y, l.W * 1.2f );
            DrawQuad( pRoad  , p.X, p.Y, p.W       , l.X, l.Y, l.W );
        }

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
