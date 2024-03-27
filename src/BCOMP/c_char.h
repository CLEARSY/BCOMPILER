/******************************* CLEARSY **************************************
* Fichier : $Id: c_char.h,v 2.0 1997-07-08 07:44:48 sl Exp $
* (C) 2008 CLEARSY
*
* Description :		Compilateur B
*					Interface d'obtention de chaines temporaires
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
#ifndef _C_CHAR_H_
#define _C_CHAR_H_

// Obtention d'une chaine temporaire de taille "au moins size"
// La chaine est valide jusqu'au prochain appel de la fonction
extern char *get_tmp_string(size_t size) ;

// Obtention d'une chaine de 1Ko parmi un ensemble de 64 chaines
// Valide jusqu'au 64e prochain appel
extern char *get_1Ko_string(void) ;

#endif /* _C_CHAR_H_ */


