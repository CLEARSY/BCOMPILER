/******************************* CLEARSY **************************************
* Fichier : $Id: c_md5.h,v 2.0 1997-09-04 13:30:18 sl Exp $
* (C) 2008 CLEARSY
*
* Description :		Compilateur B
*					Calcul de cle MD5
*
This file is part of B_COMPILER
    Copyright (C) 2008 ClearSy (contact@clearsy.com)

    B_COMPILER is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; GNU General Public License version 3
    of the License

    B_COMPILER is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with B_COMPILER; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
******************************************************************************/
#ifndef MD5_H
#define MD5_H
#include <stdint.h>

typedef uint32_t uint32;

struct MD5Context {
	uint32 buf[4];
	uint32 bits[2];
	unsigned char in[64];
};

extern void MD5Init(struct MD5Context *context);
extern void MD5Update(struct MD5Context *context, unsigned char const *buf,
	       unsigned len);
extern void MD5Final(unsigned char digest[16], struct MD5Context *context);
extern void MD5Transform(uint32 buf[4], uint32 const in[16]);

/*
 * This is needed to make RSAREF happy on some MS-DOS compilers.
 */
typedef struct MD5Context MD5_CTX;

#endif /* !MD5_H */
