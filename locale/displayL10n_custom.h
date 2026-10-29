
#ifndef dsp_full_loc
#define dsp_full_loc
#include <pgmspace.h>

/*************************************************************************************
    HOWTO:
    Copy this file to yoRadio/locale/displayL10n_custom.h
    and modify it as needed.
*************************************************************************************/

// Short weekday names
const char mon[] PROGMEM = "Mon";
const char tue[] PROGMEM = "Tue";
const char wed[] PROGMEM = "Wed";
const char thu[] PROGMEM = "Thu";
const char fri[] PROGMEM = "Fri";
const char sat[] PROGMEM = "Sat";
const char sun[] PROGMEM = "Sun";

// Full weekday names
const char monf[] PROGMEM = "Monday";
const char tuef[] PROGMEM = "Tuesday";
const char wedf[] PROGMEM = "Wednesday";
const char thuf[] PROGMEM = "Thursday";
const char frif[] PROGMEM = "Friday";
const char satf[] PROGMEM = "Saturday";
const char sunf[] PROGMEM = "Sunday";

// Months
const char jan[]  PROGMEM = "January";
const char feb[]  PROGMEM = "February";
const char mar[]  PROGMEM = "March";
const char apr[]  PROGMEM = "April";
const char may[]  PROGMEM = "May";
const char jun[]  PROGMEM = "June";
const char jul[]  PROGMEM = "July";
const char aug[]  PROGMEM = "August";
const char sep[]  PROGMEM = "September";
const char octt[] PROGMEM = "October";
const char nov[]  PROGMEM = "November";
const char decc[] PROGMEM = "December";

// Wind directions
const char wn_N[]   PROGMEM = "NORTH";
const char wn_NNE[] PROGMEM = "NNE";
const char wn_NE[]  PROGMEM = "NE";
const char wn_ENE[] PROGMEM = "ENE";
const char wn_E[]   PROGMEM = "EAST";
const char wn_ESE[] PROGMEM = "ESE";
const char wn_SE[]  PROGMEM = "SE";
const char wn_SSE[] PROGMEM = "SSE";
const char wn_S[]   PROGMEM = "SOUTH";
const char wn_SSW[] PROGMEM = "SSW";
const char wn_SW[]  PROGMEM = "SW";
const char wn_WSW[] PROGMEM = "WSW";
const char wn_W[]   PROGMEM = "WEST";
const char wn_WNW[] PROGMEM = "WNW";
const char wn_NW[]  PROGMEM = "NW";
const char wn_NNW[] PROGMEM = "NNW";

// Lookup tables
const char* const dow[] PROGMEM = {
  sun, mon, tue, wed, thu, fri, sat
};

const char* const dowf[] PROGMEM = {
  sunf, monf, tuef, wedf, thuf, frif, satf
};

const char* const mnths[] PROGMEM = {
  jan, feb, mar, apr, may, jun,
  jul, aug, sep, octt, nov, decc
};

const char* const wind[] PROGMEM = {
  wn_N, wn_NNE, wn_NE, wn_ENE,
  wn_E, wn_ESE, wn_SE, wn_SSE,
  wn_S, wn_SSW, wn_SW, wn_WSW,
  wn_W, wn_WNW, wn_NW, wn_NNW, wn_N
};

// Status messages and dialogs
const char const_PlReady[]    PROGMEM = "[ready]";
const char const_PlStopped[]  PROGMEM = "[stopped]";
const char const_PlConnect[]  PROGMEM = "[connecting]";
const char const_DlgVolume[]  PROGMEM = "VOLUME";
const char const_DlgLost[]    PROGMEM = "* SIGNAL LOST *";
const char const_DlgUpdate[]  PROGMEM = "* UPDATING *";
const char const_DlgNextion[] PROGMEM = "* NEXTION *";
const char const_getWeather[] PROGMEM = "";
const char const_waitForSD[]  PROGMEM = "SD INDEXING";

// AP / WiFi
const char apNameTxt[] PROGMEM = "AP NAME";
const char apPassTxt[] PROGMEM = "PASSWORD";
const char bootstrFmt[] PROGMEM = "Connecting to: %s";
const char apSettFmt[] PROGMEM = "SETTINGS PAGE: HTTP://%s/";

// Weather
#if EXT_WEATHER
const char weatherFmt[] PROGMEM =
  "%s, %s, %.1f\011C \007 feels like: %.1f\011C \007 pressure: %d hPa \007 humidity: %s%% \007 wind: %.1f m/s [%s]";
#else
const char weatherFmt[] PROGMEM =
  "%s, %s, %.1f\011C \007 pressure: %d hPa \007 humidity: %s%%";
#endif

const char weatherUnits[] PROGMEM = "metric";
const char weatherLang[]  PROGMEM = "en";

#endif
