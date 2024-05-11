#include "psp_graphics.h"

char list[0x20000] __attribute__((aligned(64)));

void initGu() { 
    
    sceGuInit();

    // Set up buffers
    sceGuStart(GU_DIRECT, list);
    sceGuDrawBuffer(GU_PSM_8888, (void *)0, BUFFER_WIDTH);
    sceGuDispBuffer(SCREEN_WIDTH, SCREEN_HEIGHT, (void *)0x88000, BUFFER_WIDTH);
    sceGuDepthBuffer((void *)0x110000, BUFFER_WIDTH);

    // Set up viewport
    sceGuOffset(2048 - (SCREEN_WIDTH / 2), 2048 - (SCREEN_HEIGHT / 2));
    sceGuViewport(2048, 2048, SCREEN_WIDTH, SCREEN_HEIGHT);
    sceGuEnable(GU_SCISSOR_TEST);
    sceGuScissor(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

    // Set some stuff
    sceGuDepthRange(65535, 0); // Use the full buffer for depth testing - buffer is reversed order 

    sceGuDepthFunc(GU_GEQUAL);  // Depth buffer is reversed, so GEQUAL instead of LEQUAL
    sceGuEnable(GU_DEPTH_TEST); // Enable depth testing

    sceGuFinish();
    sceGuDisplay(GU_TRUE);
}

void endGu() {
    sceGuDisplay(GU_FALSE);
    sceGuTerm();
}

//(startFrame-endFrame) Together, these functions provide the necessary setup and synchronization for rendering frames on the PSP using the PSP graphics library (libgu).
// They are typically called at the beginning and end of each frame rendering cycle in a game or graphics application. 
void startFrame()
{
    sceGuStart(GU_DIRECT, list);
    sceGuClearColor(0x00000000); // Black background 
    sceGuClear(GU_COLOR_BUFFER_BIT);
}

void endFrame()
{
    sceGuFinish();
    sceGuSync(0, 0);
    sceDisplayWaitVblankStart();
    sceGuSwapBuffers();
}

typedef struct
{
    unsigned short u, v;
    short x, y, z;
} Vertex;

void drawRect(float x, float y, float w, float h, unsigned int color)
{
    Vertex *vertices = (Vertex *)sceGuGetMemory(2 * sizeof(Vertex));

    vertices[0].x = x;
    vertices[0].y = y;

    vertices[1].x = x + w;
    vertices[1].y = y + h;

    sceGuColor(color); 
    sceGuDrawArray(GU_SPRITES, GU_TEXTURE_16BIT | GU_VERTEX_16BIT | GU_TRANSFORM_2D, 2, 0, vertices);
}
