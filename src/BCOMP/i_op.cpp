/******************************* CLEARSY **************************************
* Fichier : $Id: i_op.cpp,v 2.0 2002-09-24 09:29:56 cm Exp $
* (C) 2008 CLEARSY
*
* Description :		Compilateur B
*					B0 check pour les operations
*
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
RCS_ID("$Id: i_op.cpp,v 1.2 2002-09-24 09:29:56 cm Exp $") ;

/* Includes systeme */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

/* Includes Composant Local */
#include "c_api.h"
#include "i_toplvl.h"
#include "i_decl.h"
#include "i_type.h"

//
// donne le statut de l'opération
//

T_B0_OM_status
T_generic_op::get_status(void)
{
  T_B0_OM_status result = B0_OM_UNDETERMINED_ST;

  T_ident *param = get_in_params();
  if (param == NULL)
    {
      param = get_out_params();
	}

  if (param != NULL)
	{
      if (param->get_BOM_type() != NULL)
		{
		  result = B0_OM_CONCRETE_ST;
		}
	  else
		{
		  result = B0_OM_ABSTRACT_ST;
		}
    }
  return result;
}


// Controle B0 classic des opérations (non utilisée en BOM)
void T_op::B0_check(void)
{
  ASSERT(get_Use_B0_Declaration() == FALSE);
  TRACE3("DEBUT B0 CHECK OPERATION %x (%s:%s)",
		 this,
		 get_betree()->get_file_name()->get_value(),
		 name->get_name()->get_value()) ;
  ENTER_TRACE ; ENTER_TRACE ;

  if (B0_checked == TRUE)
	{
	  TRACE0("deja fait") ;
	  return ;
	}
  B0_checked = TRUE ;


  T_machine *root = (T_machine *)get_father() ;
  ASSERT(root->get_node_type() == NODE_MACHINE) ;

  if ( (root->get_machine_type() == MTYPE_SPECIFICATION)
	   ||
	   (is_a_local_op == TRUE) )
	{

	  // Controle B0 du typage des paramètres d'entrée
	  // Ce controle sera à déplacer dans la passe B0 Check
	  if ( (get_B0_disable_operation_input_parameters_type_check() != TRUE)
		   &&
		   ( (root->get_machine_type() == MTYPE_SPECIFICATION)
			 ||
			 (is_a_local_op == TRUE) ) )
		{
		  this->entry_parameter_type_check() ;
		}
	}


  // Puis type check du corps
  T_substitution *body;
  T_precond *precond;
  get_precond_and_body(&precond,&body);

  TRACE0("B0 check du corps") ;
  B0_check_substitution_list(body) ;
  TRACE0("fin B0 check corps");

  if ( (root->get_machine_type() == MTYPE_SPECIFICATION)
	   ||
	   (is_a_local_op == TRUE) )
	{
	  if (get_B0_disable_operation_output_parameters_type_check() != TRUE)
		{
		  T_ident *cur_ident = first_out_param ;
		  while (cur_ident != NULL)
			{
			  if ( (cur_ident->get_B_type() != NULL) &&
				   (cur_ident->get_B_type()->is_exit_parameter_type()
					== FALSE) )
				{
				  // ERREUR B0 : ce n'est pas un type de parametre de sortie
				  // d'operation
				  B0_semantic_error(cur_ident,
									CAN_CONTINUE,
									get_error_msg(E_B0_INVALID_TYPE),
									cur_ident->get_definition()
									->get_ident_type_name(),
									cur_ident->get_definition()
									->get_name()->get_value()) ;
				}
			  cur_ident = (T_ident *)cur_ident->get_next() ;
			}
		}
	}// fin de la verification des parametres de sorties d'opération

  EXIT_TRACE ; EXIT_TRACE ;
  TRACE3("FIN B0 CHECK OPERATION %x:%s",
		 this,
		 get_betree()->get_file_name()->get_value(),
		 name->get_name()->get_value()) ;
}
//
// Fonctionn de control B0 classique non utilisé en BOM
//
void T_used_op::B0_check(void)
{
  ASSERT(ref_op != NULL
		 &&
		 get_Use_B0_Declaration() == FALSE);
  ref_op->B0_check();
}

static void compare_param_list (T_ident *first_ref_ident,
										 T_ident *first_com_ident,
										 T_generic_op *gen_op)
{

  T_ident *c_ref_par = first_ref_ident;
  T_ident *c_com_par = first_com_ident;

  while (c_ref_par != NULL
		 &&
		 c_com_par != NULL)
	{

	  if (c_ref_par->get_BOM_type() != NULL
		  &&
		  c_ref_par->get_BOM_type()->is_an_ident() == TRUE
		  &&
		  c_com_par->get_BOM_type() != NULL
		  &&
		  c_com_par->get_BOM_type()->is_an_ident() == TRUE)
		{
		  T_ident *ref_BOM_type = c_ref_par->get_BOM_type()->make_ident();
		  T_ident *com_BOM_type = c_com_par->get_BOM_type()->make_ident();

		  if (ident_type_semantic_equal(ref_BOM_type,
										com_BOM_type) == FALSE)
			{
			  B0_semantic_error(c_com_par,
								CAN_CONTINUE,
								get_error_msg(E_B0_INVALID_TYPE_FOR_PARAM),
								com_BOM_type->get_name()->get_value(),
								c_ref_par->get_name()->get_value(),
								gen_op->get_op_name()->get_value(),
								ref_BOM_type->get_name()->get_value());
			}
		}

	  c_ref_par = (T_ident *)c_ref_par->get_next();
	  c_com_par = (T_ident *)c_com_par->get_next();
	}
}

//
// Vérification de l'entete d'une opération On vérifie que le ou les
// collages sont cohérent du point de vue des types B0 OM. Ainsi que
// la déclaration par rapport au statut attendu
//
void T_used_op::B0_OM_check_header_decl(T_B0_OM_status status)
{
  TRACE2("T_used_op(%p:%s)::B0_OM_check_header_decl",
		 this,
		 name->get_name()->get_value());
  ref_op->B0_OM_check_header_decl(status);

  if (name->get_homonym_in_abstraction() != NULL
	  &&
	  name->get_homonym_in_required_mach() != NULL)
	{
	  T_ident *op_name_abs = name->get_homonym_in_abstraction();
	  T_ident *op_name_req = name->get_homonym_in_required_mach();

	  ASSERT(op_name_abs->get_father()->is_an_operation() == TRUE
			 &&
			 op_name_req->get_father()->is_an_operation() == TRUE);

	  T_generic_op *abs_op = (T_generic_op *)op_name_abs->get_father();
	  T_generic_op *req_op = (T_generic_op *)op_name_req->get_father();

	  TRACE2("abs_op: %p, req_op: %p",
			 abs_op,
			 req_op);
	  compare_param_list (abs_op->get_in_params(),
						  req_op->get_in_params(),
						  this);
	  compare_param_list (abs_op->get_out_params(),
						  req_op->get_out_params(),
						  this);
	  TRACE0("pres");
	}
}


void T_used_op::B0_OM_check_body_decl()
{
  ref_op->B0_OM_check_body_decl();
}


//
// Vérification de l'entète d'une opération. must_be_concrete est vrai
// si et seulement si l'opération est dans un module concret
//
void T_op::B0_OM_check_header_decl(T_B0_OM_status at_status)
{
  TRACE2("T_op(%p,%s)::B0_OM_check_header_decl",
		 this,
		 name->get_name()->get_value());

  if (B0_header_checked == TRUE)
	{
	  return;
	}
  B0_header_checked = TRUE;
  B0_OM_name_check(name);

  if (get_status() == at_status
	  &&
	  at_status == B0_OM_CONCRETE_ST)
	{
	  T_ident *cur_param = first_in_param;
	  while(cur_param != NULL)
		{
		  first_in_param->B0_check_decl();
		  cur_param = (T_ident *)cur_param->get_next();
		}
	  cur_param = first_out_param;
	  while(cur_param != NULL)
		{
		  cur_param->B0_check_decl();
		  cur_param = (T_ident *)cur_param->get_next();
		}

	  // Controle de la cohérence éventuelle avec l'abstraction

	  if (name->get_homonym_in_abstraction() != NULL)
		{
		  T_item *item = name->get_homonym_in_abstraction()->get_father();
		  ASSERT(item->is_an_operation() == TRUE);

		  T_generic_op *abs_op = (T_generic_op *)item;


		  compare_param_list (abs_op->get_in_params(),
							  first_in_param,
							  this);
		  compare_param_list (abs_op->get_out_params(),
							  first_out_param,
							  this);
		}

	  if (first_out_param != last_out_param)
		{
		  B0_semantic_error(this,
							CAN_CONTINUE,
							get_error_msg(E_B0_OPERATION_ONLY_ONE_OUT_PARAM),
							name->get_name()->get_value());
		}

	}
  else if (get_status() != B0_OM_UNDETERMINED_ST
		   &&
		   get_status() != at_status)
	{
	  if (at_status == B0_OM_CONCRETE_ST)
		{
		  B0_semantic_error(this,
							CAN_CONTINUE,
							get_error_msg(E_B0_OPERATION_MUST_BE_CONCRETE),
							this->get_op_name()->get_value());
		}
	  else
		{
		  B0_semantic_error(this,
							CAN_CONTINUE,
							get_error_msg(E_B0_OPERATION_MUST_BE_ABSTRACT),
							this->get_op_name()->get_value());
		}
	}
}

//
// Fonction de vérificatioin B0 des corps d'opérations
// Le corps doit etre concret
//
void T_op::B0_OM_check_body_decl()
{
  if (B0_body_checked == TRUE)
	{
	  return;
	}
  B0_body_checked = TRUE;
  B0_check_substitution_list(first_body);
}
