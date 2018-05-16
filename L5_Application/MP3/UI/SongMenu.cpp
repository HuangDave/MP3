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

    setDataSource((UITableViewDataSource *) this);
    setRowHeight(kMenuRowHeight);
    setNumberOfRows(kNumRows);

    fetchSongs();
}

SongMenu::~SongMenu() { }

void SongMenu::fetchSongs() {
    mSongList.empty();

    const char dirPath[] = "1:";
    DIR directory;
    static FILINFO fileInfo;

#if _USE_LFN
    char lfnBuffer[_MAX_LFN];
    fileInfo.lfsize = _MAX_LFN-1;
    fileInfo.lfname = lfnBuffer;
#endif

    const char *mp3[] = { ".mp3", ".MP3" };
    const char *ext   = strrchr(fileInfo.fname,'.');

    FRESULT res = f_opendir(&directory, dirPath);

    if (res == FR_OK) {

        while (1) {
            fileInfo.lfsize = _MAX_LFN-1;
            fileInfo.lfname = lfnBuffer;

            res = f_readdir(&directory, &fileInfo);

            if (res != FR_OK || fileInfo.fname[0] == 0) break;

            if (!(fileInfo.fattrib & AM_DIR) && (strcmp(ext, mp3[0]) || strcmp(ext, mp3[1]))) {
                SongInfo song;

                const char *fullName = fileInfo.lfname[0] == 0 ? fileInfo.fname : fileInfo.lfname;

                // construct and save full file path by combining directory path and full file name...
                uint32_t len = strlen(dirPath) + strlen(fullName) + 1;
                char *path = new char[len];
                strcpy(path, dirPath);
                strcat(path, fullName);
                path[len-1] = '\0'; // set terminal char at the end of string
                song.path = path;

                // parse and save song name without extension...
                len = strlen(fullName) - strlen(mp3[0]) + 1;
                char *name = new char[len];
                strncpy(name, fullName, len);
                name[len-1] = '\0'; // set terminal char at the end of string
                song.name = name;

                song.fileSize = fileInfo.fsize;

                mSongList.push_back(song);
            }
        }
        f_closedir(&directory);
    }
}

SongInfo* SongMenu::songAt(uint32_t index) {
    return &(mSongList[index]);
}

// UITableViewDataSource Implementation

inline uint32_t SongMenu::numberOfItems() const {
    return mSongList.size();
}

inline void SongMenu::cellForIndex(UITableViewCell &cell, uint32_t index) {
    SongInfo info = mSongList.at(index);
    cell.setText(info.name, strlen(info.name));
}
