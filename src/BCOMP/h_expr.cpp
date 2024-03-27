/******************************* CLEARSY **************************************
* Fichier : $Id: h_expr.cpp,v 2.0 2002-07-12 13:10:53 cm Exp $
* (C) 2008 CLEARSY
*
* Description :		Compilateur B
*					Expressions binaires
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
RCS_ID("$Id: h_expr.cpp,v 1.26 2002-07-12 13:10:53 cm Exp $") ;

/* Includes systeme */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

/* Includes Composant Local */
#include "c_api.h"

FILE *T_expr::dump_html(void)
{
  TRACE1("T_expr(%x)::dump_html", this) ;

  // Dump du betree
  FILE *fd = T_item::dump_html() ;

  s_fprintf(fd, "<H2>Fields from class T_expr :</H2>") ;
  s_fprintf(fd, "<UL><LI>B_type = %s</LI></UL>\n", ptr_ref(B_type)) ;

  return fd ;
}



//
//	}{	Classe T_binary_op : methode de dump HTML
//
static const char *debug_bop_name_sct[] =
{
  /* 00 '+'		*/
  "BOP_PLUS",
  /* 01 '-'		*/
  "BOP_MINUS",
  /* 02 '*'		*/
  "BOP_TIMES",
  /* 03 '/'		*/
  "BOP_DIVIDES",
  /* 04 'mod'		*/
  "BOP_MOD",
  /* 05 '**'		*/
  "BOP_POWER",
  /* 06 '<|'		*/
  "BOP_RESTRICT",
  /* 07 '<<|'		*/
  "BOP_ANTIRESTRICT",
  /* 08 '|>'		*/
  "BOP_CORESTRICT",
  /* 09 '|>>'		*/
  "BOP_ANTICORESTRICT",
  /* 10 '<+'		*/
  "BOP_LEFT_OVERLOAD",
  /* 11 '^'		*/
  "BOP_CONCAT",
  /* 12 '->'		*/
  "BOP_HEAD_INSERT",
  /* 13 '<-'		*/
  "BOP_TAIL_INSERT",
  /* 14 '/|\'		*/
  "BOP_HEAD_RESTRICT",
  /* 15 '\|/'		*/
  "BOP_TAIL_RESTRICT",
  /* 16 '..'		*/
  "BOP_INTERVAL",
  /* 17 '/\'		*/
  "BOP_INTERSECTION",
  /* 18 '\/'		*/
  "BOP_UNION",
  /* 19 '|->'		*/
  "BOP_MAPLET",
  /* 20 '<->'		*/
  "BOP_SET_RELATION",
  /* 21 '*'		*/
  "BOP_CONSTANT_FUNCTION",
  /* 22 ';'		*/
  "BOP_COMPOSITION",
  /* 23 '><'		*/
  "BOP_DIRECT_PRODUCT",
  /* 24 '||'		*/
  "BOP_PARALLEL_PRODUCT",
  /* 25 ','		*/
  "BOP_COMMA",
  /* 26 '<->>'		*/
  "BOP_SURJ_RELATION",
  /* 27 '<<->'		*/
  "BOP_TOTAL_RELATION",
  /* 28 '<<->>'		*/
  "BOP_TOTAL_SURJ_RELATION",
  //
  //	Les operateurs ci-apres sont reserves au compilateur B
  //  Ils n'ont qu'une existence temporaire et ne sont jamais
  //	visibles dans un Betree termine
  //
  /* 29 '|'		*/
  "BOP_PIPE <BLINK>(TEMPORARY BOP)</BLINK>",
} ;

FILE *T_binary_op::dump_html(void)
{
  TRACE1("T_binary_op(%x)::dump_html", this) ;

  // Dump du betree
  FILE *fd = T_expr::dump_html() ;

  s_fprintf(fd, "<H2>Fields from class T_binary_op :</H2>") ;

  s_fprintf(fd, "<UL><LI>op1 = %s\n", ptr_ref(op1)) ;
  s_fprintf(fd, "<LI>op2 = %s\n", ptr_ref(op2)) ;
  s_fprintf(fd, "<LI>first_operand = %s\n", ptr_ref(first_operand)) ;
  s_fprintf(fd, "<LI>last_operand = %s\n", ptr_ref(last_operand)) ;
  s_fprintf(fd, "<LI>nb_operands = %d\n", nb_operands) ;

  size_t table_size = sizeof(debug_bop_name_sct) / sizeof(char *) ;

  if ( (oper  < 0) || ((size_t)oper >= table_size) )
	{
	  s_fprintf(fd,
				"<LI>oper = %d IS OUT OF BOUNDS (0 .. %d)</UL>\n",
				oper,
				table_size - 1) ;

	  toplevel_error(CAN_CONTINUE,
					 "oper = %d is out of bounds (0 .. %d)\n",
					 oper,
					 table_size - 1) ;

	}
  else
	{
	  s_fprintf(fd, "<LI>oper = %s</UL>\n", debug_bop_name_sct[oper]) ;
	}

  // Fermeture du fichier HTML
  close_html(fd) ;

  return NULL ;
}

//
//	}{	Classe T_converse : methode de dump HTML
//
FILE *T_converse::dump_html(void)
{
  TRACE1("T_converse(%x)::dump_html", this) ;

  // Dump du betree
  FILE *fd = T_expr::dump_html() ;

  s_fprintf(fd, "<H2>Fields from class T_converse :</H2>") ;

  s_fprintf(fd, "<UL><LI>expr = %s</LI></UL>\n", ptr_ref(expr)) ;

  // Fermeture du fichier HTML
  close_html(fd) ;

  return NULL ;
}

//
//	}{	Classe T_array_item : methode de dump HTML
//
FILE *T_array_item::dump_html(void)
{
  TRACE1("T_array_item(%x)::dump_html", this) ;

  // Dump du betree
  FILE *fd = T_expr::dump_html() ;

  s_fprintf(fd, "<H2>Fields from class T_array_item :</H2>") ;

  s_fprintf(fd, "<UL><LI>array_name = %s\n", ptr_ref(array_name)) ;
  s_fprintf(fd, "<LI>first_index = %s\n", ptr_ref(first_index)) ;
  s_fprintf(fd, "<LI>last_index = %s\n", ptr_ref(last_index)) ;
  s_fprintf(fd, "<LI>nb_indexes = %d\n</LI>", nb_indexes) ;
  s_fprintf(fd, "<LI>separator = %d\n</LI>", separator) ;

  // Fermeture du fichier HTML
  close_html(fd) ;

  return NULL ;
}

//
//	}{	Classe T_op_result : methode de dump HTML
//
FILE *T_op_result::dump_html(void)
{
  TRACE1("T_op_result(%x)::dump_html", this) ;

  // Dump du betree
  FILE *fd = T_expr::dump_html() ;

  s_fprintf(fd, "<H2>Fields from class T_op_result :</H2>") ;

  s_fprintf(fd, "<UL><LI>op_name = %s\n", ptr_ref(op_name)) ;
  s_fprintf(fd, "<LI>ref_builtin = %s\n", sym_ref(ref_builtin)) ;
  s_fprintf(fd, "<LI>first_in_param = %s\n", ptr_ref(first_in_param)) ;
  s_fprintf(fd, "<LI>last_in_param = %s\n", ptr_ref(last_in_param)) ;
  s_fprintf(fd, "<LI>nb_in_params = %d\n", nb_in_params) ;
  s_fprintf(fd, "<LI>ref_op = %s\n", ptr_ref(ref_op)) ;
  s_fprintf(fd, "<LI>next_op_result = %s\n", ptr_ref(next_op_result)) ;
  s_fprintf(fd, "<LI>ref_array_item = %s\n</LI></UL>", ptr_ref(ref_array_item)) ;

  // Fermeture du fichier HTML
  close_html(fd) ;

  return NULL ;
}

//
//	}{	classe T_empty_seq : dump HTML
//
FILE *T_empty_seq::dump_html(void)
{
  TRACE1("T_empty_seq(%x)::dump_html", this) ;

  // Dump du betree
  FILE *fd = T_expr::dump_html() ;

  s_fprintf(fd, "<H2>Class T_empty_seq does not have any specific field</H2>") ;

  // Fermeture du fichier HTML
  close_html(fd) ;

  return NULL ;
}

//
//	}{	classe T_empty_set : dump HTML
//
FILE *T_empty_set::dump_html(void)
{
  TRACE1("T_empty_set(%x)::dump_html", this) ;

  // Dump du betree
  FILE *fd = T_expr::dump_html() ;

  s_fprintf(fd, "<H2>Class T_empty_set does not have any specific field</H2>") ;

  // Fermeture du fichier HTML
  close_html(fd) ;

  return NULL ;
}

//
//	}{	Classe T_literal_integer : methode de dump HTML
//
FILE *T_literal_integer::dump_html(void)
{
  TRACE1("T_literal_integer(%x)::dump_html", this) ;

  // Dump du betree
  FILE *fd = T_expr::dump_html() ;

  s_fprintf(fd, "<H2>Fields from class T_literal_integer :</H2>") ;

  s_fprintf(fd,
			"<UL><LI>value = %x (%s)</LI></UL>\n",
			ptr_ref(value),
			(value == NULL) ? "NULL" : sym_ref(value->get_value())) ;

  // Fermeture du fichier HTML
  close_html(fd) ;

  return NULL ;
}

//
//	}{	Classe T_literal_string : methode de dump HTML
//
FILE *T_literal_string::dump_html(void)
{
  TRACE1("T_literal_string(%x)::dump_html", this) ;

  // Dump du betree
  FILE *fd = T_expr::dump_html() ;

  s_fprintf(fd, "<H2>Fields from class T_literal_string :</H2>") ;

  s_fprintf(fd, "value = %s\n", (value == NULL) ? "NULL" : sym_ref(value)) ;

  // Fermeture du fichier HTML
  close_html(fd) ;

  return NULL ;
}

//
//	}{	Classe T_literal_real : methode de dump HTML
//
FILE *T_literal_real::dump_html(void)
{
  TRACE1("T_literal_string(%x)::dump_html", this) ;

  // Dump du betree
  FILE *fd = T_expr::dump_html() ;

  s_fprintf(fd, "<H2>Fields from class T_literal_real :</H2>") ;

  s_fprintf(fd, "value = %s\n", (value == NULL) ? "NULL" : sym_ref(value)) ;

  // Fermeture du fichier HTML
  close_html(fd) ;

  return NULL ;
}

//
//	}{	Classe T_literal_boolean : methode de dump HTML
//
FILE *T_literal_boolean::dump_html(void)
{
  TRACE1("T_literal_boolean(%x)::dump_html", this) ;

  // Dump du betree
  FILE *fd = T_expr::dump_html() ;

  s_fprintf(fd, "<H2>Fields from class T_literal_boolean :</H2>") ;

  s_fprintf(fd, "value = %s\n", bool_ref(value)) ;

  // Fermeture du fichier HTML
  close_html(fd) ;

  return NULL ;
}

//
//	}{	Classe T_expr_with_parenthesis : methode de dump HTML
//
FILE *T_expr_with_parenthesis::dump_html(void)
{
  TRACE1("T_expr_with_parenthesis(%x)::dump_html", this) ;

  // Dump du betree
  FILE *fd = T_expr::dump_html() ;

  s_fprintf(fd, "<H2>Fields from class T_expr_with_parenthesis :</H2>") ;

  s_fprintf(fd, "<UL><LI>expr = %s</UL>\n", ptr_ref(expr)) ;

  // Fermeture du fichier HTML
  close_html(fd) ;

  return NULL ;
}

//
//	}{	Classe T_comprehensive_set : methode de dump HTML
//
FILE *T_comprehensive_set::dump_html(void)
{
  TRACE1("T_comprehensive_set(%x)::dump_html", this) ;

  // Dump du betree
  FILE *fd = T_expr::dump_html() ;

  s_fprintf(fd, "<H2>Fields from class T_comprehensive_set :</H2>") ;

  s_fprintf(fd, "<UL><LI>first_ident = %s\n", ptr_ref(first_ident)) ;
  s_fprintf(fd, "<LI>last_ident = %s\n", ptr_ref(last_ident)) ;
  s_fprintf(fd, "<LI>predicate = %s\n", ptr_ref(predicate)) ;

  // Fermeture du fichier HTML
  close_html(fd) ;

  return NULL ;
}

//
//	}{	Classe T_image : methode de dump HTML
//
FILE *T_image::dump_html(void)
{
  TRACE1("T_image(%x)::dump_html", this) ;

  // Dump du betree
  FILE *fd = T_expr::dump_html() ;

  s_fprintf(fd, "<H2>Fields from class T_image :</H2>") ;

  s_fprintf(fd, "<UL><LI>relation = %s\n", ptr_ref(relation)) ;
  s_fprintf(fd, "<LI>expr = %s</UL>\n", ptr_ref(expr)) ;

  // Fermeture du fichier HTML
  close_html(fd) ;

  return NULL ;
}

//
//	}{	Classe T_extensive_set : methode de dump HTML
//
FILE *T_extensive_set::dump_html(void)
{
  TRACE1("T_extensive_set(%x)::dump_html", this) ;

  // Dump du betree
  FILE *fd = T_expr::dump_html() ;

  s_fprintf(fd, "<H2>Fields from class T_extensive_set :</H2>") ;

  s_fprintf(fd, "<UL><LI>first_item = %s\n", ptr_ref(first_item)) ;
  s_fprintf(fd, "<LI>last_item = %s</UL>\n", ptr_ref(last_item)) ;

  // Fermeture du fichier HTML
  close_html(fd) ;

  return NULL ;
}

//
//	}{	Classe T_extensive_seq : methode de dump HTML
//
FILE *T_extensive_seq::dump_html(void)
{
  TRACE1("T_extensive_seq(%x)::dump_html", this) ;

  // Dump du betree
  FILE *fd = T_expr::dump_html() ;

  s_fprintf(fd, "<H2>Fields from class T_extensive_seq :</H2>") ;

  s_fprintf(fd, "<UL><LI>first_item = %s\n", ptr_ref(first_item)) ;
  s_fprintf(fd, "<LI>last_item = %s</UL>\n", ptr_ref(last_item)) ;

  // Fermeture du fichier HTML
  close_html(fd) ;

  return NULL ;
}

//
//	}{	Classe T_generalised_union : methode de dump HTML
//
FILE *T_generalised_union::dump_html(void)
{
  TRACE1("T_generalised_union(%x)::dump_html", this) ;

  // Dump du betree
  FILE *fd = T_expr::dump_html() ;

  s_fprintf(fd, "<H2>Fields from class T_generalised_union :</H2>") ;

  s_fprintf(fd, "<UL><LI>first_expr = %s\n", ptr_ref(first_expr)) ;
  s_fprintf(fd, "<LI>last_expr = %s</UL>\n", ptr_ref(last_expr)) ;

  // Fermeture du fichier HTML
  close_html(fd) ;

  return NULL ;
}

//
//	}{	Classe T_generalised_intersection : methode de dump HTML
//
FILE *T_generalised_intersection::dump_html(void)
{
  TRACE1("T_generalised_intersection(%x)::dump_html", this) ;

  // Dump du betree
  FILE *fd = T_expr::dump_html() ;

  s_fprintf(fd, "<H2>Fields from class T_generalised_intersection :</H2>") ;

  s_fprintf(fd, "<UL><LI>first_expr = %s\n", ptr_ref(first_expr)) ;
  s_fprintf(fd, "<LI>last_expr = %s</UL>\n", ptr_ref(last_expr)) ;

  // Fermeture du fichier HTML
  close_html(fd) ;

  return NULL ;
}

//
//	}{	Classe T_unary_op : methode de dump HTML
//
static const char *debug_uop_name_sct[] =
{
  /* '-'		*/	"UOP_MINUS",
} ;

FILE *T_unary_op::dump_html(void)
{
  TRACE1("T_unary_op(%x)::dump_html", this) ;

  // Dump du betree
  FILE *fd = T_expr::dump_html() ;

  s_fprintf(fd, "<H2>Fields from class T_unary_op :</H2>") ;

  s_fprintf(fd, "<UL><LI>operand = %s\n", ptr_ref(operand)) ;

  size_t table_size = sizeof(debug_uop_name_sct) / sizeof(char *) ;

  if ( (oper  < 0) || ((size_t)oper >= table_size) )
	{
	  s_fprintf(fd,
				"<LI>oper = %d IS OUT OF BOUNDS (0 .. %d)</UL>\n",
				oper,
				table_size - 1) ;

	  toplevel_error(CAN_CONTINUE,
					 "oper = %d is out of bounds (0 .. %d)\n",
					 oper,
					 table_size - 1) ;

	}
  else
	{
	  s_fprintf(fd,
				"<LI>oper = %s</UL>\n",
				debug_uop_name_sct[oper]) ;
	}

  // Fermeture du fichier HTML
  close_html(fd) ;

  return NULL ;
}

//
//	}{	Classe T_lambda_expr : methode de dump HTML
//
FILE *T_lambda_expr::dump_html(void)
{
  TRACE1("T_lambda_expr(%x)::dump_html", this) ;

  // Dump du betree
  FILE *fd = T_expr::dump_html() ;

  s_fprintf(fd, "<H2>Fields from class T_lambda_expr :</H2>") ;

  s_fprintf(fd, "<UL><LI>first_variable = %s\n", ptr_ref(first_variable)) ;
  s_fprintf(fd, "<LI>last_variable = %s\n", ptr_ref(last_variable)) ;
  s_fprintf(fd, "<LI>predicate = %s\n", ptr_ref(predicate)) ;
  s_fprintf(fd, "<LI>expr = %s</UL>\n", ptr_ref(expr)) ;

  // Fermeture du fichier HTML
  close_html(fd) ;

  return NULL ;
}

//
//	}{	Classe T_sigma : methode de dump HTML
//
FILE *T_sigma::dump_html(void)
{
  TRACE1("T_sigma(%x)::dump_html", this) ;

  // Dump du betree
  FILE *fd = T_expr::dump_html() ;

  s_fprintf(fd, "<H2>Fields from class T_sigma :</H2>") ;

  s_fprintf(fd, "<UL><LI>first_variable = %s\n", ptr_ref(first_variable)) ;
  s_fprintf(fd, "<LI>last_variable = %s\n", ptr_ref(last_variable)) ;
  s_fprintf(fd, "<LI>predicate = %s\n", ptr_ref(predicate)) ;
  s_fprintf(fd, "<LI>expr = %s</UL>\n", ptr_ref(expr)) ;

  // Fermeture du fichier HTML
  close_html(fd) ;

  return NULL ;
}

//
//	}{	Classe T_pi : methode de dump HTML
//
FILE *T_pi::dump_html(void)
{
  TRACE1("T_pi(%x)::dump_html", this) ;

  // Dump du betree
  FILE *fd = T_expr::dump_html() ;

  s_fprintf(fd, "<H2>Fields from class T_pi :</H2>") ;

  s_fprintf(fd, "<UL><LI>first_variable = %s\n", ptr_ref(first_variable)) ;
  s_fprintf(fd, "<LI>last_variable = %s\n", ptr_ref(last_variable)) ;
  s_fprintf(fd, "<LI>predicate = %s\n", ptr_ref(predicate)) ;
  s_fprintf(fd, "<LI>expr = %s</UL>\n", ptr_ref(expr)) ;

  // Fermeture du fichier HTML
  close_html(fd) ;

  return NULL ;
}

//
//	}{	Classe T_quantified_union : methode de dump HTML
//
FILE *T_quantified_union::dump_html(void)
{
  TRACE1("T_quantified_union(%x)::dump_html", this) ;

  // Dump du betree
  FILE *fd = T_expr::dump_html() ;

  s_fprintf(fd, "<H2>Fields from class T_quantified_union :</H2>") ;

  s_fprintf(fd, "<UL><LI>first_variable = %s\n", ptr_ref(first_variable)) ;
  s_fprintf(fd, "<LI>last_variable = %s\n", ptr_ref(last_variable)) ;
  s_fprintf(fd, "<LI>predicate = %s\n", ptr_ref(predicate)) ;
  s_fprintf(fd, "<LI>expr = %s</UL>\n", ptr_ref(expr)) ;

  // Fermeture du fichier HTML
  close_html(fd) ;

  return NULL ;
}

//
//	}{	Classe T_quantified_intersection : methode de dump HTML
//
FILE *T_quantified_intersection::dump_html(void)
{
  TRACE1("T_quantified_intersection(%x)::dump_html", this) ;

  // Dump du betree
  FILE *fd = T_expr::dump_html() ;

  s_fprintf(fd, "<H2>Fields from class T_quantified_intersection :</H2>") ;

  s_fprintf(fd, "<UL><LI>first_variable = %s\n", ptr_ref(first_variable)) ;
  s_fprintf(fd, "<LI>last_variable = %s\n", ptr_ref(last_variable)) ;
  s_fprintf(fd, "<LI>predicate = %s\n", ptr_ref(predicate)) ;
  s_fprintf(fd, "<LI>expr = %s</UL>\n", ptr_ref(expr)) ;

  // Fermeture du fichier HTML
  close_html(fd) ;

  return NULL ;
}

//
//	}{	Dump HTML
//
static const char *debug_ident_type_xst[] =
{
  /* 00 */
  "ITYPE_UNKNOWN",
  /* 01 */
  "ITYPE_CONCRETE_CONSTANT",
  /* 02 */
  "ITYPE_CONCRETE_VARIABLE",
  /* 03 */
  "ITYPE_BUILTIN",
  /* 04 */
  "ITYPE_MACHINE_NAME",
  /* 05 */
  "ITYPE_ABSTRACTION_NAME",
  /* 06 */
  "ITYPE_MACHINE_SCALAR_FORMAL_PARAM",
  /* 07 */
  "ITYPE_ABSTRACT_SET",
  /* 08 */
  "ITYPE_OP_NAME",
  /* 09 */
  "ITYPE_LOCAL_OP_NAME",
  /* 10 */
  "ITYPE_MACHINE_SET_FORMAL_PARAM",
  /* 11 */
  "ITYPE_USED_MACHINE_NAME",
  /* 12 */
  "ITYPE_ENUMERATED_VALUE",
  /* 13 */
  "ITYPE_DEFINITION_PARAM",
  /* 14 */
  "ITYPE_OP_IN_PARAM",
  /* 15 */
  "ITYPE_OP_OUT_PARAM",
  /* 16 */
  "ITYPE_LOCAL_VARIABLE",
  /* 17 */
  "ITYPE_LOCAL_QUANTIFIER",
  /* 18 */
  "ITYPE_ABSTRACT_VARIABLE",
  /* 19 */
  "ITYPE_ABSTRACT_CONSTANT",
  /* 20 */
  "ITYPE_ENUMERATED_SET",
  /* 21 */
  "ITYPE_USED_OP_NAME",
  /* 22 */
  "ITYPE_RECORD_LABEL",
  /* 23 */
  "ITYPE_ANY_QUANTIFIER",
  /* 24 */
  "ITYPE_PRAGMA_PARAMETER",
} ;

FILE *T_ident::dump_html(void)
{
  TRACE1("T_ident(%x)::dump_html", this) ;

  // Dump du betree
  FILE *fd = T_expr::dump_html() ;

  if (ref_glued != NULL)
	{
	  s_fprintf(fd,
				"<H3>Identifier %s is <FONT COLOR=\"red\">glued</FONT> with \
identifier <A HREF=\"%p.html\">%s</A> from machine %s (%s)</H3>\n",
				name->get_value(),
				ref_glued,
				ref_glued->get_name()->get_value(),
				ptr_ref(ref_glued->get_ref_machine()),
				ref_glued->get_ref_machine()->get_machine_name()->
			  	get_name()->get_value()) ;
	}

  if (implemented_by != NULL)
	{
	  s_fprintf(fd,
				"<H3>Identifier %s is <FONT COLOR=\"red\">implemented by</FONT> with \
identifier <A HREF=\"%p.html\">%s</A> from machine %s (%s)</H3>\n",
				name->get_value(),
				implemented_by,
				implemented_by->get_name()->get_value(),
				ptr_ref(implemented_by->get_ref_machine()),
				implemented_by->get_ref_machine()->get_machine_name()->
			  	get_name()->get_value()) ;
	}
  s_fprintf(fd, "<H2>Fields from class T_ident :</H2>\n<UL>") ;

  s_fprintf(fd, "<LI>name = %s</LI>\n", sym_ref(name)) ;
  s_fprintf(fd, "<LI>prefix = %s</LI>\n", sym_ref(prefix)) ;

  if (prefix != NULL)
	{
	  s_fprintf(fd, "<LI>suffix_number = %d</LI>\n", suffix_number) ;
	}

  size_t table_size = sizeof(debug_ident_type_xst) / sizeof(char *) ;

  if ( (ident_type < 0) || ((size_t)ident_type >= table_size) )
	{
	  toplevel_error(CAN_CONTINUE,
					 "ident_type %d is out of bounds 0..%d\n",
					 ident_type,
					 table_size - 1) ;
	  s_fprintf(fd, "<LI>ident_type = %d OUT OF BOUNDS 0..%d\n",
				ident_type,
				table_size - 1) ;
	}
  else
	{
	  s_fprintf(fd, "<LI>ident_type = %s\n", debug_ident_type_xst[ident_type]) ;
	}

  s_fprintf(fd, "<LI>B0_type = %s</LI>\n", ptr_ref(B0_type)) ;
  s_fprintf(fd, "<LI>def = %s</LI>\n", ptr_ref(def)) ;
  s_fprintf(fd, "<LI>ref_machine = %s</LI>\n", ptr_ref(ref_machine)) ;
  s_fprintf(fd, "<LI>ref_op = %s</LI>\n", ptr_ref(ref_op)) ;
  s_fprintf(fd, "<LI>ref_glued = %s</LI>\n", ptr_ref(ref_glued)) ;
  s_fprintf(fd, "<LI>ref_inherited = %s</LI>\n", ptr_ref(ref_inherited)) ;
  s_fprintf(fd, "<LI>ref_glued_ring = %s</LI>\n", ptr_ref(ref_glued_ring)) ;
  s_fprintf(fd, "<LI>implemented_by = %s</LI>\n", ptr_ref(implemented_by)) ;
  s_fprintf(fd, "<LI>homonym_in_abstraction = %s</LI>\n", ptr_ref(homonym_in_abstraction)) ;
  s_fprintf(fd, "<LI>homonym_in_required_mach = %s</LI>\n", ptr_ref(homonym_in_required_mach)) ;
  s_fprintf(fd, "<LI>explicitly_declared = %s</LI>\n", bool_ref(explicitly_declared)) ;
  s_fprintf(fd, "<LI>first_value = %s</LI>\n", ptr_ref(first_value)) ;
  s_fprintf(fd, "<LI>last_value = %s</LI>\n", ptr_ref(last_value)) ;
  s_fprintf(fd, "<LI>next_ident = %s</LI>\n", ptr_ref(next_ident)) ;
  s_fprintf(fd, "<LI>prev_ident = %s</LI>\n", ptr_ref(prev_ident)) ;
  s_fprintf(fd, "<LI>next_unsolved_ident = %s</LI>\n", ptr_ref(next_unsolved_ident)) ;
  s_fprintf(fd,
			"<LI>typing_location = %s</LI>\n",
			ptr_ref(typing_location)) ;
  s_fprintf(fd,
			"<LI>original_typing_location = %s</LI>\n",
			ptr_ref(original_typing_location)) ;
  s_fprintf(fd,
			"<LI>typing_pred_or_subst = %s</LI>\n",
			ptr_ref(typing_pred_or_subst)) ;

  s_fprintf(fd, "<LI>inherited = %s</LI>\n", bool_ref(inherited)) ;
  s_fprintf(fd, "<LI> BOM_lexem_type = %s</LI>\n", ptr_ref(BOM_lexem_type)) ;
  s_fprintf(fd, "<LI> BOM_type = %s</LI>\n", ptr_ref(BOM_type)) ;
  s_fprintf(fd, "<LI> BOM_lexem_init_value = %s</LI>\n",
			ptr_ref(BOM_lexem_init_value)) ;
  s_fprintf(fd, "<LI> BOM_init_value = %s</LI>\n", ptr_ref(BOM_init_value)) ;

  s_fprintf(fd, "</UL><TABLE BORDER=4 CELLPADDING=4><TR>") ;
  if (get_B_type() != NULL)
	{
	  s_fprintf(fd,
                "<TR><TD><A HREF=%p.html>get_B_type()</A></TD>",
				(size_t)get_B_type()) ;
	}
  else
	{
	  s_fprintf(fd, "<TR><TD>get_B_type()</TD>") ;
	}

  if (get_B0_type() != NULL)
	{
	  s_fprintf(fd,
                "<TD><A HREF=%p.html>get_B0_type()</A></TD></TR>",
				(size_t)get_B0_type()) ;
	}
  else
	{
	  s_fprintf(fd, "<TD>get_B0_type()</TD>") ;
	}

  s_fprintf(fd, "</TABLE>\n") ;


  if (get_node_type() == NODE_IDENT)
	{
	  // Fermeture du fichier HTML
	  close_html(fd) ;

	  return NULL ;
	}
  else
	{
	  // On passe fd a la sous-classe
	  return fd ;
	}
}

//
//	}{	Dump HTML
//
FILE *T_literal_enumerated_value::dump_html(void)
{
  TRACE1("T_literal_enumerated_value(%x)::dump_html", this) ;

  // Dump superclasse
  FILE *fd = T_ident::dump_html() ;

  s_fprintf(fd, "<H2>Fields from class T_literal_enumerated_value :</H2>\n<UL>") ;

  s_fprintf(fd, "value = %d\n", value) ;

  // Fermeture du fichier HTML
  close_html(fd) ;

  return NULL ;
}

//
//	}{	Classe T_renamed_ident
//
FILE *T_renamed_ident::dump_html(void)
{
  TRACE1("T_renamed_ident(%x)::dump_html", this) ;

  // Dump superclasse
  FILE *fd = T_ident::dump_html() ;

  s_fprintf(fd, "<H2>Fields from class T_renamed_ident :</H2>\n<UL>") ;

  s_fprintf(fd, "<LI>instance_name = %s\n", sym_ref(instance_name)) ;
  s_fprintf(fd, "<LI>component_name = %s</UL>\n", sym_ref(component_name)) ;

  // Fermeture du fichier HTML
  close_html(fd) ;

  return NULL ;
}

//
//	}{	Classe T_bound
//
FILE *T_bound::dump_html(void)
{
  TRACE1("T_bound(%x)::dump_html", this) ;

  // Dump superclasse
  FILE *fd = T_expr::dump_html() ;

  s_fprintf(fd, "<H2>Fields from class T_bound :</H2>\n<UL>") ;

  s_fprintf(fd, "<LI>ref_ident = %s\n", ptr_ref(ref_ident)) ;
  s_fprintf(fd, "<LI>value = %s\n", ptr_ref(value)) ;
  s_fprintf(fd, "<LI>is_upper_bound = %s</UL>\n", bool_ref(is_upper_bound)) ;

  // Fermeture du fichier HTML
  close_html(fd) ;

  return NULL ;
}

//
//	}{ Classe T_record
//
FILE *T_record::dump_html(void)
{
  TRACE1("T_record(%x)::dump_html", this) ;

  // Dump superclasse
  FILE *fd = T_expr::dump_html() ;

  s_fprintf(fd, "<H2>Fields from class T_record :</H2>\n<UL>") ;

  s_fprintf(fd, "<LI>first_record_item = %s\n", ptr_ref(first_record_item)) ;
  s_fprintf(fd, "<LI>last_record_item = %s\n</UL>", ptr_ref(last_record_item)) ;

  // Fermeture du fichier HTML
  close_html(fd) ;

  return NULL ;
}

//
//	}{ Classe T_record_item
//
FILE *T_record_item::dump_html(void)
{
  TRACE1("T_record_item(%x)::dump_html", this) ;

  // Dump superclasse
  FILE *fd = T_expr::dump_html() ;

  s_fprintf(fd, "<H2>Fields from class T_record_item :</H2>\n<UL>") ;

  s_fprintf(fd, "<LI>label = %s\n", ptr_ref(label)) ;
  s_fprintf(fd, "<LI>value  = %s\n</UL>", ptr_ref(value)) ;

  // Fermeture du fichier HTML
  close_html(fd) ;

  return NULL ;
}

//
//	}{ Classe T_struct
//
FILE *T_struct::dump_html(void)
{
  TRACE1("T_struct(%x)::dump_html", this) ;

  // Dump superclasse
  FILE *fd = T_expr::dump_html() ;

  s_fprintf(fd, "<H2>Fields from class T_struct :</H2>\n<UL>") ;

  s_fprintf(fd, "<LI>first_record_item = %s\n", ptr_ref(first_record_item)) ;
  s_fprintf(fd, "<LI>last_record_item = %s\n</UL>", ptr_ref(last_record_item)) ;

  // Fermeture du fichier HTML
  close_html(fd) ;

  return NULL ;
}

//
//	}{ Classe T_record_access
//
FILE *T_record_access::dump_html(void)
{
  TRACE1("T_record_access(%x)::dump_html", this) ;

  // Dump superclasse
  FILE *fd = T_expr::dump_html() ;

  s_fprintf(fd, "<H2>Fields from class T_record_access :</H2>\n<UL>") ;

  s_fprintf(fd, "<LI>record  = %s\n", ptr_ref(record)) ;
  s_fprintf(fd, "<LI>label = %s\n</UL>", ptr_ref(label)) ;

  // Fermeture du fichier HTML
  close_html(fd) ;

  return NULL ;
}

//
//	}{	Fin du fichier
//
