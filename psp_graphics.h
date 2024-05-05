#ifndef PSP_GRAPHICS_H
#define PSP_GRAPHICS_H

//The buffer is 512, because the buffer size needs to be the closest power of 2 to the psp resolution,
// and because has to be bigger than 480, is 512 instead of 256  
#define BUFFER_WIDTH 512
#define SCREEN_WIDTH 480
#define SCREEN_HEIGHT 272

#include <pspgu.h>
#include <pspdisplay.h>

//boilerplate code for setting up the psp graphics.
void initGu(char *list);

// closing psp graphics.
void endGu();

#endif /* PSP_GRAPHICS_H */
