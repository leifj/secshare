/***************************************************************************
                          encrypt.c  -  description
                             -------------------
    begin                : Fri Apr 9 2004
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
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>


int encrypt()
{
	byte*buf;

        void*enc,*hash;
        size_t tl=0,il;
	int i;
        byte*hbytes;

        /*get key*/
        genkey();
	printf("Using key: %s\n",key_prints(0,key));

        enc=aes128_new(key,ENCRYPT|CBC);

        hash=sha1_new();

	/*write data length*/
	tl=lseek(fdin,0,SEEK_END);
	lseek(fdin,0,SEEK_SET);

	buf=malloc(tl+36);

	buf[16]=(tl>>24)&0xff;
	buf[17]=(tl>>16)&0xff;
	buf[18]=(tl>>8)&0xff;
	buf[19]=tl&0xff;
        /*get IV*/
        for(i=0;i<16;i++)buf[i]=randbyte();
	printf("IV: %s\n",key_prints(0,buf));

        /*copy/encrypt data*/

        il=read(fdin,buf+20,tl);
        sha1_write(hash,buf,il+20);
	il=aes_encrypt2(enc,buf,tl+36);
	write(fdout,buf,il);

	hbytes=sha1_get(hash);

        printf("Written: %i (%i) bytes\n",il,tl);
	printf("SHA1-Checksum: %s\n",sha1_prints(0,hbytes));

	memcpy(hashv,hbytes,16);
	printf("Written Hash : %s\n",key_prints(0,hashv));

        sha1_close(hash);
        aes_close(enc);

        close(fdout);

        return 0;
}
