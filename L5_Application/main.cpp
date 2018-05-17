/*
 *     SocialLedge.com - Copyright (C) 2013
 *
 *     This file is part of free software framework for embedded processors.
 *     You can use it and/or distribute it as long as this copyright header
 *     remains unmodified.  The code is free for personal use and requires
 *     permission to use in a commercial product.
 *
 *      THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 *      OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 *      MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 *      I SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR
 *      CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 *     You can reach the author of this software at :
 *          p r e e t . w i k i @ g m a i l . c o m
 */

#include "tasks.hpp"
#include "FreeRTOS.h"
#include "queue.h"
#include "scheduler_task.hpp"

#include "MP3/UserInterface.hpp"
#include "MP3/MusicPlayer.hpp"


#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include "ff.h"

int main(void) {
    
    MusicPlayer::sharedInstance();

    UserInterface *ui = new UserInterface(PRIORITY_HIGH);

    scheduler_add_task(new terminalTask(PRIORITY_HIGH));
    scheduler_add_task(ui);
    scheduler_start();


    return 0;
}
