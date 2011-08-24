/***************************************************************************
                          decrypt.c  -  description
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

#include "secshare.h"
#include "rijndael.h"
#include "sha1.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

int decrypt()
{
	byte *buf;

        void*dec,*hash;
        size_t tl=0,il;
        byte*hbytes;

	printf("Using key: %s\n",key_prints(0,key));
	printf("Expected Hash: %s\n",key_prints(0,hashv));

        dec=aes128_new(key,DECRYPT|CBC);

        hash=sha1_new();

	tl=lseek(fdin,0,SEEK_END);
	lseek(fdin,0,SEEK_SET);

	buf=malloc(tl+36);
	if(tl<20){
        	fprintf(stderr,"Error: Premature end of file. Doesn't even have a header.\n");
                return 1;
        }
	il=read(fdin,buf,tl);
	if(il<tl){
		fprintf(stderr,"Error: unable to read file completely, got %i out of %i bytes.\n",il,tl);
		return 1;
	}
	aes_decrypt2(dec,buf,tl);
        aes_close(dec);

        /*get and discard IV*/
	printf("IV: %s\n",key_prints(0,buf));
	/*get file size*/
	tl=(buf[16]<<24) | (buf[17]<<16) | (buf[18]<<8) | buf[19];
	if(tl>(il-20)){
		fprintf(stderr,"Encoding Error: file shows size %i, but only %i available.\n",tl,il-20);
		return 1;
	}

        /*copy/decrypt data*/

        sha1_write(hash,buf,tl+20);
        write(fdout,buf+20,tl);

        /*get hash*/
        hbytes=sha1_get(hash);
        if(verbose)
                fprintf(stderr,"Checksum: %s.\n",sha1_prints(0,hbytes));
	if(memcmp(hashv,hbytes,16)){
		fprintf(stderr,"Hashes do not match, probably a faulty secret.\n");
	}
	sha1_close(hash);

        /*ignore remainder of file*/

        close(fdout);
        close(fdin);

        return 0;
}
