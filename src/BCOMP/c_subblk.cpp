/******************************* CLEARSY **************************************
* Fichier : $Id: c_subblk.cpp,v 2.0 2000-06-29 13:00:19 fl Exp $
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
RCS_ID("$Id: c_subblk.cpp,v 1.16 2000-06-29 13:00:19 fl Exp $") ;

/* Includes systeme */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

/* Includes Composant Local */
#include "c_api.h"

//
//	}{	Classe T_skip
//
// Constructeur de la classe T_skip
T_skip::T_skip(T_item **adr_first,
						T_item **adr_last,
						T_item *new_father,
						T_betree *new_betree,
						T_lexem **adr_ref_lexem)
: T_substitution(NODE_SKIP,
				 adr_first,
				 adr_last,
				 new_father,
				 new_betree,
				 adr_ref_lexem)
{
TRACE5("T_skip::T_skip(%x, %x, %x, %x, %x)",
	   adr_first,
	   adr_last,
	   new_father,
	   new_betree,
	   adr_ref_lexem) ;

 if (adr_ref_lexem != NULL) // peut etre NULL si appele depuis l'exterieur
  {
	*adr_ref_lexem = (*adr_ref_lexem)->get_next_lexem() ;
  }
}

//
//	Destructeur
//
T_skip::~T_skip(void)
{
TRACE1("T_skip::~T_skip(%x)", this) ;
}

//
//	}{	Classe T_begin
//
// Constructeur de la classe T_begin
T_begin::T_begin(T_item **adr_first,
						  T_item **adr_last,
						  T_item *new_father,
						  T_betree *new_betree,
						  T_lexem **adr_ref_lexem)
: T_substitution(NODE_BEGIN,
				 adr_first,
				 adr_last,
				 new_father,
				 new_betree,
				 adr_ref_lexem)
{
TRACE5("T_begin::T_begin(%x, %x, %x, %x, %x)",
	   adr_first,
	   adr_last,
	   new_father,
	   new_betree,
	   adr_ref_lexem) ;
syntax_analysis(adr_first, adr_last, new_father, new_betree, adr_ref_lexem) ;
}

//
//	Destructeur
//
T_begin::~T_begin(void)
{
  TRACE1("--> T_begin::~T_begin(%x)", this) ;
}

//jfm modif pour bug 2247
//rend true si la substitution ou l'operation ne mofifie pas sa
//machine, FALSE sinon
int T_begin::is_read_only(void)
{
  TRACE0("T_begin::is_read_only");
  // ici il faut verifier que toutes les substitutions du corps
  // sont bien read_only
  T_substitution *cur = get_body();

  while ( cur != NULL )
	{
	  if ( cur->is_read_only() == FALSE )
		{
		  return FALSE ;
		}
	  //ok par construction
	  cur =(T_substitution*) cur->get_next();
	}
  return TRUE ;
}

//
//	}{	Classe T_choice
//
// Constructeur de la classe T_choice
T_choice::T_choice(T_item **adr_first,
						  T_item **adr_last,
						  T_item *new_father,
						  T_betree *new_betree,
						  T_lexem **adr_ref_lexem)
: T_substitution(NODE_CHOICE,
				 adr_first,
				 adr_last,
				 new_father,
				 new_betree,
				 adr_ref_lexem)
{
TRACE5("T_choice::T_choice(%x, %x, %x, %x, %x)",
	   adr_first,
	   adr_last,
	   new_father,
	   new_betree,
	   adr_ref_lexem) ;
syntax_analysis(adr_first, adr_last, new_father, new_betree, adr_ref_lexem) ;
}

// Methode qui essaie de trouver un end qui manque
void T_choice::check_for_missing_end(void)
{
TRACE1("T_choice(%x)::check_for_missing_end", this) ;
T_lexem *choice = get_ref_lexem() ;
T_lexem *end = (T_lexem *)get_tmp() ;

TRACE2("choice = %x, end = %x", choice, end) ;

if ( 	(choice == NULL)
	 || (end == NULL)
	 || (object_test(end) == FALSE)
	 || (end->get_node_type() != NODE_LEXEM)
	 || (end->get_lex_type() != L_END) )
	{
	TRACE0("T_choice incoherent - j'abandonne") ;
	return ;
	}

if (choice->get_file_column() != end->get_file_column())
	{
	TRACE0("avant") ;
	end_warning("CHOICE", choice, end) ;
	TRACE0("apres") ;
	}
}

//
//	}{	Destructeur
//
T_choice::~T_choice(void)
{
TRACE1("T_choice::~T_choice(%x)", this) ;
list_unlink(first_body) ;
list_unlink(first_or) ;
object_unlink(end_choice) ;
}

//
//	}{	Classe T_or
//
// Constructeur de la classe T_or
T_or::T_or(T_item **adr_first,
					T_item **adr_last,
					T_item *new_father,
					T_betree *new_betree,
					T_lexem **adr_ref_lexem)
: T_item(NODE_OR,
		 adr_first,
		 adr_last,
		 new_father,
		 new_betree,
		 *adr_ref_lexem)
{
TRACE5("T_or::T_or(%x, %x, %x, %x, %x)",
	   adr_first,
	   adr_last,
	   new_father,
	   new_betree,
	   adr_ref_lexem) ;
syntax_analysis(adr_first, adr_last, new_father, new_betree, adr_ref_lexem) ;
TRACE0("fin de T_or::T_or") ;
}

//
//	}{	Destructeur
//
T_or::~T_or(void)
{
TRACE1("T_or::~T_or(%x)", this) ;
list_unlink(first_body) ;
}

//
//	}{	Classe T_precond
//
// Constructeur de la classe T_precond
T_precond::T_precond(T_item **adr_first,
						  T_item **adr_last,
						  T_item *new_father,
						  T_betree *new_betree,
						  T_lexem **adr_ref_lexem)
: T_substitution(NODE_PRECOND,
				 adr_first,
				 adr_last,
				 new_father,
				 new_betree,
				 adr_ref_lexem)
{
TRACE5("T_precond::T_precond(%x, %x, %x, %x, %x)",
	   adr_first,
	   adr_last,
	   new_father,
	   new_betree,
	   adr_ref_lexem) ;
syntax_analysis(adr_first, adr_last, new_father, new_betree, adr_ref_lexem) ;
}

//
//	}{	Destructeur
//
T_precond::~T_precond(void)
{
TRACE1("T_precond:~T_precond(%x)", this) ;
}

//
// }{	Constructeur de la classe T_assert
//
T_assert::T_assert(T_item **adr_first,
							T_item **adr_last,
							T_item *new_father,
							T_betree *new_betree,
							T_lexem **adr_ref_lexem)
  : T_substitution(NODE_ASSERT,
				   adr_first,
				   adr_last,
				   new_father,
				   new_betree,
				   adr_ref_lexem)
{
  TRACE5("T_assert::T_assert(%x, %x, %x, %x, %x)",
		 adr_first,
		 adr_last,
		 new_father,
		 new_betree,
		 adr_ref_lexem) ;

  syntax_analysis(adr_first, adr_last, new_father, new_betree, adr_ref_lexem) ;
}

//
//	Destructeur
//
T_assert::~T_assert(void)
{
}

//jfm modif pour bug 2247
//rend true si la substitution ou l'operation ne mofifie pas sa
//machine, FALSE sinon
int T_assert::is_read_only(void)
{
  // ici il faut verifier que toutes les substitutions du corps
  // sont bien read_only
  TRACE0("T_assert::is_read_only");
  T_substitution *cur = get_body();

  while ( cur != NULL )
	{
	  if ( cur->is_read_only() == FALSE )
		{
		  return FALSE ;
		}
	  //ok par construction
	  cur =(T_substitution*) cur->get_next();
	}
  return TRUE ;
}


//
// }{	Constructeur de la classe T_label
//
T_label::T_label(T_item **adr_first,
                                                        T_item **adr_last,
                                                        T_item *new_father,
                                                        T_betree *new_betree,
                                                        T_lexem **adr_ref_lexem)
  : T_substitution(NODE_LABEL,
                                   adr_first,
                                   adr_last,
                                   new_father,
                                   new_betree,
                                   adr_ref_lexem)
{
  TRACE5("T_label::T_label(%x, %x, %x, %x, %x)",
                 adr_first,
                 adr_last,
                 new_father,
                 new_betree,
                 adr_ref_lexem) ;

  syntax_analysis(adr_first, adr_last, new_father, new_betree, adr_ref_lexem) ;
}

//
//	Destructeur
//
T_label::~T_label(void)
{
}

//jfm modif pour bug 2247
//rend true si la substitution ou l'operation ne mofifie pas sa
//machine, FALSE sinon
int T_label::is_read_only(void)
{
  // ici il faut verifier que toutes les substitutions du corps
  // sont bien read_only
  TRACE0("T_assert::is_read_only");
  T_substitution *cur = get_body();

  while ( cur != NULL )
        {
          if ( cur->is_read_only() == FALSE )
                {
                  return FALSE ;
                }
          //ok par construction
          cur =(T_substitution*) cur->get_next();
        }
  return TRUE ;
}


//
// }{	Constructeur de la classe T_jumpif
//
T_jumpif::T_jumpif(T_item **adr_first,
                                                        T_item **adr_last,
                                                        T_item *new_father,
                                                        T_betree *new_betree,
                                                        T_lexem **adr_ref_lexem)
  : T_substitution(NODE_JUMPIF,
                                   adr_first,
                                   adr_last,
                                   new_father,
                                   new_betree,
                                   adr_ref_lexem)
{
  TRACE5("T_jumpif::T_jumpif(%x, %x, %x, %x, %x)",
                 adr_first,
                 adr_last,
                 new_father,
                 new_betree,
                 adr_ref_lexem) ;

  syntax_analysis(adr_first, adr_last, new_father, new_betree, adr_ref_lexem) ;
}

//
//	Destructeur
//
T_jumpif::~T_jumpif(void)
{
}

int T_jumpif::is_read_only(void)
{
  return TRUE ;
}

//
// }{	Constructeur de la classe T_witness
//
T_witness::T_witness(T_item **adr_first,
							T_item **adr_last,
							T_item *new_father,
							T_betree *new_betree,
							T_lexem **adr_ref_lexem)
  : T_substitution(NODE_WITNESS,
				   adr_first,
				   adr_last,
				   new_father,
				   new_betree,
				   adr_ref_lexem)
{
  TRACE5("T_witness::T_witness(%x, %x, %x, %x, %x)",
		 adr_first,
		 adr_last,
		 new_father,
		 new_betree,
		 adr_ref_lexem) ;

  syntax_analysis(adr_first, adr_last, new_father, new_betree, adr_ref_lexem) ;
}

//
//	Destructeur
//
T_witness::~T_witness(void)
{
}

//jfm modif pour bug 2247
//rend true si la substitution ou l'operation ne mofifie pas sa
//machine, FALSE sinon
int T_witness::is_read_only(void)
{
  // ici il faut verifier que toutes les substitutions du corps
  // sont bien read_only
  TRACE0("T_witness::is_read_only");
  T_substitution *cur = get_body();

  while ( cur != NULL )
	{
	  if ( cur->is_read_only() == FALSE )
		{
		  return FALSE ;
		}
	  //ok par construction
	  cur =(T_substitution*) cur->get_next();
	}
  return TRUE ;
}

//
//	}{	Constructeur de la classe T_if
//
T_if::T_if(T_item **adr_first,
					T_item **adr_last,
					T_item *new_father,
					T_betree *new_betree,
					T_lexem **adr_ref_lexem)
: T_substitution(NODE_IF,
				 adr_first,
				 adr_last,
				 new_father,
				 new_betree,
				 adr_ref_lexem)
{
TRACE5("T_if::T_if(%x, %x, %x, %x, %x)",
	   adr_first,
	   adr_last,
	   new_father,
	   new_betree,
	   adr_ref_lexem) ;
syntax_analysis(adr_first, adr_last, new_father, new_betree, adr_ref_lexem) ;


}

//
//	Destructeur
//
T_if::~T_if(void)
{
TRACE1("T_if::~T_if(%x)", this) ;
object_unlink(cond) ;
//object_unlink(label) ;
list_unlink(first_then_body) ;
list_unlink(first_else) ;
object_unlink(end_if) ;
}

//
//	}{	Classe T_else
//
// Constructeur de la classe T_else avec parsing
T_else::T_else(int parse_cond,
						T_item **adr_first,
						T_item **adr_last,
						T_item *new_father,
						T_betree *new_betree,
						T_lexem **adr_ref_lexem,
						T_lexem *else_ref_lexem)
: T_item(NODE_ELSE,
		 adr_first,
		 adr_last,
		 new_father,
		 new_betree,
		 else_ref_lexem)
{
TRACE6("T_else::T_else(%s,%x, %x, %x, %x, %x)",
	   (parse_cond == TRUE) ? "TRUE" : "FALSE",
	   adr_first,
	   adr_last,
	   new_father,
	   new_betree,
	   adr_ref_lexem) ;
syntax_analysis(parse_cond,
				adr_first,
				adr_last,
				new_father,
				new_betree,
				adr_ref_lexem) ;
}

// Constructeur de la classe T_else sans parsing
T_else::T_else(T_substitution *new_first_body,
						T_substitution *new_last_body,
						T_predicate *new_cond,
						T_item **adr_first,
						T_item **adr_last,
						T_item *new_father,
						T_betree *new_betree,
						T_lexem **adr_ref_lexem,
						T_lexem *else_ref_lexem)
: T_item(NODE_ELSE,
		 adr_first,
		 adr_last,
		 new_father,
		 new_betree,
		 else_ref_lexem)
{
TRACE8("T_else::T_else(%x,%x,%x,%x, %x, %x, %x, %x)",
	   new_first_body,
	   new_last_body,
	   new_cond,
	   adr_first,
	   adr_last,
	   new_father,
	   new_betree,
	   adr_ref_lexem) ;

first_body = new_first_body ;
last_body = new_last_body ;

T_item *cur = first_body ;
while (cur != NULL)
  {
	cur->set_father(this) ;
	cur = cur->get_next() ;
  }

  cond = new_cond ;
  if (cond != NULL)
	{
	  cond->set_father(this) ;
	}
}

//
//	Destructeur
//
T_else::~T_else(void)
{
TRACE1("T_else::~T_else(%x)", this) ;
object_unlink(cond) ;
list_unlink(first_body) ;
}

//	}{	Classe T_select
//
// Constructeur de la classe T_select
T_select::T_select(T_item **adr_first,
					T_item **adr_last,
					T_item *new_father,
					T_betree *new_betree,
					T_lexem **adr_ref_lexem)
: T_substitution(NODE_SELECT,
				 adr_first,
				 adr_last,
				 new_father,
				 new_betree,
				 adr_ref_lexem)
{
TRACE5("T_select::T_select(%x, %x, %x, %x, %x)",
	   adr_first,
	   adr_last,
	   new_father,
	   new_betree,
	   adr_ref_lexem) ;
syntax_analysis(adr_first, adr_last, new_father, new_betree, adr_ref_lexem) ;
}

//
//	Destructeur
//
T_select::~T_select(void)
{
TRACE1("T_select::~T_select(%x)", this) ;
object_unlink(cond) ;
list_unlink(first_then_body) ;
list_unlink(first_when) ;
object_unlink(end_select) ;
}

//
//	}{	Classe T_when
//
// Constructeur de la classe T_when
T_when::T_when(int parse_cond,
						T_item **adr_first,
						T_item **adr_last,
						T_item *new_father,
						T_betree *new_betree,
						T_lexem **adr_ref_lexem)
: T_item(NODE_WHEN,
		 adr_first,
		 adr_last,
		 new_father,
		 new_betree,
		 *adr_ref_lexem)
{
TRACE6("T_when::T_when(%s,%x, %x, %x, %x, %x)",
	   (parse_cond == TRUE) ? "TRUE" : "FALSE",
	   adr_first,
	   adr_last,
	   new_father,
	   new_betree,
	   adr_ref_lexem) ;
syntax_analysis(parse_cond,
				adr_first,
				adr_last,
				new_father,
				new_betree,
				adr_ref_lexem) ;
}

//
//	Destructeur
//
T_when::~T_when(void)
{
TRACE1("T_when::~T_when(%x)", this) ;
object_unlink(cond) ;
list_unlink(first_body) ;
}

//
//	}{	Classe T_case
//
// Constructeur de la classe T_case
T_case::T_case(T_item **adr_first,
					T_item **adr_last,
					T_item *new_father,
					T_betree *new_betree,
					T_lexem **adr_ref_lexem)
: T_substitution(NODE_CASE,
				 adr_first,
				 adr_last,
				 new_father,
				 new_betree,
				 adr_ref_lexem)
{
TRACE5("T_case(%x)::T_case(%x, %x, %x, %x, %x)",
	   adr_first,
	   adr_last,
	   new_father,
	   new_betree,
	   adr_ref_lexem) ;
syntax_analysis(adr_first, adr_last, new_father, new_betree, adr_ref_lexem) ;


}

//
//	Destructeur
//
T_case::~T_case(void)
{
  TRACE1("T_case::~T_case(%x)", this) ;
  object_unlink(case_select) ;
  list_unlink(first_case_item) ;
  object_unlink(end_either) ;
  object_unlink(end_case) ;
}

//jfm modif pour bug 2247
//rend true si la substitution ou l'operation ne mofifie pas sa
//machine, FALSE sinon
int T_case::is_read_only(void)
{
  // ici il faut verifier que toutes les substitutions du corps
  // sont bien read_only (les substitutions de tous les cas)
  TRACE0("T_case::is_read_only");
  T_case_item *cur_item = get_case_items();
  while ( cur_item != NULL )
	{
	  T_substitution *cur_sub = cur_item->get_body();
	  while ( cur_sub != NULL )
		{
		  if ( cur_sub->is_read_only() == FALSE )
			{
		  return FALSE ;
			}
		  //ok par construction
		  cur_sub =(T_substitution*) cur_sub->get_next();
		}
	  //ok par construction
	  cur_item =(T_case_item*) cur_item->get_next();
	}
  return TRUE ;
}

//
//	}{	Classe T_case_item
//
// Constructeur de la classe T_case_item
T_case_item::T_case_item(T_item **adr_first,
								  T_item **adr_last,
								  T_item *new_father,
								  T_betree *new_betree,
								  T_lexem **adr_ref_lexem)
: T_item(NODE_CASE_ITEM,
		 adr_first,
		 adr_last,
		 new_father,
		 new_betree,
		 *adr_ref_lexem)
{
TRACE5("T_case_item::T_case_item(%x, %x, %x, %x, %x)",
	   adr_first,
	   adr_last,
	   new_father,
	   new_betree,
	   adr_ref_lexem) ;
syntax_analysis(adr_first,
				adr_last,
				new_father,
				new_betree,
				adr_ref_lexem) ;
}

// Constructeur public
T_case_item::T_case_item(T_item *new_first_literal_value,
								  T_item *new_last_literal_value,
								  T_substitution *new_first_body,
								  T_substitution *new_last_body,
								  T_item **adr_first,
								  T_item **adr_last,
								  T_item *new_father,
								  T_betree *new_betree)
: T_item(NODE_CASE_ITEM,
		 adr_first,
		 adr_last,
		 new_father,
		 new_betree,
		 NULL)
{
  TRACE5("T_case_item(%x)::T_case_item(%x,%x,%x,%x)",
		 this,
		 new_first_literal_value,
		 new_last_literal_value,
		 new_first_body,
		 new_last_body) ;

  first_literal_value = new_first_literal_value ;
  last_literal_value = new_last_literal_value ;

  first_body = new_first_body ;
  last_body = new_last_body ;

  // Correction des pere
  T_item *cur = first_literal_value ;

  while (cur != NULL)
	{
	  cur->set_father(this) ;
	  cur = cur->get_next() ;
	}

  cur = first_body ;

  while (cur != NULL)
	{
	  cur->set_father(this) ;
	  cur = cur->get_next() ;
	}
}

//
//	Destructeur
//
T_case_item::~T_case_item(void)
{
TRACE1("T_case_item::~T_case_item(%x)", this) ;
list_unlink(first_literal_value) ;
list_unlink(first_body) ;
}


//
//	}{	Classe T_let
//
// Constructeur de la classe T_let
T_let::T_let(T_item **adr_first,
					T_item **adr_last,
					T_item *new_father,
					T_betree *new_betree,
					T_lexem **adr_ref_lexem)
: T_substitution(NODE_LET,
				 adr_first,
				 adr_last,
				 new_father,
				 new_betree,
				 adr_ref_lexem)
{
TRACE5("T_let::T_let(%x, %x, %x, %x, %x)",
	   adr_first,
	   adr_last,
	   new_father,
	   new_betree,
	   adr_ref_lexem) ;
syntax_analysis(adr_first, adr_last, new_father, new_betree, adr_ref_lexem) ;
}

//
//	Destructeur
//
T_let::~T_let(void)
{
}

//
//	}{	Classe T_var
//
// Constructeur de la classe T_var
T_var::T_var(T_item **adr_first,
					  T_item **adr_last,
					  T_item *new_father,
					  T_betree *new_betree,
					  T_lexem **adr_ref_lexem)
: T_substitution(NODE_VAR,
				 adr_first,
				 adr_last,
				 new_father,
				 new_betree,
				 adr_ref_lexem)
{
TRACE5("T_var::T_var(%x, %x, %x, %x, %x)",
	   adr_first,
	   adr_last,
	   new_father,
	   new_betree,
	   adr_ref_lexem) ;
syntax_analysis(adr_first, adr_last, new_father, new_betree, adr_ref_lexem) ;
}

//
//	Destructeur
//
T_var::~T_var(void)
{
TRACE1("T_var::~T_var(%x)", this) ;
}

//
//	}{	Classe T_while
//
// Constructeur de la classe T_while
T_while::T_while(T_item **adr_first,
					  T_item **adr_last,
					  T_item *new_father,
					  T_betree *new_betree,
					  T_lexem **adr_ref_lexem)
: T_substitution(NODE_WHILE,
				 adr_first,
				 adr_last,
				 new_father,
				 new_betree,
				 adr_ref_lexem)
{
TRACE5("T_while::T_while(%x, %x, %x, %x, %x)",
	   adr_first,
	   adr_last,
	   new_father,
	   new_betree,
	   adr_ref_lexem) ;
syntax_analysis(adr_first, adr_last, new_father, new_betree, adr_ref_lexem) ;

}

//
//	Destructeur
//
T_while::~T_while(void)
{
TRACE1("T_while::~T_while(%x)", this) ;
object_unlink(cond) ;
list_unlink(first_body) ;
object_unlink(invariant_keyword) ;
object_unlink(invariant) ;
object_unlink(variant_keyword) ;
object_unlink(variant) ;
object_unlink(end_while) ;
}

//
//	}{	Classe T_any
//
// Constructeur de la classe T_any
T_any::T_any(T_item **adr_first,
					  T_item **adr_last,
					  T_item *new_father,
					  T_betree *new_betree,
					  T_lexem **adr_ref_lexem)
: T_substitution(NODE_ANY,
				 adr_first,
				 adr_last,
				 new_father,
				 new_betree,
				 adr_ref_lexem)
{
TRACE5("T_any::T_any(%x, %x, %x, %x, %x)",
	   adr_first,
	   adr_last,
	   new_father,
	   new_betree,
	   adr_ref_lexem) ;
syntax_analysis(adr_first, adr_last, new_father, new_betree, adr_ref_lexem) ;
}

//
//	Destructeur
//
T_any::~T_any(void)
{
  TRACE1(">> T_any::~T_any(%x)", this) ;
  ENTER_TRACE ;

  TRACE1("libe first_ident %x", first_ident) ;
  list_unlink(first_ident) ;

  TRACE1("libe where %x", where) ;
  object_unlink(where) ;

  TRACE1("libe first_body %x", first_body) ;
  list_unlink(first_body) ;

  TRACE1("libe end_any %x", end_any) ;
  object_unlink(end_any) ;

  EXIT_TRACE ;
  TRACE1("<< T_any::~T_any(%x)", this) ;
}

//jfm modif pour bug 2247
//rend true si la substitution ou l'operation ne mofifie pas sa
//machine, FALSE sinon
int T_any::is_read_only(void)
{
  TRACE0("T_any::is_read_only");
  // ici il faut verifier que toutes les substitutions du corps
  // sont bien read_only
  T_substitution *cur = get_body();

  while ( cur != NULL )
	{
	  if ( cur->is_read_only() == FALSE )
		{
		  return FALSE ;
		}
	  //ok par construction
	  cur =(T_substitution*) cur->get_next();
	}
  return TRUE ;
}

//
//	}{	Fin du fichier
//

