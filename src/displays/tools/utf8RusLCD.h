#ifndef utf8RusLCD_h
#define utf8RusLCD_h

#include <string.h>
#include <ctype.h>

char* DspCore::utf8Rus(const char* str, bool uppercase) {
    static char buf[BUFLEN];
    int i = 0, j = 0;
    while (str[i] && j < BUFLEN - 1) {
        unsigned char c = (unsigned char)str[i];

        // polskie znaki UTF-8
        if (c == 0xC4) { // ą ć ę ł ń ó ś ź ż
            unsigned char next = (unsigned char)str[i + 1];
            switch (next) {
                case 0x85: buf[j++] = 'a'; break; // ą
                case 0x87: buf[j++] = 'c'; break; // ć
                case 0x99: buf[j++] = 'e'; break; // ę
                case 0x82: buf[j++] = 'l'; break; // ł
                case 0x84: buf[j++] = 'n'; break; // ń
                case 0xB3: buf[j++] = 'o'; break; // ó
                case 0x9F: buf[j++] = 's'; break; // ś
                case 0xBC: buf[j++] = 'z'; break; // ź
                case 0xBF: buf[j++] = 'z'; break; // ż
                default: buf[j++] = '?'; break;
            }
            i += 2; // przeskoczemy oba bajty UTF-8
            continue;
        }

        // zwykłe ASCII
        buf[j++] = uppercase ? toupper(c) : c;
        i++;
    }
    buf[j] = '\0';
    return buf;
}

#endif
