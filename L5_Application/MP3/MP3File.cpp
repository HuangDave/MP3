/*
 * MP3File.cpp
 *
 *  Created on: May 18, 2018
 *      Author: huang
 */

#include <MP3/MP3File.hpp>

#include <MP3/Drivers/SPI.hpp>
#include <stdio.h>
#include "string.h"
#include "ff.h"
#include "storage.hpp"

/**
 * Helper function to remove extra spaces from strings copied from ID3v1 tag.
 * @param str
 */
inline void removeTrailingSpaces(char *str) {
    uint32_t idx = 0;
    uint32_t i = 0;
    while (str[i] != '\0' || i < strlen(str)) {
        if (str[i] != ' ') idx = i;
        i++;
    }
    str[idx+1] = '\0';
}

/**
 * Helper function to copy a string read from a song's ID3v1 tag.
 *
 * @param dst
 * @param src
 * @param len
 */
inline void copyAndRemoveTailingSpaces(char *dst, const char *src, uint32_t len) {
    strncpy(dst,  (char *)src, len);
    dst[len] = '\0';
    removeTrailingSpaces(dst);
}

MP3File::MP3File(char *path, uint32_t fileSize) {
    mpPath = path;
    mpName = NULL;
    mFileSize = fileSize;
}

MP3File::~MP3File() {
    mpPath = NULL;
}

void MP3File::fetch() {
    if (xSemaphoreTake(SPI::spiMutex[SPI::SSP1], portMAX_DELAY)) {

        const uint8_t id3Size = 128; // size of ID3v1 tag in bytes
        uint8_t data[id3Size];

        Storage::read(mpPath, data, id3Size, mFileSize - id3Size); // read the ID3v1 tag at the end of the file...

        xSemaphoreGive(SPI::spiMutex[SPI::SSP1]);

        const uint8_t titleOffset    = 3;
        const uint8_t artistOffset   = 33;
        const uint8_t albumOffset    = 63;
        const uint8_t yearOffset     = 93;
        const uint8_t commentOffset  = 97;
        const uint8_t genreOffset    = 127;

        copyAndRemoveTailingSpaces(mHeader,  (char *)data, 3);
        copyAndRemoveTailingSpaces(mTitle,   (char *)data + titleOffset, 30);
        copyAndRemoveTailingSpaces(mArtist,  (char *)data + artistOffset, 30);
        copyAndRemoveTailingSpaces(mAlbum,   (char *)data + albumOffset, 30);
        copyAndRemoveTailingSpaces(mComment, (char *)data + commentOffset, 30);

        mYear |= data[yearOffset]   << 24;
        mYear |= data[yearOffset+1] << 16;
        mYear |= data[yearOffset+2] << 8;
        mYear |= data[yearOffset+3] << 0;

        mGenre = data[genreOffset];
    }
}

char *MP3File::getPath()        { return mpPath;    }
uint32_t MP3File::getFileSize() { return mFileSize; }
char* MP3File::getTitle()       { return mTitle;    }
char* MP3File::getArtist()      { return mArtist;   }
