/******************************* CLEARSY **************************************
* Fichier : $Id: i_check.cpp,v 2.0 2002-04-25 08:58:28 fl Exp $
* (C) 2008 CLEARSY
*
* Description :		B0 Checker, Fonctions de verification syntaxique de B0
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
RCS_ID("$Id: i_check.cpp,v 1.27 2002-04-25 08:58:28 fl Exp $") ;

/* Includes systeme */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

/* Includes Composant Local */

#include "c_api.h"
#include "i_aux.h"

#ifdef B0C
#include "i_ctx.h"
#include "i_listid.h"
#include "i_lstind.h"
#include "i_globdat.h"

/* Includes des donnees globales */

//////////////////////////////////////////////////////////////////////////////
//
// Verification des termes et des conditions
//
//
//////////////////////////////////////////////////////////////////////////////

//
// Est-ce un terme ?
//
// Fonction de vérification : est-ce un terme ?
// CTRL Ref : CSYN 9-1 a CSYN 9-10
int T_item::syntax_check_is_a_term(T_list_ident **list_ident)
{
  TRACE2("T_item(%x, %s)::syntax_check_is_a_term() ",
		 this,
		 get_class_name()) ;
  return ( (syntax_check_is_a_simple_term(list_ident) == TRUE) ||
		   (syntax_check_is_an_array_access(list_ident) == TRUE) ||
		   (syntax_check_is_an_arith_expr(list_ident) == TRUE) ||
		   (syntax_check_is_an_extension_record(list_ident) == TRUE) ||
		   (syntax_check_is_an_extension_record_access(list_ident) == TRUE) );
}

//
// Est-ce un record en extension
//
// CTRL Ref : CSYN 11-3
int T_item::syntax_check_is_an_extension_record(T_list_ident **list_ident)
{
  TRACE2("T_item(%x, %s)::syntax_check_is_an_extension_record() -> FALSE",
		 this,
		 get_class_name()) ;
  return FALSE ;
}

int T_record::syntax_check_is_an_extension_record(T_list_ident **list_ident)
{
  TRACE2("T_record(%x, %s)::syntax_check_is_an_extension_record()",
		 this,
		 get_class_name()) ;
  if (syntax_check_list_is_a_record_item(first_record_item,
										 list_ident) == TRUE)
	{
	  return TRUE;
	}
  else
	{
	  // Erreur : ce n'est pas un record B0
	  if (get_check() != B0CHK_INIT) // si on ne fait pas une 2eme passe pour RINIT 1-1
		{
		  B0_syntax_error(this,
						  CAN_CONTINUE,
						  get_error_msg(E_RECORD_IS_NOT_A_B0_RECORD),
						  make_class_name(this)) ;
		}
	  return TRUE;
	}
}


//
// Est-ce un acces a un tableau
//
// CTRL Ref : CSYN 11-1
int T_item::syntax_check_is_an_array_access(T_list_ident **list_ident)
{
  TRACE2("T_item(%x, %s)::syntax_check_is_an_array_access() -> FALSE",
		 this,
		 get_class_name()) ;
  return FALSE ;
}

// fonction auxiliaire de factorisation de code pour tester un acces a un
// tableau pour les classes T-array_item et T_op_result
int aux_is_an_array_access(T_item * name,
								   T_item * list_item,
								   T_list_ident **list_ident)
{
  TRACE2("aux_is_an_array_access(%x:%s)", name, make_class_name(name)) ;

  if (name->is_an_ident() == TRUE)
	{
	  // OK, c'est un ident.
	}
  else if (name->get_node_type() == NODE_RECORD_ACCESS)
	{
	  // ??? SL 30/08/1999 : je pense qu'un acces a un record est
	  // egalement convenable ici (cf banc de test traducteurs)
	  // Etudier plus serieusement ce probleme ...

	  // C'est a priori O.K., sinon on ne peut pas accéder directement
	  // à un élément d'un tableau si ce tableau est un champ de
	  // record !  Mais, ce n'est pas dans le manuel de référence du
	  // langage.
	}
  else if (get_B0_allow_arrays_of_arrays() == TRUE &&
		  name->syntax_check_is_an_array_access(list_ident) == TRUE)
	{
	  // Si on autorise les tableaux de tableau, alors le tableau peut
	  // lui-même être un accès à un élément de tableau.
	}
  else
	{
	  // Erreur : le tableau accédé n'a pas la bonne forme
	  if (get_check() != B0CHK_INIT)
		{
		  // si on ne fait pas une 2eme passe pour RINIT 1-1
		  B0_syntax_error(name,
						  CAN_CONTINUE,
						  get_error_msg(E_B0_INVALID_ARRAY_EXPRESSION),
						  make_class_name(name)) ;
		}
	  return FALSE;
	}

  if (syntax_check_list_is_a_term(list_item, list_ident) == TRUE)
	{
	  return TRUE;
	}
  else
	{
	  // Erreur : ce n'est pas un tableau B0
	  if (get_check() != B0CHK_INIT) // si on ne fait pas une 2eme passe pour RINIT 1-1
		{
		  B0_syntax_error(name,
						  CAN_CONTINUE,
						  get_error_msg(E_B0_INDEX_MUST_BE_A_TERM)) ;
		}
	  return FALSE;
	}
}

int T_array_item::syntax_check_is_an_array_access(T_list_ident **list_ident)
{
  TRACE2("T_array_item(%x, %s)::syntax_check_is_an_array_access()",
		 this,
		 get_class_name()) ;

  return  aux_is_an_array_access(array_name,
								first_index,
								list_ident);

}

int T_op_result::syntax_check_is_an_array_access(T_list_ident **list_ident)
{
  TRACE2("T_op_result(%x, %s)::syntax_check_is_an_array_access()",
		 this,
		 get_class_name()) ;
  if ((ref_builtin != NULL) ||
	  (nb_in_params == 0) )
	{
	  return FALSE;
	}

  return  aux_is_an_array_access(op_name,
								first_in_param,
								list_ident);

}


//
// Dans le cas d'un tableau (RTYPETAB 1-4)
// On verifie si l'identificateur n'est pas une variable concrete,
// un parametre formel, ou un parametre d'E/S d'operation
//
void T_ident::check_allowed_for_array_index()
{
  T_ident *ident = B0_get_definition();
  if ( (ident->get_ident_type() == ITYPE_ABSTRACT_VARIABLE) ||
	   (ident->get_ident_type() == ITYPE_ABSTRACT_CONSTANT) ||
	   (ident->get_ident_type() == ITYPE_CONCRETE_VARIABLE) ||
	   (ident->get_ident_type() == ITYPE_MACHINE_SCALAR_FORMAL_PARAM) ||
	   (ident->get_ident_type() == ITYPE_MACHINE_SET_FORMAL_PARAM) ||
	   (ident->get_ident_type() == ITYPE_OP_IN_PARAM) ||
	   (ident->get_ident_type() == ITYPE_OP_OUT_PARAM) )
	{
	  // ERREUR
	  if (get_check() != B0CHK_INIT) // si on ne fait pas une 2eme passe pour RINIT 1-1
		{
		  B0_semantic_error(this,
							CAN_CONTINUE,
							get_error_msg(E_B0_INVALID_IDENT_TYPE_FOR_ARRAY_INDEX),
							this->B0_get_definition()->get_ident_type_name(),
							this->B0_get_definition()->get_name()->get_value()) ;
		}
	}
}


//
// Est-ce une plage
//
int T_item::syntax_check_is_a_range(T_list_ident **list_ident,
											 T_B0_context context)
{
  TRACE2("T_item(%x, %s)::syntax_check_is_a_range() -> FALSE",
		 this,
		 get_class_name()) ;
  return FALSE ;
}

int T_expr_with_parenthesis::syntax_check_is_a_range(T_list_ident **list_ident,
															  T_B0_context context)
{
  {
	return expr->syntax_check_is_a_range(list_ident,
										 context);
  }
}

int T_list_link::syntax_check_is_a_range(T_list_ident **list_ident,
												  T_B0_context context)
{
  return ((T_item *)object)->syntax_check_is_a_range(list_ident,
													 context);
}

int T_ident::syntax_check_is_a_range(T_list_ident **list_ident,
											  T_B0_context context)
{
  TRACE2("T_ident(%x, %s)::syntax_check_is_a_range() ",
		 this,
		 get_class_name()) ;

  // Test des cycles eventuels dans les valuations ou initialisation
  // suivant la valeur du flag global check
  check_cycle(list_ident);


  if (ident_type == ITYPE_BUILTIN)
	{
	  // context : plage pour typage de parametre de machine
	  if ( (context == B0CTX_FORMAL_PARAMETER_TYPE_CONTEXT) &&
		   ( (name == get_builtin_INTEGER()->get_name()) ||
			 (name == get_builtin_NATURAL()->get_name()) ||
			 (name == get_builtin_NATURAL1()->get_name()) ) )
		{
		  // OK : c'est une plage
		  return TRUE;
		}

	  // CTRL Ref : CSYN 14-2
	  if ((name == get_builtin_NAT()->get_name()) ||
		  (name == get_builtin_NAT1()->get_name()) ||
		  (name == get_builtin_INT()->get_name()) ||
		  (name == get_builtin_BOOL()->get_name())) // ajout SL 30/08/1999
		{
		  // OK : c'est une plage
		  return TRUE;
		}
	  else if (get_allow_unbounded_builtin_types() == TRUE &&
			   (name == get_builtin_INTEGER()->get_name() ||
				name == get_builtin_NATURAL()->get_name() ||
				name == get_builtin_NATURAL1()->get_name() ))
		{
		  // OK : autorisé par ressource.
		  return TRUE;
		}
	  else
		{
		  // Erreur : ce n'est pas une plage
		  if (get_check() != B0CHK_INIT) // si on ne fait pas une 2eme passe pour RINIT1-1
			{
			  B0_syntax_error(this,
							  CAN_CONTINUE,
							  get_error_msg(E_B0_INVALID_BUILTIN_FOR_RANGE),
							  make_class_name(this)) ;
			}
		  return FALSE;
		}
	}

  // Si on est dans un contexte de valuation
  if (context == B0CTX_VALUATION_CONTEXT)
	{
	  if ( (B0_get_definition()->get_ident_type() != ITYPE_CONCRETE_CONSTANT) &&
		   (B0_get_definition()->get_ident_type() != ITYPE_ABSTRACT_SET) )
		{
		  // ERREUR Valuation : on attend une constante concrete
		  // ou un ensemble abstrait
		  B0_semantic_error(this,
							CAN_CONTINUE,
							get_error_msg(E_B0_INVALID_IDENT_TYPE_FOR_VALUATION),
							this->B0_get_definition()->get_ident_type_name(),
							this->B0_get_definition()->get_name()->get_value()) ;

		  return FALSE;
		}
	  return TRUE;
	}

  // context : dans le cas d'un tableau (RTYPETAB 1-4)
  if ( context == B0CTX_ARRAY_CONTEXT )
	{
	  check_allowed_for_array_index();
	}

  // context : plage pour typage de parametre de machine
  if ( (context == B0CTX_FORMAL_PARAMETER_TYPE_CONTEXT) &&
	   (get_definition()->get_ident_type() != ITYPE_MACHINE_SET_FORMAL_PARAM) &&
	   (get_definition()->get_ident_type() != ITYPE_MACHINE_SCALAR_FORMAL_PARAM) )
	{
	  // Erreur B0 : La nature de l'identificateur attendu est un parametre
	  // formel
	  B0_syntax_error(this,
					  CAN_CONTINUE,
					  get_error_msg(E_B0_INVALID_IDENT_TYPE_FOR_FORMAL_PARAMETER),
					  this->get_definition()->get_ident_type_name(),
					  this->get_definition()->get_name()->get_value()) ;
	  return FALSE;
	}


  // Ajout pour JPP : problemes des ensembles simples representes par une
  // constante abstraite ou une variable abstraite (interdit meme si les types
  // sont compatibles)
  if ( (B0_get_definition()->get_ident_type() == ITYPE_ABSTRACT_VARIABLE) ||
	   (B0_get_definition()->get_ident_type() == ITYPE_ABSTRACT_CONSTANT) )
	{
	  // ERREUR B0
	  if (get_check() != B0CHK_INIT) // si on ne fait pas une 2eme
		// passe pour RINIT 1-1
		{
		  B0_semantic_error(this,
							CAN_CONTINUE,
							get_error_msg(E_B0_INVALID_IDENT_TYPE_FOR_CONCRETE_VALUE),
							this->B0_get_definition()->get_ident_type_name(),
							this->B0_get_definition()->get_name()->get_value()) ;
		}
	}

  return TRUE;
}

// CTRL Ref : CSYN 14-1
int T_binary_op::syntax_check_is_a_range(T_list_ident **list_ident,
												  T_B0_context context)
{
  TRACE2("T_binary_op(%x, %s)::syntax_check_is_a_range() ",
		 this,
		 get_class_name()) ;
  if (oper == BOP_INTERVAL)
	{
	  // si les operandes sont des expressions arithmetiques, c'est
	  // une plage
	  int cur_op1 = op1->syntax_check_is_an_arith_expr(list_ident, context);
	  int cur_op2 = op2->syntax_check_is_an_arith_expr(list_ident, context);

	  if ((cur_op1 == TRUE) && (cur_op2 == TRUE))
		{
		  // c'est une plage
		  return TRUE;
		}
	  else
		{
		  // Erreur : ce n'est pas une plage
		  // les operandes doivent etre des expr arithmetiques
		  if (cur_op1 == FALSE)
			{
			  if (get_check() != B0CHK_INIT) // si on ne fait pas une 2eme passe pour RINIT1-1
				{
				  B0_syntax_error(op1,
								  CAN_CONTINUE,
								  get_error_msg(E_B0_INTERVAL_BOUND_MUST_BE_ARITH_EXPRESSION),
								  make_class_name(op1)) ;
				}
			}
		  if (cur_op2 == FALSE)
			{
			  if (get_check() != B0CHK_INIT) // si on ne fait pas une 2eme passe pour RINIT1-1
				{
				  B0_syntax_error(op2,
								  CAN_CONTINUE,
								  get_error_msg(E_B0_INTERVAL_BOUND_MUST_BE_ARITH_EXPRESSION),
								  make_class_name(op2)) ;
				}
			}

		  return FALSE;
		}
	}

  return FALSE;
}

// Fonction de vérification : est-ce une expression booleenne ?
// CTRL Ref : CSYN 11-2
int T_item::
	syntax_check_is_a_bool_expr(T_list_ident **list_ident)
{
   TRACE2("T_item(%x, %s)::syntax_check_is_a_bool_expr() -> FALSE",
		 this,
		 get_class_name()) ;
  return FALSE ;
}


// T_op_result : c'est une expresion booleenne si le parametre unique
// est une condition et si le nom de l'operation est bool
int T_op_result::
	syntax_check_is_a_bool_expr(T_list_ident **list_ident)
{
  if ((ref_builtin != NULL) &&
	  (ref_builtin->get_lex_type() == L_BOOL) &&
	  (nb_in_params == 1))
	{
	  if (get_B0_disable_predicate_syntax_check() == TRUE) {return TRUE ;};
	  if (first_in_param->syntax_check_is_a_B0_predicate(list_ident) == TRUE)
		{
		  return TRUE;
		}
	  else
		{

		  return FALSE ;
		}
	}
  else
	{
	  return FALSE ;
	}

}

//
// Est-ce un acces a un record en extension
//
// CTRL Ref : CSYN 11-4
int T_item::syntax_check_is_an_extension_record_access(T_list_ident **list_ident)
{
  TRACE2("T_item(%x, %s)::syntax_check_is_an_extension_record_access() -> FALSE",
		 this,
		 get_class_name()) ;
  return FALSE ;
}

int T_record_access::syntax_check_is_an_extension_record_access(T_list_ident **list_ident)
{
  TRACE2("T_record_access(%x, %s)::syntax_check_is_an_extension_record_access()",
		 this,
		 get_class_name()) ;
  if (check_first_record(FALSE,
						 list_ident) == TRUE)
	{
	  return TRUE;
	}
  else
	{
	  // pour etre un acces a un record en extension, il faut que le
	  // "premier record" soit un record en extension
	  return FALSE;
	}
}



//
// est-ce un terme simple ?
//
// le parametre context est utilise dans le cadre de la verification
// RTYPETAB 1-4 sur les ensembles indices des tableaux: si il vaut 1,
// on effectue la verification
//
// CTRL Ref CSYN 10-1 a CSYN 10-4
int T_item::syntax_check_is_a_simple_term(T_list_ident **list_ident,
												   T_B0_context context)
{
   TRACE2("T_item(%x, %s)::syntax_check_is_a_simple_term() -> FALSE",
		 this,
		 get_class_name()) ;
  return FALSE ;
}

int T_expr_with_parenthesis::
	syntax_check_is_a_simple_term(T_list_ident **list_ident,
								  T_B0_context context)
{
  return expr->syntax_check_is_a_simple_term(list_ident,
											 context);
}

int T_list_link::
	syntax_check_is_a_simple_term(T_list_ident **list_ident,
								  T_B0_context context)
{
  return ((T_item *)object)->syntax_check_is_a_simple_term(list_ident,
														   context);
}

int T_ident::syntax_check_is_a_simple_term(T_list_ident **list_ident,
													T_B0_context context)
{
   TRACE2("T_ident(%x, %s)::syntax_check_is_a_simple_term() -> TRUE",
		 this,
		 get_class_name()) ;


   // Test des cycles eventuels dans les valuations ou initialisation
   // suivant la valeur du flag global check
   check_cycle(list_ident);

   // contexte de valuation de tableau : controle sur les indices
   if ( (context == B0CTX_MAPLET_INDEX_CONTEXT) )
	 {
	   if (get_definition()->get_node_type() == NODE_LITERAL_ENUMERATED_VALUE)
		 {
		   return TRUE;
		 }

	   // ERREUR B0 : on attend un scalaire literal
	   return FALSE;
	 }

   // Cas de MAXINT et MININT
   if ( (get_definition()->get_ident_type() == ITYPE_BUILTIN) &&
		( (name == get_builtin_MAXINT()->get_name()) ||
		  (name == get_builtin_MININT()->get_name()) ) )
	 {
	   return TRUE;
	 }

   // contexte de verification RTYPETAB 1-4 (sur les index possibles)
   if ( (context == B0CTX_ARRAY_CONTEXT) )
	 {
	   check_allowed_for_array_index();
	 }

   // Est-ce un identfiicateur B0 (i.e., concret) ?
   int valid = FALSE;
   switch (B0_get_definition()->get_ident_type())
	 {
	 case ITYPE_ENUMERATED_VALUE:
	 case ITYPE_CONCRETE_CONSTANT:
	 case ITYPE_MACHINE_SCALAR_FORMAL_PARAM:
	 case ITYPE_MACHINE_SET_FORMAL_PARAM:
	 case ITYPE_ABSTRACT_SET:
	 case ITYPE_ENUMERATED_SET:
	 case ITYPE_CONCRETE_VARIABLE:
	 case ITYPE_OP_IN_PARAM:
	 case ITYPE_OP_OUT_PARAM:
	 case ITYPE_LOCAL_VARIABLE:
	   {
		 valid = TRUE;
		 break;
	   }
	 case ITYPE_BUILTIN:
	   {
		 // MININT et MAXINT ont déjà été traités ci-dessus.  Les
		 // autres mots clés sont valides syntaxiquement (le typage et
		 // la finitude sont vérifiés par ailleurs, lors de la
		 // construction du type B0).
		 valid = TRUE;
		 break;
	   }
	 default:
	   {
		 // Tous les autres cas ne sont pas du B0.
		 valid = FALSE;
		 break;
	   }
	 }

   // contexte de controle des instanciations B0
   if (valid == FALSE)
	 {
	   if (context == B0CTX_INSTANCIATION_CONTEXT)
		 {
		   // ERREUR B0 : Invalid instanciation
		   B0_semantic_error(this,
							 CAN_CONTINUE,
							 get_error_msg(E_B0_INVALID_IDENT_TYPE_FOR_INSTANCIATION),
							 this->B0_get_definition()->get_ident_type_name(),
							 this->B0_get_definition()->get_name()->get_value()) ;
		 }
	   else
		 {
		   // Erreur B0 : cas général
		   B0_semantic_error(this,
							 CAN_CONTINUE,
							 get_error_msg(E_B0_INVALID_IDENT_TYPE_FOR_CONCRETE_VALUE),
							 this->B0_get_definition()->get_ident_type_name(),
							 this->B0_get_definition()->get_name()->get_value()) ;
		 }
	 }
   return valid ;
}

int T_literal_integer::
	syntax_check_is_a_simple_term(T_list_ident **list_ident,
								  T_B0_context context)
{
   TRACE2("T_literal_integer(%x, %s)::syntax_check_is_a_simple_term() -> TRUE",
		 this,
		 get_class_name()) ;
   return TRUE ;
}

int T_literal_boolean::
	syntax_check_is_a_simple_term(T_list_ident **list_ident,
								  T_B0_context context)
{
  TRACE2("T_literal_boolean(%x, %s)::syntax_check_is_a_simple_term() -> TRUE",
		 this,
		 get_class_name()) ;
  return TRUE ;
}

int T_record_access::
	syntax_check_is_a_simple_term(T_list_ident **list_ident,
								  T_B0_context context )
{
  TRACE2("T_record_access(%x, %s)::syntax_check_is_a_simple_term()",
		 this,
		 get_class_name()) ;
  if (context == B0CTX_MAPLET_INDEX_CONTEXT)
	{
	   // ERREUR B0 : On attend un scalaire literal
	   return FALSE;
	}

   if (context == B0CTX_INSTANCIATION_CONTEXT)
	 {
	   // ERREUR B0 : Invalid instanciation
	   return FALSE;
	 }

   if (check_first_record(TRUE,
						  list_ident,
						  context) == TRUE)
	 {
	   return TRUE;
	 }
   else
	 {
	   // pour etre un terme simple, il faut que le "premier record" soit
	   // un T_ident
	   return FALSE;
	 }
}



//
// Fonction de vérification : est-ce un tableau ?
//
// CTRL Ref : CSYN 13-1 a CSYN 13-3
int T_item::syntax_check_is_an_array(T_list_ident **list_ident,
											  T_B0_context context)
{
   TRACE2("T_item(%x, %s)::syntax_check_is_an_array() -> FALSE",
		 this,
		 get_class_name()) ;
  return FALSE ;
}

int T_expr_with_parenthesis::
	syntax_check_is_an_array(T_list_ident **list_ident,
							 T_B0_context context)
{
  return expr->syntax_check_is_an_array(list_ident,
										context);
}

int T_list_link::syntax_check_is_an_array(T_list_ident **list_ident,
												   T_B0_context context)
{
  return ((T_item *)object)->syntax_check_is_an_array(list_ident,
													 context);
}

int T_ident::syntax_check_is_an_array(T_list_ident **list_ident,
											   T_B0_context context)
{
   TRACE2("T_ident(%x, %s)::syntax_check_is_an_array() ",
		 this,
		 get_class_name()) ;


   // Test des cycles eventuels dans les valuations ou initialisation
   // suivant la valeur du flag global check
   check_cycle(list_ident);


   return TRUE ;
}


int T_extensive_set::
	syntax_check_is_an_array(T_list_ident **list_ident,
							 T_B0_context context)
{
   TRACE2("T_extensive_set(%x, %s)::syntax_check_is_an_array()",
		 this,
		 get_class_name()) ;

   if (syntax_check_list_is_a_maplet(first_item,
									 list_ident,
									 context) == TRUE)
	 {
	   // 12/01/99
	   // On est sur que les maplets sont conformes au B0
	   // Les indices sont des litteraux...
	   // ... Les bornes ont donc pu etre calculee dans l'inference de type
	   // On peut maintenant tester si il y a des trous...
	   ASSERT(get_original_B_type() != NULL);
	   ASSERT(get_original_B_type()->is_an_array());
	   // On peut caster, protege par l'assertion
	   T_setof_type *setof = ((T_setof_type *)get_original_B_type());
	   T_product_type *product =(T_product_type *) setof->get_spec_base_type();

	   T_list_index *list_index = new T_list_index();
	   T_type *cur_type = product->get_spec_types();

	   // On recupere les bornes sous la forme (borne1,borne2)
	   T_index *bounds_init = new T_index(cur_type);

	   int i = bounds_init->get_literal();
	   while (i <= bounds_init->get_next()->get_literal())
		 {
		   T_index *index = new T_index(i);
		   list_index->add(index);

		   i++;
		 }

	   cur_type = cur_type->get_next_spec_type();
	   T_type *cur_next_spec_type = cur_type->get_next_spec_type();

	   while (cur_next_spec_type != NULL)
		 {
		   // On recupere les deux bornes courantes
		   T_index *bounds = new T_index(cur_type);


		   T_list_index *cur_list_index = list_index;
		   list_index = new T_list_index();
		   while (cur_list_index != NULL)
			 {
			   // Borne inf de l'ensemble courant
			   i = bounds->get_literal();

			   // Tant que l'on n'arrive pas a la borne sup...
			   while (i <= bounds->get_next()->get_literal())
				 {
				   // On clone l'index
				   T_index *index = new T_index(cur_list_index->get_index());

				   // On y ajoute i (de l'ensemble courant)
				   index->add(i);

				   // On lie l'index dans la liste d'index
				   list_index->add(index);

				   i++;
				 }

			   cur_list_index = cur_list_index->get_next();
			 }

		   // On passe a l'ensemble suivant...
		   cur_type = cur_next_spec_type;
		   cur_next_spec_type = cur_next_spec_type->get_next_spec_type();
		 }

	   // A ce stade, on a construit la liste totale list_index
	   // des index theoriques...
	   // Les maplets testes couvrent-ils cette liste, et sans doublon?

	   T_item *cur_maplet = first_item;
	   while (cur_maplet != NULL)
		 {
	   	   if(list_index == NULL)
			 {
			   if (get_check() != B0CHK_INIT)
				 // si on ne fait pas une 2eme passe pour RINIT 1-1
				 {
				   // Par construction, c'est un doublon!
				   B0_semantic_error(cur_maplet,
									 CAN_CONTINUE,
									 get_error_msg(E_B0_MAPLET_ALREADY_EXISTS) );
				 }
			   cur_maplet = (T_item *) cur_maplet->get_next();
			   continue;
			 }
		   T_list_index *cur_list_index = list_index->check_maplet(cur_maplet);
		   list_index = cur_list_index;

		   cur_maplet = (T_item *) cur_maplet->get_next();
		 }

	   // On verifie si il reste des index
	   // Dans ce cas, la couverture des ensembles de definition du tableau
	   // n'est pas totale...
	   if (list_index != NULL)
		 {
		   if (get_check() != B0CHK_INIT)
			 // si on ne fait pas une 2eme passe pour RINIT 1-1
			 {
			   B0_semantic_error(this,
								 CAN_CONTINUE,
								 get_error_msg(E_B0_LACKS_OF_MAPLETS_FOR_CURRENT_ARRAY));
			 }
		 }

	   return TRUE;
	 }
   else
	 {
	   // Erreur : ce n'est pas un ensemble de maplets (un tableau)
	   if (get_check() != B0CHK_INIT) // si on ne fait pas une 2eme passe pour RINIT 1-1
		 {
		   B0_syntax_error(this,
						   CAN_CONTINUE,
						   get_error_msg(E_B0_INVALID_TERM),
						   make_class_name(this)) ;
		 }
	   return FALSE;

	 }
}

int T_binary_op::syntax_check_is_an_array(T_list_ident **list_ident,
												   T_B0_context context)
{
   TRACE2("T_binary_op(%x, %s)::syntax_check_is_an_array()",
		 this,
		 get_class_name()) ;

   if ( (oper == BOP_TIMES) &&
		// On verifie que c'est un ensemble car sinon, ca pourrait etre
		// une expression arithmetique...
		(is_a_set() == TRUE))
	 {
	   T_item * cur_item = get_expr(TRUE);
	   T_item * cur_next_item = cur_item->get_next_expr(TRUE);
	   int cur_result = TRUE;
	   ASSERT(cur_next_item != NULL);
	   while (cur_next_item != NULL)
		 {
		   // AB le 5/01/99 Une plage peut etre BOOL : delta du manuel de
		   // reference 1.8 -> 1.8.1
		   if ( (cur_item->check_is_BOOL() == FALSE) &&
				(cur_item->syntax_check_is_a_range(list_ident,
												   context) == FALSE) )
			 {
			   if (get_check() != B0CHK_INIT) // si on ne fait pas une 2eme passe pour RINIT1-1
				 {
				   // Erreur : l'element n'est pas une plage
				   B0_syntax_error(cur_item,
								   CAN_CONTINUE,
								   get_error_msg(E_B0_INVALID_RANGE_FOR_ARRAY_AGGREGATE),
								   make_class_name(cur_item)) ;
				 }

			   cur_result = FALSE;
			 }
		   cur_item = cur_next_item;
		   cur_next_item = cur_next_item->get_next_expr(TRUE);
		 }
	   if (cur_item->syntax_check_is_a_singleton(list_ident, context) == FALSE)
		 {
		   if (get_check() != B0CHK_INIT) // si on ne fait pas une 2eme passe pour RINIT 1-1
			 {
			   // Erreur : la valeur des elements du tableau constant
			   // n'est pas correcte
			   B0_syntax_error(cur_item,
							   CAN_CONTINUE,
							   get_error_msg(E_B0_INVALID_VALUE_FOR_ARRAY_AGGREGATE),
							   make_class_name(cur_item)) ;
			 }

		   return FALSE;
		 }
	   return cur_result;
	 }
   // Ce n'est pas un tableau constant
   return FALSE;
}


//
// Fonction de vérification : est-ce un maplet ?
//
int T_item::syntax_check_is_a_maplet(T_list_ident **list_ident,
											  T_B0_context context)
{
   TRACE2("T_item(%x, %s)::syntax_check_is_a_maplet() -> FALSE",
		 this,
		 get_class_name()) ;
  return FALSE ;
}

int T_expr_with_parenthesis::
   syntax_check_is_a_maplet(T_list_ident **list_ident, T_B0_context context)
{
  return expr->syntax_check_is_a_maplet(list_ident,
										context);
}

int T_list_link::syntax_check_is_a_maplet(T_list_ident **list_ident,
												   T_B0_context context)
{
  return ((T_item *)object)->syntax_check_is_a_maplet(list_ident,
													  context);
}

int T_binary_op::syntax_check_is_a_maplet(T_list_ident **list_ident,
												   T_B0_context context)
{
   TRACE2("T_binary_op(%x, %s)::syntax_check_is_a_maplet()",
		 this,
		 get_class_name()) ;

   if (oper == BOP_MAPLET)
	 {
	   T_item * cur_item = get_expr(FALSE);
	   T_item * cur_next_item = cur_item->get_next_expr(FALSE);
	   int cur_result = TRUE;
	   while (cur_next_item != NULL)
		 {
		   // CTRL Ref : RTYPETAB 1-4
		   // (controle d'un terme simple avec un contexte de tableau)
           if (( cur_item->syntax_check_is_a_simple_term(list_ident, B0CTX_MAPLET_INDEX_CONTEXT) == FALSE ) ||
               ( cur_item->syntax_check_is_a_term(list_ident) == FALSE ))
			 {
               // Erreur : l'element n'est pas un terme simple OU la valeur du maplet n'est pas correcte
			   if (get_check() != B0CHK_INIT) //si on ne fait pas une 2eme passe pour RINIT1-1
				 {
				   B0_syntax_error(cur_item,
								  CAN_CONTINUE,
								  get_error_msg(E_B0_INVALID_INDEX_FOR_MAPLET),
								  make_class_name(cur_item)) ;
				 }

			   cur_result = FALSE;
			 }
		   cur_item = cur_next_item;
		   cur_next_item = cur_next_item->get_next_expr(FALSE);
		 }
	   return cur_result;
	 }
   // Ce n'est pas un maplet
   return FALSE;
}

//
// Fonction de vérification : est-ce un singleton ?
//
int T_item::syntax_check_is_a_singleton(T_list_ident **list_ident,
												 T_B0_context context)
{
   TRACE2("T_item(%x, %s)::syntax_check_is_a_singleton() -> FALSE",
		 this,
		 get_class_name()) ;
  return FALSE ;
}


int T_extensive_set::
	syntax_check_is_a_singleton(T_list_ident **list_ident,
								T_B0_context context)
{
   TRACE2("T_extensive_set(%x, %s)::syntax_check_is_a_singleton()",
		 this,
		 get_class_name()) ;
   if (first_item != last_item)
	 {
	   // Ce n'est pas un singleton
	   return FALSE;
	 }

   if (first_item->syntax_check_is_a_term(list_ident) == TRUE)
	 {
	   // C'est un singleton terme
	   return TRUE;
	 }
   else if (get_B0_allow_arrays_of_arrays() == TRUE &&
			first_item->syntax_check_is_an_array(list_ident, context) == TRUE)
	 {
	   // C'est un singleton agrégat tableau (accepté uniquement si on
	   // autorise les tableaux de tableaux).
	   return TRUE;
	 }
   else
	 {
	   // C'est un autre singleton qui n'est pas B0
	   return FALSE;
	 }
}

//
// Fonction de vérification : est-ce une expression arithmetique ?
//
// CTRL Ref : CSYN 12-1 a CSYN 12-7
int T_item::
	syntax_check_is_an_arith_expr(T_list_ident **list_ident,
										T_B0_context context)
{
   TRACE2("T_item(%x, %s)::syntax_check_is_an_arith_expr() -> FALSE",
		 this,
		 get_class_name()) ;
  return FALSE ;
}

int T_expr_with_parenthesis::
	syntax_check_is_an_arith_expr(T_list_ident **list_ident,
										T_B0_context context)
{
  return expr->syntax_check_is_an_arith_expr(list_ident,
												   context);
}

int T_list_link::
	syntax_check_is_an_arith_expr(T_list_ident **list_ident,
										T_B0_context context)
{
  return ((T_item *)object)->syntax_check_is_an_arith_expr(list_ident,
																 context);
}

int T_ident::
	syntax_check_is_an_arith_expr(T_list_ident **list_ident,
										T_B0_context context)
{
   TRACE2("T_ident(%x, %s)::syntax_check_is_an_arith_expr() ",
		 this,
		 get_class_name()) ;

   // Test des cycles eventuels dans les valuations ou initialisation
   // suivant la valeur du flag global check
   check_cycle(list_ident);


   // Cas de MAXINT et MININT
   if ( (get_definition()->get_ident_type() == ITYPE_BUILTIN) &&
		( (name == get_builtin_MAXINT()->get_name()) ||
		  (name == get_builtin_MININT()->get_name()) ) )
	 {
		  return TRUE;
	 }

   // Si le type n'est pas entier, ce n'est pas la peine d'aller plus loin
   /*  if (get_definition()->get_B_type()->is_a_concrete_integer() == FALSE)
	 {
	   return FALSE;
	   }*/

   // Dans un contexte de valuation
   if (context == B0CTX_VALUATION_CONTEXT)
	 {
	   if (B0_get_definition()->get_ident_type() != ITYPE_CONCRETE_CONSTANT)
		 {
		   // ERREUR B0 : on attend une constante concrete
		   B0_semantic_error(this,
							 CAN_CONTINUE,
							 get_error_msg(E_B0_INVALID_IDENT_TYPE_FOR_VALUATION),
							 this->B0_get_definition()->get_ident_type_name(),
							 this->B0_get_definition()->get_name()->get_value() );
		 }
	 }

   // Dans un contexte d'instanciation
   if (context == B0CTX_INSTANCIATION_CONTEXT)
	 {
	   if ( (B0_get_definition()->get_ident_type() != ITYPE_CONCRETE_CONSTANT) &&
			(B0_get_definition()->get_ident_type()
			 != ITYPE_MACHINE_SCALAR_FORMAL_PARAM) )
		 {
		   // ERREUR B0 : Invalid Instanciation
		   B0_semantic_error(this,
							 CAN_CONTINUE,
							 get_error_msg(E_B0_INVALID_IDENT_TYPE_FOR_INSTANCIATION),
							 this->B0_get_definition()->get_ident_type_name(),
							 this->B0_get_definition()->get_name()->get_value() );
		 }

	 }

   // Dans le cas d'un tableau (RTYPETAB 1-4)
   if (context == B0CTX_ARRAY_CONTEXT)
	 {
	   check_allowed_for_array_index();
	 }

   return TRUE ;
}

int T_literal_integer::
	syntax_check_is_an_arith_expr(T_list_ident **list_ident,
										T_B0_context context)
{
   TRACE2("T_literal_integer(%x, %s)::syntax_check_is_an_arith_expr() -> TRUE",
		 this,
		 get_class_name()) ;
  return TRUE ;
}

int T_record_access::
    syntax_check_is_an_arith_expr(T_list_ident **list_ident,
										T_B0_context context)
{
	return syntax_check_is_a_simple_term(list_ident,
										 context );
}

// Ajout SL 23/09/99 suite discussion avec FB pour accepter par ex t(x):=t(x)+1
int T_array_item::syntax_check_is_an_arith_expr(T_list_ident **list_ident,
														 T_B0_context context)
{
  // Il faut verifier que tous les indexes sont des termes
  T_expr *cur_index = first_index ;

  while (cur_index != NULL)
	{
	  if (cur_index->syntax_check_is_a_term(list_ident) == FALSE)
		{
		  return FALSE ;
		}

	  cur_index = (T_expr *)cur_index->get_next() ;
	}

  return TRUE ;
}

int T_binary_op::
	syntax_check_is_an_arith_expr(T_list_ident **list_ident,
										T_B0_context context)
{
   TRACE2("T_binary_op(%x, %s)::syntax_check_is_an_arith_expr() ",
		 this,
		 get_class_name()) ;

   switch (oper)
	 {
	 case BOP_TIMES :
	   {
		 // On verifie auparavant que le T_binary_op ne represente pas
		 // un ensmble.
		 // Auquel cas, c'est un certainement un tableau, mais certainement pas
		 // une expression arithmetique
		 if ( (is_a_set() == TRUE) || (get_B_type() == NULL) )
		   {
			 TRACE0("FALSE") ;
			 return FALSE;
		   }

		 // Pas de break pour etre traite comme les suivants (c'est peut-etre
		 // une expression arithmetique...
	   }
	 case BOP_PLUS :
	 case BOP_MINUS :
	 case BOP_DIVIDES :
     case BOP_MOD :
     case BOP_POWER :
	   {
		 // si les operandes sont des expressions arithmetiques, c'est
		 // une expression arithmetique
                 int cur_op1 = op1->syntax_check_is_an_arith_expr(list_ident, context) ;
                 int cur_op2 = op2->syntax_check_is_an_arith_expr(list_ident, context) ;

		 if ((cur_op1 == TRUE) &&
			 (cur_op2 == TRUE))
		   {
			 // c'est une expression arithmetique
			 TRACE0("TRUE") ;
			 return TRUE;
		   }
		 else
		   {
			 // Erreur : les operandes doivent etre des expr arithmetiques
			 if (cur_op1 == FALSE)
			   {
				 if (get_check() != B0CHK_INIT)//si on ne fait pas une 2e passe pour RINIT1-1
				   {
					 B0_syntax_error(op1,
									 CAN_CONTINUE,
									 get_error_msg(E_B0_OPERAND_MUST_BE_AN_ARITH_EXPRESSION),
									 make_class_name(op1),
									 make_class_name(this)) ;
				   }
			   }
			 if (cur_op2 == FALSE)
			   {
				 if (get_check() != B0CHK_INIT) //si on ne fait pas une 2e passe pour RINIT1-1
				   {
					 B0_syntax_error(op2,
									 CAN_CONTINUE,
									 get_error_msg(E_B0_OPERAND_MUST_BE_AN_ARITH_EXPRESSION),
									 make_class_name(op2),
									 make_class_name(this)) ;
				   }
			   }
			 TRACE0("FALSE") ;
			 return FALSE;
		   }
	   }
	 case BOP_INTERVAL :
	 case BOP_MAPLET :
	 case BOP_COMMA :
	 case BOP_PIPE :
	 case BOP_RESTRICT :
	 case BOP_ANTIRESTRICT :
	 case BOP_CORESTRICT :
	 case BOP_ANTICORESTRICT :
	 case BOP_LEFT_OVERLOAD :
	 case BOP_CONCAT :
	 case BOP_HEAD_INSERT :
	 case BOP_TAIL_INSERT :
	 case BOP_HEAD_RESTRICT :
	 case BOP_TAIL_RESTRICT :
	 case BOP_SET_RELATION :
	 case BOP_SURJ_RELATION :
	 case BOP_TOTAL_RELATION :
	 case BOP_TOTAL_SURJ_RELATION :
	 case BOP_CONSTANT_FUNCTION :
	 case BOP_DIRECT_PRODUCT :
	 case BOP_PARALLEL_PRODUCT :
	 case BOP_INTERSECTION :
	 case BOP_UNION :
	 case BOP_COMPOSITION :
	   {
		 // ce ne sont pas des expressions arithmetiques
		 TRACE0("FALSE") ;
		 return FALSE;
	   }
	   // Pas de default pour que le compilateur nous
	   // previenne en cas d'ajout
	 }
   // Pour eviter le warning
   ASSERT(FALSE);
   TRACE0("FALSE") ;
   return FALSE;
}


int T_unary_op::
	syntax_check_is_an_arith_expr(T_list_ident **list_ident,
										T_B0_context context)
{
   TRACE2("T_unary_op(%x, %s)::syntax_check_is_an_arith_expr() ",
		 this,
		 get_class_name()) ;

   switch (oper)
	 {
	 case UOP_MINUS :
	   {
		 // si l'operande est une expression arithmetique, c'est
		 // une expression arithmetique
		 if (operand->syntax_check_is_an_arith_expr(list_ident,
														  context) == TRUE)
		   {
			 // c'est une expression arithmetique
			 return TRUE;
		   }
		 else
		   {
			 if (get_check() != B0CHK_INIT) // si on ne fait pas une 2eme passe pour RINIT 1-1
			   {
				 // Erreur : l'operande doit etre une expr arithmetique
				 B0_syntax_error(operand,
								 CAN_CONTINUE,
								 get_error_msg(E_B0_OPERAND_MUST_BE_AN_ARITH_EXPRESSION),
								 make_class_name(operand),
								 make_class_name(this)) ;
			   }

			 return FALSE;
		   }
	   }
	   // Pas de default pour que le compilateur nous
	   // previenne en cas d'ajout
	 }
   // Pour eviter le warning
   ASSERT(FALSE);
   return FALSE;

}

// T_op_result :c'est une expresion arithmetique si le parametre unique
// est une expression arithmetique
// et si le nom de l'operation est succ ou pred
int T_op_result::
	syntax_check_is_an_arith_expr(T_list_ident **list_ident,
										T_B0_context context)
{
  if ((ref_builtin != NULL) &&
	  (nb_in_params == 1) &&
	  ((ref_builtin->get_lex_type() == L_SUCC) ||
	   (ref_builtin->get_lex_type() == L_PRED)))
	{
	  if (first_in_param->syntax_check_is_an_arith_expr(list_ident,
															  context) == TRUE)
		{
		  return TRUE;
		}
	  else
		{
		  // Erreur B0 : on attend une expression arithmetique
		  if (get_check() != B0CHK_INIT) // si on ne fait pas une 2eme passe pour RINIT 1-1
			{
			  B0_syntax_error(first_in_param,
							  CAN_CONTINUE,
							  get_error_msg(E_B0_OPERAND_MUST_BE_AN_ARITH_EXPRESSION),
							  make_class_name(first_in_param),
							  make_class_name(this)) ;
			}
		  return FALSE ;
		}
	}
  else
	{
	  return FALSE ;
	}

}

// T_op_result : c'est une expresion booleenne  si le parametre unique
// est une condition et si le nom de l'operation est bool
// SL 23/09/99 suite discussion avec FB au sujet d'un exemple ALSTOM,
// bool(cond) est bien un terme simple
// (pour accepter bool(not(bool(...)))
int T_op_result::syntax_check_is_a_simple_term(T_list_ident **list_ident,
														T_B0_context context)
{
  if ((ref_builtin != NULL) &&
	  (ref_builtin->get_lex_type() == L_BOOL) &&
	  (nb_in_params ==1))
	{
	  if (get_B0_disable_predicate_syntax_check() == TRUE) {return TRUE ;};
	  if (first_in_param->syntax_check_is_a_B0_predicate(list_ident) == TRUE)
		{
		  return TRUE;
		}
	  else
		{

		  return FALSE ;
		}
	}
  else
	{
	  return FALSE ;
	}

}


//
// Fonction de vérification : est-ce une expression B0 ?
//
int T_item::syntax_check_is_a_B0_expression(T_list_ident **list_ident)
{
   TRACE2("T_item(%x, %s)::syntax_check_is_a_B0_expression() -> FALSE",
		 this,
		 get_class_name()) ;
  return ( (syntax_check_is_a_term(list_ident) == TRUE) ||
		   (syntax_check_is_an_array(list_ident) == TRUE) ||
		   (syntax_check_is_a_range(list_ident) == TRUE) );
}



//
// Est-ce un predicat B0 (une condition)
//
// Fonction de vérification : est-ce un predicat B0 ?
// CTRL Ref : CSYN 7-1 a CSYN 7-8
// CTRL Ref : CSYN 8-1 a CSYN 8-6
int T_item::syntax_check_is_a_B0_predicate(T_list_ident **list_ident)
{
   TRACE2("T_item(%x, %s)::syntax_check_is_a_B0_predicate() -> FALSE",
		 this,
		 get_class_name()) ;
   // Erreur B0 : on attend une expression booleenne
   if (get_check() != B0CHK_INIT)
	 // si on ne fait pas une 2eme passe pour RINIT 1-1
	 {
	   B0_syntax_error(this,
					   CAN_CONTINUE,
					   get_error_msg(E_B0_PREDICATE_NOT_ALLOWED_FOR_CONDITION),
					   make_class_name(this)) ;
	 }
   return FALSE ;
}


int T_typing_predicate::
	syntax_check_is_a_B0_predicate(T_list_ident **list_ident)
{
   TRACE2("T_typing_predicate(%x, %s)::syntax_check_is_a_B0_predicate()",
		 this,
		 get_class_name()) ;

   switch (typing_predicate_type)
	{
	case TPRED_INCLUDED :
	case TPRED_STRICT_INCLUDED :
	case TPRED_BELONGS :
	  {
		if (get_check() != B0CHK_INIT) // si on ne fait pas une 2eme
 // passe pour RINIT 1-1
		  {
			// ces cas ne correspondent pas a des conditions
			// Erreur
			B0_syntax_error(this,
							CAN_CONTINUE,
							get_error_msg(E_B0_PREDICATE_NOT_ALLOWED_FOR_CONDITION),
							make_class_name(this)) ;
		  }
		return FALSE;
	  }
	case TPRED_EQUAL :

	  {
		if (first_ident != last_ident)
		  {
			B0_semantic_error(this,
							  CAN_CONTINUE,
							  get_error_msg(E_B0_PREDICATE_NOT_ALLOWED_FOR_CONDITION),
							  make_class_name(this));
		  }

		int cur_op1 = first_ident->syntax_check_is_a_simple_term(list_ident);
		int cur_op2 = type->syntax_check_is_a_simple_term(list_ident);
		if ( (cur_op1 == TRUE) &&
			 (cur_op2 == TRUE) )
		  {
			// CTRL Ref : RSEMPRED 1-1
			// (Controles de compatibilite des tableaux)
			//
			if (get_B0_disable_array_compatibility_check() == TRUE)
			  {return TRUE ;};

			if (get_check() != B0CHK_INIT)
			  //si on ne fait pas une 2e passe pour RINIT 1-1
			  {
				if (    (first_ident == NULL)
					 || (first_ident->make_expr() == NULL)
					 || (first_ident->make_expr()->get_original_B_type() == NULL)
				     || (type == NULL)
					 || (type->make_expr() == NULL)
					 || (type->make_expr()->get_original_B_type() == NULL)
						)
				  {
					// Reprise sur erreur
					TRACE0("reprise sur erreur") ;
					return FALSE ;
				  }

				TRACE2("ici first_ident %x type %x", first_ident, type) ;
				if ( ( (first_ident->make_expr()->get_original_B_type()
						->is_an_array()  == TRUE) ||
					   (first_ident->make_expr()->get_original_B_type()
						->is_a_record() == TRUE) ) &&
					 ( (type->make_expr()->get_original_B_type()
						->is_an_array()  == TRUE) ||
					                   (type->make_expr()->get_original_B_type()
						->is_a_record() == TRUE) ) )
				  {
					// Il y a un tableau ; on verifie la compatibilite
					T_type *left_type = first_ident->make_expr()
					  ->get_original_B_type();
					T_type *right_type = type->make_expr()
					  ->get_original_B_type();

					if (left_type->is_array_compatible_with(right_type,
															type) == FALSE)
					  {
						B0_semantic_error(this,
										  CAN_CONTINUE,
										  get_error_msg(E_B0_INCOMPATIBLE_ARRAYS_BETWEEN_LHS_AND_RHS));
					  }
				  }
			  }

			return TRUE;
		  }
		else
		  {
			// ce n'est pas une condition
			// Erreur : les operandes doivent etre des termes simples
			if (cur_op1 == FALSE)
			  {
				if (get_check() != B0CHK_INIT) //si on ne fait pas une 2e passe pour RINIT 1-1
				  {
					B0_syntax_error(first_ident,
									CAN_CONTINUE,
									get_error_msg(E_B0_OPERAND_IS_NOT_A_SIMPLE_TERM),
									make_class_name(first_ident)) ;
				  }
			  }
			if (cur_op2 == FALSE)
			  {
				if (get_check() != B0CHK_INIT) //si on ne fait pas une 2e passe pour RINIT 1-1
				  {
					B0_syntax_error(type,
									CAN_CONTINUE,
									get_error_msg(E_B0_OPERAND_IS_NOT_A_SIMPLE_TERM),
									make_class_name(type)) ;
				  }
			  }
			return FALSE;
		  }
	  }

	  // On ne peut pas passer a cet endroit (cas non prevu)
	  // On ne met pas de default pour etre prevenu par un warning

	}
   // Pour le warning
   ASSERT(FALSE);
   return FALSE;
}


int T_expr_predicate::
	syntax_check_is_a_B0_predicate(T_list_ident **list_ident)
{
  switch (predicate_type)
	{
	case EPRED_NOT_INCLUDED :
	case EPRED_NOT_STRICT_INCLUDED :
	case EPRED_NOT_BELONGS :
	  {
		if (get_check() != B0CHK_INIT) // si on ne fait pas une 2eme passe pour RINIT 1-1
		  {
			// Erreur : ce n'est pas un predicat valide pour une condition
			B0_syntax_error(this,
							CAN_CONTINUE,
							get_error_msg(E_B0_PREDICATE_NOT_ALLOWED_FOR_CONDITION),
							make_class_name(this)) ;
		  }
		return FALSE;
	  }
	case EPRED_DIFFERENT :
	case EPRED_LESS_THAN :
	case EPRED_GREATER_THAN :
	case EPRED_LESS_THAN_OR_EQUAL :
    case EPRED_GREATER_THAN_OR_EQUAL :
	  {
		int cur_expr1 = expr1->syntax_check_is_a_simple_term(list_ident);
		int cur_expr2 = expr2->syntax_check_is_a_simple_term(list_ident);
		if ((cur_expr1 == TRUE)&&
			(cur_expr2 == TRUE))
		  {
			//
			// CTRL Ref : RSEMPRED 1-1
			// (Controles de compatibilite des tableaux)
			//
			if (get_B0_disable_array_compatibility_check() == TRUE)
			  {return TRUE ;};

			if (get_check() != B0CHK_INIT)
			  //si on ne fait pas une 2e passe pour RINIT 1-1
			  {
				T_type *left_type = expr1->make_expr()
					  ->get_original_B_type();
				T_type *right_type = expr2->make_expr()
					  ->get_original_B_type();
				if ( (predicate_type == EPRED_DIFFERENT) &&
					 (left_type != NULL) &&
					 (right_type != NULL) &&
					 ( ( (left_type->is_an_array()  == TRUE) ||
						 (left_type->is_a_record() == TRUE) )  &&
					   ( (right_type->is_an_array()  == TRUE) ||
						 (right_type->is_a_record() == TRUE) ) ) )
				  {
					// Il y a un tableau ; on verifie la compatibilite

					if (left_type->is_array_compatible_with(right_type,
															expr2) == FALSE)
					  {
						B0_semantic_error(this,
										  CAN_CONTINUE,
										  get_error_msg(E_B0_INCOMPATIBLE_ARRAYS_BETWEEN_LHS_AND_RHS));
					  }
				  }
			  }

			return TRUE;
		  }
		else
		  {
			// ce n'est pas une condition
			// Erreur : les operandes doivent etre des termes simples
			if (cur_expr1 == FALSE)
			  {
				if (get_check() != B0CHK_INIT) //si on ne fait pas une 2e passe pour RINIT 1-1
				  {
					B0_syntax_error(expr1,
									CAN_CONTINUE,
									get_error_msg(E_B0_OPERAND_IS_NOT_A_SIMPLE_TERM),
									make_class_name(expr1)) ;
				  }
			  }
			if (cur_expr2 == FALSE)
			  {
				if (get_check() != B0CHK_INIT) //si on ne fait pas une 2e passe pour RINIT 1-1
				  {
					B0_syntax_error(expr2,
									CAN_CONTINUE,
									get_error_msg(E_B0_OPERAND_IS_NOT_A_SIMPLE_TERM),
									make_class_name(expr2)) ;
				  }
			  }
			return FALSE;
		  }
	  }

	  // On ne peut pas passer a cet endroit (cas non prevu)
	  // On ne met pas de default pour avoir un warning le cas echeant

	}
  // Pour le warning
  ASSERT(FALSE);
  return FALSE;
}

int T_not_predicate::
	syntax_check_is_a_B0_predicate(T_list_ident **list_ident)
{
  if (predicate->syntax_check_is_a_B0_predicate(list_ident))
	{
	  return TRUE;
	}
  else
	{
	  // Erreur : predicate doit etre une condition

	  return FALSE;
	}
}

int T_predicate_with_parenthesis::
	syntax_check_is_a_B0_predicate(T_list_ident **list_ident)
{
  if (predicate->syntax_check_is_a_B0_predicate(list_ident))
	{
	  return TRUE;
	}
  else
	{
	  // Erreur : predicate doit etre une condition

	  return FALSE;
	}
}


int T_binary_predicate::
	syntax_check_is_a_B0_predicate(T_list_ident **list_ident)
{
  switch (predicate_type)
	{
	case BPRED_IMPLIES :
	case BPRED_EQUIVALENT :
	  {
		if (get_check() != B0CHK_INIT) // si on ne fait pas une 2eme passe pour RINIT 1-1
		  {
			// Erreur : ce predicat n'est pas valide pour etre une condition
			B0_syntax_error(this,
							CAN_CONTINUE,
							get_error_msg(E_B0_PREDICATE_NOT_ALLOWED_FOR_CONDITION),
							make_class_name(this)) ;
		  }
		return FALSE;
	  }
	case BPRED_OR :
	case BPRED_CONJONCTION :
	  {
		int cur_res1 = pred1->syntax_check_is_a_B0_predicate(list_ident);
		int cur_res2 = pred2->syntax_check_is_a_B0_predicate(list_ident);
		if ( (cur_res1 == TRUE) &&
			 (cur_res2 == TRUE) )
		  {
			return TRUE;
		  }
		else
		  {
			// Erreur : les operandes doivent etre des conditions

			return FALSE;
		  }
	  }
	  // Pas de default pour avoir un warning (cas non prevu)

	}
  // Pour le warning
  ASSERT(FALSE);
  return FALSE;
}





//
// Est-ce un ensemble simple ?
//
int T_item::syntax_check_is_a_simple_set(void)
{

  return ( (check_is_BOOL() == TRUE) ||
           (check_is_FLOAT() == TRUE) ||
                              (syntax_check_is_a_range(NULL)));
}


//
// Est-ce un ensemble de fonctions totales ?
//
int T_item::syntax_check_is_a_total_function_set(void)
{
  return FALSE;
}

int T_relation::syntax_check_is_a_total_function_set(void)
{
  switch (relation_type)
	{
	case RTYPE_TOTAL_FUNCTION :
	case RTYPE_TOTAL_INJECTION :
	case RTYPE_TOTAL_SURJECTION :
	case RTYPE_BIJECTION :
	  {
		int result = TRUE;
		if (dst_set->syntax_check_is_a_simple_set() == TRUE)
		  {
			// OK, c'est un ensemble simple.
		  }
		else if (get_B0_allow_arrays_of_arrays() == TRUE &&
				 dst_set->syntax_check_is_a_total_function_set() == TRUE)
		  {
			// OK, en mode tableaux de tableaux, l'ensemble d'arrivée
			// peut aussi être un tableau.
		  }
		else
		  {
			// Erreur B0 : l'ensemble de destination doit etre simple
			B0_semantic_error(dst_set,
							  CAN_CONTINUE,
							  get_error_msg(E_B0_INVALID_DEST_SET_FOR_TOTAL_FUNCTION),
							  make_class_name(dst_set)) ;
			result = FALSE;
		  }

		// On verifie que les ensembles sources sont simples
		src_set->syntax_check_is_a_total_function_source_set() ;
		return result;
		break;
	  }
	case RTYPE_PARTIAL_FUNCTION :
	case RTYPE_PARTIAL_INJECTION :
	case RTYPE_PARTIAL_SURJECTION :
	  {
		// ce ne sont pas des fonctions totales
		B0_semantic_error(this,
						  CAN_CONTINUE,
						  get_error_msg(E_B0_INVALID_RELATION_FOR_CONCRETE_TYPE)) ;
		return FALSE;
		break;
	  }

	  // pas de warning pour etre prevenu par un warning en cas de cas
	  // non prevu
	}
  // Pour le warning
  ASSERT(FALSE);
  return FALSE;
}


// On verifie que l'expression est un produit cartesien d'ensembles simples
// et est donc valide pour etre l'ensemble source d'une fonction totale
void T_expr::syntax_check_is_a_total_function_source_set(void)
{
  if (syntax_check_is_a_simple_set() == FALSE)
	{
	  // Erreur : l'ensemble source d'une fonction totale doit etre
	  // un produit cartesien d'ensembles simples
	  B0_semantic_error(this,
						CAN_CONTINUE,
						get_error_msg(E_B0_INVALID_SRC_SET_FOR_TOTAL_FUNCTION),
						make_class_name(this)) ;
	}
}


void T_expr_with_parenthesis::
	syntax_check_is_a_total_function_source_set(void)
{
  ASSERT(expr != NULL);
  expr->syntax_check_is_a_total_function_source_set();
}

void T_binary_op::syntax_check_is_a_total_function_source_set(void)
{

  // On recupere le premier element du produit cartesien, si c'en est un
  // sinon on recupere this...
  T_item * cur_item = get_expr(TRUE);

  // On parcourt les elements et on verifie que ce sont des ensembles simples
  while (cur_item != NULL)
	{
	  if (cur_item->syntax_check_is_a_simple_set() == FALSE)
		{
		  // Erreur, on attend un ensemble simple!
		  B0_semantic_error(this,
							CAN_CONTINUE,
							get_error_msg(E_B0_INVALID_SIMPLE_SET),
							make_class_name(this)) ;
		}

	  cur_item = cur_item->get_next_expr(TRUE);
	}
}


//
// Est-ce l'expression "BOOL"
//
int T_item::check_is_BOOL(void)
{
  return FALSE;
}

int T_list_link::check_is_BOOL(void)
{
  return ((T_item *)object)->check_is_BOOL();
}

int T_ident::check_is_BOOL(void)
{
  if ( (ident_type == ITYPE_BUILTIN) &&
	   (name == get_builtin_BOOL()->get_name()) )
	{
	  return TRUE;
	}
  return FALSE;
}

//
// Est-ce l'expression "FLOAT"
//
int T_item::check_is_FLOAT(void)
{
  return FALSE;
}

int T_list_link::check_is_FLOAT(void)
{
  return ((T_item *)object)->check_is_FLOAT();
}

int T_ident::check_is_FLOAT(void)
{
  if ( (ident_type == ITYPE_BUILTIN) &&
           (name == get_builtin_FLOAT()->get_name()) )
        {
          return TRUE;
        }
  return FALSE;
}

//
// Est-ce l'expression "STRING"
//
int T_item::check_is_STRING(void)
{
  return FALSE;
}

int T_list_link::check_is_STRING(void)
{
  return ((T_item *)object)->check_is_STRING();
}

int T_ident::check_is_STRING(void)
{
  if ( (ident_type == ITYPE_BUILTIN) &&
	   (name == get_builtin_STRING()->get_name()) )
	{
	  return TRUE;
	}
  return FALSE;
}

//
// Est-ce l'expression "NAT"
//
int T_item::check_is_NATURAL(void)
{
  return FALSE;
}

int T_list_link::check_is_NATURAL(void)
{
  return ((T_item *)object)->check_is_NATURAL();
}

int T_ident::check_is_NATURAL(void)
{
  if ( (ident_type == ITYPE_BUILTIN) &&
       (name == get_builtin_NAT()->get_name()) )
    {
      return TRUE;
    }
  return FALSE;
}
//
//
// Est-ce un ensemble de record (struct conforme au B0)
//
int T_item::syntax_check_is_a_record_set(void)
{
  return FALSE;
}

int T_struct::syntax_check_is_a_record_set(void)
{
  // On verifie le type de ces record items
  return ((syntax_check_list_is_a_record_item_type(first_record_item)) ? TRUE
		  : FALSE);
}


//
//
// Est-ce un ensemble en extension de termes simples pour
// typage_appartenance_donnee_concrete
//
int T_item::syntax_check_is_a_simple_term_set(void)
{
  return FALSE;
}

int T_extensive_set::syntax_check_is_a_simple_term_set(void)
{
  // On verifie que les elements sont des termes simples
  // et en plus (context = 1) des scalaires
  return (syntax_check_list_is_a_simple_term(first_item,
											 B0CTX_ARRAY_CONTEXT) ? TRUE : FALSE);
}


//
// Autres verifications
//

// On verifie que le Betree ne modelise pas une implementation
// Emission d'un message d'erreur si ce n'est pas le cas
// disant que item est interdit en implementation
void T_betree::check_is_not_an_implementation(T_item *item)
{
  TRACE3("T_betree(%x,%s)::check_is_an_not_implementation(%x)",
		 this,
		 get_betree_name()->get_value(),
		 item) ;

  if (root->get_machine_type() == MTYPE_IMPLEMENTATION)
	{
	  //	  ASSERT(cur_item->get_father() != NULL);

	  // Erreur
	  syntax_error(item->get_ref_lexem(),
	  			   CAN_CONTINUE,
	  			   get_error_msg(E_ITEM_IS_NOT_ALLOWED_IN_AN_IMPLEMENTATION),
				   make_class_name(item)) ;
	}
}



//
// Est-ce une implementation ?
//
// On teste si le betree modelise une implementation
// renvoie TRUE dans ce cas, FALSE sinon.
int T_betree::check_is_an_implementation()
{
  TRACE2("T_betree(%x,%s)::check_is_an_implementation()",
		 this,
		 get_betree_name()->get_value()) ;

  return (get_root()->get_machine_type() == MTYPE_IMPLEMENTATION) ? TRUE
	: FALSE;
}

// On teste si l'item est defini dans une implementation
// renvoie TRUE dans ce cas, FALSE sinon.
int T_item::check_is_in_an_implementation()
{
  TRACE1("T_item(%x)::check_is_in_an_implementation()",
		 this) ;

  ASSERT(get_betree() != NULL);

  return get_betree()->check_is_an_implementation();
}


// On teste si la substitution apparaît dans un contexte
// d'implantation (i.e. dans du code concret).
// Renvoie TRUE dans ce cas, FALSE sinon.
//
// Pour cela, on remonte jusqu'à la clause dans laquelle apparaît la
// substitution.

int T_substitution::check_is_in_an_implementation()
{
  TRACE1("T_substitution(%x)::check_is_in_an_implementation()",
		 this) ;

  // Si on est pas dans une implantation, la question ne se pose pas.
  if (T_item::check_is_in_an_implementation() == FALSE)
	{
	  return FALSE;
	}

  // Dans une implantation, il faut regarder si on est dans une
  // spécification d'opération locale.
  T_item *cur = get_father();
  while (cur != NULL)
	{
	  switch (cur->get_node_type())
	  {
	  case NODE_FLAG:
		{
		  T_lexem *lexem = cur->get_ref_lexem() ;
		  if (lexem != NULL && lexem->get_lex_type() == L_INITIALISATION)
			{
			  return TRUE;
			}
		  else
			{
			  // On passe au suivant, on doit être sur un flag
			  // intermédiaire représentant par exemple un THEN.
			}
		  break;
		}
	  case NODE_OPERATION:
		{
		  T_op *op = (T_op *) cur;
		  return op->get_is_a_local_op() == FALSE;
		  break;
		}
	  default:
		{
		  // On ne fait rien, on continue la remontée
		  break;
		}
	  }
	  cur = cur->get_father();
	}
  // On doit avoir trouvé une clause !
  ASSERT(cur != NULL);
  return TRUE;
}



//
//
// Test si l'identificateur figure dans la liste des identificateurs a traiter
// (liste globale current_list_ident) et si il n'est pas encore traite (ie
// value ou initialise suivant la valeur du flag global check)
// Dans ce cas il y a une erreur
//
void T_ident::check_cycle(T_list_ident **list_ident)
{
  switch (get_check())
	{
	case B0CHK_INIT :
	  {
		// Si on teste les parties droites d'initialisation
		//
		// CTRL Ref : RINIT 1-1
		if ( (get_current_list_ident()->find_identifier(this) == TRUE) &&
			 ((*list_ident)->find_identifier(this) == FALSE) )
		  {
			// ERREUR B0 : Un identificateur non encore value figure en
			// partie droite d'une affectation
			B0_semantic_warning(this,
								BASE_WARNING,
								get_warning_msg(W_B0_DETECTED_CYCLE_IN_INITIALISATION),
								this->B0_get_definition()->get_ident_type_name(),
								this->get_name()->get_value()) ;
		  }
		break;
	  }
	case B0CHK_VAL :
	  {
		// Si on teste les parties droites de valuation
		//
		// CTRL Ref : RVAL 0-2
		// CTRL Ref : RVAL 4-1
		if ( (get_current_list_ident()->find_identifier(this) == TRUE) &&
			 ((*list_ident)->find_identifier(this) == FALSE) )
		  {
			// ERREUR B0 : Un identificateur non encore value figure en
			// partie droite d'une valuation
			B0_semantic_warning(this,
								BASE_WARNING ,
								get_warning_msg(W_B0_DETECTED_CYCLE_IN_VALUATION),
								this->B0_get_definition()->get_ident_type_name(),
								this->get_name()->get_value()) ;
		  }
		break;
	  }
	default :
	  {
		// Aucun traitement particulier pour un default
	  }
	}
}


// Suivant la valeur du parametre d'entree (TRUE ou FALSE),
// On teste si le premier record d'un acces (eventuellement recursif) a un
// record est un T_ident (parametre vaut TRUE) ou un record en extension.
// (parametre vaut FALSE) (fonction recursive)
// Exemple : si parametre vaut TRUE : Ident_ren('ident) est valide
// (la fonction rend TRUE)
//   Mais    Terme_record('Ident) n'est pas valide (la fonction rend FALSE)
//
// le parametre context est utilise dans le cadre de la verification
// RTYPETAB 1-4 : si il vaut 1, la verification est effectue
int T_record_access::check_first_record(int first_record,
												 T_list_ident **list_ident,
												 T_B0_context context)
{
  if (record->get_node_type() == NODE_RECORD_ACCESS)
	{
	  // cast valide par condition du IF
	  return ((T_record_access *) record)->check_first_record(first_record,
															  list_ident,
															  context);
	}
  else
	{
	  if (first_record == TRUE)
		{
		  if ( (context == B0CTX_ARRAY_CONTEXT) &&
			   (record->is_an_ident() == TRUE) )
			{
			  ((T_ident *)record->make_expr())->check_allowed_for_array_index();
			}
		  return record->is_an_ident();
		}
	  else
		{
		  return record->syntax_check_is_an_extension_record(list_ident);
		}
	}

}





// Dans le cas d'une instance de T_binary_op representant un operateur operat,
// (BOP_TIMES) ou (BOP_MAPLET),
// trouve recursivement le premier element
T_item * T_binary_op::get_expr(int operat)
{
  T_item * lop1 = op1;

  while (lop1 != NULL &&
		 lop1->get_node_type() == NODE_EXPR_WITH_PARENTHESIS)
	{
	  lop1 = ((T_expr_with_parenthesis *)lop1)->get_expr();
	}

  if ( (lop1->get_node_type() == NODE_BINARY_OP) &&
	   ( (oper == BOP_MAPLET) ||
		 (oper == BOP_TIMES) ||
		 (oper == BOP_COMMA) ) )
	{
	  if ( (operat == TRUE) &&
		   ( (((T_binary_op *)lop1)->get_operator() != BOP_TIMES) &&
			 (((T_binary_op *)lop1)->get_operator() != BOP_COMMA) ) )
		{
		  return lop1;
		}

	  if ( (operat == FALSE) &&
		   (((T_binary_op *)lop1)->get_operator() != BOP_MAPLET) )
		{
		  return lop1;
		}

	  return ((T_binary_op *)lop1)->get_expr(operat);

	}

  if ( (operat == TRUE) &&
	   ( (oper == BOP_TIMES) ||
		 (oper == BOP_COMMA) ) )
	{
	  return lop1;
	}

  if ( (operat == FALSE) &&
	   (oper = BOP_MAPLET) )
	{
	  return lop1;
	}

  return this;
}

T_item * T_item::get_expr(int operat)
{
  return this;
}

T_item * T_expr_with_parenthesis::get_expr(int operat)
{
  return expr->get_expr(operat);
}

// Dans le cas d'une instance de T_binary_op representant un operateur operat
// (BOP_TIMES) ou (BOP_MAPLET),
// trouve l'element suivant
T_item * T_item::get_next_expr(int operat)
{
  T_item *father = get_father();
  T_item *child = this;

  while (father != NULL &&
		 father->get_node_type() == NODE_EXPR_WITH_PARENTHESIS)
	{
	  child = father;
	  father = father->get_father();
	}

  if ((father == NULL) ||
	  (father->get_node_type() != NODE_BINARY_OP))
	{
	  return NULL;
	}
  if (father->get_node_type() == NODE_BINARY_OP)
	{
	  T_binary_op * bin_op = (T_binary_op *)father;
	  if (operat == TRUE)
		{
		  if ( (bin_op->get_operator() != BOP_TIMES) &&
			   (bin_op->get_operator() != BOP_COMMA) )
			{
			  return NULL;
			}
		}

	  if (operat == FALSE)
		{
		  if (bin_op->get_operator() != BOP_MAPLET)
			{
			  return NULL;
			}
		}

	  if (bin_op->get_op1() == child)
		{
		  // On est sur le fils gauche, on renvoie le droit
		  return bin_op->get_op2();
		}
	  // on est sur le fils droit, on demande au pere (back-tracking)
	  return ((T_item *)father)->get_next_expr(operat);

	}
  return NULL;
}


// Parcours de tous les get_def jusqu'a avoir la definition, sans s'occuper
// du collage eventuelle (on s'arrete des que ident_type <> ITYPE_UNKNOWN
T_ident *T_ident::B0_get_definition(void)
{
  T_ident *cur = this ;

  for (;;)
	{
	  T_ident *defi = cur->get_def() ;

	  if ( (defi != NULL) &&
		   (cur->get_ident_type() == ITYPE_UNKNOWN ) )
		{
		  cur = defi ;
		}
	  else
		{
		  return cur ;
		}
	}
}

//
// Fin du fichier
//

#endif // B0C

