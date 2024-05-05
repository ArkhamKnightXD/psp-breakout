#include "exit_callback.h"

//These 3 functions are for setting up the exit button.
// Exit callback function
int exit_callback(int arg1, int arg2, void *common) {
    sceKernelExitGame();
    return 0;
}

// Callback thread function
int callback_thread(SceSize args, void *argp) {
    int cbid = sceKernelCreateCallback("Exit Callback", exit_callback, NULL);
    sceKernelRegisterExitCallback(cbid);
    sceKernelSleepThreadCB();
    return 0;
}

// Function to set up exit callbacks
int setup_callbacks(void) {
    int thid = sceKernelCreateThread("update_thread", callback_thread, 0x11, 0xFA0, 0, 0);
    if(thid >= 0)
        sceKernelStartThread(thid, 0, 0);
    return thid;
}
