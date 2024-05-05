#include <pspkernel.h>
#include <pspgu.h>
#include <pspdisplay.h>
#include <pspctrl.h>

PSP_MODULE_INFO("gutest", 0, 1, 0);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_VFPU | THREAD_ATTR_USER);

#define BUFFER_WIDTH 512
#define SCREEN_WIDTH 480
#define SCREEN_HEIGHT 272

char list[0x20000] __attribute__((aligned(64)));

void initGu()
{
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

void endGu()
{
    sceGuDisplay(GU_FALSE);
    sceGuTerm();
}

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

int exit_callback(int arg1, int arg2, void *common)
{
    sceKernelExitGame();
    return 0;
}

int callback_thread(SceSize args, void *argp)
{
    int cbid = sceKernelCreateCallback("Exit Callback", exit_callback, NULL);
    sceKernelRegisterExitCallback(cbid);
    sceKernelSleepThreadCB();
    return 0;
}

int setup_callbacks(void)
{
    int thid = sceKernelCreateThread("update_thread", callback_thread, 0x11, 0xFA0, 0, 0);
    if (thid >= 0)
        sceKernelStartThread(thid, 0, 0);
    return thid;
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

    initGu();

    Rectangle bricks[72];

    int positionX = 8;
    int positionY = 20;

    int initialIndex = 0;
    int actualLenght = 9;

    for (int i = 0; i < 8; i++)
    {
        positionX = 8;

        for (int j = initialIndex; j < actualLenght; j++)
        {
            Rectangle actualBrick = {positionX, positionY, 48, 8, 0};

            bricks[j] = actualBrick;

            positionX += 52;
        }

        initialIndex += 9;
        actualLenght += 9;

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

        for (unsigned int i = 0; i < 72; i++)
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

        for (unsigned int i = 0; i < 72; i++)
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
