/*
**  $Filename: gfx\gfx.ph
**  $Release:  0
**  $Revision: 0.1
**  $Date:     06-02-94
**
**  private gfx includes
**
**   (c) 1994 ...and avoid panic by, H. Gaberschek
**      All Rights Reserved.
**
*/
/****************************************************************************
  Portions copyright (c) 2005 Vasco Alexandre da Silva Costa

  Please read the license terms contained in the LICENSE and
  publiclicensecontract.doc files which should be contained with this
  distribution.
 ****************************************************************************/

namespace Clue {

#define GFX_SEG_ADDR                    (0xA0000000L)
#define GFX_PAGE_SIZE                   (64000L)    /* 320*200 = 64000 Bytes groß */

#define GFX_LOAD_BUFFER                 StdBuffer0
#define GFX_DECR_BUFFER                 DecrBuffer

#define GFX_DECR_BUFFER_SIZE            (320*192)

#define GFX_BUBBLE_FONT_NAME            ("bubble.fnt")
#define GFX_MENU_FONT_NAME              ("menu.fnt")

#define GFX_CMAP_OFFSET                 61440   // -> Maximalgröße 320 * 192

struct PrintRect {
	uint16 us_X;
	uint16 us_Width;
};

struct ColorRange {
	byte uch_Start;
	byte uch_End;
};

struct GfxBase {
	GC *gc;

	byte uch_VideoMode;
};

GC LowerGC;
GC MenuGC;
GC LSUpperGC;
GC LSMenuGC;

/* wird verwendet für Double Buffering während der Vorbereitung */
MemRastPort ScratchRP;
/* Da man von Mem nur ganze Blöcke kopieren kann (ohne Modulo)    */
/* müssen einige Grafiken, bevor sie ins Video RAM kopiert werden */
/* hier an dieser Stelle gepuffert werden. */
/* ist als extern definfiert, da Animmodul darauf zugreifen muß */


MemRastPort StdRP0InMem;
/* wird benutzt für Objekte, die immer im Speicher sind */
MemRastPort StdRP1InMem;
/* wird benutzt für Objekte, die immer im Speicher sind */
MemRastPort RefreshRPInMem;
/* Bild, das im Moment gerade gezeigt wird (für Refresh) */
MemRastPort AnimRPInMem;
/* beinhaltet die Animphasen des gerade aktuellen Bildes */
/* um Inkonsistenzen zu vermeiden, wird jedesmal, bevor eine */
/* Animphase gezeigt wird, das gesamte Bild in den PrepareRp */
/* kopiert, der benötigte Ausschnitt wird dann von dort */
/* ins Video-RAM kopiert */
/* ist als extern definfiert, da Animmodul darauf zugreifen muß */
MemRastPort AddRPInMem;
/* in diesem Rastport befinden sich diverse zusätzliche Grafiken */
/* die in das Bild hinzugefügt werden (Gesichter, Werkzeuge)     */
/* je nach Situation...                                                       */
/* dieser RP wird je nach Situation ausgewechselt                     */
MemRastPort LSFloorRPInMem;
/* in diesem RastPort befindet sich während der Planungs-Einbruchs */
/* phasen das Bild der Böden */
MemRastPort LSObjectRPInMem;
/* in diesem RastPort befinden sich einige Objekte während der Planung */


MemRastPort BobRPInMem;

MemRastPort LSRPInMem;

Graphics::PixelFormat ScreenFormat;
Graphics::Surface *GfxBoardBase;
Graphics::Surface *Screen;

byte DecrBuffer[GFX_DECR_BUFFER_SIZE];

struct PrintRect GlobalPrintRect;
struct ColorRange GlobalColorRange;

struct GfxBase GfxBase;

List *CollectionList = NULL;
List *PictureList = NULL;

GC *l_gc;
GC *m_gc;
GC *u_gc;

struct Font *bubbleFont;
struct Font *menuFont;

static void gfxInitGC(GC *gc, uint16 x, uint16 y, uint16 w, uint16 h,
                      uint8 colorStart, uint8 colorEnd, Font *font);

static void gfxInitCollList();
static void gfxInitPictList();

static void gfxSetCMAP(const uint8 *src);

static Font *gfxOpenFont(const char *fileName, uint16 w, uint16 h,
                         unsigned char first, unsigned char last,
                         uint16 sw, uint16 sh);
static void gfxCloseFont(Font *font);

} // End of namespace Clue
