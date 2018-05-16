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

    //char lfnBuffer[_MAX_LFN];

    if (f_opendir(&directory, dirPath) == FR_OK) {     // read SD Card directory
        static FILINFO fileInfo;

        //fileInfo.lfname = lfnBuffer;
        //fileInfo.lfsize = _MAX_LFN - 1;

        while (f_readdir(&directory, &fileInfo) == FR_OK) {
            if (fileInfo.fname[0] == 0) break;

            const char *mp3[] = { ".mp3", ".MP3" };
            const char *ext   = strrchr(fileInfo.fname,'.');

            // only retreive names of mp3 files
            if (!(fileInfo.fattrib & AM_DIR) && (strcmp(ext, mp3[0]) || strcmp(ext, mp3[1]))) {
                SongInfo song;

                // TODO: get full filename

                const char *fullName = fileInfo.lfsize == 0 ? fileInfo.fname : fileInfo.lfname;

                printf("lfn[0]: %c\n", fileInfo.lfname[0]);
                printf("lfsize: %d\n", fileInfo.lfsize);

                // construct and save full file path by combining directory path and full file name...
                uint32_t len = strlen(dirPath) + strlen(fullName) + 1;
                char *path = new char[len];
                strcpy(path, dirPath);
                strcat(path, fullName);
                path[len-1] = '\0'; // set terminal char at the end of string
                song.path = path;

                // parse and save song name without extension...
                len = strlen(fullName) - strlen(ext) + 1;
                char *name = new char[len];
                strncpy(name, fullName, len);
                name[len-1] = '\0'; // set terminal char at the end of string
                song.name = name;

                // TODO: get file size

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
