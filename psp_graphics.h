#ifndef PSP_GRAPHICS_H
#define PSP_GRAPHICS_H

#include <pspgu.h>
 
#define BUFFER_WIDTH 512
#define SCREEN_WIDTH 480
#define SCREEN_HEIGHT 272

static inline void initGu(char *list) {

    sceGuInit();

    sceGuStart(GU_DIRECT, list);
    sceGuDrawBuffer(GU_PSM_8888, (void *)0, BUFFER_WIDTH);
    sceGuDispBuffer(SCREEN_WIDTH, SCREEN_HEIGHT, (void *)0x88000, BUFFER_WIDTH);
    sceGuDepthBuffer((void *)0x110000, BUFFER_WIDTH);

    sceGuOffset(2048 - (SCREEN_WIDTH / 2), 2048 - (SCREEN_HEIGHT / 2));
    sceGuViewport(2048, 2048, SCREEN_WIDTH, SCREEN_HEIGHT);
    sceGuEnable(GU_SCISSOR_TEST);
    sceGuScissor(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

    sceGuDepthRange(65535, 0); 

    sceGuDepthFunc(GU_GEQUAL); 
    sceGuEnable(GU_DEPTH_TEST); 
    sceGuFinish();
    sceGuDisplay(GU_TRUE);
}

static inline void endGu()
{
    sceGuDisplay(GU_FALSE);
    sceGuTerm();
}

#endif /* PSP_GRAPHICS_H */
