#pragma once
// Moonmachine native-controller extension, version 1. Fixed-size wire format.
// Carried ONLY by the existing authenticated streaming control/input connection.
#include <stdint.h>
#include <stddef.h>
#define LI_NATIVE_CONTROLLER_VERSION 1
#define LI_NATIVE_CONTROLLER_SIZE 80
#define LI_NATIVE_CONTROLLER_INPUT_MAGIC 0x55000020u
#define LI_NATIVE_CONTROLLER_CONTROL_TYPE 0x55f0u
#define LI_NATIVE_ATTACH 1
#define LI_NATIVE_DETACH 2
#define LI_NATIVE_INPUT 3
#define LI_NATIVE_REPLY 4
#define LI_NATIVE_ACCEPT 5
#define LI_NATIVE_REQUEST 6
#define LI_NATIVE_ERROR 7
// [0] version, [1] opcode, [2] status (0 success, 1 failure), [3] operation
// [4] payload length, [5..7] zero, [8..15] LE request ID, [16..79] payload.
// Version 1 supports one Valve 28de:1304 revision 0002 per stream, not arbitrary USB.
static inline uint64_t LiNativeRequestId(const uint8_t* p) {
    uint64_t id = 0; unsigned i;
    for (i = 0; i < 8; i++) id |= (uint64_t)p[8+i] << (8*i);
    return id;
}
static inline void LiNativeSetRequestId(uint8_t* p, uint64_t id) {
    unsigned i; for (i = 0; i < 8; i++) p[8+i] = (uint8_t)(id >> (8*i));
}
static inline unsigned LiNativeInputLength(unsigned id) {
    switch(id) {case 0x42:return 54;case 0x45:return 46;case 0x43:return 15;
    case 0x44:return 6;case 0x79:return 2;case 0x7b:return 13;default:return 0;}
}
static inline int LiNativeValidate(const uint8_t* p, size_t size, int fromHost) {
    unsigned i; uint64_t id;
    if (!p || size != LI_NATIVE_CONTROLLER_SIZE || p[0] != 1 || p[2] > 1 ||
        p[4] > 64 || p[5] || p[6] || p[7]) return 0;
    for(i = 16+p[4]; i < size; i++) if(p[i]) return 0;
    id=LiNativeRequestId(p);
    if(fromHost) {
        if(p[1] == LI_NATIVE_ACCEPT || p[1] == LI_NATIVE_ERROR)
            return !p[3] && !p[4] && !id && (p[2] == (p[1] == LI_NATIVE_ERROR));
        if(p[1] != LI_NATIVE_REQUEST || p[2] || !id || p[3]<1 || p[3]>3) return 0;
        if(p[3] <= 2) return p[4]==64 && (p[16]==1 || p[16]==2);
        return p[4]>0; // Client enforces the device-specific output allowlist.
    }
    switch(p[1]) {
    case LI_NATIVE_ATTACH:case LI_NATIVE_DETACH:
        return !p[2] && !p[3] && !p[4] && !id;
    case LI_NATIVE_INPUT:
        return !p[2] && !p[3] && !id && p[4] && p[4]==LiNativeInputLength(p[16]);
    case LI_NATIVE_REPLY:
        return !p[3] && id && (!p[2] || !p[4]) && (p[4]==0 || p[4]==64);
    default:return 0;
    }
}
