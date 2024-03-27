/******************************* CLEARSY **************************************
* Fichier : $Id: h_subst.cpp,v 2.0 1999-07-19 15:58:22 sl Exp $
* (C) 2008 CLEARSY
*
* Description :		Compilateur B
*					Substitutions "atomiques"
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
RCS_ID("$Id: h_subst.cpp,v 1.5 1999-07-19 15:58:22 sl Exp $") ;

/* Includes systeme */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

/* Includes Composant Local */
#include "c_api.h"

FILE *T_affect::dump_html(void)
{
TRACE1("T_affect(%x)::dump_html", this) ;

// Dump du betree
FILE *fd = T_substitution::dump_html() ;

s_fprintf(fd, "<H2>Fields from class T_affect :</H2>\n<UL>") ;

s_fprintf(fd, "<LI>first_name = %s</LI>\n", ptr_ref(first_name)) ;
s_fprintf(fd, "<LI>last_name = %s</LI>\n", ptr_ref(last_name)) ;

s_fprintf(fd, "<LI>first_value = %s</LI>\n", ptr_ref(first_value)) ;
s_fprintf(fd, "<LI>last_value = %s</LI></UL>\n", ptr_ref(last_value)) ;

// Fermeture du fichier HTML
close_html(fd) ;

return NULL ;
}

//
//	}{	Dump HTML
//
FILE *T_becomes_member_of::dump_html(void)
{
TRACE1("T_becomes_member_of(%x)::dump_html", this) ;

// Dump du betree
FILE *fd = T_substitution::dump_html() ;

s_fprintf(fd, "<H2>Fields from class T_becomes_member_of :</H2><UL>") ;

s_fprintf(fd, "<LI>first_ident = %s\n", ptr_ref(first_ident)) ;
s_fprintf(fd, "<LI>last_ident = %s\n", ptr_ref(last_ident)) ;

s_fprintf(fd, "<LI>set = %s</UL>\n", ptr_ref(set)) ;

// Fermeture du fichier HTML
close_html(fd) ;

return NULL ;
}

FILE *T_becomes_such_that::dump_html(void)
{
TRACE1("T_becomes_such_that(%x)::dump_html", this) ;

// Dump du betree
FILE *fd = T_substitution::dump_html() ;

s_fprintf(fd, "<H2>Fields from class T_becomes_such_that :</H2><UL>") ;

s_fprintf(fd, "<LI>first_ident = %s\n", ptr_ref(first_ident)) ;
s_fprintf(fd, "<LI>last_ident = %s\n", ptr_ref(last_ident)) ;

s_fprintf(fd, "<LI>pred = %s</UL>\n", ptr_ref(pred)) ;

// Fermeture du fichier HTML
close_html(fd) ;

return NULL ;
}

//
//	}{	Classe T_op_call : dump HTML
//
FILE *T_op_call::dump_html(void)
{
TRACE1("T_op_call(%x)::dump_html", this) ;

// Dump du betree
FILE *fd = T_substitution::dump_html() ;

s_fprintf(fd, "<H2>Fields from class T_op_call :</H2><UL>") ;

s_fprintf(fd, "<LI>op_name = %s\n", ptr_ref(op_name)) ;

s_fprintf(fd, "<LI>first_out_param = %s\n", ptr_ref(first_out_param)) ;
s_fprintf(fd, "<LI>last_out_param = %s\n", ptr_ref(last_out_param)) ;

s_fprintf(fd, "<LI>first_in_param = %s\n", ptr_ref(first_in_param)) ;
s_fprintf(fd, "<LI>last_in_param = %s\n", ptr_ref(last_in_param)) ;

s_fprintf(fd, "<LI>ref_op = %s\n", ptr_ref(ref_op)) ;

// Fermeture du fichier HTML
close_html(fd) ;

return NULL ;
}

FILE *T_skip::dump_html(void)
{
TRACE1("T_skip(%x)::dump_html", this) ;

// Dump du betree
FILE *fd = T_substitution::dump_html() ;

s_fprintf(fd, "<H2>Class T_skip does not have any specific field</H2>") ;

// Fermeture du fichier HTML
close_html(fd) ;

return NULL ;
}

//
//	}{	Methode de dump_html
//
FILE *T_begin::dump_html(void)
{
TRACE1("T_begin(%x)::dump_html", this) ;

// Dump du betree
FILE *fd = T_substitution::dump_html() ;

s_fprintf(fd, "<H2>Fields from class T_begin :</H2>") ;

s_fprintf(fd, "<UL><LI>first_body = %s\n", ptr_ref(first_body)) ;
s_fprintf(fd, "<LI>last_body = %s\n", ptr_ref(last_body)) ;

s_fprintf(fd, "<LI>end_begin = %s</UL>\n", ptr_ref(end_begin)) ;

// Fermeture du fichier HTML
close_html(fd) ;

return NULL ;
}

FILE *T_choice::dump_html(void)
{
TRACE1("T_choice(%x)::dump_html", this) ;

// Dump du betree
FILE *fd = T_substitution::dump_html() ;

s_fprintf(fd, "<H2>Fields from class T_choice :</H2>") ;

s_fprintf(fd, "<UL><LI>first_body = %s\n", ptr_ref(first_body)) ;
s_fprintf(fd, "<LI>last_body = %s\n", ptr_ref(last_body)) ;
s_fprintf(fd, "<LI>first_or = %s\n", ptr_ref(first_or)) ;
s_fprintf(fd, "<LI>last_or = %s\n", ptr_ref(last_or)) ;
s_fprintf(fd, "<LI>end_choice = %s</UL>\n", ptr_ref(end_choice)) ;

// Fermeture du fichier HTML
close_html(fd) ;

return NULL ;
}

FILE *T_or::dump_html(void)
{
TRACE1("T_or(%x)::dump_html", this) ;

// Dump du betree
FILE *fd = T_item::dump_html() ;

s_fprintf(fd, "<H2>Fields from class T_or :</H2>") ;

s_fprintf(fd, "<UL><LI>first_body = %s\n", ptr_ref(first_body)) ;
s_fprintf(fd, "<LI>last_body = %s</UL>\n", ptr_ref(last_body)) ;

// Fermeture du fichier HTML
close_html(fd) ;

return NULL ;
}

FILE *T_precond::dump_html(void)
{
TRACE1("T_precond(%x)::dump_html", this) ;

// Dump du betree
FILE *fd = T_substitution::dump_html() ;

s_fprintf(fd, "<H2>Fields from class T_precond :</H2>") ;

s_fprintf(fd, "<UL><LI>predicate = %s\n", ptr_ref(predicate)) ;

s_fprintf(fd," <LI>then_keyword = %s</UL>\n", ptr_ref(then_keyword)) ;
s_fprintf(fd, "<LI>first_body = %s\n", ptr_ref(first_body)) ;
s_fprintf(fd, "<LI>last_body = %s\n", ptr_ref(last_body)) ;
s_fprintf(fd," <LI>end_precond = %s</UL>\n", ptr_ref(end_precond)) ;

// Fermeture du fichier HTML
close_html(fd) ;

return NULL ;
}

//
//	}{	Classe T_assert
//
FILE *T_assert::dump_html(void)
{
TRACE1("T_assert(%x)::dump_html", this) ;

// Dump du betree
FILE *fd = T_substitution::dump_html() ;

s_fprintf(fd, "<H2>Fields from class T_assert :</H2>") ;

s_fprintf(fd, "<UL><LI>predicate = %s\n", ptr_ref(predicate)) ;

s_fprintf(fd, "<LI>then = %s\n", ptr_ref(then)) ;
s_fprintf(fd, "<LI>first_body = %s\n", ptr_ref(first_body)) ;
s_fprintf(fd, "<LI>last_body = %s\n", ptr_ref(last_body)) ;
s_fprintf(fd," <LI>end_assert = %s</UL>\n", ptr_ref(end_assert)) ;

// Fermeture du fichier HTML
close_html(fd) ;

return NULL ;
}

//
//	}{	Classe T_label
//
FILE *T_label::dump_html(void)
{
TRACE1("T_label(%x)::dump_html", this) ;

// Dump du betree
FILE *fd = T_substitution::dump_html() ;

s_fprintf(fd, "<H2>Fields from class T_label :</H2>") ;

s_fprintf(fd, "<UL><LI>expression = %s\n", ptr_ref(expression)) ;

s_fprintf(fd, "<LI>then = %s\n", ptr_ref(then)) ;
s_fprintf(fd, "<LI>first_body = %s\n", ptr_ref(first_body)) ;
s_fprintf(fd, "<LI>last_body = %s\n", ptr_ref(last_body)) ;
s_fprintf(fd," <LI>end_assert = %s</UL>\n", ptr_ref(end_assert)) ;

// Fermeture du fichier HTML
close_html(fd) ;

return NULL ;
}

//
//	}{	Classe T_jumpif
//
FILE *T_jumpif::dump_html(void)
{
TRACE1("T_jumpif(%x)::dump_html", this) ;

// Dump du betree
FILE *fd = T_substitution::dump_html() ;

s_fprintf(fd, "<H2>Fields from class T_jumpif :</H2>") ;

s_fprintf(fd, "<UL><LI>condition = %s\n", ptr_ref(cond)) ;

s_fprintf(fd, "<LI>to = %s\n", ptr_ref(to)) ;
s_fprintf(fd, "<LI>label = %s\n", ptr_ref(label)) ;
s_fprintf(fd," <LI>end_assert = %s</UL>\n", ptr_ref(end_jumpif)) ;

// Fermeture du fichier HTML
close_html(fd) ;

return NULL ;
}

//
//	}{	Classe T_witness
//
FILE *T_witness::dump_html(void)
{
TRACE1("T_witness(%x)::dump_html", this) ;

// Dump du betree
FILE *fd = T_substitution::dump_html() ;

s_fprintf(fd, "<H2>Fields from class T_witness :</H2>") ;

s_fprintf(fd, "<UL><LI>predicate = %s\n", ptr_ref(predicate)) ;

s_fprintf(fd, "<LI>then = %s\n", ptr_ref(then)) ;
s_fprintf(fd, "<LI>first_body = %s\n", ptr_ref(first_body)) ;
s_fprintf(fd, "<LI>last_body = %s\n", ptr_ref(last_body)) ;
s_fprintf(fd," <LI>end_witness = %s</UL>\n", ptr_ref(end_witness)) ;

// Fermeture du fichier HTML
close_html(fd) ;

return NULL ;
}

//
//	}{	Dump HTML
//
FILE *T_if::dump_html(void)
{
TRACE1("T_if(%x)::dump_html", this) ;

// Dump du betree
FILE *fd = T_substitution::dump_html() ;

s_fprintf(fd, "<H2>Fields from class T_if :</H2>") ;

s_fprintf(fd, "<UL><LI>cond = %s\n", ptr_ref(cond)) ;

s_fprintf(fd, "<LI>first_then_body = %s\n", ptr_ref(first_then_body)) ;
s_fprintf(fd, "<LI>last_then_body = %s\n", ptr_ref(last_then_body)) ;

s_fprintf(fd, "<LI>first_else = %s\n", ptr_ref(first_else)) ;
s_fprintf(fd, "<LI>last_else = %s\n", ptr_ref(last_else)) ;
s_fprintf(fd," <LI>end_if = %s\n", ptr_ref(end_if)) ;

// Fermeture du fichier HTML
close_html(fd) ;

return NULL ;
}

FILE *T_else::dump_html(void)
{
TRACE1("T_else(%x)::dump_html", this) ;

// Dump du betree
FILE *fd = T_item::dump_html() ;

s_fprintf(fd, "<H2>Fields from class T_else :</H2>") ;

s_fprintf(fd, "<UL><LI>cond = %s\n", ptr_ref(cond)) ;

s_fprintf(fd, "<LI>first_body = %s\n", ptr_ref(first_body)) ;
s_fprintf(fd, "<LI>last_body = %s\n", ptr_ref(last_body)) ;


// Fermeture du fichier HTML
close_html(fd) ;

return NULL ;
}

FILE *T_select::dump_html(void)
{
TRACE1("T_select(%x)::dump_html", this) ;

// Dump du betree
FILE *fd = T_substitution::dump_html() ;

s_fprintf(fd, "<H2>Fields from class T_select :</H2>") ;

s_fprintf(fd, "<UL><LI>cond = %s\n", ptr_ref(cond)) ;

s_fprintf(fd, "<LI>first_then_body = %s\n", ptr_ref(first_then_body)) ;
s_fprintf(fd, "<LI>last_then_body = %s\n", ptr_ref(last_then_body)) ;

s_fprintf(fd, "<LI>first_when = %s\n", ptr_ref(first_when)) ;
s_fprintf(fd, "<LI>last_when = %s\n", ptr_ref(last_when)) ;
s_fprintf(fd," <LI>end_select = %s\n", ptr_ref(end_select)) ;


// Fermeture du fichier HTML
close_html(fd) ;

return NULL ;
}

FILE *T_when::dump_html(void)
{
TRACE1("T_when(%x)::dump_html", this) ;

// Dump du betree
FILE *fd = T_item::dump_html() ;

s_fprintf(fd, "<H2>Fields from class T_when :</H2>") ;

s_fprintf(fd, "<UL><LI>cond = %s\n", ptr_ref(cond)) ;

s_fprintf(fd, "<LI>first_body = %s\n", ptr_ref(first_body)) ;
s_fprintf(fd, "<LI>last_body = %s\n", ptr_ref(last_body)) ;

// Fermeture du fichier HTML
close_html(fd) ;

return NULL ;
}

FILE *T_case::dump_html(void)
{
TRACE1("T_case(%x)::dump_html", this) ;

// Dump du betree
FILE *fd = T_substitution::dump_html() ;

s_fprintf(fd, "<H2>Fields from class T_case :</H2><UL>") ;

s_fprintf(fd, "<LI>case_select = %s\n", ptr_ref(case_select)) ;

s_fprintf(fd, "<LI>first_case_item = %s\n", ptr_ref(first_case_item)) ;
s_fprintf(fd, "<LI>last_case_item = %s\n", ptr_ref(last_case_item)) ;
s_fprintf(fd," <LI>end_either = %s\n", ptr_ref(end_either)) ;
s_fprintf(fd," <LI>end_case = %s\n", ptr_ref(end_case)) ;

// Fermeture du fichier HTML
close_html(fd) ;

return NULL ;
}

FILE *T_case_item::dump_html(void)
{
TRACE1("T_case_item(%x)::dump_html", this) ;

// Dump du betree
FILE *fd = T_item::dump_html() ;

s_fprintf(fd, "<H2>Fields from class T_case_item :</H2><UL>") ;

s_fprintf(fd, "<LI>first_literal_value = %s\n", ptr_ref(first_literal_value)) ;
s_fprintf(fd, "<LI>last_literal_value = %s\n", ptr_ref(last_literal_value)) ;

s_fprintf(fd, "<LI>first_body = %s\n", ptr_ref(first_body)) ;
s_fprintf(fd, "<LI>last_body = %s\n</UL>\n", ptr_ref(last_body)) ;

// Fermeture du fichier HTML
close_html(fd) ;

return NULL ;
}

FILE *T_let::dump_html(void)
{
TRACE1("T_let(%x)::dump_html", this) ;

// Dump du betree
FILE *fd = T_substitution::dump_html() ;

s_fprintf(fd, "<H2>Fields from class T_let :</H2><UL>") ;

s_fprintf(fd, "<LI>first_ident = %s\n", ptr_ref(first_ident)) ;
s_fprintf(fd, "<LI>last_ident = %s\n", ptr_ref(last_ident)) ;

s_fprintf(fd, "<LI>first_valuation = %s\n", ptr_ref(first_valuation)) ;
s_fprintf(fd, "<LI>last_valuation = %s\n", ptr_ref(last_valuation)) ;

s_fprintf(fd, "<LI>first_body = %s\n", ptr_ref(first_body)) ;
s_fprintf(fd, "<LI>last_body = %s\n", ptr_ref(last_body)) ;
s_fprintf(fd," <LI>end_let = %s</UL>\n", ptr_ref(end_let)) ;

// fermeture du fichier HTML
close_html(fd) ;

return NULL ;
}

FILE *T_var::dump_html(void)
{
TRACE1("T_var(%x)::dump_html", this) ;

// Dump du betree
FILE *fd = T_item::dump_html() ;

s_fprintf(fd, "<H2>Fields from class T_var :</H2><UL>") ;

s_fprintf(fd, "<LI>first_ident = %s\n", ptr_ref(first_ident)) ;
s_fprintf(fd, "<LI>last_ident = %s\n", ptr_ref(last_ident)) ;

s_fprintf(fd, "<LI>first_body = %s\n", ptr_ref(first_body)) ;
s_fprintf(fd, "<LI>last_body = %s\n", ptr_ref(last_body)) ;
s_fprintf(fd, "<LI>end_var = %s</UL>\n", ptr_ref(end_var)) ;

// Fermeture du fichier HTML
close_html(fd) ;

return NULL ;
}

FILE *T_while::dump_html(void)
{
TRACE1("T_while(%x)::dump_html", this) ;

// Dump du betree
FILE *fd = T_item::dump_html() ;

s_fprintf(fd, "<H2>Fields from class T_while :</H2><UL>") ;

s_fprintf(fd, "<LI>cond = %s\n", ptr_ref(cond)) ;

s_fprintf(fd, "<LI>first_body = %s\n", ptr_ref(first_body)) ;
s_fprintf(fd, "<LI>last_body = %s\n", ptr_ref(last_body)) ;

s_fprintf(fd, "<LI>invariant_keyword = %s\n", ptr_ref(invariant_keyword)) ;
s_fprintf(fd, "<LI>invariant = %s\n", ptr_ref(invariant)) ;
s_fprintf(fd, "<LI>variant_keyword = %s\n", ptr_ref(variant_keyword)) ;
s_fprintf(fd, "<LI>variant = %s\n", ptr_ref(variant)) ;
s_fprintf(fd, "<LI>end_while = %s</UL>\n", ptr_ref(end_while)) ;

// Fermeture du fichier HTML
close_html(fd) ;

return NULL ;
}

FILE *T_any::dump_html(void)
{
TRACE1("T_any(%x)::dump_html", this) ;

// Dump du betree
FILE *fd = T_item::dump_html() ;

s_fprintf(fd, "<H2>Fields from class T_any :</H2>") ;

s_fprintf(fd, "<UL><LI>first_ident = %s\n", ptr_ref(first_ident)) ;
s_fprintf(fd, "<LI>last_ident = %s\n", ptr_ref(last_ident)) ;

s_fprintf(fd, "<LI>where = %s\n", ptr_ref(where)) ;

s_fprintf(fd, "<LI>first_body = %s\n", ptr_ref(first_body)) ;
s_fprintf(fd, "<LI>last_body = %s\n", ptr_ref(last_body)) ;
s_fprintf(fd, "<LI>end_any = %s</UL>\n", ptr_ref(end_any)) ;

// Fermeture du fichier HTML
close_html(fd) ;

return NULL ;
}

//
//	}{ Classe T_substitution
//
FILE *T_substitution::dump_html(void)
{
TRACE1("T_substitution(%x)::dump_html", this) ;

// Dump du betree
FILE *fd = T_item::dump_html() ;

s_fprintf(fd, "<H2>Fields from class T_substitution :</H2>") ;

if (get_next() != NULL)
	{
	s_fprintf(fd, "<UL><LI>link_type = %s</UL>\n",
			(link_type == LINK_PARALLEL) ? "LINK_PARALLEL" : "LINK_SEQUENCE") ;
	}
else
	{
	s_fprintf(fd, "<UL><LI>link_type does not have a significative value</UL>\n") ;
	}

return fd ;
}

//
//	}{	Fin du fichier
//
