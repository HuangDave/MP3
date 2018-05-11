/*
 * MusicPlayer.cpp
 *
 *  Created on: May 7, 2018
 *      Author: huang
 */

#include <MP3/MusicPlayer.hpp>

#include <stdlib.h>
#include <stdio.h>
#include "string.h"
#include <iostream>
#include "ff.h"


std::vector<SongInfo> MusicPlayer::mSongList;

MusicPlayer::MusicPlayer() {
    mpDecodeTask = new DecodeTask(PRIORITY_MEDIUM);
    mStreamQueue = xQueueCreate(2, sizeof(uint8_t));
    mpCurrentSongName = NULL;

    fetchSongs();
}

MusicPlayer::~MusicPlayer() { }

DecodeTask* MusicPlayer::getDecodeTask() const { return mpDecodeTask; };

void MusicPlayer::fetchSongs() {
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
                info.name = new char[len];
                strcpy(info.name, fileInfo.fname);

                mSongList.push_back(info);
            }
        }
    }
}

void MusicPlayer::play(char *songName) {
    mDecoder.enablePlayback();

    mpCurrentSongName = songName;

    const char dirPrefix[] = "1:";
    char *fileName = (char *) malloc(strlen(dirPrefix) + strlen(mpCurrentSongName) - 1);
    strcpy(fileName, dirPrefix);
    strcat(fileName, mpCurrentSongName);

    FILE *f = fopen(fileName, "r"); // read current song in SD Card

    // get file size
    fseek(f, 0, SEEK_END);
    const uint32_t fileSize = ftell(f);
    fseek(f, 0, SEEK_SET);

    // queue 512 bytes for decoder
    for (uint32_t i = 0; i < fileSize / 512; i++) {
        if (mState & (STOPPED | CANCELLING)) break;

        uint8_t *data = new uint8_t[512];
        fread(data, 1, 512, f);
        xQueueSend(mStreamQueue, &data, portMAX_DELAY);
    }

    fclose(f);
}

void MusicPlayer::pause() {
    mDecoder.disablePlayback();
}

void MusicPlayer::incrementVolume() {

}
void MusicPlayer::decrementVolume() {

}
