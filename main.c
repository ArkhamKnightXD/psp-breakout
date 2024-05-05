#include <pspkernel.h>
#include <pspgu.h>
#include <pspdisplay.h>
#include <pspctrl.h>

//This 2 lines are necessary to setup psp module info.
PSP_MODULE_INFO("gutest", 0, 1, 0);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_VFPU | THREAD_ATTR_USER);

//The buffer is 512, because the buffer size needs to be the closest power of 2 to the psp resolution,
// and because has to be bigger than 480, is 512 instead of 256
#define BUFFER_WIDTH 512
#define SCREEN_WIDTH 480
#define SCREEN_HEIGHT 272

char list[0x20000] __attribute__((aligned(64)));

//boilerplate code for setting up the psp graphics.
void initGu()
{
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

void endGu()
{
    sceGuDisplay(GU_FALSE);
    sceGuTerm();
}

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

//end of setup graphics.

//These 3 functions are for setting up the exit button.
int exit_callback(int arg1, int arg2, void *common) {
    sceKernelExitGame();
    return 0;
}

int callback_thread(SceSize args, void *argp) {
    int cbid = sceKernelCreateCallback("Exit Callback", exit_callback, NULL);
    sceKernelRegisterExitCallback(cbid);
    sceKernelSleepThreadCB();
    return 0;
}

int setup_callbacks(void) {
    int thid = sceKernelCreateThread("update_thread", callback_thread, 0x11, 0xFA0, 0, 0);
    if(thid >= 0)
        sceKernelStartThread(thid, 0, 0);
    return thid;
}
//end exit button callbacks.

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

    vertices[1].x = x + w; // Adjusted x coordinate to include width
    vertices[1].y = y + h; // Adjusted y coordinate to include height

    sceGuColor(0xFFFFFFFF); // White, colors are ABGR
    sceGuDrawArray(GU_SPRITES, GU_TEXTURE_16BIT | GU_VERTEX_16BIT | GU_TRANSFORM_2D, 2, 0, vertices);
}

typedef struct
{
    int x, y;
    int w, h;
} Rectangle;

_Bool hasCollision(Rectangle player, Rectangle ball)
{

    return player.x < ball.x + ball.w && player.x + player.w > ball.x &&
           player.y < ball.y + ball.h && player.y + player.h > ball.y;
}

int main()
{
    setup_callbacks();

    initGu();

    Rectangle player1 = {8, SCREEN_HEIGHT / 2 - 48, 8, 48};
    Rectangle player2 = {SCREEN_WIDTH - 16, SCREEN_HEIGHT / 2 - 48, 8, 48};

    Rectangle ball = {SCREEN_WIDTH / 2 - 16, SCREEN_HEIGHT / 2 - 16, 16, 16};

    int playerSpeed = 6;

    int ballVelocityX = 4;
    int ballVelocityY = 4;

    SceCtrlData pad;

    while (1)
    {
        sceCtrlReadBufferPositive(&pad, 1);

        if (pad.Buttons & PSP_CTRL_UP && player1.y > 0)
            player1.y -= playerSpeed;

        else if (pad.Buttons & PSP_CTRL_DOWN && player1.y < SCREEN_HEIGHT - 48)
            player1.y += playerSpeed;

        if (pad.Buttons & PSP_CTRL_TRIANGLE && player2.y > 0)
            player2.y -= playerSpeed;

        else if (pad.Buttons & PSP_CTRL_CROSS && player2.y < SCREEN_HEIGHT - 48)
            player2.y += playerSpeed;

        if (ball.y < 0 || ball.y > SCREEN_HEIGHT - 16)
            ballVelocityY *= -1;
        
        if (ball.x > SCREEN_WIDTH + 16 || ball.x < -16)
        {
            ball.x = SCREEN_WIDTH / 2 - 16;
            ball.y = SCREEN_HEIGHT / 2 - 16;
        }

        if (hasCollision(player1, ball) || hasCollision(player2, ball))
            ballVelocityX *= -1;
        
        ball.x += ballVelocityX;
        ball.y += ballVelocityY;

        startFrame();

        drawRect(player1.x, player1.y, player1.w, player1.h); // Use xPos and yPos as the coordinates

        drawRect(ball.x, ball.y, ball.w, ball.h);

        drawRect(player2.x, player2.y, player2.w, player2.h);

        endFrame();

        sceKernelDelayThread(10000); // Delay for smoother animation, adjust as needed
    }

    endGu();

    return 0;
}
