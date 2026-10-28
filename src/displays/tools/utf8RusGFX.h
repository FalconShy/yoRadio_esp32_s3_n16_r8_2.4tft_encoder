#ifndef utf8RusGFX_h
#define utf8RusGFX_h

#include <ctype.h>
#include <string.h>

#ifndef BUFLEN
#define BUFLEN 256
#endif

char* DspCore::utf8Rus(const char* str, bool uppercase) {
    static char strn[BUFLEN];
    strlcpy(strn, str, BUFLEN);

    int index = 0;
    while (strn[index]) {
        uint8_t c1 = (uint8_t)strn[index];
        uint8_t c2 = (uint8_t)strn[index + 1];

        if (c1 == 0xC3 || c1 == 0xC4 || c1 == 0xC5) {
            bool found = true;
            
            switch (c1) {
                case 0xC3: // ó i Ó
                    if (c2 == 0xB3 || (uppercase && c2 == 0xB3)) strn[index] = uppercase ? 191 : 190; // ó -> Ó (191)
                    else if (c2 == 0x93) strn[index] = 191; // Ó
                    else found = false;
                    break;

                case 0xC4:
                    switch (c2) {
                        case 0x85: strn[index] = uppercase ? 183 : 184; break; // ą -> Ą (183)
                        case 0x84: strn[index] = 183; break; // Ą
                        case 0x87: strn[index] = uppercase ? 196 : 189; break; // ć -> Ć (196)
                        case 0x86: strn[index] = 196; break; // Ć
                        case 0x99: strn[index] = uppercase ? 215 : 214; break; // ę -> Ę (215)
                        case 0x98: strn[index] = 215; break; // Ę
                        default: found = false; break;
                    }
                    break;

                case 0xC5:
                    switch (c2) {
                        case 0x82: strn[index] = uppercase ? 208 : 207; break; // ł -> Ł (208)
                        case 0x81: strn[index] = 208; break; // Ł
                        case 0x84: strn[index] = uppercase ? 193 : 192; break; // ń -> Ń (193)
                        case 0x83: strn[index] = 193; break; // Ń
                        case 0x9A: strn[index] = uppercase ? 204 : 203; break; // ś -> Ś (204)
                        case 0x9B: strn[index] = 204; break; // Ś
                        case 0xBA: strn[index] = uppercase ? 188 : 187; break; // ź -> Ź (188)
                        case 0xB9: strn[index] = 188; break; // Ź
                        case 0xBC: strn[index] = uppercase ? 186 : 185; break; // ż -> Ż (186)
                        case 0xBB: strn[index] = 186; break; // Ż
                        default: found = false; break;
                    }
                    break;
                default: found = false; break;
            }

            if (found) {
                // Przesunięcie ciągu, aby usunąć zbędny bajt UTF-8
                int s = index + 2;
                while (strn[s-1]) { 
                    strn[s - 1] = strn[s]; 
                    s++; 
                }
                // Po znalezieniu polskiego znaku nie inkrementujemy indexu, 
                // bo toupper na końcu obsłuży tylko znaki < 128
            }
        }
        index++;
    }

    // Zamiana na wielkie litery dla standardowego ASCII (A-Z)
    if (uppercase) {
        for (char* p = strn; *p; p++) {
            if ((uint8_t)*p < 128) *p = toupper((unsigned char)*p);
        }
    }

    return strn;
}

#endif
