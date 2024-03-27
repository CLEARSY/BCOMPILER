/******************************* CLEARSY **************************************
* Fichier : $Id: i_typchk.cpp,v 2.0 2004-01-08 09:33:01 cm Exp $
*
* (C)2008 CLEARSY
*
* Description :		B0 Checker, Fonctions de verification de types B0
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
RCS_ID("$Id: i_typchk.cpp,v 1.23 2004-01-08 09:33:01 cm Exp $") ;

/* Includes systeme */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

/* Includes Composant Local */

#include "c_api.h"
#include "i_aux.h"
#include "i_subst.h"


#ifdef B0C


//////////////////////////////////////////////////////////////////////////////
//
// Verification de types
//
//
//////////////////////////////////////////////////////////////////////////////

//
// Est-ce une chaine de caracteres
//
int T_type::is_a_string(void)
{
  return FALSE;
}

int T_predefined_type::is_a_string(void)
{
  return ((type == BTYPE_STRING) ? TRUE : FALSE);
}


//
// Est-ce un entier concret
//
int T_type::is_a_concrete_integer(void)
{
  return FALSE;
}
int T_type::is_a_concrete_boolean(void)
{
  return FALSE;
}

int T_predefined_type::is_a_concrete_integer(void)
{
  // Si le pere est un set_of, il faut verifier que le type est borne
  // pour pouvoir etre implementable
  if ( (get_father() != NULL) &&
	   (get_father()->get_node_type() == NODE_SETOF_TYPE) )

	{
	  return (( (type == BTYPE_INTEGER) &&
				(is_bounded() == TRUE) ) ? TRUE
			  : FALSE);
	}
  return ( (type == BTYPE_INTEGER) ? TRUE : FALSE );
}

int T_predefined_type::is_a_concrete_boolean(void)
{
  return ( (type == BTYPE_BOOL) ? TRUE : FALSE );
}


//
// Est-ce un booleen
//
int T_type::is_a_boolean(void)
{
  return FALSE;
}

int T_predefined_type::is_a_boolean(void)
{
  return ((type == BTYPE_BOOL) ? TRUE : FALSE);
}


//
// Est-ce un element d'ensemble abstrait
//
int T_type::is_an_abstract_element(void)
{
  return FALSE;
}

int T_abstract_type::is_an_abstract_element(void)
{
  return TRUE;
}

//
// Est-ce un element d'un ensemble abstrait ou enumere
//
int T_type::is_an_abstract_or_enumerated_set_element(void)
{
  return FALSE;
}

int T_enumerated_type::is_an_abstract_or_enumerated_set_element(void)
{
  return TRUE;
}

int T_abstract_type::is_an_abstract_or_enumerated_set_element(void)
{
  return TRUE;
}


//
// Est-ce un ensemble abstrait ou enumere
//
int T_type::is_an_abstract_or_enumerated_set(void)
{
  return FALSE;
}

int T_setof_type::is_an_abstract_or_enumerated_set(void)
{
  return ((base_type->is_an_abstract_or_enumerated_set_element()) ? TRUE
	: FALSE);
}


//
// Est-ce un intervalle d'entiers ou abstrait
//
int T_type::is_an_integer_or_abstract_interval(void)
{
  return FALSE;
}

int T_setof_type::is_an_integer_or_abstract_interval(void)
{
  return ( ( (base_type->is_a_concrete_integer()) ||
			 (base_type->is_a_concrete_boolean()) || // ajout SL 30/08/1999
			 (base_type->is_an_abstract_element()) ) ? TRUE
		   : FALSE);
}


//
// Est-ce un type tableau
//
int T_type::is_an_array(void)
{
  return FALSE;
}

int T_setof_type::is_an_array(void)
{
  // Il faut que le type de base soit un T_product_type
  if (base_type->get_node_type() != NODE_PRODUCT_TYPE)
	{
	  return FALSE ;
	}

  // On est en presence d'un tableau : Est-ce un tableau B0 ?
  // (Le cast est garanti par le IF precedent)
  T_type *cur_type = ((T_product_type *)base_type)->get_spec_types();
  int result = TRUE;
  while (cur_type != NULL && result == TRUE)
	{
	  // Les types des elements des ensembles index et de l'ensemble d'arrivee
	  // doivent etre des types d'element d'ensembles simples
	  // CTRL Ref : RTYPETAB 1-1
	  // CTRL Ref : RTYPETAB 1-2 (par compatibilite des types a l'analyse
	  // semantique
	  // CTRL Ref : RTYPETAB 1-5
	  if (cur_type->is_a_simple_set_element() == TRUE)
		{
		  // OK, c'est ce qu'on veut.
		}
	  else if (get_B0_allow_arrays_of_arrays() == TRUE &&
			   cur_type->get_next_spec_type() == NULL &&
			   cur_type->is_an_array() == TRUE)
		{
		  // OK, si on autorise les tableaux de tableaux, l'ensemble
		  // d'arrivée peut lui-même être un tableau.

		  // Nota: C'est le deuxième prédicat qui vérifie que c'est
		  // l'ensemble d'arrivée.
		}
	  else
		{
		  // Erreur B0
		  result = FALSE;
		}

	  // On passe au type suivant
	  cur_type = cur_type->get_next_spec_type();
	}
  return result;

}


//
// Est-ce un type record
//
int T_type::is_a_record(void)
{
  return FALSE;
}

int T_record_type::is_a_record(void)
{
  return ( (type_check_list_is_a_label_type(first_label) == TRUE) ? TRUE
		   : FALSE ) ;
}


//
// Est-ce un type d'element d'ensemble simple (non dans le cas general)
//
int T_type::is_a_simple_set_element(void)
{
  return ( ( (is_a_concrete_integer() == TRUE) ||
			 (is_a_boolean() == TRUE) ||
			 (is_an_abstract_or_enumerated_set_element() == TRUE) ) ? TRUE
		   : FALSE);
}




//
// Fonctions virtuelles de verification des types de chaque donnee concrete
//

// Type d'une constante concrete
// CTRL Ref : RTYPE 2-1-1 a RTYPE 2-1-6
int T_type::is_concrete_constant_type(void)
{
  return ( ( (is_concrete_variable_type() == TRUE) ||
			 (is_an_integer_or_abstract_interval() == TRUE) ) ? TRUE
		   : FALSE );
}

// Type d'une variable concrete
// CTRL Ref : RTYPE 3-1-1 a RTYPE 3-1-5
int T_type::is_concrete_variable_type(void)
{
  return ( ( (is_a_concrete_integer() == TRUE) ||
             (is_a_float() == TRUE) ||
			 (is_a_boolean() == TRUE) ||
			 (is_an_abstract_or_enumerated_set_element() == TRUE) ||
   			 (is_an_array() == TRUE) ||
			 (is_a_record() == TRUE) ) ? TRUE
		   : FALSE );
}

// Type d'un parametre scalaire de machine
// CTRL Ref : RTYPE 1-1-1 a RTYPE 1-1-5
int T_type::is_scalare_parameter_type(void)
{
  return ( ( (is_a_concrete_integer() == TRUE) ||
             (is_a_float() == TRUE) ||
			 (is_a_boolean() == TRUE) ||
			 (is_an_abstract_or_enumerated_set_element() == TRUE) ||

			 // cas d'un parametre scalaire de type ensemble
			 (is_an_integer_or_abstract_interval() == TRUE) ||
			 (is_an_abstract_or_enumerated_set() == TRUE) ||
   			 (is_an_array() == TRUE) ) ? TRUE
		   : FALSE );
}

// Type d'un parametre d'entree d'operation
// CTRL Ref : RTYPE 4-1-1 a RTYPE 4-1-6
int T_type::is_entry_parameter_type(void)
{
  return ( ( (is_concrete_variable_type() == TRUE) ||
			 (is_a_string() == TRUE) ) ? TRUE
		   : FALSE );
}

// Type d'un parametre de sortie d'operation
// CTRL Ref : RTYPE 5-1-1 a RTYPE 5-1-5
int T_type::is_exit_parameter_type(void)
{
  return ( (is_concrete_variable_type() == TRUE) ? TRUE
		   : FALSE );
}

// Type d'une variable locale
// CTRL Ref : RTYPE 5-1-1 a RTYPE 5-1-5 (suite)
int T_type::is_local_variable_type(void)
{
  return ( (is_concrete_variable_type() == TRUE) ? TRUE
		   : FALSE );
}

//
// Controle du typage des constantes concretes
//


// Vérifier la validité du type de IDENT pour le B0.  B0_DEF est la
// définition au sens B0 de IDENT.
static void internal_check_ident_type(T_expr *ident,
											   T_ident *B0_def)
{
  TRACE4("internal_check_ident_type(%p::%s, %p '%s')",
		 ident, ident->get_class_name(),
		 B0_def, B0_def->get_name()->get_value()) ;

  T_type *type = B0_def->get_original_B_type();
  T_error_code error_code = (T_error_code) 0;
  if (type != NULL)
	{
	  switch (B0_def->get_ident_type())
		{
		case ITYPE_CONCRETE_VARIABLE :
		  {
			if (type->is_concrete_variable_type() == FALSE)
			  {
				error_code = E_B0_INVALID_TYPE ;
			  }
			// En mode B0_enable_typing_identifiers, il ne faut pas
			// afficher d'erreur si la constante est un identificateur
			// de typage.
			if (get_B0_enable_typing_identifiers() == TRUE &&
				B0_def->get_B_type() != NULL &&
				B0_def->get_B_type()->get_typing_ident() != NULL)
			  {
				error_code = (T_error_code) 0;
			  }
			break ;
		  }

		case ITYPE_CONCRETE_CONSTANT :
		  {
			if (type->is_concrete_constant_type() == FALSE)
			  {
				error_code = E_B0_INVALID_TYPE ;
			  }

			// En mode B0_enable_typing_identifiers, il ne faut pas
			// afficher d'erreur si la constante est un identificateur
			// de typage.
			if (get_B0_enable_typing_identifiers() == TRUE &&
				B0_def->get_B_type() != NULL &&
				B0_def->get_B_type()->get_typing_ident() != NULL)
			  {
				error_code = (T_error_code) 0;
			  }
			break ;
		  }

		case ITYPE_MACHINE_SCALAR_FORMAL_PARAM :
		  {
			if (type->is_scalare_parameter_type() == FALSE)
			  {
				error_code = E_B0_INVALID_TYPE ;
			  }
			// En mode B0_enable_typing_identifiers, il ne faut pas
			// afficher d'erreur si la constante est un identificateur
			// de typage.
			if (get_B0_enable_typing_identifiers() == TRUE &&
				B0_def->get_B_type() != NULL &&
				B0_def->get_B_type()->get_typing_ident() != NULL)
			  {
				error_code = (T_error_code) 0;
			  }
			break ;
		  }

		case ITYPE_OP_IN_PARAM :
		  {
			if (type->is_entry_parameter_type() == FALSE)
			  {
				error_code = E_B0_INVALID_TYPE ;
			  }
			// En mode B0_enable_typing_identifiers, il ne faut pas
			// afficher d'erreur si la constante est un identificateur
			// de typage.
			if (get_B0_enable_typing_identifiers() == TRUE &&
				B0_def->get_B_type() != NULL &&
				B0_def->get_B_type()->get_typing_ident() != NULL)
			  {
				error_code = (T_error_code) 0;
			  }
			break ;
		  }

		default :
		  {
			// Pas de verification
			error_code = (T_error_code) 0;
		  }
		}
	}

  if (error_code != (T_error_code) 0)
	{
	  B0_semantic_error(ident,
						CAN_CONTINUE,
						get_error_msg(error_code),
						B0_def->get_ident_type_name(),
						B0_def->get_name()->get_value()) ;
	}
}



// Fonction auxiliaire utilisée par la suivante: Vérifie que ITEM peut
// bien être utilisé en partie droite du prédicat de typage par
// appartenance de B0_DEF.
static void internal_check_is_belong_expr_right(T_item *item,
														 T_ident *B0_def)
{
  TRACE4("internal_check_is_belong_expr_right(%p::%s, %p '%s')",
		 item, item->get_class_name(),
		 B0_def, B0_def->get_name()->get_value()) ;

  T_error_code error_code = (T_error_code) 0;
  switch (B0_def->get_ident_type())
	{
	case ITYPE_CONCRETE_VARIABLE :
	  {
		if (item->is_belong_concrete_data_expr() == FALSE)
		  {
			//Erreur B0 : ce n'est pas une expression de typage pour
			//une variable concrete
			error_code = E_B0_INVALID_TYPING_EXPR_FOR_CONCRETE_VARIABLE;
		  }
		break ;
	  }

	case ITYPE_CONCRETE_CONSTANT :
	  {
		if (item->is_belong_concrete_data_expr() == FALSE)
		  {
			//Erreur B0 : ce n'est pas une expression de typage pour
			//une constante concrete
			error_code = E_B0_INVALID_TYPING_EXPR_FOR_CONCRETE_CONSTANT;
		  }
		break ;
	  }

	case ITYPE_MACHINE_SCALAR_FORMAL_PARAM :
	  {
		if (item->is_belong_scalar_formal_param_expr() == FALSE)
		  {
			//Erreur B0 : ce n'est pas une expression de typage pour
			//un parametre scalaire de machine
			error_code = E_B0_INVALID_TYPING_EXPR_FOR_FORMAL_PARAMETER ;
		  }
		break ;
	  }

	case ITYPE_OP_IN_PARAM :
	  {
		if (item->is_belong_in_op_param_expr() == FALSE)
		  {
			//Erreur B0 : ce n'est pas une expression de typage pour
			//un parametre d'entree d'operation
			error_code = E_B0_INVALID_TYPING_EXPR_FOR_IN_OP_PARAMETER ;
		  }
		break ;
	  }

	default :
	  {
		// Pas de verification
		error_code = (T_error_code) 0;
	  }
	}

  if (error_code != (T_error_code) 0)
	{
	  B0_semantic_error(item,
						CAN_CONTINUE,
						get_error_msg(error_code),
						make_class_name(item),
						B0_def->get_name()->get_value()) ;
	}
} // internal_check_is_belong_expr_right


// Fonction auxiliaire utilisee lors de la verification des predicats
// de typage par appartance des données concretes.  En parametre :
// le prédicat de typage et la déclaration B0 à vérifier.
static void internal_check_is_belong_expr(T_typing_predicate *tpred,
												   T_ident *B0_def)
{
  T_expr *first_ident = tpred->get_identifiers() ;
  T_item *type = tpred->get_type() ;

  TRACE2("internal_check_is_belong_expr(%p, %p)", first_ident, type) ;

  // Attention, dans le cas du typage d'un unique identificateur,
  // l'expression de typage est la partie droite du predicat de typage
  // dans son integralite : cas de P1 : BOOL * BOOL ...
  T_item *cur_item = NULL;
  if (first_ident->get_next() == NULL)
	{
	  cur_item = type;
	}
  else
	{
	  cur_item = type->get_expr(TRUE);
	}

  T_expr *cur_ident = first_ident;
   while (cur_ident != NULL)
	{
	  if (cur_item == NULL)
		{
		  // Erreur B0 : tous les identificateurs ne sont pas types
		  // Erreur deja signalee par le controle de type (analyse semantique)
		  TRACE0("reprise sur erreur") ;
		  return;
		}

	  else if (cur_ident->is_an_ident() == FALSE)
		{
		  TRACE1("%s pas un ident", make_class_name(cur_ident)) ;
		  // Rien a faire
		}

	  else if (cur_ident->make_ident()->get_name() == B0_def->get_name())
		{
		  // On a trouvé l'identificateur à vérifier.

		  if (B0_def->get_definition()->get_B0_type_checked() == TRUE)
			{
			  // Deja fait
			  TRACE0("deja fait") ;
			}
		  else
			{
			  // On note qu'on a rencontré le prédicat de typage de
			  // l'identificateur.
			  B0_def->get_definition()->set_B0_type_checked(TRUE);

			  // On vérifie alors le type de l'identificateur.
			  internal_check_ident_type(cur_ident, B0_def);

			  // Puis on vérifie la partie droite du prédicat.
			  internal_check_is_belong_expr_right(cur_item, B0_def);
			}
		}
	  cur_ident = (T_expr *)cur_ident->get_next();
	  cur_item = cur_item->get_next_expr(TRUE);
	}
} // internal_check_is_belong_expr


// Fonction auxiliaire utilisée par la suivante: Vérifie que ITEM peut
// bien être utilisé en partie droite du prédicat de typage de B0_DEF.
static void internal_check_is_equal_expr_right(T_item *item,
														T_ident *B0_def)
{
  TRACE4("internal_check_is_equal_expr_right(%p::%s, %x '%s')",
		 item, item->get_class_name(),
		 B0_def, B0_def->get_name()->get_value()) ;

  T_error_code error_code = (T_error_code) 0;
  switch (B0_def->get_ident_type())
	{
	case ITYPE_CONCRETE_VARIABLE :
	  {
		if (item->is_equal_concrete_variable_expr() == FALSE)
		  {
			//Erreur B0 : ce n'est pas une expression de typage pour
			//une variable concrete
			error_code = E_B0_INVALID_TYPING_EXPR_FOR_CONCRETE_VARIABLE;
		  }
		break ;
	  }

	case ITYPE_CONCRETE_CONSTANT :
	  {
		if (item->is_equal_concrete_constant_expr() == FALSE)
		  {
			//Erreur B0 : ce n'est pas une expression de typage pour
			//une constante concrete
			error_code = E_B0_INVALID_TYPING_EXPR_FOR_CONCRETE_CONSTANT;

			// Sauf (derniere chance) en mode
			// B0_enable_typing_identifiers, si la partie droite
			// est un tableau ou un record qui definit un type
			if (get_B0_enable_typing_identifiers() == TRUE &&
				B0_def->get_B_type() != NULL &&
				B0_def->get_B_type()->get_typing_ident() != NULL)
			  {
				error_code = (T_error_code) 0 ;
			  }
		  }
		break ;
	  }

	case ITYPE_MACHINE_SCALAR_FORMAL_PARAM :
	  {
		if (item->is_equal_scalar_formal_param_expr() == FALSE)
		  {
			//Erreur B0 : ce n'est pas une expression de typage pour
			//un parametre scalaire de machine
			error_code = E_B0_INVALID_TYPING_EXPR_FOR_FORMAL_PARAMETER ;
		  }
		break ;
	  }

	case ITYPE_OP_IN_PARAM :
	  {
		if (item->is_equal_in_op_param_expr() == FALSE)
		  {
			//Erreur B0 : ce n'est pas une expression de typage pour
			//un parametre d'entree d'operation
			error_code = E_B0_INVALID_TYPING_EXPR_FOR_IN_OP_PARAMETER ;
		  }
		break ;
	  }

	default :
	  {
		// Pas de verification
		error_code = (T_error_code) 0;
	  }
	}

  if (error_code != (T_error_code) 0)
	{
	  B0_semantic_error(item,
						CAN_CONTINUE,
						get_error_msg(error_code),
						make_class_name(item),
						B0_def->get_name()->get_value()) ;
	}
} // internal_check_is_equal_expr_right


// Fonction auxiliaire utilisee lors de la verification des predicats
// de typage par égalité des données concretes.  En parametre : la
// liste des identificateurs (partie gauche du predicat de typage), et
// l'expression de typage ainsi que la nature des identificateurs à
// vérifier.
static void internal_check_is_equal_expr(T_typing_predicate *tpred,
												  T_ident *B0_def)
{
  TRACE2("internal_check_is_equal_expr(%x, type)",
		 tpred->get_identifiers(),
		 tpred->get_type()) ;

  T_item *cur_item = tpred->get_type();
  T_expr *cur_ident = tpred->get_identifiers();
  while (cur_ident != NULL)
	{
	  if (cur_item == NULL)
		{
		  // Erreur B0 : tous les identificateurs ne sont pas types
		  // Erreur deja signalee par le controle de type (analyse
		  // semantique)
		  TRACE0("reprise sur erreur") ;
		  return;
		}

	  else if (cur_ident->is_an_ident() == FALSE)
		{
		  TRACE1("%s pas un ident", make_class_name(cur_ident)) ;
		  // Rien a faire
		}

	  else if (cur_ident->make_ident()->get_name() == B0_def->get_name())
		{
		  // On a trouvé l'identificateur à vérifier.

		  if (B0_def->get_definition()->get_B0_type_checked() == TRUE)
			{
			  // Deja fait
			  TRACE0("deja fait") ;
			}
		  else
			{
			  // On note qu'on a rencontré le prédicat de typage de
			  // l'identificateur.
			  B0_def->get_definition()->set_B0_type_checked(TRUE);

			  // On vérifie alors le type de l'identificateur.
			  internal_check_ident_type(cur_ident, B0_def);

			  // Puis on vérifie la partie droite du prédicat.
			  internal_check_is_equal_expr_right(cur_item, B0_def);
			}
		}
	  cur_ident = (T_expr *)cur_ident->get_next();
	  cur_item = cur_item->get_next_expr(TRUE);
	}
} // internal_check_is_equal_expr



// Fonction auxiliaire utilisée par la suivante: Vérifie que ITEM peut
// bien être utilisé en partie droite du prédicat de typage de B0_DEF.
static void internal_check_is_included_expr_right(T_item *item,
														   T_ident *B0_def)
{
  TRACE4("internal_check_is_included_expr_right(%p::%s, %x '%s')",
		 item, item->get_class_name(),
		 B0_def, B0_def->get_name()->get_value()) ;


  T_error_code error_code = (T_error_code) 0;
  switch (B0_def->get_ident_type())
	{
	case ITYPE_CONCRETE_VARIABLE :
	case ITYPE_OP_IN_PARAM :
	  {
		// Erreur B0 : l'inclusion n'est pas autorisee pour typer une
		// variable concrete ou un paramètre d'entrée d'opération.
		B0_semantic_error(item->get_father(),
						  CAN_CONTINUE,
						  get_error_msg(E_B0_INVALID_TYPING_PREDICATE));
		error_code = (T_error_code) 0 ;
		break ;
	  }

	case ITYPE_CONCRETE_CONSTANT :
	  {
		// CTRL Ref : RTYPE 2-4-1
		if (item->syntax_check_is_a_simple_set() == FALSE)
		  {
			//Erreur B0
			error_code = E_B0_INVALID_TYPING_EXPR_FOR_CONCRETE_CONSTANT;
		  }
		break ;
	  }

	case ITYPE_MACHINE_SCALAR_FORMAL_PARAM :
	  {
		// CTRL Ref : RTYPE 2-4-1
		if (item->syntax_check_is_a_simple_set() == FALSE)
		  {
			//Erreur B0
			error_code = E_B0_INVALID_TYPING_EXPR_FOR_FORMAL_PARAMETER;
		  }
		break ;
	  }

	default :
	  {
		// Pas de verification
		error_code = (T_error_code) 0;
	  }
	}

  if (error_code != (T_error_code) 0)
	{
	  B0_semantic_error(item,
						CAN_CONTINUE,
						get_error_msg(error_code),
						make_class_name(item),
						B0_def->get_name()->get_value()) ;
	}
} // internal_check_is_included_expr_right


// Fonction auxiliaire utilisee lors de la verification des predicats
// de typage par inclusion des données concretes.  En parametre :
// l'identificateur (partie gauche du predicat de typage), et
// l'expression de typage ainsi que la nature des identificateurs à
// vérifier.
static void internal_check_is_included_expr(T_typing_predicate *tpred,
													 T_ident *B0_def)
{
  T_item *ident = tpred->get_identifiers() ;
  T_item *type = tpred->get_type() ;

  TRACE2("internal_check_is_included_expr(%x, type)", ident, type) ;

  if (type == NULL)
	{
	  // Erreur B0 : tous les identificateurs ne sont pas types
	  // Erreur deja signalee par le controle de type (analyse
	  // semantique)
	  TRACE0("reprise sur erreur") ;
	  return;
	}

  else if (ident->is_an_ident() == FALSE)
	{
	  TRACE1("%s pas un ident", make_class_name(ident)) ;
	  // Rien a faire
	}

  else
	{
	  if (B0_def->get_definition()->get_B0_type_checked() == TRUE)
		{
		  // Deja fait
		  TRACE0("deja fait") ;
		}
	  else
		{
		  // On note qu'on a rencontré le prédicat de typage de
		  // l'identificateur.
		  B0_def->get_definition()->set_B0_type_checked(TRUE);

		  // On vérifie alors le type de l'identificateur.
		  internal_check_ident_type((T_ident *) ident, B0_def);

		  // Puis on vérifie la partie droite du prédicat.
		  internal_check_is_included_expr_right(type, B0_def);
		}
	}
} // internal_check_is_included_expr



//
// Controles semantiques sur le type des donnees concretes
//

void T_betree::operation_B0_check(void)
{
  TRACE0("T_betree::operation_B0_check");
  ASSERT(root != NULL);
  root->operation_B0_check();
}

void T_machine::operation_B0_check(void)
{
  TRACE0("T_machine::operation_B0_check");
  T_list_link *cur_linked_op = first_op_list;
  TRACE1("cur_linked_op = %p", cur_linked_op);

  while (cur_linked_op != NULL)
	{
	  T_generic_op *op = (T_generic_op *)cur_linked_op->get_object();
	  op->B0_check();
	  cur_linked_op = (T_list_link *)cur_linked_op->get_next();
	}

  T_op *cur_loc_op = first_local_operation;
  while (cur_loc_op != NULL)
	{
	  cur_loc_op->B0_check();
	  cur_loc_op = (T_op *)cur_loc_op->get_next();
	}
  // l'initialisation
  B0_check_substitution_list(first_initialisation) ;

}

//  Vérifie LE prédicat de typage de chaque déclaration de la liste
//  passée en paramètre.
static void internal_decl_list_type_check(T_ident *decl_list)
{
  T_ident *cur_decl = decl_list ;
  while (cur_decl != NULL)
    {
      T_item *item = cur_decl->get_typing_pred_or_subst() ;
	  if (item != NULL && item->get_node_type() == NODE_TYPING_PREDICATE)
		{
		  // On vérifie le prédicat de typage
		  T_typing_predicate *tpred = (T_typing_predicate *) item ;
		  switch (tpred->get_typing_predicate_type())
			{
			case TPRED_EQUAL :
			  {
				internal_check_is_equal_expr(tpred, cur_decl);
				break;
			  }
			case TPRED_BELONGS :
			  {
				internal_check_is_belong_expr(tpred, cur_decl);
				break;
			  }
			case TPRED_INCLUDED :
			case TPRED_STRICT_INCLUDED :
			  {
				internal_check_is_included_expr(tpred, cur_decl);
				break;
			  }
			  // pas de default pour etre prevenu par le compilateur
			  // en cas de cas non prevu
			}
		}
	  else
		{
		  // Reprise sur erreur: pas de prédicat de typage dans l'arbre.
		  TRACE0("Reprise sur erreur") ;
		}
	  cur_decl = (T_ident *) cur_decl->get_next() ;
	}
}

void T_betree::concrete_constant_type_check(void)
{
  // On vérifie LE prédicat de typage de chaque constante concrète du
  // composant.

  ASSERT(root != NULL);
  internal_decl_list_type_check(root->get_concrete_constants()) ;
}


void T_betree::concrete_variable_type_check(void)
{
  // On vérifie LE prédicat de typage de chaque variable concrète du
  // composant.

  ASSERT(root != NULL);
  internal_decl_list_type_check(root->get_concrete_variables()) ;
}


//
// Controle du typage des parametres scalaires de machine
//
void T_betree::scalar_parameter_type_check(void)
{
  ASSERT(root != NULL);
  internal_decl_list_type_check(root->get_params()) ;
}

//
// Controle du typage des parametres d'entree d'operation
//
void T_op::entry_parameter_type_check(void)
{
  TRACE1("T_op::entry_parameter_type_check (%s)",
		 name->get_name()->get_value()) ;
  internal_decl_list_type_check(get_in_params()) ;
}



//
// Verification des expressions de typage des donnees concretes
// en fonction des predicats de typage
//

// Cas d'une constante concrete
// Predicat de typage =
// CTRL Ref : RTYPE 2-3-1 a RTYPE 2-3-5
int T_item::is_equal_concrete_constant_expr(void)
{
  return ( (syntax_check_is_an_array(NULL) == TRUE) ||
		   (syntax_check_is_a_term(NULL) == TRUE) ||
		   (syntax_check_is_a_range(NULL) == TRUE) );
}

// Predicat de typage :
// CTRL Ref : RTYPE 2-2-1 a RTYPE 2-2-4
// CTRL Ref : RTYPE 3-2-1 a RTYPE 3-2-4
int T_item::is_belong_concrete_data_expr(void)
{
  return ( (syntax_check_is_a_simple_set() == TRUE) ||
		   (syntax_check_is_a_simple_term_set() == TRUE) ||
		   (syntax_check_is_a_record_set() == TRUE) ||
		   (syntax_check_is_a_total_function_set() == TRUE) );
}


// Cas d'une variable concrete
// Predicat de typage =
// CTRL Ref : RTYPE 3-3-1
int T_item::is_equal_concrete_variable_expr(void)
{
  return (syntax_check_is_a_term(NULL) == TRUE);
}


// Cas d'un parametre formel scalaire de machine
// Predicat de typage =
// CTRL Ref : RTYPE 1-2-1
int T_item::is_equal_scalar_formal_param_expr(void)
{
  return (is_equal_concrete_variable_expr() == TRUE);
}

// Predicat de typage :
// CTRL Ref : RTYPE 1-2-2 a RTYPE 1-2-5
int T_item::is_belong_scalar_formal_param_expr(void)
{
  return ( (check_is_BOOL() == TRUE) ||
           (check_is_FLOAT() == TRUE) ||
                              (syntax_check_is_a_range(NULL, B0CTX_FORMAL_PARAMETER_TYPE_CONTEXT)
			== TRUE) ||
		   (syntax_check_is_a_total_function_set() == TRUE) );
}

// Cas d'un parametre d'entree d'operation
// Predicat de typage =
// CTRL Ref : RTYPE 4-3-1
int T_item::is_equal_in_op_param_expr(void)
{
  return (is_equal_concrete_variable_expr() == TRUE);
}

// Predicat de typage :
// CTRL Ref : RTYPE 4-2-1 a RTYPE 4-2-5
int T_item::is_belong_in_op_param_expr(void)
{
  return ( (check_is_STRING() == TRUE) ||
		   (is_belong_concrete_data_expr() == TRUE) );
}




////////////////////////////////////////////////////////////////////////
//
// Controle de compatibilite pour les tableaux en B0
//
////////////////////////////////////////////////////////////////////////

// Fonctions de compatibilte des tableaux

// Cas general
int T_type::is_array_compatible_with(T_type *ref_type,
											  T_item *array)
{
  TRACE6("T_type(%x <%s>)::is_array_compatible_with(%x:%s, %x:%s) -> FALSE",
		 this,
		 make_type_name()->get_value(),
		 ref_type,
		 (ref_type == NULL) ? "null" : ref_type->make_type_name()->get_value(),
		 array,
		 make_class_name(array)) ;
  return FALSE;
}

// Cas specifique des records : Si il y a des tableaux en jeu, on en teste
// la compatibilite, sinon, on ne teste rien!
int T_record_type::is_array_compatible_with(T_type *ref_type,
													 T_item *array)
{
  TRACE6("T_record_type(%x <%s>)::is_array_compatible_with(%x:%s, %x:%s)",
		 this,
		 make_type_name()->get_value(),
		 ref_type,
		 (ref_type == NULL) ? "null" : ref_type->make_type_name()->get_value(),
		 array,
		 make_class_name(array)) ;
  if ((ref_type == NULL) ||
	  (ref_type->get_node_type() != NODE_RECORD_TYPE) )
	{
	  return FALSE ;
	}

  // On peut caster, protege par le IF
  T_record_type *ref = (T_record_type *) ref_type;

  // Label_types du record courant
  T_label_type * cur_list_labels = first_label;

  // Label_types du record en parametre
  T_label_type * ref_list_labels = ref->get_labels();

  // Resultat intermediaire
  int cur_result = TRUE;

  while (cur_list_labels != NULL)
	{
	  // Type du label courant du record courant
	  T_type *type = cur_list_labels->get_type();

	  // Y-a-t-il un tableau en question (ou un record : un tableau peut
	  // se cacher dans un record recursif...On explore ce dernier dans ce cas)
	  if ( (type->is_an_array() == TRUE) ||
		   (type->is_a_record() == TRUE) )
		{
		  // On recupere le type du label courant du record en parametre
		  T_type *cur_ref_type = ref_list_labels->get_type();

		  // Compatibilite?
		  if (type->is_array_compatible_with(cur_ref_type,
											 array) == FALSE)
			{
			  cur_result = FALSE;
			}
		}

	  cur_list_labels =
		(T_label_type *)cur_list_labels->get_next() ;
	  ref_list_labels =
		(T_label_type *)ref_list_labels->get_next() ;
	}

  return cur_result;

}

int T_product_type::is_array_compatible_with(T_type *ref_type,
													  T_item *array)
{
  TRACE6("T_product_type(%x <%s>)::is_array_compatible_with(%x:%s, %x:%s)",
		 this,
		 make_type_name()->get_value(),
		 ref_type,
		 (ref_type == NULL) ? "null" : ref_type->make_type_name()->get_value(),
		 array,
		 make_class_name(array)) ;
  if ((ref_type == NULL) ||
	  (ref_type->get_node_type() != NODE_PRODUCT_TYPE))
	{
	  return FALSE ;
	}

  T_product_type *ref = (T_product_type *)ref_type ;

  // On parcourt les types des ensembles indices des tableaux

  T_type *cur_type = get_spec_types();
  T_type *cur_next_type = cur_type->get_next_spec_type();

  T_type *cur_ref_type = ref->get_spec_types();
  T_type *cur_ref_next_type = cur_ref_type->get_next_spec_type();

  // Resultat intermediaire
  int result = TRUE;

  // Le controle de type de la phase semantique nous assure que les tableaux
  // ont le meme nombre d'ensembles indices...
  // Par prudence, on fait egalement le test sur cur_ref_next_type...
  while ( (cur_next_type != NULL) &&
		  (cur_ref_next_type != NULL) )
	{
	  if (cur_type->is_array_compatible_with(cur_ref_type,
											 array) == FALSE)
		{
		  // Les tableaux ne sont pas compatibles...
		  result = FALSE;
		}

	  // Ensembles suivants
	  cur_type = cur_next_type;
	  cur_next_type = cur_next_type->get_next_spec_type();

	  cur_ref_type = cur_ref_next_type;
	  cur_ref_next_type = cur_ref_next_type->get_next_spec_type();
	}
  return result;
}

int T_setof_type::is_array_compatible_with(T_type *ref_type,
													T_item *array)
{
  TRACE6("T_setof_type(%x <%s>)::is_array_compatible_with(%x:%s, %x:%s)",
		 this,
		 make_type_name()->get_value(),
		 ref_type,
		 (ref_type == NULL) ? "null" : ref_type->make_type_name()->get_value(),
		 array,
		 make_class_name(array)) ;
  // Il faut que ref_type soit un T_setof_type
  if ((ref_type == NULL) ||
	  (ref_type->get_node_type() != NODE_SETOF_TYPE))
	{
	  return FALSE ;
	}

  // Recursivite oblige...
  return base_type->is_array_compatible_with(((T_setof_type*)ref_type)->base_type,
											 array) ;
}

int T_abstract_type::is_array_compatible_with(T_type *ref_type,
													   T_item *array)
{
  TRACE6("T_abstract_type(%x <%s>)::is_array_compatible_with(%x:%s, %x:%s)",
		 this,
		 make_type_name()->get_value(),
		 ref_type,
		 (ref_type == NULL) ? "null" : ref_type->make_type_name()->get_value(),
		 array,
		 make_class_name(array)) ;
  if (ref_type == NULL)
	{
	  return FALSE;
	}

  // Dans le cas ou array est une expression (on ne peut etre que dans une
  // implementation), on controle la compatibilite avec le type courant apres
  // valuation, car array a alors un type plonge, et on n'a plus le type avant
  // valuation... (l'inference de type sera corrige ulterieurement
  // Vu avec JPP le 21/01/99
  if ( (array->is_an_ident() == FALSE) &&
	   // ... et ...simple verification...
	   (get_after_valuation_type() != NULL) )
	{
	  return get_after_valuation_type()->is_array_compatible_with(ref_type,
																  array) ;
	}


  // Il faut que ref_type soit un type abstrait ...
  if (ref_type->get_node_type() != NODE_ABSTRACT_TYPE)
	{
	  return FALSE ;
	}

  // ... avec le meme type_definition
  T_symbol *cur_set = set->get_name() ;
  T_symbol *ref_set = ((T_abstract_type *)ref_type)->set->get_name() ;

  return cur_set->compare(ref_set) ;
}


int T_enumerated_type::is_array_compatible_with(T_type *ref_type,
														 T_item *array)
{
  TRACE6("T_enumerated_type(%x <%s>)::is_array_compatible_with(%x:%s, %x:%s)",
		 this,
		 make_type_name()->get_value(),
		 ref_type,
		 (ref_type == NULL) ? "null" : ref_type->make_type_name()->get_value(),
		 array,
		 make_class_name(array)) ;
  // Il faut que ref_type soit un type enumere ...
  if ((ref_type == NULL) ||
	  (ref_type->get_node_type() != NODE_ENUMERATED_TYPE))
	{
	  return FALSE ;
	}

  // ... avec le meme type_definition
  T_symbol *cur_set = type_definition->get_name() ;
  T_symbol *ref_set = ((T_enumerated_type *)ref_type)->type_definition->get_name() ;

  return cur_set->compare(ref_set) ;
}


int T_predefined_type::is_array_compatible_with(T_type *ref_type,
														 T_item *array)
{
  TRACE6("T_predefined_type(%x <%s>)::is_array_compatible_with(%x:%s, %x:%s)",
		 this,
		 make_type_name()->get_value(),
		 ref_type,
		 (ref_type == NULL) ? "null" : ref_type->make_type_name()->get_value(),
		 array,
		 make_class_name(array)) ;
  // Il faut que ce soit un T_predefined_type ...
  if ((ref_type == NULL) ||
	  (ref_type->get_node_type() != NODE_PREDEFINED_TYPE))
	{
	  return FALSE ;
	}


  // ... avec le meme ref_type ...

  T_predefined_type *rtype = (T_predefined_type*)ref_type ;
  if  (type != rtype->get_type())
	{
	  return FALSE ;
	}

  // Cas d'un type booleen
  if (type == BTYPE_BOOL)
	{
	  return TRUE;
	}


  // ... et un meme ref_intervalle dans le cas d'un typage par appartenance
  // a une constante intervalle
  if ( ( (ref_interval != NULL) &&
		 (rtype->get_ref_interval() == NULL) ) ||
	   ( (ref_interval == NULL) &&
		 (rtype->get_ref_interval() != NULL) ) )
	{
	  // Les deux types ne sont pas compatible au sens de la compatibilite
	  // de tableaux (meme ensemble de definition syntaxiquement)
	  return FALSE;
	}


  if ( (ref_interval != NULL) &&
	   (rtype->get_ref_interval() != NULL) )
	{
	  T_symbol *cur_interval = ref_interval->get_name() ;
	  T_symbol *cur_ref_interval = rtype->get_ref_interval()->get_name();
	  return cur_interval->compare(cur_ref_interval) ;
	}

  // ... et des bornes egales (et existantes...) dans le cas d'intervalles
  // explicites

  // Il faut que les deux types soient bornes
  if ( (is_bounded() == FALSE) ||
	   (rtype->is_bounded() == FALSE) )
	{
	  return FALSE;
	}


  // Si on n'arrive ici, les deux types sont bornees
  // Les ensembles indices, au typage, sont des intervalles explicites.

  // Les bornes sont-elles syntaxiquement egales ?

  // (On ne se preoccupe pas des parentheses qui ne sont pas necessaires
  //  pour evaluer sans ambiguite une expression car elle est stocke
  //  dans un arbre binaire)
  T_expr *ref_lower_bound = rtype->get_lower_bound()->strip_parenthesis();
  T_expr *ref_upper_bound = rtype->get_upper_bound()->strip_parenthesis();

  T_expr *cur_lower_bound = get_lower_bound()->strip_parenthesis();
  T_expr *cur_upper_bound = get_upper_bound()->strip_parenthesis();
  // Resultat intermediaire
  int result = TRUE;

  if (cur_lower_bound->is_array_compatible_with(ref_lower_bound) == FALSE)
	{
	  result = FALSE;
	}

  if (cur_upper_bound->is_array_compatible_with(ref_upper_bound) == FALSE)
	{
	  result = FALSE;
	}

  return result;

}


// Fonctions de compatibilite des tableaux pour les expressions
// Parsent les bornes des ensembles indices

// Cas par defaut pour la fonction virtuelle
int T_item::is_array_compatible_with(T_item *ref_item)
{
  TRACE4("T_item(%x <%s>)::is_array_compatible_with(%x:%s)",
		 this,
		 make_class_name(this),
		 ref_item,
		 make_class_name(ref_item)) ;
  return FALSE;
}


int T_ident::is_array_compatible_with(T_item *ref_item)
{
  TRACE4("T_ident(%x <%s>)::is_array_compatible_with(%x:%s)",
		 this,
		 get_name()->get_value(),
		 ref_item,
		 make_class_name(ref_item)) ;
  T_expr *ref_expr = ref_item->make_expr();

  // Il faut que ref_expr soit un T_ident
  if (ref_expr->get_node_type() != NODE_IDENT)
	{
	  return FALSE;
	}

  // C'est un T_ident...on peut caster allegrement...
  T_symbol *ref_symbol = ((T_ident *) ref_expr)->get_name();

  // Il faut que les ident aient le meme nom
  return name->compare(ref_symbol) ;
}

int
T_expr_with_parenthesis::is_array_compatible_with(T_item *ref_item)
{
  TRACE4("T_expr_with_parenthesis(%x <%s>)::is_array_compatible_with(%x:%s)",
		 this,
		 make_class_name(this),
		 ref_item,
		 make_class_name(ref_item)) ;
  T_expr *ref_expr = ref_item->make_expr();

  // Il faut que ref_expr soit un T_expr_with_parenthesis
  if (ref_expr->get_node_type() != NODE_EXPR_WITH_PARENTHESIS)
	{
	  return FALSE;
	}

  // On peut caster, protege par le IF
  T_expr_with_parenthesis *cur_expr = (T_expr_with_parenthesis *) ref_expr;
  return expr->is_array_compatible_with(cur_expr->get_expr());
}


int T_literal_integer::is_array_compatible_with(T_item *ref_item)
{
  TRACE4("T_literal_integer(%x <%s>)::is_array_compatible_with(%x:%s)",
		 this,
		 make_class_name(this),
		 ref_item,
		 make_class_name(ref_item)) ;
  T_expr *ref_expr = ref_item->make_expr();

  // Il faut que ref_expr soit un T_literal_integer
  if (ref_expr->get_node_type() != NODE_LITERAL_INTEGER)
	{
	  return FALSE;
	}

  // C'est un T_literal_integer...on peut caster allegrement...
  T_integer *ref_value =
	((T_literal_integer *) ref_expr)->get_value();

  // Il faut que les litteraux aient la meme valeur
  return value->is_equal_to(ref_value);
}

int T_binary_op::is_array_compatible_with(T_item *ref_item)
{
  TRACE4("T_binary_op(%x <%s>)::is_array_compatible_with(%x:%s)",
		 this,
		 make_class_name(this),
		 ref_item,
		 make_class_name(ref_item)) ;
  T_expr *ref_expr = ref_item->make_expr();

  // Il faut que ref_expr soit un T_binary_op, et de meme operateur...
  if ( (ref_expr->get_node_type() != NODE_BINARY_OP) ||
	   ( (ref_expr->get_node_type() == NODE_BINARY_OP) &&
		 // On peut caster protege par la condition du IF
		 (oper != ((T_binary_op *) ref_expr)->get_operator()) ) )
	{
	  return FALSE;
	}

  // Les operateurs sont identiques
  // Les operandes le sont-elles?
  switch (oper)
	 {
	 case BOP_TIMES :
	 case BOP_PLUS :
	 case BOP_MINUS :
	 case BOP_DIVIDES :
     case BOP_MOD :
     case BOP_POWER :
	   {
		 // Resultat intermediaire
		 int result = TRUE;

		 // On peut caster : On sait que c'est un T_binary_op
		 // (sinon, on ne serait pas la...)
		 if (op1->is_array_compatible_with( ((T_binary_op *)ref_expr)
											->get_op1()) == FALSE)
		   {
			 result = FALSE;
		   }
		 if (op2->is_array_compatible_with( ((T_binary_op *)ref_expr)
											->get_op2()) == FALSE)
		   {
			 result = FALSE;
		   }

		 return result;
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
		 // Le controle de type de la passe semantique nous garantit
		 // que l'on est en presence d'expressions arithmetiques
		 // On ne peut rencontrer ces cas
		 return FALSE;
	   }
	   // Pas de default pour que le compilateur nous
	   // previenne en cas d'ajout
	 }
   // Pour eviter le warning
   ASSERT(FALSE);
   return FALSE;
}


int T_unary_op::is_array_compatible_with(T_item *ref_item)
{
  TRACE4("T_unary_op(%x <%s>)::is_array_compatible_with(%x:%s)",
		 this,
		 make_class_name(this),
		 ref_item,
		 make_class_name(ref_item)) ;
  T_expr *ref_expr = ref_item->make_expr();

  // Il faut que ref_expr soit un T_unary_op, et de meme operateur...
  if ( (ref_expr->get_node_type() != NODE_UNARY_OP) ||
	   ( (ref_expr->get_node_type() == NODE_UNARY_OP) &&
		 // On peut caster protege par la condition du IF
		 (oper != ((T_unary_op *) ref_expr)->get_operator()) ) )
	{
	  return FALSE;
	}

  // L'operateur est identique
  // L'operande l'est-elle?
  switch (oper)
	 {
	 case UOP_MINUS :
	   {
		 if (operand->is_array_compatible_with( ((T_unary_op *)ref_expr)
												->get_operand()) == FALSE)
		   {
			 return FALSE;
		   }
		 return TRUE;
	   }
	   // Pas de default pour que le compilateur nous
	   // previenne en cas d'ajout
	 }
   // Pour eviter le warning
   ASSERT(FALSE);
   return FALSE;
}

int T_op_result::is_array_compatible_with(T_item *ref_item)
{
  TRACE4("T_op_result(%x <%s>)::is_array_compatible_with(%x:%s)",
		 this,
		 make_class_name(this),
		 ref_item,
		 make_class_name(ref_item)) ;
  T_expr *ref_expr = ref_item->make_expr();

  // Il faut que ref_expr soit un T_op_result
  if (ref_expr->get_node_type() != NODE_OP_RESULT)
	{
	  return FALSE;
	}

  // On peut maintenant caster...
  T_op_result *ref_op = (T_op_result *) ref_expr;

  if ( (ref_builtin == NULL) ||
	   (nb_in_params != 1) ||
	   (ref_op->get_ref_builtin() == NULL) ||
	   (ref_op->get_nb_in_params() != 1) ||
	   ( (L_SUCC != ref_builtin->get_lex_type()) &&
		 (L_PRED != ref_builtin->get_lex_type()) ) ||
	   (ref_builtin != ref_op->get_ref_builtin()) )
	{
	  return FALSE;
	}

  // a ce stade, ce sont deux succ ou deux pred...
  // Leurs parametres sont-ils compatibles (syntaxiquement)?
  return first_in_param->is_array_compatible_with(ref_op->get_in_params()) ;
}


int T_record_access::is_array_compatible_with(T_item *ref_item)
{
  TRACE4("T_record_access(%x <%s>)::is_array_compatible_with(%x:%s)",
		 this,
		 make_class_name(this),
		 ref_item,
		 make_class_name(ref_item)) ;
  T_expr *ref_expr = ref_item->make_expr();

  // Il faut que ref_expr soit un T_record_access
  if (ref_expr->get_node_type() != NODE_RECORD_ACCESS)
	{
	  return FALSE;
	}

  // On peut maintenant caster...
  T_record_access *ref_record_access = (T_record_access *) ref_expr;

  // Il faut que les records consideres soient des identificateurs identiques
  T_expr *cur_record = get_first_record();
  T_expr *ref_record = ref_record_access->get_first_record();

  if ( (cur_record->get_node_type() == NODE_IDENT) &&
	   (ref_record->get_node_type() == NODE_IDENT) )
	{
	  // On peut caster, protege par la condition du IF
	  T_ident *cur_ident = (T_ident *) cur_record;
	  T_ident *ref_ident = (T_ident *) ref_record;

	  return (cur_ident->get_name() == ref_ident->get_name()) ? TRUE : FALSE;
	}

  return FALSE;

}

// Fonction permettant de remonter au record dans un acces a ce
// dernier ; l'acces pouvant etre recursif
T_expr * T_record_access::get_first_record(void)
{
  if (record->get_node_type() == NODE_RECORD_ACCESS)
	{
	  // cast valide par condition du IF
	  return ((T_record_access *) record)->get_first_record() ;
	}
  else
	{
	  return record;
	}
}


//
// Fin du fichier
//

#endif // B0C

