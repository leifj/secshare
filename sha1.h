/***************************************************************************
                          sha1.h  -  description
                             -------------------
    begin                : Sun Apr 11 2004
    copyright            : (C) 2004 by Konrad Rosenbaum
    email                : konrad@silmor.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

void sha1_write( void *context, byte *inbuf, size_t inlen);
byte*sha1_get(void*);

void sha1_hash_buffer (char *outbuf, const char *buffer, size_t length);

void*sha1_new();
void sha1_close(void*);

char* sha1_prints(char*,byte*);


