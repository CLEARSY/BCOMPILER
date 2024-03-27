/******************************* CLEARSY **************************************
* Fichier : $Id: t_pred.cpp,v 2.0 2002-07-12 12:57:41 cm Exp $
* (C) 2008 CLEARSY
*
* Description :		Compilateur B
*					Type check pour les predicats
*
* Compilations :	-DDEBUG_PRED_TYPE pour tracer les types
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
RCS_ID("$Id: t_pred.cpp,v 1.52 2002-07-12 12:57:41 cm Exp $") ;

/* Includes systeme */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

/* Includes Composant Local */
#include "c_api.h"
#include "v_ldreq.h"
#include "v_toplvl.h"
#include "c_tstamp.h"
#include "t_misc.h"


#define QUANTIFIED_TYPING_PREDICATE 2

/* reference avant */
void type_check_loop(T_type *real_type,
							  T_typing_predicate_case tcase,
							  T_expr *first_ident,
							  T_expr *last_ident,
							  T_item *father,
							  T_item *type,
                              T_typing_predicate_type typing_predicate_type,
                              T_item * tpred);


//
//	}{	Classe T_typing_predicate
//

// Fonction generique de type check

// Pour les predicats typants, on doit faire de l'inference de type.
// Ce n'est necessaire que si l'expression qui doit etre typee
// contient des variables de type.
// Si ce n'est pas le cas, il suffit de faire de la vérification de
// type.
void T_typing_predicate::type_check(void)
{
#ifdef DEBUG_PRED_TYPE
  TRACE4("DEBUT T_typing_predicate(%x %s:%d:%d)::type_check",
		 this,
		 get_ref_lexem()->get_file_name()->get_value(),
		 get_ref_lexem()->get_file_line(),
		 get_ref_lexem()->get_file_column()) ;
  ENTER_TRACE ;
#endif

  T_expr *cur_ident = first_ident ;

  // Calcul des types en partie gauche
  while (cur_ident != NULL)
    {
      T_expr::set_warn_ident(FALSE);
      cur_ident->make_type() ;
      T_expr::set_warn_ident(TRUE);
      cur_ident = (T_expr *)cur_ident->get_next() ;
    }

  // A FAIRE : justifier le cast
  T_item *real_type = (T_item *)(type->follow_indirection()) ;

  T_type *ttype;
  if (real_type->is_an_expr() == FALSE)
  {
      if (real_type->is_a_type() == FALSE) {
          TRACE2("!!! PB, le type %x est un %s et pas une EXPR",
                 real_type,
                 real_type->get_class_name()) ;
          semantic_error(real_type,
                         CAN_CONTINUE,
                         get_error_msg(E_NOT_AN_EXPRESSION),
                         make_type_name(real_type)->get_value()) ;
          // On ne peut plus rien faire !
          return ;
      } else {
          ttype = (T_type *)real_type;
      }
  }
  else
  {
      // on commence par calculer le type
      real_type->make_type() ;


      //GP 4/01/99: pour tester
      TRACE2("T_typing_predicate::type_check : Le b type est: %s(%x) \n",
             (real_type->get_B_type() == NULL) ? "PAS ENCORE TYPE"
                                               : real_type->get_B_type()->make_type_name()->get_value() ,
             real_type->get_B_type()) ;
      //GP 6/01/99
      //CTRL Ref : ETYPE 1
      real_type->check_type() ;
      ttype = real_type->get_B_type() ;

  }

  T_typing_predicate_case tcase = ITYPE_1_1 ; // init pour eviter un warning
  switch (typing_predicate_type)
	{
	case TPRED_BELONGS :
	  {
		tcase = ITYPE_1_1 ;
		break ;
	  }
	case TPRED_INCLUDED :
	case TPRED_STRICT_INCLUDED :
	  {
		tcase = ITYPE_1_2 ;
		break ;
	  }
	case TPRED_EQUAL :
	  {
		tcase = ITYPE_1_3 ;
		break ;
	  }
	// Pas de default pour que le compilateur nous previenne en
	// cas d'oubli
	}

  // On lance l'inference de type
  type_check_loop(ttype,
				  tcase,
				  first_ident,
				  last_ident,
				  this,
				  type,
                  typing_predicate_type,
                  this) ;

#ifdef DEBUG_PRED_TYPE
  ENTER_TRACE ;
  TRACE1("FIN   T_typing_predicate(%x)::type_check", this) ;
#endif

}

// Fonction generique de type check
void T_ident::type_check()
{
  TRACE1("T_ident::type_check(this=%p)",this);
  T_expr::type_check();
  if (get_Use_B0_Declaration() == TRUE
      &&
      BOM_lexem_type != NULL)
    {
      //calcul du type de prédicat
      T_typing_predicate_type typing_predicate_type;
      T_typing_predicate_case tcase;
      if (BOM_lexem_type->get_lex_type() == L_BELONGS)
		{
		  typing_predicate_type = TPRED_BELONGS;
		  tcase = ITYPE_1_1 ;
		}
      else
		{
		  typing_predicate_type = TPRED_EQUAL;
		  tcase = ITYPE_1_3 ;
		}

      // A FAIRE : justifier le cast
      T_item *real_type = (T_item *)(BOM_type->follow_indirection()) ;
      if (real_type->is_an_expr() == FALSE)
		{
		  TRACE2("!!! PB, le type %x est un %s et pas une EXPR",
				 real_type,
				 real_type->get_class_name()) ;
		  semantic_error(real_type,
						 CAN_CONTINUE,
						 get_error_msg(E_NOT_AN_EXPRESSION),
						 make_type_name(real_type)->get_value()) ;
		  // On ne peut plus rien faire !
		  return ;
		}

	  // on commence par calculer le type
	  real_type->make_type() ;
	  TRACE2("T_ident::type_check : Le b type est: %s(%x) \n",
			 (real_type->get_B_type() == NULL) ? "PAS ENCORE TYPE"
			 : real_type->get_B_type()->make_type_name()->get_value() ,
			 real_type->get_B_type()) ;

	  // On lance l'inference de type
	  T_type *ttype = real_type->get_B_type() ;

      type_check_loop(ttype,
					  tcase,
					  this,
					  this,
					  this,
					  BOM_type,
                      typing_predicate_type,
                      this);
	  TRACE2("Apres type_check_loop:pour ident %p B_type:%p",this,get_B_type());

	  // Vérification que la valeur d'initialisation éventuelle est
	  // compatible avec le type de l'ident

	  if (BOM_init_value != NULL)
		{
		  BOM_init_value->make_type();

		  set_B_type(BOM_init_value->get_B_type(), BOM_init_value->get_ref_lexem());
		}
    }
}


// Typage par ':'
// CTRL Ref : ITYPE 1-1
void belongs_check_and_set_type(T_expr *expr,
										 T_setof_type *type,
										 T_lexem *type_decla,
										 T_item *father_pred_or_ident)
{
#ifdef DEBUG_PRED_TYPE
  TRACE3("belongs_check_and_set_type(%x, %x, %x)", expr, type, type_decla) ;
#endif

  T_type *base_type = (type->get_spec_base_type() == NULL)
	? type->get_base_type()
	: type->get_spec_base_type() ;


  T_ident *ident = (expr->is_an_ident() == TRUE)
	? expr->make_ident() : (T_ident *)NULL ;

#ifdef DEBUG_PRED_TYPE
  TRACE2("expr(%x) -> base type %s",
		 expr,
		 (base_type == NULL) ? "" : base_type->make_type_name()->get_value()) ;
#endif


  // Cas 1 : B est un ensemble de fonctions totales, injections totales,
  // surjections totales ou bijections : Gamma(B) = Psi(T)
  // Alors Gamma(A) = T

#ifndef NO_CHECKS
  if (type->get_father() == NULL)
	{
	  TRACE1("PANIC : type %x -> get_father NULL",
			 type) ;
	  semantic_error(type,
					 CAN_CONTINUE,
					 "PANIC type %x -> get_father NULL",
					 type) ;
	  return ;
	}
#endif

  T_item *father = type->get_father() ;
#ifdef DEBUG_PRED_TYPE
  TRACE2("type->father %p:%s",
		 father,
		 father->get_class_name()) ;
#endif

  // EVOL2841: on saute les éventuelles parenthèses
  if (father->get_node_type() == NODE_EXPR_WITH_PARENTHESIS)
	{
	  father = ((T_expr *) father)->strip_parenthesis();
	}
#ifdef DEBUG_PRED_TYPE
  TRACE2("type->father (après saut des parenthèses) %p:%s",
		 father,
		 father->get_class_name()) ;
#endif

  if (father->get_node_type() == NODE_RELATION)
	{
	  T_relation *rel = (T_relation *)father ;
	  T_relation_type rtype = rel->get_relation_type() ;
	  TRACE1("ici rtype = %d", rtype) ;
	  if (	(rtype == RTYPE_TOTAL_FUNCTION)
			|| (rtype == RTYPE_TOTAL_INJECTION)
			|| (rtype == RTYPE_TOTAL_SURJECTION)
			|| (rtype == RTYPE_BIJECTION) )
		{
#ifdef DEBUG_PRED_TYPE
		  TRACE0("cas 1") ;
#endif

		  // A FAIRE : verifier le cast
#ifdef DEBUG_PRED_TYPE
		  const char *name = base_type->make_type_name()->get_value() ;
		  TRACE1("DEBUT CLONAGE de %s", name) ;
		  ENTER_TRACE ;
#endif

		  T_type *res = base_type->clone_type(father_pred_or_ident, father_pred_or_ident->get_betree(), father_pred_or_ident->get_ref_lexem()) ;
#ifdef DEBUG_PRED_TYPE
		  EXIT_TRACE ;
		  TRACE2("FIN CLONAGE type %x %x", type, type->get_typing_ident()) ;
#endif

#ifdef DEBUG_PRED_TYPE
		  name = res->make_type_name()->get_value() ;
		  TRACE1("RESULTAT DU CLONAGE = %s", name) ;
#endif

		  expr->set_B_type(res, type_decla) ;
		  res->unlink() ;

#ifdef DEBUG_PRED_TYPE
		  TRACE1("ici apres lecture type %s",
				 expr->get_B_type()->make_type_name()->get_value()) ;
#endif

		  return ;
		}
	}

  // Cas 2 : B est un ensemble d'ensembles index large, i.e.
  // Gamma(B) = Psi(Psi(K(T, U, V)))
  // et A est une constante
  if (	 (expr->is_a_constant() == TRUE)
	  && (base_type->is_an_extended_index_set() == TRUE) )
	{
	  // Il faut construire Setof(K(T, A-, A+))
#ifdef DEBUG_PRED_TYPE
	  TRACE0("Cas 2 (ensemble index large)") ;
#endif

	  T_type *inside = ((T_setof_type *)base_type)->get_base_type() ;
	  // Cast justifie ci-apres
	  T_base_type *new_inside = (T_base_type *)inside->clone_type(NULL,
																  father_pred_or_ident->get_betree(),
																  father_pred_or_ident->get_ref_lexem()) ;
	  ASSERT(new_inside->is_a_base_type() == TRUE) ; // car provient d'ens idx large

	  if ( (ident != NULL) && (new_inside->are_bounds_set() == FALSE) )
		{
		  // On etablit les bornes
#ifdef DEBUG_PRED_TYPE
		  TRACE0("On met les bornes A+, A-") ;
#endif

		  T_bound *low = new T_bound(ident,
									 FALSE,
									 NULL,
									 NULL,
									 new_inside,
									 father_pred_or_ident->get_betree(),
									 father_pred_or_ident->get_ref_lexem()) ;
		  T_bound *up = new T_bound(ident,
									TRUE,
									NULL,
									NULL,
									new_inside,
									father_pred_or_ident->get_betree(),
									father_pred_or_ident->get_ref_lexem()) ;

		  new_inside->set_bounds(low, up) ;
		}
#ifdef TRACE
	  else
		{
#ifdef DEBUG_PRED_TYPE
		  TRACE0("on garde la valuation des bornes") ;
#endif

		}
#endif

	  T_type *res = new T_setof_type(new_inside,
									 NULL,
									 father_pred_or_ident->get_betree(),
									 type->get_ref_lexem()) ;
	  new_inside->unlink() ;
	  expr->set_B_type(res, expr->get_ref_lexem()) ;
	  res->unlink() ;
	  return ;
	}

  // Cas 3 : cas par defaut
  // Recupere le type, en mettant les bornes a zero
#ifdef DEBUG_PRED_TYPE
   TRACE2("Cas 3, par defaut : clonage : %x, pere : %x", base_type, expr) ;
#endif
   T_type* expr_type = expr->get_B_type();

   if (expr_type == NULL || expr_type->has_wildcards(FALSE)) {
       // cas ou il faut faire de l'inference de type sur l'operande gauche
    T_type *res = base_type->clone_type(father_pred_or_ident, father_pred_or_ident->get_betree(), father_pred_or_ident->get_ref_lexem()) ;

#ifdef DEBUG_PRED_TYPE
  TRACE2("  clone : %x, type_decla : %x", res, type_decla) ;
#endif

  res->reset_bounds() ;
  TRACE2("AVANT MAJ TYPE %x:%s", expr, expr->get_class_name()) ;
  ENTER_TRACE ;
  expr->set_B_type(res, type_decla) ;
  res->unlink() ;
  EXIT_TRACE ;
  TRACE3("APRES MAJ TYPE %x:%s type %p", expr, expr->get_class_name(), expr->get_B_type()) ;

  TRACE0(">> TYPING_IDENT_ANALYSIS") ;

  TRACE7("expr %x %s:%d:%d %s is_a_variable %s is_a_constant %s",
		 expr,
		 expr->get_ref_lexem()->get_file_name()->get_value(),
		 expr->get_ref_lexem()->get_file_line(),
		 expr->get_ref_lexem()->get_file_column(),
		 expr->get_class_name(),
		 (expr->is_a_variable() == TRUE) ? "TRUE" : "FALSE",
		 (expr->is_a_constant() == TRUE) ? "TRUE" : "FALSE") ;

  // Recuperation de l'identificateur typant
  T_ident *ref_typing_ident = type->get_typing_ident() ;
  if (ref_typing_ident != NULL)
	{
	  TRACE1("maj du typing_ident de %x", expr) ;
	  expr->set_typing_ident(ref_typing_ident) ;
	}
   }
   else {
      // cas ou il ne faut faire que de la verification de type sur l'operande gauche
      // et eventuellement de l'inference de type sur l'operande droite.
      if (expr_type->is_compatible_with(base_type) == FALSE) {
          T_type* btype = ((T_setof_type*) type)->get_base_type();
	  semantic_error(expr,
                         CAN_CONTINUE,
                         get_error_msg(E_INCOMPATIBLE_TYPES_IN_PRED),
                         make_type_name(expr)->get_value(),
                         make_type_name(btype)->get_value()) ;
         if (type->get_node_type() == NODE_SETOF_TYPE &&
             type->get_base_type()->is_a_base_type() == TRUE &&
             expr->is_an_ident() == TRUE) {
             T_ident* id = (T_ident*) expr;
             semantic_error_details(id->get_typing_location(),
                                    get_error_msg(E_LOCATION_OF_IDENT_TYPE),
                                    make_type_name(expr)->get_value(),
				    id->get_def_type_name(),
				    id->get_name()->get_value()) ;
         }
      }

   }
  TRACE0("<< TYPING_IDENT_ANALYSIS") ;
}

// Fonction auxilaire d'affectation de type a un identificateur
void included_check_and_set_type(T_expr *expr,
										  T_setof_type *type,
										  T_lexem *type_decla,
										  T_item *father)
{
#ifdef DEBUG_PRED_TYPE
  TRACE3("included_check_and_set_type(%x, %x, %x)", expr, type, type_decla) ;
#endif


  // Cas 1 : B est un ensemble index large, i.e.
  // Gamma(B) = Psi(K(T, U, V))
  // et A est une constante
  if (	  (expr->is_a_constant() == TRUE)
	   && (type->is_an_extended_index_set() == TRUE) )
	{
	  // Il faut construire Setof(K(T, A-, A+))
#ifdef DEBUG_PRED_TYPE
	  TRACE0("Cas 1 (ensemble index large)") ;
#endif

	  T_type *inside = ((T_setof_type *)type)->get_base_type() ;
	  // Cast justifie apres
	  TRACE0("AVANT CLONAGE") ; ENTER_TRACE ;
	  T_base_type *new_inside = (T_base_type *)inside->clone_type(expr,
																  father->get_betree(),
																  father->get_ref_lexem()) ;
	  EXIT_TRACE ; TRACE0("APRES CLONAGE") ;
	  ASSERT(new_inside->is_a_base_type() == TRUE) ; // car provient d'ens idx large

	  T_ident *ident = (expr->is_an_ident() == TRUE)
		? expr->make_ident() : (T_ident *)NULL ;

	  if (ident != NULL)
		{
#ifdef DEBUG_PRED_TYPE
		  TRACE0("on met les bornes A-, A+)") ;
#endif

		  T_bound *low = new T_bound(ident,
									 FALSE,
									 NULL,
									 NULL,
									 new_inside,
									 father->get_betree(),
									 father->get_ref_lexem()) ;
		  T_bound *up = new T_bound(ident,
									TRUE,
									NULL,
									NULL,
									new_inside,
									father->get_betree(),
									father->get_ref_lexem()) ;

		  ((T_base_type *)new_inside)->set_bounds(low, up) ;
		  low->unlink() ;
		  up->unlink() ;

		  // Tres important : mise a jour du name
#ifdef DEBUG_PRED_TYPE
		  TRACE3("mise a jour du name de %x:%s avec %s",
				 new_inside,
				 new_inside->make_type_name()->get_value(),
				 ident->get_name()->get_value()) ;
#endif

		  ((T_base_type *)new_inside)->set_name(ident) ;

		  if (new_inside->get_node_type() == NODE_PREDEFINED_TYPE)
			{
			  ((T_predefined_type *)new_inside)->set_ref_interval(ident) ;
			}
		}

	  T_setof_type *res = new T_setof_type(new_inside,
										   expr,
										   father->get_betree(),
										   type->get_ref_lexem()) ;
	  new_inside->unlink() ;

#ifdef DEBUG_PRED_TYPE
	  TRACE3("Ici on a cree le type %x avec ref_lexem %x (type %x)",
			 res, type->get_ref_lexem(), type) ;
#endif

	  expr->set_B_type(res, expr->get_ref_lexem()) ;
	  object_unlink(res) ;
	  return ;
	}

  // Cas 2 : cas par defaut
  // Recupere le type, en mettant les bornes a zero
#ifdef DEBUG_PRED_TYPE
  TRACE0("Cas 2, par defaut") ;
#endif

  T_type* expr_type = expr->get_B_type();


  if (expr_type == NULL || expr_type->has_wildcards(FALSE)) {
      // cas ou il faut faire de l'inference de type sur l'operande gauche

      // Cast justifie car type est un T_Setof_type
      // Verifie a posteriori par l'assertion
      T_setof_type *res = (T_setof_type *)type->clone_type(father,
                                                           father->get_betree(),
                                                           father->get_ref_lexem()) ;

      if (res->get_node_type() == NODE_SETOF_TYPE)
          {
              res->reset_bounds() ;

              T_type *new_inside = res->get_base_type() ;

              if ( (new_inside->is_a_base_type() == TRUE) && (expr->is_an_ident() == TRUE) )
                  {
                      // Tres important : mise a jour du name
                      T_ident *ident = expr->make_ident() ;
#ifdef DEBUG_PRED_TYPE
                      TRACE3("mise a jour du name de %x:%s avec %s",
                             new_inside,
                             new_inside->make_type_name()->get_value(),
                             ident->get_name()->get_value()) ;
#endif

                      ((T_base_type *)new_inside)->set_name(ident) ;

                      if (new_inside->get_node_type() == NODE_PREDEFINED_TYPE)
                          {
                              ((T_predefined_type *)new_inside)->set_ref_interval(ident) ;
                          }
                  }
          }

      TRACE1("expr = %x", expr) ;
      TRACE1("expr = %s", expr->get_class_name()) ;
      ASSERT(expr->is_an_expr() == TRUE) ;
      expr->set_B_type(res, type_decla) ;
      res->unlink() ;
  }
  else {
      // cas ou il ne faut faire que de la verification de type sur l'operande gauche
      // et eventuellement de l'inference de type sur l'operande droite.
      if (expr_type->is_compatible_with(type) == FALSE) {
	  semantic_error(expr,
                         CAN_CONTINUE,
                         get_error_msg(E_INCOMPATIBLE_TYPES_IN_PRED),
                         make_type_name(expr)->get_value(),
                         type->make_type_name()->get_value()) ;
         if (type->get_node_type() == NODE_SETOF_TYPE &&
             type->get_base_type()->is_a_base_type() == TRUE &&
             expr->is_an_ident() == TRUE) {
             T_ident* id = (T_ident*) expr;
             semantic_error_details(id->get_typing_location(),
                                    get_error_msg(E_LOCATION_OF_IDENT_TYPE),
                                    make_type_name(id)->get_value(),
				    id->get_def_type_name(),
				    id->get_name()->get_value()) ;
         }
      }
  }
}

// Fonction auxilaire d'affectation de type a un identificateur
void equals_check_and_set_type(T_expr *expr,
										T_type *type,
										T_lexem *type_decla,
										T_item *father)
{
#ifdef DEBUG_PRED_TYPE
  TRACE3("equals_check_and_set_type(%x, %x, %x)", expr, type, type_decla) ;
  TRACE3("%s:%d:%d",
		 expr->get_ref_lexem()->get_file_name()->get_value(),
		 expr->get_ref_lexem()->get_file_line(),
		 expr->get_ref_lexem()->get_file_column()) ;
#endif

  T_type *res = NULL ;

  /*
  fprintf(stderr,
          "equals_check_and_set_type\n"
          "\texpr: %p\n"
          "\ttype: %p\n"
          "\tfather: %p\n",
          expr, type, father);
  if(expr != nullptr) expr->dump_html();
  if(type != nullptr) type->dump_html();
  if(father != nullptr) father->dump_html();
  */

  // Cas 1 : B est un ensemble index large, i.e.
  // Gamma(B) = Psi(K(T, U, V))
  // et A est une constante
  if (	  (expr->is_a_constant() == TRUE)
		  && (type->is_an_extended_index_set() == TRUE) )
	{
	  // Il faut construire Setof(K(T, A-, A+))
#ifdef DEBUG_PRED_TYPE
	  TRACE0("Cas 1 (ensemble index large) -> on fabrique un Setof(K(T, A-, A+))") ;
#endif

	  T_type *inside = ((T_setof_type *)type)->get_base_type() ;
	  // Cast justifie ci-apres
	  T_base_type *new_inside = (T_base_type *)inside->clone_type(NULL,
																  father->get_betree(),
																  father->get_ref_lexem()) ;
	  ASSERT(new_inside->is_a_base_type() == TRUE) ; // car provient d'ens idx large

	  T_ident *ident = (expr->is_an_ident() == TRUE)
		? expr->make_ident() : (T_ident *)NULL ;

#ifdef DEBUG_PRED_TYPE
	  TRACE3("ici expr =%x:%s, ident = %x",
			 expr,
			 expr->get_class_name(),
			 ident) ;
#endif

	  if (ident != NULL)
		{
		  if (new_inside->are_bounds_set() == FALSE)
			{
#ifdef DEBUG_PRED_TYPE
			  TRACE0("on fabrique un setof(k(t,a-,a+))") ;
#endif

			  T_bound *low = new T_bound(ident,
										 FALSE,
										 NULL,
										 NULL,
										 new_inside,
										 father->get_betree(),
										 father->get_ref_lexem()) ;
			  T_bound *up = new T_bound(ident,
										TRUE,
										NULL,
										NULL,
										new_inside,
										father->get_betree(),
										father->get_ref_lexem()) ;

			  new_inside->set_bounds(low, up) ; // prise de responsabilite
			}

		  // Tres important : mise a jour du name
#ifdef DEBUG_PRED_TYPE
		  TRACE3("mise a jour du name de %x:%s avec %s",
				 new_inside,
				 new_inside->make_type_name()->get_value(),
				 ident->get_name()->get_value()) ;
#endif

		  new_inside->set_name(ident) ;

		  if (new_inside->get_node_type() == NODE_PREDEFINED_TYPE)
			{
			  ((T_predefined_type *)new_inside)->set_ref_interval(ident) ;
			}
		  TRACE2("ICI new_inside %x:%s",
				 new_inside,
				 new_inside->make_type_name()->get_value()) ;
		}
#ifdef TRACE
	  else
		{
		  TRACE0("on garde les bornes") ;
		}
#endif

	  res = new T_setof_type(new_inside,
							 NULL,
							 father->get_betree(),
							 type->get_ref_lexem()) ;
	  new_inside->unlink() ;
	  expr->set_B_type(res, expr->get_ref_lexem()) ;
	  res->unlink() ;
	}
  else
	{
#if 0
	  // Cas 2 : A est une constante, B est un produit cartesien
	  // Recupere le type, en mettant les bornes a zero

	  // Debut SL 15/01/99 en fait il faut tester l'objet dont le type est
	  // B, et pas B en tant que type. C'est cet objet qui doit etre un
	  // produit cartesien pour que les bornes soient remises a NULL. Mais
	  // dans cette fonction, on n'a pas acces a cet objet

	  // Ce n'est pas grave car la remise a NULL des bornes etait faite
	  // pour faire "comme le TC". Dans le TC, cela servait a preparer
	  // l'interdiction de l'utilisation de A comme ensemble index. Ce
	  // controle est a present fait dans le B0C

	  // Donc ce cas particulier n'existe plus

	  /// Fin SL 15/01/99
#endif

	  // Cas 3 : cas par defaut
	  // Recupere le type
#ifdef DEBUG_PRED_TYPE
	  TRACE2("Cas 3, par defaut <type = %x:%s>",
			 type,
			 make_type_name(type)->get_value()) ;
#endif
          T_type* expr_type = expr->get_B_type();

          if (expr_type == NULL || expr_type->has_wildcards(FALSE)) {
              // cas ou il faut faire de l'inference de type sur l'operande gauche

              // Si le type en partie droite est un record, on ne fait
              // pas de copie privee car on va vouloir le modifier
              // s'il comporte des jokers
              res = NULL ;

              if (type->get_node_type() == NODE_RECORD_TYPE)
                  {
                      res = type ;
                      res->link() ;
                  }
              else
                  {
                      res = type->clone_type(father, father->get_betree(), father->get_ref_lexem()) ;
                  }

              if (res->is_a_set() == TRUE)
                  {
                      // Tres important : mise a jour du name
                      // Cast justifie a posteriori le cas echeant
                      T_base_type *new_inside = (T_base_type *)
                          ((T_setof_type *)res)->get_base_type() ;
                      T_ident *ident = (expr->is_an_ident() == TRUE)
                          ? expr->make_ident() : (T_ident *)NULL ;

                      if ( (ident != NULL) &&
			   (new_inside != NULL) &&
			   (new_inside->is_a_base_type() == TRUE) )
                          {
#ifdef DEBUG_PRED_TYPE
                              TRACE3("mise a jour du name de %x:%s avec %s",
                                     new_inside,
                                     new_inside->make_type_name()->get_value(),
                                     ident->get_name()->get_value()) ;
#endif

                              new_inside->set_name(ident) ;

                              if (new_inside->get_node_type() == NODE_PREDEFINED_TYPE)
                                  {
                                      ((T_predefined_type *)new_inside)->set_ref_interval(ident) ;
                                  }

                              TRACE2("ICI new_inside %x:%s",
                                     new_inside,
                                     new_inside->make_type_name()->get_value()) ;
                          }
                  }

              expr->set_B_type(res, type_decla) ;
              res->unlink() ;
          }
          else {
              // cas ou il faut ne faire que de la verification de type a gauche
              // et eventuellement de l'inference a droite.
              if (expr_type->is_compatible_with(type) == FALSE) {
                  semantic_error(expr,
                                 CAN_CONTINUE,
                                 get_error_msg(E_INCOMPATIBLE_TYPES_IN_PRED),
                                 make_type_name(expr)->get_value(),
                                 type->make_type_name()->get_value()) ;
              }
          }
        }

  TRACE1(">> TYPING_IDENT_ANALYSIS (res = %p)", res) ;

  if (    (res != NULL)
	   && (expr->is_an_ident() == TRUE)
	   && (expr->get_B_type() != NULL)// peut arriver si le ctx n'etait pas typant
		  // cf bug 2233

		  // Bug 2258 : seuls les types tableaux et records sont a
		  // prendre en compte lors du calcul des identificateurs
		  // typants
	   && (    (expr->get_B_type()->is_an_array_type_definition() == TRUE)
			|| (expr->get_B_type()->is_a_record_type_definition() == TRUE)
			|| (expr->get_B_type()->is_an_integer_type_definition() == TRUE) )
	 )
	{
	  // Cast justifie par le test ci-dessus
	  T_ident *name = (T_ident *)expr->make_expr() ;
	  TRACE2("maj typing ident de %x avec %s",
			 expr->get_B_type(),
			 name->get_name()->get_value()) ;
	  expr->get_B_type()->set_typing_ident(name) ;
	}

  TRACE0("<< TYPING_IDENT_ANALYSIS") ;
}



// Typage d'un identificateur a la fois
void single_type(T_expr *expr,
                 T_type *type,
                 T_item *rhs,
                 T_typing_predicate_case tcase,
                 T_item *father)
{
#ifdef DEBUG_PRED_TYPE
  TRACE5("T_typing_predicate(%x)::single_type(%x, %x:%s, tcase = %d)",
		 father,
		 expr,
		 type,
		 (type == NULL) ? "NULL" : type->make_type_name()->get_value(),
		 tcase) ;
#endif
  /*
  fprintf(stderr,
          "T_typing_predicate::single_type\n"
          "\texpr: %p\n"
          "\ttype: %p\n"
          "\trhs: %p\n"
          "\tfather: %p\n",
          expr, type, rhs, father);
  if (expr != NULL)expr->dump_html();
  if (type != NULL)type->dump_html();
  if (rhs != NULL)rhs->dump_html();
  if (father != NULL)father->dump_html();
  */
  T_lexem *ref_lexem = rhs->get_ref_lexem();

  T_setof_type *setof = NULL ;

  // Pour les cas ITYPE_1_1 et ITYPE_1_2, le type doit obligatoirement
  // etre un ensemble
  // On en profite pour mettre a jour setof
  if (tcase != ITYPE_1_3)
	{
	  if (type->is_a_set() == FALSE)
		{
		  semantic_error(father,//type,
						 CAN_CONTINUE,
						 get_error_msg(E_RHS_OF_BELONGS_MUST_BE_A_SET),
						 type->make_type_name()->get_value()) ;
		  return ;
		}
	  setof = (T_setof_type *)type ;
	}

  switch(tcase)
	{
	case ITYPE_1_1 :
	  {
		belongs_check_and_set_type(expr, setof, ref_lexem, father) ;
		break ;
	  }

	case ITYPE_1_2 :
	  {
		included_check_and_set_type(expr, setof, ref_lexem, father) ;
		break ;
	  }

	case ITYPE_1_3 :
	  {
		equals_check_and_set_type(expr, type, ref_lexem, father) ;
		break ;
	  }

	// Pas de default pour que le compilateur nous
	// previenne si on oublie un cas
	}
  if(rhs->is_an_expr()) {
      T_expr* expr_rhs = (T_expr*) rhs;
      T_type* type_rhs = expr_rhs->get_B_type();
      if(type_rhs == NULL ||
         (type_rhs->has_wildcards(TRUE) &&
          type_rhs->is_compatible_with(expr->get_B_type()))) {
          expr_rhs->set_B_type(expr->get_B_type(), ref_lexem);
      }
  }
}

// Type check, predicats de typage
// Boucle sur les identificateurs
// real_type est le type du prédicat apres follow_indirection
// tcase est la nature du typage (ITYPE_1_1, 1_2, ...)
// first_ident est le premier ident typé
// last_ident est le dernier ident_typé
// father est l'item utilisé comme père pour
// typing_predicate_type est la nature du prédicat


// !!!
void type_check_loop(T_type *real_type,
							  T_typing_predicate_case tcase,
							  T_expr *first_ident,
							  T_expr *last_ident,
							  T_item *father,
							  T_item *type,
                              T_typing_predicate_type typing_predicate_type,
                              T_item * tpred)
{
#ifdef DEBUG_PRED_TYPE
  TRACE0("T_typing_predicate::type_check_loop") ;
#endif

#ifdef DEBUG_PRED_TYPE
  TRACE2("real type %x %s",
		 real_type,
		 (real_type == NULL) ? "NULL" :real_type->make_type_name()->get_value()) ;
#endif

  if (real_type == NULL)
	{
	  // Reprise sur erreur
#ifdef DEBUG_PRED_TYPE
	  TRACE0("ARG .. type = NULL") ;
#endif

	  return ;
	}

#ifdef DEBUG_PRED_TYPE
  TRACE2("ici first_ident %x:%s", first_ident, first_ident->get_class_name()) ;
#endif

  // Cas particulier : Affectation d'un seul identificateur

  if (first_ident==last_ident)
	{
	  // A FAIRE : pas vrai, peut etre un produit de setof et
	  // meme plus generalement un ensemble (?)
	  // Le type en partie droite DOIT etre un Setof
	  // A FAIRE
#ifdef DEBUG_PRED_TYPE
	  TRACE0("cas particulier : affectation d'un identificateur") ;
	  TRACE1("ici type %x", type) ;
#endif

	  single_type(first_ident, real_type, type, tcase, father) ;

	  //GP 22/01/99
	  //Utilisation des string
	  if(tcase == ITYPE_1_1)
		{
		  //TRUE car c'est du BELONGS
		  first_ident->check_string_uses(TRUE) ;
		}
	  else
		{
		  //FALSE car ce n'est pas du BELONGS
		  first_ident->check_string_uses(FALSE) ;
		}

	  return ;
	}

  // Cas general : affectation de n identificateurs
  // Il faut que le type soit un Setof(produit cartesien de n ensembles)
#ifdef DEBUG_PRED_TYPE
  TRACE0("Cas general : affectation de n identificateurs") ;
#endif

  size_t nb_ident = 0 ;
  size_t nb_types = 0 ;
  T_product_type *ptype = NULL ;

  T_expr *cur = first_ident ;
  while (cur != NULL)
	{
	  nb_ident ++ ;
	  cur = (T_expr *)cur->get_next() ;
	}

  if (typing_predicate_type == TPRED_EQUAL)
	{
	  // Partie droite : un produit cartesien
	  if (real_type->get_node_type() == NODE_PRODUCT_TYPE)
		{
		  ptype = (T_product_type *)real_type ;
		  nb_types = ptype->get_nb_types() ;
		}
	}
  else
	{
	  // Partie droite : un ensemble de produits cartesiens
	  if (real_type->get_node_type() == NODE_SETOF_TYPE)
		{
		  T_setof_type *setof = (T_setof_type *)real_type ;

		  if (setof->get_base_type()->get_node_type() == NODE_PRODUCT_TYPE)
			{
			  ptype = (T_product_type *)(setof->get_base_type()) ;
			  nb_types = ptype->get_nb_types() ;
			}
		}
	}


#ifdef DEBUG_PRED_TYPE
  TRACE2("nb_ident %d nb_types %d", nb_ident, nb_types) ;
#endif

  // Erreur si type n'est pas un produit cartesien, ou un produit cartesien
  // du mauvais nombre d'ensembles
  //
  // Sauf dans le cas du ':' (tcase ITYPE_1_1) ou il faut plus
  // d'ensembles que d'ident
  if (    ( (tcase != ITYPE_1_1) && (nb_ident != nb_types) )
		  || ( /*(tcase == ITYPE_1_1) implicite && */ (nb_ident > nb_types) ) )
	{
	  semantic_error(first_ident,
					 CAN_CONTINUE,
					 get_error_msg(E_WRONG_NUMBER_OF_TYPES_GIVEN),
					 real_type->make_type_name()->get_value(),
					 nb_ident) ;
	  return ;
	}

  // C'est parti, on affecte les types aux identificateurs
  //
  // Algo general (pas pour x, y : E1*E2*E3)
  if ((tcase != ITYPE_1_1) || (nb_ident == nb_types) )
	{
	  cur = first_ident ;

	  T_type *cur_type = ptype->get_types() ;
	  TRACE3("AFFECT ptype %x:%s, cur_type %s",
			 ptype,
			 ptype->make_type_name()->get_value(),
			 cur_type->make_type_name()->get_value()) ;

	  while (cur != NULL)
		// Il suffit de tester cur car on sait que nb_types == nb_ident
		{
		  if (cur_type == NULL)
			{
			  fprintf(stderr,
					  "PANIC %s:%d:%d\n",
					  father->get_ref_lexem()->get_file_name()->get_value(),
					  father->get_ref_lexem()->get_file_line(),
					  father->get_ref_lexem()->get_file_column()) ;
			  TRACE4("PANIC %s:%d:%d this %x\n",
					 father->get_ref_lexem()->get_file_name()->get_value(),
					 father->get_ref_lexem()->get_file_line(),
					 father->get_ref_lexem()->get_file_column(),
					 father) ;
			  fflush(stderr) ;
			}
		  ASSERT(cur_type != NULL) ;
		  T_type *next_type = cur_type->get_next_type() ;
		  /*
			#ifdef DEBUG_PRED_TYPE
			TRACE4("ici on type %x:%s avec %x:%s",
			cur, ((T_ident *)cur)->get_name()->get_value(),
			cur_type, cur_type->make_type_name()) ;
			#endif*/

		  int do_unlink = FALSE ;
		  if (typing_predicate_type != TPRED_EQUAL)
			{
			  // Il faut fabriquer un Setof(xx) pour emuler
			  // x: S(E) et y:S(F) a partir de x,y : S(E*F)
			  // Pour cela on clone cur_type car la fabrication du Setof modifie
			  // son pere
			  T_type *clone = cur_type->clone_type(NULL,
												   father->get_betree(),
												   father->get_ref_lexem()) ;
			  cur_type = new T_setof_type(clone,
										  father,
										  father->get_betree(),
										  father->get_ref_lexem()) ;
			  clone->unlink() ;
			  do_unlink = TRUE ;
			}

		  single_type(cur, cur_type, type, tcase, father) ;

		  //GP 22/01/99
		  //Utilisation des string
		  if(tcase == ITYPE_1_1)
			{
			  //TRUE car c'est du BELONGS
			  cur->check_string_uses(TRUE) ;
			}
		  else
			{
			  //FALSE car ce n'est pas du BELONGS
			  cur->check_string_uses(FALSE) ;
			}


		  if (do_unlink == TRUE)
			{
			  cur_type->unlink() ;
			}
		  cur = (T_ident *)cur->get_next() ;
		  cur_type = next_type ;
		}
	}
  else
	{
	  // Algo specialise pour x, y : E1*E2*E3
	  //
	  // On parcourt les idents a l'envers
	  cur = last_ident ;

	  T_type *cur_type = ptype ;

	  TRACE3("AFFECT ptype %x:%s, cur_type %s",
			 ptype,
			 ptype->make_type_name()->get_value(),
			 cur_type->make_type_name()->get_value()) ;

	  while (cur != NULL)
		// Il suffit de tester cur car on sait que nb_types == nb_ident
		{
		  if (cur_type == NULL)
			{
			  fprintf(stderr,
					  "PANIC %s:%d:%d\n",
					  father->get_ref_lexem()->get_file_name()->get_value(),
					  father->get_ref_lexem()->get_file_line(),
					  father->get_ref_lexem()->get_file_column()) ;
			  TRACE4("PANIC %s:%d:%d father %x\n",
					 father->get_ref_lexem()->get_file_name()->get_value(),
					 father->get_ref_lexem()->get_file_line(),
					 father->get_ref_lexem()->get_file_column(),
					 father) ;
			  fflush(stderr) ;
			}
		  ASSERT(cur_type != NULL) ;

		  // Prochain ident
		  T_ident *next_ident = (T_ident *)cur->get_prev() ;

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

		  TRACE2("ident_type %s next_type %s",
				 make_type_name(ident_type),
				 make_type_name(next_type)) ;

		  // Il faut fabriquer un Setof(xx) pour emuler
		  // x: S(E) et y:S(F) a partir de x,y : S(E*F)
		  // Pour cela on clone cur_type car la fabrication du Setof modifie
		  // son pere
		  T_type *clone = ident_type->clone_type(NULL,father->get_betree(),father->get_ref_lexem());
		  ident_type = new T_setof_type(clone, father, father->get_betree(), father->get_ref_lexem());


          if(cur->get_node_type() == NODE_EXPR_WITH_PARENTHESIS /*&& ident_type->get_node_type() == NODE_PRODUCT_TYPE*/)
                  {
                     T_expr * inner = ((T_expr_with_parenthesis *)cur)->strip_parenthesis();
                     T_expr * inner_clone = (T_expr *)inner->clone_object();
                     T_typing_predicate * typepred = new T_typing_predicate(inner_clone,ident_type,typing_predicate_type,
                                                                                 NULL, NULL, NULL, tpred->get_betree(), tpred->get_ref_lexem());
                     typepred->type_check();
                   }
                   else
		  single_type(cur, ident_type, type, tcase, father) ;

		  //TRUE car c'est du BELONGS
		  cur->check_string_uses(TRUE) ;

		  // La suite
		  cur = next_ident ;
		  cur_type = next_type ;
		}
	}
}

//
//	}{	Classe T_binary_predicate
//
void T_binary_predicate::type_check(void)
{
#ifdef DEBUG_PRED_TYPE
  TRACE1("T_binary_predicate(%x)::type_check", this) ;
#endif

  pred1->type_check() ;
  pred2->type_check() ;
}

//
//	}{	Classe T_not_predicate
//
// CTRL Ref VTYPE1 (2.1)
void T_not_predicate::type_check(void)
{
#ifdef DEBUG_PRED_TYPE
  TRACE1("T_not_predicate(%x)::type_check", this) ;
#endif

  predicate->type_check() ;
}

//
//	}{	Classe T_expr_predicate
//
void T_expr_predicate::type_check(void)
{
#ifdef DEBUG_PRED_TYPE
  TRACE1("T_expr_predicate(%x)::type_check", this) ;
#endif


  // On commence par calculer les types des deux expressions
  expr1->make_type() ;
  expr2->make_type() ;

  // Ensuite, selon le type de predicat, on agit
  switch(predicate_type)
	{
	case EPRED_LESS_THAN :
	case EPRED_GREATER_THAN :
	case EPRED_LESS_THAN_OR_EQUAL :
	case EPRED_GREATER_THAN_OR_EQUAL :
	  {
		type_check_order() ;
		return ;
      }

	case EPRED_NOT_INCLUDED :
	case EPRED_NOT_STRICT_INCLUDED :
	  {
		type_check_not_included() ;
		return ;
	  }

	case EPRED_DIFFERENT :
	  {
		type_check_different() ;
		return ;
	  }

	case EPRED_NOT_BELONGS :
	  {
		type_check_not_belongs() ;
		return ;
	  }

	// Pas de default pour que le compilateur nous previenne
	// si on a oublie des cas
	}
}

// Type check pour les relations d'ordre
// CTRL Ref: VTYPE 1 (7)
void T_expr_predicate::type_check_order(void)
{
#ifdef DEBUG_PRED_TYPE
  TRACE1("T_expr_predicate(%x)::type_check_order", this) ;
#endif

  //GP 07/01/99
  //Pour lever une erreur si expr1 et expr2 sont des idents non encore type
  expr1->check_type() ;
  expr2->check_type() ;

  // Il faut verifier que les deux operandes sont de type entier
  T_type *t1 = expr1->get_B_type() ;

  if ( (t1 != NULL) && (t1->is_an_integer() == FALSE) &&
       (t1->is_a_real() == FALSE) &&
       (t1->is_a_float() == FALSE))
  {
      semantic_error(this,
                     CAN_CONTINUE,
                     get_error_msg(E_FIRST_OPERAND_OF_PRED_SHOULD_BE_AN_INTEGER),
                     make_type_name(expr1)->get_value()) ;
  }

  T_type *t2 = expr2->get_B_type() ;
  if ( (t2 != NULL) && (t2->is_an_integer() == FALSE)  &&
       (t2->is_a_real() == FALSE)  &&
       (t2->is_a_float() == FALSE))
  {
      semantic_error(this,
                     CAN_CONTINUE,
                     get_error_msg(E_SECOND_OPERAND_OF_PRED_SHOULD_BE_AN_INTEGER),
                     make_type_name(expr2)->get_value()) ;
  }
  if ( (t1 != NULL) && (t2 != NULL) && (t1->is_an_integer() == TRUE)  && (t2->is_a_real() == TRUE))
  {
      semantic_error(this,
                     CAN_CONTINUE,
                     get_error_msg(E_SECOND_OPERAND_OF_PRED_SHOULD_BE_AN_INTEGER),
                     make_type_name(expr2)->get_value()) ;
  }
  if ( (t1 != NULL) && (t2 != NULL) && (t1->is_an_integer() == TRUE)  && (t2->is_a_float() == TRUE))
  {
      semantic_error(this,
                     CAN_CONTINUE,
                     get_error_msg(E_SECOND_OPERAND_OF_PRED_SHOULD_BE_AN_INTEGER),
                     make_type_name(expr2)->get_value()) ;
  }
  if ( (t1 != NULL) && (t2 != NULL) && (t1->is_a_real() == TRUE)  && (t2->is_an_integer() == TRUE))
  {
      semantic_error(this,
                     CAN_CONTINUE,
                     get_error_msg(E_SECOND_OPERAND_OF_PRED_SHOULD_BE_A_REAL),
                     make_type_name(expr1)->get_value()) ;
  }
  if ( (t1 != NULL) && (t2 != NULL) && (t1->is_a_real() == TRUE)  && (t2->is_a_float() == TRUE))
  {
      semantic_error(this,
                     CAN_CONTINUE,
                     get_error_msg(E_SECOND_OPERAND_OF_PRED_SHOULD_BE_A_REAL),
                     make_type_name(expr1)->get_value()) ;
  }
  if ( (t1 != NULL) && (t2 != NULL) && (t1->is_a_float() == TRUE)  && (t2->is_an_integer() == TRUE))
  {
      semantic_error(this,
                     CAN_CONTINUE,
                     get_error_msg(E_SECOND_OPERAND_OF_PRED_SHOULD_BE_A_FLOAT),
                     make_type_name(expr1)->get_value()) ;
  }
  if ( (t1 != NULL) && (t2 != NULL) && (t1->is_a_float() == TRUE)  && (t2->is_a_real() == TRUE))
  {
      semantic_error(this,
                     CAN_CONTINUE,
                     get_error_msg(E_SECOND_OPERAND_OF_PRED_SHOULD_BE_A_REAL),
                     make_type_name(expr1)->get_value()) ;
  }


  // Et c'est tout !
}

// Type check pour les "non inclusions"
// CTRL Ref: VTYPE 1 (6)
void T_expr_predicate::type_check_not_included(void)
{
#ifdef DEBUG_PRED_TYPE
  TRACE1("T_expr_predicate(%x)::type_check_not_included", this) ;
#endif


  // Il faut verifier que les deux operandes sont de type ensemble
  if( expr1->is_a_set() == FALSE)
	{
	  semantic_error(this,
					 CAN_CONTINUE,
					 get_error_msg(E_FIRST_OPERAND_OF_PRED_SHOULD_BE_A_SET),
					 make_type_name(expr1)->get_value()) ;
	}

  if (expr2->is_a_set() == FALSE)
	{
	  semantic_error(this,
					 CAN_CONTINUE,
					 get_error_msg(E_SECOND_OPERAND_OF_PRED_SHOULD_BE_A_SET),
					 make_type_name(expr2)->get_value()) ;
	}

  // Et c'est tout !
}

// Type check pour '/='
// CTRL Ref: VTYPE 1(4)
void T_expr_predicate::type_check_different(void)
{
#ifdef DEBUG_PRED_TYPE
  TRACE1("T_expr_predicate(%x)::type_check_different", this) ;
#endif


  //GP 07/01/99
  //Pour lever une erreur si expr1 et expr2 sont des idents non encore type
  expr1->check_type() ;
  expr2->check_type() ;

  // Il faut verifier que les deux operandes sont de meme type

  T_type *t1 = expr1->get_B_type() ;
  if (t1 == NULL)
	{
	  // Reprise sur erreur
	  return ;
	}
  T_type *t2 = expr2->get_B_type() ;
  if (t2 == NULL)
	{
	  // Reprise sur erreur
	  return ;
	}
  if (t1->is_compatible_with(t2) == FALSE)
	{
	  semantic_error(this,
					 CAN_CONTINUE,
					 get_error_msg(E_INCOMPATIBLE_TYPES_IN_PRED),
					 make_type_name(expr1)->get_value(),
					 make_type_name(expr2)->get_value()) ;

	}

  // Et c'est tout !
}


// Type check pour '/:'
// CTRL Ref: VTYPE 1 (5)
void T_expr_predicate::type_check_not_belongs(void)
{
#ifdef DEBUG_PRED_TYPE
  TRACE1("T_expr_predicate(%x)::type_check__not_belongs", this) ;
#endif

  //GP 07/01/99
  //Pour lever une erreur si expr1 et expr2 sont des idents non encore type
  expr1->check_type() ;
  expr2->check_type() ;

  // Il faut verifier que les deux operandes sont de meme type

  T_type *t1 = expr1->get_B_type() ;
  if (t1 == NULL)
	{
	  // Reprise sur erreur
	  return ;
	}
  T_type *t2 = expr2->get_B_type() ;
  if (t2 == NULL)
	{
	  // Reprise sur erreur
	  return ;
	}

  if (t2->is_a_set() == FALSE)
	{
	  semantic_error(this,
					 CAN_CONTINUE,
					 get_error_msg(E_SECOND_OPERAND_OF_PRED_SHOULD_BE_A_SET),
					 t2->make_type_name()->get_value()) ;
	  return ;
	}


  // Cast justifie grace au test ci-dessus
  T_type *base_type = ((T_setof_type *)t2)->get_base_type() ;
  if (t1->is_compatible_with(base_type) == FALSE)
	{
	  semantic_error(this,
					 CAN_CONTINUE,
					 get_error_msg(E_INCOMPATIBLE_TYPES_IN_PRED),
					 make_type_name(expr1)->get_value(),
					 make_type_name(expr2)->get_value()) ;

	}

  // Et c'est tout !
}


//
//	}{	Classe T_universal_predicate
//
// CTRL Ref: VTYPE1 (2.3)
void T_universal_predicate::type_check(void)
{
#ifdef DEBUG_PRED_TYPE
  TRACE1("T_universal_predicate(%x)::type_check", this) ;
#endif


  /* Il faut verifier que les variables sont deux a deux distinctes */
  check_no_double_definition(first_variable) ;

  /* Puis verifier le predicat a l'interieur */
  predicate->type_check() ;

  //GP 15/01/99
  //Il faut que les variables soient typee
  check_type_ident_list(get_variables()) ;
}

//
//	}{	Classe T_existential_predicate
//
// CTRL Ref: VTYPE1 (2.2)
void T_existential_predicate::type_check(void)
{
#ifdef DEBUG_PRED_TYPE
  TRACE1("T_existential_predicate(%x)::type_check", this) ;
#endif

  /* Il faut verifier que les variables sont deux a deux distinctes */
  check_no_double_definition(first_variable) ;

  /* Puis verifier le predicat a l'interieur */
  predicate->type_check() ;

  //GP 15/01/99 Verifier que les variables sont toutes typee
  check_type_ident_list(get_variables()) ;
}

//
//	}{ Classe T_predicate_with_parenthesis
//
void T_predicate_with_parenthesis::type_check(void)
{
#ifdef DEBUG_PRED_TYPE
  TRACE1("T_predicate_with_parenthesis(%x)::type_check", this) ;
#endif

  ENTER_TRACE ;
  predicate->type_check() ;
  EXIT_TRACE ;
}

//
//
// Fonctions pour savoir si une formule est typante
//
// 3 grandes categories :
// 1) les predicats : de typage
// 2) les substitutions de typage
// 3) les predicats/expressions introduisant des quantificateurs locaux
// 4) le reste
//

//
// 1) les predicats : de typage
//

int T_predicate_with_parenthesis::is_a_typing_formula(void)
{
#ifdef DEBUG_PRED_TYPE
  TRACE1("T_predicate_with_parenthesis(%x)::is_a_typing_formula() ?", this) ;
#endif // DEBUG_PRED_TYPE

  // Il ne faut pas oublier que l'analyse se fait "en remontant" : si
  // on est arrive ici, c'est que predicate est potentiellement un
  // real_typing_predicate. Il faut donc propager au pere si c'est un
  // predicat, soit dire oui
  if ( (get_father() != NULL) && (get_father()->is_a_predicate() == TRUE) )
	{
      int is_father_typing = get_father()->is_a_typing_formula() ;
      if (is_father_typing == TRUE)
      {
          return TRUE;
      }
      else if (is_father_typing == QUANTIFIED_TYPING_PREDICATE)
      {
          // Le père est un "=>" dans un quelque soit. Seul le prédicat1 est typant
          if (this == ((T_binary_predicate*) get_father())->get_pred1())
          {
              return TRUE;
          }
          else
          {
              return FALSE;
          }
      }
      else
      {
          return FALSE;
      }
	}
  else
	{
	  return TRUE ;
	}

}

int T_binary_predicate::is_a_typing_formula(void)
{
#ifdef DEBUG_PRED_TYPE
  TRACE4("T_binary_predicate(%x,%d)::is_a_typing_formula(father %x %s) ?",
		 this,
		 predicate_type,
		 get_father(),
		 (get_father() == NULL) ? "" : get_father()->get_class_name()) ;
#endif // DEBUG_PRED_TYPE

  // Pour commencer, Est-ce une predicat typant ?
  // Oui ssi c'est un "&" ou un "=>" dans un quelque soit
  int is_valid = TRUE ;

  if (predicate_type == BPRED_IMPLIES)
	{
	  // C'est a priori mal parti SAUF dans le super cas particulier
	  // du predicat universel, du type :
	  // !xx(xx : INT => xx > 0)  : dans ce cas le '=>' est dans la syntaxe
	  // Pour detecter ce cas il faut verifier les peres : () puis !
	  if (    (get_father() != NULL)
		   && (get_father()->get_node_type() == NODE_PREDICATE_WITH_PARENTHESIS)
		   && (get_father()->get_father() != NULL)
		   && (get_father()->get_father()->get_node_type()
			   == NODE_UNIVERSAL_PREDICATE) )
		{
		  // Saved by the bell
          is_valid = QUANTIFIED_TYPING_PREDICATE;
		}
	  else
		{
		  // Cas general : ce n'est pas typant
		  is_valid = FALSE ;
		}
	}
  else if (predicate_type != BPRED_CONJONCTION)
	{
	  is_valid = FALSE ;
	}


  if (is_valid == FALSE)
	{
	  // Pas la peine de continuer !
	  return FALSE ;
	}

  if (get_father() == NULL)
	{
	  // C'est bon
      return is_valid ;
	}
  else if (get_father()->is_a_predicate() == FALSE)
	{
	  // C'est bon, on a fini le predicat
      return is_valid ;
	}
  // Il faut continuer a remonter les predicats
  else
  {
      int is_father_typing = get_father()->is_a_typing_formula() ;
      if (is_father_typing == TRUE)
      {
          return is_valid;
      }
      else if (is_father_typing == QUANTIFIED_TYPING_PREDICATE)
      {
          // Le père est un "=>" dans un quelque soit. Seul le prédicat1 est typant
          if (this == ((T_binary_predicate*) get_father())->pred1)
          {
              return TRUE;
          }
          else
          {
              return FALSE;
          }
      }
      else
      {
          return FALSE;
      }
  }
}

int T_typing_predicate::is_a_typing_formula(void)
{
#ifdef DEBUG_PRED_TYPE
  TRACE3("T_typing_predicate(%x)::is_a_typing_formula(father %x:%s)",
		 this,
		 get_father(),
		 (get_father() == NULL) ? "" : get_father()->get_class_name()) ;
#endif // DEBUG_PRED_TYPE

  if (get_father() == NULL)
	{
	  return TRUE ;
	}
  else if (get_father()->is_a_predicate() == FALSE)
	{
	  // C'est bon, on a fini le predicat
	  return TRUE ;
	}
  else
	{
      int is_father_typing = get_father()->is_a_typing_formula() ;
      if (is_father_typing == TRUE)
      {
          return TRUE;
      }
      else if (is_father_typing == QUANTIFIED_TYPING_PREDICATE)
      {
          // Le père est un "=>" dans un quelque soit. Seul le prédicat1 est typant
          if (this == ((T_binary_predicate*) get_father())->get_pred1())
          {
              return TRUE;
          }
          else
          {
              return FALSE;
          }
      }
      else
      {
          return FALSE;
      }
	}
}

//
// 2) les substitutions de typage
//

// :=
int T_affect::is_a_typing_formula(void)
{
#ifdef DEBUG_PRED_TYPE
  TRACE1("T_affect(%x)::is_a_typing_formula() -> TRUE",
		 this) ;
#endif // DEBUG_PRED_TYPE

  return TRUE ;
}

// <--
int T_op_call::is_a_typing_formula(void)
{
#ifdef DEBUG_PRED_TYPE
  TRACE1("T_op_call(%x)::is_a_typing_formula() -> TRUE",
		 this) ;
#endif // DEBUG_PRED_TYPE

  return TRUE ;
}

// ::
int T_becomes_such_that::is_a_typing_formula(void)
{
#ifdef DEBUG_PRED_TYPE
  TRACE1("T_becomes_such_that(%x)::is_a_typing_formula() -> TRUE",
		 this) ;
#endif // DEBUG_PRED_TYPE

  return TRUE ;
}

// :
int T_becomes_member_of::is_a_typing_formula(void)
{
#ifdef DEBUG_PRED_TYPE
  TRACE1("T_becomes_member_of(%x)::is_a_typing_formula() -> TRUE",
		 this) ;
#endif // DEBUG_PRED_TYPE

  return TRUE ;
}

//
// 3) les predicats/expressions introduisant des quantificateurs locaux
//
int T_existential_predicate::is_a_typing_formula(void)
{
#ifdef DEBUG_PRED_TYPE
  TRACE1("T_existential_predicate(%x)::is_a_typing_formula() -> TRUE",
		 this) ;
#endif // DEBUG_PRED_TYPE

  return TRUE ;
}

int T_universal_predicate::is_a_typing_formula(void)
{
#ifdef DEBUG_PRED_TYPE
  TRACE1("T_universal_predicate(%x)::is_a_typing_formula() -> TRUE",
		 this) ;
#endif // DEBUG_PRED_TYPE

  return TRUE ;
}

int T_lambda_expr::is_a_typing_formula(void)
{
#ifdef DEBUG_PRED_TYPE
  TRACE1("T_lambda_expression(%x)::is_a_typing_formula() -> TRUE",
		 this) ;
#endif // DEBUG_PRED_TYPE

  return TRUE ;
}

int T_quantified_union::is_a_typing_formula(void)
{
#ifdef DEBUG_PRED_TYPE
  TRACE1("T_quantified_union(%x)::is_a_typing_formula() -> TRUE",
		 this) ;
#endif // DEBUG_PRED_TYPE

  return TRUE ;
}

int T_quantified_intersection::is_a_typing_formula(void)
{
#ifdef DEBUG_PRED_TYPE
  TRACE1("T_quantified_intersection(%x)::is_a_typing_formula() -> TRUE",
		 this) ;
#endif // DEBUG_PRED_TYPE

  return TRUE ;
}

int T_sigma::is_a_typing_formula(void)
{
#ifdef DEBUG_PRED_TYPE
  TRACE1("T_sigma(%x)::is_a_typing_formula() -> TRUE",
		 this) ;
#endif // DEBUG_PRED_TYPE

  return TRUE ;
}

int T_pi::is_a_typing_formula(void)
{
#ifdef DEBUG_PRED_TYPE
  TRACE1("T_pi(%x)::is_a_typing_formula() -> TRUE",
		 this) ;
#endif // DEBUG_PRED_TYPE

  return TRUE ;
}

//
// 4) le reste
//
// Par defaut, non ...
int T_item::is_a_typing_formula(void)
{
#ifdef DEBUG_PRED_TYPE
  TRACE2("T_item(%x:%s)::is_a_typing_formula() -> FALSE",
		 this,
		 get_class_name()) ;
#endif // DEBUG_PRED_TYPE

  return FALSE ;
}

// ... sauf les cas particuliers :
// operation : oui (pour l'heritage dans une implementation depuis la spec)
int T_op::is_a_typing_formula(void)
{
#ifdef DEBUG_PRED_TYPE
  TRACE2("T_op(%x:%s)::is_a_typing_formula() -> TRUE",
		 this,
		 get_op_name()->get_value()) ;
#endif // DEBUG_PRED_TYPE

  return TRUE ;
}

// flag : oui (pour l'heritage)
int T_flag::is_a_typing_formula(void)
{
#ifdef DEBUG_PRED_TYPE
  TRACE2("T_flag(%x:%s)::is_a_typing_formula() -> TRUE",
		 this,
		 get_ref_lexem()->get_lex_ascii()) ;
#endif // DEBUG_PRED_TYPE

  return TRUE ;
}

// valuation : oui
int T_valuation::is_a_typing_formula(void)
{
#ifdef DEBUG_PRED_TYPE
  TRACE1("T_valuation(%x)::is_a_typing_formula() -> TRUE",
		 this) ;
#endif // DEBUG_PRED_TYPE

  return TRUE ;
}

// identificateur : oui  (ex : enumeres)
int T_ident::is_a_typing_formula(void)
{
#ifdef DEBUG_PRED_TYPE
  TRACE1("T_ident(%x)::is_a_typing_formula() -> TRUE",
		 this) ;
#endif // DEBUG_PRED_TYPE

  return TRUE ;
}

// machine : oui  (parametre formels)
int T_machine::is_a_typing_formula(void)
{
#ifdef DEBUG_PRED_TYPE
  TRACE1("T_machine(%x)::is_a_typing_formula() -> TRUE",
		 this) ;
#endif // DEBUG_PRED_TYPE

  return TRUE ;
}

//
//	}{	Fin du type check des predicats
//
