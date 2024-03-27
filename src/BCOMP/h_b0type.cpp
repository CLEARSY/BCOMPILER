/******************************* CLEARSY **************************************
* Fichier : $Id: h_b0type.cpp,v 2.0 1999-07-22 15:17:39 sl Exp $
* (C) 2008 CLEARSY
*
* Description :		Compilateur B
*					Dump HTML des types B0
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
#include "c_port.h"
RCS_ID("$Id: h_b0type.cpp,v 1.4 1999-07-22 15:17:39 sl Exp $") ;

/* Includes systeme */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

/* Includes Composant Local */
#include "c_api.h"

FILE *T_B0_type::dump_html(void)
{
  TRACE1("T_B0_type(%x)::dump_html", this) ;

  FILE *fd = T_item::dump_html() ;

  s_fprintf(fd, "<H2>Fields from class Class T_B0_type :</H2>") ;

  s_fprintf(fd, "<UL><LI>B_type = %s</LI>\n", ptr_ref(B_type)) ;
  s_fprintf(fd, "<LI>is_a_type_definition = %s</LI></UL>\n",
			bool_ref(is_a_type_definition)) ;

  return fd ;
}

//
//	}{	Classe T_B0_base_type : methode de dump HTML
//
static const char *debug_base_type_sct[] =
{
  /* 00 '='		*/	"BTYPE_INTEGER",
					/* 01 '/='		*/	"BTYPE_BOOL",
					/* 02 '<'		*/	"BTYPE_STRING",
} ;

FILE *T_B0_base_type::dump_html(void)
{
  TRACE1("T_B0_base_type(%x)::dump_html", this) ;

  FILE *fd = T_B0_type::dump_html() ;

  s_fprintf(fd, "<H2>Fields from class T_B0_base_type :</H2>\n<UL>") ;

  size_t table_size = sizeof(debug_base_type_sct) / sizeof(char *) ;

  if ( (type  < 0) || ((size_t)type >= table_size) )
	{
	  s_fprintf(fd,
				"<LI>type = %d IS OUT OF BOUNDS (0 .. %d)\n",
				type,
				table_size - 1) ;

	  toplevel_error(CAN_CONTINUE,
					 "type = %d is out of bounds (0 .. %d)\n",
					 type,
					 table_size - 1) ;

	}
  else
	{
	  s_fprintf(fd, "<LI>type = %s</LI>\n", debug_base_type_sct[type]) ;
	}

  s_fprintf(fd, "<LI>name = %s</LI>\n", ptr_ref(name)) ;
  s_fprintf(fd, "<LI>is_bound = %s</LI>\n", bool_ref(is_bound)) ;
  s_fprintf(fd, "<LI>min = %s</LI>\n", ptr_ref(min)) ;
  s_fprintf(fd, "<LI>max = %s</LI>\n", ptr_ref(max)) ;
  s_fprintf(fd, "<LI>ref_type = %s</LI></UL>\n", ptr_ref(ref_type)) ;



  // Fermeture du fichier HTML
  close_html(fd) ;

  return NULL ;

}

//
//	}{	Classe T_B0_array_type : methode de dump HTML
//
FILE *T_B0_array_type::dump_html(void)
{
  TRACE1("T_B0_array_type(%x)::dump_html", this) ;

  FILE *fd = T_B0_type::dump_html() ;

  s_fprintf(fd, "<H2>Fields from class T_B0_array_type :</H2>\n<UL>") ;

  s_fprintf(fd, "<LI>first_src_type = %s</LI>\n", ptr_ref(first_src_type)) ;
  s_fprintf(fd, "<LI>last_src_type = %s</LI>\n", ptr_ref(last_src_type)) ;
  s_fprintf(fd, "<LI>dst_type = %s</LI>\n", ptr_ref(dst_type)) ;
  s_fprintf(fd, "<LI>ref_machine = %s</LI>\n", ptr_ref(ref_machine)) ;
  s_fprintf(fd, "<LI>name = %s</LI>\n", sym_ref(name)) ;
  s_fprintf(fd, "<LI>next_decla = %s</LI>\n", ptr_ref(next_decla)) ;
  s_fprintf(fd, "<LI>prev_decla = %s</LI></UL>\n", ptr_ref(prev_decla)) ;

  // Fermeture du fichier HTML
  close_html(fd) ;

  return NULL ;

}

//
//	}{	Classe T_B0_interval_type : methode de dump HTML
//
FILE *T_B0_interval_type::dump_html(void)
{
  TRACE1("T_B0_interval_type(%x)::dump_html", this) ;

  FILE *fd = T_B0_type::dump_html() ;

  s_fprintf(fd, "<H2>Fields from class T_B0_interval_type :</H2>\n<UL>") ;

  s_fprintf(fd, "<LI>prev_itype = %s</LI>\n", ptr_ref(prev_itype)) ;
  s_fprintf(fd, "<LI>next_itype = %s</LI>\n", ptr_ref(next_itype)) ;
  s_fprintf(fd, "<LI>name = %s</LI>\n", ptr_ref(name)) ;
  s_fprintf(fd, "<LI>min = %s</LI>\n", ptr_ref(min)) ;
  s_fprintf(fd, "<LI>min_bound = %s</LI>\n", ptr_ref(min_bound)) ;
  s_fprintf(fd, "<LI>max = %s</LI>\n", ptr_ref(max)) ;
  s_fprintf(fd, "<LI>max_bound = %s</LI></UL>\n", ptr_ref(max_bound)) ;
  s_fprintf(fd, "<LI>abstract_type = %s</LI></UL>\n", ptr_ref(abstract_type)) ;

  // Fermeture du fichier HTML
  close_html(fd) ;

  return NULL ;

}

//
//	}{	Classe T_B0_enumerated_type : methode de dump HTML
//
FILE *T_B0_enumerated_type::dump_html(void)
{
  TRACE1("T_B0_enumerated_type(%x)::dump_html", this) ;

  FILE *fd = T_B0_type::dump_html() ;

  s_fprintf(fd, "<H2>Fields from class T_B0_enumerated_type :</H2>\n<UL>") ;

  s_fprintf(fd, "<LI>name = %s</LI>\n", ptr_ref(name)) ;
  s_fprintf(fd, "<LI>ref_machine = %s</LI>\n", ptr_ref(ref_machine)) ;
  s_fprintf(fd,
			"<LI>first_enumerated_value = %s</LI>\n",
			ptr_ref(first_enumerated_value)) ;
  s_fprintf(fd,
			"<LI>last_enumerated_value = %s</LI></UL>\n",
			ptr_ref(last_enumerated_value)) ;

  // Fermeture du fichier HTML
  close_html(fd) ;

  return NULL ;

}

//
//	}{	Classe T_B0_abstract_type : methode de dump HTML
//
FILE *T_B0_abstract_type::dump_html(void)
{
  TRACE1("T_B0_abstract_type(%x)::dump_html", this) ;

  FILE *fd = T_B0_type::dump_html() ;

  s_fprintf(fd, "<H2>Fields from class T_B0_abstract_type :</H2>\n<UL>") ;

  s_fprintf(fd, "<LI>name = %s</LI>\n", ptr_ref(name)) ;
  s_fprintf(fd, "<LI>valuation_name = %s</LI>\n", ptr_ref(valuation_name)) ;
  s_fprintf(fd, "<LI>ref_machine = %s</LI>\n", ptr_ref(ref_machine)) ;
  s_fprintf(fd, "<LI>ref_interval = %s</LI>\n", ptr_ref(ref_interval)) ;
  s_fprintf(fd, "<LI>ref_decla = %s</LI>\n", ptr_ref(ref_decla)) ;

  // Fermeture du fichier HTML
  close_html(fd) ;

  return NULL ;

}


//
//	}{	Fin du fichier
//
