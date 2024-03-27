/******************************* CLEARSY **************************************
* Fichier : $Id: c_pragma.cpp,v 2.0 2007-06-04 08:41:33 jburlando Exp $
* (C) 2008 CLEARSY
*
* Description :		Compilateur B
*					Pragmas
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
RCS_ID("$Id: c_pragma.cpp,v 1.19 2007-06-04 08:41:33 jburlando Exp $") ;

/* Includes systeme */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

/* Includes Composant Local */
#include "c_api.h"

//
//	}{	Constructeur de la classe T_pragma
//
T_pragma::T_pragma(const char *new_name,
							T_comment *new_ref_comment,
							T_machine *new_ref_machine,
							T_pragma_lexem *new_pragma_lexem,
							T_pragma **adr_first_local_pragma,
							T_pragma **adr_last_local_pragma,
							T_item **adr_first_global_pragma,
							T_item **adr_last_global_pragma,
							T_item *new_father,
							T_betree *new_betree,
							T_lexem *new_ref_lexem)
  : T_item(NODE_PRAGMA,
		   adr_first_global_pragma,
		   adr_last_global_pragma,
		   new_father,
		   new_betree,
		   new_ref_lexem)
{
  TRACE9("T_pragma(%x)::T_pragma(<%s> local (%x, %x), global(%x, %x), %x, %x, %x",
		 this,
		 new_name,
		 adr_first_local_pragma,
		 adr_last_local_pragma,
		 adr_first_global_pragma,
		 adr_last_global_pragma,
		 new_father,
		 new_betree,
		 new_ref_lexem) ;

  name = lookup_symbol(new_name, strlen(new_name)) ;
  ref_comment = new_ref_comment ;
  first_param = last_param = NULL ;
  ref_machine = new_ref_machine ; // ref consultative

  ref_comment->link() ;

  first_param = NULL ;
  last_param = NULL ;
  translated = FALSE ;

  T_pragma_lexem *cur_lexem = new_pragma_lexem->get_next_lexem() ;

  int done = FALSE ;
  if (cur_lexem == NULL)
	{
	  TRACE0("fin du pragma (pas de params)") ;
	  done = TRUE ;
	}

  if (done == FALSE)
	{
	  if (cur_lexem->get_lex_type() != PL_OPEN_PAREN)
		{
		  // Erreur : pragma sans ident
		  TRACE0("Erreur : pragma xx yy sans '('") ;
		  syntax_error(cur_lexem,
					   CAN_CONTINUE,
					   get_error_msg(E_PRAGMA_WITHOUT_OPEN_PAREN),
					   ref_comment->get_comment()->get_value()) ;
		  return ;
		}

	  T_pragma_lexem *prev_lexem = cur_lexem ;
	  cur_lexem = cur_lexem->get_next_lexem() ;
	  int error = FALSE ;
	  int done = FALSE ;

	  while (done == FALSE)
		{
		  TRACE2("dans la boucle : cur_lexem %x %s",
				 cur_lexem,
				 get_pragma_lex_type_name(cur_lexem->get_lex_type())) ;

		  if ( 	  	 (cur_lexem == NULL)
					 || (     (cur_lexem->get_lex_type() != PL_IDENT)
							  && (cur_lexem->get_lex_type() != PL_STRING) )
					 || (error == TRUE) )
			{
			  TRACE0("fin du pragma au milieu des  params)") ;
              syntax_warning((cur_lexem == NULL) ? prev_lexem->get_ref_comment()->get_ref_lexem() : cur_lexem->get_ref_comment()->get_ref_lexem(),
                           EXTENDED_WARNING,
						   get_error_msg(E_EOF_INSIDE_PRAGMA),
						   ref_comment->get_comment()->get_value()) ;

			  // On ignore tous les parametres
			  first_param = last_param = NULL ;
			  done = TRUE ;
			}

		  if (done == FALSE)
			{
			  TRACE1("nouveau parametre \"%s\"", cur_lexem->get_value()) ;
			  if (cur_lexem->get_lex_type() == PL_IDENT)
				{
				  (void)new T_ident(cur_lexem->get_value(),
									ITYPE_PRAGMA_PARAMETER,
									(T_item **)&first_param,
									(T_item **)&last_param,
									this,
									get_betree(),
									get_ref_lexem()) ;
				}
			  else
				{
				  // PL_STRING
				  TRACE0("on reconnait un PL_STRING") ;
				  ASSERT(cur_lexem->get_lex_type() == PL_STRING) ;
				  (void)new T_literal_string(cur_lexem->get_value(),
											  cur_lexem->get_value_len(),
											  (T_item **)&first_param,
											  (T_item **)&last_param,
											  this,
											  get_betree(),
											  get_ref_lexem()) ;
				}

			  prev_lexem = cur_lexem ;
			  cur_lexem = cur_lexem->get_next_lexem() ;

			  if (cur_lexem != NULL)
				{
				  switch(cur_lexem->get_lex_type())
					{
					case PL_CLOSE_PAREN :
					  {
						TRACE0("fin du pragma (params)") ;
						// Il faut verifier qu'il ne reste rien apres le pragma
						cur_lexem = cur_lexem->get_next_lexem() ;

						if (cur_lexem != NULL)
						  {
							// Erreur il y a des caracteres parasites en
							// fin de pragma
							syntax_error(cur_lexem,
										 CAN_CONTINUE,
										 get_error_msg(E_TRAILING_GARBAGE_AFTER_PRAGMA)) ;
						  }
						done = TRUE ;
						break ;
					  }

					case PL_COMMA :
					  {
						TRACE0("',' : on continue") ;
						prev_lexem = cur_lexem ;
						cur_lexem = cur_lexem->get_next_lexem() ;
						break ;
					  }

					default :
					  {
						// On note l'erreur
						TRACE0("error , on continue") ;
						error = TRUE ;
					  }
					}
				}

			  // encore une fois !
			}

		}
	}

  // Enfin, chainage en queue de la liste globale
  TRACE0("chainage ...") ;
  if (*adr_last_local_pragma == NULL)
	{
	  // Liste vide
	  *adr_first_local_pragma = this ;
	  prev_pragma = NULL ;
	}
  else
	{
	  // Chainage en queue
	  (*adr_last_local_pragma)->next_pragma = this ;
	  prev_pragma = *adr_last_local_pragma ;
	}

  next_pragma = NULL ;
  *adr_last_local_pragma = this ;
}

T_pragma::~T_pragma(void)
{
}

// Pragma NON TRADUIT suivant
T_pragma *T_pragma::get_next_pragma(void)
{
  T_pragma *res = next_pragma ;

  for (;;)
	{
	  if (res == NULL)
		{
		  return res ;
		}
	  if (res->translated == FALSE)
		{
		  return res ;
		}
	  res = res->next_pragma ;
	}
}

// Pragma NON TRADUIT precedant
T_pragma *T_pragma::get_prev_pragma(void)
{
  T_pragma *res = prev_pragma ;

  for (;;)
	{
	  if (res == NULL)
		{
		  return res ;
		}
	  if (res->translated == FALSE)
		{
		  return res ;
		}
	  res = res->prev_pragma ;
	}
}

// Verifier qu'il n'y a pas d'autre pragma local
// Renvoie TRUE/FALSE
int T_pragma::check_no_other_local_pragma(void)
{
  // A FAIRE : avec des messages d'erreur
  ASSERT(next_pragma == NULL) ;

  return TRUE ;
}


// Verifier qu'un pragma n'a pas de parametre
void T_pragma::check_no_param(void)
{
  // A FAIRE : avec des messages d'erreur
  ASSERT(first_param == NULL) ;
}

// Verifier qu'un pragma n'a qu'un seul parametre, et de type string
// Retourne le symbole correspondant
T_symbol *T_pragma::check_single_string_param(void)
{
  // A FAIRE : avec des messages d'erreur
  ASSERT(first_param->get_next() == NULL) ;
  ASSERT(first_param->get_node_type() == NODE_LITERAL_STRING) ;
  return ((T_literal_string *)first_param)->get_value() ;
}

// Verifier qu'un pragma n'a qu'un seul parametre, et de type string
// Retourne les symboles correspondant
void T_pragma::check_two_string_params(T_symbol **adr_p1,
												T_symbol **adr_p2)
{
  // A FAIRE : avec des messages d'erreur
  ASSERT(first_param->get_next() != NULL) ;
  ASSERT(first_param->get_next()->get_next() == NULL) ;
  ASSERT(first_param->get_node_type() == NODE_LITERAL_STRING) ;
  ASSERT(first_param->get_next()->get_node_type() == NODE_LITERAL_STRING) ;
  if (adr_p1 != NULL)
	{
	  *adr_p1 = ((T_literal_string *)first_param)->get_value() ;
	}
  if (adr_p2 != NULL)
	{
	  *adr_p2 = ((T_literal_string *)first_param->get_next())->get_value() ;
	}
}


//
// Fin du fichier c_pragma.cpp
//
