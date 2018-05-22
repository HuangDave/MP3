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

MP3File::MP3File(char *path) {
    mpPath = path;
    mpName = NULL;
    mFileSize = 0;
}

MP3File::~MP3File() {
    mpPath = NULL;
}

void MP3File::fetchTag() {
    if (xSemaphoreTake(SPI::spiMutex[SPI::SSP1], portMAX_DELAY)) {

        const uint8_t id3Size = 128; // size of ID3v1 tag in bytes
        uint8_t data[id3Size];
        Storage::read(mpPath, data, id3Size, mFileSize - id3Size); // read the ID3v1 tag at the end of the file...

        const uint8_t titleOffset    = 3;
        const uint8_t artistOffset   = 33;
        const uint8_t albumOffset    = 63;
        const uint8_t yearOffset     = 93;
        const uint8_t commentOffset  = 97;
        const uint8_t zeroByteOffset = 125;
        const uint8_t trackNumOffset = 126;
        const uint8_t genreOffset    = 127;

        ID3v1 tag;
        strncpy(tag.header,  (char *)data, 3);
        strncpy(tag.title,   (char *)data + titleOffset, 30);
        strncpy(tag.artist,  (char *)data + artistOffset, 30);
        strncpy(tag.album,   (char *)data + albumOffset, 30);
        strncpy(tag.comment, (char *)data + commentOffset, 30);

        tag.year |= data[yearOffset]   << 24;
        tag.year |= data[yearOffset+1] << 16;
        tag.year |= data[yearOffset+2] << 8;
        tag.year |= data[yearOffset+3] << 0;

        tag.zeroByte = data[zeroByteOffset];
        tag.trackNum = data[trackNumOffset];
        tag.genre = data[genreOffset];

        mMetaData = tag;
    }
}
