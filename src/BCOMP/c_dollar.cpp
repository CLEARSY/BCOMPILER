/******************************* CLEARSY **************************************
* Fichier : $Id: c_dollar.cpp,v 2.0 2000-10-20 12:12:40 fl Exp $
*
* (C) 2008 CLEARSY
*
* Description :		Compilateur B : Verifications d'utilisation des $0
*
* Compilations :	-DDEBUG_DOLLAR_ZERO pour avoir les traces
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
RCS_ID("$Id: c_dollar.cpp,v 1.5 2000-10-20 12:12:40 fl Exp $") ;

// Includes systeme
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

// Includes Composant Local
#include "c_api.h"

//
// Controles syntaxiques d'utilisation du $0
//
void T_ident::syntaxic_check_dollar_zero_integrity(void)
{
#ifdef DEBUG_DOLLAR_ZERO
  TRACE2("T_ident(%x : \"%s\")::syntaxic_check_dollar_zero_integrity",
		 this,
		 name->get_value()) ;
#endif

  // Remarque : depuis reunion 02/02/99 l'utilisation de $0 dans un
  // ASSERT est interdite

  T_item *cur = get_father() ;

  // Pour savoir si on a trouve le WHILE ou le BECOMES SUCH THAT
  int done = FALSE ;

  // Pour savoir si dans le cas d'un WHILE on vient d'un ASSERT ou de l'INVARIANT,
  // on memorise :
  // 1) la Premiere substitution trouvee sur le chemin de recerche
  // (qui doit etre NULL ou ASSERT)
  T_substitution *first_subst = NULL ;
  // 2) la Premiere clause trouvee sur le chemin de recerche
  // (qui doit etre NULL ou INVARIANT)
  T_flag *first_clause = NULL ;

  while (done == FALSE)
	{
#ifdef DEBUG_DOLLAR_ZERO
	  TRACE1("cur = %x", cur) ;
#endif


	  if (    (cur == NULL)
		   || (cur->get_node_type() == NODE_OPERATION) )
		{
		  TRACE0("erreur !!!") ;
		  syntax_error(get_ref_lexem(),
					   CAN_CONTINUE,
					   get_error_msg(E_BEFORE_ONLY_IN_WHILE_AND_BEC_SUCH_THAT)) ;
		  return ;
		}

#ifdef DEBUG_DOLLAR_ZERO
	  TRACE1("cur :: %s", cur->get_class_name()) ;
#endif

	  if ( (cur->is_a_substitution() == TRUE) && (first_subst == NULL) )
		{
		  // C'est la premiere substitution
		  first_subst = (T_substitution *)cur ;
		}
	  else if ( (cur->get_node_type() == NODE_FLAG == TRUE)&&(first_clause == NULL))
		{
		  // C'est la premiere substitution
		  first_clause = (T_flag *)cur ;
		}

	  if (cur->get_node_type() == NODE_BECOMES_SUCH_THAT)
		{
#ifdef DEBUG_DOLLAR_ZERO
		  TRACE1("ok, classe %s", cur->get_class_name()) ;
#endif

		  done = TRUE ;
		}
	  else 	  if (cur->get_node_type() == NODE_WITNESS)
		{
#ifdef DEBUG_DOLLAR_ZERO
		  TRACE1("ok, classe %s", cur->get_class_name()) ;
#endif

		  done = TRUE ;
		}
	  else if (cur->get_node_type() == NODE_WHILE)
		{
		  // Il faut qu'on ai ete dans l'invariant
		  TRACE2("analyse de WHILE first_subst %x first_clause %x",
				 first_subst,
				 first_clause) ;
		  if (first_clause != NULL)
			{
			  // Il faut que cela soit un INVARIANT
			  if (first_clause->get_ref_lexem()->get_lex_type() == L_INVARIANT)
				{
				  done = TRUE ;
				}
			}

		  if (done == FALSE)
			{
			  syntax_error(get_ref_lexem(),
						   CAN_CONTINUE,
						   get_error_msg(E_BEFORE_ONLY_IN_INVARIANT_OF_WHILE),
						   get_base_name()->get_value()) ;
			  // On arrete les degats ici
			  return ;
			}
		}

	  if (done == FALSE)
		{
		  cur = cur->get_father() ;
		}
	}

  // Ici on sait qu'on est dans un NODE_BECOMES_SUCH_THAT ou un NODE_WHILE
  // La suite des controles est semantique : elle est realisee dans
  // semantic_check_dollar_zero_integrity
  // On ne stocke pas cette information (pas de champ dispo) - de toutes
  // facons c'est facile a recalculer et on ne le fait que pour les
  // idents avec des $0 -- je pense que c'est mieux que de perdre 4 octets
}

//
// Controles semantiques d'utilisation du $0
//
void T_ident::semantic_check_dollar_zero_integrity(void)
{
#ifdef DEBUG_DOLLAR_ZERO
  TRACE2("T_ident(%x : \"%s\")::semantic_check_dollar_zero_integrity",
		 this,
		 name->get_value()) ;
#endif

  // On cherche a nouveau le contexte d'utilisation
  // Cf commentaire en fin de fonction precedente
  T_item *cur = get_father() ;

  // Pour savoir si on a trouve le WHILE ou le BECOMES SUCH THAT
  int done = FALSE ;

  // Le code est offensif car on a deja fait les verifications syntaxiques
  while (done == FALSE)
	{
	  ASSERT(cur != NULL) ;
	  ASSERT(cur->get_node_type() != NODE_OPERATION) ;

	  if (cur->get_node_type() == NODE_BECOMES_SUCH_THAT)
		{
		  done = TRUE ;
		}
	  else if (cur->get_node_type() == NODE_WITNESS)
		{
		  done = TRUE ;
		}
	  else if (cur->get_node_type() == NODE_WHILE)
		{
		  done = TRUE ;
		}

	  if (done == FALSE)
		{
		  cur = cur->get_father() ;
		}
	}


  // Ici on sait qu'on est dans un NODE_BECOMES_SUCH_THAT ou un NODE_WHILE
  // CTRL REF: CSYN 2
  ASSERT(cur != NULL) ;
  TRACE2("cur %x %s", cur, cur->get_class_name()) ;
  ASSERT(    (cur->get_node_type() == NODE_WHILE)
		  ||   (cur->get_node_type() == NODE_WITNESS)
		  || (cur->get_node_type() == NODE_BECOMES_SUCH_THAT) ) ;
  if (cur->get_node_type() == NODE_BECOMES_SUCH_THAT)
	{
	  // Becomes such that
	  // this doit faire partie des variables de la substitution
	  T_becomes_such_that *subst = (T_becomes_such_that *)cur ;

	  T_ident *cur_ident = subst->get_lvalues() ;

	  while (cur_ident != NULL)
		{
		  TRACE4("cur_ident %x:%s base_name %x:%s",
				 cur_ident->get_name(),
				 cur_ident->get_name()->get_value(),
				 get_base_name(),
				 get_base_name()->get_value()) ;
		  if (cur_ident->get_name() == get_base_name())
			{
			  // Ok on l'a trouve
			  return ;
			}
		  cur_ident = (T_ident *)cur_ident->get_next() ;
		}

	  // Ici, on n'a pas trouve l'identificateur : erreur
	  semantic_error(this,
					 CAN_CONTINUE,
					 get_error_msg(E_BEFORE_ONLY_IF_VAR_PART_OF_BECOMES_SUCH_THAT),
					 get_base_name()->get_value()) ;
	}
  else if (cur->get_node_type() == NODE_WITNESS)
	{
	  //TODO LB
	}
 else
	{
	  // While
	  // this doit etre une variable de l'abstraction du composant
	  // et doit etre modifiee dans la boucle
	  T_while *subst = (T_while *)cur ;

	  // On verifie que this est une variable de l'abstraction
	  T_machine *machine = NULL ;
	  subst->find_machine_and_op_def(&machine, NULL) ;
	  ASSERT(subst != NULL) ;
	  T_machine *abstraction = machine->get_ref_abstraction() ;
	  TRACE2("on cherche si %s est defini dans %s",
			 get_base_name()->get_value(),
			 (abstraction == NULL)
			 ? "null" : abstraction->get_machine_name()->get_name()->get_value()) ;
	  if (    (abstraction == NULL)
		   || (abstraction->find_variable(get_base_name()) == NULL) )
		{
		  const char *mach_name ;
		  mach_name = machine->get_machine_name()->get_name()->get_value() ;
		  const char *abst_name ;
		  abst_name = (abstraction == NULL)
			? "" : abstraction->get_machine_name()->get_name()->get_value() ;
		  semantic_error(this,
						 CAN_CONTINUE,
						 get_error_msg(E_BEFORE_ONLY_IF_VAR_OF_WHILE_SPECIFIED_IN_ABSTRACTION),
						 get_base_name()->get_value(),
						 mach_name,
						 abst_name) ;

		  // On arrete les degats ici
		  return ;
		}

	  // SL 27/01/99 la variable doit aussi etre collee
	  // SL 02/02/99 ou : homonyme a une variable de l'abstraction
	  int is_glued = FALSE ;
	  int is_from_abstraction = FALSE ;
	  // Definition de l'ident : this est l'ident avec le $0, et son
	  // def atterit dans la spec (et pas dans l'implementation
	  // courante)
	  T_ident *cur_def = get_def() ;
	  // On cherche cet ident dans l'implementation courante
	  T_machine *cur_machine = get_ref_machine() ;
	  T_ident *imp_def = cur_machine->find_variable(cur_def->get_name()) ;

	  if (imp_def != NULL)
		{
		  TRACE2("imp_def %x glued %x", imp_def, imp_def->get_ref_glued()) ;
		  if (imp_def->get_ref_glued() != NULL)
			{
			  // Premier cas : c'est colle
			  TRACE0("OK COLLE") ;
			  is_glued = TRUE ;
			}
		  else
			{
			  // Deuxieme cas : c'est homonyme a une variable de l'abstraction
			  T_machine *cur_abstraction = cur_machine->get_ref_abstraction() ;
			  ASSERT(cur_abstraction != NULL) ;
			  TRACE2("cur_machine %x %s",
					 cur_abstraction,
					 cur_abstraction->get_machine_name()->get_name()->get_value()) ;
			  if (cur_abstraction->find_variable(cur_def->get_name()) != NULL)
				{
				  // Ok trouve. Il faut en plus que la variable soit concrete
				  if (imp_def->get_ident_type() == ITYPE_CONCRETE_VARIABLE)
					{
					  is_from_abstraction = TRUE ;
					}
				}
			}
		}

	  if ( (is_glued == FALSE) && (is_from_abstraction == FALSE) )
		{
		  semantic_error(this,
						 CAN_CONTINUE,
						 get_error_msg(E_BEFORE_ONLY_IF_VAR_OF_WHILE_GLUED_OR_FROM_ABSTRACTION),
						 get_base_name()->get_value()) ;

		  // On s'arrete la
		  return ;
		}

	  // Fin SL 27/01/99

	  // On verifie que this est modifie dans la substitution
	  // SL 27/01/99 suite reunion 26/01 CG,DS,FB ce n'est
	  // pas une erreur mais un warning
	  TRACE0("on verifie qu'on est modifie dans le WHILE") ;
	  int modified = FALSE ;
	  T_substitution *cur_while_body = subst->get_body() ;

	  while ( (modified == FALSE) && (cur_while_body != NULL) )
		{
		  if (cur_while_body->modifies(get_definition()) == TRUE)
			{
			  modified = TRUE ;
			}
		  else
			{
			  cur_while_body = (T_substitution *)cur_while_body->get_next() ;
			}
		}
	  // FL, le 12/10/2000
	  // Suppression de ce warning qui n'est plus valide :
	  // Voir Gub 2550
#if 0
	  if (modified == FALSE)
		{
		  semantic_warning(this,
						   BASE_WARNING,
						   get_warning_msg(W_USE_BEFORE_ON_VAR_NOT_MODIFIED_IN_WHILE),
						   get_base_name()->get_value()) ;
		}
#endif  // #if 0

	  // Ok !
	}
}


// Fonction de haut niveau qui effectue la verification syntaxique des $0
void syntaxic_check_dollar_zero(T_betree *betree)
{
#ifdef DEBUG_DOLLAR_ZERO
  TRACE0(">> syntaxic_check_dollar_zero") ;
  ENTER_TRACE ;
#endif // DEBUG_DOLLAR_ZERO

  T_list_link *cur_dollar_link = betree->get_root()->get_dollar_zero_identifiers() ;

  while (cur_dollar_link != NULL)
	{
	  T_ident *cur_dollar_ident = (T_ident *)cur_dollar_link->get_object() ;
	  cur_dollar_ident->syntaxic_check_dollar_zero_integrity() ;

#ifdef DEBUG_DOLLAR_ZERO
	  TRACE1("cur_dollar_ident$0 %x", cur_dollar_ident) ;
#endif // DEBUG_DOLLAR_ZERO

	  cur_dollar_link = (T_list_link *)cur_dollar_link->get_next() ;
	}

#ifdef DEBUG_DOLLAR_ZERO
  EXIT_TRACE ;
  TRACE0("<< syntaxic_check_dollar_zero") ;
#endif // DEBUG_DOLLAR_ZERO
}

// Fonction de haut niveau qui effectue la verification semantique des $0
void semantic_check_dollar_zero(T_betree *betree)
{
#ifdef DEBUG_DOLLAR_ZERO
  TRACE0(">> semantic_check_dollar_zero") ;
  ENTER_TRACE ;
#endif // DEBUG_DOLLAR_ZERO

  T_list_link *cur_dollar_link = betree->get_root()->get_dollar_zero_identifiers() ;

  while (cur_dollar_link != NULL)
	{
	  T_ident *cur_dollar_ident = (T_ident *)cur_dollar_link->get_object() ;
	  cur_dollar_ident->semantic_check_dollar_zero_integrity() ;

#ifdef DEBUG_DOLLAR_ZERO
	  TRACE1("cur_dollar_ident$0 %x", cur_dollar_ident) ;
#endif // DEBUG_DOLLAR_ZERO

	  cur_dollar_link = (T_list_link *)cur_dollar_link->get_next() ;
	}

#ifdef DEBUG_DOLLAR_ZERO
  EXIT_TRACE ;
  TRACE0("<< semantic_check_dollar_zero") ;
#endif // DEBUG_DOLLAR_ZERO
}

//
// Fin du fichier
//
