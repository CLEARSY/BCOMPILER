/******************************* CLEARSY **************************************
* Fichier : $Id: i_valchk.cpp,v 2.0 2003-01-14 10:12:44 lv Exp $
*
* (C) 2008 CLEARSY
*
* Description :		B0 Checker, Controles semantiques de valuation,
*                   d'instanciation, et d'initialisation
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
RCS_ID("$Id: i_valchk.cpp,v 1.13 2003-01-14 10:12:44 lv Exp $") ;

/* Includes systeme */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>


/* Includes Composant Local */
#include "c_api.h"

/* Includes des donnees globales */

#ifdef B0C
#include "i_aux.h"
#include "i_ctx.h"
#include "i_listid.h"
#include "i_globdat.h"

//
// Reference avant comparaison syntaxique de valuation
//
static int expr_syntaxic_equal(T_expr *expr, T_expr *ref_expr);

//
//
// Controles sur les valuations des constantes concretes et des ensembles
// abstraits.
//
//

void T_object::valuation_check(T_list_ident **list_ident)
{
  TRACE2("T_object(%x:%s)::valuation_check() : ON NE FAIT RIEN",
		 this,
		 get_class_name()) ;
}

void T_betree::valuation_check(T_list_ident **list_ident)
{
  if (root != NULL)
	{
	  root->valuation_check(list_ident);
	}
}

void T_machine::valuation_check(T_list_ident **list_ident)
{
  TRACE1("AB MACHINE %s",machine_name->get_name()->get_value());

  // On va verifier R_VAL 0-1 et 0-2
  // On positionne la variable globale check a 2
  set_check(B0CHK_VAL);

  // On construit la liste des identificateurs a valuer
  // Cette liste est stockee dans la variable globale current_list_ident
  T_list_ident *list = new T_list_ident();

  // En implementation, les ensembles abstraits
  // de la specification doivent etre values

  if (this->get_machine_type() == MTYPE_IMPLEMENTATION)
	{
	  T_ident *cur_spec_set = this->get_sets() ;
	  while (cur_spec_set != NULL)
		{
		  if ( (cur_spec_set->get_implemented_by() == NULL)
			   &&
 			   (cur_spec_set->get_ident_type() == ITYPE_ABSTRACT_SET) )
			{
			  // On ne value pas les ensembles colles
			  list->add_identifier(cur_spec_set);
			}
		  cur_spec_set = (T_ident *)cur_spec_set->get_next() ;
		}

	  // De plus, on value les constantes concretes non collees
	  // avec une autre constante concrete
	  T_ident *cur_cst = this->get_concrete_constants() ;
	  while (cur_cst != NULL)
		{
		  if (cur_cst->get_implemented_by() == NULL)
			{
			  // Pas besoin de valuer les constantes collees
			  list->add_identifier(cur_cst);
			}
		  cur_cst = (T_ident *)cur_cst->get_next() ;
		}
	}

  // On initialise la variable globale des identificateurs a valuer
  set_current_list_ident(list);

  // On cree la liste des identificateurs values (vide pour l'instant)
  T_list_ident *list_valued_ident = new T_list_ident;

  T_valuation *cur_valuation = first_value;
  while (cur_valuation != NULL)
  {
	cur_valuation->valuation_check(&list_valued_ident);

	cur_valuation = (T_valuation *) cur_valuation->get_next();
  }

  // On repositionne le flag global a 0 (aucune verification)
  set_check(B0CHK_DEFAULT);
}

void T_valuation::valuation_check(T_list_ident **list_ident)
{
  ASSERT (value != NULL);
  // Le contexte pour le controles sur les valuations est
  // B0CTX_VALUATION_CONTEXT
  T_type *type = ident->B0_get_definition()->get_original_B_type();
  if (type == NULL)
	{
	  return;
	}

  // Cas d'une constante scalaire:
  //
  // CTRL Ref : RVAL 1-1
  if ( (type->is_a_base_type() == TRUE) &&
	   (ident->B0_get_definition()->get_ident_type() == ITYPE_CONCRETE_CONSTANT) )
	{
	  if (value->syntax_check_is_a_term(list_ident) == FALSE)
		{
		  // Erreur B0 : la valuation n'est pas valide
		  B0_semantic_error(value,
							CAN_CONTINUE,
							get_error_msg(E_B0_INVALID_EXPR_FOR_VALUATION),
							make_class_name(value),
							ident->B0_get_definition()->get_ident_type_name(),
							ident->get_name()->get_value()) ;
		}
	}
  else if (get_disable_scalar_set_constant_valuation_b0_check() == TRUE &&
		   ident->get_B0_type() != NULL &&
		   ident->get_B0_type()->get_node_type() == NODE_B0_INTERVAL_TYPE)
	{
	  // EVOL 2812:
	  // On ne contr�le pas la valuation pour une constante concr�te
	  // ensemble de scalaire (conform�ment � la ressource.
	}

  // Cas d'une constante concrete non scalaire ou d'un ensemble abstrait
  // Verification syntaxique : le controle sur le type fait le reste...
  // (crie avant)
  //
  // CTRL Ref : RVAL 2-1 a RVAL 2-3
  // CTRL Ref : RVAL 3-1 a RVAL 3-4
  // CTRL Ref : RVAL 5-1 a RVAL 5-2
  else if ( (value->syntax_check_is_a_range(list_ident,
											B0CTX_VALUATION_CONTEXT) == FALSE) &&
			(value->syntax_check_is_an_array(list_ident) == FALSE) &&
			(value->syntax_check_is_an_extension_record(list_ident) == FALSE) )
	{
	  // Erreur B0 : la valuation n'est pas valide
	  int is_invalid = TRUE ;

	  // Sauf (derniere chance) en mode B0_enable_typing_identifiers,
	  // si la partie droite est un tableau ou un record qui definit un type
	  if (    (get_B0_enable_typing_identifiers() == TRUE)
		   && (ident->get_B0_type() != NULL)
		   && (ident->get_B0_type()->get_is_a_type_definition() == TRUE) )
		{
		  is_invalid = FALSE ;
		}

	  if (is_invalid == TRUE)
		{
		  B0_semantic_error(value,
							CAN_CONTINUE,
							get_error_msg(E_B0_INVALID_EXPR_FOR_VALUATION),
							make_class_name(value),
							ident->B0_get_definition()->get_ident_type_name(),
							ident->get_name()->get_value()) ;
		}
	}


  //
  // Controle de compatibilite des tableaux...
  //

  // Sauf : en mode B0_enable_typing_identifiers (HIA) ou c'est a la
  // charge du programmeur
  if (    (get_B0_disable_array_compatibility_check() != TRUE)
	   && (get_B0_enable_typing_identifiers() != TRUE) )
	{
	  if ( ( (ident->get_original_B_type()->is_an_array() == TRUE) ||
			 (ident->get_original_B_type()->is_a_record() == TRUE) ) &&

		   ( (value->get_original_B_type()->is_an_array() == TRUE) ||
			 (value->get_original_B_type()->is_a_record() == TRUE) ) )
		{
		  // C'est un tableau ; on verifie la compatibilite
		  T_type *left_type = ident->get_original_B_type();
		  T_type *right_type = value->get_original_B_type();

		  if (left_type->is_array_compatible_with(right_type,
												  value) == FALSE)
			{
			  B0_semantic_error(this,
								CAN_CONTINUE,
								get_error_msg(E_B0_INCOMPATIBLE_ARRAYS_BETWEEN_LHS_AND_RHS));
			}
		}
	}



  // Si l'identificateur est deja value explicitement dans la clause
  // VALUATION, ou
  // si il ne figure pas dans la liste des identificateurs a valuer
  // (identificateur colle), on emet un message d'erreur
  //
  // CTRL Ref : RVAL 0-1
  if ( ((*list_ident)->find_identifier(ident) == TRUE) ||
	   (get_current_list_ident()->find_identifier(ident) == FALSE) )
	{
	  // ERREUR B0 : On ne peut valuer deux fois un identificateur

	  B0_semantic_error(ident,
						CAN_CONTINUE,
						get_error_msg(E_B0_IDENTIFIER_ALREADY_VALUED),
						ident->B0_get_definition()->get_ident_type_name(),
						ident->get_name()->get_value()) ;
	  return;
	}
  (*list_ident)->add_identifier(ident);
}



//
//
// Controles sur les instanciations de parametres formels de machines
//
//

void T_object::instanciation_check(void)
{
  TRACE2("T_object(%x:%s)::instanciation_check() : ON NE FAIT RIEN",
		 this,
		 get_class_name()) ;
}

void T_betree::instanciation_check(void)
{
  if (root != NULL)
	{
	  root->instanciation_check();
	}
}

void T_machine::instanciation_check(void)
{
  if (machine_type != MTYPE_IMPLEMENTATION)
	{
	  return;
	}

  instanciation_check_list(first_imports);
  instanciation_check_list(first_extends);
}

void T_used_machine::instanciation_check(void)
{
  // CTRL Ref : RINST 1-1 a RINST 1-6
  // CTRL Ref : RINST 2-1 a RINST 2-4
  instanciation_check_list(first_use_param);
}


//
//
// Fonctions de controle des cycles dans les initialisations
//
// CTRL Ref : RINIT 1-1
void T_object::initialisation_check(T_list_ident **list_ident)
{
  TRACE2("T_object(%x:%s)::initialisation_check() : ON NE FAIT RIEN",
		 this,
		 get_class_name()) ;
}

void T_betree::initialisation_check(T_list_ident **list_ident)
{
  // On ne fait la verification RINIT 1-1 que dans les implementations
  // Pour le reste, ce sera des evolutions...
  if ( (root != NULL) && (check_is_an_implementation() == TRUE) )
	{
	  root->initialisation_check(list_ident);
	}
}

void T_machine::initialisation_check(T_list_ident **list_ident)
{
  // On va verifier RINIT 1-1
  // On positionne la variable globale check a 1
  set_check(B0CHK_INIT);

  // On construit la liste des identificateurs a initialiser
  // Cette liste est stockee dans la variable globale current_list_ident
  T_list_ident *list = new T_list_ident();

  // On initialise les variables concretes non collees
  T_ident *cur_var = this->get_concrete_variables() ;
  while (cur_var != NULL)
	{
	  if (cur_var->get_implemented_by() == NULL)
		{
		  // Pas besoin de valuer les variables impl�ment�e � l'exterieur
		  list->add_identifier(cur_var);
		}
	  cur_var = (T_ident *)cur_var->get_next() ;
	}


  // On initialise la variable globale des identificateurs a initialiser
  set_current_list_ident(list);

  // On cree la liste des identificateurs initialises (vide pour l'instant)
  T_list_ident *list_initialised_ident = new T_list_ident();

  // Controle des substitutions de la clause INITIALISATION
  initialisation_check_list(first_initialisation,
							&list_initialised_ident);


  // On repositionne le flag global a 0 (aucune verification)
  set_check(B0CHK_DEFAULT);
}


void T_begin::initialisation_check(T_list_ident **list_ident)
{
  initialisation_check_list(first_body,
							list_ident);
}

void T_var::initialisation_check(T_list_ident **list_ident)
{
  initialisation_check_list(first_body,
							list_ident);
}

void T_affect::initialisation_check(T_list_ident **list_ident)
{
  if ( (first_value->syntax_check_is_a_term(list_ident) == TRUE) ||
	   (first_value->syntax_check_is_an_array(list_ident) == TRUE) );

  // Si l'affectation concerne un identificateur, et que cet identificateur
  // n'est pas dans la liste des initialises, et qu'il est dans la liste
  // des identificateurs a initialiser, on le rajoute
  if ( (first_name->get_node_type() == NODE_IDENT) &&
	   ((*list_ident)->find_identifier((T_ident *)first_name) == FALSE) &&
	   (get_current_list_ident()->find_identifier((T_ident *)first_name) == TRUE) )
	{
	  (*list_ident)->add_identifier((T_ident *)first_name);
	}
}

void T_op_call::initialisation_check(T_list_ident **list_ident)
{
  syntax_check_list_is_an_op_in_param(first_in_param,
									  list_ident);

  // Si on a en sortie des identificateurs, et que ces identificateurs
  // ne sont pas dans la liste des initialises, et qu'ils sont dans la liste
  // des identificateurs a initialiser, on les rajoute
  T_item *cur_item = first_out_param;
  while (cur_item != NULL)
	{
	  if ((cur_item->get_node_type() == NODE_IDENT) &&
		  ((*list_ident)->find_identifier((T_ident *)cur_item) == FALSE)  &&
		  (get_current_list_ident()->find_identifier((T_ident *)cur_item) == TRUE))
		{
		  (*list_ident)->add_identifier((T_ident *)cur_item);
		}
	  cur_item = (T_item *) cur_item->get_next();
	}
}

void T_if::initialisation_check(T_list_ident **list_ident)
{
  // Verification du predicat
  if (cond->syntax_check_is_a_B0_predicate(list_ident) == TRUE) ;

  // On clone la T_list_ident pour la partie THEN
  T_list_ident *cur_list_ident = new T_list_ident(*list_ident);
  initialisation_check_list(first_then_body,
							&cur_list_ident);

  // On gere les parties ELSE
  initialisation_get_intersection(first_else,
								  list_ident);

  // On prend l'intersection entre *list_ident (resultat de l'intersection
  // des parties ELSE) et cur_list_ident (resultat de la partie THEN)
  (*list_ident)->make_intersection(cur_list_ident);

}

void T_else::initialisation_check(T_list_ident **list_ident)
{
  // Verification du predicat
  if (cond != NULL)
	{
	  cond->syntax_check_is_a_B0_predicate(list_ident);
	}

  initialisation_check_list(first_body,
							list_ident);
}

void T_case::initialisation_check(T_list_ident **list_ident)
{
  // On gere les parties CASE_ITEM

  // On teste si il existe une partie default
  if (last_case_item->get_literal_values() != NULL)
	{
	  // Il n'y a pas de default
	  // A la sortie du case, il n'y aura donc pas de nouvelle
	  //initialisation effective

	  // Pour ce faire, on lance le controle avec un clone de list_ident
	  // (on passe quand meme en revue les parties droites d'affectation...)
	  T_list_ident *clone = new T_list_ident(*list_ident);
	  initialisation_get_intersection(first_case_item,
									  &clone);
	  return;
	}
  // Il existe un cas default (cas sans probleme)
  initialisation_get_intersection(first_case_item,
								  list_ident);
}

void T_case_item::initialisation_check(T_list_ident **list_ident)
{
  initialisation_check_list(first_body,
							list_ident);
}

void T_assert::initialisation_check(T_list_ident **list_ident)
{
  initialisation_check_list(first_body,
							list_ident);
}

void T_label::initialisation_check(T_list_ident **list_ident)
{
  initialisation_check_list(first_body,
                                                        list_ident);
}

void T_witness::initialisation_check(T_list_ident **list_ident)
{
  initialisation_check_list(first_body,
							list_ident);
}

void T_jumpif::initialisation_check(T_list_ident **list_ident)
{
}

void T_while::initialisation_check(T_list_ident **list_ident)
{
  // Verification du predicat
  if (cond->syntax_check_is_a_B0_predicate(list_ident) == TRUE) ;

  // On ne peut pas savoir si l'on passe dans la boucle : on considere
  // qu'elle n'initialise aucun identificateur
  // Pour ce faire, on lance le controle avec un clone de list_ident
  // (on passe quand meme en revue les parties droites d'affectation...)
  T_list_ident *clone = new T_list_ident(*list_ident);
  initialisation_check_list(first_body,
							&clone);

}


//
// Controle des valuations pour BOM
//


// Controle B0 sur les valuations d'ensembles A cette ocasion on
// accroche au champ BOM_type le type de la valeur La passe s�mantique
// assure d�j� que la valuation est un ensemble d'entier.  On se
// contente alors de v�rifier uniquement que la valuation est BOOL un
// identificateur qui est une constante concr�te ou un ident qui est
// un ensemble dif�r�
void B0_OM_sets_valuation_check(T_valuation *first_valuation)
{
  T_valuation *cur_valuation = first_valuation;

  while (cur_valuation != NULL)
	{
	  T_ident *def_set = cur_valuation->get_ident()->get_def();
	  if (def_set->get_ident_type() == ITYPE_ABSTRACT_SET)
		{
		  int valuation_OK = FALSE;
		  T_expr *value = cur_valuation->get_value();
		  // Seul cas valide possible la valuation est un ident
		  if (value->is_an_ident() == TRUE)
			{
			  T_ident *ident_value = value->make_ident();

			  // Premier cas une constante concr�te
			  if(ident_value->get_def()->get_ident_type() ==
				 ITYPE_CONCRETE_CONSTANT)
				{
				  // On ne g�n�re pas le message d'erreur par defaut
				  // des valuations d'ensemble diff�r�s mais
				  // �ventuellement un message d'erreur sp�cifique
				  // pour la valuation avec une constante du meme
				  // module
				  valuation_OK = TRUE;

				  T_machine *cons_mach = NULL;
				  T_machine *set_mach = NULL;
				  T_op *op = NULL;

				  ident_value->get_def()->
					find_machine_and_op_def(&cons_mach, &op);
				  def_set->find_machine_and_op_def(&set_mach, &op);

				  if (cons_mach == set_mach)
					{
					  B0_semantic_error(cur_valuation->get_ident(),
										CAN_CONTINUE,
										get_error_msg(E_B0_SET_VALUED_WITH_CONSTANTS_FROM_SAME_MODULE),
										def_set->get_name()->get_value(),
										ident_value->get_name()->get_value());


					}
				  T_ident *cur_ident = def_set;
				  while (cur_ident->get_def() != NULL)
					{
					  cur_ident = cur_ident->get_def();
					}
				  cur_ident->set_BOM_type(ident_value->get_def());

				}
			  // Deuxi�me cas un ensemble dif�r�
			  else if(ident_value->get_def()->get_ident_type() ==
					  ITYPE_ABSTRACT_SET)
				{
				  valuation_OK = TRUE;
				  TRACE2("attribution de la valuation au set %s %p",
						 def_set->get_name()->get_value(),
						 def_set);
				  T_ident *cur_ident = def_set;
				  while (cur_ident->get_def() != NULL)
					{
					  cur_ident = cur_ident->get_def();
					}
				  cur_ident->set_BOM_type(ident_value->get_def());
				}
			}
		  if (valuation_OK == FALSE)
			{
			  B0_semantic_error(cur_valuation->get_ref_lexem(),
								CAN_CONTINUE,
								get_error_msg(E_B0_INVALID_VALUATION_OF_ABSTRACT_SET ),
								def_set->get_name()->get_value());
			}
		}

	  cur_valuation = (T_valuation *)cur_valuation->get_next();
	}
}

//
// V�rification de la valuation des constantes
//
void B0_OM_constants_valuation_check(T_valuation *first_valuation)
{
  TRACE0("B0_OM_constants_valuation_check");
  T_valuation *cur_valuation = first_valuation;

  while (cur_valuation != NULL)
	{
	  T_ident *def_constant = cur_valuation->get_ident()->get_def();
	  if (def_constant->get_ident_type() == ITYPE_CONCRETE_CONSTANT)
		{
		  TRACE1("concrete_constante: %s",
				 def_constant->get_name()->get_value());
		  if (def_constant->is_a_BOM_type() == TRUE)
			{
			  TRACE1("%p est un type",
					 def_constant);
			  // V�rifie que la valuation est d�finie � l'identique
			  // syntaxiquement
			  ASSERT(def_constant->get_BOM_type()->is_an_expr());
			  if (expr_syntaxic_equal(def_constant->get_BOM_type()->make_expr(),
							  cur_valuation->get_value()) == FALSE)
				{
				  B0_semantic_error(cur_valuation,
									CAN_CONTINUE,
									get_error_msg(E_B0_INVALID_TYPE_VALUATION),
									def_constant->get_name()->get_value());
				}
			}
		  else
			{
			  TRACE1("%p une constante ",
					 def_constant);
			  // On v�rifie juste que les types sont coh�rents
			  // On r�cup�re le type de la constante
			  if (def_constant->get_BOM_type()->is_an_ident() == TRUE)
				{
				  T_ident *B0_OM_type = def_constant->get_BOM_type()
					->make_ident();
				  T_expr *value = cur_valuation->get_value();
				  value->B0_check_expr(B0_OM_type,
									   cur_valuation->get_ref_lexem(),
									   value->get_ref_lexem(),
									   B0_CTX_VALUATION);
				}
			  else
				{
				  // recup�ration sur erreur
				}
			}

		}

	  cur_valuation = (T_valuation *)cur_valuation->get_next();
	}
  TRACE0("<--B0_OM_constants_valuation_check");
}

//
// Fonction pour comparer la valeur de d�finition d'une constante
// type et sa valuation
// Seuls cas:
//     ident
//     0..b-->ident
//
static int expr_syntaxic_equal(T_expr *expr, T_expr *ref_expr)
{
  TRACE0("expr_syntaxic_equal");
  int result = FALSE;

  // Tout d'abord, un cas particulier pour les identificateurs.
  if (expr->is_an_ident() == TRUE)
	{
	  if (ref_expr->is_an_ident() == TRUE)
		{
		  // Tous les deux des idents, on compare les noms.
		  T_symbol *name = expr->make_ident()->get_name();
		  T_symbol *ref_name = ref_expr->make_ident()->get_name();
		  result = name->compare(ref_name);
		  TRACE0("deux identificateurs");
		}
	  else
		{
		  // Pas m��me nature des deux c��t��s.
		  result = FALSE;
		  TRACE0("pas meme nature");
		}
	}
  else if (expr->get_node_type() != ref_expr->get_node_type())
	{
	  result  = FALSE;
	  TRACE0("pas meme nature");
	}
  else
	{
	  switch(expr->get_node_type())
		{
		case NODE_RELATION:
		  {
			T_relation *rela_expr = (T_relation *)expr;
			T_relation *rela_ref_expr = (T_relation *)ref_expr;
			result = (expr_syntaxic_equal(rela_expr->get_src_set(),
								  rela_ref_expr->get_src_set()) == TRUE
					  &&
					  expr_syntaxic_equal(rela_expr->get_dst_set(),
								  rela_ref_expr->get_dst_set()) == TRUE
					  &&
					  rela_expr->get_relation_type()
					  == rela_ref_expr->get_relation_type());
			break;
		  }
		case NODE_BINARY_OP:
		  {
			ASSERT(((T_binary_op *)expr)->get_op1()->is_an_expr() == TRUE
				   &&
				   ((T_binary_op *)expr)->get_op2()->is_an_expr() == TRUE
				   &&
				   ((T_binary_op *)ref_expr)->get_op1()->is_an_expr() == TRUE
				   &&
				   ((T_binary_op *)ref_expr)->get_op2()->is_an_expr() == TRUE);

			T_expr *expr_op1 = ((T_binary_op *)expr)->get_op1()->make_expr();
			T_expr *expr_op2 = ((T_binary_op *)expr)->get_op2()->make_expr();
			T_expr *ref_expr_op1 = ((T_binary_op *)ref_expr)->get_op1()->make_expr();
			T_expr *ref_expr_op2 = ((T_binary_op *)ref_expr)->get_op2()->make_expr();

			result = (expr_syntaxic_equal(expr_op1,ref_expr_op1) == TRUE
					  &&
					  expr_syntaxic_equal(expr_op2,ref_expr_op2) == TRUE
					  &&
					  ((T_binary_op *)expr)->get_operator() ==
					  ((T_binary_op *)ref_expr)->get_operator());
			break;
		  }
		case NODE_LITERAL_INTEGER:
		  {
			int val_expr;
			int val_ref_expr;
			val_expr = ((T_literal_integer *)expr)->get_value()->
			  get_int_value();
			val_ref_expr = ((T_literal_integer *)ref_expr)->get_value()->
			  get_int_value();
			result = (val_expr == val_ref_expr);
			break;
		  }
		case NODE_ARRAY_ITEM:
		  {
			ASSERT(((T_array_item *)expr)->get_array_name()
				   ->is_an_expr() == TRUE);
			ASSERT(((T_array_item *)ref_expr)->get_array_name()
				   ->is_an_expr() == TRUE);

			T_expr *array_name = ((T_array_item *)expr)->get_array_name()
			  ->make_expr();
			T_expr *ref_array_name = ((T_array_item *)ref_expr)->get_array_name()
			  ->make_expr();

			result = expr_syntaxic_equal(array_name,
										 ref_array_name);
			if (result == TRUE)
			  {
				// On v�rifie les indices
				T_expr *cur_ind = ((T_array_item *)expr)->get_indexes();
				T_expr *cur_ref_ind = ((T_array_item *)ref_expr)->get_indexes();
				while (cur_ind != NULL
					   &&
					   cur_ref_ind != NULL
					   &&
					   result == TRUE)
				  {
					result = expr_syntaxic_equal(cur_ind,
												 cur_ref_ind);
					cur_ind = (T_expr *)cur_ind->get_next();
					cur_ref_ind = (T_expr *)cur_ref_ind->get_next();
				  }
				if (result == TRUE)
				  {
					result = (cur_ind == NULL
							  &&
							  cur_ref_ind == NULL);
				  }
			  }

			break;
		  }
		default:
		  TRACE1("type du noeud: %d",expr->get_node_type());
		  ASSERT(FALSE);
		}
	}
  TRACE3("%s<--expr_syntaxic_equal(%p,%p)",
		 result == TRUE ? "true" :"false",
		 expr,
		 ref_expr);

  return result;
}


//
// Fin du fichier
//

#endif // B0C

