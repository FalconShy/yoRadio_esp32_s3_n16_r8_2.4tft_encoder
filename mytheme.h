#ifndef _my_theme_h
#define _my_theme_h

/*
    Neonowy motyw dla YoRadio
    Agresywny, ale czytelny
*/

#define ENABLE_THEME
#ifdef ENABLE_THEME

#define COLOR_BACKGROUND         0,   0,  0     // głębszy granat, mniej czerni
#define COLOR_STATION_NAME       0, 220, 220     // cyjan, ale nie wypalarka
#define COLOR_STATION_BG         0,   0,   0
#define COLOR_STATION_FILL       0, 200, 200

#define COLOR_SNG_TITLE_1        220,  0, 220
#define COLOR_SNG_TITLE_2        255, 140, 20

#define COLOR_WEATHER            210, 190, 40

#define COLOR_VU_MAX             220,  40,  40   // nie krzyczy
#define COLOR_VU_MIN             0, 255, 0

#define COLOR_CLOCK              0, 220, 220
#define COLOR_CLOCK_BG           0,   0,   0
#define COLOR_SECONDS            220, 0, 120

#define COLOR_DAY_OF_W           0, 200, 120
#define COLOR_DATE               0, 120, 220

#define COLOR_BUFFER             10, 10, 10
#define COLOR_IP                 220, 220, 0
#define COLOR_VOLUME_VALUE       0, 200, 120
#define COLOR_RSSI               220, 0, 220

#define COLOR_VOLBAR_OUT         255, 140, 20
#define COLOR_VOLBAR_IN          0,   0, 220

#define COLOR_DIGITS             0, 220, 220
#define COLOR_DIVIDER            200, 0, 200
#define COLOR_BITRATE            220, 40, 40
#define COLOR_HEAP               0, 200, 120

// Aktualny (bez zmian)
#define COLOR_PL_CURRENT         220, 220, 0
#define COLOR_PL_CURRENT_BG      0,   0,   0
#define COLOR_PL_CURRENT_FILL    220, 0, 120

#define COLOR_PLAYLIST_0         0,   220, 220   
#define COLOR_PLAYLIST_1         187, 0,   187   
#define COLOR_PLAYLIST_2         0,   84,  154   
#define COLOR_PLAYLIST_3         140, 77,  11    
#define COLOR_PLAYLIST_4         50,  50,  0

#endif
#endif
