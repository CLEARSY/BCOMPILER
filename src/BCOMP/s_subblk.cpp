/******************************* CLEARSY **************************************
* Fichier : $Id: s_subblk.cpp,v 2.0 2004-01-08 09:33:02 cm Exp $
* (C) 2008 CLEARSY
*
* Description :		Compilateur B
*					Parsing des substitutions de type "block"
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
RCS_ID("$Id: s_subblk.cpp,v 1.19 2004-01-08 09:33:02 cm Exp $") ;

/* Includes systeme */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

/* Includes Composant Local */
#include "c_api.h"
#include "s_synta.h"
#include "s_lr.h"

//
//	}{	Classe T_begin
//
// Analyse Syntaxique de la classe T_begin
void T_begin::syntax_analysis(T_item **adr_first,
									   T_item **adr_last,
									   T_item *new_father,
									   T_betree *new_betree,
									   T_lexem **adr_ref_lexem)
{
  TRACE5("T_begin::syntax_analysis(%x, %x, %x, %x, %x)",
		 adr_first,
		 adr_last,
		 new_father,
		 new_betree,
		 adr_ref_lexem) ;

  // On cherche la substitution (apres avoir saute le BEGIN)
  T_lexem *cur_lexem = (*adr_ref_lexem)->get_next_lexem() ;
  first_body = last_body = NULL ;
  T_substitution *body = syntax_get_substitution((T_item **)&first_body,
												 (T_item **)&last_body,
												 this,
												 get_betree(),
												 &cur_lexem) ;

  if (body == NULL)
	{
	  // A FAIRE ...
	  assert(FALSE) ;
	}

  // On verifie le end

  if ((cur_lexem == NULL) || (cur_lexem->get_lex_type() != L_END))
	{
	  syntax_unexpected_error(cur_lexem, FATAL_ERROR, get_catalog(C_END)) ;
	}

  end_begin = new T_flag(this, new_betree, cur_lexem) ;

  //TRACE0("ok fin de new T_begin") ;
  *adr_ref_lexem = cur_lexem->get_next_lexem() ;
}

//
//	}{	Classe T_choice
//
// Analyse Syntaxique de la classe T_choice
void T_choice::syntax_analysis(T_item **adr_first,
										T_item **adr_last,
										T_item *new_father,
										T_betree *new_betree,
										T_lexem **adr_ref_lexem)
{
  TRACE5("T_choice::syntax_analysis(%x, %x, %x, %x, %x)",
		 adr_first,
		 adr_last,
		 new_father,
		 new_betree,
		 adr_ref_lexem) ;

  // On cherche la substitution
  T_lexem *cur_lexem = (*adr_ref_lexem)->get_next_lexem() ;
  syntax_check_eof(cur_lexem) ;

  T_substitution *body = syntax_get_substitution((T_item **)&first_body,
												 (T_item **)&last_body,
												 this,
												 get_betree(),
												 &cur_lexem) ;

  if (body == NULL)
	{
	  // A FAIRE ...
	  assert(FALSE) ;
	}

  first_or = last_or = NULL ;

  // Maintenant on a : soit des OR, soit des END
  for (;;)
	{
	  TRACE2("ici cur_lexem = %x type %d",
			 cur_lexem, (cur_lexem == NULL) ? -1 : cur_lexem->get_lex_type()) ;

	  if (cur_lexem == NULL)
		{
		  syntax_unexpected_error(cur_lexem, FATAL_ERROR, get_catalog(C_OR_END)) ;
		}

	  T_lex_type lex_type = cur_lexem->get_lex_type() ;

	  switch(lex_type)
		{
		case L_END :
		  {
			TRACE0("ok fin de new T_choice") ;
			end_choice = new T_flag(this, get_betree(), cur_lexem) ;
			*adr_ref_lexem = cur_lexem->get_next_lexem() ;

			return ;
		  }

		case L_OR :
		  {
			TRACE0("Ok , un OR") ;
			(void)new T_or((T_item **)&first_or,
						   (T_item **)&last_or,
						   this,
						   get_betree(),
						   &cur_lexem) ;
			break ;
		  }

		default :
		  {
			syntax_unexpected_error(cur_lexem, FATAL_ERROR, get_catalog(C_OR_END)) ;
		  }
		}
	}
}

//
//	}{	Classe T_or
//
// Analyse Syntaxique de la classe T_or
void T_or::syntax_analysis(T_item **adr_first,
									T_item **adr_last,
									T_item *new_father,
									T_betree *new_betree,
									T_lexem **adr_ref_lexem)
{
  TRACE5("T_or::syntax_analysis(%x, %x, %x, %x, %x)",
		 adr_first,
		 adr_last,
		 new_father,
		 new_betree,
		 adr_ref_lexem) ;
  ENTER_TRACE ;

  // On saute le 'OR'
  *adr_ref_lexem = (*adr_ref_lexem)->get_next_lexem() ;

  // On cherche la substitution
  T_substitution *body = syntax_get_substitution((T_item **)&first_body,
												 (T_item **)&last_body,
												 this,
												 get_betree(),
												 adr_ref_lexem) ;

  if (body == NULL)
	{
	  // A FAIRE ...
	  assert(FALSE) ;
	}

  // C'est tout !
  EXIT_TRACE ;
  TRACE0("fin de T_or::T_syntax_analysis") ;
}

//
//	}{	Classe T_precond
//
// Analyse Syntaxique de la classe T_precond
void T_precond::syntax_analysis(T_item **adr_first,
										 T_item **adr_last,
										 T_item *new_father,
										 T_betree *new_betree,
										 T_lexem **adr_ref_lexem)
{
  TRACE5("T_precond::syntax_analysis(%x, %x, %x, %x, %x)",
		 adr_first,
		 adr_last,
		 new_father,
		 new_betree,
		 adr_ref_lexem) ;

  if (new_father && !new_father->is_an_operation())
  {
    if (get_allow_pre_in_substitution() == TRUE)
    {
      // on émet un avertissement malgré tout
      syntax_warning(*adr_ref_lexem, BASE_WARNING, get_catalog(C_PRECOND)) ;
    }
    else
    {
      syntax_unexpected_error(*adr_ref_lexem, FATAL_ERROR, get_catalog(C_PRECOND)) ;
    }
  }

  // On cherche le predicat apres avoir saute le "PRE"
  T_lexem *cur_lexem = (*adr_ref_lexem)->get_next_lexem() ;

  predicate = syntax_get_predicate(NULL,
								   NULL,
								   this,
								   get_betree(),
								   &cur_lexem) ;

#if 0 // A RAJOUTER APRES
  if (predicate == NULL)
	{
	  // A FAIRE ...
	  TRACE1("pb predicate = NULL") ;
	  assert(FALSE) ;
	}
#endif

  // On doit avoir un THEN
  if ((cur_lexem == NULL) || (cur_lexem->get_lex_type() != L_THEN))
	{
	  syntax_unexpected_error(cur_lexem, FATAL_ERROR, get_catalog(C_THEN)) ;
	}

  then_keyword = new T_flag(this, get_betree(), cur_lexem) ;
  cur_lexem = cur_lexem->get_next_lexem() ;

  first_body = last_body = NULL ;
  T_substitution *body = syntax_get_substitution((T_item **)&first_body,
												 (T_item **)&last_body,
												 this,
												 get_betree(),
												 &cur_lexem) ;

  if (body == NULL)
	{
	  // A FAIRE
	  assert(FALSE) ;
	}

  if ((cur_lexem == NULL) || (cur_lexem->get_lex_type() != L_END))
	{
	  syntax_unexpected_error(cur_lexem, FATAL_ERROR, get_catalog(C_END)) ;
	}

  TRACE0("ok fin de new T_precond") ;
  *adr_ref_lexem = cur_lexem->get_next_lexem() ;

  end_precond = new T_flag(this, get_betree(), cur_lexem) ;

  return ;
}

//
// }{	Analyse Syntaxique de la classe T_assert
//
void T_assert::syntax_analysis(T_item **adr_first,
										T_item **adr_last,
										T_item *new_father,
										T_betree *new_betree,
										T_lexem **adr_ref_lexem)
{
  TRACE5("T_assert::syntax_analysis(%x, %x, %x, %x, %x)",
		 adr_first,
		 adr_last,
		 new_father,
		 new_betree,
		 adr_ref_lexem) ;

  // On cherche le predicate apres avoir saute le "ASSERT"
  T_lexem *cur_lexem = (*adr_ref_lexem)->get_next_lexem() ;
  syntax_check_eof(cur_lexem) ;

  predicate = syntax_get_predicate(NULL,
								   NULL,
								   this,
								   get_betree(),
								   &cur_lexem) ;

#if 0 // A RAJOUTER APRES
  if (predicate == NULL)
	{
	  // A FAIRE ...
	  TRACE1("pb predicate = NULL") ;
	  assert(FALSE) ;
	}
#endif

  // On doit avoir un THEN
  if ((cur_lexem == NULL) || (cur_lexem->get_lex_type() != L_THEN))
	{
	  syntax_unexpected_error(cur_lexem, FATAL_ERROR, get_catalog(C_THEN)) ;
	}

  then = new T_flag(this, get_betree(), cur_lexem) ;

  cur_lexem = cur_lexem->get_next_lexem() ;
  syntax_check_eof(cur_lexem) ;

  first_body = last_body = NULL ;

  T_substitution *body = syntax_get_substitution((T_item **)&first_body,
												 (T_item **)&last_body,
												 then,// utilise pour FROM_ASSERT...
												 get_betree(),
												 &cur_lexem) ;

  if (body == NULL)
	{
	  // A FAIRE
	  assert(FALSE) ;
	}

  if ((cur_lexem == NULL) || (cur_lexem->get_lex_type() != L_END))
	{
	  syntax_unexpected_error(cur_lexem, FATAL_ERROR, get_catalog(C_END)) ;
	}

  TRACE0("ok fin de new T_assert") ;
  *adr_ref_lexem = cur_lexem->get_next_lexem() ;

  end_assert = new T_flag(this, get_betree(), cur_lexem) ;
  return ;
}

//
// }{	Analyse Syntaxique de la classe T_label
//
void T_label::syntax_analysis(T_item **adr_first,
                                                                                T_item **adr_last,
                                                                                T_item *new_father,
                                                                                T_betree *new_betree,
                                                                                T_lexem **adr_ref_lexem)
{
  TRACE5("T_label::syntax_analysis(%x, %x, %x, %x, %x)",
                 adr_first,
                 adr_last,
                 new_father,
                 new_betree,
                 adr_ref_lexem) ;

  // On cherche l'expression apres avoir saute le "LABEL"
  T_lexem *cur_lexem = (*adr_ref_lexem)->get_next_lexem() ;
  syntax_check_eof(cur_lexem) ;

  expression = syntax_get_expr(NULL,
                                                                   NULL,
                                                                   this,
                                                                   get_betree(),
                                                                   &cur_lexem) ;

#if 0 // A RAJOUTER APRES
  if (predicate == NULL)
        {
          // A FAIRE ...
          TRACE1("pb predicate = NULL") ;
          assert(FALSE) ;
        }
#endif

  // On doit avoir un THEN
  if ((cur_lexem == NULL) || (cur_lexem->get_lex_type() != L_THEN))
        {
          syntax_unexpected_error(cur_lexem, FATAL_ERROR, get_catalog(C_THEN)) ;
        }

  then = new T_flag(this, get_betree(), cur_lexem) ;

  cur_lexem = cur_lexem->get_next_lexem() ;
  syntax_check_eof(cur_lexem) ;

  first_body = last_body = NULL ;

  T_substitution *body = syntax_get_substitution((T_item **)&first_body,
                                                                                                 (T_item **)&last_body,
                                                                                                 then,// utilise pour FROM_ASSERT...
                                                                                                 get_betree(),
                                                                                                 &cur_lexem) ;

  if (body == NULL)
        {
          // A FAIRE
          assert(FALSE) ;
        }

  if ((cur_lexem == NULL) || (cur_lexem->get_lex_type() != L_END))
        {
          syntax_unexpected_error(cur_lexem, FATAL_ERROR, get_catalog(C_END)) ;
        }

  TRACE0("ok fin de new T_label") ;
  *adr_ref_lexem = cur_lexem->get_next_lexem() ;

  end_assert = new T_flag(this, get_betree(), cur_lexem) ;
  return ;
}

//
// }{	Analyse Syntaxique de la classe T_jumpif
//
void T_jumpif::syntax_analysis(T_item **adr_first,
                                                                                T_item **adr_last,
                                                                                T_item *new_father,
                                                                                T_betree *new_betree,
                                                                                T_lexem **adr_ref_lexem)
{
  TRACE5("T_jumpif::syntax_analysis(%x, %x, %x, %x, %x)",
                 adr_first,
                 adr_last,
                 new_father,
                 new_betree,
                 adr_ref_lexem) ;

  // On cherche l'expression apres avoir saute le "JUMPIF"
  T_lexem *cur_lexem = (*adr_ref_lexem)->get_next_lexem() ;
  syntax_check_eof(cur_lexem) ;

  cond = syntax_get_predicate(NULL,
                                                                   NULL,
                                                                   this,
                                                                   get_betree(),
                                                                   &cur_lexem) ;
 // On doit avoir un TO
  if ((cur_lexem == NULL) || (cur_lexem->get_lex_type() != L_TO))
        {
          syntax_unexpected_error(cur_lexem, FATAL_ERROR, get_catalog(C_TO)) ;
        }

  cur_lexem = cur_lexem->get_next_lexem() ;
  syntax_check_eof(cur_lexem) ;

  to = new T_flag(this, get_betree(), cur_lexem) ;

  label = syntax_get_expr(NULL,
                                                                   NULL,
                                                                   this,
                                                                   get_betree(),
                                                                   &cur_lexem) ;
   if ((cur_lexem == NULL) || (cur_lexem->get_lex_type() != L_END))
        {
          syntax_unexpected_error(cur_lexem, FATAL_ERROR, get_catalog(C_END)) ;
        }

  TRACE0("ok fin de new T_label") ;
  *adr_ref_lexem = cur_lexem->get_next_lexem() ;

  end_jumpif = new T_flag(this, get_betree(), cur_lexem) ;
  return ;
}

//
// }{	Analyse Syntaxique de la classe T_witness
//
void T_witness::syntax_analysis(T_item **adr_first,
										T_item **adr_last,
										T_item *new_father,
										T_betree *new_betree,
										T_lexem **adr_ref_lexem)
{
  TRACE5("T_witness::syntax_analysis(%x, %x, %x, %x, %x)",
		 adr_first,
		 adr_last,
		 new_father,
		 new_betree,
		 adr_ref_lexem) ;

  // On cherche le predicate apres avoir saute le "WITNESS"
  T_lexem *cur_lexem = (*adr_ref_lexem)->get_next_lexem() ;
  syntax_check_eof(cur_lexem) ;

  predicate = syntax_get_predicate(NULL,
								   NULL,
								   this,
								   get_betree(),
								   &cur_lexem) ;


#if 0 // A RAJOUTER APRES
  if (predicate == NULL)
	{
	  // A FAIRE ...
	  TRACE1("pb predicate = NULL") ;
	  assert(FALSE) ;
	}
#endif

  // Liste temporaire
  T_item *first = NULL ;
  T_item *last = NULL ;

  TRACE0("avant extract_params") ;
  predicate->extract_params((int)BPRED_CONJONCTION,
                      this,
                      &first,
                      &last) ;
  TRACE2("apres extract_params first =%x, last=%x", first, last) ;

  T_item *cur = first ;
  TRACE1("cur = %x", cur) ;
  while (cur != NULL)
  {
      TRACE1("cur : %s", cur->get_class_name()) ;
      if (	(cur->get_node_type() == NODE_TYPING_PREDICATE)
            && ( ((T_typing_predicate *)cur)->get_typing_predicate_type()
                 == TPRED_EQUAL) )
      {
          ;
      }
      else
      {
          syntax_unexpected_error(cur,
                                  CAN_CONTINUE,
                                  get_catalog(C_WITNESS_VALUATION)) ;
      }

      cur = cur->get_next() ;
      TRACE1("cur = %x", cur) ;
  }

  TRACE0("retour parseur LR ...") ;


  // On doit avoir un THEN
  if ((cur_lexem == NULL) || (cur_lexem->get_lex_type() != L_THEN))
	{
	  syntax_unexpected_error(cur_lexem, FATAL_ERROR, get_catalog(C_THEN)) ;
	}

  then = new T_flag(this, get_betree(), cur_lexem) ;

  cur_lexem = cur_lexem->get_next_lexem() ;
  syntax_check_eof(cur_lexem) ;

  first_body = last_body = NULL ;

  T_substitution *body = syntax_get_substitution((T_item **)&first_body,
												 (T_item **)&last_body,
												 then,// utilise pour FROM_WITNESS...
												 get_betree(),
												 &cur_lexem) ;

  if (body == NULL)
	{
	  // A FAIRE
	  assert(FALSE) ;
	}

  if ((cur_lexem == NULL) || (cur_lexem->get_lex_type() != L_END))
	{
	  syntax_unexpected_error(cur_lexem, FATAL_ERROR, get_catalog(C_END)) ;
	}

  TRACE0("ok fin de new T_witness") ;
  *adr_ref_lexem = cur_lexem->get_next_lexem() ;

  end_witness = new T_flag(this, get_betree(), cur_lexem) ;
  return ;
}

//
//	}{	Analyse Syntaxique de la classe T_if
//
void T_if::syntax_analysis(T_item **adr_first,
									T_item **adr_last,
									T_item *new_father,
									T_betree *new_betree,
									T_lexem **adr_ref_lexem)
{
  TRACE5("T_if::syntax_analysis(%x, %x, %x, %x, %x)",
		 adr_first,
		 adr_last,
		 new_father,
		 new_betree,
		 adr_ref_lexem) ;

  // On cherche le predicat apres avoir saute le "IF"
  T_lexem *cur_lexem = (*adr_ref_lexem)->get_next_lexem() ;

  TRACE0("parse condition") ;
  cond = syntax_get_predicate(NULL,
							  NULL,
							  this,
							  get_betree(),
							  &cur_lexem) ;

#if 0 // A RAJOUTER APRES
  if (cond == NULL)
	{
	  // A FAIRE ...
	  TRACE1("pb cond = NULL") ;
	  assert(FALSE) ;
	}
#endif

  // On doit avoir un THEN
  TRACE0("parse THEN") ;
  if ((cur_lexem == NULL) || (cur_lexem->get_lex_type() != L_THEN))
	{
	  syntax_unexpected_error(cur_lexem, FATAL_ERROR, get_catalog(C_THEN)) ;
	}

  cur_lexem = cur_lexem->get_next_lexem() ;
  syntax_check_eof(cur_lexem) ;

  // Alors ...
  first_then_body = last_then_body = NULL ;
  first_else = last_else = NULL ;

  TRACE0("parse body") ;
  T_substitution *body = syntax_get_substitution((T_item **)&first_then_body,
												 (T_item **)&last_then_body,
												 this,
												 get_betree(),
												 &cur_lexem) ;

  if (body == NULL)
	{
	  // A FAIRE
	  assert(FALSE) ;
	}

  syntax_check_eof(cur_lexem);
  // Traitement des elsif
  while (cur_lexem->get_lex_type() == L_ELSIF)
	{
	  TRACE0("ELSIF detecte") ;
	  T_lexem *ref_lexem = cur_lexem ;
	  T_else *else_clause = new T_else(TRUE,
									   (T_item **)&first_else,
									   (T_item **)&last_else,
									   this,
									   get_betree(),
									   &cur_lexem,
									   ref_lexem) ;

	  if (else_clause == NULL)
		{
		  // A FAIRE
		  assert(FALSE) ;
		}
	  
	  syntax_check_eof(cur_lexem);
	}

  // Parse du ELSE
  if (cur_lexem->get_lex_type() == L_ELSE)
	{
	  TRACE0("ELSE detecte") ;
	  T_else *else_clause = new T_else(FALSE,
									   (T_item **)&first_else,
									   (T_item **)&last_else,
									   this,
									   get_betree(),
									   &cur_lexem,
									   cur_lexem) ;
	  if (else_clause == NULL)
			  {
				// A FAIRE
				assert(FALSE) ;
			  }

	}

  syntax_check_eof(cur_lexem);

  // On attend un end !
  if (cur_lexem->get_lex_type() != L_END)
	{
	  TRACE1("ici :: pb cur_lexem %s",
			 cur_lexem->get_lex_name()) ;
	  syntax_unexpected_error(cur_lexem,
							  FATAL_ERROR,
							  get_catalog(C_END)) ;
	}

  else
	{
	  // Fin du if
	  TRACE0("fin de T_if::T_if") ;
	  end_if = new T_flag(this, get_betree(), cur_lexem) ;
	  *adr_ref_lexem = cur_lexem->get_next_lexem() ;
	}

}

//
//	}{	Classe T_else
//
// Analyse Syntaxique de la classe T_else
void T_else::syntax_analysis(int parse_cond,
									  T_item **adr_first,
									  T_item **adr_last,
									  T_item *new_father,
									  T_betree *new_betree,
									  T_lexem **adr_ref_lexem)
{
  TRACE6("T_else::syntax_analysis(%s,%x, %x, %x, %x, %x)",
		 (parse_cond == TRUE) ? "TRUE" : "FALSE",
		 adr_first,
		 adr_last,
		 new_father,
		 new_betree,
		 adr_ref_lexem) ;

  // On cherche la substitution predicat apres avoir saute le "THEN" ou le "ELSE"
  T_lexem *cur_lexem = (*adr_ref_lexem)->get_next_lexem() ;

  if (parse_cond == TRUE)
	{
	  cond = syntax_get_predicate(NULL, NULL, this, get_betree(), &cur_lexem) ;

#if 0
	  // A FAIRE :: rajouter apres
	  if (cond == NULL)
		{
		  // A FAIRE
		  TRACE0("pb cond") ;
		  assert(FALSE) ;
		}
#endif

	  TRACE0("sur THEN") ;
	  if ((cur_lexem == NULL) || (cur_lexem->get_lex_type() != L_THEN))
		{
		  syntax_unexpected_error(cur_lexem, FATAL_ERROR, get_catalog(C_THEN)) ;
		}

	  cur_lexem = cur_lexem->get_next_lexem() ;
	}
  else
	{
	  cond = NULL ;
	}

  first_body = last_body = NULL ;
  T_substitution *body = syntax_get_substitution((T_item **)&first_body,
												 (T_item **)&last_body,
												 this,
												 get_betree(),
												 &cur_lexem) ;

  if (body == NULL)
	{
	  // A FAIRE
	  assert(FALSE) ;
	}

  TRACE0("fin de T_else::syntax_analysis") ;
  syntax_check_eof(cur_lexem) ;
  *adr_ref_lexem = cur_lexem ; //->get_next_lexem() ;
  return ;
}

//
//	}{	Classe T_select
//
// Analyse Syntaxique de la classe T_select
void T_select::syntax_analysis(T_item **adr_first,
										T_item **adr_last,
										T_item *new_father,
										T_betree *new_betree,
										T_lexem **adr_ref_lexem)
{
  TRACE5("T_select::syntax_analysis(%x, %x, %x, %x, %x)",
		 adr_first,
		 adr_last,
		 new_father,
		 new_betree,
		 adr_ref_lexem) ;

  // On cherche le predicat apres avoir saute le "SELECT"
  T_lexem *cur_lexem = (*adr_ref_lexem)->get_next_lexem() ;
  syntax_check_eof(cur_lexem) ;

  TRACE0("parse condition") ;
  cond = syntax_get_predicate(NULL,
							  NULL,
							  this,
							  get_betree(),
							  &cur_lexem) ;

#if 0 // A RAJOUTER APRES
  if (cond == NULL)
	{
	  // A FAIRE ...
	  TRACE1("pb cond = NULL") ;
	  assert(FALSE) ;
	}
#endif

  // On doit avoir un THEN
  TRACE0("parse THEN") ;
  if ((cur_lexem == NULL) || (cur_lexem->get_lex_type() != L_THEN))
	{
	  syntax_unexpected_error(cur_lexem, FATAL_ERROR, get_catalog(C_THEN)) ;
	}

  cur_lexem = cur_lexem->get_next_lexem() ;
  syntax_check_eof(cur_lexem) ;

  // Alors ...
  first_then_body = last_then_body = NULL ;
  first_when = last_when = NULL ;

  TRACE0("parse body") ;
  T_substitution *body = syntax_get_substitution((T_item **)&first_then_body,
												 (T_item **)&last_then_body,
												 this,
												 get_betree(),
												 &cur_lexem) ;

  if (body == NULL)
	{
	  // A FAIRE
	  assert(FALSE) ;
	}

  // Traitement des WHEN
  while (cur_lexem->get_lex_type() == L_WHEN)
	{
	  TRACE0("WHEN detecte") ;
	  T_when *when_clause = new T_when(TRUE,
									   (T_item **)&first_when,
									   (T_item **)&last_when,
									   this,
									   get_betree(),
									   &cur_lexem) ;

	  if (when_clause == NULL)
		{
		  // A FAIRE
		  assert(FALSE) ;
		}

	}

  // Parse du ELSE
  if (cur_lexem->get_lex_type() == L_ELSE)
	{
	  TRACE0("ELSE detecte") ;
	  T_else *else_clause = new T_else(FALSE,
									   (T_item **)&first_when,
									   (T_item **)&last_when,
									   this,
									   get_betree(),
									   &cur_lexem,
									   cur_lexem) ;
	  if (else_clause == NULL)
		{
		  // A FAIRE
		  assert(FALSE) ;
		}

	}

  // On attend un end !
  if (cur_lexem->get_lex_type() != L_END)
	{
	  TRACE1("ici :: pb cur_lexem %s",
			 cur_lexem->get_lex_name()) ;
	  syntax_unexpected_error(cur_lexem,
							  FATAL_ERROR,
							  get_catalog(C_WHEN_ELSE_END)) ;
	}

  else //On a bien un end
	{
	  // Fin du select
	  TRACE0("fin de T_select::syntax_analysis") ;
	  end_select = new T_flag(this, get_betree(), cur_lexem) ;
	  *adr_ref_lexem = cur_lexem->get_next_lexem() ;
	}

}

//
//	}{	Classe T_when
//
// Analyse Syntaxique de la classe T_when
void T_when::syntax_analysis(int parse_cond,
									  T_item **adr_first,
									  T_item **adr_last,
									  T_item *new_father,
									  T_betree *new_betree,
									  T_lexem **adr_ref_lexem)
{
  TRACE6("T_when::syntax_analysis(%s,%x, %x, %x, %x, %x)",
		 (parse_cond == TRUE) ? "TRUE" : "FALSE",
		 adr_first,
		 adr_last,
		 new_father,
		 new_betree,
		 adr_ref_lexem) ;

  T_lexem *cur_lexem = (*adr_ref_lexem)->get_next_lexem() ;
  if (parse_cond == TRUE)
	{
	  cond = syntax_get_predicate(NULL, NULL, this, get_betree(), &cur_lexem) ;

#if 0
	  // A FAIRE :: rajouter apres
	  if (cond == NULL)
		{
		  // A FAIRE
		  TRACE0("pb cond") ;
		  assert(FALSE) ;
		}
#endif

	  if ((cur_lexem == NULL) || (cur_lexem->get_lex_type() != L_THEN))
		{
		  syntax_unexpected_error(cur_lexem, FATAL_ERROR, get_catalog(C_THEN)) ;
		}

	  cur_lexem = cur_lexem->get_next_lexem() ;
	}
  else
	{
	  cond = NULL ;
	}

  // On cherche la substitution predicat apres avoir saute le "THEN" ou le "ELSE"
  TRACE3("ICI cur_lexem %s (%d:%d)",
		 cur_lexem->get_lex_name(),
		 cur_lexem->get_file_line(),
		 cur_lexem->get_file_column()) ;

  syntax_check_eof(cur_lexem) ;

  T_substitution *body = syntax_get_substitution((T_item **)&first_body,
												 (T_item **)&last_body,
												 this,
												 get_betree(),
												 &cur_lexem) ;

  if (body == NULL)
	{
	  // A FAIRE
	  assert(FALSE) ;
	}

  TRACE0("fin de T_when::syntax_analysis") ;
  *adr_ref_lexem = cur_lexem ;
  return ;
}

//
//	}{	Classe T_case
//
// Analyse Syntaxique de la classe T_case
void T_case::syntax_analysis(T_item **adr_first,
									  T_item **adr_last,
									  T_item *new_father,
									  T_betree *new_betree,
									  T_lexem **adr_ref_lexem)
{
  TRACE5("T_case::syntax_analysis(%x, %x, %x, %x, %x)",
		 adr_first,
		 adr_last,
		 new_father,
		 new_betree,
		 adr_ref_lexem) ;

  // On cherche l'expression apres avoir saute le "CASE"
  T_lexem *cur_lexem = (*adr_ref_lexem)->get_next_lexem() ;

  TRACE0("parse expression") ;
  case_select = syntax_get_expr(NULL, NULL, this, get_betree(), &cur_lexem) ;

  TRACE1("case_select = %x", case_select) ;

  TRACE0("parse OF") ;
  // On doit avoir un OF
  if ((cur_lexem == NULL) || (cur_lexem->get_lex_type() != L_OF))
	{
	  syntax_unexpected_error(cur_lexem, FATAL_ERROR, get_catalog(C_OF)) ;
	}

  cur_lexem = cur_lexem->get_next_lexem() ;

  first_case_item = last_case_item = NULL ;

   // On doit avoir un EITHER
  if (cur_lexem->get_lex_type() != L_EITHER)
	{
	  syntax_unexpected_error(cur_lexem,
							  FATAL_ERROR,
							  get_catalog(C_EITHER)) ;
	}
  else // On a bien un EITHER...
	{
	  T_case_item *case_item = new T_case_item((T_item **)&first_case_item,
											   (T_item **)&last_case_item,
											   this,
											   get_betree(),
											   &cur_lexem) ;

	  if (case_item == NULL)
		{
		  // A FAIRE
		  assert(FALSE) ;
		}

	}

  // Traitement des OR
  while (cur_lexem->get_lex_type() == L_OR)
	{
	  T_case_item *case_item = new T_case_item((T_item **)&first_case_item,
											   (T_item **)&last_case_item,
											   this,
											   get_betree(),
											   &cur_lexem) ;
	  if (case_item == NULL)
		{
		  // A FAIRE
		  assert(FALSE) ;
		}

	}

  // Traitement du ELSE
  if (cur_lexem->get_lex_type() == L_ELSE)
	{
	  T_case_item *case_item = new T_case_item((T_item **)&first_case_item,
											   (T_item **)&last_case_item,
											   this,
											   get_betree(),
											   &cur_lexem) ;
	  if (case_item == NULL)
		{
		  // A FAIRE
		  assert(FALSE) ;
		}
	}

  // On doit avoir un END !
  if (cur_lexem->get_lex_type() != L_END)
	{
	  syntax_unexpected_error(cur_lexem,
							  FATAL_ERROR,
							  get_catalog(C_OR_ELSE_END)) ;
	}
  else  // On a bien un END
	{
	  // Fin du CASE
	  // On stocke le END pour trace
	  // Il en faut encore un ...
	  TRACE0("fin de T_case::syntax_analysis") ;
	  end_either = new T_flag(this, get_betree(), cur_lexem) ;
	  cur_lexem = cur_lexem->get_next_lexem() ;

	  if ( (cur_lexem == NULL) || (cur_lexem->get_lex_type() != L_END) )
		{
		  syntax_unexpected_error(cur_lexem, FATAL_ERROR, get_catalog(C_END));
		}

	  end_case = new T_flag(this, get_betree(), cur_lexem) ;
	  *adr_ref_lexem = cur_lexem->get_next_lexem() ;
	}


}

//
//	}{	Classe T_case_item
//
// Analyse Syntaxique de la classe T_case_item
void T_case_item::syntax_analysis(T_item **adr_first,
										   T_item **adr_last,
										   T_item *new_father,
										   T_betree *new_betree,
										   T_lexem **adr_ref_lexem)
{
  TRACE5("T_case_item::syntax_analysis(%x, %x, %x, %x, %x)",
		 adr_first,
		 adr_last,
		 new_father,
		 new_betree,
		 adr_ref_lexem) ;
  ENTER_TRACE ;

  // On cherche la substitution predicat apres avoir saute le "THEN" ou le "ELSE"
  T_lexem *cur_lexem = *adr_ref_lexem ;
  T_lex_type lex_type = cur_lexem->get_lex_type() ;
  first_literal_value = last_literal_value = NULL ;


  if ( (lex_type == L_EITHER) || (lex_type == L_OR) )
	{
	  TRACE0("on a un EITHER ou un OR") ;

	  cur_lexem = cur_lexem->get_next_lexem() ;
	  syntax_check_eof(cur_lexem) ;

#if 0
	  int encore = TRUE ;

	  while (encore == TRUE)
		{
		  // Non, le parseur mange les ',' et rend un terme simple !
		  T_item *term = syntax_get_simple_term(&first_literal_value,
												&last_literal_value,
												this,
												get_betree(),
												&cur_lexem) ;

		  if (term == NULL)
			{
			  // A FAIRE
			  assert(FALSE) ;
			}

		  if ((cur_lexem != NULL) && (cur_lexem->get_lex_type() == L_COMMA))
			{
			  cur_lexem = cur_lexem->get_next_lexem() ;
			  syntax_check_eof(cur_lexem) ;
			}
		  else
			{
			  // Fin des termes simples
			  encore = FALSE ;
			}
		}

#endif
	  // Non, le parseur mange les ',' et rend un terme simple !
	  T_item *term = syntax_get_simple_term(NULL,
											NULL,
											this,
											get_betree(),
											&cur_lexem) ;

	  if (term == NULL)
		{
		  // A FAIRE
		  assert(FALSE) ;
		}

	  // On recree la liste
	  term->extract_params((int)BOP_COMMA,
						   this,
						   (T_item **)&first_literal_value,
						   (T_item **)&last_literal_value) ;

	  // Parse le THEN
	  if ((cur_lexem == NULL) || (cur_lexem->get_lex_type() != L_THEN))
		{
		  syntax_unexpected_error(cur_lexem, FATAL_ERROR, get_catalog(C_THEN)) ;
		}

	  cur_lexem = cur_lexem->get_next_lexem() ;
	}
  else
	{
	  // Saute le ELSE
	  TRACE0("on a un ELSE :: on le saute") ;
	  cur_lexem = cur_lexem->get_next_lexem() ;
	}

  // On parse la substitution
  TRACE0("parse la substitution") ;
  first_body = last_body = NULL ;

  T_substitution *body = syntax_get_substitution((T_item **)&first_body,
												 (T_item **)&last_body,
												 this,
												 get_betree(),
												 &cur_lexem) ;

  if (body == NULL)
	{
	  // A FAIRE
	  assert(FALSE) ;
	}

  EXIT_TRACE ;
  TRACE0("fin de T_case_item::syntax_analysis") ;
  *adr_ref_lexem = cur_lexem ;
  return ;
}

//
//	}{	Classe T_let
//
// Analyse Syntaxique de la classe T_let
void T_let::syntax_analysis(T_item **adr_first,
									 T_item **adr_last,
									 T_item *new_father,
									 T_betree *new_betree,
									 T_lexem **adr_ref_lexem)
{
  TRACE5("T_let::syntax_analysis(%x, %x, %x, %x, %x)",
		 adr_first,
		 adr_last,
		 new_father,
		 new_betree,
		 adr_ref_lexem) ;

  // On cherche les identificateurs apres avoir saute le "LET"
  T_lexem *cur_lexem = (*adr_ref_lexem)->get_next_lexem() ;

  TRACE0("parse identificateurs") ;

  first_ident = last_ident = NULL ;

  if ( (cur_lexem == NULL) || (cur_lexem->get_lex_type() != L_IDENT) )
	{
	  syntax_unexpected_error(cur_lexem,
							  FATAL_ERROR,
							  get_catalog(C_LOCAL_QUANTIFIER)) ;
	}

  T_ident *cur_ident = syntax_get_ident(ITYPE_LOCAL_QUANTIFIER,
										(T_item **)&first_ident,
										(T_item **)&last_ident,
										this,
										get_betree(),
										&cur_lexem) ;

  if (cur_ident == NULL)
	{
	  // A FAIRE ...
	  TRACE0("pb cur_ident = NULL") ;
	  assert(FALSE) ;
	}

  while ( (cur_lexem != NULL) && (cur_lexem->get_lex_type() == L_COMMA) )
	{
	  // On a une virgule donc on parse un identificateur de plus
	  TRACE0("ident suivant") ;

	  cur_lexem = cur_lexem->get_next_lexem() ;

	  syntax_check_eof(cur_lexem) ;
          if ( (cur_lexem == NULL) || (cur_lexem->get_lex_type() != L_IDENT) )
          {
              syntax_unexpected_error(cur_lexem,
                                      FATAL_ERROR,
                                      get_catalog(C_LOCAL_QUANTIFIER)) ;
          }

	  T_ident *cur_ident = syntax_get_ident(ITYPE_LOCAL_QUANTIFIER,
											(T_item **)&first_ident,
											(T_item **)&last_ident,
											this,
											get_betree(),
											&cur_lexem) ;

	  if (cur_ident == NULL)
		{
		  // A FAIRE ...
		  TRACE0("pb cur_ident = NULL") ;
		  assert(FALSE) ;
		}

	}

  TRACE0("parse BE") ;

  // On doit avoir un BE
  if ((cur_lexem == NULL) || (cur_lexem->get_lex_type() != L_BE))
	{
	  syntax_unexpected_error(cur_lexem, FATAL_ERROR, get_catalog(C_BE)) ;
	}

  cur_lexem = cur_lexem->get_next_lexem() ;
  first_valuation = last_valuation = NULL ;

  // Ident = Expr parse par analyseur LR
  // L'analyseur LR mange egalement les '&' donc on le laisse travailler
  TRACE0("apres BE :: on appelle le parseur LR") ;
  first_valuation = last_valuation = NULL ;
  T_item *res = syntax_call_LR_parser(NULL,
									  NULL,
									  get_father(),
									  get_betree(),
									  &cur_lexem) ;

  TRACE0("on extrait les valuations ...") ;

  // Liste temporaire
  T_item *first = NULL ;
  T_item *last = NULL ;

  TRACE0("avant extract_params") ;
  res->extract_params((int)BPRED_CONJONCTION,
					  this,
					  &first,
					  &last) ;
  TRACE2("apres extract_params first =%x, last=%x", first, last) ;

  T_item *cur = first ;
  TRACE1("cur = %x", cur) ;
  while (cur != NULL)
	{
	  TRACE1("cur : %s", cur->get_class_name()) ;
	  if (	(cur->get_node_type() == NODE_TYPING_PREDICATE)
			&& ( ((T_typing_predicate *)cur)->get_typing_predicate_type()
				 == TPRED_EQUAL) )
		{
		  (void)new T_valuation((T_typing_predicate *)cur, // pose un lien
								(T_item **)&first_valuation,
								(T_item **)&last_valuation,
								this,
								get_betree()) ;
		}
	  else
		{
		  syntax_unexpected_error(cur,
								  CAN_CONTINUE,
								  get_catalog(C_LET_VALUATION)) ;
		}

	  cur = cur->get_next() ;
	  TRACE1("cur = %x", cur) ;
	}

  TRACE0("retour parseur LR ...") ;

  TRACE0("IN :: on cheche la substitution de fin") ;
  if ( (cur_lexem == NULL) || (cur_lexem->get_lex_type() != L_IN) )
	{
	  syntax_unexpected_error(cur_lexem, FATAL_ERROR, get_catalog(C_IN)) ;
	}

  cur_lexem = cur_lexem->get_next_lexem() ;

  syntax_check_eof(cur_lexem) ;

  T_substitution *body = syntax_get_substitution((T_item **)&first_body,
												 (T_item **)&last_body,
												 this,
												 get_betree(),
												 &cur_lexem) ;

  assert(body != NULL) ; // A FAIRE

  if ( (cur_lexem == NULL) || (cur_lexem->get_lex_type() != L_END) )
	{
	  syntax_unexpected_error(cur_lexem, FATAL_ERROR, get_catalog(C_END)) ;
	}

  TRACE0("fin de T_let::syntax_analysis") ;
  end_let = new T_flag(this, get_betree(), cur_lexem) ;
  *adr_ref_lexem = cur_lexem->get_next_lexem() ;
  return ;
}

//
//	}{	Classe T_var
//
// Analyse Syntaxique de la classe T_var
static int parse_local_var(T_lexem **adr_cur_lexem,
									T_ident **first_ident,
									T_ident **last_ident,
									T_var *sub_var,
									T_op *op_def,
									T_machine *mach_def)

{
  int result = TRUE;
  T_lexem *cur_lexem = *adr_cur_lexem;
  if ( (cur_lexem == NULL) || (cur_lexem->get_lex_type() != L_IDENT) )
	{
	  syntax_unexpected_error(cur_lexem,
							  FATAL_ERROR,
							  get_catalog(C_IDENT)) ;
	}

  T_ident *loc_var = new T_ident(cur_lexem->get_value(),
								   ITYPE_LOCAL_VARIABLE,
								   (T_item **)first_ident,
								   (T_item **)last_ident,
								   sub_var,
								   sub_var->get_betree(),
								   cur_lexem) ;

  assert(loc_var) ; // A FAIRE
  loc_var->set_ref_op(op_def) ;
  loc_var->set_ref_machine(mach_def) ;

  cur_lexem = cur_lexem->get_next_lexem() ;
  syntax_check_eof(cur_lexem) ;

  if ( (cur_lexem != NULL) && (cur_lexem->get_lex_type() == L_COMMA) )
	{
	  TRACE0("ok, un ident de plus") ;
	  cur_lexem = cur_lexem->get_next_lexem() ;
	}
  else
	{
	  TRACE0("fin de la liste des idf") ;
	  result = FALSE ;
	}
  *adr_cur_lexem = cur_lexem;
  return result;
}

//
// parse une varaible locale typ�e et �ventuellement initialis�e
//
static int parse_typed_local_var(T_lexem **adr_cur_lexem,
										  T_ident **first_local_ident,
										  T_ident **last_local_ident,
										  T_var *sub_var,
										  T_op *op_def,
										  T_machine *mach_def)

{
  TRACE0("parse_typed_local_var");
  int result = TRUE;
  T_lexem *cur_lexem = *adr_cur_lexem;
  syntax_check_eof(cur_lexem);

  // On parse toute la declaration
  T_item *item = syntax_call_LR_parser(NULL,
									   NULL,
									   NULL,
									   sub_var->get_betree(),
									   &cur_lexem);

  if (item == NULL || item->is_a_predicate() == FALSE)
	{
	  syntax_predicate_expected_error(item,FATAL_ERROR);
	}

  // On extrait du pr�dicat pr�c�dent la declaration et
  // l'initialisation eventuelle
  T_predicate *type_pred;
  T_predicate *init_pred = NULL;
  if (item->get_node_type() == NODE_BINARY_PREDICATE
	  &&
	  ((T_binary_predicate *) item)->get_predicate_type() == BPRED_CONJONCTION)
	{
	  // Il y a une init
	  type_pred = ((T_binary_predicate *) item)->get_pred1();
	  init_pred = ((T_binary_predicate *) item)->get_pred2();
	  delete item;
	  item = NULL;
	}
  else
	{
	  // Il n'y a pas d'initialisation
	  type_pred = (T_predicate *) item;
	}

  ASSERT(type_pred != NULL);

  if (type_pred->get_node_type() != NODE_TYPING_PREDICATE)
	{
	  syntax_unexpected_error(type_pred->get_ref_lexem(),
							  FATAL_ERROR,
							  get_catalog(C_TYPING_PREDICAT));
	}
  if (((T_typing_predicate *)type_pred)->get_typing_predicate_type() != TPRED_BELONGS)
	{
	  syntax_unexpected_error(type_pred->get_ref_lexem(),
							  FATAL_ERROR,
							  get_catalog(C_BELONGS));
	}

  T_typing_predicate *typing_pred = (T_typing_predicate *)type_pred;

  // V�rification qu'un seul identificateur est d�clar�
  T_expr *first_ident = typing_pred->get_identifiers();
  if (first_ident->get_next() != NULL)
	{
	  ASSERT(first_ident->get_next()->is_an_ident() == TRUE);
	  syntax_unexpected_error(((T_ident *)first_ident->get_next())->get_ref_lexem(),
							  FATAL_ERROR,
							  get_catalog(C_BELONGS));
	}
  ASSERT(first_ident->is_an_ident() == TRUE);

  // Loc_var est l'identificateur racine de la d�claration
  T_ident *loc_var = (T_ident *)first_ident;
  loc_var->set_BOM_lexem_type(typing_pred->get_ref_lexem());

  ASSERT(typing_pred->get_type()->is_an_expr() == TRUE);

  loc_var->set_BOM_type(typing_pred->get_type());
  typing_pred->get_type()->set_father(loc_var);

  if (init_pred != NULL)
	{
	  TRACE1("la variable %s est initialis�e", loc_var->get_name()->get_value());

	  if (init_pred->get_node_type() != NODE_TYPING_PREDICATE)
		{
		  syntax_unexpected_error(type_pred->get_ref_lexem(),
								  FATAL_ERROR,
								  get_catalog(C_TYPING_PREDICAT));
		}
	  if (((T_typing_predicate *)init_pred)->get_typing_predicate_type() != TPRED_EQUAL)
		{
		  syntax_unexpected_error(type_pred,
								  FATAL_ERROR,
								  get_catalog(C_EQUALS));
		}
	  T_typing_predicate *init_typing_pred = (T_typing_predicate *)init_pred;

	  // V�rification qu'un seul identificateurs est en partie droite
	  T_expr *first_ident = init_typing_pred->get_identifiers();
	  if (first_ident->get_next() != NULL)
		{
		  ASSERT(first_ident->get_next()->is_an_ident() == TRUE);
		  syntax_unexpected_error(((T_ident *)first_ident->get_next())->get_ref_lexem(),
								  FATAL_ERROR,
								  get_catalog(C_EQUALS));
		}
	  ASSERT(first_ident->is_an_ident() == TRUE);

	  // On v�rifie que l'ident initialis� est bien l'ident d�clar�
	  T_ident *init_ident = (T_ident *)first_ident;
	  if (init_ident->get_name()->compare(loc_var->get_name()) != TRUE)
		{
		  syntax_error(init_ident->get_ref_lexem(),
					   CAN_CONTINUE,
					   get_error_msg(E_IDENT_INIT_VAR_LOCAL),
					   init_ident->get_name()->get_value(),
					   loc_var->get_name()->get_value());
		}

	  ASSERT(init_typing_pred->get_type()->is_an_expr() == TRUE);
	  loc_var->set_BOM_init_value(init_typing_pred->get_ref_lexem(),
								  init_typing_pred->get_type());
	  ((T_expr *)init_typing_pred->get_type())->set_father(loc_var);

	  init_ident->set_ident_type(ITYPE_UNKNOWN);
	  init_ident->set_father(loc_var);
	  init_ident->set_ref_op(op_def);
	  init_ident->set_ref_machine(mach_def);
	  init_ident->set_def(loc_var);

	  // suppressio de la liste des identificateurs non r�solu
	  get_object_manager()->delete_unsolved_ident(init_ident);
	  delete init_typing_pred;
	  init_typing_pred = NULL;
	  init_pred = NULL;
	}


  // mise � jours des liens de chainage des variables locales:
  loc_var->set_next(NULL);
  loc_var->set_prev(*last_local_ident);
  if ((*last_local_ident) != NULL)
    {
	  (*last_local_ident)->set_next(loc_var);
	}
  TRACE1("MAJ derni�re variables locales %p", loc_var);
  *last_local_ident = loc_var;
  if ((*first_local_ident) == NULL)
	{
	  TRACE1("Mise � jours de la premi�re variable locale: %p", loc_var);
	  *first_local_ident = loc_var;
	}

  // mise � jours du betree
  loc_var->set_ident_type(ITYPE_LOCAL_VARIABLE);
  loc_var->set_father(sub_var);
  loc_var->set_ref_op(op_def);
  loc_var->set_ref_machine(mach_def);


  // suppression du predicat de typage
  delete type_pred;
  type_pred = NULL;


  if (cur_lexem->get_lex_type() == L_SCOL)
	{
	  TRACE0("ok, un ident de plus") ;
	  cur_lexem = cur_lexem->get_next_lexem() ;
	}
  else
	{
	  TRACE1("fin de la liste des variables locales; type lexem courant:%s",
			 cur_lexem != NULL ? get_lex_type_ascii(cur_lexem->get_lex_type()) : "null") ;
	  result = FALSE ;
	}
  *adr_cur_lexem = cur_lexem;
  return result;
}

void T_var::syntax_analysis(T_item **adr_first,
									 T_item **adr_last,
									 T_item *new_father,
									 T_betree *new_betree,
									 T_lexem **adr_ref_lexem)
{
  TRACE5("T_var::syntax_analysis(%x, %x, %x, %x, %x)",
		 adr_first,
		 adr_last,
		 new_father,
		 new_betree,
		 adr_ref_lexem) ;
  ENTER_TRACE ;

  // Recherche de l'operation et de la machine de definition
  T_op *op_def = NULL ;
  T_machine *mach_def = NULL ;

  find_machine_and_op_def(&mach_def, &op_def) ;

  // On cherche les identificateurs apres avoir saute le "VAR"
  T_lexem *cur_lexem = (*adr_ref_lexem)->get_next_lexem() ;
  syntax_check_eof(cur_lexem);

  // Pour savoir si on parse un autre identificateur
  int encore = TRUE ;
  first_ident = last_ident = NULL ;

  while (encore == TRUE)
	{
	  if (get_Use_B0_Declaration() == FALSE
		  ||
		  mach_def->get_machine_type() == MTYPE_REFINEMENT)
		{
		  encore = parse_local_var(&cur_lexem,
								   &first_ident,
								   &last_ident,
								   this,
								   op_def,
								   mach_def);
		}
	  else
		{
		  encore = parse_typed_local_var(&cur_lexem,
										 &first_ident,
										 &last_ident,
										 this,
										 op_def,
										 mach_def);

		  TRACE2("sortie parse_typed_local_var type lexem courant : %s, %p",
				 cur_lexem != NULL ? get_lex_type_ascii(cur_lexem->get_lex_type()) : "null",
				 last_ident) ;

		}
	}

  // On parse le "IN"
  TRACE1("parse IN; type lexem courant : %s",
		 cur_lexem != NULL ? get_lex_type_ascii(cur_lexem->get_lex_type()) : "null") ;

  if ( (cur_lexem == NULL) || (cur_lexem->get_lex_type() != L_IN) )
	{
	  syntax_unexpected_error(cur_lexem, FATAL_ERROR, get_catalog(C_IN)) ;
	}

  TRACE0("apres test");
  cur_lexem = cur_lexem->get_next_lexem() ;
  syntax_check_eof(cur_lexem);

  first_body = last_body = NULL ;
  T_substitution *body = syntax_get_substitution((T_item **)&first_body,
												 (T_item **)&last_body,
												 this,
												 get_betree(),
												 &cur_lexem) ;

  if (body == NULL)
	{
	  // A FAIRE
	  assert(FALSE) ;
	}

  syntax_check_eof(cur_lexem);
  if (cur_lexem->get_lex_type() != L_END)
	{
	  syntax_ident_expected_error(cur_lexem, FATAL_ERROR);
	}

  // On memorise la localisation du end
  end_var = new T_flag(this, get_betree(), cur_lexem) ;
  *adr_ref_lexem = cur_lexem->get_next_lexem() ;

  EXIT_TRACE ;
  TRACE0("fin de T_var::syntax_analysis") ;
}

//
//	}{	Classe T_while
//
// Analyse Syntaxique de la classe T_while
void T_while::syntax_analysis(T_item **adr_first,
									   T_item **adr_last,
									   T_item *new_father,
									   T_betree *new_betree,
									   T_lexem **adr_ref_lexem)
{
  TRACE5("T_while::syntax_analysis(%x, %x, %x, %x, %x)",
		 adr_first,
		 adr_last,
		 new_father,
		 new_betree,
		 adr_ref_lexem) ;
  ENTER_TRACE ;

  // On cherche les identificateurs apres avoir saute le "WHILE"
  T_lexem *cur_lexem = (*adr_ref_lexem)->get_next_lexem() ;

  syntax_check_eof(cur_lexem) ;


  // On parse la condition
  cond = syntax_get_predicate(NULL, NULL, this, get_betree(), &cur_lexem) ;

  // On parse le do
  assert(cond) ; // A FAIRE

  if ((cur_lexem == NULL) || (cur_lexem->get_lex_type() != L_DO) )
	{
	  syntax_unexpected_error(cur_lexem, FATAL_ERROR, get_catalog(C_DO)) ;
	}

  cur_lexem = cur_lexem->get_next_lexem() ;

  first_body = last_body = NULL ;
  T_substitution *instr = syntax_get_substitution((T_item **)&first_body,
												  (T_item **)&last_body,
												  this,
												  get_betree(),
												  &cur_lexem) ;

  // A FAIRE :: verifier que instr est une instruction

  assert(instr) ; // A FAIRE

  // Invariant
  TRACE0("parse invariant") ;

  if ((cur_lexem == NULL) || (cur_lexem->get_lex_type() != L_INVARIANT) )
	{
	  syntax_unexpected_error(cur_lexem,
							  FATAL_ERROR,
							  get_catalog(C_INVARIANT)) ;
	}

  invariant_keyword = new T_flag(this, get_betree(), cur_lexem) ;

  cur_lexem = cur_lexem->get_next_lexem() ;
  invariant = syntax_get_predicate(NULL,
								   NULL,
								   invariant_keyword,
								   get_betree(),
								   &cur_lexem) ;

  assert(invariant) ; // A FAIRE ;

  // Variant
  TRACE0("parse variant") ;
  if ((cur_lexem == NULL) || (cur_lexem->get_lex_type() != L_VARIANT))
	{
	  syntax_unexpected_error(cur_lexem, FATAL_ERROR, get_catalog(C_VARIANT)) ;
	}

  variant_keyword = new T_flag(this, get_betree(), cur_lexem) ;
  cur_lexem = cur_lexem->get_next_lexem() ;
  syntax_check_eof(cur_lexem) ;
  variant = syntax_get_expr(NULL, NULL, variant_keyword, get_betree(), &cur_lexem) ;

  // END
  if ((cur_lexem == NULL) || (cur_lexem->get_lex_type() != L_END))
	{
	  syntax_unexpected_error(cur_lexem, FATAL_ERROR, get_catalog(C_END)) ;
	}

  // On memorise la localisation du end
  end_while = new T_flag(this, get_betree(), cur_lexem) ;
  *adr_ref_lexem = cur_lexem->get_next_lexem() ;

  EXIT_TRACE ;
  TRACE0("fin de T_while::syntax_analysis") ;
}

//
//	}{	Classe T_any
//
// Analyse Syntaxique de la classe T_any
void T_any::syntax_analysis(T_item **adr_first,
									 T_item **adr_last,
									 T_item *new_father,
									 T_betree *new_betree,
									 T_lexem **adr_ref_lexem)
{
  TRACE5("T_any::syntax_analysis(%x, %x, %x, %x, %x)",
		 adr_first,
		 adr_last,
		 new_father,
		 new_betree,
		 adr_ref_lexem) ;

  // On cherche les identificateurs apres avoir saute le "LET"
  T_lexem *cur_lexem = (*adr_ref_lexem)->get_next_lexem() ;
  syntax_check_eof(cur_lexem) ;

  TRACE0("parse identificateurs") ;

  first_ident = last_ident = NULL ;

  if ( (cur_lexem == NULL) || (cur_lexem->get_lex_type() != L_IDENT) )
	{
	  syntax_unexpected_error(cur_lexem,
							  FATAL_ERROR,
							  get_catalog(C_LOCAL_QUANTIFIER)) ;
	}

  T_ident *cur_ident = syntax_get_ident(ITYPE_ANY_QUANTIFIER,
										(T_item **)&first_ident,
										(T_item **)&last_ident,
										this,
										get_betree(),
										&cur_lexem) ;

  if (cur_ident == NULL)
	{
	  TRACE0("pb cur_ident = NULL") ;
	  assert(FALSE) ;
	}

  while ( (cur_lexem != NULL) && (cur_lexem->get_lex_type() == L_COMMA) )
	{
	  // On a une virgule donc on parse un identificateur de plus
	  TRACE0("ident suivant") ;

	  cur_lexem = cur_lexem->get_next_lexem() ;

	  syntax_check_eof(cur_lexem) ;

	  if (cur_lexem->get_lex_type() != L_IDENT)
		{
		  syntax_unexpected_error(cur_lexem,
								  FATAL_ERROR,
								  get_catalog(C_LOCAL_QUANTIFIER)) ;
		}

      T_ident *cur_ident = syntax_get_ident(ITYPE_ANY_QUANTIFIER,
											(T_item **)&first_ident,
											(T_item **)&last_ident,
											this,
											get_betree(),
											&cur_lexem) ;

	  if (cur_ident == NULL)
		{
		  TRACE0("pb cur_ident = NULL") ;
		  syntax_unexpected_error(cur_lexem, FATAL_ERROR, get_catalog(C_IDENT)) ;
		}

	}

  // On doit avoir un WHERE
  TRACE0("parse WHERE") ;

  if ((cur_lexem == NULL) || (cur_lexem->get_lex_type() != L_WHERE))
	{
	  syntax_unexpected_error(cur_lexem, FATAL_ERROR, get_catalog(C_WHERE)) ;
	}

  cur_lexem = cur_lexem->get_next_lexem() ;
  syntax_check_eof(cur_lexem) ;

  where = NULL ;

  // Pour savoir si le Ident=Expr precedent s'est fini par une virgule
  int can_continue = TRUE ;

  // Traitement du contenu du where
  while (can_continue == TRUE)
	{
	  // A faire :: integere typage_donnee_abs
	  where = syntax_get_predicate(NULL, NULL, this, get_betree(), &cur_lexem) ;

	  assert(where) ; // A FAIRE

	  if ((cur_lexem != NULL) && (cur_lexem->get_lex_type() == L_AND))
		{
		  // On reste dans la boucle
		  TRACE0("L_AND->on repart pour un tour") ;
		  cur_lexem = cur_lexem->get_next_lexem() ;
		  syntax_check_eof(cur_lexem) ;
		}
	  else
		{
		  can_continue = FALSE ;
		}
	}

  // Parse THEN ...
  TRACE0("parse then") ;

  if ((cur_lexem == NULL) || (cur_lexem->get_lex_type() != L_THEN))
	{
	  syntax_unexpected_error(cur_lexem,
							  FATAL_ERROR,
							  get_catalog(C_THEN)) ;
	}

  cur_lexem = cur_lexem->get_next_lexem() ;

  syntax_check_eof(cur_lexem) ;

  first_body = last_body = NULL ;
  T_substitution *body = syntax_get_substitution((T_item **)&first_body,
												 (T_item **)&last_body,
												 this,
												 get_betree(),
												 &cur_lexem) ;

  if (body == NULL)
	{
	  stop() ;
	}

  // Parse END ...
  TRACE0("parse end") ;

  if ((cur_lexem == NULL) || (cur_lexem->get_lex_type() != L_END))
	{
	  syntax_unexpected_error(cur_lexem, FATAL_ERROR, get_catalog(C_END)) ;
	}

  // On memorise la localisation du end
  end_any = new T_flag(this, get_betree(), cur_lexem) ;
  *adr_ref_lexem = cur_lexem->get_next_lexem() ;
  EXIT_TRACE ;
  TRACE0("fin de T_any::syntax_analysis") ;
}

//
//	}{	Fin du fichier
//

