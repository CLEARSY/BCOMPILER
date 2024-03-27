/******************************* CLEARSY **************************************
* Fichier : $Id: d_dskstr.h,v 2.0 1997-07-08 07:44:57 sl Exp $
* (C) 2008 CLEARSY
*
* Description :		Compilateur B
*					Interface de la classe de base T_disk_string
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
#ifndef _C_DISK_STRING_H_
#define _C_DISK_STRING_H_

class T_disk_string
	{
	friend T_disk_string *find_string(const char *str) ;

	// Addresse RAM de la chaine
	char 		*address ;

	// Index alloue a l'objet
	size_t	index ;

	// Chainage
	T_disk_string	*next ;

	public :
	// Ajout d'un objet dans la table
	T_disk_string(char *string) ;
	T_disk_string(char *string, size_t len) ;

	// Methodes de lecture
	const void *get_address(void) { return address ; } ;
	size_t get_index(void) { return index ; } ;
	T_disk_string *get_next(void) { return next ; } ;

	// Autres methodes
	void dump_disk(void) ;
	} ;

extern void init_disk_string_write(T_disk_manager *disk_manager) ;
extern void init_disk_string_read(void) ;
extern void exit_disk_string(void) ;
extern size_t dump_disk_string(void) ;
extern void load_disk_string(void *start, size_t len) ;

extern char *disk_string_find_string(size_t index) ;

// Creation d'une nouvelle chaine
extern size_t disk_string_get_string_index(const char *adr) ;

#endif /* _C_DISK_STRING_H_ */



