#include <pspkernel.h>
#include <pspgu.h>
#include <pspdisplay.h>
#include <pspctrl.h>
#include "exit_callback.h" 
#include "psp_graphics.h"

PSP_MODULE_INFO("gutest", 0, 1, 0);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_VFPU | THREAD_ATTR_USER);

#define BRICKS_SIZE 112

char list[0x20000] __attribute__((aligned(64)));

void startFrame()
{
    sceGuStart(GU_DIRECT, list);
    sceGuClearColor(0x00000000); 
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

void drawRect(float x, float y, float w, float h)
{
    Vertex *vertices = (Vertex *)sceGuGetMemory(2 * sizeof(Vertex));

    vertices[0].x = x;
    vertices[0].y = y;

    vertices[1].x = x + w;
    vertices[1].y = y + h;

    sceGuColor(0xFFFFFFFF); 
    sceGuDrawArray(GU_SPRITES, GU_TEXTURE_16BIT | GU_VERTEX_16BIT | GU_TRANSFORM_2D, 2, 0, vertices);
}

typedef struct
{
    int x, y;
    int w, h;
    _Bool isDestroyed;
} Rectangle;

_Bool hasCollision(Rectangle bounds, Rectangle ball)
{
    return bounds.x < ball.x + ball.w && bounds.x + bounds.w > ball.x &&
           bounds.y < ball.y + ball.h && bounds.y + bounds.h > ball.y;
}

int main()
{
    setup_callbacks();

    initGu(list);

    Rectangle bricks[BRICKS_SIZE];

    int positionX;
    int positionY = 20;

    int initialIndex = 0;
    int actualLenght = 14;

    for (int i = 0; i < 8; i++)
    {
        positionX = 2;

        for (int j = initialIndex; j < actualLenght; j++)
        {
            Rectangle actualBrick = {positionX, positionY, 32, 8, 0};

            bricks[j] = actualBrick;

            positionX += 34;
        }

        initialIndex += 14;
        actualLenght += 14;

        positionY += 10;
    }

    Rectangle player = {SCREEN_WIDTH / 2, SCREEN_HEIGHT - 16, 48, 8};

    Rectangle ball = {SCREEN_WIDTH / 2 - 12, SCREEN_HEIGHT / 2 - 12, 12, 12};

    int playerSpeed = 6;

    int ballVelocityX = 4;
    int ballVelocityY = 4;

    SceCtrlData pad;

    while (1)
    {
        sceCtrlReadBufferPositive(&pad, 1);

        if (pad.Buttons & PSP_CTRL_LEFT && player.x > 0)
            player.x -= playerSpeed;

        else if (pad.Buttons & PSP_CTRL_RIGHT && player.x < SCREEN_WIDTH - 48)
            player.x += playerSpeed;

        if (ball.y > SCREEN_HEIGHT + 12)
        {
            ball.x = SCREEN_WIDTH / 2 - 12;
            ball.y = SCREEN_HEIGHT / 2 - 12;

            ballVelocityX *= -1;
        }

        if (ball.y < 0)
            ballVelocityY *= -1;

        if (ball.x < 0 || ball.x > SCREEN_WIDTH - 12)
            ballVelocityX *= -1;

        if (hasCollision(player, ball))
            ballVelocityY *= -1;

        for (unsigned int i = 0; i < BRICKS_SIZE; i++)
        {
            if (!bricks[i].isDestroyed && hasCollision(bricks[i], ball))
            {
                ballVelocityY *= -1;
                bricks[i].isDestroyed = 1;
            }
        }

        ball.x += ballVelocityX;
        ball.y += ballVelocityY;

        startFrame();

        for (unsigned int i = 0; i < BRICKS_SIZE; i++)
        {
            if (!bricks[i].isDestroyed)
                drawRect(bricks[i].x, bricks[i].y, bricks[i].w, bricks[i].h); 
        }

        drawRect(player.x, player.y, player.w, player.h);

        drawRect(ball.x, ball.y, ball.w, ball.h);

        endFrame();

        sceKernelDelayThread(10000);
    }

    endGu();
    return 0;
}
