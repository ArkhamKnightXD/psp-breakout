#ifndef EXIT_CALLBACK_H
#define EXIT_CALLBACK_H

#include <pspkernel.h>

// Exit callback function
int exit_callback(int arg1, int arg2, void *common);

// Callback thread function
int callback_thread(SceSize args, void *argp);

// Function to set up exit callbacks
int setup_callbacks(void);

#endif /* EXIT_CALLBACK_H */
