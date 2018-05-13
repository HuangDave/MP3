/*
 * SongMenu.cpp
 *
 *  Created on: May 10, 2018
 *      Author: huang
 */

#include <MP3/UI/SongMenu.hpp>

#include <stdlib.h>
#include <stdio.h>
#include "string.h"
#include <iostream>
#include "ff.h"

SongMenu::SongMenu(Frame frame) : UITableView(frame) {

    const uint8_t kMenuRowHeight = 10;
    const uint8_t kNumRows = 10;

    setDataSource((UITableViewDataSource *)this);
    //setDelegate((UITableViewDelegate *)this);
    setRowHeight(kMenuRowHeight);
    setNumberOfRows(kNumRows);

    fetchSongs();
}

SongMenu::~SongMenu() { }

void SongMenu::fetchSongs() {
    mSongList.empty();

    DIR directory;

    if (f_opendir(&directory, "1:") == FR_OK) {     // read SD Card directory
        static FILINFO fileInfo;

        while (f_readdir(&directory, &fileInfo) == FR_OK) {
            if (fileInfo.fname[0] == 0) break;

            const char *mp3[] = { ".mp3", ".MP3" };
            char *ext= strrchr(fileInfo.fname,'.');

            // only retreive names of mp3 files
            if (!(fileInfo.fattrib & AM_DIR) && (strcmp(ext, mp3[0]) || strcmp(ext, mp3[1]))) {
                SongInfo info;

                // TODO: get full filename
                // TODO: save a copy of filename without the .mp3 or .MP3 extension

                uint8_t len = strlen(fileInfo.fname);
                info.fmtName = new char[len];
                strcpy(info.fmtName, fileInfo.fname);

                len = fileInfo.lfsize;
                printf("lfsize: %d\n", len);
                info.fullName = new char[len];
                strcpy(info.fullName, fileInfo.lfname);

                for (uint8_t i = 0; i < len - 1; i++)
                    printf("%c", info.fullName[i]);
                printf("\n");

                mSongList.push_back(info);
            }
        }
    }
}

// UITableViewDataSource Implementation

uint32_t SongMenu::numberOfItems() const {
    printf("list: %d\n", mSongList.size());
    return mSongList.size();
}

void SongMenu::cellForIndex(UITableViewCell &cell, uint32_t index) {
    SongInfo info = mSongList[index];
    cell.setText(info.fmtName, strlen(info.fmtName));
}
