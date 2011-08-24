/***************************************************************************
                          rijndael.h  -  description
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


/* CBC: CT2 = E(PT2 xor CT1); PT2 = D(CT2) xor CT1; */

/*create a new context for AES 128bit */
void* aes128_new(const void*key,int mode);

/*store data into buffer, returns amount written*/
int aes_write(void*ctx,void*in,int len);

/*read data from AES engine - encrypted version of the data in the buffer*/
int aes_encrypt(void*ctx,void*out,int len);
/*read data from AES engine - decrypted version of the data in the buffer*/
int aes_decrypt(void*ctx,void*out,int len);

/*read data from AES engine - encrypted version of the data in the buffer*/
int aes_encrypt2(void*ctx,byte*out,int len);
/*read data from AES engine - decrypted version of the data in the buffer*/
int aes_decrypt2(void*ctx,byte*out,int len);

/*if there is still data in the buffer: pad with zeroes, en/de-crypt and write
to out; you may set out to NULL to discard that block*/
int aes_pad(void*ctx);

/*return how much we had to pad*/
int aes_mod(void*ctx);

void aes_close(void*ctx);
