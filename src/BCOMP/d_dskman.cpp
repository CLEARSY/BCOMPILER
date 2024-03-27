/***************************** CLEARSY **************************************
* Fichier : $Id: d_dskman.cpp,v 2.0 2001-11-26 10:00:05 cc Exp $
* (C) 2008 CLEARSY
*
* Description :	Gestionnaire de disque
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
****************************************************************************/
#include "c_port.h"
RCS_ID("$Id: d_dskman.cpp,v 1.17 2001-11-26 10:00:05 cc Exp $") ;

/* Includes systeme */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

/* Includes Composant Local */
#include "c_api.h"
#include "d_dskman.h"
#include "d_dskobj.h"
#include "d_dskstr.h"
#include "c_chain2.h"

const char* bom = "\xef\xbb\xbf";

//
//	}{ Magic numbers
//
// ATTENTION FORMAT OBLIGATOIRE "x999"<88yy>
// Avec y=numero majeur de version en ascii (31='1')
// et x=1 pour syntaxic, 2 pour semantic
static size_t disk_betree_magic_sit[] =
{
  // 00 FILE_SYNTAXIC_BETREE
  0x19998832,
  // 01 FILE_SEMANTIC_BETREE
  0x29998832,
  // 02 FILE_B0_CHECKED_BETREE
  0x39998832,
  // 03 FILE_DEP_BETREE
  0x49998832,
  // 04 FILE_COMPONENT_LIST
  0x12345679,
  // 05 FILE_COMPONENT_CHECKSUMS
  0x59998832,
  // 06 FILE_READ_JOKER
  0, // non significatif
} ;

size_t total_real_size_sop = 0 ;

size_t get_file_magic(T_betree_file_type betree_file_type)
{
TRACE1("get_file_magic(%d)",
	   betree_file_type) ;
TRACE2("get_file_magic(%d)->0x%08x",
	   betree_file_type, disk_betree_magic_sit[betree_file_type]) ;
return disk_betree_magic_sit[betree_file_type] ;
}

// Retrouver le file_type a partir du magic
T_betree_file_type magic2file_type(size_t magic)
{
  size_t i = 0 ;

  while (i <= (size_t)FILE_READ_JOKER)
	{
	  T_betree_file_type ftype = (T_betree_file_type)i ;
	  if (get_file_magic(ftype) == magic)
		{
		  return ftype ;
		}
	  i++ ;
	}

  assert(FALSE) ; // A FAIRE : internal error
  return FILE_READ_JOKER ; // pour le warning
}


//
//	}{	Classe T_disk_manager
//
T_disk_manager::T_disk_manager(FILE *new_fd,
										const char *new_file_name,
										T_betree_file_type new_betree_file_type,
										T_betree_file_mode new_betree_file_mode)
{
TRACE3("T_disk_manager::T_disk_manager(%x, %d, %d)",
	   new_fd, new_betree_file_type, new_betree_file_mode) ;

offset = 0 ;
total_real_size_sop = 0 ;
fd = new_fd ;
file_name = new_file_name ;
fdstream = new std::fstream(file_name, fstream::out);

if (new_betree_file_mode == FILE_WRITE_BETREE)
	{
	  TRACE0("->ecriture magic number") ;
      //write(get_file_magic(new_betree_file_type)) ;
      write_strNoCheck(bom);
	}
 else
   {
	 TRACE1("betree_file_mode %d is not yet implemented", new_betree_file_mode) ;
	 assert(FALSE) ;
	}
}

T_disk_manager::~T_disk_manager()
{
TRACE2("T_disk_manager::~T_disk_manager(%x) offset=%d", this, offset) ;

 int res_close =  fclose(fd);

 while (res_close == -1 && errno == EINTR)
   {
	 res_close =  fclose(fd);
   }

 if (res_close != 0)
   {
	 toplevel_error(FATAL_ERROR,
					get_error_msg(E_PROBLEM_CLOSING_FILE),
					file_name,
					strerror(errno)) ;
   }

}

void T_disk_manager::write(int integer)
{
#ifdef PERSIST_TRACE
TRACE3("T_disk_manager(%x)::write <integer> (%d) OFFSET %d",
	   this,
	   integer,
	   offset) ;
#endif

int size = sizeof(int) ;

// Transformation en format Network
integer = htonl(integer) ;

int real_size ;
if ((real_size = fwrite(&integer, size, 1, fd)) <= 0)
	{
	toplevel_error(FATAL_ERROR,
								get_error_msg(E_PROBLEM_WRITING_FILE),
								file_name,
								strerror(errno)) ;
	}

real_size *= sizeof(int) ;
assert(size == real_size) ;
offset += size ;
total_real_size_sop += real_size ;
assert(offset == total_real_size_sop) ;
}

void T_disk_manager::write(T_object *object)
{
#ifdef PERSIST_TRACE
TRACE1("T_disk_manager::write <object>(%x)", object) ;
#endif

 if (object == NULL)
	{
	write((int)0) ;
	return ;
	}

 size_t index = disk_object_get_object_index(object) ;
#ifdef PERSIST_TRACE
TRACE2("l'index de l'objet %x est %d --> appel de write(integer)", object, index) ;
#endif

write((int) index) ;
}

void T_disk_manager::write(const char *str)
{
#ifdef PERSIST_TRACE
TRACE1("T_disk_manager::write <char *>(%s)", (str == NULL) ? "NULL" : str) ;
#endif

if (str == NULL)
	{
#ifdef PERSIST_TRACE
	TRACE0("pointeur NULL : on appelle write <integer> 0") ;
#endif
	write((int)0) ;
	return ;
	}

size_t index = disk_string_get_string_index(str) ;

#ifdef PERSIST_TRACE
TRACE1("on ecrit l'index %d", index) ;
#endif
write((int)index) ; return ;
}

void T_disk_manager::write(void *adr)
{
#ifdef PERSIST_TRACE
TRACE1("T_disk_manager::write <void *>(%x) -> on sauve NULL", adr) ;
#endif

write((int)0) ;
return ;
// pour le warning
assert(adr) ;
}

void T_disk_manager::write_string(const char *str, size_t len)
{
#ifdef PERSIST_TRACE
TRACE3("T_disk_manager::write_string(%s::%d) OFFSET %d", str, len, offset) ;
#endif

size_t real_size ;
if ((real_size = fwrite(str, len + 1, 1, fd)) <= 0) // len + 1 car on met le '\0'
	{
	toplevel_error(FATAL_ERROR,
								get_error_msg(E_PROBLEM_WRITING_FILE),
								file_name,
								strerror(errno)) ;
	}

offset += (len + 1) ; // len+1 car on met le '\0'
real_size *= len + 1 ;
#ifdef PERSIST_TRACE
TRACE2("len + 1 %d real_size %d", len + 1, real_size) ;
#endif
assert((len + 1) == real_size) ;			// A FAIRE !!
total_real_size_sop += real_size ;
assert(offset == total_real_size_sop) ;		// A FAIRE !!!
#ifdef PERSIST_TRACE
TRACE0("fin de T_disk_manager::write <integer>") ;
#endif
}

void T_disk_manager::write_strNoCheck(const char *str)
{
#ifdef PERSIST_TRACE
TRACE3("T_disk_manager::write_strNoCheck(%s)", str) ;
#endif

if(strlen(str) > 0)
    *fdstream << str;

//size_t real_size ;
//if ((strlen(str) > 0) && ((real_size = fwrite(str, strlen(str), 1, fd)) <= 0))
//    {
//    toplevel_error(FATAL_ERROR,
//                                get_error_msg(E_PROBLEM_WRITING_FILE),
//                                file_name,
//                                strerror(errno)) ;
//    }

#ifdef PERSIST_TRACE
TRACE0("fin de T_disk_manager::write_strNoCheck") ;
#endif
}


void T_disk_manager::write_checksums(const T_component_checksums* chks)
{
    if(chks)
    {
        *fdstream << *chks;
    }
}
