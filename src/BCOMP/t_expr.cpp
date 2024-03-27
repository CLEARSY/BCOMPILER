/******************************* CLEARSY **************************************
* Fichier : $Id: t_expr.cpp,v 2.0 2007-01-11 16:58:59 eprun Exp $
* (C) 2008 CLEARSY
*
* Description :		Compilateur B
*					Fabrication du type des expressions
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
RCS_ID("$Id: t_expr.cpp,v 1.57 2007-01-11 16:58:59 eprun Exp $") ;

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

//GP 25/01/99
//Fonction qui dans la cas general leve un message d'erreur quand
//une expression est du type string
void T_expr::prohibit_strings_use(void)
{
    // Initialisation du gestionnaire de projets, si ce n'est deja fait
    init_project_manager() ;

    // Chargement du projet
    T_project *project = get_project_manager()->get_current() ;

    int isValidationProject = 0;

    if (project != NULL) {
        isValidationProject = project->isValidation() ;
        if (isValidationProject)
            return;
    }
    else
    {
        isValidationProject = (T_project::get_default_project_type() == PROJECT_VALIDATION);
        if(isValidationProject)
        {
            return;
        }
    }

  if(get_B_type() == NULL)
	{
	  //reprise sur erreur
	  TRACE1("REPRISE SUR ERREUR T_expr(%x)::prohibit_strings_use", this) ;
	  return ;
	}

  if(get_B_type()->is_based_on_string() == TRUE)
	{
	  semantic_error(this,
					 CAN_CONTINUE,
					 get_error_msg(E_STRING_TYPE_ONLY_FOR_OP_INPUT_PARAM)) ;
	}
}

// Classe T_expr
// Fonction generique de calcul de type
void T_expr::make_type(void)
{
  // Si le type a deja ete calcule, il n'y a rien a faire
  if (type_inferred == TRUE)
	{
#ifdef FULL_TRACE
	  TRACE4("T_expr(%x:%s)::make_type() deja type, type = %x:%s",
			 this,
			 get_class_name(),
			 get_B_type(),
			 get_B_type()->make_type_name()->get_value()) ;
#endif
	  return ;
	}

  // Sinon, il faut calculer le type !!!
  internal_make_type() ;

  //GP 25/01/99
  //L'utilisation des strings est intedit dans toutes
  //les expressions, sauf pour les T_ident et les T_literals_strings
  //pour les quels la fonction ci-dessous est surchargge
  prohibit_strings_use() ;

  type_inferred = TRUE ;
}

// Classe T_ident : Fonction generique de calcul de type
// Les builtins ont des types
void T_ident::internal_make_type(void)
{
#ifdef FULL_TRACE
  TRACE2("T_ident(%x::%s)::make_type", this, name->get_value()) ;
#endif

  if (ident_type != ITYPE_BUILTIN)
	{
	  // Pas de type
	  // A FAIRE : ce n'est pas vrai car ca pourrait etre un ensemble abstrait
	  // ou un ensemble enumere
	  return ;
	}

  // le type pour les builtins est fait lors de l'initialisation
#ifdef FULL_TRACE
  TRACE1("%s est un builtin, le type vient de l'initialisation",
		 name->get_value()) ;
#endif
}

//GP 25/01/99
void T_ident::prohibit_strings_use(void)
{
  TRACE0("T_ident::prohibit_strings_use") ;

  //Les ident peuvent etres du type strings dans certains cas
  //on ne l'interdit pas maintenant
  //Le controle est fait au moment au l'ident est type

  return ;
}

//
//	}{	Classe T_extensive_set
//

//Fonction auxilliare qui dit si un T_ident est un "index correct"
// ou un maplet dont les indices sont des corrects.
// Correct = LITERAL_INTEGER, LITERAL_ENUMERATED, LITERAL_BOOL
// cur_dim
// est renseigne avec la dimension : 1 pour un entier, 2 et plus pour
// un tableau
static int are_indexes_literal_integers(T_item *cur_item,
												  int &cur_dim)
{
  cur_dim = 0 ;
  int top = TRUE ;
  for (;;)
	{
	  TRACE2("are_indexes_literal_integers(%x:%s)",
			 cur_item,
			 cur_item->get_class_name()) ;
	  switch (cur_item->get_node_type())
		{
		case NODE_IDENT : //GP 16/02/99 AB bug
		  {
			if(((T_ident*)cur_item)->get_def()==NULL)
			  {
				//Aucun chance que se soit un Literal ou un BOOL
				return FALSE ;
			  }
			else
			  {
				//Faut voir def si c'est un LITERAL ou un BOOL
				cur_item=((T_ident*)cur_item)->get_definition() ;
			  }
			break ;
		  }
		case NODE_LITERAL_INTEGER :
		case NODE_LITERAL_ENUMERATED_VALUE : //GP 16/02/99 AB bug
		case NODE_LITERAL_BOOLEAN :  //GP 16/02/99 correction AB bug
		  {
			// Ok
			cur_dim ++ ;
			TRACE2("are_indexes_literal_integers(%x) -> TRUE dim %d",
				   cur_item, cur_dim) ;
			return TRUE ;
		  }

		case NODE_EXPR_WITH_PARENTHESIS :
		  {
			// On saute les ()
			cur_item = ((T_expr_with_parenthesis *)cur_item)->get_expr() ;
			break ;
		  }

		case NODE_UNARY_OP :
		  {
			// On saute les -
			cur_item = ((T_expr_with_parenthesis *)cur_item)->get_expr() ;
		  }

		case NODE_BINARY_OP :
		  {
			T_binary_op *binop = (T_binary_op *)cur_item ;
			T_binary_operator oper = binop->get_operator() ;

			if (oper == BOP_MAPLET)
			  {

				if (top == TRUE)
				  {
					// Racine de l'expression, on ne teste pas le fils
					// droit qui ne contient pas d'index
					top = FALSE ;
				  }
				else
				  {
					// Il faut tester le fils droit, qui doit contenir
					// lui aussi un index ...
					int right_dim ;
					int ok = are_indexes_literal_integers(binop->get_op2(),
														   right_dim) ;

					if (ok == FALSE)
					  {
						// Pas bon
						// Ok
						TRACE1("are_indexes_literal_integers(%x) -> FALSE",
							   cur_item) ;
						return FALSE ;
					  }

					// Il faut corriger cur_dim
					cur_dim += right_dim ;
				  }
				// On continue avec le fils gauche, qui contient la
				// suite de l'expression
				cur_item = binop->get_op1() ;

				// On sort du switch
				break ;
			  }

			; // Else : fall into
		  }

		default :
		  {
			// Pas bon
			// Ok
			TRACE1("are_indexes_literal_integers(%x) -> FALSE", cur_item) ;
			return FALSE ;
		  }
		}

	  // On continue (cas du BOP_MAPLET)
	}
}

// Fonction auxiliaire qui dit rend l'indice entier
// dans T_item a la dimension dim
// Prerequis : are_indexes_literal_integers(cur_item, &ref_dim) == TRUE
// et dim <= ref_dim
// Retourne NULL si le type de l'indice à la dimension dim n'est pas
// bornable, c'est à dire LITERAL_BOOLEAN ou LITERAL_ENUMERATED
static T_literal_integer *get_bound(T_item *item, int dim)
{
  T_item *cur_item = item ;
  int done = FALSE ;
#ifndef NO_CHECKS
  int real_dim ;
  ASSERT(are_indexes_literal_integers(cur_item, real_dim) == TRUE) ;
  ASSERT(dim <= real_dim) ;
#endif // NO_CHECKS

  /* Cas general : P1*P2*P3*..Pn, modelise en :
  //        |->
  //        / \
  //       .  in
  //      .
  //    |->
  //    / \
  //  |->  i3
  //  / \
  // i1 i2
  // On sait maintenant comment parcourir l'item : on va
  // tout en bas, et on remonte
  */
  int negative = FALSE ; // Pour savoir si le nombre est negatif
  while (done == FALSE)
	{
	  TRACE2("cur_item(%x) %s", cur_item, cur_item->get_class_name()) ;
	  switch (cur_item->get_node_type())
		{
		  // GP 16/09/99
		  // AB Bug
		case NODE_LITERAL_BOOLEAN :
		case NODE_LITERAL_ENUMERATED_VALUE :
		case NODE_IDENT :
		  {
		    //Pas de bornes
		    return NULL ;
		  }
		case NODE_LITERAL_INTEGER :
		  {
			// Ok, on est en bas !
			done = TRUE ;
			break ;
		  }

		case NODE_EXPR_WITH_PARENTHESIS :
		  {
			// On saute les ()
			cur_item = ((T_expr_with_parenthesis *)cur_item)->get_expr() ;
			break ;
		  }

		case NODE_UNARY_OP :
		  {
			// Un - unaire
			negative = (negative == TRUE) ? FALSE : TRUE ;
			cur_item = ((T_expr_with_parenthesis *)cur_item)->get_expr() ;
			break ;
		  }

		case NODE_BINARY_OP :
		  {
			T_binary_operator oper = ((T_binary_op *)cur_item)->get_operator() ;

			if (oper == BOP_MAPLET)
			  {
				// On continue avec le fils gauche, qui contient la
				// suite de l'expression
				cur_item = ((T_binary_op *)cur_item)->get_op1() ;

				// On sort du switch
				break ;
			  }

			; // Else : fall into
		  }

		default :
		  {
			// Pas bon
			assert(FALSE) ; // Protege par la precondition
		  }
		}
	}

  // Ici on sait que cur_item pointe sur le T_literal_integer du bas,
  // numerote 1

  int cur_dim = 1 ;

  // Cas particulier : index 1
  if (dim == 1)
	{
	  ASSERT(cur_item->get_node_type() == NODE_LITERAL_INTEGER) ;
	  return ((T_literal_integer *)cur_item) ;
	}

  // Ici, dans le cas general, on reference les T_binary_op
  while (cur_dim < dim)
	{
	  cur_item = cur_item->get_father() ;
	  cur_dim ++ ;
	}


  // Ici on est ou l'on veut, et la valeur est dans le fils droit
  if (cur_item->get_node_type() == NODE_BINARY_OP)
	{
	  cur_item = ((T_binary_op *)cur_item)->get_op2() ;
	  TRACE5("cur_item %s:%d:%d %x %s",
			 cur_item->get_ref_lexem()->get_file_name()->get_value(),
			 cur_item->get_ref_lexem()->get_file_line(),
			 cur_item->get_ref_lexem()->get_file_column(),
			 cur_item,
			 cur_item->get_class_name()) ;

	}

  //GP 16/02/99
  //AB bug
  if((cur_item->get_node_type() == NODE_LITERAL_ENUMERATED_VALUE) ||
	 (cur_item->get_node_type() == NODE_LITERAL_BOOLEAN) ||
	 (cur_item->get_node_type() == NODE_IDENT))
	{
	  //C'est un booleen ou un enumere donc
	  //il n'y a pas de borne
	  return NULL ;
	}

  ASSERT(cur_item->get_node_type() == NODE_LITERAL_INTEGER) ;
  return ((T_literal_integer *)cur_item) ;
}

// Fonction auxiliaire qui rend le ne type d'un Produit cartesien
// T1*T2...Tn
static T_predefined_type *get_type(T_type *type, int dim)
{
  TRACE3("get_type(%x:%s) dim %d", type, type->make_type_name()->get_value(), dim) ;
  // Cas particulier : dim = 1, predefined type
  if (type->get_node_type() == NODE_PREDEFINED_TYPE)
	{
	  ASSERT(dim == 1) ;
	  return (T_predefined_type *)type ;
	}

  /* Cas general : P1*P2*P3*..Pn, modelise en :
  //         *
  //        / \
  //       .  Pn
  //      .
  //     *
  //    / \
  //   *  P3
  //  / \
  // P1 P2
  // On va tout en bas, et on remonte
  */
  int done = FALSE ;
  T_item *cur_item = type ;
  T_item *cur_father = type->get_father() ;
  while (done == FALSE)
	{
	  TRACE2("DOWN cur_item(%x) %s", cur_item, cur_item->get_class_name()) ;
	  // Au passage, on corrige les peres qui sont mauvais
	  // (ils pointent sur le T_extensive_set)
	  TRACE4("fix cur_item %x:%s father %x:%s",
			 cur_item,
			 cur_item->get_class_name(),
			 cur_father,
			 cur_father->get_class_name()) ;
	  cur_item->set_father(cur_father) ;
	  cur_father = cur_item ;
	  switch (cur_item->get_node_type())
		{
		case NODE_PREDEFINED_TYPE :
		  {
			// Ok, on est en bas !
			done = TRUE ;
			break ;
		  }

		case NODE_PRODUCT_TYPE :
		  {
			cur_item = ((T_product_type *)cur_item)->get_type1() ;
			break ;
		  }

		default :
		  {
			// Pas bon, mais impossible selon l'inference de type
			assert(FALSE) ;
		  }
		}
	}

  // Ici on sait que cur_item pointe sur le T_predefined_type du bas,
  // numerote 0. On remonte a son pere pour commencer
  int cur_dim = 1 ;

  // Cas particulier : index 1
  if (dim == 1)
	{
	  ASSERT(cur_item->get_node_type() == NODE_PREDEFINED_TYPE) ;
	  return ((T_predefined_type *)cur_item) ;
	}

  // Cas general : index >= 2
  while (cur_dim < dim)
	{
	  TRACE4("UPcur_item(%x) %s dim %d/%d",
			 cur_item,
			 cur_item->get_class_name(),
			 cur_dim,
			 dim) ;
	  cur_item = cur_item->get_father() ;
	  cur_dim ++ ;
	}

  // Ici on est ou l'on veut, et la valeur est dans le fils droit
  TRACE2("cur_item(%x) %s", cur_item, cur_item->get_class_name()) ;
  ASSERT(cur_item->get_node_type() == NODE_PRODUCT_TYPE) ;
  cur_item = ((T_product_type *)cur_item)->get_type2() ;
  ASSERT(cur_item->get_node_type() == NODE_PREDEFINED_TYPE) ;
  return ((T_predefined_type *)cur_item) ;
}


// Fonction generique de calcul de type
// Substitution du type A := B
// CTRL Ref : VTYPE 2-3 (4)
void T_extensive_set::internal_make_type(void)
{
  TRACE1("DEBUT T_extensive_set(%x)::make_type", this) ;

  T_expr *cur_item = first_item ;
  T_type *ref_type = NULL ; //NULL = type de base pas encore determine

  //GP Correction apportee, pour que le type de {[], [1,2,3]}
  //ne soit plus du type Setof([]), mais du meme type que {[1,2,3], []}

  //Premier parcours des elements de l'ensemble, pour déterminer
  //le type de base de l'ensemble
  while (cur_item != NULL)
	{
	  TRACE3("--- AVANT calcul type %x:%s (type = %x) ---",
			 cur_item,
			 cur_item->get_class_name(),
			 make_type_name(cur_item)) ;
	  ENTER_TRACE ;

	  cur_item->make_type() ;

	  EXIT_TRACE ;
	  TRACE3("--- APRES calcul type %x:%s (type = %x) ---",
			 cur_item,
			 cur_item->get_class_name(),
			 make_type_name(cur_item)) ;

	  T_type* cur_type = cur_item->get_B_type() ;

	  if (cur_type == NULL)
		{
		  //Un élément n'a pas de type
		  semantic_error(cur_item,
						 CAN_CONTINUE,
						 get_error_msg(E_UNCOMPATIBLE_EXTENSIVE_ITEM_TYPE),
						 make_type_name(cur_item)->get_value(),
						 (ref_type == NULL)
						 ? get_catalog(C_NO_TYPE_GIVEN)
						 : ref_type->make_type_name()->get_value()) ;
		  return ;
		}

	  if (ref_type == NULL)
		{
		  // On n'a pas encore determine le type de base de l'ensemble
		  if (cur_type->has_wildcards() == FALSE)
			{
			  // Le type n'est pas basé sur un type généric
			  // donc il peut servir pour typer l'ensemble
			  TRACE0("Le type n'est pas basé sur un type généric") ;
			  ref_type = cur_item->get_B_type() ;
			  TRACE1("Type affecte à ref_type:%x :",ref_type);
			}
		  else
			{
			  TRACE0("Le type est basé sur un type généric") ;
			}
		}

	  cur_item = (T_expr *)cur_item->get_next() ;
	}

  //Tous les élément sont basé sur des types génériques
  //donc on utilise le type du premier élément pour typé l'ensemble
  int all_generic = FALSE ;
  if (ref_type == NULL)
	{
	  TRACE0("Tous les éléments sont basé sur des types génériques") ;
	  ref_type = first_item->get_B_type()  ;
	  all_generic = TRUE ;
	}


  // Si tous les elements sont des entiers litteraux, on calcule les bornes
  int only_lit_int_indexes = TRUE ;
  int dim ; // Dimension des elements

  if (all_generic == FALSE)
	{
	  // Deuxieme parcours : verification de coherence des types, et
	  // calcul eventuel des bornes

	  //Initialisation pour le deuxieme parcours
	  cur_item = first_item ;


	  while (cur_item != NULL)
		{
		  TRACE3("cur_item(%x:%s) only_int %s lower_bound %x upper_bound %x",
				 cur_item,
				 cur_item->get_class_name(),
				 (only_lit_int_indexes == TRUE) ? "TRUE" : "FALSE") ;

		  if (only_lit_int_indexes == TRUE)
			{
			  // On regarde si l'element courant en est aussi un
			  only_lit_int_indexes = are_indexes_literal_integers(cur_item, dim) ;
			}

		  // GP Correstion BUG 1946: on utilise is_compatible_with(ref_type, TRUE)
		  // avec TRUE pour dire que l'on autorise deux jokers
		  TRACE4("verif compatibilite %x:%s avec %x:%s",
				 cur_item->get_B_type(),
				 make_type_name(cur_item)->get_value(),
				 ref_type,
				 make_type_name(ref_type)->get_value()) ;
		  if (    (cur_item->get_B_type() == NULL)
			   || ( (cur_item->get_B_type() != ref_type)
					&& (cur_item->get_B_type()->is_compatible_with(ref_type, TRUE)
				   == FALSE)) )
			{
			  // On verifie que le type est compatible avec celui des autres
			  // elements
			  TRACE2("cur_item(%x)->get_B_type() == %x",
					 cur_item,cur_item->get_B_type()) ;

			  semantic_error(cur_item,
							 CAN_CONTINUE,
							 get_error_msg(E_UNCOMPATIBLE_EXTENSIVE_ITEM_TYPE),
							 make_type_name(cur_item)->get_value(),
							 (ref_type == NULL)
							 ? get_catalog(C_NO_TYPE_GIVEN)
							 : ref_type->make_type_name()->get_value()) ;
			  ref_type = NULL ;
			}

		  cur_item = (T_expr *)cur_item->get_next() ;
		}
	}
  else // !(all_generic == FALSE)
	{
	  only_lit_int_indexes = FALSE ;
	}

  // Le type resultat est : setof(ref_type)
  if (ref_type == NULL)
	{
	  // Reprise sur erreur
	  TRACE0("ref_type == NULL : on abdandonne") ;
	  return ;
	}

  ASSERT(ref_type) ;
  TRACE0("Fabrication du setof_type") ;
  T_type *base_type = ref_type->clone_type(this, get_betree(), get_ref_lexem()) ;

  if (only_lit_int_indexes == TRUE)
	{
	  // On peut positionner les bornes !
	  TRACE1("on positionne les bornes pour les %d dimensions index", dim) ;
	  int cur_dim = 0 ;

	  while (++cur_dim <= dim)
		{
		  TRACE2(">> Calcul bornes dimension %d/%d", cur_dim, dim) ;
		  ENTER_TRACE ;

		  cur_item = first_item ;
		  T_literal_integer *lower_bound = NULL ;
		  T_literal_integer *upper_bound = NULL ;

		  int is_bounded = TRUE ; //GP 16/02/99 TRUE si la dimension est bornee
		  while (cur_item != NULL)
			{
			  // Recuperation des bornes de l'item courant
			  T_literal_integer *cur_bound = get_bound(cur_item, cur_dim) ;

			  //cur_bound == NULL quand pas de bornes
			  //Par exemple pour les BOOL ou ENUMERATED
			  if (cur_bound != NULL) //GP 16/02/99 AB bug
			    {
				  // Positionnement eventuel des bornes
			      if (cur_bound->is_less_than(lower_bound) == TRUE)
					{
					  lower_bound = cur_bound ;
					  TRACE1("lower_bound = %s",
							 lower_bound->get_value()->get_value()->get_value()) ;
					}
			      // on ne peut pas mettre de else pour ce test car dans
			      // le cas d'une liste a un element, les deux tests sont
			      // vrais
			      if (cur_bound->is_greater_than(upper_bound) == TRUE)
					{
					  upper_bound = cur_bound ;
					  TRACE1("upper_bound = %s",
							 upper_bound->get_value()->get_value()->get_value()) ;
					}
			    }
			  else
				{
				  //La dimension ne peut pas etre bornee
				   is_bounded = FALSE ;
				}

			  cur_item = (T_expr *)cur_item->get_next() ;
			}

		  if(is_bounded == TRUE)
			{
			  // Recuperation du type
			  // Cast justifie par construction
			  T_base_type *bounded_type = get_type((T_product_type *)base_type,
												   cur_dim) ;

			  // Positionnement des bornes
			  bounded_type->set_bounds(lower_bound, upper_bound) ;

			  EXIT_TRACE ;
			  TRACE4("<< Calcul bornes dimension %d/%d -> %s .. %s",
					 cur_dim,
					 dim,
					 lower_bound->get_value()->get_value()->get_value(),
					 upper_bound->get_value()->get_value()->get_value()) ;
			}
		  else
			{
			  TRACE0("PAS DE BORNE") ;
			}

		}
	}

  T_setof_type *setof = new T_setof_type(base_type,
										 this,
										 get_betree(),
										 first_item->get_ref_lexem()) ;
  base_type->unlink() ;


  set_B_type(setof, get_ref_lexem()) ;
  setof->unlink() ;
  TRACE2("FIN T_extensive_set(%x)::make_type -> %s",
		 this,
		 setof->make_type_name()->get_value());

}


//
//	}{	Classe T_comprehensive_set
//

// Fonction generique de calcul de type
// Substitution du type A := B
// CTRL Ref : VTYPE 2-3 (2)
void T_comprehensive_set::internal_make_type(void)
{
  TRACE1("DEBUT T_comprehensive_set(%x)::make_type", this) ;

  // On verifie que tous les identificateurs sont distincts
  check_no_double_definition(first_ident) ;

  // Type check du predicat
  predicate->type_check() ;

  // On verifie que tous les identificateurs sont types
  size_t nb_idents = 0 ;
  int ok = TRUE ;
  T_ident *cur = first_ident ;
  while (cur != NULL)
	{
	  nb_idents ++ ;
	  if (cur->get_B_type() == NULL)
		{
		  semantic_error(cur,
						 CAN_CONTINUE,
						 get_error_msg(E_NO_TYPE_FOR_IDENT),
						 cur->get_ident_type_name(),
						 cur->get_name()->get_value()) ;
		  ok = FALSE ;
		}
	  cur = (T_ident *)cur->get_next() ;
	}

  if (ok == FALSE)
	{
	  // On ne peut pas continuer
	  return ;
	}

  // On construit un T_setof_type(X)
  // avec X = type du premier element s'il n'y en a qu'un
  // X = t1 * t2 * .. tn ou les ti sont les types des elements
  // A FAIRE
  T_type *new_base_type = NULL ;

  if (nb_idents == 1)
	{
	  new_base_type = first_ident->get_B_type()->clone_type(NULL,
															get_betree(),
															get_ref_lexem()) ;
	}
  else
	{
	  T_type *ltype = NULL ;
	  cur = first_ident ;
	  while (cur != NULL)
		{
		  T_type *res = cur->get_B_type()->clone_type(NULL,
													  get_betree(),
													  get_ref_lexem()) ;

		  if (ltype == NULL)
			{
			  ltype = res ;
			}
		  else
			{
			  T_type *save_ltype = ltype ;
			  ltype = new T_product_type(ltype,
										 res,
										 NULL,
										 get_betree(),
										 get_ref_lexem()) ;
			  save_ltype->unlink() ;
			  res->unlink() ;
			}
		  cur = (T_ident *)cur->get_next() ;
		}

	  new_base_type = ltype ;
	}

  T_setof_type *setof = new T_setof_type(new_base_type,
										 NULL,
										 get_betree(),
										 get_ref_lexem()) ;
  new_base_type->unlink() ;

  set_B_type(setof, get_ref_lexem()) ;
  setof->unlink() ;
  TRACE1("FIN T_comprehensive_set(%x)::make_type", this) ;
}


//
//	}{	Classe T_converse
//

// Fonction generique de calcul de type
// Expression du type E~, ou E est une relation
// CTRL Ref : VTYPE 2-4 (4)
void T_converse::internal_make_type(void)
{
  TRACE1("DEBUT T_converse(%x)::make_type", this) ;

  expr->make_type() ;

  // L'operande doit etre une relation
  if (expr->is_a_relation() == FALSE)
	{
	  semantic_error(expr,
					 CAN_CONTINUE,
					 get_error_msg(E_OPERAND_OF_CONVERSE_IS_NOT_A_RELATION),
					 make_type_name(expr)->get_value()) ;
	  return ;
	}

  // type = Setof(SRC * DST)
  // Il faut fabriquer : Setof(DST * SRC)
  T_setof_type *setof = (T_setof_type *)(expr->get_B_type()) ;
  ASSERT(setof->is_a_set() == TRUE) ;

  T_product_type *prod = (T_product_type *)(setof->get_base_type());

  if (prod->get_node_type() == NODE_GENERIC_TYPE)
	{
	  // Cas particulier {}~
	  T_type *res = prod->clone_type(NULL, get_betree(), get_ref_lexem()) ;
	  set_B_type(res, get_ref_lexem()) ;
	  res->unlink() ;
	  return ;
	}

  ASSERT(prod->get_node_type() == NODE_PRODUCT_TYPE) ;

  TRACE1(">> clonage t1 %x", prod->get_type1()) ;
  ENTER_TRACE ; ENTER_TRACE ;
  T_type *c1 = prod->get_type1()->clone_type(NULL, get_betree(), get_ref_lexem()) ;
  EXIT_TRACE ; EXIT_TRACE ;
  TRACE2("<< clonage t1 %x -> c1 %x", prod->get_type1(), c1) ;

  TRACE1(">> clonage t2", prod->get_type2()) ;
  ENTER_TRACE ; ENTER_TRACE ;
  T_type *c2 = prod->get_type2()->clone_type(NULL, get_betree(), get_ref_lexem()) ;
  EXIT_TRACE ; EXIT_TRACE ;
  TRACE2("<< clonage t2 %x -> c2 %x", prod->get_type2(), c2) ;

  T_type *nprod = new T_product_type(c2, c1, NULL, get_betree(), get_ref_lexem()) ;
  c1->unlink() ;
  c2->unlink() ;
  T_type *res = new T_setof_type(nprod, this, get_betree(), get_ref_lexem()) ;
  nprod->unlink() ;
  set_B_type(res, get_ref_lexem()) ;
  res->unlink() ;
  TRACE1("FIN T_converse(%x)::make_type", this) ;
}


//
//	}{	Classe T_relation
//

// Fonction generique de calcul de type pour une relation
// CTRL Ref: VTYPE 2-5 (1)
void T_relation::internal_make_type(void)
{
  TRACE1("T_relation(%x)::make_type", this) ;

  src_set->make_type() ;
  dst_set->make_type() ;

  int error = FALSE ;

  if (src_set->is_a_set() == FALSE)
	{
	  semantic_error(src_set,
					 CAN_CONTINUE,
					 get_error_msg(E_OPERAND_SHOULD_BE_A_SET),
					 make_type_name(src_set)->get_value()) ;
	  error = TRUE ;
	}

  if (dst_set->is_a_set() == FALSE)
	{
	  semantic_error(dst_set,
					 CAN_CONTINUE,
					 get_error_msg(E_OPERAND_SHOULD_BE_A_SET),
					 make_type_name(dst_set)->get_value()) ;
	  error = TRUE ;
	}

  if (error == TRUE)
	{
	  // On ne peut pas continuer
	  return ;
	}

  // On fabrique le resultat setof(setof(base_src * base_dst))
  // Casts justifies par le test ci-dessus
  T_type *src_base = ((T_setof_type *)src_set->get_B_type())->get_base_type() ;
  T_type *dst_base = ((T_setof_type *)dst_set->get_B_type())->get_base_type() ;
  T_type *clone1 = src_base->clone_type(NULL, get_betree(), get_ref_lexem()) ;
  T_type *clone2 = dst_base->clone_type(NULL, get_betree(), get_ref_lexem()) ;

  // Vérrue: on acroche le typing ident éventuel des père des 'base
  // type' au clone des 'base_type'. Sinon le typing_ident est perdu
  // ce qui pose problème pour le traducteur HIA.
  clone1->set_typing_ident(src_set->get_typing_ident());
  clone2->set_typing_ident(dst_set->get_typing_ident());
  T_product_type *prod = new T_product_type(clone1,
											clone2,
											NULL,
											get_betree(),
											get_ref_lexem()) ;

  clone1->unlink() ;
  clone2->unlink() ;
  T_setof_type *set ;
  T_setof_type *res ;

  set = new T_setof_type(prod, NULL, get_betree(), get_ref_lexem()) ;
  prod->unlink() ;
  res = new T_setof_type(set, NULL, get_betree(), get_ref_lexem()) ;
  set->unlink() ;

  // On affecte le type
  set_B_type(res, get_ref_lexem()) ;
  res->unlink() ;
}

//
//	}{ Classe T_expr_with_parenthesis
//
void T_expr_with_parenthesis::internal_make_type(void)
{
  TRACE1("T_expr_with_parenthesis(%x)::make_type", this) ;
  // CTRL Ref: VTYPE 2-6 (11)
  ENTER_TRACE ;

  expr->make_type() ;

  //GP 07/01/99
  //Pour lever une erreur si expr est un ident non type
  expr->check_type() ;

  if (expr->get_B_type() == NULL)
	{
	  // Reprise sur erreur
	  TRACE2("Panic : expr = %x %s->B_type()==NULL", expr, expr->get_class_name()) ;
	  TRACE3("expr : %s:%d:%d",
			 expr->get_ref_lexem()->get_file_name()->get_value(),
			 expr->get_ref_lexem()->get_file_line(),
			 expr->get_ref_lexem()->get_file_column()) ;
	  return ;
	}

  T_type *type = expr->get_B_type()->clone_type(this,
												get_betree(),
												get_ref_lexem()) ;

  set_B_type(type, get_ref_lexem()) ;
  type->unlink() ;

  EXIT_TRACE ;
}

//
//	}{ Classe T_array_item
//
void T_array_item::internal_make_type(void)
{
  TRACE1("T_array_item(%x)::make_type", this) ;
  // CTRL Ref: VTYPE 2-5 (6)
  ENTER_TRACE ;

  // Calcul du type du tableau
  TRACE0("calcul du type du tableau") ;
  array_name->make_type() ;

  // Calcul du type des indexes: on fabrique le produit cartesien au fur et
  // a mesure
  TRACE0("calcul du type des indexes") ;
  T_expr *cur_index = first_index ;
  T_type *index_type = NULL ;

  TRACE1("cur_index = %x", cur_index) ;
  int error = FALSE ;
  while (cur_index != NULL)
	{
      if (cur_index->is_an_expr() == TRUE)
      {
          cur_index->make_type() ;

          T_type *cur_type = cur_index->get_B_type();
          if(cur_type)
          {
              cur_type = cur_type->clone_type(this,get_betree(),get_ref_lexem()) ;

              if (index_type != NULL)
              {
                  index_type = new T_product_type(index_type,
                                                  cur_type,
                                                  NULL,
                                                  get_betree(),
                                                  get_ref_lexem()) ;

              }
              else
              {
                  index_type = cur_type ;
              }
          }
      }
      else
      {
          semantic_error(cur_index,
                         CAN_CONTINUE,
                         get_error_msg(E_NOT_AN_EXPRESSION),
                         make_class_name(cur_index)) ;

          error = TRUE ;
      }
      cur_index = (T_expr *)cur_index->get_next() ;
  }

  if ( (array_name->is_an_expr() == FALSE)
      || (array_name->get_B_type() == NULL)
      || (index_type == NULL)
	   || (error == TRUE) )
	{
	  // Reprise sur erreur
	  TRACE0("reprise sur erreur") ;
	  return ;
	}

  if ( (array_name->is_a_set() == FALSE) || (array_name->is_a_relation() == FALSE) )
	{
	  type_check_error() ;
	  TRACE0("reprise sur erreur") ;
	  return ;
	}

  // Casts valide d'apres les tests
  T_setof_type *array_type = (T_setof_type *)array_name->get_B_type() ;
  T_product_type *prod = (T_product_type *)array_type->get_base_type() ;

  // Il faut que le type src du tableau soit compatible avec le type
  // des indexes
  if (index_type->is_compatible_with(prod->get_type1()) == FALSE)
	{
	  semantic_error(this,
					 CAN_CONTINUE,
					 get_error_msg(E_BAD_INDEXES_FOR_ARRAY),
					 index_type->make_type_name()->get_value(),
					 prod->get_type1()->make_type_name()->get_value()) ;
	  return ;
	}

  // Ok, c'est bon !
  // Le resultat est du type prod->dst
  T_type *res = prod->get_type2()->clone_type(this, get_betree(), get_ref_lexem()) ;
  set_B_type(res, get_ref_lexem()) ;
  res->unlink() ;

  TRACE2("le type du T_array_item est %x <%s>",
		 res,
		 res->make_type_name()->get_value()) ;
}


// Fonction auxilaire de production du message d'erreur
void T_array_item::type_check_error(void)
{
  // Si c'est un identificateur, on a un meilleur message
  if (array_name->is_an_ident() == TRUE)
	{
	  T_ident *array = array_name->make_ident() ;
	  semantic_error(this,
					 CAN_CONTINUE,
					 get_error_msg(E_IDENT_IS_NOT_AN_ARRAY),
					 array->get_name()->get_value(),
					 array_name->get_B_type()->make_type_name()->get_value()) ;
	  semantic_error_details(array->get_def(),
							 get_error_msg(E_LOCATION_OF_IDENT_TYPE),
							 array_name->get_B_type()->make_type_name()->get_value(),
							 array->get_ident_type_name(),
							 array->get_name()->get_value()) ;

	}
  else
	{
	  semantic_error(this,
					 CAN_CONTINUE,
					 get_error_msg(E_ITEM_IS_NOT_AN_ARRAY),
					 array_name->get_B_type()->make_type_name()->get_value()) ;
	}
}

//
//	}{ Classe T_image
//
// CTRL Ref: VTYPE 2-4(3)
void T_image::internal_make_type(void)
{
  TRACE1("T_image(%x)::make_type", this) ;
  TRACE4("%s:%d:%d T_image(%x)::make_type",
		 get_ref_lexem()->get_file_name()->get_value(),
		 get_ref_lexem()->get_file_line(),
		 get_ref_lexem()->get_file_column(),
		 this) ;

  // R[E]
  // (1) R doit etre de type Setof(t * v)
  // (2) E doit etre de type Setof(t')
  // (3) t et t' doivent etres compatible (GP 15/05 correction BUG 2162)
  // (4) Le resultat est de type Setof(v)

#ifndef NO_CHECKS
  if (relation == NULL)
	{
	  TRACE1("INTERNAL_ERROR : this = %x, relation == NULL", this) ;
	  semantic_error(get_ref_lexem(),
					 CAN_CONTINUE,
					 "INTERNAL ERROR at %s:%d : relation = NULL",
					 __FILE__, __LINE__) ;
	  return ;
	}
#endif
  relation->make_type() ;
  expr->make_type() ;
  //GP 07/01/99
  relation->check_type() ;
  expr->check_type() ;

  TRACE2("relation %s, expr %s",
		 make_type_name(relation)->get_value(),
		 make_type_name(expr)->get_value()) ;

  if ( (relation->get_B_type() == NULL) || (expr->get_B_type() == NULL) )
	{
	  // Reprise sur erreur
	  TRACE0("on sort ...") ;
	  return ;
	}

  // (1) R doit etre de type Setof(t * v)
  if (relation->is_a_relation() == FALSE)
	{
	  semantic_error(relation,
					 CAN_CONTINUE,
					 get_error_msg(E_FIRST_OPERAND_OF_IMAGE_SHOULD_BE_A_RELATION),
					 make_type_name(relation)->get_value()) ;
	  return ;
	}

  // Cast justifie par le test ci-dessus et le test ci-apres
  T_setof_type *rtype = (T_setof_type *)relation->get_B_type() ;
  T_type *tp = rtype->get_relation_src_type() ;

  // (2) E doit etre de type Setof(t')
  if (expr->is_a_set() == FALSE)
	{
	  // On fabrique le type Setof(tp) pour le message
	  T_setof_type *msg = new T_setof_type(tp, this, get_betree(), get_ref_lexem());
	  semantic_error(expr,
					 CAN_CONTINUE,
					 get_error_msg(E_SECOND_OPERAND_OF_IMAGE_SHOULD_BE_A_SUBSET_OF),
					 make_type_name(msg)->get_value(),
					 make_type_name(expr)->get_value()) ;
	  return ;
	}

  // (3) t et t' doivent etres compatible (GP 15/05 correction BUG 2162)

  // Casts justifies car expr est un setof (test ci-dessus)
  // et relation est une relation
  T_type * t = ((T_setof_type*)expr->get_B_type())->get_base_type() ;
  if (t->is_compatible_with(tp) == FALSE)
	{
	  // On fabrique les types de base Setof(...) pour le message
	  T_setof_type *given = new T_setof_type(t,
											 this,
											 get_betree(),
											 get_ref_lexem());
	  T_setof_type *expected = new T_setof_type(tp,
												this,
												get_betree(),
												get_ref_lexem());
	  semantic_error(expr,
					 CAN_CONTINUE,
					 get_error_msg(E_INVALID_IMAGE_INDEX_TYPE),
					 make_type_name(given)->get_value(),
					 make_type_name(expected)->get_value()) ;
	  return ;
	}


  // Cast justifie car relation est une relation
  T_type *dst = rtype->get_relation_dst_type() ;

  // (4) Le resultat est de type Setof(v)
  T_type *clone = dst->clone_type(NULL, get_betree(), get_ref_lexem()) ;
  T_type *res = new T_setof_type(clone, this, get_betree(), get_ref_lexem()) ;
  clone->unlink() ;
  set_B_type(res, get_ref_lexem()) ;
  res->unlink() ;
}

//
//	}{ Classe T_extensive_seq
//
void T_extensive_seq::internal_make_type(void)
{
  TRACE1("T_extensive_seq(%x)::make_type", this) ;

  T_item *cur = first_item ;
  T_type *first_type = NULL ;
  int error = FALSE ;
  while (cur != NULL)
	{
	  cur->make_type() ;

	  if (cur->is_an_expr() == TRUE)
		{
		  T_type *res = cur->get_B_type() ;

		  //GP 07/01/99
		  cur->check_type() ;

		  if (res == NULL)
			{
			  // Reprise sur erreur
			  TRACE0("res = NULL , on sort") ;
			  return ;
			}

		  if (first_type == NULL)
			{
			  first_type = res ;
			}
		  else
			{
			  if (res->is_equal_to(first_type) == FALSE)
				{
                  semantic_error(cur,
								 CAN_CONTINUE,
								 get_error_msg(E_INCOMPATIBLE_TYPES_IN_EXPR),
								 res->make_type_name()->get_value(),
								 first_type->make_type_name()->get_value()) ;
				  error = TRUE ;
				}
			  // Si le premier type est [] et que le cur n'est pas []
			  // alors on precise le first_type
			  if ( (first_type->get_node_type() == NODE_GENERIC_TYPE)
				   && (res->get_node_type() != NODE_GENERIC_TYPE) )
				{
				  TRACE2("maj first_type qui passe de %s a %s",
						 first_type->make_type_name()->get_value(),
						 res->make_type_name()->get_value()) ;
				  first_type = res ;
				}
			}
		}
	  else
		{
		  semantic_error(cur,
						 CAN_CONTINUE,
						 get_error_msg(E_NOT_AN_EXPRESSION),
						 make_type_name(cur)->get_value()) ;
		  error = TRUE ;
		}
	  cur = cur->get_next() ;
	}

  if (error == TRUE)
	{
	  return ;
	}

  // On construit un Setof(btype(INTEGER, ?, ?) * first)
  T_type *type_int = new T_predefined_type(BTYPE_INTEGER,
										   NULL,
										   NULL,
										   NULL,
										   get_betree(),
										   get_ref_lexem()) ;
  T_type *clone_first = first_type->clone_type(this,
											   get_betree(),
											   get_ref_lexem()) ;
  T_product_type *prod = new T_product_type(type_int,
											clone_first,
											NULL,
											get_betree(),
											get_ref_lexem()) ;
  type_int->unlink( );
  clone_first->unlink() ;

  T_type *res = new T_setof_type(prod,
								 this,
								 get_betree(),
								 get_ref_lexem()) ;
  prod->unlink() ;

  set_B_type(res, get_ref_lexem()) ;
  res->unlink() ;
}

//
//	}{ Classe T_empty_seq
//
void T_empty_seq::internal_make_type(void)
{
  TRACE1("T_empty_seq(%x)::make_type", this) ;

  // Le type de l'ensemble vide est Setof(btype(INT)*joker)
  T_type *Z = new T_predefined_type(BTYPE_INTEGER,
									NULL,
									NULL,
									this,
									get_betree(),
									get_ref_lexem()) ;
  T_type *joker = new T_generic_type(this, get_betree(), get_ref_lexem()) ;

  T_type *product = new T_product_type(Z,
									   joker,
									   this,
									   get_betree(),
									   get_ref_lexem()) ;

  T_setof_type *type = new T_setof_type(product,
										this,
										get_betree(),
										get_ref_lexem()) ;

  set_B_type(type, get_ref_lexem()) ;
  type->unlink() ;
}

//
//	}{ Classe T_empty_set
//
void T_empty_set::internal_make_type(void)
{
  TRACE1("T_empty_set(%x)::make_type", this) ;

  // Le type de l'ensemble vide est Setof(joker)
  T_type *joker = new T_generic_type(this, get_betree(), get_ref_lexem()) ;

  T_setof_type *type = new T_setof_type(joker,
										this,
										get_betree(),
										get_ref_lexem()) ;

  set_B_type(type, get_ref_lexem()) ;
  type->unlink() ;
}

// CTRL Ref: VTYPE 2-1 (3)
void T_unary_op::internal_make_type(void)
{
    TRACE1("T_unary_op(%x)::make_type", this) ;

    operand->make_type() ;

    //GP 07/01/99
    operand->check_type() ;

    if (operand->get_B_type() == NULL)
    {
        // Reprise sur erreur
        TRACE0("panic, operand->get_B_type() = NULL") ;
        return ;
    }

    if (operand->get_B_type()->is_an_integer() == FALSE)
    {
        if (operand->get_B_type()->is_a_real() == FALSE)
        {
            semantic_error(operand,
                           CAN_CONTINUE,
                           get_error_msg(E_OPERAND_SHOULD_BE_AN_INTEGER),
                           make_type_name(operand)->get_value()) ;
            return ;
        }
        else {
            // Le type resultant est : réel
            T_type *type = new T_predefined_type(BTYPE_REAL,
                                                 NULL,
                                                 NULL,
                                                 this,
                                                 get_betree(),
                                                 get_ref_lexem()) ;

            set_B_type(type, get_ref_lexem()) ;
            type->unlink() ;
        }
    }
    else {
        // Le type resultant est : entier
        T_type *type = new T_predefined_type(BTYPE_INTEGER,
                                             NULL,
                                             NULL,
                                             this,
                                             get_betree(),
                                             get_ref_lexem()) ;

        set_B_type(type, get_ref_lexem()) ;
        type->unlink() ;
    }
}

// Typage des entiers litteraux
void T_literal_integer::internal_make_type(void)
{
TRACE1("T_literal_integer(%x)::make_type", this) ;

T_type *type = new T_predefined_type(BTYPE_INTEGER,
									 NULL,
									 NULL,
									 this,
									 get_betree(),
									 get_ref_lexem()) ;

set_B_type(type, get_ref_lexem()) ;
type->unlink() ;
}

//GP 25/01/99
//Fonction qui dans le cas general leve une erreur
//quand une expression est du type string
//Sauf dans ce cas particulier
void T_literal_string::prohibit_strings_use(void)
{
  //On ne peut pas interdire a une chaine de caractere
  //d'etre du type string

  return ;
}

// Typage des entiers litteraux
void T_literal_string::internal_make_type(void)
{
TRACE1("T_literal_string(%x)::make_type", this) ;

T_type *type = new T_predefined_type(BTYPE_STRING,
									 NULL,
									 NULL,
									 this,
									 get_betree(),
									 get_ref_lexem()) ;

set_B_type(type, get_ref_lexem()) ;
type->unlink() ;
}

// Typage des reels litteraux
void T_literal_real::internal_make_type(void)
{
TRACE1("T_literal_real(%x)::make_type", this) ;

T_type *type = new T_predefined_type(BTYPE_REAL,
                                                                         NULL,
                                                                         NULL,
                                                                         this,
                                                                         get_betree(),
                                                                         get_ref_lexem()) ;

set_B_type(type, get_ref_lexem()) ;
type->unlink() ;
}

// Typage des entiers litteraux
void T_literal_boolean::internal_make_type(void)
{
TRACE1("T_literal_boolean(%x)::make_type", this) ;

T_type *type = new T_predefined_type(BTYPE_BOOL,
									 NULL,
									 NULL,
									 this,
									 get_betree(),
									 get_ref_lexem()) ;

set_B_type(type, get_ref_lexem()) ;
type->unlink() ;
}

// Type check des lambda_expressions
// CTRL Ref: VTYPE 2-5 (2)
void T_lambda_expr::internal_make_type(void)
{
  TRACE1("T_lambda_expr(%x)::make_type", this) ;

  // On a %x1 .. xn.(P | E)
  // P doit typer x1..xn, dont les types sont alors alpha1 ... alphan
  // E est de type beta
  // Le type resultant est : Setof(alpha1 * ... alphan * beta)

  predicate->type_check() ;
  expr->make_type() ;

  // Il faut verifier que toutes les variables ont ete typees
  int error = FALSE ;
  T_ident *cur_var = first_variable ;
  while (cur_var != NULL)
	{
	  if (cur_var->get_B_type() == NULL)
		{
		  error = TRUE ;
		  semantic_error(cur_var,
						 CAN_CONTINUE,
						 get_error_msg(E_VAR_OF_LAMBDA_EXPR_NOT_TYPED),
						 cur_var->get_name()->get_value()) ;
		}
	  cur_var = (T_ident *)cur_var->get_next() ;
	}


  if ( (error == TRUE) || (expr->get_B_type() == NULL) )
	{
	  // Reprise sur erreur
	  return ;
	}

  // On fabrique le resultat
  TRACE0("fabrication du resultat") ;
  T_type *ltype = NULL ;
  cur_var = first_variable ;

  while (cur_var != NULL)
	{
	  T_type *cur_type = cur_var->get_B_type()->clone_type(NULL,
														   get_betree(),
														   get_ref_lexem()) ;

	  if (ltype == NULL)
		{
		  ltype = cur_type ;
		}
	  else
		{
		  T_type *save_ltype = ltype ;
		  ltype = new T_product_type(ltype,
									 cur_type,
									 NULL,
									 get_betree(),
									 get_ref_lexem()) ;
		  save_ltype->unlink() ;
		  cur_type->unlink() ;
		}

	  cur_var = (T_ident *)cur_var->get_next() ;
	}

  T_type *clone = expr->get_B_type()->clone_type(NULL,
												 get_betree(),
												 get_ref_lexem()) ;

  T_product_type *prod ;
  prod = new T_product_type(ltype, clone, NULL, get_betree(), get_ref_lexem()) ;
  ltype->unlink() ;
  clone->unlink() ;

  T_setof_type *res = new T_setof_type(prod, this, get_betree(), get_ref_lexem()) ;
  prod->unlink() ;
  set_B_type(res, get_ref_lexem()) ;
  res->unlink() ;
}

// Type check de PI
// CTRL Ref: VTYPE 2-1-4
void T_pi::internal_make_type(void)
{
  // On verifie que tous les identificateurs sont distincts
  check_no_double_definition(first_variable) ;

  // Typage des variables
  predicate->type_check() ;

  // L'expression doit etre de type entier
  expr->make_type() ;

  if ( (expr->get_B_type() == NULL)
             || (expr->get_B_type()->is_an_integer() == FALSE
                 && expr->get_B_type()->is_a_real() == FALSE) )
	{
	  semantic_error(expr,
					 CAN_CONTINUE,
                     get_error_msg(E_OPERAND_SHOULD_BE_AN_INTEGER_OR_REAL),
					 make_type_name(expr)->get_value()) ;
	  return ;
	}

  // Le resultat est de type entier
  T_type *type = NULL;

    if (expr->get_B_type()->is_an_integer() == TRUE) {
          type = new T_predefined_type(BTYPE_INTEGER,
									   NULL,
									   NULL,
									   this,
									   get_betree(),
									   get_ref_lexem()) ;
    } else {
        type = new T_predefined_type(BTYPE_REAL,
                                                                   NULL,
                                                                   NULL,
                                                                   this,
                                                                   get_betree(),
                                                                   get_ref_lexem()) ;
    }


  set_B_type(type, get_ref_lexem()) ;
  type->unlink() ;
  //GP 18/01/99
  //Toutes les variables introduites par PI doivent etre typee
  check_type_ident_list(get_variables()) ;
  }


// Type check de SIGMA
// CTRL Ref: VTYPE 2-1-4
void T_sigma::internal_make_type(void)
{
  // On verifie que tous les identificateurs sont distincts
  check_no_double_definition(first_variable) ;

  // Typage des variables
  predicate->type_check() ;

  // L'expression doit etre de type entier
  expr->make_type() ;

  if ( (expr->get_B_type() == NULL)
            || (expr->get_B_type()->is_an_integer() == FALSE
                && expr->get_B_type()->is_a_real() == FALSE) )
	{
	  semantic_error(expr,
					 CAN_CONTINUE,
					 get_error_msg(E_OPERAND_SHOULD_BE_AN_INTEGER),
					 make_type_name(expr)->get_value()) ;
	  return ;
	}

  // Le resultat est de type entier
  T_type *type = NULL;
 if (expr->get_B_type()->is_an_integer() == TRUE) {
         type =  new T_predefined_type(BTYPE_INTEGER,
									   NULL,
									   NULL,
									   this,
									   get_betree(),
                                                                           get_ref_lexem());
 } else {
     type = new T_predefined_type(BTYPE_REAL,
                                                                NULL,
                                                                NULL,
                                                                this,
                                                                get_betree(),
                                                                get_ref_lexem()) ;
 }

  set_B_type(type, get_ref_lexem()) ;
  type->unlink() ;

  //GP 18/01/99
  //Toutes les variables introduites par SIGMA doivent etre typee
  check_type_ident_list(get_variables()) ;
      }


// Union quantifiee
void T_quantified_union::internal_make_type(void)
{
  TRACE1("T_quantified_union(%x)::internal_make_type", this) ;

  // Typage des identificateurs
  predicate->type_check() ;




  // Typage de l'expression
  expr->make_type() ;

  //GP 07/01/99
  expr->check_type() ;
  predicate->check_type() ;

  // Le resultat est du meme type que expr
  T_type *type = expr->get_B_type() ;

  if (type != NULL)
	{
	  T_type *res = type->clone_type(this, get_betree(), get_ref_lexem()) ;
	  set_B_type(res, get_ref_lexem()) ;
	  res->unlink() ;
	}
  //GP 18/01/99
  //CTRL Ref : VTYPE 2-3-6
  if (expr->is_a_set() == FALSE)
	{
	  semantic_error(expr,
					 CAN_CONTINUE,
					 get_error_msg(E_OPERAND_SHOULD_BE_A_SET),
					 make_type_name(expr)->get_value()) ;
	}
  check_type_ident_list(get_variables()) ;
}

// Intersection quantifiee
void T_quantified_intersection::internal_make_type(void)
{
  TRACE1("T_quantified_intersection(%x)::internal_make_type", this) ;

  // Typage des identificateurs
  predicate->type_check() ;

  // Typage de l'expression
  expr->make_type() ;

  //GP 07/01/99
  expr->check_type() ;
  predicate->check_type() ;

  // Le resultat est du meme type que expr
  T_type *type = expr->get_B_type() ;

  if (type != NULL)
	{
	  T_type *res = type->clone_type(this, get_betree(), get_ref_lexem()) ;
	  set_B_type(res, get_ref_lexem()) ;
	  res->unlink() ;
	}
  //GP 18/01/99
  //CTRL Ref : VTYPE 2-3-6
  if (expr->is_a_set() == FALSE)
	{
	  semantic_error(expr,
					 CAN_CONTINUE,
					 get_error_msg(E_OPERAND_SHOULD_BE_A_SET),
					 make_type_name(expr)->get_value()) ;
	}
  check_type_ident_list(get_variables()) ;
}


void T_generalised_union::internal_make_type(void)
{
  TRACE1("T_generalised_union(%x)::internal_make_type", this) ;
  int nb_in_params = 0;
  // Typage des expressions
  T_expr *cur_expr = first_expr ;
  T_setof_type *ref_type = NULL ;

  while (cur_expr != NULL)
	{
	  cur_expr->make_type() ;



	  T_type *cur_type = cur_expr->get_B_type() ;

	  if (cur_type != NULL)
		{
		  if (ref_type == NULL)
			{
			  // Le type doit etre de la forme Setof(Setof(t))
			  // Par defaut ce n'est pas le cas !
			  int valid = FALSE ;
			  if (cur_type->is_a_set() == TRUE)
				{
				  T_type *inside = ((T_setof_type *)cur_type)->get_base_type() ;
				  if (inside->is_a_set() == TRUE)
					{
					  valid = TRUE ;
					}
				}

			  if (valid == FALSE)
				{
				  semantic_error(cur_expr,
								 CAN_CONTINUE,
								 get_error_msg(E_TYPE_SHOULD_BE_A_SET_OF_SET),
								 cur_type->make_type_name()->get_value()) ;
				}
			  else
				{
				  // Cast valide car on a fait les tests ci-dessus
				  ref_type = (T_setof_type *)cur_type ;
				}
			}
		  else
			{
			  if (cur_type->is_compatible_with(ref_type) == FALSE)
				{
				  semantic_error(cur_expr,
								 CAN_CONTINUE,
								 get_error_msg(E_INCOMPATIBLE_TYPES_IN_EXPR),
								 ref_type->make_type_name()->get_value(),
								 cur_type->make_type_name()->get_value()) ;
				}
			}

		}

	  //GP 07/01/99
	  cur_expr->check_type() ;

	  cur_expr = (T_expr *)cur_expr->get_next() ;

      nb_in_params++;
  }

  if (nb_in_params != 1) {
      syntax_error(this->get_ref_lexem(),
                   CAN_CONTINUE,
                   get_error_msg(E_BUILTIN_OP_HAS_ONLY_ONE_OPERAND),
                   get_lex_type_ascii(L_GUNION),
                   nb_in_params) ;
  }

  if (ref_type != NULL)
  {
	  T_type *res = ref_type->get_base_type()->clone_type(this,
														  get_betree(),
														  get_ref_lexem()) ;
	  set_B_type(res, get_ref_lexem()) ;
	  res->unlink() ;
	}
}

void T_generalised_intersection::internal_make_type(void)
{
  TRACE1("T_generalised_intersection(%x)::internal_make_type", this) ;

  // Typage des expressions
  T_expr *cur_expr = first_expr ;
  T_setof_type *ref_type = NULL ;
  int nb_in_params = 0;

  while (cur_expr != NULL)
	{
	  cur_expr->make_type() ;

	  //GP 07/01/99
	  cur_expr->check_type() ;

	  T_type *cur_type = cur_expr->get_B_type() ;

	  if (cur_type != NULL)
		{
		  if (ref_type == NULL)
			{
			  // Le type doit etre de la forme Setof(Setof(t))
			  // Par defaut ce n'est pas le cas !
			  int valid = FALSE ;
			  if (cur_type->is_a_set() == TRUE)
				{
				  T_type *inside = ((T_setof_type *)cur_type)->get_base_type() ;
				  if (inside->is_a_set() == TRUE)
					{
					  valid = TRUE ;
					}
				}

			  if (valid == FALSE)
				{
				  semantic_error(cur_expr,
								 CAN_CONTINUE,
								 get_error_msg(E_TYPE_SHOULD_BE_A_SET_OF_SET),
								 cur_type->make_type_name()->get_value()) ;
				}
			  else
				{
				  // Cast valide car on a fait les tests ci-dessus
				  ref_type = (T_setof_type *)cur_type ;
				}
			}
		  else
			{
			  if (cur_type->is_compatible_with(ref_type) == FALSE)
				{
				  semantic_error(cur_expr,
								 CAN_CONTINUE,
								 get_error_msg(E_INCOMPATIBLE_TYPES_IN_EXPR),
								 ref_type->make_type_name()->get_value(),
								 cur_type->make_type_name()->get_value()) ;
				}
			}
		}

	  cur_expr = (T_expr *)cur_expr->get_next() ;
      nb_in_params++;
	}

  if (nb_in_params != 1) {
      syntax_error(this->get_ref_lexem(),
                   CAN_CONTINUE,
                   get_error_msg(E_BUILTIN_OP_HAS_ONLY_ONE_OPERAND),
                   get_lex_type_ascii(L_GINTER),
                   nb_in_params) ;
  }

  if (ref_type != NULL)
	{
	  T_type *res = ref_type->get_base_type()->clone_type(this,
														  get_betree(),
														  get_ref_lexem()) ;
	  set_B_type(res, get_ref_lexem()) ;
	  res->unlink() ;
	}
}

void T_bound::internal_make_type(void)
{
  // On ne calcule jamais le type des bornes
  // A FAIRE : internal_error
  assert(FALSE) ;
}

//
//	}{	Fin du type check des expressions
//

