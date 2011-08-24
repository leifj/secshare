/***************************************************************************
                          common.c  -  description
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

#include "sha1.h"


#include <sys/time.h>
#include <time.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

byte key[16];
byte hashv[16];

char* key_prints(char*s,byte*h)
{
        static const char hex[]="0123456789ABCDEF";
        static char sr[64];
        int i,j;
        if(!s)s=sr;

        *s=0;
        for(i=j=0;i<16;i++){
                s[j++]=hex[h[i]>>4];
                s[j++]=hex[h[i]&15];
                if(i&1 && i<17)s[j++]=' ';
        }
        s[j]=0;


        return s;
}


void genkey()
{
	int i;
        for(i=0;i<16;i++)key[i]=randbyte();
}

/*entropy pool*/
static byte*pool=0;
static int ppos=20;

/*This is a rather silly and simple random function.
  It should be good enough for IVs and session keys.

  Principle: initialize generator with some real near enough random.
  Then hash the random before using it: this makes it hard to calculate
  back into the original entropy pool.
*/

static void init_seth_rand()
{
        struct timeval tv;
        static byte pool_[20];
        pool=pool_;

        if(gettimeofday(&tv,NULL)){
                fprintf(stderr,"Unable to seed random generator. Oops, giving up.\n");
                exit(1);
        }
        srand(tv.tv_usec);
}

static void rand_refill()
{
        void*rh;
        int i,r;
        byte*b;

        if(pool==NULL)init_seth_rand();

        ppos=0;
        /*we assume each rand() call will give us 8 random bits (it probably
        has more, but you can't have too much random).*/
        rh=sha1_new();
        /*make the period longer by making the next block dependend on the current one*/
        sha1_write(rh,pool,20);
        /*add the random to the soup*/
        for(i=0;i<20;i++){
                r=rand();
                sha1_write(rh,(void*)&r,sizeof(r));
        }
        /*fill the pool*/
        b=sha1_get(rh);
        memcpy(pool,b,20);
        sha1_close(rh);
}

byte randbyte()
{
        if(ppos>=20)rand_refill();
        return pool[ppos++];
}
