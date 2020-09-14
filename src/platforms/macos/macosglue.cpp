// This file contains all the C functions needed by the Wireguard swift code.

#include <stdlib.h>
#include <string.h>

#ifndef MACOS_EXTENSION
#include <QDebug>
#else
#include <stdio.h>
#endif

// Key base64/hex functions
// ------------------------

#define WG_KEY_LEN (32)
#define WG_KEY_LEN_BASE64 (45)
#define WG_KEY_LEN_HEX (65)

#define EXPORT __attribute__((visibility("default")))

extern "C" {
EXPORT void key_to_base64(char base64[WG_KEY_LEN_BASE64], const uint8_t key[WG_KEY_LEN]);
EXPORT bool key_from_base64(uint8_t key[WG_KEY_LEN], const char *base64);

EXPORT void key_to_hex(char hex[WG_KEY_LEN_HEX], const uint8_t key[WG_KEY_LEN]);
EXPORT bool key_from_hex(uint8_t key[WG_KEY_LEN], const char *hex);

EXPORT void write_msg_to_log(const char *tag, const char *msg);
}

EXPORT void key_to_base64(char base64[WG_KEY_LEN_BASE64], const uint8_t key[WG_KEY_LEN])
{
    const char range[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    const char padchar = '=';
    int padlen = 0;

    char *out = base64;
    const uint8_t *in = key;

    for (int i = 0; i < WG_KEY_LEN;) {
        int chunk = 0;
        chunk |= int(in[i++]) << 16;
        if (i == WG_KEY_LEN) {
            padlen = 2;
        } else {
            chunk |= int(in[i++]) << 8;
            if (i == WG_KEY_LEN) {
                padlen = 1;
            } else {
                chunk |= int(in[i++]);
            }
        }

        int j = (chunk & 0x00fc0000) >> 18;
        int k = (chunk & 0x0003f000) >> 12;
        int l = (chunk & 0x00000fc0) >> 6;
        int m = (chunk & 0x0000003f);

        *out++ = range[j];
        *out++ = range[k];

        if (padlen > 1) {
            *out++ = padchar;
        } else {
            *out++ = range[l];
        }
        if (padlen > 0) {
            *out++ = padchar;
        } else {
            *out++ = range[m];
        }
    }

    base64[WG_KEY_LEN_BASE64 - 1] = 0;
}

EXPORT bool key_from_base64(uint8_t key[WG_KEY_LEN], const char *base64)
{
    if (strlen(base64) != WG_KEY_LEN_BASE64 - 1 || base64[WG_KEY_LEN_BASE64 - 2] != '=') {
        return false;
    }

    unsigned int buf = 0;
    int nbits = 0;
    uint8_t *out = key;
    int offset = 0;
    for (int i = 0; i < WG_KEY_LEN_BASE64; ++i) {
        int ch = base64[i];
        int d;

        if (ch >= 'A' && ch <= 'Z') {
            d = ch - 'A';
        } else if (ch >= 'a' && ch <= 'z') {
            d = ch - 'a' + 26;
        } else if (ch >= '0' && ch <= '9') {
            d = ch - '0' + 52;
        } else if (ch == '+') {
            d = 62;
        } else if (ch == '/') {
            d = 63;
        } else {
            d = -1;
        }

        if (d != -1) {
            buf = (buf << 6) | d;
            nbits += 6;
            if (nbits >= 8) {
                nbits -= 8;
                out[offset++] = buf >> nbits;
                buf &= (1 << nbits) - 1;
            }
        }
    }

    return true;
}

inline char toHex(uint8_t value)
{
    return "0123456789abcdef"[value & 0xF];
}

inline int fromHex(uint8_t c)
{
    return ((c >= '0') && (c <= '9'))
               ? int(c - '0')
               : ((c >= 'A') && (c <= 'F')) ? int(c - 'A' + 10)
                                            : ((c >= 'a') && (c <= 'f')) ? int(c - 'a' + 10) : -1;
}

EXPORT void key_to_hex(char hex[WG_KEY_LEN_HEX], const uint8_t key[WG_KEY_LEN])
{
    char *hexData = hex;
    const unsigned char *data = (const unsigned char *) key;
    for (int i = 0, o = 0; i < WG_KEY_LEN; ++i) {
        hexData[o++] = toHex(data[i] >> 4);
        hexData[o++] = toHex(data[i] & 0xf);
    }

    hex[WG_KEY_LEN_HEX - 1] = 0;
}

EXPORT bool key_from_hex(uint8_t key[WG_KEY_LEN], const char *hex)
{
    if (strlen(hex) != WG_KEY_LEN_HEX - 1) {
        return false;
    }

    bool odd_digit = true;
    unsigned char *result = (unsigned char *) key + WG_KEY_LEN;
    for (int i = WG_KEY_LEN_HEX - 1; i >= 0; --i) {
        int tmp = fromHex((unsigned char)(hex[i]));
        if (tmp == -1) {
            continue;
        }

        if (odd_digit) {
            --result;
            *result = tmp;
            odd_digit = false;
        } else {
            *result |= tmp << 4;
            odd_digit = true;
        }
    }

    return true;
}

// Logging functions
// -----------------

EXPORT void write_msg_to_log(const char *tag, const char *msg)
{
#ifndef MACOS_EXTENSION
    qDebug() << "Swift log - tag:" << tag << "msg: " << msg;
#else
    fprintf(stderr, "tag: %s - msg: %s\n", tag, msg);
#endif
}