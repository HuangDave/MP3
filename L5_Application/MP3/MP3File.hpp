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

    char *mpPath;
    char *mpName;
    uint32_t mFileSize;

    // ID3v1 Metadata of MP3File.

    char mHeader[4];     // 3  bytes - should contain "TAG"
    char mTitle[31];     // 30 bytes - 30 characters of title
    char mArtist[31];    // 30 bytes - 30 characters of artist name
    char mAlbum[31];     // 30 bytes - 30 characters of album name
    uint32_t mYear;      // 4  bytes - 4 digit year
    char mComment[31];   // 28 or 30 bytes - Comment
    uint8_t mGenre;      // 1  byte - Index of the track's genre (0-255)

public:

    MP3File(char *path, uint32_t fileSize);
    ~MP3File();

    /**
     * Fetch and update the song's info from SD Card.
     */
    void fetch();

    void setName(char *name);

    char* getPath();
    uint32_t getFileSize();
    char* getTitle();
    char* getArtist();
};

#endif /* MP3FILE_HPP_ */
