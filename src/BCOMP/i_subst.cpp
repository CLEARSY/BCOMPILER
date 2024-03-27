/******************************* CLEARSY **************************************
* Fichier : $Id: i_subst.cpp,v 2.0 2002-07-16 09:37:16 cm Exp $
* (C) 2008 CLEARSY
*
* Description :		Compilateur B
*					B0 check pour les subtitutions
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
RCS_ID("$Id: i_subst.cpp,v 1.1 2002-07-16 09:37:16 cm Exp $") ;

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

#include "i_aux.h"
#include "i_ctx.h"
#include "i_listid.h"
#include "i_globdat.h"
#include "i_type.h"

//
//	}{	Classe T_substitution
//

void T_substitution::B0_check()
{
  // ne fait rien
  TRACE0("T_substitution::B0_check");
}
//
//	}{	Classe T_affect
//

// Fonction generique de type check
// Substitution du type A := B
// CTRL Ref : ITYPE 2-1
static void T_affect_B0_check(T_affect *sub_af)
{
  TRACE1("DEBUT T_affect_B0_check(%x)", sub_af) ;
  ENTER_TRACE ;

  T_expr *f_name = sub_af->get_lvalues();
  T_expr *f_value = sub_af->get_values();
  // CTRL Ref : ESYNEXP 4
  // CTRL Ref : ESYNEXP 5
  if (get_B0_disable_expression_syntax_check() == TRUE)
	{
	  return ;
	}

  if (sub_af->check_is_in_an_implementation() == TRUE)
	{
	  if ( (f_name->get_next() != NULL) ||
		   (f_value->get_next() != NULL) )
		{
		  //Erreur : il ne peut y avoir de listes (le parallelisme est interdit
		  // en implementation
		   B0_syntax_error(f_name,
						   CAN_CONTINUE,
						   get_error_msg(E_B0_INVALID_SIMULTANEOUS_AFFECTATION)) ;
		}

	  // CTRL Ref : ESYNEXP 3
	  ASSERT(f_name != NULL);
	  if (f_name->syntax_check_is_a_term(NULL) == FALSE)
		{
		  //Erreur : n'est pas conforme a une partie gauche d'affectation
		  // en implementation
		  B0_syntax_error(f_name,
						  CAN_CONTINUE,
						  get_error_msg(E_B0_INVALID_LHS_IN_AFFECTATION)) ;
		}

	  ASSERT(f_value != NULL);
	  TRACE5("on regarde si %x %s %s:%d:%d est un terme ou un tableau",
			 f_value,
			 make_class_name(f_value),
			 f_value->get_ref_lexem()->get_file_name()->get_value(),
			 f_value->get_ref_lexem()->get_file_line(),
			 f_value->get_ref_lexem()->get_file_column()) ;
	  if ( (f_value->syntax_check_is_a_term(NULL) == FALSE) &&
		   (f_value->syntax_check_is_an_array(NULL) == FALSE) )
		{
		  //Erreur : n'est pas conforme a une partie droite d'affectation
		  // en implementation
		  B0_syntax_error(f_value,
						  CAN_CONTINUE,
						  get_error_msg(E_B0_INVALID_RHS_IN_AFFECTATION)) ;
		}

	  // Cas des predefinis : les ensembles sont interdits
	  // seuls MAXINT et MININT sont autorises
	  if (f_value->is_an_ident() == TRUE)
		{
		  // On peut caster, protege par le IF
		  T_ident *ident_value = (T_ident *) f_value->make_expr();
		  if ( (ident_value->get_definition()->get_ident_type()
				== ITYPE_BUILTIN) &&
			   (ident_value->get_definition()->get_name()
				!=  get_builtin_MININT()->get_name()) &&
			   (ident_value->get_definition()->get_name()
				!=  get_builtin_MAXINT()->get_name()) )
			{
			  //Erreur : n'est pas conforme a une partie droite d'affectation
			  // en implementation
			  B0_syntax_error(f_value,
							  CAN_CONTINUE,
							  get_error_msg(E_B0_INVALID_RHS_IN_AFFECTATION)) ;
			}
		}


	  //
	  // Controles de compatibilite des tableaux
	  //
	  if (get_B0_disable_array_compatibility_check() == TRUE)
		{
		  return ;
		};

	  // En mode typing_identifiers (HIA) on ne verifie pas la
	  // compatibilite des tableaux, elle est a la charge du
	  // programmeur (qui peut du coup ecrire du code plus "pointu",
	  // charge a lui d'assurer cette compatibilite)
	  if (get_B0_enable_typing_identifiers() == TRUE)
		{
		  return ;
		}


	  // CTRL Ref : RSEMSUBST 2-1
	  if ( (f_name->get_original_B_type() != NULL) &&
		   ( (f_name->get_original_B_type()->is_an_array() == TRUE) ||
			 (f_name->get_original_B_type()->is_a_record() == TRUE) ) &&

		   ( (f_value->get_original_B_type() != NULL) &&
			 ( (f_value->get_original_B_type()->is_an_array() == TRUE) ||
			 (f_value->get_original_B_type()->is_a_record() == TRUE) ) ) )
		{
		  // C'est un tableau ; on verifie la compatibilite
		  T_type *left_type = f_name->get_original_B_type();
		  T_type *right_type = f_value->get_original_B_type();

		  if ((left_type != NULL) &&
			  (left_type->is_array_compatible_with(right_type,
												   f_value) == FALSE) )
			{
			  B0_semantic_error(sub_af,
				  CAN_CONTINUE,
				  get_error_msg(E_B0_INCOMPATIBLE_ARRAYS_BETWEEN_LHS_AND_RHS));
			}
		}

	}

  EXIT_TRACE ;
  TRACE1("FIN T_affect_B0_check(%x)", sub_af) ;

}

static void T_affect_B0_OM_check(T_affect *sub_af)
{
  TRACE1("DEBUT T_affect_B0_OM_check(%x)", sub_af) ;
  ENTER_TRACE ;

  T_expr *f_name = sub_af->get_lvalues();
  T_expr *f_value = sub_af->get_values();

  if ( (f_name->get_next() != NULL) ||
	   (f_value->get_next() != NULL) )
	{
	  //Erreur : il ne peut y avoir de listes (le parallelisme est interdit
	  // en implementation
	  B0_syntax_error(f_name,
					  CAN_CONTINUE,
					  get_error_msg(E_B0_INVALID_SIMULTANEOUS_AFFECTATION)) ;
	}
  else
	{
	  T_ident *lhs_type = f_name->B0_check_expr(NULL,
												NULL,
												f_name->get_ref_lexem(),
												B0_CTX_GENERAL);
	  T_ident *rhs_type = f_value->B0_check_expr(NULL,
												 NULL,
												 f_name->get_ref_lexem(),
												 B0_CTX_GENERAL);

	  if (rhs_type != NULL
		  &&
		  lhs_type != NULL
		  &&
		  ident_type_semantic_equal(rhs_type, lhs_type) == FALSE)
		{
		  B0_semantic_error(sub_af,
							CAN_CONTINUE,
							get_error_msg(E_EXPR_INCOMPATIBLE_TYPES),
							rhs_type->get_name()->get_value(),
							lhs_type->get_name()->get_value());
		}
	  if (rhs_type != NULL
		  &&
		  rhs_type->is_an_array_type() == TRUE)
		{
		  B0_semantic_error(f_value,
							CAN_CONTINUE,
							get_error_msg(E_B0_ARRAY_EXPR_FORBIDDEN));
		}
	}

  EXIT_TRACE;
}
void T_affect::B0_check(void)
{
  if (get_Use_B0_Declaration() == FALSE)
	{
	  T_affect_B0_check(this);
	}
  else
	{
	  T_affect_B0_OM_check(this);
	}
}

//
//	}{	Classe T_op_call
//

//
// Fonction verifiant la compatibilite de tableaux
// entre parametres formels et effectifs
//
void T_op_call::internal_compatibility_B0_check(T_ident *cur_formal_param,
													  T_expr *cur_effective_param)
{
  // Sauf :: en mode typing_identifiers (HIA) ou la
  // compatibilite est a la charge du programmeur
  if (get_B0_enable_typing_identifiers() == TRUE)
	{
	  return ;
	}

  // Boucle parcourant les parametres
  while (cur_formal_param != NULL)
	{
	  if ( ( (cur_effective_param->get_original_B_type()->is_an_array()
			  == TRUE) ||
			 (cur_effective_param->get_original_B_type()->is_a_record()
			  == TRUE) )  &&
		   ( (cur_formal_param->get_original_B_type()->is_an_array()
			  == TRUE) ||
			 (cur_formal_param->get_original_B_type()->is_a_record()
			  == TRUE) ) )
		{
		  // Il y a un tableau ; on verifie la compatibilite
		  T_type *left_type = cur_effective_param->get_original_B_type();
		  T_type *right_type = cur_formal_param->get_original_B_type();

		  if (left_type->is_array_compatible_with(right_type,
												  cur_formal_param) == FALSE)
			{
			  B0_semantic_error(this,
								CAN_CONTINUE,
								get_error_msg(E_B0_INCOMPATIBLE_ARRAYS_BETWEEN_FORMAL_AND_EFFECTIVE_PARAM));
			}
		}

	  // Parametres suivant...
	  if (cur_effective_param->get_next() != NULL)
		{
		  cur_effective_param=cur_effective_param->get_next()->make_expr();
		}
	  cur_formal_param = (T_ident *) cur_formal_param->get_next();
	}
}

// Fonction generique de type check
// Substitution du type A := B
// CTRL Ref : ITYPE 2-2
static void T_op_call_B0_check(T_op_call *op_call)
{
  TRACE1("DEBUT T_op_call_B0_check(%p)", op_call) ;
  ENTER_TRACE ;

  if (get_B0_disable_expression_syntax_check() == TRUE) {return ;};
  T_betree * betree =  op_call->get_betree();

  // Expression interdite en implementation dans certains cas
  if (op_call->get_op_name()->is_an_ident() == FALSE)
	{
	  //Erreur
	  betree->check_is_not_an_implementation(op_call) ;
	}

  // Dans une implementation, les parametres effectifs doivent etre des
  // expressions B0 ou des chaines literales
  // CTRL Ref : ESYNEXP 6
  if (op_call->check_is_in_an_implementation() == TRUE)
	{
	  // Les parametres effectifs sont-ils  conformes a ceux
	  // autorises en implementation
	  syntax_check_list_is_an_op_in_param(op_call->get_in_params(),NULL);

	  //
	  // CTRL Ref : RSEMSUBST 2-2
	  //
	  if (get_B0_disable_array_compatibility_check() == TRUE) {return ;};

	  // On recupere l'operation referencee
	  T_generic_op * ref_op = op_call->get_ref_op();
	  ASSERT(ref_op != NULL);
	  // Traitement des parametres d'entree
	  if (op_call->get_in_params() != NULL)
		{
		  // On recupere les parametres formels

		  T_ident *cur_formal_param = ref_op->get_in_params();
		  ASSERT(cur_formal_param != NULL);

		  // Si on arrive ici, c'est qu'il y a bien le meme nombre de
		  // parametres formels et effectifs

		  // On recupere un pointeur sur les parametres effectifs
		  T_expr *cur_effective_param = op_call->get_in_params()->make_expr();

		  op_call->internal_compatibility_B0_check(cur_formal_param,
												   cur_effective_param);

		}

	  // Traitement des parametres de sortie
	  if (op_call->get_out_params() != NULL)
		{

		  // On recupere les parametres formels
		  T_ident *cur_formal_param = ref_op->get_out_params();
		  ASSERT(cur_formal_param != NULL);

		  // Si on arrive ici, c'est qu'il y a bien le meme nombre de
		  // parametres formels et effectifs

		  // On recupere un pointeur sur les parametres effectifs
		  T_expr *cur_effective_param = op_call->get_out_params()->make_expr();

		  op_call->internal_compatibility_B0_check(cur_formal_param,
												   cur_effective_param);
		}
	}

  EXIT_TRACE ;
  TRACE1("FIN T_op_call_B0_check(%x)", op_call) ;

}

static void anti_aliasing_check(T_generic_op *op,
										 T_ident *first_var,
										 T_item *first_eff_param)
{
  TRACE0("anti_aliasing_check");
  T_ident *cur_var = first_var;
  while (cur_var != NULL)
	{
	  TRACE3("operation %s %p inlinee: %s",
			 op->get_op_name()->get_value(),
			 op,
			 op->get_is_inlined() == TRUE ? "true" : "false");
	  if (cur_var->is_an_array() == TRUE
		  ||
		  op->get_is_inlined() == TRUE)
		{
		  T_item *cur_eff_param = first_eff_param;
		  while (cur_eff_param != NULL)
			{
			  T_ident *var_reference = NULL;
			  ASSERT(cur_eff_param->is_an_expr() == TRUE);
			  var_reference = cur_eff_param->make_expr()->B0_OM_refer_to(cur_var);

			  if (var_reference != NULL)
				{
				  B0_semantic_error(var_reference,
									CAN_CONTINUE,
									get_error_msg(E_B0_IDENT_CANT_BE_REFERED),
									cur_var->get_name()->get_value());
				}
			  cur_eff_param = cur_eff_param->get_next();
			}
		}
	  cur_var = (T_ident *)cur_var->get_next();
	}
}


static void T_op_call_B0_OM_check(T_op_call *op_call)
{
  TRACE0("T_op_call_B0_OM_check");
  // Hypoth�se le nombre de param�tres effectif est le bon
  // V�rifi� en passe s�mantique

  T_generic_op *op = op_call->get_ref_op();

  // Controle du types du param�tres de retour �ventuel
  T_item *out_param = op_call->get_out_params();
  if (out_param != NULL)
	{
	  T_ident *out_form_param = op->get_out_params();
	  if (out_form_param->get_BOM_type() != NULL)
		{
		  if (out_form_param->get_BOM_type()->is_an_ident() == TRUE)
			{
			  T_ident *form_type = out_form_param->get_BOM_type()->make_ident();
			  ASSERT(out_param->is_an_expr() == TRUE);
			  T_ident *eff_type = out_param->make_expr()
				->B0_check_expr(form_type,
								NULL,
								out_param->get_ref_lexem(),
								B0_CTX_GENERAL);

			  if (eff_type != NULL
				  &&
				  ident_type_semantic_equal(form_type, eff_type) == FALSE)
				{
				  B0_semantic_error(out_param,
									CAN_CONTINUE,
									get_error_msg(E_B0_INCOMPATIBLE_TYPE_FOR_PARAMS),
									eff_type->get_name()->get_value(),
									form_type->get_name()->get_value(),
									out_form_param->get_name()->get_value());
				}
			}
		  else
			{
			  // reprise sur erreur
			}
		}
	  else
		{
		  // reprise sur erreur
		}
	}

  // Control des coh�rences de type des param�tres d'entr�e

  T_item *cur_eff_param = op_call->get_in_params();
  T_ident *cur_form_param = op->get_in_params();
  while (cur_eff_param != NULL)
	{
	  if (cur_form_param != NULL
		  &&
		  cur_form_param->get_BOM_type() != NULL
		  &&
		  cur_form_param->get_BOM_type()->is_an_ident() == TRUE)
		{
		  T_ident *form_type = cur_form_param->get_BOM_type()->make_ident();
		  ASSERT(cur_eff_param->is_an_expr() == TRUE);
		  T_ident *eff_type = cur_eff_param->make_expr()
			->B0_check_expr(form_type,
							NULL,
							cur_eff_param->get_ref_lexem(),
							B0_CTX_GENERAL);
		  if (eff_type != NULL
			  &&
			  ident_type_semantic_equal(form_type, eff_type) == FALSE)
			{
			  B0_semantic_error(cur_eff_param,
								CAN_CONTINUE,
								get_error_msg(E_B0_INCOMPATIBLE_TYPE_FOR_PARAMS),
								eff_type->get_name()->get_value(),
								form_type->get_name()->get_value(),
								cur_form_param->get_name()->get_value());

			}
		}
	  else
		{
		  // reprise sur erreur
		}
	  cur_eff_param = cur_eff_param->get_next();
	  cur_form_param = (T_ident *)cur_form_param->get_next();
	}


  TRACE0("control d'anti-aliasing");
  //Premi�re restriction le param�tre de sortie ne doit pas etre
  //utilis� dans les param�tres effectif d'entr�e

  T_item *first_eff_param = op_call->get_in_params();
  if (first_eff_param != NULL)
	{
	  // Controle des param�tres de sortie
	  T_item *first_out_param = op_call->get_out_params();
	  if (first_out_param != NULL)
		{
		  if(first_out_param->is_an_ident() == TRUE)
			{
			  T_ident *first_var = (T_ident *)first_out_param;

			  anti_aliasing_check(op,
								  first_var,
								  first_eff_param);
			}
		  else
			{
			  //reprise sur erreur
			}
		}


	  T_machine *op_mach = op->get_betree()->get_root();
	  T_ident *first_concrete_var = op_mach->get_concrete_variables();
	  anti_aliasing_check(op,
						  first_concrete_var,
						  first_eff_param);
	}



}


void T_op_call::B0_check(void)
{
  if (get_Use_B0_Declaration() == FALSE)
	{
	 T_op_call_B0_check(this);
	}
  else
	{
	  T_op_call_B0_OM_check(this);
	}
}

//
//	}{	Classe T_becomes_member_of
//

static void T_becomes_member_of_B0_OM_check(T_becomes_member_of *becomes_sub)
{
  TRACE1("DEBUT T_becomes_member_of_B0_OM_check(%x)", becomes_sub) ;
  ENTER_TRACE ;

  T_expr *name = becomes_sub->get_lvalues();
  T_expr *type = becomes_sub->get_set();


  if (type->is_an_ident() == FALSE
	  ||
	  name->is_an_ident() == FALSE)
	{
	  B0_semantic_error(becomes_sub,
						CAN_CONTINUE,
						get_error_msg(E_B0_INVALID_BECOMES_MEMBER));
	}
  else
	{
	  T_ident *ident_type = (T_ident *)type;

	  if (ident_type->is_a_BOM_type() == FALSE)
		{
		  B0_semantic_error(becomes_sub,
							CAN_CONTINUE,
							get_error_msg(E_B0_INVALID_BECOMES_MEMBER));
		}
	  if ( (name->get_next() != NULL) ||
		   (type->get_next() != NULL) )
		{
		  //Erreur : il ne peut y avoir de listes (le parallelisme est
		  //interdit en implementation
		  B0_syntax_error(name,
						  CAN_CONTINUE,
						  get_error_msg(E_B0_INVALID_SIMULTANEOUS_AFFECTATION)) ;
		}
	  else
		{
		  T_ident *lhs_type = name->B0_check_expr(NULL,
												  NULL,
												  name->get_ref_lexem(),
												  B0_CTX_GENERAL);

		  if (lhs_type != NULL
			  &&
			  ident_type_semantic_equal(ident_type, lhs_type) == FALSE)
			{
			  B0_semantic_error(becomes_sub,
								CAN_CONTINUE,
								get_error_msg(E_EXPR_INCOMPATIBLE_TYPES),
								ident_type->get_name()->get_value(),
								lhs_type->get_name()->get_value());
			}
		  if (ident_type->is_a_BOM_complete_type() == FALSE)
			{
			  B0_semantic_error(becomes_sub,
								CAN_CONTINUE,
								get_error_msg(E_B0_INVALID_TYPE_FOR_BECOMES_MEMBER),
								ident_type->get_name()->get_value());

			}
		}
	}
  EXIT_TRACE;
}

// Fonction generique de type check
// Substitution du type A :: B
// CTRL Ref : ITYPE 2-3
// Classe T_becomes_member_of : on type !
void T_becomes_member_of::B0_check(void)
{
  TRACE0("T_becomes_member_of::B0_check()");
  if (get_Use_B0_Declaration() == TRUE)
	{
	  T_becomes_member_of_B0_OM_check(this);
	}

}

//
//	}{ Classe T_precond
//
void T_precond::B0_check(void)
{
  TRACE1("T_precond(%x)::B0_check()", this) ;
  ENTER_TRACE ;
  predicate->B0_check() ;
  B0_check_substitution_list(first_body) ;
  EXIT_TRACE ;
}

//
//	}{ Classe T_choice
//
void T_choice::B0_check(void)
{
  TRACE1("T_choice(%x)::B0_check()", this) ;
  ENTER_TRACE ;
  B0_check_substitution_list(first_body) ;
  T_item *cur = first_or ;
  while (cur != NULL)
	{
	  cur->B0_check() ;
	  cur = cur->get_next() ;
	}
  EXIT_TRACE ;
}

//
//	}{ Classe T_or
//
void T_or::B0_check(void)
{
  TRACE1("T_or(%x)::B0_check()", this) ;
  ENTER_TRACE ;
  B0_check_substitution_list(first_body) ;
  EXIT_TRACE ;
}

//
//	}{ Classe T_if
//
void T_if::B0_check(void)
{
  TRACE1("T_if(%x)::B0_check()", this) ;
  ENTER_TRACE ;

  B0_check_substitution_list(first_then_body) ;
  T_item *cur = first_else ;
  while (cur != NULL)
	{
	  cur->B0_check() ;
	  cur = cur->get_next() ;
	}
  EXIT_TRACE ;

  // Dans une implementation, le predicat du IF doit etre un predicat B0
  // (une condition)
  if (get_B0_disable_predicate_syntax_check() == TRUE) {return ;};
  if (check_is_in_an_implementation() == TRUE)
	{
	  if (get_Use_B0_Declaration() == FALSE)
		{
		  cond->syntax_check_is_a_B0_predicate(NULL);
		}
	  else
		{
		  cond->B0_OM_check();
		}
	};

}

//
//	}{ Classe T_else
//
void T_else::B0_check(void)
{
  TRACE1("T_else(%x)::B0_check()", this) ;
  ENTER_TRACE ;

  B0_check_substitution_list(first_body) ;
  EXIT_TRACE ;

  // Dans une implementation, le predicat du ELSIF doit etre un predicat B0
  // (une condition)
  if (get_B0_disable_predicate_syntax_check() == TRUE) {return ;};
  if ( (get_father()->check_is_in_an_implementation() == TRUE)
	   &&
	   (cond != NULL))
	{
	  if (get_Use_B0_Declaration() == FALSE)
		{
		  cond->syntax_check_is_a_B0_predicate(NULL);
		}
	  else
		{
		  cond->B0_OM_check();
		}
	};

}

//
//	}{ Classe T_when
//
void T_when::B0_check(void)
{
  TRACE1("T_when(%x)::B0_check()", this) ;
  ENTER_TRACE ;

  B0_check_substitution_list(first_body) ;
  EXIT_TRACE ;
}

//
//	}{ Classe T_assert
//
void T_assert::B0_check(void)
{
  TRACE1("T_assert(%x)::B0_check()", this) ;
  ENTER_TRACE ;
  //  predicate->B0_check() ;
  B0_check_substitution_list(first_body) ;
  EXIT_TRACE ;

}

//
//	}{ Classe T_label
//
void T_label::B0_check(void)
{
  TRACE1("T_label(%x)::B0_check()", this) ;
  ENTER_TRACE ;
  //  predicate->B0_check() ;
  B0_check_substitution_list(first_body) ;
  EXIT_TRACE ;

}

//
//	}{ Classe T_jumpif
//
void T_jumpif::B0_check(void)
{
  TRACE1("T_jumpif(%x)::B0_check()", this) ;
  ENTER_TRACE ;
  if (check_is_in_an_implementation() == TRUE)
        {
          if (get_Use_B0_Declaration() == FALSE)
                {
                  cond->syntax_check_is_a_B0_predicate(NULL);
                }
          else
                {
                  cond->B0_OM_check();
                }
        };
  EXIT_TRACE ;

}

//
//	}{ Classe T_witness
//
void T_witness::B0_check(void)
{
  TRACE1("T_witness(%x)::B0_check()", this) ;
  ENTER_TRACE ;
  //  predicate->B0_check() ;
  B0_check_substitution_list(first_body) ;
  EXIT_TRACE ;

}

//
//	}{ Classe T_select
//
void T_select::B0_check(void)
{
  TRACE1("T_select(%x)::B0_check()", this) ;
  ENTER_TRACE ;
  cond->B0_check() ;
  B0_check_substitution_list(first_then_body) ;
  T_item *cur = first_when ;
  while (cur != NULL)
	{
	  cur->B0_check() ;
	  cur = cur->get_next() ;
	}
  EXIT_TRACE ;
}

//
//	}{ Classe T_case
//

void T_case::B0_check(void)
{
  TRACE1("T_case(%x)::B0_check()", this) ;

  ENTER_TRACE ;
  //GP avec TESTS_SYNTAXE/REMPLACEMENT_US/MchTestExMU1E151.mch
  // segmentation fault car case_select = NULL
  TRACE1("case_select %x", case_select) ;

  T_item *cur = first_case_item ;
  while (cur != NULL)
	{
	  cur->B0_check() ;
	  cur = cur->get_next() ;
	}


  // Dans une implementation, l'expression de selection d'un CASE doit etre
  // un terme simple, ainsi que les expression des differents cas possibles
  // CTRL Ref : ESYNEXP 2
  if (get_B0_disable_expression_syntax_check() == TRUE) {return ;};
  if (check_is_in_an_implementation() == TRUE)
	{
	  if (case_select->syntax_check_is_a_simple_term(NULL) == 0)
		{
		 B0_syntax_error(case_select,
						 CAN_CONTINUE,
						 get_error_msg(E_B0_INVALID_CASE_SELECTOR),
						 make_class_name(case_select)) ;
		}
	}

}

//
//	}{ Classe T_case_item
//
void T_case_item::B0_check(void)
{
  TRACE1("T_case_item(%x)::B0_check()", this) ;


  ENTER_TRACE ;

  B0_check_substitution_list(first_body) ;
  EXIT_TRACE ;
}

//
//	}{ Classe T_any
//
void T_any::B0_check(void)
{
  TRACE1("T_any(%x)::B0_check()", this) ;

  // On commence par tout typer ...
  ENTER_TRACE ;
  TRACE0("Typage des idents") ;

  B0_check_substitution_list(first_body) ;
  EXIT_TRACE ;

}

static void T_var_B0_check(T_var *sub_var)
{
  TRACE1("T_var(%x)::B0_check", sub_var) ;

  TRACE0("type check du corps") ;
  B0_check_substitution_list(sub_var->get_body()) ;


  // Verification B0 du typage des variables locales
  if (get_B0_disable_locale_variables_type_check() != TRUE)
	{
	  local_variable_type_check_list(sub_var->get_identifiers());
	}

    // Verification de la non existence de cycles dans les affectations
    // concernant des variables locales, en implementation
    // On va verifier RINIT 1-1
  // On positionne la variable globale check a 1
  if (get_B0_disable_variables_initialisation_check() != TRUE)
	{
	  if (sub_var->check_is_in_an_implementation() == TRUE)
		{
		  set_check(B0CHK_INIT);

		  // On construit la liste des identificateurs a initialiser
		  // Cette liste est stockee dans la variable globale
		  // current_list_ident
		  T_list_ident *list = new T_list_ident();

		  T_ident *cur_var = sub_var->get_identifiers();
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
		  initialisation_check_list(sub_var->get_body(),
									&list_initialised_ident);


		  // On repositionne le flag global a 0 (aucune verification)
		  set_check(B0CHK_DEFAULT);
		}
	}
}

static void T_var_B0_OM_check(T_var *sub_var)
{

  // On v�rifie les d�clarations des variables locales L'intialisation
  // �ventuelle ou necessaire est control�e � cette ocasion
  T_ident *cur_var = sub_var->get_identifiers();
  while (cur_var != NULL)
	{
	  cur_var->B0_check_decl();
	  cur_var = (T_ident *)cur_var->get_next();
	}
  B0_check_substitution_list(sub_var->get_body()) ;
}

// Classe T_var : on type !
void T_var::B0_check(void)
{
  if (get_Use_B0_Declaration() == FALSE)
	{
	  T_var_B0_check(this);
	}
  else
	{
	  T_var_B0_OM_check(this);
	}
}

// Classe T_begin : on type !
void T_begin::B0_check(void)
{
  TRACE1("T_begin(%x)::B0_check", this) ;

  // Le corps
  TRACE0("type check du corps") ;
  B0_check_substitution_list(first_body) ;
}

//
//	}{ Classe T_let
//
void T_let::B0_check(void)
{
  TRACE1("T_let(%x)::B0_check()", this) ;



  EXIT_TRACE ;
}

//
//	}{ Classe T_while
//
void T_while::B0_check(void)
{
  TRACE1("T_while(%x)::B0_check()", this) ;

  ENTER_TRACE ;

  B0_check_substitution_list(first_body) ;

  // Dans une implementation, le predicat de condition d'un WHILE doit etre
  // un predicat B0 (une condition)
  // CTRL Ref : ESYNPRE 1
  // CTRL Ref : ESYNEXP 1
  if (get_B0_disable_predicate_syntax_check() == TRUE) {return ;};
  if (get_Use_B0_Declaration() == FALSE)
	{
	  cond->syntax_check_is_a_B0_predicate(NULL);
	}
  else
	{
	  cond->B0_OM_check();
	}

}


//
//	}{	Classe T_becomes_such_that
//

// Classe T_becomes_such_that : on type !
void T_becomes_such_that::B0_check(void)
{
  TRACE1("T_becomes_such_that(%x)::B0_check", this) ;

  // !!!!!!!!! control B0 ???
}

//
// Type check d'une liste de substitutions
//
void B0_check_substitution_list(T_substitution *list)
{
  TRACE1("B0_check_substitution_list(%x)", list) ;

  T_substitution *cur = list ;

  //(1) type_check de la liste
  while (cur != NULL)
	{
	  cur->B0_check() ;
	  cur = (T_substitution *)cur->get_next() ;
	}
}


//
//	}{	Fin du B0 check des substitutions
//
