/******************************* CLEARSY **************************************
* Fichier : $Id: h_type.cpp,v 2.0 2001-07-19 16:27:07 lv Exp $
* (C) 2008 CLEARSY
*
* Description :		Compilateur B
*					Dump HTML des types
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
RCS_ID("$Id: h_type.cpp,v 1.13 2001-07-19 16:27:07 lv Exp $") ;

/* Includes systeme */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

/* Includes Composant Local */
#include "c_api.h"

FILE *T_type::dump_html(void)
{
  TRACE1("T_type(%x)::dump_html", this) ;

  FILE *fd = T_item::dump_html() ;

  s_fprintf(fd, "<H2>Class T_type :</H2>") ;

  s_fprintf(fd, "<LI>B0_type = %s</LI></UL>\n", ptr_ref(B0_type)) ;
  s_fprintf(fd, "<LI>type_name = %s</LI>\n", sym_ref(type_name)) ;
  s_fprintf(fd, "<LI>typing_ident = %s</LI>\n", ptr_ref(typing_ident)) ;
  s_fprintf(fd, "</UL>") ;

  return fd ;
}

FILE *T_constructor_type::dump_html(void)
{
  TRACE1("T_constructor_type(%x)::dump_html", this) ;

  FILE *fd = T_type::dump_html() ;

  s_fprintf(fd, "<H2>Fields from class T_constructor_type :</H2>") ;

  return fd ;
}

FILE *T_base_type::dump_html(void)
{
  TRACE1("T_base_type(%x)::dump_html", this) ;

  FILE *fd = T_type::dump_html() ;

  s_fprintf(fd, "<H2>Fields from class T_base_type :</H2>") ;

  s_fprintf(fd, "<UL><LI>name = %s\n</LI>", ptr_ref(name)) ;
  s_fprintf(fd, "<LI>lower_bound = %s\n</LI>", ptr_ref(lower_bound)) ;
  s_fprintf(fd, "<LI>upper_bound = %s\n</LI></UL>", ptr_ref(upper_bound)) ;

  s_fprintf(fd,
			"<FONT color=red>=> is_bounded() = %s\n</font>",
			bool_ref(is_bounded())) ;

  return fd ;
}

//
//	}{	Classe T_base_type : methode de dump HTML
//
static const char *debug_base_type_sct[] =
{
  /* 00 '='		*/	"BTYPE_INTEGER",
					/* 01 '/='		*/	"BTYPE_BOOL",
					/* 02 '<'		*/	"BTYPE_STRING",
} ;

FILE *T_predefined_type::dump_html(void)
{
  TRACE1("T_predefined_type(%x)::dump_html", this) ;

  FILE *fd = T_base_type::dump_html() ;

  s_fprintf(fd, "<H2>Fields from class T_predefined_type :</H2>") ;

  size_t table_size = sizeof(debug_base_type_sct) / sizeof(char *) ;

  if ( (type  < 0) || ((size_t)type >= table_size) )
	{
	  s_fprintf(fd,
				"<LI>type = %d IS OUT OF BOUNDS (0 .. %d)</UL>\n",
				type,
				table_size - 1) ;

	  toplevel_error(CAN_CONTINUE,
					 "type = %d is out of bounds (0 .. %d)\n",
					 type,
					 table_size - 1) ;

	}
  else
	{
	  s_fprintf(fd,
				"<LI>type = %s\n",
				debug_base_type_sct[type]) ;
	}

  s_fprintf(fd, "<LI>ref_interval = %s</UL></LI>\n", ptr_ref(ref_interval)) ;

  // Fermeture du fichier HTML
  close_html(fd) ;

  return NULL ;

}

//
//	}{	Classe T_product_type : methode de dump HTML
//
FILE *T_product_type::dump_html(void)
{
  TRACE1("T_product_type(%x)::dump_html", this) ;

  FILE *fd = T_constructor_type::dump_html() ;

  s_fprintf(fd, "<H2>Fields from class T_product_type :</H2>") ;
  s_fprintf(fd, "<UL><LI>type1 = %s</LI>\n", ptr_ref(type1)) ;
  s_fprintf(fd, "<LI>type2 = %s</LI>\n", ptr_ref(type2)) ;

  close_html(fd) ;

  return NULL ;

}

//
//	}{	Classe T_setof_type : methode de dump HTML
//
FILE *T_setof_type::dump_html(void)
{
  TRACE1("T_setof_type(%x)::dump_html", this) ;

  FILE *fd = T_constructor_type::dump_html() ;

  s_fprintf(fd, "<H2>Fields from class T_setof_type :</H2>") ;
  s_fprintf(fd, "<UL><LI>base_type = %s</LI>\n", ptr_ref(base_type)) ;
  s_fprintf(fd, "<LI>get_base_type() = %s</LI></UL>\n", ptr_ref(get_base_type())) ;

#ifdef DEBUG_TYPES
  if ((get_father() != NULL)
	  && (get_father()->is_a_type() == FALSE)
	  && (get_father()->is_a_relation() == TRUE))
	{
	  s_fprintf(fd,
				"<HR><UL><LI>get_relation_src_type = %s",
				ptr_ref(get_relation_src_type())) ;
	  s_fprintf(fd,
				"<LI>get_relation_dst_type = %s</UL>",
				ptr_ref(get_relation_dst_type())) ;
	}
#endif

  if (get_node_type() == NODE_SETOF_TYPE)
	{
	  close_html(fd) ;

	  return NULL ;
	}

  // Pour les sous classes
  return fd ;

}

//
//	}{	Classe T_abstract_type : methode de dump HTML
//
FILE *T_abstract_type::dump_html(void)
{
  TRACE1("T_abstract_type(%x)::dump_html", this) ;

  FILE *fd = T_base_type::dump_html() ;

  s_fprintf(fd, "<H2>Fields from class T_abstract_type :</H2>") ;
  s_fprintf(fd, "<UL><LI>set = %s</LI>\n", ptr_ref(set)) ;
  s_fprintf(fd, "<LI>next_abstract_type = %s</LI>\n", ptr_ref(next_abstract_type)) ;
  s_fprintf(fd,
			"<LI>after_valuation_type = %s</LI>\n",
			ptr_ref(after_valuation_type)) ;
  s_fprintf(fd, "<LI>valuation_ident = %s</LI></UL>\n", ptr_ref(valuation_ident)) ;

  close_html(fd) ;

  return NULL ;

}

//
//	}{	Classe T_enumerated_type : methode de dump HTML
//
FILE *T_enumerated_type::dump_html(void)
{
  TRACE1("T_enumerated_type(%x)::dump_html", this) ;

  FILE *fd = T_type::dump_html() ;

  s_fprintf(fd, "<H2>Fields from class T_enumerated_type :</H2>") ;

  s_fprintf(fd,
			"<UL><LI>type_definition = %s</UL></LI>\n",
			ptr_ref(type_definition)) ;

  close_html(fd) ;

  return NULL ;

}

//
//	}{	Classe T_generic_type : methode de dump HTML
//
FILE *T_generic_type::dump_html(void)
{
  TRACE1("T_generic_type(%x)::dump_html", this) ;

  FILE *fd = T_type::dump_html() ;

  s_fprintf(fd, "<H2>Fields from class T_generic_type :</H2>") ;

  s_fprintf(fd, "<UL><LI>type = %s</LI></UL>\n", ptr_ref(type)) ;

  close_html(fd) ;

  return NULL ;

}

//
//	}{	Classe T_label_type : methode de dump HTML
//
FILE *T_label_type::dump_html(void)
{
  TRACE1("T_label_type(%x)::dump_html", this) ;

  FILE *fd = T_type::dump_html() ;

  s_fprintf(fd, "<H2>Fields from class T_label_type :</H2>") ;

  s_fprintf(fd, "<UL><LI>name = %s</LI></UL>\n", ptr_ref(name)) ;
  s_fprintf(fd, "<LI>type = %s</UL></LI>\n", ptr_ref(type)) ;

  close_html(fd) ;

  return NULL ;
}

//
//	}{	Classe T_record_type : methode de dump HTML
//
FILE *T_record_type::dump_html(void)
{
  TRACE1("T_record_type(%x)::dump_html", this) ;

  FILE *fd = T_constructor_type::dump_html() ;

  s_fprintf(fd, "<H2>Fields from class T_record_type :</H2>") ;

  s_fprintf(fd, "<UL><LI>first_label = %s</LI>\n", ptr_ref(first_label)) ;
  s_fprintf(fd, "<LI>last_label = %s</UL></LI>\n", ptr_ref(last_label)) ;

  close_html(fd) ;

  return NULL ;
}

//
//	}{	Classe T_B0_record_type : methode de dump HTML
//
FILE *T_B0_record_type::dump_html(void)
{
  TRACE1("T_B0_record_type(%x)::dump_html", this) ;

  FILE *fd = T_B0_type::dump_html() ;

  s_fprintf(fd, "<H2>Fields from class T_B0_record_type :</H2>") ;

  s_fprintf(fd, "<UL><LI>first_label = %s</LI>\n", ptr_ref(first_label)) ;
  s_fprintf(fd, "<LI>last_label = %s</LI>\n", ptr_ref(last_label)) ;
  s_fprintf(fd, "<LI>ref_machine = %s</LI>\n", ptr_ref(ref_machine)) ;
  s_fprintf(fd, "<LI>name = %s</LI>\n", sym_ref(name)) ;
  s_fprintf(fd, "<LI>prev_decla = %s</LI>\n", ptr_ref(prev_decla)) ;
  s_fprintf(fd, "<LI>next_decla = %s</LI></UL>\n", ptr_ref(next_decla)) ;

  close_html(fd) ;

  return NULL ;
}

//
//	}{	Classe T_B0_label_type : methode de dump HTML
//
FILE *T_B0_label_type::dump_html(void)
{
  TRACE1("T_B0_label_type(%x)::dump_html", this) ;

  FILE *fd = T_B0_type::dump_html() ;

  s_fprintf(fd, "<H2>Fields from class T_B0_label_type :</H2>") ;

  s_fprintf(fd, "<UL><LI>name = %s</LI></UL>\n", ptr_ref(name)) ;
  s_fprintf(fd, "<LI>type = %s</UL></LI>\n", ptr_ref(type)) ;

  close_html(fd) ;

  return NULL ;
}

//
//	}{	Fin du fichier
//
