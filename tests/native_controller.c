#include "../src/NativeController.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>
int main(void) {
    uint8_t p[80]={1,LI_NATIVE_ATTACH};
    unsigned i,j; uint32_t random=19;
    assert(LiNativeValidate(p,80,0)); assert(!LiNativeValidate(p,80,1));
    for(i=0;i<80;i++)assert(!LiNativeValidate(p,i,0));
    assert(!LiNativeValidate(NULL,80,0));
    p[1]=LI_NATIVE_INPUT;p[4]=54;p[16]=0x42;
    assert(LiNativeValidate(p,80,0));
    p[4]=53;assert(!LiNativeValidate(p,80,0));p[4]=54;
    p[79]=1;assert(!LiNativeValidate(p,80,0));p[79]=0;
    memset(p,0,80);p[0]=1;p[1]=LI_NATIVE_REQUEST;p[3]=1;p[4]=64;p[16]=1;
    assert(!LiNativeValidate(p,80,1));LiNativeSetRequestId(p,UINT64_C(0xfedcba9876543210));
    assert(LiNativeRequestId(p)==UINT64_C(0xfedcba9876543210));assert(LiNativeValidate(p,80,1));
    assert(!LiNativeValidate(p,80,0));
    p[3]=4;assert(!LiNativeValidate(p,80,1));
    memset(p,0,80);p[0]=1;p[1]=LI_NATIVE_REPLY;p[2]=1;LiNativeSetRequestId(p,1);
    assert(LiNativeValidate(p,80,0));p[4]=64;assert(!LiNativeValidate(p,80,0));
    for(i=0;i<100000;i++) {
        for(j=0;j<80;j++){random=random*1664525+1013904223;p[j]=(uint8_t)(random>>24);}
        (void)LiNativeValidate(p,i%81,0);(void)LiNativeValidate(p,i%81,1);
    }
    puts("Native-controller bounds, direction, request ID and malformed-packet tests passed");
}
