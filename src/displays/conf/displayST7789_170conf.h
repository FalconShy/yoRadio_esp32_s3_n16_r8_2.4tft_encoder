/*************************************************************************************
    ST7789 320x170 displays configuration file.
    Copy this file to yoRadio/src/displays/conf/displayST7789conf_custom.h
    and modify it
    More info on https://github.com/e2002/yoradio/wiki/Widgets#widgets-description
*************************************************************************************/

#ifndef displayST7789conf_h
#define displayST7789conf_h

#define DSP_WIDTH       320
#define DSP_HEIGHT      170
#define TFT_FRAMEWDT    4
#define MAX_WIDTH       DSP_WIDTH-TFT_FRAMEWDT*2

#if BITRATE_FULL
  #define TITLE_FIX 44
#else
  #define TITLE_FIX 0
#endif
#define bootLogoTop     38    // było 68, przesunięcie proporcjonalne do wysokości

/* SROLLS  */                               /* {{ left, top, fontsize, align }, buffsize, uppercase, width, scrolldelay, scrolldelta, scrolltime } */
const ScrollConfig metaConf       PROGMEM = {{ TFT_FRAMEWDT, TFT_FRAMEWDT, 3, WA_LEFT }, 140, false, MAX_WIDTH, 5000, 5, 30 };
const ScrollConfig title1Conf     PROGMEM = {{ TFT_FRAMEWDT, 34, 2, WA_LEFT }, 140, false, MAX_WIDTH, 5000, 4, 30 };   // było 48
const ScrollConfig title2Conf     PROGMEM = {{ TFT_FRAMEWDT, 54, 2, WA_LEFT }, 140, false, MAX_WIDTH, 5000, 4, 30 };   // było 72
const ScrollConfig playlistConf   PROGMEM = {{ TFT_FRAMEWDT, 82, 2, WA_LEFT }, 140, false, MAX_WIDTH, 1000, 2, 30 };             // było 112
const ScrollConfig apTitleConf    PROGMEM = {{ TFT_FRAMEWDT, TFT_FRAMEWDT, 3, WA_CENTER }, 140, false, MAX_WIDTH, 0, 4, 20 };
const ScrollConfig apSettConf     PROGMEM = {{ TFT_FRAMEWDT, DSP_HEIGHT-TFT_FRAMEWDT-16, 2, WA_LEFT }, 140, false, MAX_WIDTH, 0, 4, 30 }; // było 240
const ScrollConfig weatherConf    PROGMEM = {{ 8, 60, 2, WA_LEFT }, 140, false, MAX_WIDTH, 0, 4, 30 }; // było 87

/* BACKGROUNDS  */                          /* {{ left, top, fontsize, align }, width, height, outlined } */
const FillConfig   metaBGConf     PROGMEM = {{ 0, 0, 0, WA_LEFT }, DSP_WIDTH, 30, false };      // było 38
const FillConfig   metaBGConfInv  PROGMEM = {{ 0, 28, 0, WA_LEFT }, DSP_WIDTH, 1, false };      // było 38
const FillConfig   volbarConf     PROGMEM = {{ TFT_FRAMEWDT, DSP_HEIGHT-TFT_FRAMEWDT-3, 0, WA_LEFT }, MAX_WIDTH, 4, true }; // było 240
const FillConfig  playlBGConf     PROGMEM = {{ 0, 65, 0, WA_LEFT }, DSP_WIDTH, 18, false };     // było 107, 24
const FillConfig  heapbarConf     PROGMEM = {{ 0, DSP_HEIGHT-1, 0, WA_LEFT }, DSP_WIDTH, 1, false }; // było 239

/* WIDGETS  */                              /* { left, top, fontsize, align } */
const WidgetConfig bootstrConf    PROGMEM = { 0, 150, 1, WA_CENTER };   // było 182
const WidgetConfig bitrateConf    PROGMEM = { 70, 127, 1, WA_LEFT };    // było 191

const WidgetConfig voltxtConf     PROGMEM = { 0, 151, 1, WA_CENTER };   // było 214
const WidgetConfig  iptxtConf     PROGMEM = { TFT_FRAMEWDT, 151, 1, WA_LEFT }; // było 214
const WidgetConfig   rssiConf     PROGMEM = { TFT_FRAMEWDT, 145, 2, WA_RIGHT }; // było 208

const WidgetConfig numConf        PROGMEM = { 0, 90+20, 52, WA_CENTER }; // było 120+30
const WidgetConfig apNameConf     PROGMEM = { TFT_FRAMEWDT, 40, 2, WA_CENTER }; // było 66
const WidgetConfig apName2Conf    PROGMEM = { TFT_FRAMEWDT, 60, 2, WA_CENTER }; // było 90
const WidgetConfig apPassConf     PROGMEM = { TFT_FRAMEWDT, 90, 2, WA_CENTER }; // było 130
const WidgetConfig apPass2Conf    PROGMEM = { TFT_FRAMEWDT, 110, 2, WA_CENTER }; // było 154
const WidgetConfig  clockConf     PROGMEM = { 0, 130, 40, WA_RIGHT };   // przesunięcie zegara
const WidgetConfig vuConf         PROGMEM = { TFT_FRAMEWDT, 80, 1, WA_LEFT }; // było 100
const WidgetConfig bootWdtConf    PROGMEM = { TFT_FRAMEWDT, 120, 2, WA_CENTER };
const ProgressConfig bootPrgConf  PROGMEM = { 100, 14, 4 };
                                              /* przesuniecie od lewej, przesunięcie od góry, x, wielkość czcionki, ogólna wielkość*/
const BitrateConfig fullbitrateConf PROGMEM = {{ 70, 95, 2, WA_LEFT}, 48 }; // było 43 a potem 34

/* BANDS  */                                /* { onebandwidth, onebandheight, bandsHspace, bandsVspace, numofbands, fadespeed } */
const VUBandsConfig bandsConf     PROGMEM = { 24, 65, 4, 2, 10, 2 };    // było 100

/* STRINGS  */
const char         numtxtFmt[]    PROGMEM = "%d";
const char           rssiFmt[]    PROGMEM = "WiFi %d";
const char          iptxtFmt[]    PROGMEM = "\010 %s";
const char         voltxtFmt[]    PROGMEM = "\023\025%d";
const char        bitrateFmt[]    PROGMEM = "%d kBs";

/* MOVES  */                                /* { left, top, width } */
const MoveConfig    clockMove     PROGMEM = { 8, 120, -1 };             // było 180 - nie wiem do czego to
const MoveConfig   weatherMoveVU  PROGMEM = { 70, 74, 250 };            // było 97
const MoveConfig   weatherMove    PROGMEM = { 8, 74, MAX_WIDTH };       // było 97

#endif

