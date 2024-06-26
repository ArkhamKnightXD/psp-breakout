#include <pspctrl.h>
#include "exit_callback.h" 
#include "psp_graphics.h"

PSP_MODULE_INFO("BREAKOUT", 0, 1, 0);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_VFPU | THREAD_ATTR_USER);

const unsigned int BRICKS_SIZE = 112; 

typedef struct
{
    int x, y;
    int w, h;
    _Bool isDestroyed;
    unsigned int color; 
} Rectangle;

_Bool hasCollision(Rectangle bounds, Rectangle ball)
{
    return bounds.x < ball.x + ball.w && bounds.x + bounds.w > ball.x &&
           bounds.y < ball.y + ball.h && bounds.y + bounds.h > ball.y;
}

int main()
{
    setup_callbacks();

    initGu();

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
            //blue color
            unsigned int color = 0xFFFFFF00;

            //red color
            if (i % 2 == 0)
                color = 0xFF0000FF;

            Rectangle actualBrick = {positionX, positionY, 32, 8, 0, color};

            bricks[j] = actualBrick;

            positionX += 34;
        }

        initialIndex += 14;
        actualLenght += 14;

        positionY += 10;
    }

    Rectangle player = {SCREEN_WIDTH / 2, SCREEN_HEIGHT - 16, 42, 8};

    Rectangle ball = {SCREEN_WIDTH / 2 - 12, SCREEN_HEIGHT / 2 - 12, 12, 12};

    int playerSpeed = 6;

    int ballVelocityX = 3;
    int ballVelocityY = 3;

    SceCtrlData pad;

    while (1)
    {
        sceCtrlReadBufferPositive(&pad, 1);

        if (pad.Buttons & PSP_CTRL_LEFT && player.x > 0)
            player.x -= playerSpeed;

        else if (pad.Buttons & PSP_CTRL_RIGHT && player.x < SCREEN_WIDTH - player.w)
            player.x += playerSpeed;

        if (ball.y > SCREEN_HEIGHT + 12)
        {
            ball.x = SCREEN_WIDTH / 2 - 12;
            ball.y = SCREEN_HEIGHT / 2 - 12;

            ballVelocityX *= -1;
        }

        if (ball.x < 0 || ball.x > SCREEN_WIDTH - 12)
            ballVelocityX *= -1;

        if (hasCollision(player, ball) || ball.y < 0)
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
                drawRect(bricks[i].x, bricks[i].y, bricks[i].w, bricks[i].h, bricks[i].color); 
        }

        drawRect(player.x, player.y, player.w, player.h, 0xFFFFFFFF);

        drawRect(ball.x, ball.y, ball.w, ball.h, 0xFFFFFFFF);

        endFrame();
    }

    endGu();
    return 0;
}
