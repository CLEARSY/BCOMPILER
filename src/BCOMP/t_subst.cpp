/******************************* CLEARSY **************************************
* Fichier : $Id: t_subst.cpp,v 2.0 2002-07-12 13:14:28 cm Exp $
* (C) 2008 CLEARSY
*
* Description :		Compilateur B
*					Type check pour les subtitutions
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
RCS_ID("$Id: t_subst.cpp,v 1.62 2002-07-12 13:14:28 cm Exp $") ;

/* Includes systeme */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

#include<deque>

/* Includes Composant Local */
#include "c_api.h"
#include "v_ldreq.h"
#include "v_toplvl.h"
#include "c_tstamp.h"
#include "t_misc.h"

#ifdef B0C
#include "i_aux.h"
#include "i_ctx.h"
#include "i_listid.h"
#include "i_globdat.h"
#endif //B0C

// Forward
static void check_no_double_literal_value(T_case_item*list_case_item) ;

//
//	}{	Classe T_affect
//

// Fonction generique de type check
// Substitution du type A := B
// CTRL Ref : ITYPE 2-1
void T_affect::type_check(void)
{
  TRACE1("DEBUT T_affect(%x)::type_check", this) ;
  ENTER_TRACE ;

  // Verification de l'unicite des identificateurs en partie gauche
  check_no_double_ident(first_name);

  // Il faut deja autant d'identificateurs que de valeurs
  // On le verifie. Au passage, on calcule les types
  size_t nb_names = 0 ;
  size_t nb_values = 0 ;

  ENTER_TRACE ;
  TRACE2("> TC names (%x, %x)", first_name, last_name) ;
  T_expr *cur_name = first_name ;
  while (cur_name != NULL)
	{
	  nb_names ++ ;
	  cur_name->make_type() ;
	  cur_name = (T_expr *)cur_name->get_next() ;
	}

  T_expr *cur_value = first_value ;
  TRACE2("> TC values (%x, %x)", first_value, last_value) ;
  while (cur_value != NULL)
	{
	  nb_values ++ ;
	  cur_value->make_type() ;
	  //GP 07/01/99
	  //CTRL Ref : ETYPE 1
	  cur_value->check_type() ;
	  cur_value = (T_expr *)cur_value->get_next() ;
	}

  TRACE0("> check") ;
  if (nb_values != nb_names)
	{
	  semantic_error(first_name,
					 CAN_CONTINUE,
					 get_error_msg(E_UNBALANCED_AFFECTATION),
					 nb_names,
					 nb_values) ;
	  return ;
	}

  // Ici on sait qu'il y a autant de names que de values
  cur_name = first_name ;
  cur_value = first_value ;

  while (cur_name != NULL)
	{
	  ASSERT(cur_value != NULL) ;
	  if (cur_value->get_B_type() == NULL)
		{
		  // reprise sur erreur
		  cur_name->set_B_type(NULL, NULL) ;

		}
	  else
		{
                    /*
                       Dans la cas ou la valeur affectee n'est pas completement typee,
                       on doit pouvoir inferer le type a partir de la cible de
                       l'affectation.
                    */
                    T_type *type;
                    if(cur_value->get_B_type()->has_wildcards()) {
                        if (cur_name->get_B_type() == NULL)
                        {
                          T_ident* id = (T_ident*) cur_name;
                          semantic_error_details(id->get_typing_location(),
                                                 get_error_msg(E_CAN_NOT_TYPE_WITH_A_GENERIC_TYPE),
                                                 id->get_name()->get_value(),
                                                 make_type_name(cur_value)->get_value());
                        }
                        else
                        {
                          type =
                              cur_name->get_B_type()->clone_type(cur_value,
                                                                 cur_value->get_betree(),
                                                                 cur_name->get_ref_lexem()) ;
                          cur_value->set_B_type(type, cur_name->get_ref_lexem());
                          if (type->get_typing_ident() != NULL)
                              {
                                  TRACE2("TYPING_IDENT recup typing_ident de %x ie %x",
                                         type,
                                         type->get_typing_ident())  ;
                                  cur_value->set_typing_ident(type->get_typing_ident()) ;
                              }
                          if (cur_value->get_B_type()->is_compatible_with(type) == FALSE &&
                              cur_name->is_an_ident() == TRUE) {
                              T_ident* id = (T_ident*) cur_name;
                              semantic_error_details(id->get_typing_location(),
                                                     get_error_msg(E_LOCATION_OF_IDENT_TYPE),
                                                     make_type_name(cur_name)->get_value(),
                                                     id->get_def_type_name(),
                                                     id->get_name()->get_value()) ;
                          }
                        }
                    }
                    else {
		       type =
			cur_value->get_B_type()->clone_type(cur_name,
												cur_name->get_betree(),
												cur_value->get_ref_lexem()) ;

		  if (cur_name->is_an_ident() == TRUE)
			{
			  TRACE3("on affecte le type %x:%s a %s",
					 type,
					 type->make_type_name()->get_value(),
					 cur_name->make_ident()->get_name()->get_value()) ;
			}

		  cur_name->set_B_type(type, cur_value->get_ref_lexem()) ;

		  if (type->get_typing_ident() != NULL)
			{
			  TRACE2("TYPING_IDENT recup typing_ident de %x ie %x",
					 type,
					 type->get_typing_ident())  ;
			  cur_name->set_typing_ident(type->get_typing_ident()) ;
			}

		  //
		  // Debut Correction anomalie 2249
		  //
		  // Il faut, le cas echeant,
		  // propager les affectations de labels
		  //
		  // ex : si x : struct(y : INT) alors dans le TC de x := rec(0) il faut
		  // enrichir le type de la partie droite pour savoir que le nom du label
		  // est y
		  //
		  // Cette info est necessaire par exemple pour le GNF
		  //
		  // Ce travail est fait par la methode is_compatible_with, mais dans
		  // le cas de l'affectation des substitutions, on clone le type
		  // (cur_value->get_B_type() dans type) avant ce travail pour pouvoir
		  // travailler tranquillement sur une copie privee
		  //
		  // Or dans le cas des records, cela nous prive de la
		  // recuperation des labels, qui a lieu dans type et pas dans
		  // cur_value->get_B_type()
		  //
		  // Pour recuperer ces labels, on appelle ::is_compatible_with entre
		  // cur_value->get_B_type() et type. On est sur que le resultat sera
		  // TRUE (et cela ne nous interesse pas) mais surtout on profite du fait
		  // que la methode re-propage ces labels dans cur_value->get_B_type()
		  // > ESSAI
		  (void)cur_value->get_B_type()->is_compatible_with(type) ;
		  // < ESSAI

		  //
		  // Fin correction anomalie 2249
		  //

		  type->unlink() ;
		}
		  //GP 25/01/99
		  //Interdir l'usage des strings
		  cur_name->check_string_uses(FALSE) ;
                }
	  // Couple (nom, valeur) suivant
	  cur_name = (T_expr *)cur_name->get_next() ;
	  cur_value = (T_expr *)cur_value->get_next() ;
	}

  EXIT_TRACE ;
  TRACE1("FIN T_affect(%x)::type_check", this) ;
}

// Fonction generique de type check
// Substitution du type A <-- B
// CTRL Ref : ITYPE 2-2
void T_op_call::type_check(void)
{
  TRACE1("DEBUT T_op_call(%x)::type_check", this) ;
  ENTER_TRACE ;

  // Verification de l'unicite des identificateur en partie gauche
  check_no_double_ident(first_out_param);

  if (op_name->is_an_ident() == FALSE)
	{
	  semantic_error(op_name,
					 CAN_CONTINUE,
					 get_error_msg(E_CALLED_OP_NAME_SHOULD_BE_AN_IDENT),
					 make_class_name(op_name)) ;

	  return ;
	}

  // Mise a jour de ref_op
  // Pour cela, on utilise le champ def de op_name qui pointe sur
  // le nom de la bonne operation
  T_ident *def_ident = ((T_ident *)op_name)->get_def() ;

  if(def_ident == NULL)
  {
      semantic_error(op_name, CAN_CONTINUE, get_error_msg(E_CALLED_ITEM_IS_NOT_AN_OPERATION),
                     ((T_ident*)op_name)->get_name()->get_value());
      return;
  }

  T_item *op_def = def_ident->get_father() ;
  ASSERT(op_def != NULL) ; // garanti par betree


  if (op_def->is_an_operation() == FALSE)
	{
	  const char *the_op_name = op_name->get_ref_lexem()->get_value() ;
	  semantic_error(op_name,
					 CAN_CONTINUE,
					 get_error_msg(E_CALLED_ITEM_IS_NOT_AN_OPERATION),
					 the_op_name) ;
	  semantic_error_details(op_def,
							 get_error_msg(E_LOCATION_OF_ITEM_DEF),
							 the_op_name) ;
	  return ;
	}

  ref_op = (T_generic_op *)op_def ;

  // ref_op peut avoir ete obtenu par clonage si c'est un T_used_op
  // il faut lancer le TC pour mettre le ref_op->ref_op a jour
  ref_op->type_check() ;

  // Parametres de entree
  // Il faut autant de parametres effectifs que de parametres
  // formels
  TRACE0("VERIFICATION DES PARAMETRES DE ENTREE") ;
  size_t nb_effective_in = 0 ;
  size_t nb_formal_in = 0 ;

  ENTER_TRACE ;
  T_item *cur_effective_in = first_in_param ;
  while (cur_effective_in != NULL)
	{
	  nb_effective_in ++ ;
	  cur_effective_in->make_type() ;
	  cur_effective_in = cur_effective_in->get_next() ;
	}

  T_ident *cur_formal_in = ref_op->get_in_params() ;
  while (cur_formal_in != NULL)
	{
	  nb_formal_in ++ ;
	  cur_formal_in = (T_ident *)cur_formal_in->get_next() ;
	}

  if (nb_effective_in != nb_formal_in)
	{
	  TRACE2("ici ref_op %x %s", ref_op, ref_op->get_class_name()) ;

	  if (ref_op->get_node_type() == NODE_USED_OP)
		{
		  T_generic_op *toto = ((T_used_op *)ref_op)->get_ref_op() ;

		  while (toto->get_node_type() == NODE_USED_OP)
			{
			  toto = ((T_used_op *)toto)->get_ref_op() ;
			}

		  TRACE2("ici toto %x %s", toto, toto->get_class_name()) ;

		  T_op *tutu = (T_op *)toto ;
		  ASSERT(tutu->get_node_type() == NODE_OPERATION) ;

		  TRACE2("name %s ref_machine %s",
				 tutu->get_name()->get_name()->get_value(),
				 tutu->get_name()->get_ref_machine()->get_machine_name()->get_name()->get_value()) ;

		}

	  semantic_error(this,
					 CAN_CONTINUE,
					 get_error_msg(E_WRONG_NUMBER_OF_EFFECTIVE_IN_PARAMS),
					 ref_op->get_op_name()->get_value(),
					 nb_effective_in,
					 nb_formal_in) ;
	  semantic_error_details(ref_op,
							 get_error_msg(E_LOCATION_OF_OP_SPEC)) ;
	  return ;
	}

  // Parametres de sortie
  // Il faut autant de parametres effectifs que de parametres
  // formels
  // A FAIRE : verifier deux a deux distincts
  TRACE0("VERIFICATION DES PARAMETRES DE SORTIE") ;
  size_t nb_effective_out = 0 ;
  size_t nb_formal_out = 0 ;

  ENTER_TRACE ;
  T_item *cur_effective_out = first_out_param ;
  while (cur_effective_out != NULL)
	{
	  nb_effective_out ++ ;
	  cur_effective_out->make_type() ;
	  cur_effective_out = cur_effective_out->get_next() ;
	}

  T_ident *cur_formal_out = ref_op->get_out_params() ;
  while (cur_formal_out != NULL)
	{
	  nb_formal_out ++ ;
	  cur_formal_out = (T_ident *)cur_formal_out->get_next() ;
	}

  if (nb_effective_out != nb_formal_out)
	{
	  semantic_error(this,
					 CAN_CONTINUE,
					 get_error_msg(E_WRONG_NUMBER_OF_EFFECTIVE_OUT_PARAMS),
					 ref_op->get_op_name()->get_value(),
					 nb_effective_out,
					 nb_formal_out) ;
	  semantic_error_details(ref_op,
							 get_error_msg(E_LOCATION_OF_OP_SPEC)) ;
	  return ;
	}

  // En sortie :
  // Ici on sait qu'il y a autant de parametres effectifs que de parametres formels
  cur_effective_out = first_out_param ;
  cur_formal_out = ref_op->get_out_params() ;

  while (cur_effective_out != NULL)
	{
	  ASSERT(cur_formal_out != NULL) ;

	  TRACE1("cur_formal_out = %x", cur_formal_out) ;
	  TRACE1("cur_formal_out->get_B_type = %x", cur_formal_out->get_B_type()) ;

	  T_type *ref_type = cur_formal_out->get_B_type() ;

	  if (ref_type == NULL)
		{
		  // Reprise sur erreur
		  TRACE0("panic ref_type = NULL") ;
		  return ;
		}
	  T_type *type = ref_type->clone_type(NULL,
										  cur_effective_out->get_betree(),
										  cur_effective_out->get_ref_lexem()) ;
	  ((T_ident*)cur_effective_out)->set_B_type(type,
												cur_effective_out->get_ref_lexem());
	  type->unlink() ;

	  cur_effective_out = (T_expr *)cur_effective_out->get_next() ;
	  cur_formal_out = (T_ident *)cur_formal_out->get_next() ;
	}

  // En entree :
  // Ici on sait qu'il y a autant de parametres effectifs que de parametres formels
  cur_effective_in = first_in_param ;
  cur_formal_in = ref_op->get_in_params() ;

  while (cur_effective_in != NULL)
	{
	  cur_effective_in->check_type() ;
	  ASSERT(cur_formal_in != NULL) ;

	  TRACE1("cur_formal_in = %x", cur_formal_in) ;
	  TRACE1("cur_formal_in->get_B_type = %x", cur_formal_in->get_B_type()) ;

	  T_type *ref_type = cur_formal_in->get_B_type() ;

	  if (ref_type == NULL)
		{
		  // Reprise sur erreur
		  TRACE0("panic ref_type = NULL") ;
		  return ;
		}
	  T_type *type = ref_type->clone_type(NULL,
										  cur_effective_in->get_betree(),
										  cur_effective_in->get_ref_lexem()) ;
	  ((T_ident *)cur_effective_in)->set_B_type(type,
												cur_effective_in->get_ref_lexem());
	  type->unlink() ;

	  cur_effective_in = (T_expr *)cur_effective_in->get_next() ;
	  cur_formal_in = (T_ident *)cur_formal_in->get_next() ;
	}


  // Controles supplementaires en cas d'appel d'une operation locale
  // SEMLOC 4a : on ne peut pas utiliser comme parametre de sortie une variable
  // concrete modifiee dans la spec de l'op loc
  semloc_4a() ;

  // SEMLOC 4b : on ne peut pas appeller une operation importee dans
  // une specification d'operation locale avec comme parametre de
  // sortie une variable importee modifiee dans la specification de
  // l'operation importee
  semloc_4b() ;

  EXIT_TRACE ;
  TRACE1("FIN T_op_call(%x)::type_check", this) ;

}

//
//	}{	Classe T_becomes_member_of
//

// Fonction generique de type check
// Substitution du type A :: B
// CTRL Ref : ITYPE 2-3
// Classe T_becomes_member_of : on type !
void T_becomes_member_of::type_check(void)
{
  TRACE1("T_becomes_member_of(%x)::type_check", this) ;

  // Verification de l'unicité des identificateurs en partie gauche
  check_no_double_ident(first_ident);

  set->make_type() ;
  //GP 07/01/99
  //CTRL Ref : ETYPE 1
  set->check_type() ;

  if (set->get_B_type() == NULL)
	{
	  // Erreur lors de la generation de type : on sort
	  TRACE0("panic : type = NULL") ;
	  return ;
	}

  T_ident *cur_ident = first_ident ;
  T_type *first_type ;

  size_t nb_idents = 0 ;
  while (cur_ident != NULL)
	{
	  cur_ident->make_type() ;
	  nb_idents ++ ;
	  cur_ident = (T_ident *)cur_ident->get_next() ;
	}

  // Il faut que la partie droite soit un ensemble !
  if (set->is_a_set() == FALSE)
	{
	  semantic_error(set,
					 CAN_CONTINUE,
					 get_error_msg(E_RHS_OF_BECOMES_MEMBER_OF_MUST_BE_A_SET),
					 make_type_name(set)->get_value()) ;
	  return ;
	}

  T_setof_type *setof = (T_setof_type *)(set->get_B_type()) ;
  T_type *base = setof->get_base_type() ;
  size_t nb_sets = 0 ;

  ASSERT(base != NULL) ;

  TRACE2("base %x:%s", base, base->make_type_name()->get_value()) ;
  if (/* (setof->is_a_seq() == TRUE) ||*/
	  (base->get_node_type() != NODE_PRODUCT_TYPE) )
	{
	  // Partie droite : un seul ensemble
	  TRACE1("Partie droite : un seul ensemble (setof->is_a_seq(): %s)",
			 (setof->is_a_seq() == TRUE)? "TRUE":"FALSE") ;
	  nb_sets = 1 ;
	  first_type = base ;

	  // Recuperation du typing_ident s'il y a lieu
	  first_type->set_typing_ident(set->get_typing_ident()) ;
	}
  else
	{
	  // Ici en partie droite on a soit un produit cartesien d'ensembles et
	  // dans ce cas on met a jour nb_sets directement (cas a,b :: E1 * E2)
	  T_type *cur = ((T_product_type *)base)->get_types() ;
	  first_type = cur ;
	  nb_sets = ((T_product_type *)base)->get_nb_types() ;

	  TRACE2("Partie droite :  plusieurs ensembles:%d (setof->is_a_seq(): %s)",
			 nb_sets, (setof->is_a_seq() == TRUE)? "TRUE":"FALSE") ;
	}

  if (nb_sets < nb_idents) // correction bug 2150 (was !=)
	{
	  TRACE2("!!! nb_sets %d, nb_idents %d", nb_sets, nb_idents) ;
	  semantic_error(first_ident,
					 CAN_CONTINUE,
					 get_error_msg(E_UNBALANCED_BECOMES_MEMBER_OF),
					 nb_idents,
					 nb_sets) ;
	  return ;
	}


  // Nouvel Algo d'inference de type (correction Bug 2150)
  // Ici on sait qu'il y a au moins autant de types que d'idents
  // On parcours la liste des types a l'envers a partir de last_type
  T_type *cur_type = base ;


  /*
    Nouveau nouvel algorithme, mais pas definitif:
    On veut typer les expressions du cote droit, donc la
    variable 'set', quand on connait deja le type du cote
    gauche. Par exemple, dans une initialisation
    v0, v1, v2 :: {{}}*{{}}*{{}}
    on veut determiner le type de toutes les sous-expressions
    à droite, à partir du type de v0, v1 et v2.
    L'arbre syntaxique est ((v0,v1),v2), et on le parcours
    de la droite vers la gauche.
    On obtient successivement les types t2, t1 et t0.
    On veut construire le type (t0 * t1) * t2.
    Il faut donc empiler ces types dans une structure de donnees
    rhs_types.
  */

  int changed = FALSE;
  std::deque<T_type*> rhs_types;

  cur_ident = last_ident ;

  while (cur_ident != NULL)
	{
	  ASSERT(cur_type != NULL) ; // verifie ci-dessus

	  // Prochain ident
	  T_ident *next_ident = (T_ident *)cur_ident->get_prev() ;

	  // Prochain type
	  T_type *next_type = NULL ;

	  // Type de l'ident courant
	  T_type *ident_type ;

	  if (next_ident == NULL)
		{
		  // On prend tout ce qui reste pour typer !
		  ident_type = cur_type ;
		  next_type = NULL ;
		}
	  else
		{
		  // Cas general
		  if (cur_type->get_node_type() == NODE_PRODUCT_TYPE)
			{
			  T_product_type *ptype = (T_product_type *)cur_type ;
			  ident_type = ptype->get_type2() ;
			  next_type = ptype->get_type1() ;
			}
		  else
			{
			  ident_type = cur_type ;
			  next_type = NULL ;
			}
		}
      /*
        Dans la cas ou la valeur affectee n'est pas completement typee,
        on doit pouvoir inferer le type a partir de la cible de
        l'affectation.
      */
      if(ident_type == NULL || ident_type->has_wildcards(TRUE)) {
          rhs_types.push_front(cur_ident->get_B_type());
          changed = TRUE;
      } else {
          // Verification que cur_ident est compatible avec ident_type
          T_type *new_type = ident_type->clone_type
              (cur_ident, cur_ident->get_betree(), cur_ident->get_ref_lexem());
          cur_ident->set_B_type(new_type, cur_ident->get_ref_lexem()) ;
          rhs_types.push_front(ident_type);

          TRACE2("TYPING_IDENT recup typing_ident de %x ie %x",
                 new_type,
                 new_type->get_typing_ident())  ;
          cur_ident->set_typing_ident(new_type->get_typing_ident()) ;
      }
	  //GP 25/01/99
	  //Interdir l'usage des strings
	  cur_ident->check_string_uses(FALSE) ;

	  cur_type = next_type ;
	  cur_ident = next_ident ;
	}
  if(changed == TRUE && 1 <= rhs_types.size()) {
      T_type* rhs_type = rhs_types.at(0);
      for(int i = 1; i < rhs_types.size(); ++i) {
          T_type* tmp = rhs_type;
          rhs_type = new T_product_type(rhs_type, rhs_types.at(i),
                                        set,
                                        set->get_betree(),
                                        set->get_ref_lexem());
          tmp->unlink();
      }
      T_type* prhs_type =
          new T_setof_type(rhs_type, set, set->get_betree(),
                           set->get_ref_lexem());
      set->set_B_type(prhs_type, set->get_ref_lexem());
      rhs_type->unlink();
      // test if assignment has failed to print diagnostic assistance
      if (set->get_B_type() != prhs_type) {
          T_ident *cur_ident = first_ident ;
          while (cur_ident != NULL) {
              semantic_error_details(cur_ident->get_typing_location(),
                                     get_error_msg(E_LOCATION_OF_IDENT_TYPE),
                                     cur_ident->get_B_type()->make_type_name()->get_value(),
                                     cur_ident->get_ident_type_name(),
                                     cur_ident->get_name()->get_value()) ;
              cur_ident = (T_ident *)cur_ident->get_next() ;
          }
      }
  }

}

//
//	}{ Classe T_precond
//
void T_precond::type_check(void)
{
  TRACE1("T_precond(%x)::type_check()", this) ;
  ENTER_TRACE ;
  predicate->type_check() ;
  type_check_substitution_list(first_body) ;
  EXIT_TRACE ;
}

//
//	}{ Classe T_choice
//
void T_choice::type_check(void)
{
  TRACE1("T_choice(%x)::type_check()", this) ;
  ENTER_TRACE ;
  type_check_substitution_list(first_body) ;
  T_item *cur = first_or ;
  while (cur != NULL)
	{
	  cur->type_check() ;
	  cur = cur->get_next() ;
	}
  EXIT_TRACE ;
}

//
//	}{ Classe T_or
//
void T_or::type_check(void)
{
  TRACE1("T_or(%x)::type_check()", this) ;
  ENTER_TRACE ;
  type_check_substitution_list(first_body) ;
  EXIT_TRACE ;
}

//
//	}{ Classe T_if
//
void T_if::type_check(void)
{
  TRACE1("T_if(%x)::type_check()", this) ;
  ENTER_TRACE ;
  cond->type_check() ;
  type_check_substitution_list(first_then_body) ;
  T_item *cur = first_else ;
  while (cur != NULL)
	{
	  cur->type_check() ;
	  cur = cur->get_next() ;
	}
  EXIT_TRACE ;
}

//
//	}{ Classe T_else
//
void T_else::type_check(void)
{
  TRACE1("T_else(%x)::type_check()", this) ;
  ENTER_TRACE ;
  if (cond != NULL)
	{
	  cond->type_check() ;
	}
  type_check_substitution_list(first_body) ;
  EXIT_TRACE ;
}

//
//	}{ Classe T_when
//
void T_when::type_check(void)
{
  TRACE1("T_when(%x)::type_check()", this) ;
  ENTER_TRACE ;
  if (cond != NULL)
	{
	  cond->type_check() ;
	}
  type_check_substitution_list(first_body) ;
  EXIT_TRACE ;
}

//
//	}{ Classe T_assert
//
void T_assert::type_check(void)
{
  TRACE1("T_assert(%x)::type_check()", this) ;
  ENTER_TRACE ;
  predicate->type_check() ;
  type_check_substitution_list(first_body) ;
  EXIT_TRACE ;
}

//
//	}{ Classe T_label
//
void T_label::type_check(void)
{
  TRACE1("T_label(%x)::type_check()", this) ;
  ENTER_TRACE ;
  expression->type_check() ;
  type_check_substitution_list(first_body) ;
  EXIT_TRACE ;
}

//
//	}{ Classe T_jumpif
//
void T_jumpif::type_check(void)
{
  TRACE1("T_jumpif(%x)::type_check()", this) ;
  ENTER_TRACE ;
  cond->type_check() ;
  EXIT_TRACE ;
}
//
//	}{ Classe T_witness
//
void T_witness::type_check(void)
{
  TRACE1("T_witness(%x)::type_check()", this) ;
  ENTER_TRACE ;
  predicate->type_check() ;
  type_check_substitution_list(first_body) ;
  EXIT_TRACE ;
}

//
//	}{ Classe T_select
//
void T_select::type_check(void)
{
  TRACE1("T_select(%x)::type_check()", this) ;
  ENTER_TRACE ;
  cond->type_check() ;
  type_check_substitution_list(first_then_body) ;
  T_item *cur = first_when ;
  while (cur != NULL)
	{
	  cur->type_check() ;
	  cur = cur->get_next() ;
	}
  EXIT_TRACE ;
}

//
//	}{ Classe T_case
//
void T_case::type_check(void)
{
  TRACE1("T_case(%x)::type_check()", this) ;

  ENTER_TRACE ;
  //GP avec TESTS_SYNTAXE/REMPLACEMENT_US/MchTestExMU1E151.mch
  // segmentation fault car case_select = NULL
  TRACE1("case_select %x", case_select) ;
  case_select->type_check() ;
  T_item *cur = first_case_item ;
  while (cur != NULL)
	{
	  cur->type_check() ;
	  cur = cur->get_next() ;
	}

  check_no_double_literal_value(first_case_item) ;
  EXIT_TRACE ;
}

//
//	}{ Classe T_case_item
//
void T_case_item::type_check(void)
{
  TRACE1("T_case_item(%x)::type_check()", this) ;


  ENTER_TRACE ;
  //GP 21/12/98: il faut le type du case select
  // CTRL Ref : VTYPE 3-4
  T_type *type_case_select = ((T_case*)get_father())->get_case_select()->get_B_type() ;
  if(type_case_select == NULL)
	{
	  // Reprise sur erreur
	  return;
	}

  T_item *cur = first_literal_value ;
  while (cur != NULL)
	{
	  cur->type_check() ;
	  //GP 21/12/98
	  //Types autorises: entiers litteraux, enumeres litteraux, boolens
	  if(get_disable_case_selector_check() == TRUE ||
		 cur->is_expected_in_case_branch() == TRUE)
		{
	 		T_type * type_cur = cur->get_B_type() ;
			if (type_cur != NULL)
			  {
				if(type_cur->is_compatible_with(type_case_select)==FALSE)
				  {
					semantic_error(cur,
								   CAN_CONTINUE,
								   get_error_msg(E_BRANCH_TYPE_DIFFER_FROM_SELECTOR_TYPE),
								   type_cur->make_type_name()->get_value(),
								   type_case_select->make_type_name()->get_value()) ;
				  }
			  }
		}
	  else
		{
		  semantic_error(cur,
						 CAN_CONTINUE,
						 get_error_msg(E_UNEXPECTED_TYPE_CASE_BRANCH),
						 (cur->is_an_ident() == TRUE)
						 ? ((T_ident *)cur)->get_name()->get_value()
						 : make_class_name(cur));
		}

	  cur = cur->get_next() ;
	}
  type_check_substitution_list(first_body) ;
  EXIT_TRACE ;
}

//
//	}{ Classe T_any
//
void T_any::type_check(void)
{
  TRACE1("T_any(%x)::type_check()", this) ;

  // On commence par tout typer ...
  ENTER_TRACE ;
  TRACE0("Typage des idents") ;
  check_no_double_definition(first_ident) ;
  T_ident *cur = first_ident ;
  while (cur != NULL)
	{
	  cur->type_check() ;

	  cur = (T_ident *)cur->get_next() ;
	}
  where->type_check() ;

  // Debut correction bug 2209 : verifier que les idents sont types dans le where
  check_type_ident_list(first_ident,
						get_lex_type_ascii(L_WHERE),
						get_ref_lexem()->get_lex_ascii()) ;
  // Fin correction bug 2209

  type_check_substitution_list(first_body) ;
  EXIT_TRACE ;

  // Puis on verifie que les identificateurs ne sont pas ecrits dans
  // la substitution
  TRACE0("Verification de non-ecriture des idents") ;
  T_ident *cur_ident = first_ident ;

  while (cur_ident != NULL)
	{
	  TRACE2("ici cur_ident = %x:%s", cur_ident, cur_ident->get_class_name()) ;
	  T_substitution *cur_subst = first_body ;
	  while (cur_subst != NULL)
		{
		  TRACE2("ici cur_subst = %x:%s", cur_subst, cur_subst->get_class_name()) ;
		  if (cur_subst->modifies(cur_ident) == TRUE)
			{
			  semantic_error(cur_subst,
							 CAN_CONTINUE,
							 get_error_msg(E_ANY_DATA_CAN_NOT_BE_MODIFIED),
							 cur_ident->get_name()->get_value()) ;
			}
		  cur_subst = (T_substitution *)cur_subst->get_next() ;
		}
	  cur_ident = (T_ident *)cur_ident->get_next() ;
	}
}


// Classe T_var : on type !
void T_var::type_check(void)
{
  TRACE1("T_var(%x)::type_check", this) ;

  // les idents doivent etre deux a deux distincts
  check_no_double_definition(first_ident) ;

  // On typecheck les variables locales en BOM
  if (get_Use_B0_Declaration() == TRUE)
	{
	  T_ident *cur_ident = first_ident;
	  while (cur_ident != NULL)
		{
		  cur_ident->type_check();
		  cur_ident = (T_ident *)cur_ident->get_next();
		}
	}
  // Le corps
  TRACE0("type check du corps") ;
  type_check_substitution_list(first_body) ;

  // Debut correction bug 2209 : verifier que les idents sont types dans le VAR
  // + EVOL 2794
  if (get_allow_unused_local_variables() == FALSE)
	{
	  check_type_ident_list(first_ident,
							get_lex_type_ascii(L_IN),
							get_lex_type_ascii(L_VAR)) ;
	}
  // Fin correction bug 2209 + EVOL 2794

#ifdef B0C
  // Verification B0 du typage des variables locales
  if (get_B0_disable_locale_variables_type_check() != TRUE)
	{
	  //	  local_variable_type_check_list(first_ident);
	}

  // Verification de la non existence de cycles dans les affectations
  // concernant des variables locales, en implementation
  // On va verifier RINIT 1-1
  // On positionne la variable globale check a 1

  if (get_B0_disable_variables_initialisation_check() != TRUE
	  &&
	  get_Use_B0_Declaration() == FALSE)
	{
	  if (check_is_in_an_implementation() == TRUE)
		{
		  set_check(B0CHK_INIT);

		  // On construit la liste des identificateurs a initialiser
		  // Cette liste est stockee dans la variable globale
		  // current_list_ident
		  T_list_ident *list = new T_list_ident();

		  T_ident *cur_var = first_ident;
		  while (cur_var != NULL)
			{
			  list->add_identifier(cur_var);
			  cur_var = (T_ident *)cur_var->get_next() ;
			}


		  // On initialise la variable globale des identificateurs
		  // a initialiser
		  set_current_list_ident(list);

		  // On cree la liste des identificateurs initialises
		  // (vide pour l'instant)
		  T_list_ident *list_initialised_ident = new T_list_ident();

		  // Controle des substitutions du corps de T_var
		  initialisation_check_list(first_body,
									&list_initialised_ident);


		  // On repositionne le flag global a 0 (aucune verification)
		  set_check(B0CHK_DEFAULT);
		}
	}

#endif // B0C
}

// Classe T_begin : on type !
void T_begin::type_check(void)
{
  TRACE1("T_begin(%x)::type_check", this) ;

  // Le corps
  TRACE0("type check du corps") ;
  type_check_substitution_list(first_body) ;
}

//
//	}{ Classe T_let
//
void T_let::type_check(void)
{
  TRACE1("T_let(%x)::type_check()", this) ;

  // On verifie que les identificateurs sont deux a deux distincts
  check_no_double_definition(first_ident) ;

  //GP 07/11/99
  //CTRL Ref : VTYPE 3-6
  //Les variables introduites par le LET ne doivent pas etre modifiees
  //dans la Substitution du LET.
  T_ident * cur_ident = first_ident ;
  while (cur_ident != NULL)
	{
	  T_substitution* cur_subst = first_body ;
	  while(cur_subst != NULL)
		{
		  if(cur_subst->modifies(cur_ident) == TRUE)
			{
			  semantic_error(cur_ident,
							 CAN_CONTINUE,
							 get_error_msg(E_LHS_OF_LET_SUBSTITUTION_CAN_NOT_BE_A_LOC_VAR),
							 cur_ident->get_name()->get_value()) ;
			}
		  cur_subst=(T_substitution*)cur_subst->get_next() ;
		}
	  cur_ident = (T_ident*)cur_ident->get_next() ;
	}

  ENTER_TRACE ;

  T_item* cur = first_ident ;
  while (cur != NULL)
	{
	  cur->type_check() ;
	  cur = cur->get_next() ;
	}

  T_valuation *cur_val = first_valuation ;
  while (cur_val != NULL)
	{
	  // Verifier que les parties gauches doivent provenir du let uniquement
	  // Pour cela il faut que def->father() i.e. le let soit le LET en
	  // cours d'analyse
	  T_ident *cur_ident = cur_val->get_ident() ;
	  T_ident *cur_def =
		(cur_ident->get_def() == NULL) ? cur_ident : cur_ident->get_def() ;

	  TRACE3("DEBUT DE L'ETUDE DE %s, type %x:%s",
			 cur_ident->get_name()->get_value(),
			 cur_ident->get_B_type(),
			 (cur_ident->get_B_type() == NULL)
			 ? "" : cur_ident->get_B_type()->make_type_name()->get_value()) ;
	  if (cur_def->get_father() != this)
		{
		  semantic_error(cur_ident,
						 CAN_CONTINUE,
						 get_error_msg(E_LHS_OF_LET_VALUATION_MUST_BE_A_LOC_VAR),
						 cur_ident->get_name()->get_value()) ;

		}
	  else if (cur_ident->get_B_type() != NULL)
		{
		  // les variables ne doivent etre typees qu'une seule fois
		  semantic_error(cur_ident,
						 CAN_CONTINUE,
						 get_error_msg(E_MULTIPLE_TYPE_IN_LET_VALUATION),
						 cur_ident->get_name()->get_value()) ;
		  semantic_error_details(cur_ident->get_typing_location(),
								 get_error_msg(E_LOCATION_OF_IDENT_TYPE),
								 cur_ident->get_B_type()->make_type_name()->get_value(),
								 cur_ident->get_ident_type_name(),
								 cur_ident->get_name()->get_value()) ;
		}
	  else
		{
		  // C'est bon , on peut faire le type check !
		  cur_val->type_check() ;
		}

	  TRACE3("FIN DE L'ETUDE DE %s, type %x:%s",
			 cur_ident->get_name()->get_value(),
			 cur_ident->get_B_type(),
			 (cur_ident->get_B_type() == NULL)
			 ? "" : cur_ident->get_B_type()->make_type_name()->get_value()) ;
	  cur_val = (T_valuation *)cur_val->get_next() ;
	}

  //V�rification du typage de chaque variable locale.
  check_type_ident_list(first_ident,
						get_lex_type_ascii(L_BE),
						get_lex_type_ascii(L_LET)) ;

  type_check_substitution_list(first_body) ;


  EXIT_TRACE ;
}

//
//	}{ Classe T_while
//
void T_while::type_check(void)
{
  TRACE1("T_while(%x)::type_check()", this) ;

  ENTER_TRACE ;
  cond->type_check() ;

  type_check_substitution_list(first_body) ;
  invariant->type_check() ;
  variant->type_check() ;

  if (    (variant->get_B_type() == NULL)
	   || (variant->get_B_type()->is_an_integer() == FALSE) )
	{
	  semantic_error(variant,
					 CAN_CONTINUE,
					 get_error_msg(E_VARIANT_IS_NOT_AN_INTEGER),
					 make_type_name(variant)->get_value()) ;
	}

  EXIT_TRACE ;
}


//
//	}{	Classe T_becomes_such_that
//

// Classe T_becomes_such_that : on type !
void T_becomes_such_that::type_check(void)
{
  TRACE1("T_becomes_such_that(%x)::type_check", this) ;

  // Verification de l'unicite des identificateur en partie gauche
  check_no_double_ident(first_ident);

  // Verification du predicat en partie droite
  pred->type_check() ;

  // Il faut verifier que les identificateurs sont types
  T_ident *cur_ident = first_ident ;

  while (cur_ident != NULL)
	{
	  if (cur_ident->get_B_type() == NULL)
		{
		  semantic_error(pred,
						 CAN_CONTINUE,
						 get_error_msg(E_IDENT_NOT_TYPED_IN_BECOMES_SUCH_THAT_PRED),
						 cur_ident->get_name()->get_value()) ;
		  semantic_error_details(cur_ident,
								 get_error_msg(E_LOCATION_OF_ITEM_DEF),
								 cur_ident->get_name()->get_value()) ;
		}
	  else
		{
		  //GP 25/01/99
		  //Interdir l'usage des strings
		  cur_ident->check_string_uses(FALSE) ;
		}

	  cur_ident = (T_ident *)cur_ident->get_next() ;
	}

}

// Type check d'une liste de substitutions
//GP 21/01/99
//Desormais le type_check_substitution_list est faite en deux
//passes
//(1) type_check de la liste
//(2) parallel_checks
// Pour eviter de faire un next_parallel->parallels_check()
//avant next_parallel->type_check()
void type_check_substitution_list(T_substitution *list)
{
  TRACE1("type_check_substitution_list(%x)", list) ;

  T_substitution *cur = list ;

  //(1) type_check de la liste
  while (cur != NULL)
	{
	  if (cur->get_ref_lexem() == NULL)
		{
		  TRACE2("TC LIST cur %x %s",
			 cur,
			 cur->get_class_name()) ;
		}
	  else
		{
		  TRACE5("TC LIST cur %x %s",
				 cur,
				 cur->get_class_name(),
				 cur->get_ref_lexem()->get_file_name()->get_value(),
				 cur->get_ref_lexem()->get_file_line(),
				 cur->get_ref_lexem()->get_file_column()) ;
		}
	  cur->type_check() ;
	  cur = (T_substitution *)cur->get_next() ;
	}


#ifndef EVOL_INCLUDES
  //(2) parallel_checks de la liste
  // On verifie les substitutions de la liste en les parcourant
  // Pour chaque element, on fait la verification avec les elements précédents
  // Dans le cas S1 || S2 ; S3 || S4
  // Parenthese ((S1 || S2) ; S3) || S4
  // Pour S1 aucune verification
  // Pour S2 verification avec S1
  // Pour S3 aucune verification
  // Pour S4 verification avec S1, S2, S3
  // Pour chaque element pris de gauche à droite, on parcourt les élements vers la gauche si le lien avec
  // le premier prédécesseur est un || dans le reste du parcours on ne tient pas compte du lien.
  cur = list ;
  while (cur != NULL)
	{
	  if (cur->get_ref_lexem() == NULL)
		{
		  TRACE2("PC LIST cur %x %s",
				 cur,
				 cur->get_class_name()) ;
		}
	  else
		{
		  TRACE5("PC LIST cur %x %s",
				 cur,
				 cur->get_class_name(),
				 cur->get_ref_lexem()->get_file_name()->get_value(),
				 cur->get_ref_lexem()->get_file_line(),
				 cur->get_ref_lexem()->get_file_column()) ;
		}
      T_substitution *prev = (T_substitution *)cur->get_prev() ;

      T_substitution *prev_parallel = prev ;
      T_substitution_link_type cur_link_type ;
      if (prev_parallel != NULL) {
        cur_link_type = prev_parallel->get_link_type() ;
      }

      while ( (prev_parallel != NULL) && (cur_link_type == LINK_PARALLEL) )
		{
		  TRACE4("verification de non modification en parallele de %x:%s et %x:%s",
				 cur,
				 cur->get_class_name(),
                 prev_parallel,
                 prev_parallel->get_class_name()) ;
          cur->parallel_checks(prev_parallel) ;

		  // La suite !
          //cur_link_type = prev_parallel->get_link_type() ;
          prev_parallel = (T_substitution *)prev_parallel->get_prev() ;
		}
      cur = (T_substitution *)cur->get_next() ;
	}
#endif


}

//GP 28/12/98
int T_literal_boolean::is_same_value(T_item* case_branch)
{
  if ((case_branch->get_node_type() == NODE_LITERAL_BOOLEAN) &&
	  (value == ((T_literal_boolean*)case_branch)->get_value()))
	{
	  return TRUE ;
	}
  else
	{
	  return FALSE ;
	}
}

int T_literal_integer::is_same_value(T_item* case_branch)
{
  if ((case_branch->get_node_type() != NODE_LITERAL_INTEGER))
	{
	  //Reprise sur erreur
	  return FALSE ;
	}
  else
	{
	  T_integer * case_branch_value =
		((T_literal_integer*)case_branch)->get_value() ;

	  return case_branch_value->is_equal_to(value) ;
	}
}

int T_literal_enumerated_value::is_same_value(T_item* case_branch)
{
  if (case_branch->get_node_type() != NODE_LITERAL_ENUMERATED_VALUE)
	{
	  //Reprise sur erreur
	  return FALSE ;
	}
  else
	{
	  T_symbol* case_branch_name=
		((T_literal_enumerated_value*)case_branch)->get_name() ;

	  if (get_name()->compare(case_branch_name))
		{
		  return TRUE ;
		}
	  else
		{
		  return FALSE ;
		}
	}
}


//GP 22/12/98
//Donne le literal suivant, dans un case.
//si le suivant est null, alors passe au case_item suivant.
static T_item* get_next_literal(T_item* cur,
										  T_case_item** adr_cur_ci)
{
  TRACE0("get_next_literal()") ;
  cur = (T_item*)cur->get_next() ;
  if (cur == NULL)
	{
	  *adr_cur_ci =(T_case_item*)(*adr_cur_ci)->get_next() ;
	  if (*adr_cur_ci ==NULL)
		{
		  return NULL ;
		}
	  else
		{
		  return (*adr_cur_ci)->get_literal_values() ;
		}
	}
  else
	{
	  return cur ;
	}
}


//GP 22/12/98
// Verifie que la liste des case_item ne contient pas de doublons
static void check_no_double_literal_value(T_case_item*list_case_item)
{
  // Verification bete et incrementale pour l'instant
  // Devrait suffir en terme de performances car les listes
  // sont courtes et les comparaisons de symboles optimisees

  TRACE0("check_no_double_literal_value()") ;
	ENTER_TRACE ;
  T_case_item *cur_ci = list_case_item ;
  T_item* cur = (list_case_item==NULL)
	? (T_item*)NULL
	: list_case_item->get_literal_values() ;

  while(cur != NULL)
	{
	  T_case_item *check_ci = cur_ci ;
	  T_item* check = get_next_literal(cur, &check_ci) ;

	  while (check != NULL)
		{
		  if (cur->is_same_value(check) == TRUE)
			{
			  semantic_error(check,
							 CAN_CONTINUE,
							 get_error_msg(E_BRANCH_VALUE_ALREADY_EXIST)) ;

			  semantic_error_details(cur,
									 get_error_msg(E_IDENT_CLASH_OTHER_LOCATION)) ;

			}

		  check = get_next_literal(check, &check_ci) ;
		}
	  cur=get_next_literal(cur, &cur_ci) ;
	}

  EXIT_TRACE ;

}


//
//	}{	Fin du type check des substitutions
//

// Configuration pour emacs
// Local Variables:
// tab-with: 4
// End:
