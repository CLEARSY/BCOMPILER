/******************************* CLEARSY **************************************
* Fichier : $Id: d_dskman.h,v2.0 1998-11-27 10:03:10 sl Exp $
* (C) 2008 CLEARSY
*
* Description :		Compilateur B
*					Interface de la classe de base T_disk_manager
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
#ifndef _C_DISK_MANAGER_H_
#define _C_DISK_MANAGER_H_

#include <fstream>
#include "c_betree.h"

//
//	}{	Gestionnaire disque
//
class T_disk_manager
{
  std::fstream      *fdstream;
  FILE				*fd ;
  size_t			 offset ;
  const char		*file_name ;

public :
  T_disk_manager(FILE *new_fd,
				 const char *new_file_name,
				 T_betree_file_type new_betree_file_type,
				 T_betree_file_mode new_betree_file_mode) ;

  virtual ~T_disk_manager() ;

  void write_strNoCheck(const char *str) ;
  void write_checksums(const T_component_checksums* chks) ;
  void write(int integer) ;
  void write(T_object *object) ;
  void write(const char *str) ;
  void write(void *adr) ;
  void write_string(const char *str, size_t len) ;

  size_t get_offset(void) { return offset ; } ;
} ;

// Obtenir le magic d'un file_type donne
extern size_t get_file_magic(T_betree_file_type betree_file_type) ;

// Retrouver le file_type a partir du magic
extern T_betree_file_type magic2file_type(size_t magic) ;

#endif /* _C_DISK_MANAGER_H_ */



