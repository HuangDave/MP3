/*
 * MP3File.hpp
 *
 *  Created on: May 18, 2018
 *      Author: huang
 */

#ifndef MP3FILE_HPP_
#define MP3FILE_HPP_

#include "LPC17xx.h"

class MP3File {

protected:

    typedef struct {
        char header[3];     // 3  bytes - should contain "TAG"
        char title[30];        // 30 bytes - 30 characters of title
        char artist[30];       // 30 bytes - 30 characters of artist name
        char album[30];        // 30 bytes - 30 characters of album name
        uint32_t year;       // 4  bytes - 4 digit year
        char comment[30];      // 28 or 30 bytes - Comment
        uint8_t zeroByte;   // 1  byte - 0 if the track number is stored
        uint8_t trackNum;   // 1  byte - Track number, is invalid if zeroByte is not 0
        uint8_t genre;      // 1  byte - Index of the track's genre (0-255)
    } ID3v1;

    char *mpPath;
    char *mpName;

    ID3v1 mMetaData;

    uint32_t mFileSize;

public:

    MP3File(char *path);
    ~MP3File();

    void fetchTag();
};

#endif /* MP3FILE_HPP_ */
