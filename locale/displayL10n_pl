#ifndef dsp_full_loc
#define dsp_full_loc
#include <pgmspace.h>

/*************************************************************************************
    HOWTO:
    Skopiuj ten plik do yoRadio/locale/displayL10n_custom.h
    i zmodyfikuj według potrzeb
*************************************************************************************/

// Dni tygodnia skrócone
const char mon[] PROGMEM = "pn";
const char tue[] PROGMEM = "wt";
const char wed[] PROGMEM = "śr";
const char thu[] PROGMEM = "cz";
const char fri[] PROGMEM = "pt";
const char sat[] PROGMEM = "sb";
const char sun[] PROGMEM = "nd";

// Dni tygodnia pełne
const char monf[] PROGMEM = "poniedziałek";
const char tuef[] PROGMEM = "wtorek";
const char wedf[] PROGMEM = "środa";
const char thuf[] PROGMEM = "czwartek";
const char frif[] PROGMEM = "piątek";
const char satf[] PROGMEM = "sobota";
const char sunf[] PROGMEM = "niedziela";

// Miesiące
const char jan[]  PROGMEM = "styczeń";
const char feb[]  PROGMEM = "luty";
const char mar[]  PROGMEM = "marzec";
const char apr[]  PROGMEM = "kwiecień";
const char may[]  PROGMEM = "maj";
const char jun[]  PROGMEM = "czerwiec";
const char jul[]  PROGMEM = "lipiec";
const char aug[]  PROGMEM = "sierpień";
const char sep[]  PROGMEM = "wrzesień";
const char octt[] PROGMEM = "październik";
const char nov[]  PROGMEM = "listopad";
const char decc[] PROGMEM = "grudzień";

// Kierunki wiatru
const char wn_N[]   PROGMEM = "PÓŁNOC";
const char wn_NNE[] PROGMEM = "PNP";
const char wn_NE[]  PROGMEM = "PNW";
const char wn_ENE[] PROGMEM = "WNP";
const char wn_E[]   PROGMEM = "WSCHÓD";
const char wn_ESE[] PROGMEM = "WSP";
const char wn_SE[]  PROGMEM = "PŁD-WSCH";
const char wn_SSE[] PROGMEM = "PPW";
const char wn_S[]   PROGMEM = "POŁUDNIE";
const char wn_SSW[] PROGMEM = "PPZ";
const char wn_SW[]  PROGMEM = "PŁD-ZACH";
const char wn_WSW[] PROGMEM = "ZSP";
const char wn_W[]   PROGMEM = "ZACHÓD";
const char wn_WNW[] PROGMEM = "ZNP";
const char wn_NW[]  PROGMEM = "PN-ZACH";
const char wn_NNW[] PROGMEM = "PNP";

// Tablice
const char* const dow[]   PROGMEM = { sun, mon, tue, wed, thu, fri, sat };
const char* const dowf[]  PROGMEM = { sunf, monf, tuef, wedf, thuf, frif, satf };
const char* const mnths[] PROGMEM = { jan, feb, mar, apr, may, jun, jul, aug, sep, octt, nov, decc };
const char* const wind[]  PROGMEM = {
  wn_N, wn_NNE, wn_NE, wn_ENE, wn_E, wn_ESE, wn_SE, wn_SSE,
  wn_S, wn_SSW, wn_SW, wn_WSW, wn_W, wn_WNW, wn_NW, wn_NNW, wn_N
};

// Statusy i dialogi
const char const_PlReady[]   PROGMEM = "[gotowe]";
const char const_PlStopped[] PROGMEM = "[zatrzymane]";
const char const_PlConnect[] PROGMEM = "[łączenie]";
const char const_DlgVolume[] PROGMEM = "GŁOŚNOŚĆ";
const char const_DlgLost[]   PROGMEM = "* BRAK SYGNAŁU *";
const char const_DlgUpdate[] PROGMEM = "* AKTUALIZACJA *";
const char const_DlgNextion[]PROGMEM = "* NEXTION *";
const char const_getWeather[]PROGMEM = "";
const char const_waitForSD[] PROGMEM = "INDEKS SD";

// AP / WiFi
const char apNameTxt[] PROGMEM = "NAZWA AP";
const char apPassTxt[] PROGMEM = "HASŁO";
const char bootstrFmt[] PROGMEM = "Próba połączenia: %s";
const char apSettFmt[] PROGMEM = "STRONA USTAWIEŃ: HTTP://%s/";

// Pogoda
#if EXT_WEATHER
const char weatherFmt[] PROGMEM =
  "%s, %.1f\011C \007 odczuwalna: %.1f\011C \007 ciśnienie: %d hPa \007 wilgotność: %s%% \007 wiatr: %.1f m/s [%s]";
#else
const char weatherFmt[] PROGMEM =
  "%s, %.1f\011C \007 ciśnienie: %d hPa \007 wilgotność: %s%%";
#endif

const char weatherUnits[] PROGMEM = "metric";
const char weatherLang[]  PROGMEM = "pl";

#endif
