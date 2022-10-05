/* "Let's make 16 games in C++ - part 7: OUTRUN" - by FamTrinli
    ===========================================

   Youtube: https://youtu.be/N60lBZDEwJ8?list=PLB_ibvUSN7mzUffhiay5g5GUHyJRO4DYr
   This implementation version is upto: 3m07s in the video

   Implementation by Joseph21, october 5, 2022
   -------------------------------------------

   Fase 4a - trees along the track

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

#include <float.h>

#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

// keep the screen dimensions constant and vary the resolution by adapting the pixel size
#define SCREEN_X   1024
#define SCREEN_Y    768
#define PIXEL_X       1
#define PIXEL_Y       1


int width  = SCREEN_X;
int height = SCREEN_Y;

int   roadW = 2000;      // road width
int   segL  =  200;      // segment length
float camD  =    0.84f;  // camera depth - dunno where the value is coming from...

class OutrunGame : public olc::PixelGameEngine {

public:
    OutrunGame() {
        sAppName = "Outrun Racing";
    }

private:
    struct Line {
        float x, y, z;    // 3d center of line
        float X, Y, W;    // screen coord (why not int?)
        float scale = 1.0f;
        float curve;
        float spriteX = 0.0f;
        float clip = 0.0f;
        olc::Decal *pDec = nullptr;
        int decWidth  = 0;
        int decHeight = 0;

        Line() {
            x       = 0.0f;
            y       = 0.0f;
            z       = 0.0f;
            curve   = 0.0f;
            spriteX = 0.0f;
        }

        // transformation from world to screen coordinates
        void project( int camX, int camY, int camZ ) {   // I don't understand this method
            scale = camD / (z - camZ);  // scale is inversely proportional to distance (depth) between line and camera
/*
 * X ==         0  =>  scale * (x - camX) == -1  =>  x - camX == -1 / scale
 * X == width / 2  =>  scale == 0 or x == camX
 * X == width      =>  scale * (x - camX) == +1  =>  x - camX == +1 / scale
 */
            X = (1 + scale * (x - camX)) * width  / 2;   // scale the horizontal difference and put it into view horizontally
/*
 * Y =      0      =>  scale * (y - camY) == +1  =>  y - camY == +1 / scale
 * Y = height / 2  =>  scale == 0 or y == camY
 * Y = height      =>  scale * (y - camY) == -1  =>  y - camY == -1 / scale
 */
            Y = (1 - scale * (y - camY)) * height / 2;   // scale the vertical difference

            W = scale * roadW * width / 2;
        }

        void drawSprite( olc::PixelGameEngine *gfx ) {
            if (pDec == nullptr) {
                    std::cout << "WARNING: drawSprite() --> called while pDec == nullptr" << std::endl;
            } else {
                int w = decWidth;
                int h = decHeight;

                float destX = X + scale * spriteX * width / 2.0f;   // project X for spriteX
                float destY = Y + 4.0f;                             // Sprite is lifted up 4 pixels w.r.t. the line it's on

                float destW = w * W / 266.0f;    // scale the size of the sprite
                float destH = h * W / 266.0f;    // why is this number 266 used?

                destX += destW * spriteX;        // offsetX
                destY += destH * (-1);           // offsetY

                float clipH = destY + destH - clip;
                if (clipH < 0) clipH = 0;
                if (clipH >= destH) return;

                gfx->DrawPartialDecal(
                    olc::vf2d( destX, destY ),
                    olc::vf2d( destW, destH ),
                    pDec,
                    olc::vf2d( 0, 0 ),
                    olc::vf2d( w, h - h * clipH / destH )
                );
            }
        }
    };

    // my implementation of DrawQuad(), using Decal drawing functionality
    void DrawQuad( olc::Decal *decPtr, olc::Pixel &c, int x1, int y1, int w1, int x2, int y2, int w2 ) {

        if (y1 >= SCREEN_Y / 2 && y2 >= SCREEN_Y / 2) { // only draw a quad if the input data can be trusted!!
                                                        // this is a hack to prevent a persistent bug

            // create vector of quad corner points from the parameters
            std::vector<olc::vf2d> points = {
                { float( x1 - w1 ), float( y1 ) },
                { float( x2 - w2 ), float( y2 ) },
                { float( x2 + w2 ), float( y2 ) },
                { float( x1 + w1 ), float( y1 ) }
            };
            // create vector of texture coordinates from vector of points (just normalize in [0.0f, 1.0f])
            std::vector<olc::vf2d> textures;
            for (auto elt : points) {
                olc::vf2d texture = { elt.x / float( SCREEN_X ), elt.y / float( SCREEN_Y ) };
                textures.push_back( texture );
            }
            // draw the quad using the decal ptr and colour
            DrawPolygonDecal( decPtr, points, textures, c );
        }
    }

    std::vector<Line> lines;
    int N;
    int pos     = 0;
    int playerX = 0;

// colours for grass, rumble and road
#define  GRASS_COL1    olc::Pixel(  16, 200,  16 )    // two shades of green
#define  GRASS_COL2    olc::Pixel(   0, 154,   0 )
#define RUMBLE_COL1    olc::Pixel( 255, 255, 255 )    // black and white blocks
#define RUMBLE_COL2    olc::Pixel(   0,   0,   0 )
#define   ROAD_COL1    olc::Pixel( 107, 107, 107 )    // two nuances of grey
#define   ROAD_COL2    olc::Pixel( 103, 103, 103 )

    // for the DrawQuad() function, just create 1 white decal, and tint it with the right colour...
    olc::Decal *pWhiteDec  = nullptr;
    olc::Decal *pBackgrDec = nullptr;
    olc::Decal *pTreeDec   = nullptr;

public:
    bool OnUserCreate() override {

        // initialize a white decal
        auto create_white_decal = [=] () {
            olc::Sprite *auxSpritePtr = new olc::Sprite( SCREEN_X, SCREEN_Y );
            SetDrawTarget( auxSpritePtr );
            Clear( olc::WHITE );
            SetDrawTarget( nullptr );
            olc::Decal *auxDecalPtr = new olc::Decal( auxSpritePtr );
            delete auxSpritePtr;
            return auxDecalPtr;
        };
        pWhiteDec = create_white_decal();

        auto load_decal_from_file = [=]( std::string sFileName, int &w, int &h ) {
            olc::Sprite *auxSpritePtr = new olc::Sprite( sFileName );
            olc::Decal *auxDecalPtr = new olc::Decal( auxSpritePtr );
            w = auxSpritePtr->width;
            h = auxSpritePtr->height;
            delete auxSpritePtr;
            if (w == 0 || h == 0) {
                delete auxDecalPtr;
                auxDecalPtr = nullptr;
            }
            return auxDecalPtr;
        };
        int notUsed, wTree, hTree;
        pBackgrDec   = load_decal_from_file( "./images/bg.png", notUsed, notUsed );
        if (pBackgrDec == nullptr) std::cout << "ERROR: OnUserCreate() --> couldn't load ./images/bg.png" << std::endl;
        pTreeDec = load_decal_from_file( "./images/5.png" , wTree, hTree );
        if (pTreeDec == nullptr) std::cout << "ERROR: OnUserCreate() --> couldn't load ./images/2.png" << std::endl;

        // fill the screen lines with the distance info
        // store the depth info for 1600 segments
        // where is this choice of 1600 coming from?
        for (int i = 0; i < 1600; i++) {
            Line line;
            line.z = i * segL;
            // put curve in this traject
            if (i > 300 && i < 700) line.curve = 0.5;
            // put hills in this traject
            if (i > 750) line.y = sin( i / 30.0f ) * 1500;
            // put trees along the track
            if ((i % 20) == 0) {
                line.spriteX = -2.5f;
                line.pDec = pTreeDec;
                line.decWidth = wTree;
                line.decHeight = hTree;
            } else {
                line.spriteX = 0.0f;
                line.pDec = nullptr;
                line.decWidth = 0.0f;
                line.decHeight = 0.0f;
            }

            lines.push_back( line );
        }
        N = lines.size();

        return true;
    }

    bool OnUserUpdate( float fElapsedTime ) override {

/* How is this number of 300 lines determined?
 * Testing turns out that with about 300 lines the screen is filled bottom up to halfway (the horizon).
 * Adding more lines doesn't add any visual effect, but removing lines is visible, and brings the
 * horizon closer and lower on the screen.
 */
        int nrLines = 300;

        // user controls
        if (GetKey( olc::Key::RIGHT ).bHeld) playerX += 150;
        if (GetKey( olc::Key::LEFT  ).bHeld) playerX -= 150;
        if (GetKey( olc::Key::UP    ).bHeld) pos     += 150;
        if (GetKey( olc::Key::DOWN  ).bHeld) pos     -= 150;

        // wrap around the track
        while (pos >= N * segL) pos -= N * segL;
        while (pos < 0) pos += N * segL;

        // drawing

        // draw background decal
        DrawPartialDecal(
            { 0.0f, 0.0f },
            { SCREEN_X, SCREEN_Y / 2 + 20.0f },  // fill top half of the screen
            pBackgrDec,
            {   0.0f,   0.0f },          // use complete bg decal
            { 768.0f, 411.0f }
        );

        int startPos = pos / segL;
        int camH = 1500 + lines[startPos].y;
        float x = 0.0f;
        float dx = 0.0f;
        int maxy = height;

        // draw road
        for (int n = startPos; n < startPos + nrLines; n++) {
            Line &l = lines[ n % N ];

/* Whence the & here in the declartion of variable l?
 * geeksforgeeks: "When a variable is declared as a reference, it becomes an alternative name
 *                 for an existing variable. A variable can be declared as a reference by +
 *                 putting ‘&’ in the declaration."
 * In this case, instead of adressing lines[n%N].project(), you can adress l.project()
*/
            // parameters are camX, camY and camZ
            // where is the choice of 1500 (now part of camH) coming from?
            l.project( playerX - x, camH, pos - (n >= N ? N * segL : 0));
            x += dx;
            dx += l.curve;

            l.clip = maxy;  // this statement is not explicitly handled in the vid. It is visible, but that's it :(
                            // took me a lot of time to debug this

            if (l.Y >= maxy) {    // this code was buggy as well. The maxy value wasn't altered if l.Y >= maxy.
                maxy = l.Y;       // This caused a lot of skipped quad drawing...
                continue;
            }

            olc::Pixel cGrass  = ((n / 3) % 2) ? GRASS_COL1  : GRASS_COL2 ;
            olc::Pixel cRumble = ((n / 3) % 2) ? RUMBLE_COL1 : RUMBLE_COL2;
            olc::Pixel cRoad   = ((n / 3) % 2) ? ROAD_COL1   : ROAD_COL2  ;

            Line p = lines[ (n - 1) % N ];  // previous line

            // draw grass, rumble and road
            DrawQuad( pWhiteDec, cGrass , int(   0 ), int( p.Y ), int( width      ), int(   0 ), int( l.Y ), int( width      ));
            DrawQuad( pWhiteDec, cRumble, int( p.X ), int( p.Y ), int( p.W * 1.2f ), int( l.X ), int( l.Y ), int( l.W * 1.2f ));
            DrawQuad( pWhiteDec, cRoad  , int( p.X ), int( p.Y ), int( p.W        ), int( l.X ), int( l.Y) , int( l.W        ));
        }

        // draw objects
        for (int n = startPos + nrLines; n > startPos; n--) {
            Line line = lines[ n % N ];
            if (line.pDec != nullptr)
                line.drawSprite( this );
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
