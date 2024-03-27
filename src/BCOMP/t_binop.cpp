/******************************* CLEARSY **************************************
* Fichier : $Id: t_binop.cpp,v 2.0 1999-10-06 13:43:13 sl Exp $
* (C) 2008 CLEARSY
*
* Description :		Compilateur B
*					Fabrication du type des expressions binaires
*
* Compilation:		-DDEBUG_BINOP_TYPE pour avoir des traces
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
RCS_ID("$Id: t_binop.cpp,v 1.26 1999-10-06 13:43:13 sl Exp $") ;

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

// Classe T_binary_op : Fonction generique de calcul de type
// Les builtins ont des types
void T_binary_op::internal_make_type(void)
{
#ifdef DEBUG_BINOP_TYPE
  TRACE3("T_binary_op(%x,%s,%d)::make_type",
		 this,
		 make_class_name(this),
		 get_node_type()) ;
#endif

  // Calcul de la liste des operandes : lors du type check
  fix_operand_list() ;

  // Fabrication du type des operandes
  T_item *cur_operand = first_operand ;

  while (cur_operand != NULL)
	{
#ifdef DEBUG_BINOP_TYPE
	  TRACE1("calcul du type de %x", cur_operand) ;
#endif
	  cur_operand->make_type() ;
	  // GP 7/01/99
	  TRACE1("Classe de cur_operand: %s", cur_operand->get_class_name()) ;
	  cur_operand->check_type() ;

	  cur_operand = cur_operand->get_next() ;
	}

  // Re-Force le calcul, necessaire dans certains cas
  op1->make_type() ;
  op2->make_type() ;

  // Verifications associees
  switch(oper)
	{
	case BOP_INTERVAL :
	  {
		// Type intervalle i.e. Setof(INTEGER, xx, yy)
		make_bop_interval_type() ;
		return ;
	  }

	case BOP_TIMES :
	  {
		make_bop_times_type() ;
		return ;
	  }

    case BOP_DIVIDES :
        case BOP_PLUS :
      {
            make_bop_divides_plus_type() ;
            return ;
      }

        case BOP_MOD :
            {
                make_bop_mod_type() ;
		return ;
            }
        case BOP_POWER :
            {
                make_bop_power_type();
                return;
            }

	case BOP_MINUS :
            {
		make_bop_minus_type() ;
		return ;
            }

	case BOP_SET_RELATION :
	  {
		make_bop_set_relation_type() ;
		return ;
	  }

	case BOP_SURJ_RELATION :
	  {
		make_bop_set_relation_type() ;
		return ;
	  }

	case BOP_TOTAL_RELATION :
	  {
		make_bop_set_relation_type() ;
		return ;
	  }

	case BOP_TOTAL_SURJ_RELATION :
	  {
		make_bop_set_relation_type() ;
		return ;
	  }

	case BOP_COMPOSITION :
	  {
		make_bop_composition_type() ;
		return ;
	  }

	case BOP_MAPLET :
	case BOP_COMMA :
	  {
		make_bop_maplet_type() ;
		return ;
	  }

	case BOP_UNION :
	case BOP_INTERSECTION :
	  {
		make_bop_union_intersect_type() ;
		return ;
	  }

	case BOP_HEAD_INSERT :
	  {
		make_bop_head_insert_type() ;
		return ;
	  }

	case BOP_TAIL_INSERT :
	  {
		make_bop_tail_insert_type() ;
		return ;
	  }

	case BOP_HEAD_RESTRICT :
	case BOP_TAIL_RESTRICT :
	  {
		make_bop_head_tail_restrict_type() ;
		return ;
	  }

	case BOP_RESTRICT :
	case BOP_ANTIRESTRICT :
	  {
		make_bop_restrict_antirestrict_type() ;
		return ;
	  }

	case BOP_CORESTRICT :
	case BOP_ANTICORESTRICT :
	  {
		make_bop_corestrict_anticorestrict_type() ;
		return ;
	  }

	case BOP_DIRECT_PRODUCT :
	  {
		make_bop_direct_product_type() ;
		return ;
	  }

	case BOP_LEFT_OVERLOAD :
	  {
		make_bop_left_overload_type() ;
		return ;
	  }

	case BOP_CONCAT :
	  {
		make_bop_concat_type() ;
		return ;
	  }

	case BOP_PARALLEL_PRODUCT :
	  {
		make_bop_parallel_product_type() ;
		return ;
	  }
	default :
	  {
		TRACE0("ICI CAS PAR DEFAUT - ON NE FAIT RIEN") ;
		TRACE4("PANIC BOP %d non traite (%s:%d:%d)\n",
			   oper,
			   get_ref_lexem()->get_file_name()->get_value(),
			   get_ref_lexem()->get_file_line(),
			   get_ref_lexem()->get_file_column()) ;
#ifndef MATRA
		internal_error(get_ref_lexem(),
					   __FILE__,
					   __LINE__,
					   "Type check of binary operator %d \
is not yet implemented",
					   oper) ;
			return ;
#else
		int REMETTRE_CE_CODE_UNE_FOIS_LE_TRANSCODEUR_LIVRE_POUR_METEOR = 5 ;
#endif
	  }
	}
}

// CTRL Ref: VTYPE 2-3 (5)
void T_binary_op::make_bop_interval_type(void)
{
#ifdef DEBUG_BINOP_TYPE
  TRACE2("T_binary_op(%x)::make_bop_interval_type (nb_operands = %d)",
		 this, nb_operands) ;
#endif

  // On verifie qu'il n'y a que deux operandes
  if (nb_operands != 2)
	{
	  semantic_error(this, CAN_CONTINUE, get_error_msg(E_INVALID_INTERVAL)) ;
	  return ;
	}

  //GP 15/1/99
  // op1 et de op2 doit etre de type entier
  // CTRL Ref : VTYPE 2-3-5
   if ((op1->get_B_type()== NULL) ||
	   (op1->get_B_type()->is_an_integer() == FALSE))
	 {
	   semantic_error(op1,
					  CAN_CONTINUE,
					  get_error_msg(E_OPERAND_SHOULD_BE_AN_INTEGER),
					  make_type_name(op1)->get_value()) ;
	 }

   if ((op2->get_B_type()==NULL) ||
	   (op2->get_B_type()->is_an_integer() == FALSE))
	 {
	   semantic_error(op2,
					  CAN_CONTINUE,
					  get_error_msg(E_OPERAND_SHOULD_BE_AN_INTEGER),
					  make_type_name(op2)->get_value()) ;
	 }



  if (op1->is_an_expr() == FALSE)
	{
	  semantic_error(op1,
					 CAN_CONTINUE,
					 get_error_msg(E_NOT_AN_EXPRESSION),
					 make_class_name(op1)) ;
	  return ;
	}

  if (op2->is_an_expr() == FALSE)
	{
	  semantic_error(op2,
					 CAN_CONTINUE,
					 get_error_msg(E_NOT_AN_EXPRESSION),
					 make_class_name(op1)) ;
	  return ;
	}

  T_type *base = new T_predefined_type(BTYPE_INTEGER,
									   op1->make_expr(),
									   op2->make_expr(),
									   this,
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

// CTRL Ref: VTYPE 2-1 (1.1)
// CTRL Ref: VTYPE 2-1 (1.4)
// CTRL Ref: VTYPE 2-1 (1.5)
void T_binary_op::make_bop_divides_plus_type(void)
{
#ifdef DEBUG_BINOP_TYPE
    TRACE1("T_binary_op(%x)::make_bop_divides_plus_type", this) ;
#endif

    T_type *type;
    T_item *cur = first_operand ;
    T_builtin_type btype = BTYPE_BOOL;

    while (cur != NULL)
    {
        cur->make_type() ;


        if (cur->get_B_type() == NULL)
        {
            // Reprise sur erreur
            TRACE0("panic, cur->get_B_type() = NULL") ;
            return ;
        }

        if (cur->get_B_type()->is_an_integer() == TRUE) {
            if (btype == BTYPE_REAL) {
                semantic_error(cur,
                               CAN_CONTINUE,
                               get_error_msg(E_OPERAND_SHOULD_BE_A_REAL),
                               make_type_name(cur)->get_value()) ;
            } else if (btype == BTYPE_FLOAT) {
                semantic_error(cur,
                               CAN_CONTINUE,
                               get_error_msg(E_OPERAND_SHOULD_BE_A_FLOAT),
                               make_type_name(cur)->get_value()) ;
            } else {
                btype = BTYPE_INTEGER;
            }
        } else if (cur->get_B_type()->is_a_real() == TRUE) {
            if (btype == BTYPE_INTEGER) {
                semantic_error(cur,
                               CAN_CONTINUE,
                               get_error_msg(E_OPERAND_SHOULD_BE_AN_INTEGER),
                               make_type_name(cur)->get_value()) ;
            } else if (btype == BTYPE_FLOAT) {
                semantic_error(cur,
                               CAN_CONTINUE,
                               get_error_msg(E_OPERAND_SHOULD_BE_A_FLOAT),
                               make_type_name(cur)->get_value()) ;
            } else {
                btype = BTYPE_REAL;
            }
        } else if (cur->get_B_type()->is_a_float() == TRUE) {
            if (btype == BTYPE_INTEGER) {
                semantic_error(cur,
                               CAN_CONTINUE,
                               get_error_msg(E_OPERAND_SHOULD_BE_AN_INTEGER),
                               make_type_name(cur)->get_value()) ;
            } else if (btype == BTYPE_REAL) {
                semantic_error(cur,
                               CAN_CONTINUE,
                               get_error_msg(E_OPERAND_SHOULD_BE_A_REAL),
                               make_type_name(cur)->get_value()) ;
            } else {
                btype = BTYPE_FLOAT;
            }
        } else {
            semantic_error(cur,
                           CAN_CONTINUE,
                           get_error_msg(E_OPERAND_SHOULD_BE_AN_INTEGER_A_REAL_OR_A_FLOAT),
                           make_type_name(cur)->get_value()) ;
        }


        cur = cur->get_next() ;
    }

    if (btype == BTYPE_BOOL) {
        btype = BTYPE_INTEGER;
    }
    // Le type resultant est : entier
    type = new T_predefined_type(btype,
                                 NULL,
                                 NULL,
                                 this,
                                 get_betree(),
                                 get_ref_lexem()) ;


    TRACE2("mise a jour du type de %x avec %x", this, type) ;
    set_B_type(type, get_ref_lexem()) ;
    type->unlink() ;
}

// CTRL Ref: VTYPE 2-1 (1.1)
// CTRL Ref: VTYPE 2-1 (1.4)
// CTRL Ref: VTYPE 2-1 (1.5)
void T_binary_op::make_bop_mod_type(void)
{
#ifdef DEBUG_BINOP_TYPE
  TRACE1("T_binary_op(%x)::make_bop_mod_type", this) ;
#endif


  T_item *cur = first_operand ;

  while (cur != NULL)
        {
          cur->make_type() ;


          if (cur->get_B_type() == NULL)
                {
                  // Reprise sur erreur
                  TRACE0("panic, cur->get_B_type() = NULL") ;
                  return ;
                }

          if (cur->get_B_type()->is_an_integer() == FALSE)
                {
                  semantic_error(cur,
                                                 CAN_CONTINUE,
                                                 get_error_msg(E_OPERAND_SHOULD_BE_AN_INTEGER),
                                                 make_type_name(cur)->get_value()) ;
                }

          cur = cur->get_next() ;
        }

  // Le type resultant est : entier
  T_type *type = new T_predefined_type(BTYPE_INTEGER,
                                                                           NULL,
                                                                           NULL,
                                                                           this,
                                                                           get_betree(),
                                                                           get_ref_lexem()) ;

  TRACE2("mise a jour du type de %x avec %x", this, type) ;
  set_B_type(type, get_ref_lexem()) ;
  type->unlink() ;
}

void T_binary_op::make_bop_power_type(void)
{
#ifdef DEBUG_BINOP_TYPE
  TRACE1("T_binary_op(%x)::make_bop_power_type", this) ;
#endif

  T_item *cur = first_operand ;
  T_builtin_type fop = BTYPE_BOOL;

  while (cur != NULL)
        {
          cur->make_type() ;


          if (cur->get_B_type() == NULL)
                {
                  // Reprise sur erreur
                  TRACE0("panic, cur->get_B_type() = NULL") ;
                  return ;
                }

          if (fop == BTYPE_BOOL) {

              if (cur->get_B_type()->is_an_integer() == TRUE) {
                  fop = BTYPE_INTEGER;
              } else if (cur->get_B_type()->is_a_real() == TRUE) {
                  fop = BTYPE_REAL;
              } else {
                  semantic_error(cur,
                                 CAN_CONTINUE,
                                 get_error_msg(E_OPERAND_SHOULD_BE_AN_INTEGER_OR_REAL),
                                 make_type_name(cur)->get_value()) ;
              }
          } else if (cur->get_B_type()->is_an_integer() == FALSE) {
              semantic_error(cur,
                             CAN_CONTINUE,
                             get_error_msg(E_OPERAND_SHOULD_BE_AN_INTEGER),
                             make_type_name(cur)->get_value()) ;
          }

          cur = cur->get_next() ;
        }

  T_type *type  = new T_predefined_type(fop,
                                                                           NULL,
                                                                           NULL,
                                                                           this,
                                                                           get_betree(),
                                                                           get_ref_lexem()) ;


  TRACE2("mise a jour du type de %x avec %x", this, type) ;
  set_B_type(type, get_ref_lexem()) ;
  type->unlink() ;
}


// CTRL Ref: VTYPE 2-3 (6.1)
void T_binary_op::make_bop_minus_type(void)
{
#ifdef DEBUG_BINOP_TYPE
    TRACE1("T_binary_op(%x)::make_bop_minus_type", this) ;
#endif


    // Deux types de "moins"
    // Moins entier : CTRL Ref: VTYPE 2-1 (1.2)
    // Moins ensembliste : CTRL Ref: VTYPE 2-3 (6.1)
    int int_minus = 0 ; 	// Pour savoir si c'est un moins entier 0:unknow 1:int 2:real 3:float
    int set_minus = FALSE ;		// Pour savoir si c'est un moins ensemblise

    T_item *cur = first_operand ;
    T_type *first_type = NULL ;
    int error = FALSE ;

    while (cur != NULL)
    {
        cur->make_type() ;

        if (cur->get_B_type() == NULL)
        {
            // Reprise sur erreur
            return ;
        }

        if (cur->get_B_type()->is_an_integer() == TRUE)
        {
            if (int_minus == 0)
            {
                if (set_minus == TRUE)
                {
                    // Erreur : c'est un moins ensemblise !
                    semantic_error(cur,
                                   CAN_CONTINUE,
                                   get_error_msg(E_OPERAND_SHOULD_BE_A_SET),
                                   make_type_name(cur)->get_value()) ;
                    error = TRUE ;
                }
                else
                {
                    // Pas de probleme !
#ifdef DEBUG_BINOP_TYPE
                    TRACE0("On passe en mode int_minus") ;
#endif

                    int_minus = 1 ;
                }
            } else if (int_minus == 2) {
                semantic_error(cur,
                               CAN_CONTINUE,
                               get_error_msg(E_OPERAND_SHOULD_BE_A_REAL),
                               make_type_name(cur)->get_value()) ;
                error = TRUE ;
            } else if (int_minus == 3) {
                semantic_error(cur,
                               CAN_CONTINUE,
                               get_error_msg(E_OPERAND_SHOULD_BE_A_FLOAT),
                               make_type_name(cur)->get_value()) ;
                error = TRUE ;
            }
            // Sinon c'est que int_minus == TRUE, et donc qu'il n'y a pas de
            // probleme
        }
        else if (cur->get_B_type()->is_a_real() == TRUE)
        {
            if (int_minus == 0)
            {
                if (set_minus == TRUE)
                {
                    // Erreur : c'est un moins ensemblise !
                    semantic_error(cur,
                                   CAN_CONTINUE,
                                   get_error_msg(E_OPERAND_SHOULD_BE_A_SET),
                                   make_type_name(cur)->get_value()) ;
                    error = TRUE ;
                }
                else
                {
                    // Pas de probleme !
#ifdef DEBUG_BINOP_TYPE
                    TRACE0("On passe en mode int_minus") ;
#endif

                    int_minus = 2 ;
                }
            } else if (int_minus == 1) {
                semantic_error(cur,
                               CAN_CONTINUE,
                               get_error_msg(E_OPERAND_SHOULD_BE_AN_INTEGER),
                               make_type_name(cur)->get_value()) ;
                error = TRUE ;
            } else if (int_minus == 3) {
                semantic_error(cur,
                               CAN_CONTINUE,
                               get_error_msg(E_OPERAND_SHOULD_BE_A_FLOAT),
                               make_type_name(cur)->get_value()) ;
                error = TRUE ;
            }
            // Sinon c'est que int_minus == TRUE, et donc qu'il n'y a pas de
            // probleme
        }
        else if (cur->get_B_type()->is_a_float() == TRUE)
        {
            if (int_minus == 0)
            {
                if (set_minus == TRUE)
                {
                    // Erreur : c'est un moins ensemblise !
                    semantic_error(cur,
                                   CAN_CONTINUE,
                                   get_error_msg(E_OPERAND_SHOULD_BE_A_SET),
                                   make_type_name(cur)->get_value()) ;
                    error = TRUE ;
                }
                else
                {
                    // Pas de probleme !
#ifdef DEBUG_BINOP_TYPE
                    TRACE0("On passe en mode int_minus") ;
#endif

                    int_minus = 3 ;
                }
            } else if (int_minus == 1) {
                semantic_error(cur,
                               CAN_CONTINUE,
                               get_error_msg(E_OPERAND_SHOULD_BE_AN_INTEGER),
                               make_type_name(cur)->get_value()) ;
                error = TRUE ;
            } else if (int_minus == 2) {
                semantic_error(cur,
                               CAN_CONTINUE,
                               get_error_msg(E_OPERAND_SHOULD_BE_A_REAL),
                               make_type_name(cur)->get_value()) ;
                error = TRUE ;
            }
            // Sinon c'est que int_minus == TRUE, et donc qu'il n'y a pas de
            // probleme
        }
        else if (cur->is_a_set() == TRUE)
        {
            if (set_minus == FALSE)
            {
                if (int_minus == 1)
                {
                    // Erreur : c'est un moins entier !
                    semantic_error(cur,
                                   CAN_CONTINUE,
                                   get_error_msg(E_OPERAND_SHOULD_BE_AN_INTEGER),
                                   make_type_name(cur)->get_value()) ;
                    error = TRUE ;
                }
                else if (int_minus == 2)
                {
                   semantic_error(cur,
                                   CAN_CONTINUE,
                                   get_error_msg(E_OPERAND_SHOULD_BE_A_REAL),
                                   make_type_name(cur)->get_value()) ;
                    error = TRUE ;
                }
                else if (int_minus == 3)
                {
                   semantic_error(cur,
                                   CAN_CONTINUE,
                                   get_error_msg(E_OPERAND_SHOULD_BE_A_FLOAT),
                                   make_type_name(cur)->get_value()) ;
                    error = TRUE ;
                }
                else
                {
                    // Pas de probleme !
#ifdef DEBUG_BINOP_TYPE
                    TRACE0("On passe en mode set_minus") ;
#endif

                    set_minus = TRUE ;
                }
            }

            // Sinon c'est que set_minus == TRUE, et donc qu'il n'y a pas de
            // probleme, a condition que les ensembles soient compatibles
            if (cur != first_operand)
            {
                // Les deux premiers tests du if sont necessaires
                // pour la reprise sur erreur
                if ( (cur->get_B_type() != NULL)
                     && (first_type != NULL)
                     && (cur->get_B_type()->is_compatible_with(first_type) == FALSE) )
                {
                    semantic_error(cur,
                                   CAN_CONTINUE,
                                   get_error_msg(E_UNCOMPATIBLE_SET_OPERAND),
                                   make_type_name(cur)->get_value(),
                                   first_type->make_type_name()->get_value()) ;
                    error = TRUE ;
                }
            }
            else
            {
                first_type = cur->get_B_type() ;
            }
        }
        else
        {
            // Erreur : l'operande n'est ni un entier, ni un ensemble
            semantic_error(cur,
                           CAN_CONTINUE,
                           get_error_msg(E_OPERAND_SHOULD_BE_AN_INTEGER_OR_A_SET),
                           make_type_name(cur)->get_value()) ;
            error = TRUE ;
        }

        cur = cur->get_next() ;
    }

    if (error == TRUE)
    {
        // On ne peut pas continuer
        TRACE0("panic !") ;
        return ;
    }

    if (int_minus == 1)
    {
#ifdef DEBUG_BINOP_TYPE
        TRACE0("On fabrique un type entier") ;
#endif

        T_type *type = new T_predefined_type(BTYPE_INTEGER,
                                             NULL,
                                             NULL,
                                             this,
                                             get_betree(),
                                             get_ref_lexem()) ;
        set_B_type(type, get_ref_lexem()) ;
        type->unlink() ;
    }
    else if (int_minus == 2)
    {
        T_type *type = new T_predefined_type(BTYPE_REAL,
                                             NULL,
                                             NULL,
                                             this,
                                             get_betree(),
                                             get_ref_lexem()) ;
        set_B_type(type, get_ref_lexem()) ;
        type->unlink() ;
    }
    else if (int_minus == 3)
    {
        T_type *type = new T_predefined_type(BTYPE_FLOAT,
                                             NULL,
                                             NULL,
                                             this,
                                             get_betree(),
                                             get_ref_lexem()) ;
        set_B_type(type, get_ref_lexem()) ;
        type->unlink() ;
    }
    else /* set_minus == TRUE */
    {
#ifdef DEBUG_BINOP_TYPE
        TRACE0("On fabrique un type ensemble ( = type de op1)") ;
#endif

        T_type *base = first_type->clone_type(NULL, get_betree(), get_ref_lexem()) ;
        // Il faut mettre les bornes a NULL !
        base->reset_bounds() ;
        set_B_type(base, get_ref_lexem()) ;
        base->unlink() ;
    }
}


void T_binary_op::make_bop_times_type(void)
{
#ifdef DEBUG_BINOP_TYPE
    TRACE1("T_binary_op(%x)::make_bop_times_type", this) ;
#endif


    // Deux types de "multiplication"
    // Multiplication entiere : CTRL Ref: VTYPE 2-1 (1.2)
    // Multiplication ensembliste : CTRL Ref: VTYPE 2-3 (1)
    int int_times = 0 ; 	// Pour savoir si c'est un moins entier 0:unknow 1:int 2:real 3:float
    int set_times = FALSE ;		// Pour savoir si c'est un moins ensemblise

    T_item *cur = first_operand ;

    while (cur != NULL)
    {
        cur->make_type() ;

        if (cur->is_an_expr() == FALSE)
        {
            semantic_error(cur->get_ref_lexem(),
                           CAN_CONTINUE,
                           get_error_msg(E_NOT_AN_EXPRESSION),
                           make_class_name(cur)) ;
            return ;
        }

        // On sait que cur est une expression : on cherche son type
        TRACE2("ici cur %x:%s", cur, cur->get_class_name()) ;
        if (cur->get_B_type() == NULL)
        {
            // Reprise sur erreur
            return ;
        }

        if (cur->get_B_type()->is_an_integer() == TRUE)
        {
            if (int_times == 0)
            {
                if (set_times == TRUE)
                {
                    // Erreur : c'est un moins ensemblise !
                    semantic_error(cur,
                                   CAN_CONTINUE,
                                   get_error_msg(E_OPERAND_SHOULD_BE_A_SET),
                                   make_type_name(cur)->get_value()) ;
                }
                else
                {
                    // Pas de probleme !
#ifdef DEBUG_BINOP_TYPE
                    TRACE0("On passe en mode int_times") ;
#endif

                    int_times = 1 ;
                }
            } else if (int_times == 2) {
                semantic_error(cur,
                               CAN_CONTINUE,
                               get_error_msg(E_OPERAND_SHOULD_BE_A_REAL),
                               make_type_name(cur)->get_value()) ;
            } else if (int_times == 3) {
                semantic_error(cur,
                               CAN_CONTINUE,
                               get_error_msg(E_OPERAND_SHOULD_BE_A_FLOAT),
                               make_type_name(cur)->get_value()) ;
            }
            // Sinon c'est que int_times == TRUE, et donc qu'il n'y a pas de
            // probleme
        }
        else if (cur->get_B_type()->is_a_real() == TRUE)
        {
            if (int_times == 0)
            {
                if (set_times == TRUE)
                {
                    // Erreur : c'est un moins ensemblise !
                    semantic_error(cur,
                                   CAN_CONTINUE,
                                   get_error_msg(E_OPERAND_SHOULD_BE_A_SET),
                                   make_type_name(cur)->get_value()) ;
                }
                else
                {
                    // Pas de probleme !
#ifdef DEBUG_BINOP_TYPE
                    TRACE0("On passe en mode int_times") ;
#endif

                    int_times = 2 ;
                }
            } else if (int_times == 1) {
                semantic_error(cur,
                               CAN_CONTINUE,
                               get_error_msg(E_OPERAND_SHOULD_BE_AN_INTEGER),
                               make_type_name(cur)->get_value()) ;
            } else if (int_times == 3) {
                semantic_error(cur,
                               CAN_CONTINUE,
                               get_error_msg(E_OPERAND_SHOULD_BE_A_FLOAT),
                               make_type_name(cur)->get_value()) ;
            }
            // Sinon c'est que int_times == TRUE, et donc qu'il n'y a pas de
            // probleme
        }
        else if (cur->get_B_type()->is_a_float() == TRUE)
        {
            if (int_times == 0)
            {
                if (set_times == TRUE)
                {
                    // Erreur : c'est un moins ensemblise !
                    semantic_error(cur,
                                   CAN_CONTINUE,
                                   get_error_msg(E_OPERAND_SHOULD_BE_A_SET),
                                   make_type_name(cur)->get_value()) ;
                }
                else
                {
                    // Pas de probleme !
#ifdef DEBUG_BINOP_TYPE
                    TRACE0("On passe en mode int_times") ;
#endif

                    int_times = 3 ;
                }
            } else if (int_times == 1) {
                semantic_error(cur,
                               CAN_CONTINUE,
                               get_error_msg(E_OPERAND_SHOULD_BE_AN_INTEGER),
                               make_type_name(cur)->get_value()) ;
            } else if (int_times == 2) {
                semantic_error(cur,
                               CAN_CONTINUE,
                               get_error_msg(E_OPERAND_SHOULD_BE_A_REAL),
                               make_type_name(cur)->get_value()) ;
            }
            // Sinon c'est que int_times == TRUE, et donc qu'il n'y a pas de
            // probleme
        }
        else if (cur->is_a_set() == TRUE)
        {
            if (set_times == FALSE)
            {
                if (int_times == 1)
                {
                    // Erreur : c'est un moins entier !
                    semantic_error(cur,
                                   CAN_CONTINUE,
                                   get_error_msg(E_OPERAND_SHOULD_BE_AN_INTEGER),
                                   make_type_name(cur)->get_value()) ;
                }
                else if (int_times == 2)
                {
                    // Erreur : c'est un moins entier !
                    semantic_error(cur,
                                   CAN_CONTINUE,
                                   get_error_msg(E_OPERAND_SHOULD_BE_A_REAL),
                                   make_type_name(cur)->get_value()) ;
                }
                else if (int_times == 3)
                {
                    // Erreur : c'est un moins entier !
                    semantic_error(cur,
                                   CAN_CONTINUE,
                                   get_error_msg(E_OPERAND_SHOULD_BE_A_FLOAT),
                                   make_type_name(cur)->get_value()) ;
                }
                else
                {
                    // Pas de probleme !
#ifdef DEBUG_BINOP_TYPE
                    TRACE0("On passe en mode set_times") ;
#endif

                    set_times = TRUE ;
                }
            }

            // Sinon c'est que set_times == TRUE, et donc qu'il n'y a pas de
            // probleme (!!! dans E1 - E2, les deux types doivent etre
            // compatibles. Ce n'est pas le cas dans E1 * E2)
        }
        else
        {
            // Erreur : l'operande n'est ni un entier, ni un ensemble
            semantic_error(cur,
                           CAN_CONTINUE,
                           get_error_msg(E_OPERAND_SHOULD_BE_AN_INTEGER_OR_A_SET),
                           make_type_name(cur)->get_value()) ;
        }

        cur = cur->get_next() ;
    }

    if (int_times == 1)
    {
#ifdef DEBUG_BINOP_TYPE
        TRACE0("On fabrique un type entier") ;
#endif

        T_type *type = new T_predefined_type(BTYPE_INTEGER,
                                             NULL,
                                             NULL,
                                             this,
                                             get_betree(),
                                             get_ref_lexem()) ;
        set_B_type(type, get_ref_lexem()) ;
        type->unlink() ;
    }
    else if (int_times == 2)
    {
        T_type *type = new T_predefined_type(BTYPE_REAL,
                                             NULL,
                                             NULL,
                                             this,
                                             get_betree(),
                                             get_ref_lexem()) ;
        set_B_type(type, get_ref_lexem()) ;
        type->unlink() ;
    }
    else if (int_times == 3)
    {
        T_type *type = new T_predefined_type(BTYPE_FLOAT,
                                             NULL,
                                             NULL,
                                             this,
                                             get_betree(),
                                             get_ref_lexem()) ;
        set_B_type(type, get_ref_lexem()) ;
        type->unlink() ;
    }
    else /* set_times == TRUE */
    {
        // Fabrication d'un SETOF(product)
        // CTRL Ref : VTYPE 2-3, cas 1
#ifdef DEBUG_BINOP_TYPE
        TRACE0("ICI ON FABRIQUE UN SETOF(PRODUCT_TYPE)") ;
        ENTER_TRACE ;
#endif

        // on recalcule le type des operandes binaires
        // necessaire dans le cas d'operandes naires, op2 est un binary_op
        // qui n'a pas encore de type
        op1->make_type() ;
        op2->make_type() ;

        // Premier type
        if (op1->is_a_set() == FALSE)
        {
            TRACE1("!! %x n'est pas un ensemble", op1) ;
            semantic_error(op1,
                           CAN_CONTINUE,
                           get_error_msg(E_ITEM_OF_CARTESIAN_PROD_IS_NOT_A_SET)) ;
            return ;
        }

        T_setof_type *cur_type = ((T_setof_type *)(op1->get_B_type())) ;
        TRACE3("op1 %s:%d:%d",
               op1->get_ref_lexem()->get_file_name()->get_value(),
               op1->get_ref_lexem()->get_file_line(),
               op1->get_ref_lexem()->get_file_column()) ;
        TRACE2("cur_type %x:%s", cur_type, cur_type->get_class_name()) ;
        //ASSERT(cur_type->get_node_type() == NODE_SETOF_TYPE) ;
        ASSERT(cur_type->is_a_set() == TRUE) ;
        T_type *cur_base_type = cur_type->get_base_type() ;
        TRACE1("cur_base_type %x", cur_base_type) ;
        T_type *cl1 = cur_base_type->clone_type(NULL, get_betree(), get_ref_lexem()) ;

        // Deuxieme type
        if (op2->is_a_set() == FALSE)
        {
            TRACE1("!! %x n'est pas un ensemble", op2) ;
            semantic_error(op2,
                           CAN_CONTINUE,
                           get_error_msg(E_ITEM_OF_CARTESIAN_PROD_IS_NOT_A_SET)) ;
            return ;
        }

        TRACE2("op2 = %x:%s", op2, op2->get_class_name()) ;
        cur_type = ((T_setof_type *)(op2->get_B_type())) ;
        TRACE2("cur_type = %x:%s", cur_type, cur_type->get_class_name()) ;
        TRACE2("cur_type = %x:%s", cur_type, cur_type->make_type_name()->get_value());
        cur_base_type = cur_type->get_base_type() ;
        TRACE1("cur_base_type = %x", cur_base_type) ;
        TRACE2("cur_base_type = %x:%s",
               cur_base_type, cur_base_type->get_class_name()) ;
        TRACE2("cur_base_type = %x:%s",
               cur_base_type, cur_base_type->make_type_name()->get_value()) ;
        T_type *cl2 = cur_base_type->clone_type(NULL, get_betree(), get_ref_lexem()) ;


        // Produit cartesien
        T_product_type *prod = new T_product_type(cl1,
                                                  cl2,
                                                  NULL, // mis a jour ds setof
                                                  get_betree(),
                                                  get_ref_lexem()) ;

        cl1->unlink() ;
        cl2->unlink() ;


        // Resultat
        T_setof_type *setof = new T_setof_type(prod,
                                               this,
                                               get_betree(),
                                               get_ref_lexem()) ;
        prod->unlink() ;
        set_B_type(setof, get_ref_lexem()) ;
        setof->unlink() ;
#ifdef DEBUG_BINOP_TYPE
        EXIT_TRACE ;
        TRACE2("FIN FABRICATION DU PRODUCT TYPE : _this = %x, type %x",
               this,
               get_B_type()) ;
#endif
        return ;
    }
}

// CTRL Ref: VTYPE 2-4 (1)
void T_binary_op::make_bop_set_relation_type(void)
{
#ifdef DEBUG_BINOP_TYPE
  TRACE2("T_binary_op(%x)::make_bop_set_relation_type (nb_operands = %d)",
		 this, nb_operands) ;
#endif

  T_item *cur = first_operand ;
  int error = FALSE ;

  while (cur != NULL)
	{
	  if (cur->is_a_set() == FALSE)
		{
		  semantic_error(cur,
						 CAN_CONTINUE,
						 get_error_msg(E_OPERAND_OF_SET_RELATION_IS_NOT_A_SET),
						 make_type_name(cur)->get_value()) ;
		  error = TRUE ;
		}
	  cur = cur->get_next() ;
	}

  if (error == TRUE)
	{
	  // On ne peut pas continuer
	  return ;
	}

  // Relation A <--> B
  // Type de A : Setof(tA)
  // Type de B : Setof(tB)
  // On fabrique un Setof(Setof(tA * tB))

  // 1) on cree le produit cartesien
  op1->make_type() ;
  op2->make_type() ;

  // Casts justifies par les tests ci-dessus (cur->is_a_set)
  T_setof_type *s1 = (T_setof_type *)op1->get_B_type() ;
  T_setof_type *s2 = (T_setof_type *)op2->get_B_type() ;

  T_type *b1 = s1->get_base_type()->clone_type(NULL, get_betree(), get_ref_lexem()) ;
  T_type *b2 = s2->get_base_type()->clone_type(NULL, get_betree(), get_ref_lexem()) ;

  T_product_type *prod = new T_product_type(b1,
											b2,
											NULL,
											get_betree(),
											get_ref_lexem()) ;

  b1->unlink() ;
  b2->unlink() ;

  // 3) on cree le setof(setof)
  T_setof_type *set1 ;
  T_setof_type *set2 ;
  set1 = new T_setof_type(prod, NULL, get_betree(), get_ref_lexem()) ;
  prod->unlink() ;
  set2 = new T_setof_type(set1, NULL, get_betree(), get_ref_lexem()) ;
  set1->unlink() ;

  // Et c'est tout !
  set_B_type(set2, get_ref_lexem()) ;
  set2->unlink() ;
}

// CTRL Ref: VTYPE 2-4 (5)
void T_binary_op::make_bop_composition_type(void)
{
#ifdef DEBUG_BINOP_TYPE
  TRACE2("T_binary_op(%x)::make_bop_composition_type (nb_operands = %d)",
		 this, nb_operands) ;
#endif

  // Toutes les operandes doivent etre des relations
  // On le fait en binaire

  op1->make_type() ;
  op2->make_type() ;

  int error = FALSE ;
  TRACE2("CHECK_OP1 %s;%s",
		 op1->get_class_name(),
		 make_type_name(op1)->get_value()) ;
  if (op1->is_a_relation() == FALSE)
	{
	  semantic_error(op1,
					 CAN_CONTINUE,
					 get_error_msg(E_OPERAND_OF_COMPOSITION_SHOULD_BE_A_RELATION),
					 make_type_name(op1)->get_value()) ;
	  error = TRUE ;
	}

  TRACE2("CHECK_OP2 %s;%s",
		 op2->get_class_name(),
		 make_type_name(op2)->get_value()) ;
  if (op2->is_a_relation() == FALSE)
	{
	  semantic_error(op2,
					 CAN_CONTINUE,
					 get_error_msg(E_OPERAND_OF_COMPOSITION_SHOULD_BE_A_RELATION),
					 make_type_name(op2)->get_value()) ;
	  error = TRUE ;
	}

  if (error == TRUE)
	{
	  // On ne peut pas continuer
	  return ;
	}

  // On travaille en binaire
  // Casts justifies par les tests ci-dessus
  T_setof_type *set1 = ((T_setof_type *)op1->get_B_type()) ;
  T_setof_type *set2 = ((T_setof_type *)op2->get_B_type()) ;

  T_type *first_dst = set1->get_relation_dst_type() ;
  T_type *second_src = set2->get_relation_src_type() ;


  if (second_src->is_equal_to(first_dst) == FALSE)
	{
	  semantic_error(this,
					 CAN_CONTINUE,
					 get_error_msg(E_OPERANDS_OF_COMPOSITION_SHOULD_HAVE_MATCHING_TYPES),
					 first_dst->make_type_name()->get_value(),
					 second_src->make_type_name()->get_value()) ;
	  semantic_error_details(second_src,
							 get_error_msg(E_LOCATION_OF_BOGUS_COMPOSITION_OPERAND)) ;
	}

  // On fabrique une relation du premier ensemble de depart vers le
  // dernier ensemble d'arrivee
  // Casts justifies par les premiers tests
  T_type *c1 = set1->get_relation_src_type()->clone_type(NULL,
														 get_betree(),
														 get_ref_lexem()) ;
  T_type *c2 = set2->get_relation_dst_type()->clone_type(NULL,
														 get_betree(),
														 get_ref_lexem()) ;

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

// CTRL Ref: VTYPE 2-2 (1)
void T_binary_op::make_bop_maplet_type(void)
{
#ifdef DEBUG_BINOP_TYPE
  TRACE2("T_binary_op(%x)::make_bop_maplet_type (nb_operands = %d)",
		 this, nb_operands) ;
#endif


  op1->make_type() ;
  op2->make_type() ;

  if ( (op1->get_B_type() == NULL) || (op2->get_B_type() == NULL) )
	{
	  // Reprise sur erreur
#ifdef DEBUG_BINOP_TYPE
	  TRACE8("op1 %x:%s type %x:%s, op2 %x:%s type %x:%s reprise sur erreur",
			 op1,
			 op1->get_class_name(),
			 op1->get_B_type(),
			 make_type_name(op1)->get_value(),
			 op2,
			 op2->get_class_name(),
			 op2->get_B_type(),
			 make_type_name(op2)->get_value()) ;
#endif
	  return ;
	}

  T_type *t1 = op1->get_B_type()->clone_type(NULL, get_betree(), get_ref_lexem()) ;
  T_type *t2 = op2->get_B_type()->clone_type(NULL, get_betree(), get_ref_lexem()) ;

  T_product_type *prod ;
  prod = new T_product_type(t1, t2, NULL, get_betree(), get_ref_lexem()) ;
  t1->unlink() ;
  t2->unlink() ;
  set_B_type(prod, get_ref_lexem()) ;
  prod->unlink() ;
}

// CTRL Ref: VTYPE 2-3 (6.2, 6.3)
void T_binary_op::make_bop_union_intersect_type(void)
{
#ifdef DEBUG_BINOP_TYPE
  TRACE1("T_binary_op(%x)::make_bop_union_intersect", this) ;
#endif


  T_item *cur = first_operand ;
  T_type *first_type = NULL ;
  int error = FALSE ;
  while (cur != NULL)
	{
	  cur->make_type() ;
	  T_type *res = cur->get_B_type() ;

	  if (res == NULL)
		{
		  error = TRUE ;
		}
	  else
		{
		  if (!res->is_a_set())
			{
			  semantic_error(res,
							 CAN_CONTINUE,
							 get_error_msg(E_OPERAND_SHOULD_BE_A_SET),
							 res->make_type_name()->get_value()) ;
			  error = TRUE ;
			}
		  else if (first_type == NULL)
			{
			  first_type = res ;
			}
		  else if (res->is_equal_to(first_type) == FALSE)
			{
			  semantic_error(res,
							 CAN_CONTINUE,
							 get_error_msg(E_INCOMPATIBLE_TYPES_IN_EXPR),
							 res->make_type_name()->get_value(),
							 first_type->make_type_name()->get_value()) ;
			  error = TRUE ;
			}
		}
	  cur = cur->get_next() ;
	}

  if (error == TRUE)
	{
	  return ;
	}

  T_type *res = first_type->clone_type(this, get_betree(), get_ref_lexem()) ;

  // Il faut mettre les bornes a NULL
  res->reset_bounds() ;

  set_B_type(res, get_ref_lexem()) ;
  res->unlink() ;
}

// CTRL Ref: VTYPE 2-6 (6)
// sequence ^ sequence
void T_binary_op::make_bop_concat_type(void)
{
#ifdef DEBUG_BINOP_TYPE
  TRACE1("T_binary_op(%x)::make_bop_head_insert", this) ;
#endif

  op1->make_type() ;
  op2->make_type() ;

  if ( (op1->get_B_type() == NULL) || (op2->get_B_type() == NULL) )
	{
	  // Reprise sur erreur
	  return ;
	}

  T_setof_type *seq_type1 = (T_setof_type *)op1->get_B_type() ;

  if (op1->is_a_seq() == FALSE)
	{
	  semantic_error(op1,
					 CAN_CONTINUE,
					 get_error_msg(E_LHS_OF_BINOP_SHOULD_BE_A_SEQ),
					 seq_type1->make_type_name()->get_value()) ;
	  return ;
	}

  T_setof_type *seq_type2 = (T_setof_type *)op2->get_B_type() ;
  if (op2->is_a_seq() == FALSE)
	{
	  semantic_error(op2,
					 CAN_CONTINUE,
					 get_error_msg(E_RHS_OF_BINOP_SHOULD_BE_A_SEQ),
					 seq_type2->make_type_name()->get_value()) ;
	  return ;
	}

  if (seq_type1->is_equal_to(seq_type2) == FALSE)
	{
	  semantic_error(this,
					 CAN_CONTINUE,
					 get_error_msg(E_EXPR_INCOMPATIBLE_TYPES),
					 make_type_name(op1)->get_value(),
					 make_type_name(op2)->get_value()) ;
	}

  // Resultat : type de s1
  T_type *res = seq_type1->clone_type(this, get_betree(), get_ref_lexem()) ;
  set_B_type(res, get_ref_lexem()) ;
  res->unlink() ;
}


// CTRL Ref: VTYPE 2-6 (7)
// element -> sequence
// Il faut verifier que l'element est du type des elements de
// la sequence. Si oui, le type est celui de la sequence
void T_binary_op::make_bop_head_insert_type(void)
{
#ifdef DEBUG_BINOP_TYPE
  TRACE1("T_binary_op(%x)::make_bop_head_insert", this) ;
#endif

  op1->make_type() ;
  op2->make_type() ;

  if ( (op1->get_B_type() == NULL) || (op2->get_B_type() == NULL) )
	{
	  // Reprise sur erreur
	  return ;
	}

  T_setof_type *seq_type = (T_setof_type *)op2->get_B_type() ;
  T_type *item_type = op1->get_B_type() ;

  if (op2->is_a_seq() == FALSE)
	{
	  semantic_error(op2,
					 CAN_CONTINUE,
					 get_error_msg(E_RHS_OF_BINOP_SHOULD_BE_A_SEQ),
					 seq_type->make_type_name()->get_value()) ;
	  return ;
	}

  // Cast valide car c'est une sequence
  T_type *base_type = ((T_setof_type *)seq_type)->get_seq_base_type() ;

  if (item_type->is_equal_to(base_type) == FALSE)
	{
	  semantic_error(op1,
					 CAN_CONTINUE,
					 get_error_msg(E_ILLEGAL_TYPE_FOR_ITEM_TO_ADD_IN_SEQ),
					 item_type->make_type_name()->get_value(),
					 seq_type->make_type_name()->get_value()) ;
	  return ;
	}

  T_type *res = seq_type->clone_type(this, get_betree(), get_ref_lexem()) ;
  set_B_type(res, get_ref_lexem()) ;
  res->unlink() ;
}


// CTRL Ref: VTYPE 2-6 (8)
// element -> sequence
// Il faut verifier que l'element est du type des elements de
// la sequence. Si oui, le type est celui de la sequence
void T_binary_op::make_bop_tail_insert_type(void)
{
#ifdef DEBUG_BINOP_TYPE
  TRACE1("T_binary_op(%x)::make_bop_tail_insert", this) ;
#endif

  op1->make_type() ;
  op2->make_type() ;

  if ( (op1->get_B_type() == NULL) || (op2->get_B_type() == NULL) )
	{
	  // Reprise sur erreur
	  return ;
	}

  T_setof_type *seq_type = (T_setof_type *)op1->get_B_type() ;
  T_type *item_type = op2->get_B_type() ;

  if (op1->is_a_seq() == FALSE)
	{
	  semantic_error(op1,
					 CAN_CONTINUE,
					 get_error_msg(E_LHS_OF_BINOP_SHOULD_BE_A_SEQ),
					 seq_type->make_type_name()->get_value()) ;
	  return ;
	}

  // Cast valide car c'est une sequence
  T_type *base_type = ((T_setof_type *)seq_type)->get_seq_base_type() ;

  if (item_type->is_equal_to(base_type) == FALSE)
	{
	  semantic_error(op1,
					 CAN_CONTINUE,
					 get_error_msg(E_ILLEGAL_TYPE_FOR_ITEM_TO_ADD_IN_SEQ),
					 item_type->make_type_name()->get_value(),
					 seq_type->make_type_name()->get_value()) ;
	  return ;
	}

  T_type *res = seq_type->clone_type(this, get_betree(), get_ref_lexem()) ;
  set_B_type(res, get_ref_lexem()) ;
  res->unlink() ;
}

// CTRL Ref: VTYPE 2-6 (9)
void T_binary_op::make_bop_head_tail_restrict_type(void)
{
#ifdef DEBUG_BINOP_TYPE
  TRACE1("T_binary_op(%x)::make_bop_head_tail_restrict", this) ;
#endif

  op1->make_type() ;
  op2->make_type() ;

  if ( (op1->get_B_type() == NULL) || (op2->get_B_type() == NULL) )
	{
	  // Reprise sur erreur
	  return ;
	}

  T_setof_type *seq_type = (T_setof_type *)op1->get_B_type() ;

  if (op1->is_a_seq() == FALSE)
	{
	  semantic_error(op1,
					 CAN_CONTINUE,
					 get_error_msg(E_LHS_OF_BINOP_SHOULD_BE_A_SEQ),
					 seq_type->make_type_name()->get_value()) ;
	  return ;
	}

  if (op2->get_B_type()->is_an_integer() == FALSE)
	{
	  semantic_error(op2,
					 CAN_CONTINUE,
					 get_error_msg(E_OPERAND_SHOULD_BE_AN_INTEGER),
					 op2->get_B_type()->make_type_name()->get_value()) ;
	  return ;
	}

  T_type *res = seq_type->clone_type(this, get_betree(), get_ref_lexem()) ;
  set_B_type(res, get_ref_lexem()) ;
  res->unlink() ;
}

// CTRL Ref: VTYPE 2-4 (9)
void T_binary_op::make_bop_restrict_antirestrict_type(void)
{
#ifdef DEBUG_BINOP_TYPE
  TRACE5("T_binary_op(%x:%s:%d:%d)::make_bop_restrict_antirestrict_type (nb_operands = %d)",
		 this,
		 get_ref_lexem()->get_file_name()->get_value(),
		 get_ref_lexem()->get_file_line(),
		 get_ref_lexem()->get_file_column(),
		 nb_operands) ;
#endif


  // op1 doit etre un ensemble de type Setof(t) et op2 doit etre une
  // relation de type Setof(t*v). Alors le resultat est Setof(t*v)
  op1->make_type() ;
  op2->make_type() ;

  // Lexeme de l'operateur binaire
  // Pour des pb de construction, get_ref_lexem() rend le premier lexeme
  // de op1
  if ( (op1->get_B_type() == NULL) || (op2->get_B_type() == NULL) )
	{
	  return ;
	}

  if (op2->is_a_relation() == FALSE)
	{
	  semantic_error(op2,
					 CAN_CONTINUE,
					 get_error_msg(E_SECOND_OPERAND_OF_ANTI_RESTRICT_IS_NOT_A_RELATION),
					 make_type_name(op2)->get_value()) ;
	  return ;
	}

  if (op1->is_a_set() == FALSE)
	{
	  semantic_error(op1,
					 CAN_CONTINUE,
					 get_error_msg(E_FIRST_OPERAND_OF_ANTI_RESTRICT_IS_NOT_A_SET),
					 make_type_name(op1)->get_value()) ;
	  return ;
	}

  // Casts justifies par les tests ci-dessus
  T_setof_type *op1type = (T_setof_type *)op1->get_B_type() ;
  T_setof_type *op2type = (T_setof_type *)op2->get_B_type() ;

  T_type *op2src = op2type->get_relation_src_type() ;
  T_type *op1base = op1type->get_base_type() ;

#ifdef DEBUG_BINOP_TYPE
  TRACE1("CHECK op2src %s", op2src->make_type_name()->get_value()) ;
  TRACE1("CHECK op1base %s", op1base->make_type_name()->get_value()) ;
#endif // DEBUG_BINOP_TYPE

  if (    (op2src->is_equal_to(op1base) == FALSE)
		  // DEBUT correction bug 2167
	   && (op1base->get_node_type() != NODE_GENERIC_TYPE)  // joker lhs
		  // FIN correction bug 2167
	   && (op2src->get_node_type() != NODE_GENERIC_TYPE) ) // joker rhs
	{
	  semantic_error(this,
					 CAN_CONTINUE,
					 get_error_msg(E_OP2RELSRC_INCOMPATIBLE_TYPES_OP1BASE),
					 op2type->get_relation_src_type()->make_type_name()->get_value(),
					 op1type->make_type_name()->get_value()) ;
	}

  T_type *res = op2type->clone_type(this, get_betree(), get_ref_lexem()) ;

  set_B_type(res, get_ref_lexem()) ;
  res->unlink() ;
}

// CTRL Ref: VTYPE 2-4 (10)
void T_binary_op::make_bop_corestrict_anticorestrict_type(void)
{
#ifdef DEBUG_BINOP_TYPE
  TRACE2("T_binary_op(%x)::make_bop_corestrict_anticorestrict_type (nb_op = %d)",
		 this, nb_operands) ;
#endif


  // op2 doit etre un ensemble de type Setof(t) et op1 doit etre une
  // relation de type Setof(t*v). Alors le resultat est Setof(t*v)
  op2->make_type() ;
  op1->make_type() ;

  if ( (op2->get_B_type() == NULL) || (op1->get_B_type() == NULL) )
	{
	  return ;
	}

  if (op1->is_a_relation() == FALSE)
	{
	  semantic_error(op1,
					 CAN_CONTINUE,
					 get_error_msg(E_FIRST_OPERAND_OF_CORESTRICT_IS_NOT_A_RELATION),
					 make_type_name(op1)->get_value()) ;
	  return ;
	}

  if (op2->is_a_set() == FALSE)
	{
	  semantic_error(op2,
					 CAN_CONTINUE,
					 get_error_msg(E_SECOND_OPERAND_OF_CORESTRICT_IS_NOT_A_SET),
					 make_type_name(op2)->get_value()) ;
	  return ;
	}

  // Casts justifies par les tests ci-dessus
  T_setof_type *op2type = (T_setof_type *)op2->get_B_type() ;
  T_setof_type *op1type = (T_setof_type *)op1->get_B_type() ;


  T_type *op1dst = op1type->get_relation_dst_type() ;
  T_type *op2base = op2type->get_base_type() ;

#ifdef DEBUG_BINOP_TYPE
  TRACE1("CHECK op1dst %s", op1dst->make_type_name()->get_value()) ;
  TRACE1("CHECK op2base %s", op2base->make_type_name()->get_value()) ;
#endif // DEBUG_BINOP_TYPE

  if (    (op1dst->is_equal_to(op2base) == FALSE)
		  // DEBUT correction Bug 2167
	   && (op2base->get_node_type() != NODE_GENERIC_TYPE)  // joker rhs
		  // FIN correction Bug 2167
	   && (op1dst->get_node_type() != NODE_GENERIC_TYPE) ) // joker lhs
	{
	  semantic_error(this,
					 CAN_CONTINUE,
					 get_error_msg(E_OP1RELDST_INCOMPATIBLE_TYPES_OP2BASE),
					 op1type->get_relation_dst_type()->make_type_name()->get_value(),
					 op2type->make_type_name()->get_value()) ;
	}

  T_type *res = op1->get_B_type()->clone_type(this, get_betree(), get_ref_lexem()) ;

  set_B_type(res, get_ref_lexem()) ;
  res->unlink() ;
}

// CTRL Ref: VTYPE 2-4 (11)
void T_binary_op::make_bop_direct_product_type(void)
{
#ifdef DEBUG_BINOP_TYPE
  TRACE2("T_binary_op(%x)::make_bop_direct_product_type (nb_operands = %d)",
		 this, nb_operands) ;
#endif


  // op1 doit etre une relation de t -> u et op2 une relation de t -> v
  // Resultat: Seotf(t*(u*v))
  op2->make_type() ;
  op1->make_type() ;

  if ( (op2->get_B_type() == NULL) || (op1->get_B_type() == NULL) )
	{
	  return ;
	}

  if (op1->is_a_relation() == FALSE)
	{
	  semantic_error(op1,
					 CAN_CONTINUE,
					 get_error_msg(E_FIRST_OPERAND_OF_DPROD_IS_NOT_A_RELATION),
					 make_type_name(op1)->get_value()) ;
	  return ;
	}

  if (op2->is_a_relation() == FALSE)
	{
	  semantic_error(op2,
					 CAN_CONTINUE,
					 get_error_msg(E_SECOND_OPERAND_OF_DPROD_IS_NOT_A_RELATION),
					 make_type_name(op2)->get_value()) ;
	  return ;
	}

  // Casts justifies par les tests ci-dessus
  T_setof_type *op2type = (T_setof_type *)op2->get_B_type() ;
  T_setof_type *op1type = (T_setof_type *)op1->get_B_type() ;

  T_type *op1_src = op1type->get_relation_src_type() ;
  T_type *op2_src = op2type->get_relation_src_type() ;
  T_type *op1_dst = op1type->get_relation_dst_type() ;
  T_type *op2_dst = op2type->get_relation_dst_type() ;

  TRACE2("cmp op1_src %s op2_src %s",
		 make_type_name(op1_src)->get_value(),
		 make_type_name(op2_src)->get_value()) ;

  if (op1_src->is_equal_to(op2_src) == FALSE)
	{
	  semantic_error(this,
					 CAN_CONTINUE,
					 get_error_msg(E_EXPR_INCOMPATIBLE_TYPES),
					 op1type->make_type_name()->get_value(),
					 op2type->make_type_name()->get_value()) ;
	}

  // On fait: Setof(op1src*(op1dst*op2dst))

  T_type *c1 = op1_dst->clone_type(NULL, get_betree(), get_ref_lexem());
  T_type *c2 = op2_dst->clone_type(NULL, get_betree(), get_ref_lexem());

  T_product_type *p1 = new T_product_type(c1,
										  c2,
										  this,
										  get_betree(),
										  get_ref_lexem()) ;
  c1->unlink() ;
  c2->unlink() ;

  T_type *c3 = op1_src->clone_type(NULL, get_betree(), get_ref_lexem());

  T_product_type *p2 = new T_product_type(c3,
										  p1,
										  this,
										  get_betree(),
										  get_ref_lexem()) ;

  c3->unlink() ;
  p1->unlink() ;

  T_type *res = new T_setof_type(p2, this, get_betree(), get_ref_lexem()) ;
  p2->unlink() ;

  set_B_type(res, get_ref_lexem()) ;
  res->unlink() ;
}

// CTRL Ref: VTYPE 2-4 (12)
void T_binary_op::make_bop_parallel_product_type(void)
{
#ifdef DEBUG_BINOP_TYPE
  TRACE2("T_binary_op(%x)::make_bop_parallel_product_type (nb_operands = %d)",
		 this, nb_operands) ;
#endif


  // op1 doit etre une relation de t -> u et op2 une relation de v -> w
  // Resultat: Seotf((t*v)*(u*w))
  op2->make_type() ;
  op1->make_type() ;

#ifdef DEBUG_BINOP_TYPE
  TRACE1("op1 %s", make_type_name(op1)->get_value()) ;
  TRACE1("op2 %s", make_type_name(op2)->get_value()) ;
#endif // DEBUG_BINOP_TYPE

  if ( (op2->get_B_type() == NULL) || (op1->get_B_type() == NULL) )
	{
	  return ;
	}

  if (op1->is_a_relation() == FALSE)
	{
	  semantic_error(op1,
					 CAN_CONTINUE,
					 get_error_msg(E_FIRST_OPERAND_OF_PPROD_IS_NOT_A_RELATION),
					 make_type_name(op1)->get_value()) ;
	  return ;
	}

  if (op2->is_a_relation() == FALSE)
	{
	  semantic_error(op2,
					 CAN_CONTINUE,
					 get_error_msg(E_SECOND_OPERAND_OF_PPROD_IS_NOT_A_RELATION),
					 make_type_name(op2)->get_value()) ;
	  return ;
	}

  // Casts justifies par les tests ci-dessus
  T_setof_type *op2type = (T_setof_type *)op2->get_B_type() ;
  T_setof_type *op1type = (T_setof_type *)op1->get_B_type() ;

  T_type *op1_src = op1type->get_relation_src_type() ;
  T_type *op2_src = op2type->get_relation_src_type() ;
  T_type *op1_dst = op1type->get_relation_dst_type() ;
  T_type *op2_dst = op2type->get_relation_dst_type() ;

  // On fait: Setof((op1src*op2src)*(op1dst*op2dst))

  T_type *c1 = op1_dst->clone_type(NULL, get_betree(), get_ref_lexem());
  T_type *c2 = op2_dst->clone_type(NULL, get_betree(), get_ref_lexem());

  T_product_type *p1 = new T_product_type(c1,
										  c2,
										  this,
										  get_betree(),
										  get_ref_lexem()) ;

  c1->unlink() ;
  c2->unlink() ;

  T_type *c3 = op1_src->clone_type(NULL, get_betree(), get_ref_lexem());
  T_type *c4 = op2_src->clone_type(NULL, get_betree(), get_ref_lexem());

  T_product_type *p2 = new T_product_type(c3,
										  c4,
										  this,
										  get_betree(),
										  get_ref_lexem()) ;

  c3->unlink() ;
  c4->unlink() ;

  T_product_type *p3 = new T_product_type(p2,
										  p1,
										  this,
										  get_betree(),
										  get_ref_lexem()) ;

  p1->unlink() ;
  p2->unlink() ;

  T_type *res = new T_setof_type(p3, this, get_betree(), get_ref_lexem()) ;
  p3->unlink() ;

  set_B_type(res, get_ref_lexem()) ;
  res->unlink() ;
}

// CTRL Ref: VTYPE 2-4 (10)
void T_binary_op::make_bop_left_overload_type(void)
{
#ifdef DEBUG_BINOP_TYPE
  TRACE2("T_binary_op(%x)::make_bop_left_overload_type (nb_operands = %d)",
		 this, nb_operands) ;
#endif

  // op1 doit etre une relation de t -> v et op2 une relation de t -> v
  // Resultat: Seotf(t*v))
  op2->make_type() ;
  op1->make_type() ;

  if ( (op2->get_B_type() == NULL) || (op1->get_B_type() == NULL) )
	{
	  return ;
	}

  if (op1->is_a_relation() == FALSE)
	{
	  semantic_error(op1,
					 CAN_CONTINUE,
					 get_error_msg(E_FIRST_OPERAND_OF_LOVERLOAD_IS_NOT_A_RELATION),
					 make_type_name(op1)->get_value()) ;
	  return ;
	}

  if (op2->is_a_relation() == FALSE)
	{
	  semantic_error(op2,
					 CAN_CONTINUE,
					 get_error_msg(E_SECOND_OPERAND_OF_LOVERLOAD_IS_NOT_A_RELATION),
					 make_type_name(op2)->get_value()) ;
	  return ;
	}

  // Casts justifies par les tests ci-dessus
  // Casts justifies par les tests ci-dessus
  T_setof_type *op2type = (T_setof_type *)op2->get_B_type() ;
  T_setof_type *op1type = (T_setof_type *)op1->get_B_type() ;

  T_type *op1_src = op1type->get_relation_src_type() ;
  T_type *op2_src = op2type->get_relation_src_type() ;
  T_type *op1_dst = op1type->get_relation_dst_type() ;
  T_type *op2_dst = op2type->get_relation_dst_type() ;

  T_type *ref_type = op1type ;
  if (op1type->get_node_type() == NODE_GENERIC_TYPE)
	{
	  ref_type = op2type ;
	}
  else if (op2type->get_node_type() == NODE_GENERIC_TYPE)
	{
	  // ref_type ne change pas mais on ne veut pas entrer dans le
	  // moteur d'inférence ci-apres. Donc on repete l'instruction
	  // pour montrer qu'on a bien vu ce cas
	  ref_type = op1type ;
	}
  else
	{
	  // Cas general
	  if (op1_src->is_equal_to(op2_src) == FALSE)
		{
		  semantic_error(this,
						 CAN_CONTINUE,
						 get_error_msg(E_EXPR_INCOMPATIBLE_TYPES),
						 op1type->make_type_name()->get_value(),
						 op2type->make_type_name()->get_value()) ;
		}

	  if (op1_dst->is_equal_to(op2_dst) == FALSE)
		{
		  semantic_error(this,
						 CAN_CONTINUE,
						 get_error_msg(E_EXPR_INCOMPATIBLE_TYPES),
						 op1type->make_type_name()->get_value(),
						 op2type->make_type_name()->get_value()) ;
		}
	}

  // Fabrication du type: il suffit dde cloner le type de ref_type
  T_type *res = ref_type->clone_type(this, get_betree(), get_ref_lexem()) ;

  set_B_type(res, get_ref_lexem()) ;
  res->unlink() ;
}

//
//	}{	Fin du type check des expressions
//
