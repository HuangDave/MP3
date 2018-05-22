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

inline void removeTrailingSpaces(char *str) {
    uint32_t idx = 0;
    uint32_t i = 0;
    while (str[i] != '\0' || i < strlen(str)) {
        if (str[i] != ' ') idx = i;
        i++;
    }
    str[idx+1] = '\0';
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

        for (uint8_t i = 0; i < 128; i++)
            printf("%x", data[i]);
        printf("\n");

        const uint8_t titleOffset    = 3;
        const uint8_t artistOffset   = 33;
        const uint8_t albumOffset    = 63;
        const uint8_t yearOffset     = 93;
        const uint8_t commentOffset  = 97;
        const uint8_t genreOffset    = 127;

        strncpy(header,  (char *)data, 3);
        header[3] = '\0';
        removeTrailingSpaces(header);
        strncpy(title,   (char *)data + titleOffset, 30);
        title[30] = '\0';
        removeTrailingSpaces(title);
        strncpy(artist,  (char *)data + artistOffset, 30);
        artist[30] = '\0';
        removeTrailingSpaces(artist);
        strncpy(album,   (char *)data + albumOffset, 30);
        album[30] = '\0';
        removeTrailingSpaces(album);
        strncpy(comment, (char *)data + commentOffset, 30);
        comment[30] = '\0';
        removeTrailingSpaces(comment);

        year |= data[yearOffset]   << 24;
        year |= data[yearOffset+1] << 16;
        year |= data[yearOffset+2] << 8;
        year |= data[yearOffset+3] << 0;

        genre = data[genreOffset];
    }
}
