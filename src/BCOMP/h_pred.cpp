/******************************* CLEARSY **************************************
* Fichier : $Id: h_pred.cpp,v 2.0 1998-01-13 10:29:11 sl Exp $
* (C) 2008 CLEARSY
*
* Description :		Compilateur B
*					Predicats
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
RCS_ID("$Id: h_pred.cpp,v 1.4 1998-01-13 10:29:11 sl Exp $") ;

/* Includes systeme */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

/* Includes Composant Local */
#include "c_api.h"

//
//	}{	Classe T_binary_predicate : dump HTML
//
static const char *debug_binary_predicate_type_name_sct[] =
	{
	/* 'or'		*/	"BPRED_OR",
	/* '=>'		*/	"BPRED_IMPLIES",
	/* '<=>'	*/	"BPRED_EQUIVALENT",
	/* '&'		*/	"BPRED_CONJONCTION",
	} ;

FILE *T_binary_predicate::dump_html(void)
{
TRACE1("T_binary_predicate(%x)::dump_html", this) ;

// Dump du betree
FILE *fd = T_predicate::dump_html() ;

s_fprintf(fd, "<H2>Fields from class T_binary_predicate :</H2>") ;

s_fprintf(fd, "<UL><LI>pred1 = %s\n", ptr_ref(pred1)) ;
s_fprintf(fd, "<LI>pred2 = %s\n", ptr_ref(pred2)) ;

size_t table_size = sizeof(debug_binary_predicate_type_name_sct) / sizeof(char *) ;

if ( (predicate_type  < 0) || ((size_t)predicate_type >= table_size) )
	{
	s_fprintf(fd,
			"<LI>predicate_type = %d IS OUT OF BOUNDS (0 .. %d)</UL>\n",
			predicate_type,
			table_size - 1) ;

	toplevel_error(CAN_CONTINUE,
								"predicate_type = %d is out of bounds (0 .. %d)\n",
								predicate_type,
								table_size - 1) ;

	}
else
	{
	s_fprintf(fd,
			"<LI>predicate_type = %s</UL>\n",
			debug_binary_predicate_type_name_sct[predicate_type]) ;
	}

// Fermeture du fichier HTML
close_html(fd) ;

return NULL ;
}

//
//	}{	Classe T_expr_predicate : dump HTML
//
static const char *debug_expr_predicate_type_name_sct[] =
	{
//	/* '='		*/	"EPRED_EQUAL",
	/* '/='		*/	"EPRED_DIFFERENT",
//	/* ':'		*/	"EPRED_BELONGS",
	/* '/:'		*/	"EPRED_NOT_BELONGS",
//	/* '<:'		*/	"EPRED_INCLUDED",
//	/* '<<:'	*/	"EPRED_STRICT_INCLUDED",
	/* '/<:'	*/	"EPRED_NOT_INCLUDED",
	/* '/<<:'	*/	"EPRED_NOT_STRICT_INCLUDED",
	/* '<'		*/	"EPRED_LESS_THAN",
	/* '>'		*/	"EPRED_GREATER_THAN",
	/* '<='		*/	"EPRED_LESS_THAN_OR_EQUAL",
    /* '>='		*/	"EPRED_GREATER_THAN_OR_EQUAL",
	} ;

FILE *T_expr_predicate::dump_html(void)
{
TRACE1("T_expr_predicate(%x)::dump_html", this) ;

// Dump du betree
FILE *fd = T_predicate::dump_html() ;

s_fprintf(fd, "<H2>Fields from class T_expr_predicate :</H2>") ;

s_fprintf(fd, "<UL><LI>expr1 = %s\n", ptr_ref(expr1)) ;
s_fprintf(fd, "<LI>expr2 = %s\n", ptr_ref(expr2)) ;

size_t table_size = sizeof(debug_expr_predicate_type_name_sct) / sizeof(char *) ;

if ( (predicate_type  < 0) || ((size_t)predicate_type >= table_size) )
	{
	s_fprintf(fd,
			"<LI>predicate_type = %d IS OUT OF BOUNDS (0 .. %d)</UL>\n",
			predicate_type,
			table_size - 1) ;

	toplevel_error(CAN_CONTINUE,
								"predicate_type = %d is out of bounds (0 .. %d)\n",
								predicate_type,
								table_size - 1) ;

	}
else
	{
	s_fprintf(fd,
			"<LI>predicate_type = %s</UL>\n",
			debug_expr_predicate_type_name_sct[predicate_type]) ;
	}

// Fermeture du fichier HTML
close_html(fd) ;

return NULL ;
}

//
//	}{	Classe T_typing_predicate : dump HTML
//
static const char *debug_typing_predicate_type_name_sct[] =
	{
	/* '='		*/	"TPRED_EQUAL",
	/* ':'		*/	"TPRED_BELONGS",
	/* '<:'		*/	"TPRED_INCLUDED",
	/* '<<:'	*/	"TPRED_STRICT_INCLUDED",
	} ;

FILE *T_typing_predicate::dump_html(void)
{
  TRACE1("T_typing_predicate(%x)::dump_html", this) ;
  // Dump du betree
  FILE *fd = T_predicate::dump_html() ;

  s_fprintf(fd, "<H2>Fields from class T_typing_predicate :</H2>") ;

  s_fprintf(fd, "<UL><LI>first_ident = %s\n", ptr_ref(first_ident)) ;
  s_fprintf(fd, "<LI>last_ident = %s\n", ptr_ref(last_ident)) ;
  s_fprintf(fd, "<LI>type = %s\n", ptr_ref(type)) ;

  size_t table_size = sizeof(debug_typing_predicate_type_name_sct)/sizeof(char *) ;

  if ( (typing_predicate_type  < 0)||((size_t)typing_predicate_type >= table_size) )
	{
	  s_fprintf(fd,
				"<LI>typing_predicate_type = %d IS OUT OF BOUNDS (0 .. %d)</UL>\n",
				typing_predicate_type,
				table_size - 1) ;

	  toplevel_error(CAN_CONTINUE,
					 "typing_predicate_type = %d is out of bounds (0 .. %d)\n",
					 typing_predicate_type,
					 table_size - 1) ;

	}
  else
	{
	  s_fprintf(fd,
				"<LI>typing_predicate_type = %s</UL>\n",
				debug_typing_predicate_type_name_sct[typing_predicate_type]) ;
	}

  // Fermeture du fichier HTML
  close_html(fd) ;

  return NULL ;
}

//
//	}{	Classe T_predicate_with_parenthesis : methode de dump HTML
//
FILE *T_predicate_with_parenthesis::dump_html(void)
{
TRACE1("T_predicate_with_parenthesis(%x)::dump_html", this) ;

// Dump du betree
FILE *fd = T_predicate::dump_html() ;

s_fprintf(fd, "<H2>Fields from class T_predicate_with_parenthesis :</H2>") ;

s_fprintf(fd, "<UL><LI>predicate = %s</UL>\n", ptr_ref(predicate)) ;

// Fermeture du fichier HTML
close_html(fd) ;

return NULL ;
}

//
//	}{	Classe T_not_predicate : methode de dump HTML
//
FILE *T_not_predicate::dump_html(void)
{
TRACE1("T_not_predicate(%x)::dump_html", this) ;

// Dump du betree
FILE *fd = T_predicate::dump_html() ;

s_fprintf(fd, "<H2>Fields from class T_not_predicate :</H2>") ;

s_fprintf(fd, "<UL><LI>predicate = %s</UL>\n", ptr_ref(predicate)) ;

// Fermeture du fichier HTML
close_html(fd) ;

return NULL ;
}

//
//	}{	Classe T_universal_predicate : methode de dump HTML
//
FILE *T_universal_predicate::dump_html(void)
{
TRACE1("T_universal_predicate(%x)::dump_html", this) ;

// Dump du betree
FILE *fd = T_predicate::dump_html() ;

s_fprintf(fd, "<H2>Fields from class T_universal_predicate :</H2>") ;

s_fprintf(fd, "<UL><LI>first_variable = %s\n", ptr_ref(first_variable)) ;
s_fprintf(fd, "<LI>last_variable = %s\n", ptr_ref(last_variable)) ;
s_fprintf(fd, "<LI>predicate = %s\n", ptr_ref(predicate)) ;

// Fermeture du fichier HTML
close_html(fd) ;

return NULL ;
}

//
//	}{	Classe T_existential_predicate : methode de dump HTML
//
FILE *T_existential_predicate::dump_html(void)
{
TRACE1("T_existential_predicate(%x)::dump_html", this) ;

// Dump du betree
FILE *fd = T_predicate::dump_html() ;

s_fprintf(fd, "<H2>Fields from class T_existential_predicate :</H2>") ;

s_fprintf(fd, "<UL><LI>first_variable = %s\n", ptr_ref(first_variable)) ;
s_fprintf(fd, "<LI>last_variable = %s\n", ptr_ref(last_variable)) ;
s_fprintf(fd, "<LI>predicate = %s\n", ptr_ref(predicate)) ;

// Fermeture du fichier HTML
close_html(fd) ;

return NULL ;
}

//
//	}{	Fin du fichier
//

