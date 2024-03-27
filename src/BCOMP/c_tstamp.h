/******************************* CLEARSY **************************************
* Fichier : $Id: c_tstamp.h,v 2.0 1998-01-28 11:35:31 sl Exp $
* (C) 2008 CLEARSY
*
* Description :		Compilateur B
*					Interface du gestionnaire de timestamp
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
#ifndef _C_TSTAMP_H_
#define _C_TSTAMP_H_

// Obtention du timestamp courant
extern size_t get_timestamp(void) ;

// Forcer le timestamp courant
extern void set_timestamp(size_t new_timestamp) ;

// Passage a un nouveau timestamp
extern void next_timestamp(void) ;

// Obtention du timestamp2 courant
extern size_t get_timestamp2(void) ;

// Forcer le timestamp courant
extern void set_timestamp2(size_t new_timestamp) ;

// Passage a un nouveau timestamp2
extern void next_timestamp2(void) ;

#endif /* _C_TSTAMP_H_ */


