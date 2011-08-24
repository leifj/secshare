/***************************************************************************
                          seth.h  -  description
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

#include <sys/types.h>  /* for byte and u32 typedefs */
typedef u_int8_t byte;
typedef u_int32_t u32;


/*#include "config.h"*/
#include "endianess.h"


/*Return values*/

#define SELFTEST_FAILED -1
#define INV_KEYLEN -2

/*Modi*/

#define ENCRYPT 0
#define DECRYPT 1

#define MASK_CRYPT 0xf

/*I've made CBC 0 so that you are more secure if you forget it. Use of ECB is
  discouraged. You may try it for experiments, but don't use it on production
  data.*/
#define ECB 0x10
#define CBC 0x00

#define MASK_MODE 0xf0

/*seth.c*/
extern int verbose;
extern int fdin;
extern int fdout;

/*encrypt.c*/
int secshare_encrypt();

/*decrypt.c*/
int secshare_decrypt();

/*common.c*/

void genkey();
byte randbyte();
char* key_prints(char*,byte*);

extern byte key[16];
extern byte hashv[16];

/*share.c*/

void sharecalc();
void share(char*);
void unshare(char*);
void sharevector(int);
void sharematrix(int);
