/******************************* CLEARSY **************************************
* Fichier : $Id: t_opres.cpp,v 2.0 2001-07-19 16:32:23 lv Exp $
* (C) 2008-2025 CLEARSY
*
* Description :		Compilateur B
*					Fabrication du type des expressions pour les sequences
*
This file is part of B_COMPILER
    Copyright (C) 2008-2025 CLEARSY (contact@clearsy.com)

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
RCS_ID("$Id: t_opres.cpp,v 1.22 2001-07-19 16:32:23 lv Exp $") ;

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


// Fonction generique de calcul de type
void T_op_result::internal_make_type(void)
{
  if (ref_array_item != NULL)
	{
	  // En fait c'est un T_array_item qu'on a mal corrige
	  ref_array_item->make_type() ;
	  set_B_type(ref_array_item->get_B_type(), get_ref_lexem()) ;
	  return ;
	}

  TRACE2("T_op_result(%x::%s)::make_type",
		 this,
		 (op_name->is_an_ident() == TRUE)
		 ? ((T_ident *)op_name)->get_name()->get_value()
		 : op_name->get_class_name()) ;


  if (ref_op == NULL)
	{
	  TRACE0("c'est un builtin") ;
	  if (ref_builtin == NULL)
		{
		  TRACE1("panic this %x ref_builtin = NULL, ref_op = NULL", this) ;
		  fprintf(stderr,
                                  "%s:%d: panic this %p ref_builtin = NULL, ref_op = NULL\n",
				  __FILE__, __LINE__,
                                  this) ;
		  assert(FALSE) ;
		  return ;
		}

	  // On calcule le type des parametres, puis on verifie la compatibilite,
	  // puis on fait le type
	  // Seul cas particulier : bool() car bool prend en parametre un predicat
	  // dont on ne cherche pas le type ...

	  assert(ref_builtin != NULL) ; // A FAIRE
	  TRACE1("ref_builtin = %x", ref_builtin) ;
	  if (ref_builtin->get_lex_type() == L_BOOL)
		{
		  vtype_bool() ;
		  return ;
		}

	  // Calcul du type des parametres
	  TRACE0("Calcul du type des parametres") ;
	  ENTER_TRACE ; ENTER_TRACE ;

	  T_item *cur_param = first_in_param ;
	  while (cur_param != NULL)
		{
		  cur_param->make_type() ;
		  cur_param = cur_param->get_next() ;
		}

	  EXIT_TRACE ; EXIT_TRACE ;
	  TRACE0("Verification de la compatibilite du type des parametres") ;

	  switch (ref_builtin->get_lex_type())
		{
		case L_POW :
		case L_POW1 :
		case L_FIN :
		case L_FIN1 :
		  {
			vtype_pow_fin(ref_builtin) ;
			return ;
		  }

		case L_MAX :
		case L_MIN :
		  {
			vtype_min_max(ref_builtin) ;
			return ;
          }
		case L_CARD :
		  {
			vtype_card(ref_builtin) ;
			return ;
		  }
		case L_REALOP:
		{
		  vtype_from_integer_to_real(ref_builtin);
		  return;
		}
		case L_FLOOR:
		case L_CEILING:
		{
		  vtype_from_real_to_integer(ref_builtin);
		  return;
		}
		case L_SEQUENCE :
		case L_NON_EMPTY_SEQUENCE :
		case L_INJECTIVE_SEQ :
		case L_NON_EMPTY_INJECTIVE_SEQ :
		case L_PERMUTATION :
		  {
			vtype_seq_iseq_perm(ref_builtin) ;
			return ;
		  }

		case L_FRONT :
		case L_TAIL :
		case L_REV :
		  {
			vtype_front_tail_rev(ref_builtin) ;
			return ;
		  }

		case L_FIRST :
		case L_LAST :
		  {
			vtype_first_last(ref_builtin) ;
			return ;
		  }

		case L_SIZE :
		  {
			vtype_size(ref_builtin) ;
			return ;
		  }

		case L_ID :
		  {
			vtype_id(ref_builtin) ;
			return ;
		  }

		case L_DOM :
		  {
			vtype_dom(ref_builtin) ;
			return ;
		  }

		case L_RAN :
		  {
			vtype_ran(ref_builtin) ;
			return ;
		  }

		case L_CLOSURE :
		case L_CLOSURE1 :
		  {
			vtype_closure(ref_builtin) ;
			return ;
		  }

		case L_ITERATE :
		  {
			vtype_iterate(ref_builtin) ;
			return ;
		  }

		case L_PRJ1 :
		  {
			vtype_prj(ref_builtin, TRUE) ;
			return ;
		  }

		case L_PRJ2 :
		  {
			vtype_prj(ref_builtin, FALSE) ;
			return ;
		  }

		case L_GENERALISED_CONCAT :
		  {
			vtype_conc(ref_builtin) ;
			return ;
		  }

		case L_SUCC :
		case L_PRED :
		  {
			vtype_succ_pred(ref_builtin) ;
			return ;
		  }

		case L_TRANS_FUNC :
		  {
			vtype_fnc(ref_builtin) ;
			return ;
			}

		case L_TRANS_REL :
		  {
			vtype_rel(ref_builtin) ;
			return ;
		  }

		  /* GP 9/02/99 Evolution des arbres */
		case L_TREE :
		case L_BTREE :
		  {
			vtype_tree(ref_builtin) ;
			return ;
		  }

		case L_CONST :
		  {
			vtype_const(ref_builtin) ;
			return ;
		  }

		case L_TOP :
		  {
			vtype_top(ref_builtin) ;
			return ;
		  }

		case L_SONS :
		  {
			vtype_sons(ref_builtin) ;
			return ;
		  }

		case L_PREFIX :
		case L_POSTFIX :
		  {
			vtype_pre_post_fix(ref_builtin) ;
			return ;
		  }

		case L_SIZET :
		  {
			vtype_sizet(ref_builtin) ;
			return ;
		  }

		case L_MIRROR :
		  {
			vtype_mirror(ref_builtin) ;
			return ;
		  }

		case L_RANK :
		  {
			vtype_rank(ref_builtin) ;
			return ;
		  }

		case L_FATHER :
		  {
			vtype_father(ref_builtin) ;
			return ;
		  }

		case L_SON :
		  {
			vtype_son(ref_builtin) ;
			return ;
		  }

		case L_SUBTREE :
		  {
			vtype_subtree(ref_builtin) ;
			return ;
		  }

		case L_ARITY :
		  {
			vtype_arity(ref_builtin) ;
			return ;
		  }

		case L_BIN :
		  {
			vtype_bin(ref_builtin) ;
			return ;
		  }

		case L_LEFT :
		case L_RIGHT :
		  {
			vtype_left_right(ref_builtin) ;
			return ;
		  }

		case L_INFIX :
		  {
			vtype_infix(ref_builtin) ;
			return ;
		  }

		default :
		  {
			internal_error(get_ref_lexem(),
						   __FILE__,
						   __LINE__,
						   "Type check of builtin function %s \
is not yet implemented",
						   get_lex_type_ascii(ref_builtin->get_lex_type())) ;
			return ;
		  }
		}
	}

}

// Fonction auxiliaire qui verifie qu'il n'y a qu'un seul argument
// a la fonction et qui calcule son type.
// Renvoie TRUE si ok, FALSE sinon
int T_op_result::check_single_param(T_keyword *keyword)
{
  if (nb_in_params != 1)
	{
	  // Il faut un seul parametre ...
	  semantic_error(this,
					 CAN_CONTINUE,
					 get_error_msg(E_BUILTIN_OP_HAS_ONLY_ONE_OPERAND),
					 get_lex_type_ascii(keyword->get_lex_type()),
					 nb_in_params) ;

	  return FALSE ;
	}

  first_in_param->make_type() ;
  return TRUE ;
}

// CTRL Ref: VTYPE 2-3 (3.1, 3.2, 3.3, 3.4)
// Un seul parametre, de type ensemble. Le resultat
// est de type Setof(type_de_parametre)
void T_op_result::vtype_pow_fin(T_keyword *keyword)
{
  if (check_single_param(keyword) == FALSE)
	{
	  // IL faut un seul parametre
	  return ;
	}
  else if (first_in_param->is_a_set() == FALSE)
	{
	  // ... et de type ensemble
	  semantic_error(this,
					 CAN_CONTINUE,
					 get_error_msg(E_OPERAND_OF_BUILTIN_OP_IS_NOT_A_SET),
					 get_lex_type_ascii(keyword->get_lex_type()),
					 make_type_name(first_in_param)->get_value()) ;
	}
  else
	{
	  // Tout va bien !
	  TRACE0("Ok, construction setof(type)") ;
	  T_type *type = first_in_param->get_B_type() ;
	  T_type *base = type->clone_type(NULL,
									  get_betree(),
									  get_ref_lexem()) ;
	  T_type *set = new T_setof_type(base,
									 this,
									 get_betree(),
									 get_ref_lexem()) ;
	  base->unlink() ;
	  set_B_type(set, get_ref_lexem()) ;
	  set->unlink() ;
	}

}

// CTRL Ref: VTYPE 2-1 (3.1, 3.2)
// Un seul parametre, de type ensemble d'entiers. Le resultat
// est de type entier
void T_op_result::vtype_min_max(T_keyword *keyword)
{
    int is_integer_set = FALSE;

  if (check_single_param(keyword) == FALSE)
	{
	  // IL faut un seul parametre
	  return ;
	}

  is_integer_set = first_in_param->is_an_integer_set();

  if (is_integer_set == TRUE)
	{
      // Tout va bien !
      TRACE0("Ok, construction type") ;
          T_type *type;

          type = new T_predefined_type(BTYPE_INTEGER,
                                           NULL,
                                           NULL,
                                           this,
                                           get_betree(),
                                           get_ref_lexem()) ;

          set_B_type(type, get_ref_lexem()) ;
          type->unlink() ;
        }
  else if (first_in_param->is_a_real_set() == TRUE) {
      T_type *type;

      type = new T_predefined_type(BTYPE_REAL,
                                       NULL,
                                       NULL,
                                       this,
                                       get_betree(),
                                       get_ref_lexem()) ;

      set_B_type(type, get_ref_lexem()) ;
      type->unlink() ;
	}
  else
	{
semantic_error(this,
               CAN_CONTINUE,
                                   get_error_msg(E_OPERAND_OF_BUILTIN_OP_IS_NOT_AN_INT_SET),
               get_lex_type_ascii(keyword->get_lex_type()),
               make_type_name(first_in_param)->get_value()) ;
}
}



// CTRL Ref: VTYPE 2-1 (3.3)
// Un seul parametre, de type ensemble. Le resultat
// est de type entier
void T_op_result::vtype_card(T_keyword *keyword)
{
  if (check_single_param(keyword) == FALSE)
	{
	  // IL faut un seul parametre
	  return ;
	}
  if (first_in_param->is_a_set() == FALSE)
	{
	  // ... et de type ensemble
	  semantic_error(this,
					 CAN_CONTINUE,
                                         get_error_msg(E_OPERAND_OF_BUILTIN_OP_IS_NOT_A_SET),
					 get_lex_type_ascii(keyword->get_lex_type()),
					 make_type_name(first_in_param)->get_value()) ;
	}
  else
	{
	  // Tout va bien !
	  TRACE0("Ok, construction type") ;
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


// CTRL Ref: VTYPE 2-6 (1)
// Un seul parametre, de type ensemble. Le resultat
// est de type entier
void T_op_result::vtype_seq_iseq_perm(T_keyword *keyword)
{
  TRACE2("T_op_result(%x)::vtype_seq_iseq_perm(%x)", this, keyword) ;
  if (check_single_param(keyword) == FALSE)
	{
	  // IL faut un seul parametre
	  return ;
	}
  if (first_in_param->is_a_set() == FALSE)
	{
	  // ... et de type ensemble
	  semantic_error(this,
					 CAN_CONTINUE,
					 get_error_msg(E_OPERAND_OF_BUILTIN_OP_IS_NOT_A_SET),
					 get_lex_type_ascii(keyword->get_lex_type()),
					 make_type_name(first_in_param)->get_value()) ;

	  return ;
	}

  // Tout va bien !
  // seq(E) : On construit le type Setof(Setof(INTEGER * Type(E)))
  TRACE0("-- DEBUT CONSTRUCTION TYPE Setof(Setof(INTEGER * Type(E)))") ;
  ENTER_TRACE ;

  // Ajout de "INTEGER" dans le produit cartesien
  T_type *type_int = new T_predefined_type(BTYPE_INTEGER,
										   NULL,
										   NULL,
										   this,
										   get_betree(),
										   get_ref_lexem()) ;

  // Ajout de "type de E" dans le produit cartesien
  // Cast justifie par le if
  T_setof_type *setof = (T_setof_type *)first_in_param->get_B_type() ;

  T_type *clone = setof->get_base_type()->clone_type(NULL,
													 get_betree(),
													 get_ref_lexem()) ;

  // Produit cartesien
  T_product_type *prod = new T_product_type(type_int,
											clone,
											NULL,
											get_betree(),
											get_ref_lexem()) ;
  type_int->unlink() ;
  clone->unlink() ;

  T_type *set1 = new T_setof_type(prod, NULL, get_betree(), get_ref_lexem()) ;
  prod->unlink() ;
  T_type *set2 = new T_setof_type(set1, NULL, get_betree(), get_ref_lexem()) ;
  set1->unlink() ;
  set_B_type(set2, get_ref_lexem()) ;
  set2->unlink() ;

  EXIT_TRACE ;
  TRACE0("-- FIN CONSTRUCTION TYPE") ;
}

// CTRL Ref: VTYPE 2-6 (3) (4)
// Un seul parametre, de type ensemble Seotf(ENS). Le resultat
// est de type Setof(Z * ENS) avec ENS atomique
void T_op_result::vtype_front_tail_rev(T_keyword *keyword)
{
  TRACE2("T_op_result(%x)::vtype_front_tail_rev(%x)", this, keyword) ;
  ENTER_TRACE ;

  if (check_single_param(keyword) == FALSE)
	{
	  // IL faut un seul parametre
	  return ;
	}
  if (first_in_param->is_a_non_empty_seq() == FALSE)
	{
	  // ... et de type ensemble
	  semantic_error(this,
					 CAN_CONTINUE,
					 get_error_msg(E_OPERAND_OF_BUILTIN_OP_IS_NOT_A_NON_EMPTY_SEQ),
					 get_lex_type_ascii(keyword->get_lex_type()),
					 make_type_name(first_in_param)->get_value()) ;

	  return ;
	}

  // Ajout de Z dans prod
  T_type *z = new T_predefined_type(BTYPE_INTEGER,
									NULL,
									NULL,
									NULL,
									get_betree(),
									get_ref_lexem()) ;

  // Ajout de ENS dans prod
  T_setof_type *type = (T_setof_type *)first_in_param->get_B_type() ;
  T_type *base_type = type->get_seq_base_type() ;
  T_type *clone = base_type->clone_type(NULL, get_betree(), get_ref_lexem()) ;

  T_product_type *prod ;
  prod = new T_product_type(z, clone, NULL, get_betree(), get_ref_lexem()) ;
  z->unlink() ;
  clone->unlink() ;

  // Fabrication du setof
  T_setof_type *res ;
  res = new T_setof_type(prod, this, get_betree(), get_ref_lexem()) ;
  prod->unlink() ;

  // C'est tout !
  set_B_type(res, get_ref_lexem()) ;
  res->unlink() ;
  EXIT_TRACE ;
}

// CTRL Ref: VTYPE 2-6 (2)
// Un seul parametre, de type sequence d'elements de F.
// Le resultat est de type element de F.
void T_op_result::vtype_first_last(T_keyword *keyword)
{
  TRACE2("T_op_result(%x)::vtype_first_last(%x)", this, keyword) ;
  ENTER_TRACE ;

  if (check_single_param(keyword) == FALSE)
	{
	  // IL faut un seul parametre
	  return ;
	}
  if (first_in_param->is_a_non_empty_seq() == FALSE)
	{
	  // ... et de type ensemble
	  semantic_error(this,
					 CAN_CONTINUE,
					 get_error_msg(E_OPERAND_OF_BUILTIN_OP_IS_NOT_A_NON_EMPTY_SEQ),
					 get_lex_type_ascii(keyword->get_lex_type()),
					 make_type_name(first_in_param)->get_value()) ;

	  return ;
	}

  T_setof_type *type = (T_setof_type *)first_in_param->get_B_type() ;
  T_type *base_type = type->get_seq_base_type() ;
  T_type *res = base_type->clone_type(this,
									  get_betree(),
									  get_ref_lexem()) ;
  set_B_type(res, get_ref_lexem()) ;
  res->unlink() ;
  EXIT_TRACE ;
}

// CTRL Ref: VTYPE 2-6 (5)
// Un seul parametre, de type sequence. Le resultat est de type INTEGER
void T_op_result::vtype_size(T_keyword *keyword)
{
  TRACE2("T_op_result(%x)::vtype_size(%x)", this, keyword) ;
  ENTER_TRACE ;

  if (check_single_param(keyword) == FALSE)
	{
	  // IL faut un seul parametre
	  return ;
	}

  if (first_in_param->is_a_seq() == FALSE)
	{
	  // ... et de type ensemble
	  semantic_error(this,
					 CAN_CONTINUE,
					 get_error_msg(E_OPERAND_OF_BUILTIN_OP_IS_NOT_A_NON_EMPTY_SEQ),
					 get_lex_type_ascii(keyword->get_lex_type()),
					 make_type_name(first_in_param)->get_value()) ;

	  return ;
	}

  T_type *res = new T_predefined_type(BTYPE_INTEGER,
									  NULL,
									  NULL,
									  this,
									  get_betree(),
									  get_ref_lexem()) ;

  set_B_type(res, get_ref_lexem()) ;
  res->unlink() ;
  EXIT_TRACE ;
}

// CTRL Ref: VTYPE 2-4 (2)
// Un seul parametre, de type ensemble. Le resultat
// Setof(type * type)
void T_op_result::vtype_id(T_keyword *keyword)
{
  if (check_single_param(keyword) == FALSE)
	{
	  // IL faut un seul parametre
	  return ;
	}
  if (first_in_param->is_a_set() == FALSE)
	{
	  // ... et de type ensemble
	  semantic_error(this,
					 CAN_CONTINUE,
					 get_error_msg(E_OPERAND_OF_BUILTIN_OP_IS_NOT_A_SET),
					 get_lex_type_ascii(keyword->get_lex_type()),
					 make_type_name(first_in_param)->get_value()) ;
	  return ;
	}

  // On y va
  // Cast justifie par le test ci-dessus
  T_type *base_type = ((T_setof_type *)first_in_param->get_B_type())
	->get_base_type() ;

  T_type *c1 = base_type->clone_type(NULL,  get_betree(), get_ref_lexem()) ;
  T_type *c2 = base_type->clone_type(NULL, get_betree(), get_ref_lexem()) ;
  T_product_type *prod ;
  prod = new T_product_type(c1, c2, NULL, get_betree(), get_ref_lexem()) ;
  c1->unlink() ;
  c2->unlink() ;

  T_setof_type *res ;
  res = new T_setof_type(prod, this, get_betree(), get_ref_lexem()) ;
  prod->unlink() ;
  set_B_type(res, get_ref_lexem()) ;
  res->unlink() ;
}


// CTRL Ref: VTYPE 2-4 (6)
// Un seul parametre, de type relation de A vers B.. Le resultat
// est de type Setof(type(A))
void T_op_result::vtype_dom(T_keyword *keyword)
{
  TRACE1("T_op_result(%x)::vtype_dom", this) ;
  if (check_single_param(keyword) == FALSE)
	{
	  // IL faut un seul parametre
	  return ;
	}

  if (first_in_param->is_a_relation() == FALSE)
	{
	  // ... et de type relation
	  semantic_error(this,
					 CAN_CONTINUE,
					 get_error_msg(E_OPERAND_OF_BUILTIN_OP_IS_NOT_A_RELATION),
					 get_lex_type_ascii(keyword->get_lex_type()),
					 make_type_name(first_in_param)->get_value()) ;
	  return ;
	}

  // On y va
  T_setof_type *rtype = (T_setof_type *)(first_in_param->get_B_type()) ;
  T_type *src = rtype->get_relation_src_type() ;
  TRACE1("ici src=%s", src->make_type_name()->get_value()) ;
  T_type *clone = src->clone_type(NULL, get_betree(), get_ref_lexem()) ;

  T_setof_type *res ;
  res = new T_setof_type(clone, this, get_betree(), get_ref_lexem()) ;
  clone->unlink() ;
  set_B_type(res, get_ref_lexem()) ;
  res->unlink() ;
}


// CTRL Ref: VTYPE 2-4 (7)
// Un seul parametre, de type relation de A vers B.. Le resultat
// est de type Setof(type(B))
void T_op_result::vtype_ran(T_keyword *keyword)
{
  if (check_single_param(keyword) == FALSE)
	{
	  // IL faut un seul parametre
	  return ;
	}
  if (first_in_param->is_a_relation() == FALSE)
	{
	  // ... et de type relation
	  semantic_error(this,
					 CAN_CONTINUE,
					 get_error_msg(E_OPERAND_OF_BUILTIN_OP_IS_NOT_A_RELATION),
					 get_lex_type_ascii(keyword->get_lex_type()),
					 make_type_name(first_in_param)->get_value()) ;
	  return ;
	}

  // On y va
  T_setof_type *rtype = (T_setof_type *)(first_in_param->get_B_type()) ;
  T_type *src = rtype->get_relation_dst_type() ;
  T_type *clone = src->clone_type(NULL, get_betree(), get_ref_lexem()) ;
  T_setof_type *res ;
  res = new T_setof_type(src, this, get_betree(), get_ref_lexem()) ;
  clone->unlink() ;
  set_B_type(res, get_ref_lexem()) ;
  res->unlink() ;
}


// CTRL Ref: VTYPE 2-4 (14)
// Un seul parametre, de type relation de A vers A.. Le resultat
// est de type Setof(type(A)*type(A))
void T_op_result::vtype_closure(T_keyword *keyword)
{
  if (check_single_param(keyword) == FALSE)
	{
	  // IL faut un seul parametre ...
	  return ;
	}
  if (first_in_param->is_a_relation() == FALSE)
	{
	  // ... et de type relation ...
	  semantic_error(this,
					 CAN_CONTINUE,
					 get_error_msg(E_OPERAND_OF_BUILTIN_OP_IS_NOT_A_RELATION),
					 get_lex_type_ascii(keyword->get_lex_type()),
					 make_type_name(first_in_param)->get_value()) ;
	  return ;
	}

  // Cast justie ci-dessus
  T_setof_type *set = (T_setof_type *)(first_in_param->get_B_type()) ;
  T_type *first_type = set->get_relation_src_type() ;
  T_type *second_type = set->get_relation_dst_type() ;

  if (first_type->is_equal_to(second_type) == FALSE)
	{
	  semantic_error(this,
					 CAN_CONTINUE,
					 get_error_msg(
								   E_OPERAND_OF_BUILTIN_OP_IS_NOT_A_RELATION_FROM_A_TO_A),
					 get_lex_type_ascii(keyword->get_lex_type()),
					 make_type_name(first_in_param)->get_value()) ;
	}

  // On construit le type
  T_type *c1 = first_type->clone_type(NULL, get_betree(), get_ref_lexem()) ;
  T_type *c2 = first_type->clone_type(NULL, get_betree(), get_ref_lexem()) ;

  T_product_type *prod ;
  prod = new T_product_type(c1, c2, NULL, get_betree(), get_ref_lexem()) ;
  c1->unlink() ;
  c2->unlink() ;

  T_type *res = new T_setof_type(prod, this, get_betree(), get_ref_lexem()) ;
  prod->unlink() ;
  set_B_type(res, get_ref_lexem()) ;
  res->unlink() ;
}


// CTRL Ref: VTYPE 2-4 (13)
// Deux parametres, de type relation de A vers A et entier.. Le resultat
// est de type Setof(type(A)*type(A))
void T_op_result::vtype_iterate(T_keyword *keyword)
{
  if (nb_in_params != 2)
	{
	  // Il faut deux parametres ...
	  semantic_error(this,
					 CAN_CONTINUE,
					 get_error_msg(E_BUILTIN_OP_NEEDS_TWO_OPERANDS),
					 get_lex_type_ascii(keyword->get_lex_type()),
					 nb_in_params) ;

	  return ;
	}

  first_in_param->make_type() ;
  last_in_param->make_type() ;

  if (first_in_param->get_B_type() == NULL ||
	  last_in_param->get_B_type()  == NULL)
	{
	  /* Reprise sur erreur */
	  return;
	}

  if (first_in_param->is_a_relation() == FALSE)
	{
	  // le premier de type relation ...
	  semantic_error(this,
					 CAN_CONTINUE,
					 get_error_msg(E_FIRST_OPERAND_OF_BUILTIN_OP_IS_NOT_A_RELATION),
					 get_lex_type_ascii(keyword->get_lex_type()),
					 make_type_name(first_in_param)->get_value()) ;
	  return ;
	}

  // Cast justie ci-dessus
  T_setof_type *set = (T_setof_type *)(first_in_param->get_B_type()) ;
  T_type *first_type = set->get_relation_src_type() ;
  T_type *second_type = set->get_relation_dst_type() ;

  if (first_type->is_equal_to(second_type) == FALSE)
	{
	  semantic_error(this,
					 CAN_CONTINUE,
					 get_error_msg(
								   E_FIRST_OPERAND_OF_BUILTIN_OP_IS_NOT_A_RELATION_FROM_A_TO_A),
					 get_lex_type_ascii(keyword->get_lex_type()),
					 make_type_name(first_in_param)->get_value()) ;
	}

  if (last_in_param->get_B_type()->is_an_integer() == FALSE)
	{
	  // le deuxieme de type entier ...
	  semantic_error(this,
					 CAN_CONTINUE,
					 get_error_msg(E_SECOND_OPERAND_OF_BUILTIN_OP_IS_NOT_AN_INTEGER),
					 get_lex_type_ascii(keyword->get_lex_type()),
                     make_type_name(last_in_param)->get_value()) ;
	  return ;
	}

  // On construit le type
  // On construit le type
  T_type *c1 = first_type->clone_type(NULL, get_betree(), get_ref_lexem()) ;
  T_type *c2 = first_type->clone_type(NULL, get_betree(), get_ref_lexem()) ;

  T_product_type *prod ;
  prod = new T_product_type(c1, c2, NULL, get_betree(), get_ref_lexem()) ;
  c1->unlink() ;
  c2->unlink() ;

  T_type *res = new T_setof_type(prod, this, get_betree(), get_ref_lexem()) ;
  prod->unlink() ;
  set_B_type(res, get_ref_lexem()) ;
  res->unlink() ;
}


// CTRL Ref: VTYPE 2-2 (2)
// Deux parametres A et B de type ensemble
// Le resultat est de type Setof(A*B*A) si is_prj1 = TRUE, Setof(A*B*B) sinon
void T_op_result::vtype_prj(T_keyword *keyword, int is_prj1)
{
  TRACE2("T_op_result(%x):vtype_prj(%x)", this, keyword) ;

  if (nb_in_params != 2)
	{
	  // Il faut deux parametres ...
	  semantic_error(this,
					 CAN_CONTINUE,
					 get_error_msg(E_BUILTIN_OP_NEEDS_TWO_OPERANDS),
					 get_lex_type_ascii(keyword->get_lex_type()),
					 nb_in_params) ;

	  return ;
	}

  first_in_param->make_type() ;
  last_in_param->make_type() ;

  if (first_in_param->is_a_set() == FALSE)
	{
	  // ... et de type ensemble
	  semantic_error(this,
					 CAN_CONTINUE,
					 get_error_msg(E_FIRST_OPERAND_OF_BUILTIN_OP_IS_NOT_A_SET),
					 get_lex_type_ascii(keyword->get_lex_type()),
					 make_type_name(first_in_param)->get_value()) ;
	  return ;
	}

  if (last_in_param->is_a_set() == FALSE)
	{
	  // ... et de type ensemble
	  semantic_error(this,
					 CAN_CONTINUE,
					 get_error_msg(E_SECOND_OPERAND_OF_BUILTIN_OP_IS_NOT_A_SET),
					 get_lex_type_ascii(keyword->get_lex_type()),
					 make_type_name(last_in_param)->get_value()) ;
	  return ;
	}

  // On fabrique le type
  // On ajoute A, B, A
  // Cast justifie par les tests ci-dessus
  T_setof_type *sa = (T_setof_type *)first_in_param->get_B_type() ;
  T_type *a = sa->get_base_type() ;
  T_type *t1 = a->clone_type(NULL, get_betree(), get_ref_lexem()) ;

  T_setof_type *sb = (T_setof_type *)last_in_param->get_B_type() ;
  T_type *b = sb->get_base_type() ;
  T_type *t2 = b->clone_type(NULL, get_betree(), get_ref_lexem()) ;

  T_type *t3 = NULL ;
  if (is_prj1 == TRUE)
	{
	  t3 = a->clone_type(NULL, get_betree(), get_ref_lexem()) ;
	}
  else
	{
	  t3 = b->clone_type(NULL, get_betree(), get_ref_lexem()) ;
	}

  T_product_type *p1 ;
  p1 = new T_product_type(t1, t2, NULL, get_betree(), get_ref_lexem()) ;
  t1->unlink() ;
  t2->unlink() ;

  T_product_type *p2 ;
  p2 = new T_product_type(p1, t3, NULL, get_betree(), get_ref_lexem()) ;
  p1->unlink() ;
  t3->unlink() ;

  T_type *res = new T_setof_type(p2, this, get_betree(), get_ref_lexem()) ;
  p2->unlink() ;
  set_B_type(res, get_ref_lexem()) ;
  res->unlink() ;
}

// Type Bool
void T_op_result::vtype_bool(void)
{
  TRACE1("T_op_result(%x):vtype_bool()", this) ;

  // Debut correction anomalie 2161
  // Il faut d'abord verifier le type des parametres
  T_item *cur_param = first_in_param ;

  while (cur_param != NULL)
	{
	  cur_param->type_check() ;
	  cur_param = cur_param->get_next() ;
	}

  // Fin correction anomalie 2161

  T_predefined_type *res = new T_predefined_type(BTYPE_BOOL,
												 NULL,
												 NULL,
												 this,
												 get_betree(),
												 get_ref_lexem()) ;
  set_B_type(res, get_ref_lexem()) ;
  res->unlink() ;
}

// Type succ, pred
// CTRL Ref: VTYPE 2-1 (2)
void T_op_result::vtype_succ_pred(T_keyword *keyword)
{
  TRACE1("T_op_result(%x):vtype_succ_pred()", this) ;

  // Il faut un seul parametre ...
  check_single_param(keyword) ;

  // ... et de type entier
  if ( (first_in_param->get_B_type() == NULL)
	   || (first_in_param->get_B_type()->is_an_integer() == FALSE) )
	{
	  // ... et de type relation
	  semantic_error(this,
					 CAN_CONTINUE,
					 get_error_msg(E_OPERAND_OF_BUILTIN_OP_IS_NOT_AN_INTEGER),
					 get_lex_type_ascii(keyword->get_lex_type()),
					 make_type_name(first_in_param)->get_value()) ;
	  return ;
	}

  // Le resultat est de type entier
   T_predefined_type *res = new T_predefined_type(BTYPE_INTEGER,
												 NULL,
												 NULL,
												 this,
												 get_betree(),
												 get_ref_lexem()) ;

 set_B_type(res, get_ref_lexem()) ;
  res->unlink() ;
}


// CTRL Ref: VTYPE 2-6 (10)
// Un seul parametre, de type Suite de Suites. Le resultat
// est de type Suite

// GP 14/04/99 correction bug 2165
// un seul parametre pour conc(S)
// Le type du parametre doit etre P(Z*P(Z2*T))
void T_op_result::vtype_conc(T_keyword *keyword)
{
  TRACE1("T_op_result(%x)::vtype_conc(%x)", this) ;

  // Il faut un seul parametre S ...
  if (check_single_param(keyword) == FALSE)
	{
	  return ;
	}


  T_type *param_type = first_in_param->get_B_type() ;
  T_type *conc_type = NULL ; // type qui servira à construire
  // le type de conc: sequence vide ou sequence suivant le type de S

  // (1) S doit deja etre du type P(Z*X)
  if (first_in_param->is_a_seq() == FALSE)
	{
	  semantic_error(first_in_param,
					 CAN_CONTINUE,
					get_error_msg(E_OPERAND_OF_BUILTIN_OP_IS_NOT_A_SEQ_OF_SEQ),
					 get_lex_type_ascii(keyword->get_lex_type()),
					 make_type_name(first_in_param)->get_value()) ;
	  return ;
	}


  //  2 Cas possibles
  // (1) param_type n'est pas une sequence vide
  // (2) param_type est une sequence vide



  if (param_type->is_a_non_empty_seq() == TRUE)
	{
	  // Les 2 Casts sont vrais car S est une sequence non vide,
	  // donc du type P(Z*X)
	  T_setof_type* setof = (T_setof_type*)param_type ;
	  //prod = Z*X
	  T_product_type* prod = (T_product_type*)setof->get_base_type() ;

	  //(1) param_type n'est pas une sequence vide
	  T_type *type_X = prod->get_type2() ;

	  // Le type de X doit etre une sequence
	  if (type_X->is_a_seq() == FALSE)
		{
	  	  semantic_error(first_in_param,
						 CAN_CONTINUE,
						 get_error_msg
						 (E_OPERAND_OF_BUILTIN_OP_IS_NOT_A_SEQ_OF_SEQ),
						 get_lex_type_ascii(keyword->get_lex_type()),
						 make_type_name(first_in_param)->get_value()) ;
		  return ;
		}

	  conc_type = type_X ;
	}
  else
	{
	  // (2) param_type est une sequence vide
	  // Rien a verifie c'est autorise

	  conc_type = param_type ;
	}

  // Le type de conc est
  T_type* res = conc_type->clone_type(this,
									  get_betree(),
									  get_ref_lexem()) ;

  set_B_type(res, get_ref_lexem()) ;
  res->unlink() ;
  TRACE0("-- FIN CONSTRUCTION TYPE") ;


}


// CTRL Ref: ???
// Un seul parametre, de type relation d'ensembles S(T1*S(T2))
// Le resultat est de type S(T1*S(T2))
void T_op_result::vtype_rel(T_keyword *keyword)
{
  TRACE1("T_op_result(%x)::vtype_rel", this) ;
  if (check_single_param(keyword) == FALSE)
	{
	  // IL faut un seul parametre
	  return ;
	}

  if (first_in_param->is_a_relation() == FALSE)
	{
	  // ... et de type relation
	  semantic_error(this,
					 CAN_CONTINUE,
					 get_error_msg(E_OPERAND_OF_BUILTIN_OP_IS_NOT_A_RELATION),
					 get_lex_type_ascii(keyword->get_lex_type()),
					 make_type_name(first_in_param)->get_value()) ;
	  return ;
	}

  // On y va
  T_setof_type *rtype = (T_setof_type *)(first_in_param->get_B_type()) ;
  T_type *src = rtype->get_relation_src_type() ;
  T_type *dst = rtype->get_relation_dst_type() ;

  if (dst->is_a_set() == FALSE)
	{
	  // ... et de type ensemble
	  semantic_error(dst,
                     CAN_CONTINUE,
					 get_error_msg(E_OPERAND_OF_BUILTIN_OP_IS_NOT_A_SET),
					 get_lex_type_ascii(keyword->get_lex_type()),
                     make_type_name(dst)->get_value()) ;
      return ;
	}


  T_type *clone_src = src->clone_type(NULL, get_betree(), get_ref_lexem()) ;
  T_type *clone_inside_dst =
	((T_setof_type *)dst)->get_base_type()->clone_type(NULL,
													   get_betree(),
													   get_ref_lexem()) ;

  T_product_type *prod ;
  prod = new T_product_type(clone_src,
							clone_inside_dst,
							this,
							get_betree(),
							get_ref_lexem()) ;
  clone_src->unlink() ;
  clone_inside_dst->unlink() ;

  T_setof_type *res ;
  res = new T_setof_type(prod, this, get_betree(), get_ref_lexem()) ;
  prod->unlink() ;

  set_B_type(res, get_ref_lexem()) ;
  res->unlink() ;
}

// CTRL Ref: ???
// Un seul parametre, de type relation S(T1*T2)
// Le resultat est de type S(T1*S(T2))
void T_op_result::vtype_fnc(T_keyword *keyword)
{
  TRACE1("T_op_result(%x)::vtype_fnc", this) ;
  if (check_single_param(keyword) == FALSE)
	{
	  // IL faut un seul parametre
	  return ;
	}

  if (first_in_param->is_a_relation() == FALSE)
	{
	  // ... et de type relation
	  semantic_error(this,
					 CAN_CONTINUE,
					 get_error_msg(E_OPERAND_OF_BUILTIN_OP_IS_NOT_A_RELATION),
					 get_lex_type_ascii(keyword->get_lex_type()),
					 make_type_name(first_in_param)->get_value()) ;
	  return ;
	}

  // On y va
  T_setof_type *rtype = (T_setof_type *)(first_in_param->get_B_type()) ;
  T_type *T1 = rtype->get_relation_src_type() ;
  T_type *T2 = rtype->get_relation_dst_type() ;

  T_type *clone_T1 = T1->clone_type(NULL, get_betree(), get_ref_lexem()) ;
  T_type *clone_T2 = T2->clone_type(NULL, get_betree(), get_ref_lexem()) ;
  T_setof_type *inside ;
  inside = new T_setof_type(clone_T2, this, get_betree(), get_ref_lexem()) ;
  clone_T2->unlink() ;

  T_product_type *prod ;
  prod = new T_product_type(clone_T1, inside, this, get_betree(), get_ref_lexem()) ;
  clone_T1->unlink() ;

  T_setof_type *res ;
  res = new T_setof_type(prod, this, get_betree(), get_ref_lexem()) ;
  prod->unlink() ;

  set_B_type(res, get_ref_lexem()) ;
  res->unlink() ;
}



//construit une sequence de type P(Z1*T)
static T_setof_type* create_a_sequence_type(T_type* elem_type,
													 T_item* father,
													 T_betree* betree,
													 T_lexem* lexem)
{
  TRACE0("create_a_sequence_type") ;

  //duplication de elem_type
  T_type* type_T = elem_type->clone_type(NULL, betree, lexem) ;

  //Construction de Z1
  T_type* type_Z1 = new T_predefined_type(BTYPE_INTEGER,
										  NULL,
										  NULL,
										  father,
										  betree,
										  lexem) ;
  // Construction de Z1*T
  T_product_type* type_prod = new T_product_type(type_Z1,
												 type_T,
												 NULL,
												 betree,
												 lexem) ;
  type_Z1->unlink() ;
  type_T->unlink() ;

  // construction de P(Z1*T)
  T_setof_type* type_seq = new T_setof_type(type_prod,
											 NULL,
											 betree,
											 lexem) ;
  type_prod->unlink() ;

  return type_seq ;
}


//Construit une séquence du type P(Z*Z)
static T_setof_type* create_an_integer_sequence_type(T_item* father,
													 T_betree* betree,
													 T_lexem* lexem)
{
  TRACE0("create_an_integer_sequence_type") ;

  //Construction de Z
  T_type* type_Z = new T_predefined_type(BTYPE_INTEGER,
										 NULL,
										 NULL,
										 father,
										 betree,
										 lexem) ;


  // construction de P(Z*Z2)
  T_setof_type* type_seq = create_a_sequence_type(type_Z,
												  NULL,
												  betree,
												  lexem) ;

  type_Z->unlink() ;

  return type_seq ;
}

//construit et retourne le type P(Z*Z2)*T (T=node_type)
static T_product_type* create_a_tree_node_type(T_type* node_type,
														T_item* father,
														T_betree* betree,
														T_lexem* lexem)
{
  TRACE0("create_a_tree_node_type") ;

  ASSERT(node_type != NULL) ;

  //Construction de T
  T_type* type_T = node_type->clone_type(NULL, betree, lexem) ;



  // construction de P(Z*Z2)
  T_setof_type* type_seq = create_an_integer_sequence_type(NULL,
														   betree,
														   lexem) ;


  //construction de P(Z1*Z2) * T
  T_product_type* type_node = new T_product_type(type_seq,
												 type_T,
												 NULL,
												 betree,
												 lexem) ;
  type_seq->unlink() ;
  type_T->unlink() ;

  return type_node ;

}



// Construit et retourne le type P(P(Z*Z)*T)
static T_setof_type* create_a_tree_type(T_type* node_type,
												 T_item* father,
												 T_betree* betree,
												 T_lexem* lexem)
{
  TRACE0("create_a_tree_type") ;

  //Construction de P(Z*Z)*T
  T_product_type* type_node = create_a_tree_node_type(node_type,
													  father,
													  betree,
													  lexem) ;

  //construction de P(P(Z*Z)*T)
  T_setof_type* type_tree = new T_setof_type(type_node,
											  NULL,
											  betree,
											  lexem) ;

  type_node->unlink() ;

  return type_tree ;
}

//Pour node_type=P(Z*Z)*T retourne T
static T_type* get_node_value_type(T_product_type* node_type)
{
  return node_type->get_type2() ;
}

//Pour tree_type=P(P(Z*Z)*T) retourne T
static T_type* get_tree_node_value_type(T_setof_type* tree_type)
{
  //Assert pour proteger le cast
  ASSERT(tree_type->is_a_tree_type()==TRUE) ;

  return get_node_value_type((T_product_type*)tree_type->get_base_type()) ;
}

//Cree un message d'erreur et retourne FALSE si :
// -- le type de first_param n'est pas un arbre P(P(Z*Z)*T)
// -- OU le type de second_param n'est pas une sequence d'entier
static int check_first_tree_second_int_seq(T_item* first_param,
													T_item* second_param,
													T_keyword* keyword)

{
  //Est-ce du type arbre ?
  if ((first_param->get_B_type() == NULL) ||
	 (first_param->get_B_type()->is_a_tree_type() == FALSE))
	{
	  semantic_error(first_param,
					 CAN_CONTINUE,
					 get_error_msg(E_OPERAND_OF_BUILTIN_OP_IS_NOT_A_TREE),
					 get_lex_type_ascii(keyword->get_lex_type()),
					 make_type_name(first_param)->get_value()) ;
	  return FALSE;
	}

  // Est-ce du type sequence d'entier ?
  int correct_type = FALSE ;

  // (1) Est-ce une sequence
  if ((second_param->get_B_type() != NULL) &&
	  (second_param->get_B_type()->is_a_seq()==TRUE))
	{
	  //Cast garanti par le test ci-dessus
	  T_setof_type* param_type = (T_setof_type*)second_param->get_B_type() ;

	  //(2) Est-ce une sequence non vide
	  if (second_param->get_B_type()->is_a_non_empty_seq()==TRUE)
		{
		  //C'est une sequence non vides

		  //Est-ce une sequende du type P(Z*Z)
		  if (param_type->get_seq_base_type()->is_an_integer())
			{
			  //oui c'est du type P(Z*Z)
			  correct_type = TRUE ;
			}
		}
	  else
		{
		  //C'est une sequence vide
		  correct_type = TRUE ;
		}
	}

  if (correct_type == FALSE)
	{
	  semantic_error(second_param,
					 CAN_CONTINUE,
					 get_error_msg(E_OPERAND_OF_BUILTIN_OP_IS_NOT_SEQ_OF_INT),
					 get_lex_type_ascii(keyword->get_lex_type()),
					 make_type_name(second_param)->get_value()) ;
	  return FALSE ;
	}


  return TRUE ;
}

//CTRL Ref: VTYPE 2-9-1
//dans tree(S) et btree(S), S doit etre un ensemble
//le type de tree(S) et btree(S) est P(P(P(Z*Z)*T))
void T_op_result::vtype_tree(T_keyword *keyword)
{
  TRACE1("T_op_result(%x)::vtype_tree", this) ;
  if (check_single_param(keyword) == FALSE)
	{
	  //Il faut un seule parametre
	  return ;
	}

  if ((first_in_param->get_B_type() == NULL) ||
	 (first_in_param->get_B_type()->is_a_set() == FALSE))
	{
	  semantic_error(this,
					 CAN_CONTINUE,
					 get_error_msg(E_OPERAND_SHOULD_BE_A_SET),
					 make_type_name(first_in_param)->get_value()) ;
	  return ;
	}

  //Cast garanti par le test ci-dessus
  T_setof_type* type_T = (T_setof_type*)first_in_param->get_B_type() ;

  //construction de P(P(Z*Z)*T)
  T_setof_type* type_tree = create_a_tree_type(type_T->get_base_type(),
											   this,
											   get_betree(),
											   get_ref_lexem()) ;

  //construction de P(P(P(Z*Z)*T)
  T_setof_type* type_setof_tree = new T_setof_type(type_tree,
												   NULL,
												   get_betree() ,
												   get_ref_lexem()) ;
  type_tree->unlink() ;

  set_B_type(type_setof_tree, get_ref_lexem()) ;

  type_setof_tree->unlink() ;


}

//CTRL Ref: VTYPE 2-9-2
//const(x,q) : x de type quelconque T, q de type P(Z*P(P(Z*Z)*T))
//type resutant : P(P(Z*Z)*T)
void T_op_result::vtype_const(T_keyword *keyword)
{
  TRACE1("T_op_result(%x)::vtype_const", this) ;
  if (nb_in_params != 2)
	{
	  // Il faut deux parametres ...
	  semantic_error(this,
					 CAN_CONTINUE,
					 get_error_msg(E_BUILTIN_OP_NEEDS_TWO_OPERANDS),
					 get_lex_type_ascii(keyword->get_lex_type()),
					 nb_in_params) ;

	  return ;
	}

  first_in_param->make_type() ;
  last_in_param->make_type() ;

  T_type* type_x=first_in_param->get_B_type() ;
  T_type* type_q=last_in_param->get_B_type() ;
  T_type* type_T1=NULL ; //Pour l'instant, pour eviter un warning

  int correct_type = FALSE ;

  //(1) q doit etre du type P(Z*P(P(Z*Z)*T1))

  // Debut correction bug 2192 SL
  // [] est une valeur acceptable pour q
  if (last_in_param->get_node_type() == NODE_EMPTY_SEQ)
	{
	  // C'est bon
	  correct_type = TRUE ;
	}
  else
	{
	  // -a: q doit etre une sequence: P(Z*ARBRE)
	  if ( (type_q != NULL) &&
		  (type_q->is_a_seq()==TRUE))
		{

		  T_setof_type* seq = (T_setof_type*)type_q ;

		  // -b: ARBRE doit etre du type P(P(Z*Z)*T1)
		  //Cast justifie par le test ci-dessus

		  if (seq->get_seq_base_type()->is_a_tree_type()==TRUE)
			{
			  //cast justifie par le test ci-dessus
			  type_T1=
				get_tree_node_value_type((T_setof_type*)seq->get_seq_base_type()) ;

			  correct_type = TRUE ;
			}
		}
	}

  if (correct_type == FALSE)
	{
	  //q n'est pas du type P(Z*P(P(Z*Z)*T1))
	  semantic_error(this,
					 CAN_CONTINUE,
					 get_error_msg(E_OPERAND_OF_BUILTIN_OP_IS_NOT_A_SEQ_OF_TREE),
					 get_lex_type_ascii(keyword->get_lex_type()),
					 make_type_name(last_in_param)->get_value()) ;
	  return ;
	}

  //(2) type de T1 doit etre egal au type de x
  if ((type_x == NULL) ||
	 (type_x->is_compatible_with(type_T1) == FALSE))
	{
	  semantic_error(first_in_param,
					 CAN_CONTINUE,
					 get_error_msg(E_ILLEGAL_TYPE_FOR_NODE_TO_ADD_IN_TREE),
					 make_type_name(type_x)->get_value(),
					 make_type_name(type_T1)->get_value()) ;
	  return ;
	}

  //
  //  Construction du type de const(x,q)
  //

  T_setof_type * type_tree = create_a_tree_type (type_x,
												 this,
												 get_betree(),
												 get_ref_lexem()) ;

  set_B_type(type_tree, get_ref_lexem()) ;

  type_tree->unlink() ;


}

//CTRL Ref: VTYPE 2-9-3
//TYPE(top(t)) = T & TYPE(t) = P(P(Z*Z)*T)
void T_op_result::vtype_top(T_keyword *keyword)
{
  //
  // Verification du nombre et du type des parametres
  //

  TRACE1("T_op_result(%x)::vtype_top", this) ;
  if (check_single_param(keyword) == FALSE)
	{
	  //Il faut un seul parametre
	  return ;
	}

  if ((first_in_param->get_B_type()==NULL) ||
	 (first_in_param->get_B_type()->is_a_tree_type() == FALSE))
	{
	  semantic_error(this,
					 CAN_CONTINUE,
					 get_error_msg(E_OPERAND_OF_BUILTIN_OP_IS_NOT_A_TREE),
					 get_lex_type_ascii(keyword->get_lex_type()),
					 make_type_name(first_in_param)->get_value()) ;
	  return ;
	}


  //
  //  Construction du type de top(t)
  //


  //Cast justifie par le test précédent
  set_B_type(get_tree_node_value_type((T_setof_type*)first_in_param->get_B_type()), get_ref_lexem());


}

//CTRL Ref: VTYPE 2-9-4
//TYPE[sons(t)] = P(Z*P(P(Z*Z)*T)) et TYPE[t] = P(P(Z*Z)*T)
void T_op_result::vtype_sons(T_keyword *keyword)
{
  TRACE1("T_op_result(%x)::vtype_sons", this) ;

  //
  // Vérification du nombre et du type des parametres
  //

  if (check_single_param(keyword) == FALSE)
	{
	  //Il faut un seul parametre
	  return ;
	}

  if ((first_in_param->get_B_type() == NULL) ||
	 (first_in_param->get_B_type()->is_a_tree_type() == FALSE))
	{
	  semantic_error(this,
					 CAN_CONTINUE,
					 get_error_msg(E_OPERAND_OF_BUILTIN_OP_IS_NOT_A_TREE),
					 get_lex_type_ascii(keyword->get_lex_type()),
					 make_type_name(first_in_param)->get_value()) ;
	  return ;
	}


  //
  //  Construction du type de sons(t): P(Z*P(P(Z*Z)*T))
  //

  //Cast justifie par le test ci-dessus
  T_setof_type* param_type = (T_setof_type*)first_in_param->get_B_type() ;



  //(1) recuperation du type de T
  T_type* type_T = get_tree_node_value_type(param_type) ;

  //(2) creation de P(P(Z*Z)*T)
  T_setof_type* tree_type = create_a_tree_type(type_T,
											   this,
											   get_betree(),
											   get_ref_lexem());

  //(3) creation de P(Z*P(P(Z*Z)*T))
  T_setof_type *seq_of_tree = create_a_sequence_type(tree_type,
													 this,
													 get_betree(),
													 get_ref_lexem()) ;

  tree_type->unlink() ;

  //(4) Affectation a sons(t) du type P(Z*P(P(Z*Z)*T))
  set_B_type(seq_of_tree, get_ref_lexem());

  seq_of_tree->unlink() ;
}

//CTRL Ref: VTYPE 2-9-6
// TYPE[prefix(t)]=P(Z*T) & TYPE[t]=P(P(Z*Z)*T)
void T_op_result::vtype_pre_post_fix(T_keyword *keyword)
{
  TRACE1("T_op_result(%x)::vtype_pre_post_fix", this) ;

  //
  // Vérification du nombre et du type des parametres
  //

  if (check_single_param(keyword) == FALSE)
	{
	  //Il faut un seul parametre
	  return ;
	}

  if ((first_in_param->get_B_type()==NULL) ||
	 (first_in_param->get_B_type()->is_a_tree_type() == FALSE))
	{
	  semantic_error(this,
					 CAN_CONTINUE,
					 get_error_msg(E_OPERAND_OF_BUILTIN_OP_IS_NOT_A_TREE),
					 get_lex_type_ascii(keyword->get_lex_type()),
					 make_type_name(first_in_param)->get_value()) ;
	  return ;
	}


  //
  //  Construction du type de prefix(t): P(Z*T)
  //

  //Cast justifie par le test ci-dessus
  T_setof_type* param_type = (T_setof_type*)first_in_param->get_B_type() ;

  //(1) Recuperation du type de T
  T_type* type_T = get_tree_node_value_type(param_type) ;

  //(2) construction de P(Z*T)
  T_setof_type* seq_type = create_a_sequence_type(type_T,
												  this,
												  get_betree(),
												  get_ref_lexem()) ;

  set_B_type(seq_type, get_ref_lexem());

  seq_type->unlink() ;


}

//CTRL Ref: VTYPE 2-9-5
//TYPE[sizet(t)] = Z & TYPE[t] = P(P(Z*Z)*T)
void T_op_result::vtype_sizet(T_keyword *keyword)
{
  TRACE1("T_op_result(%x)::vtype_sizet", this) ;


  //
  // Vérification du nombre et du type des parametres
  //

  if (check_single_param(keyword) == FALSE)
	{
	  //Il faut un seul parametre
	  return ;
	}

  if ((first_in_param->get_B_type()==NULL) ||
	 (first_in_param->get_B_type()->is_a_tree_type() == FALSE))
	{
	  semantic_error(this,
					 CAN_CONTINUE,
					 get_error_msg(E_OPERAND_OF_BUILTIN_OP_IS_NOT_A_TREE),
					 get_lex_type_ascii(keyword->get_lex_type()),
					 make_type_name(first_in_param)->get_value()) ;
	  return ;
	}


  //
  //  Construction du type de sizet(t): Z
  //

  T_type *res = new T_predefined_type(BTYPE_INTEGER,
									  NULL,
									  NULL,
									  this,
									  get_betree(),
									  get_ref_lexem()) ;

  set_B_type(res, get_ref_lexem()) ;
  res->unlink() ;

}

//CTRL Ref: VTYPE 2-9-7
//TYPE[mirror(t)] = P(P(Z*Z)*T) & TYPE(t) = P(P(Z*Z)*T)
void T_op_result::vtype_mirror(T_keyword *keyword)
{
  TRACE1("T_op_result(%x)::vtype_mirror", this) ;



  //
  // Vérification du nombre et du type des parametres
  //

  if (check_single_param(keyword) == FALSE)
	{
	  //Il faut un seul parametre
	  return ;
	}

  if ((first_in_param->get_B_type() == NULL) ||
	 (first_in_param->get_B_type()->is_a_tree_type() == FALSE))
	{
	  semantic_error(this,
					 CAN_CONTINUE,
					 get_error_msg(E_OPERAND_OF_BUILTIN_OP_IS_NOT_A_TREE),
					 get_lex_type_ascii(keyword->get_lex_type()),
					 make_type_name(first_in_param)->get_value()) ;
	  return ;
	}

  //
  //  Construction du type de mirror(t)
  //

  //Cast garanti par test ci-dessus
  T_setof_type* param_type = (T_setof_type*)first_in_param->get_B_type() ;

  //Recuperation du type des noeuds de l'arbre
  T_type* type_T = get_tree_node_value_type(param_type) ;

  T_setof_type * type_tree = create_a_tree_type (type_T,
												 this,
												 get_betree(),
												 get_ref_lexem()) ;

  set_B_type(type_tree, get_ref_lexem()) ;

  type_tree->unlink() ;

}

//CTRL Ref: VTYPE 2-9-8
//TYPE[rank(t,n)] = INTEGER & TYPE[t]=P(P(Z*Z)*T) & TYPE[n]=P(Z*Z)
void T_op_result::vtype_rank(T_keyword *keyword)
{
  TRACE1("T_op_result(%x)::vtype_rank", this) ;

  //
  // Vérification du nombre et du type des parametres
  //

  if (nb_in_params != 2)
	{
	  // Il faut deux parametres ...
	  semantic_error(this,
					 CAN_CONTINUE,
					 get_error_msg(E_BUILTIN_OP_NEEDS_TWO_OPERANDS),
					 get_lex_type_ascii(keyword->get_lex_type()),
					 nb_in_params) ;

	  return ;
	}

  first_in_param->make_type() ;
  last_in_param->make_type() ;


  if (check_first_tree_second_int_seq(first_in_param,
									 last_in_param,
									 keyword)==FALSE)
	{
	  //first_in_param ou last_in_param n'est pas du bon type
	  return ;
	}

  //
  //  Construction du type de rank(t,n): Z
  //

  T_type *res = new T_predefined_type(BTYPE_INTEGER,
									  NULL,
									  NULL,
									  this,
									  get_betree(),
									  get_ref_lexem()) ;

  set_B_type(res, get_ref_lexem()) ;
  res->unlink() ;


}
//CTRL Ref: VTYPE 2-9-9
//TYPE[father(t,n)]=P(Z*Z) & TYPE[t]=P(P(Z*Z)*T) & TYPE[n]=P(Z*Z)
void T_op_result::vtype_father(T_keyword *keyword)
{
  TRACE1("T_op_result(%x)::vtype_father", this) ;


  //
  // Vérification du nombre et du type des parametres
  //

  if (nb_in_params != 2)
	{
	  // Il faut deux parametres ...
	  semantic_error(this,
					 CAN_CONTINUE,
					 get_error_msg(E_BUILTIN_OP_NEEDS_TWO_OPERANDS),
					 get_lex_type_ascii(keyword->get_lex_type()),
					 nb_in_params) ;

	  return ;
	}

  first_in_param->make_type() ;
  last_in_param->make_type() ;


  if (check_first_tree_second_int_seq(first_in_param,
									 last_in_param,
									 keyword)==FALSE)
	{
	  //first_in_param ou last_in_param n'est pas du bon type
	  return ;
	}

  //
  //  Construction du type de father(t,n): P(Z*Z)
  //

  T_setof_type *integer_seq = create_an_integer_sequence_type(this,
															  get_betree(),
															  get_ref_lexem());

  set_B_type(integer_seq, get_ref_lexem()) ;
  integer_seq->unlink() ;

}

//CTRL Ref: VTYPE 2-9-10
//TYPE[son(t,n,i)]=P(Z*Z) & TYPE[t]=P(P(Z*Z)*T) & TYPE[n]=P(Z*Z) & TYPE[i]=Z
void T_op_result::vtype_son(T_keyword *keyword)
{
  TRACE1("T_op_result(%x)::vtype_son", this) ;


  //
  // Vérification du nombre et du type des parametres
  //

  if (nb_in_params != 3)
	{
	  // Il faut deux parametres ...
	  semantic_error(this,
					 CAN_CONTINUE,
					 get_error_msg(E_BUILTIN_OP_NEEDS_THREE_OPERANDS),
					 get_lex_type_ascii(keyword->get_lex_type()),
					 nb_in_params) ;

	  return ;
	}

  T_item*first_op = first_in_param ;
  T_item*second_op = first_in_param->get_next() ;
  T_item*third_op = last_in_param ;

  first_op->make_type() ;
  second_op->make_type() ;
  third_op->make_type() ;

  if (check_first_tree_second_int_seq(first_op,
									 second_op,
									 keyword)==FALSE)
	{
	  //le premier operande ou le deuxieme n'est pas du bon type
	  return ;
	}

    if ( (third_op->get_B_type() == NULL)
	   || (third_op->get_B_type()->is_an_integer() == FALSE) )
	{
	  // le troisieme n'est pas du type integer
	  semantic_error(third_op,
					 CAN_CONTINUE,
					 get_error_msg(E_OPERAND_OF_BUILTIN_OP_IS_NOT_AN_INTEGER),
					 get_lex_type_ascii(keyword->get_lex_type()),
					 make_type_name(third_op)->get_value()) ;
	  return ;
	}

	//
	//  Construction du type de son(t,n,i): P(Z*Z)
	//

	T_setof_type *integer_seq=create_an_integer_sequence_type(this,
															  get_betree(),
															  get_ref_lexem());

	set_B_type(integer_seq, get_ref_lexem()) ;
	integer_seq->unlink() ;


}

//CTRL Ref: VTYPE 2-9-11
//TYPE[subtree(t,n)]=P(P(Z*Z)*T) & TYPE[t]=P(P(Z*Z)*T) & TYPE[n]=P(Z*Z)
void T_op_result::vtype_subtree(T_keyword *keyword)
{
  TRACE1("T_op_result(%x)::vtype_subtree", this) ;


  //
  // Vérification du nombre et du type des parametres
  //

  if (nb_in_params != 2)
	{
	  // Il faut deux parametres ...
	  semantic_error(this,
					 CAN_CONTINUE,
					 get_error_msg(E_BUILTIN_OP_NEEDS_TWO_OPERANDS),
					 get_lex_type_ascii(keyword->get_lex_type()),
					 nb_in_params) ;

	  return ;
	}

  first_in_param->make_type() ;
  last_in_param->make_type() ;


  if (check_first_tree_second_int_seq(first_in_param,
									 last_in_param,
									 keyword)==FALSE)
	{
	  //first_in_param ou last_in_param n'est pas du bon type
	  return ;
	}

  //
  //  Construction du type de subtree(t, n) = P(P(Z*Z)*T)
  //

  //Cast garanti par test ci-dessus
  T_setof_type* param_type = (T_setof_type*)first_in_param->get_B_type() ;

  //Recuperation du type des noeuds de l'arbre
  T_type* type_T = get_tree_node_value_type(param_type) ;

  T_setof_type * type_tree = create_a_tree_type (type_T,
												 this,
												 get_betree(),
												 get_ref_lexem()) ;

  set_B_type(type_tree, get_ref_lexem()) ;

  type_tree->unlink() ;


}

//CTRL Ref: VTYPE 2-9-12
//TYPE[arity(t,n)] = INTEGER & TYPE[t]= P(P(Z*Z)*T) & TYPE(n) = P(Z*Z)
void T_op_result::vtype_arity(T_keyword *keyword)
{
  TRACE1("T_op_result(%x)::vtype_arity", this) ;

  TRACE1("T_op_result(%x)::vtype_subtree", this) ;


  //
  // Vérification du nombre et du type des parametres
  //

  if (nb_in_params != 2)
	{
	  // Il faut deux parametres ...
	  semantic_error(this,
					 CAN_CONTINUE,
					 get_error_msg(E_BUILTIN_OP_NEEDS_TWO_OPERANDS),
					 get_lex_type_ascii(keyword->get_lex_type()),
					 nb_in_params) ;

	  return ;
	}

  first_in_param->make_type() ;
  last_in_param->make_type() ;


  if (check_first_tree_second_int_seq(first_in_param,
									 last_in_param,
									 keyword)==FALSE)
	{
	  //first_in_param ou last_in_param n'est pas du bon type
	  return ;
	}

  //
  //  Construction du type de arity(t,n): Z
  //

  T_type *res = new T_predefined_type(BTYPE_INTEGER,
									  NULL,
									  NULL,
									  this,
									  get_betree(),
									  get_ref_lexem()) ;

  set_B_type(res, get_ref_lexem()) ;
  res->unlink() ;


}

//Fonction qui verifie le type de l'argument pour l'oeprateur bin(x)
static int check_bin_1_op(T_keyword *keyword,
								   T_item* operand)
{
  TRACE0("check_bin_1_op") ;

  //Retourne FALSE, pour les reprises sur erreur
  return (operand->get_B_type() == NULL ) ? FALSE : TRUE ;
}

//Fonction qui verifie le type de l'argument pour l'oeprateur bin(g,x,d)
//(1) TYPE[g]= P(P(Z*Z)*Tg)
//(2) TYPE[d]= P(P(Z*Z)*Td)
//(3) TYPE[x] = Tg
//(4) TYPE[x] = Td
static int check_bin_3_op(T_keyword *keyword,
								   T_item* operand_g,
								   T_item* operand_x,
								   T_item* operand_d)
{
  TRACE0("check_bin_3_op") ;


  //(1) TYPE[g]= P(P(Z*Z)*Tg) ??
  if ((operand_g->get_B_type() == NULL) ||
	 (operand_g->get_B_type()->is_a_tree_type() == FALSE))
	{
	  semantic_error(operand_g,
					 CAN_CONTINUE,
					 get_error_msg(E_OPERAND_OF_BUILTIN_OP_IS_NOT_A_TREE),
					 get_lex_type_ascii(keyword->get_lex_type()),
					 make_type_name(operand_g)->get_value()) ;
	  return FALSE;
	}

  //(2) TYPE[d]= P(P(Z*Z)*Td) ??
  if ((operand_d->get_B_type() == NULL) ||
	 (operand_d->get_B_type()->is_a_tree_type() == FALSE))
	{
	  semantic_error(operand_g,
					 CAN_CONTINUE,
					 get_error_msg(E_OPERAND_OF_BUILTIN_OP_IS_NOT_A_TREE),
					 get_lex_type_ascii(keyword->get_lex_type()),
					 make_type_name(operand_g)->get_value()) ;
	  return FALSE;
	}

  T_type* type_x = operand_x->get_B_type() ;

  //Casts garanti par les deux tests ci-dessus
  T_type* type_g =
	get_tree_node_value_type((T_setof_type*)operand_g->get_B_type()) ;
  T_type* type_d =
	get_tree_node_value_type((T_setof_type*)operand_d->get_B_type()) ;

  if ((type_x == NULL) || (type_d == NULL) || (type_g == NULL))
	{
	  TRACE0("REPRISE SUR ERREUR : check_bin_3_op") ;
	  return FALSE ;
	}

  //(3) TYPE[x] = Tg ??
  if (type_x->is_compatible_with(type_g) == FALSE)
	{
	  semantic_error(operand_x,
					 CAN_CONTINUE,
					 get_error_msg(E_ILLEGAL_TYPE_FOR_NODE_TO_ADD_IN_TREE),
					 make_type_name(type_x)->get_value(),
					 make_type_name(type_g)->get_value()) ;
	  return FALSE;
	}

   //(4) TYPE[x] = Td ??
  if (type_x->is_compatible_with(type_d) == FALSE)
	{
	  semantic_error(operand_x,
					 CAN_CONTINUE,
					 get_error_msg(E_ILLEGAL_TYPE_FOR_NODE_TO_ADD_IN_TREE),
					 make_type_name(type_x)->get_value(),
					 make_type_name(type_d)->get_value()) ;
	  return FALSE;
	}

  return TRUE ;
}


//CTRL Ref: VTYPE 2-9-13
//Vérifier pour bin(g,x,d):
// -- (1) TYPE[g]= P(P(Z*Z)*Tg)
// -- (2) TYPE[d]= P(P(Z*Z)*Td)
// -- (3) TYPE[x] = Tg
// -- (4) TYPE[x] = Td
// TYPE[bin(x)] = TYPE[bin(g,x,d)]=P(P(Z*Z)*T)
void T_op_result::vtype_bin(T_keyword *keyword)
{
  TRACE1("T_op_result(%x)::vtype_bin", this) ;

  //
  // Vérification du nombre et du type des parametres
  //


  if ((nb_in_params != 1) && (nb_in_params != 3) )
	{
	  // Il faut un ou trois ...
	  semantic_error(this,
					 CAN_CONTINUE,
					 get_error_msg(E_BUILTIN_OP_NEEDS_ONE_OR_THREE_OPERANDS),
					 get_lex_type_ascii(keyword->get_lex_type()),
					 nb_in_params) ;

	  return ;
	}

  int correct_type = FALSE ;
  T_type* type_x = NULL ;//en attendant pour eviter le warning

  if (nb_in_params == 1)
	{
	  //Check du parametre dans le cas bin(x)
	  correct_type = check_bin_1_op(keyword, first_in_param) ;
	  type_x = first_in_param->get_B_type() ;

	}
  else // nb_in_params == 3
	{
	  //checks desparametres dans le cas bin(g,x,d)
	  correct_type = check_bin_3_op(keyword,
									first_in_param,
									first_in_param->get_next(),
									last_in_param) ;
	  type_x = first_in_param->get_next()->get_B_type() ;
	}

  if (correct_type != TRUE)
	{
	  //On s'arrete là, le ou les parametres n'ont pas le bon type
	  return ;
	}

  //
  //  Construction du type de bin(x) ou bin(g,x,d) = P(P(Z*Z)*TYPE[x])
  //

  T_setof_type * type_tree = create_a_tree_type (type_x,
												 this,
												 get_betree(),
												 get_ref_lexem()) ;

  set_B_type(type_tree, get_ref_lexem()) ;

  type_tree->unlink() ;

}

//CTRL Ref: VTYPE 2-9-14
//TYPE[left(t)] = TYPE[right(t)] = P(P(Z*Z)*T)
//TYPE[t] = P(P(Z*Z)*T)
void T_op_result::vtype_left_right(T_keyword *keyword)
{
  TRACE1("T_op_result(%x)::vtype_left_right", this) ;

  //
  // Vérification du nombre et du type des parametres
  //

  if (check_single_param(keyword) == FALSE)
	{
	  //Il faut un seul parametre
	  return ;
	}

  if ((first_in_param->get_B_type() == NULL) ||
	 (first_in_param->get_B_type()->is_a_tree_type() == FALSE))
	{
	  semantic_error(this,
					 CAN_CONTINUE,
					 get_error_msg(E_OPERAND_OF_BUILTIN_OP_IS_NOT_A_TREE),
					 get_lex_type_ascii(keyword->get_lex_type()),
					 make_type_name(first_in_param)->get_value()) ;
	  return ;
	}

  //
  //  Construction du type de left(t) ou right(t)
  //

  //Cast garanti par test ci-dessus
  T_setof_type* param_type = (T_setof_type*)first_in_param->get_B_type() ;

  //Recuperation du type des noeuds de l'arbre
  T_type* type_T = get_tree_node_value_type(param_type) ;

  T_setof_type * type_tree = create_a_tree_type (type_T,
												 this,
												 get_betree(),
												 get_ref_lexem()) ;

  set_B_type(type_tree, get_ref_lexem()) ;

  type_tree->unlink() ;


}

//CTRL Ref: VTYPE 2-9-15
//TYPE[infix(t)]=P(Z*T) & TYPE[t]=P(P(Z*Z)*T)
void T_op_result::vtype_infix(T_keyword *keyword)
{
  TRACE1("T_op_result(%x)::vtype_infix", this) ;

  //
  // Vérification du nombre et du type des parametres
  //

  if (check_single_param(keyword) == FALSE)
	{
	  //Il faut un seul parametre
	  return ;
	}

  if ((first_in_param->get_B_type()==NULL) ||
	 (first_in_param->get_B_type()->is_a_tree_type() == FALSE))
	{
	  semantic_error(this,
					 CAN_CONTINUE,
					 get_error_msg(E_OPERAND_OF_BUILTIN_OP_IS_NOT_A_TREE),
					 get_lex_type_ascii(keyword->get_lex_type()),
					 make_type_name(first_in_param)->get_value()) ;
	  return ;
	}


  //
  //  Construction du type de infix(t): P(Z*T)
  //

  //Cast justifie par le test ci-dessus
  T_setof_type* param_type = (T_setof_type*)first_in_param->get_B_type() ;

  //(1) Recuperation du type de T
  T_type* type_T = get_tree_node_value_type(param_type) ;

  //(2) construction de P(Z*T)
  T_setof_type* seq_type = create_a_sequence_type(type_T,
												  this,
												  get_betree(),
												  get_ref_lexem()) ;

  set_B_type(seq_type, get_ref_lexem());

  seq_type->unlink() ;


}


//
//	}{	Fin du type check des expressions de sequence
//

void T_op_result::vtype_from_integer_to_real(T_keyword *keyword)
{
  TRACE1("T_op_result(%x):vtype_from_integer_to_real()", this) ;
  check_single_param(keyword) ;
  if ( (first_in_param->get_B_type() == NULL)
	   || (first_in_param->get_B_type()->is_an_integer() == FALSE) )
	{
	  semantic_error(this,
					 CAN_CONTINUE,
					 get_error_msg(E_OPERAND_OF_BUILTIN_OP_IS_NOT_AN_INTEGER),
					 get_lex_type_ascii(keyword->get_lex_type()),
					 make_type_name(first_in_param)->get_value()) ;
	  return ;
	}
  T_type *type;
  type = new T_predefined_type(BTYPE_REAL,
							   NULL,
							   NULL,
							   this,
							   get_betree(),
							   get_ref_lexem()) ;
  set_B_type(type, get_ref_lexem()) ;
  type->unlink() ;
}

void T_op_result::vtype_from_real_to_integer(T_keyword *keyword)
{
  TRACE1("T_op_result(%x):vtype_from_real_to_integer()", this) ;
  check_single_param(keyword) ;
  if ( (first_in_param->get_B_type() == NULL)
	   || (first_in_param->get_B_type()->is_a_real() == FALSE) )
	{
	  semantic_error(this,
					 CAN_CONTINUE,
					 get_error_msg(E_OPERAND_OF_BUILTIN_OP_IS_NOT_A_REAL),
					 get_lex_type_ascii(keyword->get_lex_type()),
					 make_type_name(first_in_param)->get_value()) ;
	  return ;
	}
  T_type *res = new T_predefined_type(BTYPE_INTEGER,
									  NULL,
									  NULL,
									  this,
									  get_betree(),
									  get_ref_lexem()) ;
  set_B_type(res, get_ref_lexem()) ;
  res->unlink() ;
}

