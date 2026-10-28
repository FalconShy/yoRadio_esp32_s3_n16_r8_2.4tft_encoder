/*************************************************************************************
    ST7789 320x240 displays configuration file.
    Copy this file to yoRadio/src/displays/conf/displayST7789conf_custom.h
    and modify it
    More info on https://github.com/e2002/yoradio/wiki/Widgets#widgets-description
*************************************************************************************/

#ifndef displayST7789conf_h
#define displayST7789conf_h

#define DSP_WIDTH       320
#define TFT_FRAMEWDT    2
#define MAX_WIDTH       DSP_WIDTH-TFT_FRAMEWDT*2
//#define PLMITEMS        11
//#define PLMITEMLENGHT   40
//#define PLMITEMHEIGHT   22

#if BITRATE_FULL
  #define TITLE_FIX 44
#else
  #define TITLE_FIX 0
#endif
#define bootLogoTop     68

/* SROLLS  */                            /* {{ left, top, fontsize, align }, buffsize, uppercase, width, scrolldelay, scrolldelta, scrolltime } */
const ScrollConfig metaConf       PROGMEM = {{ 3, 89, 3, WA_CENTER }, 140, true, MAX_WIDTH-4, 5000, 1, 30 };
const ScrollConfig title1Conf     PROGMEM = {{ 3, 128, 2, WA_CENTER }, 140, true, MAX_WIDTH, 5000, 1, 30 };
const ScrollConfig title2Conf     PROGMEM = {{ 3, 153, 2, WA_CENTER }, 140, true, MAX_WIDTH, 5000, 1, 30 };
const ScrollConfig playlistConf   PROGMEM = {{ TFT_FRAMEWDT, 112, 2, WA_LEFT }, 140, true, MAX_WIDTH, 1000, 2, 30 };
const ScrollConfig apTitleConf    PROGMEM = {{ TFT_FRAMEWDT, TFT_FRAMEWDT, 3, WA_CENTER }, 140, false, MAX_WIDTH, 0, 4, 20 };
const ScrollConfig apSettConf     PROGMEM = {{ TFT_FRAMEWDT, 240-TFT_FRAMEWDT-16, 2, WA_LEFT }, 140, false, MAX_WIDTH, 0, 4, 30 };
const ScrollConfig weatherConf    PROGMEM = {{ 3, 48, 2, WA_RIGHT }, 140, true, MAX_WIDTH-4, 5000, 1, 60 };

/* BACKGROUNDS  */                       /* {{ left, top, fontsize, align }, width, height, outlined } */
const FillConfig metaBGConf       PROGMEM = {{3, 68, 0, WA_CENTER}, DSP_WIDTH - 6, 2, true}; // Trage o linie 
//const FillConfig   metaBGConf     PROGMEM = {{ 0, 0, 0, WA_LEFT }, DSP_WIDTH, 38, false };
const FillConfig   metaBGConfInv  PROGMEM = {{ 0, 183, 0, WA_LEFT }, DSP_WIDTH, 1, false }; 
const FillConfig   volbarConf     PROGMEM = {{ 3, 183, 0, WA_CENTER }, MAX_WIDTH, 2, true };  // a doua linie 

const FillConfig  playlBGConf     PROGMEM = {{ 0, 107, 0, WA_LEFT }, DSP_WIDTH, 24, false };
const FillConfig  heapbarConf     PROGMEM = {{ 0, 235, 0, WA_LEFT }, DSP_WIDTH, 1, false };

/* WIDGETS  */                           /* { left, top, fontsize, align } */
const WidgetConfig bootstrConf    PROGMEM = { 0, 182, 1, WA_CENTER };

const WidgetConfig bitrateConf    PROGMEM = { 30, 194, 1, WA_LEFT };    // było 191

const WidgetConfig iptxtConf     PROGMEM = { 10, 9, 1, WA_LEFT };
const WidgetConfig  voltxtConf     PROGMEM = { 20, 194, 2, WA_RIGHT };
const WidgetConfig   rssiConf     PROGMEM = { 20, 26, 2, WA_LEFT };
const WidgetConfig numConf        PROGMEM = { 0, 160, 52, WA_CENTER };
const WidgetConfig apNameConf     PROGMEM = { TFT_FRAMEWDT, 66, 2, WA_CENTER };
const WidgetConfig apName2Conf    PROGMEM = { TFT_FRAMEWDT, 90, 2, WA_CENTER };
const WidgetConfig apPassConf     PROGMEM = { TFT_FRAMEWDT, 130, 2, WA_CENTER };
const WidgetConfig apPass2Conf    PROGMEM = { TFT_FRAMEWDT, 154, 2, WA_CENTER };

const WidgetConfig  clockConf     PROGMEM = { 0, 39, 34, WA_RIGHT };   // offset ceas 230 POZITIE CEAS 
//const WidgetConfig vuConf         PROGMEM = { 0, 135, 1, WA_LEFT }; // było 100 ptr vertical
const WidgetConfig vuConf         PROGMEM = { 27, 198, 1, WA_CENTER };  //bun0612
const WidgetConfig bootWdtConf    PROGMEM = { 0, 162, 1, WA_CENTER };
const ProgressConfig bootPrgConf  PROGMEM = { 90, 14, 4 };
//const BitrateConfig fullbitrateConf PROGMEM = {{ 40, 145, 3, WA_LEFT}, 85 }; // było 43 a potem 34
const BitrateConfig fullbitrateConf PROGMEM = {{229, 218, 2, WA_RIGHT}, 34 };
//const WidgetConfig vuConf         PROGMEM = { 1, 28, 1, WA_LEFT };

/* BANDS  */                             /* { onebandwidth, onebandheight, bandsHspace, bandsVspace, numofbands, fadespeed } */
//const VUBandsConfig bandsConf     PROGMEM = { 12, 48, 2, 1, 8, 3 }; //pentru vertical
//const VUBandsConfig bandsConf     PROGMEM = { (MAX_WIDTH-TFT_FRAMEWDT*1)/2, 8, TFT_FRAMEWDT, 1, 15, 3 };	//Boombox
//const VUBandsConfig bandsConf     PROGMEM = { 220, 15, TFT_FRAMEWDT + 3, 1, 40, 3 };	//Boombox
const VUBandsConfig bandsConf     PROGMEM = { 152, 12, 13, 4, 16, 4 };	//Boombox 0612
//const VUBandsConfig bandsConf     PROGMEM = { 140, 6, 22, 2, 12, 5 };" 142, 15, 12, 5, 22, 4


/* STRINGS  */
const char         numtxtFmt[]    PROGMEM = "%d";
const char           rssiFmt[]    PROGMEM = "%ddB";
const char          iptxtFmt[]    PROGMEM = "\010 %s";
const char         voltxtFmt[]    PROGMEM = "VOL%d";
const char        bitrateFmt[]    PROGMEM = "%d kBs";

/* MOVES  */                             /* { left, top, width } */
const MoveConfig    clockMove     PROGMEM = { 0, 180, -1 };
const MoveConfig   weatherMove    PROGMEM = { 3, 48, MAX_WIDTH };
const MoveConfig   weatherMoveVU  PROGMEM = { 3, 48, MAX_WIDTH };

#endif
