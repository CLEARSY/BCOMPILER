/******************************* CLEARSY **************************************
* Fichier : $Id: c_binexp.cpp,v 2.0 2000-11-03 12:52:14 lv Exp $
* (C) 2008 CLEARSY
*
* Description :		Compilateur B
*					Expressions binaires
*
* Compilation :
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
RCS_ID("$Id: c_binexp.cpp,v 1.27 2000-11-03 12:52:14 lv Exp $") ;

/* Includes systeme */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

/* Includes Composant Local */
#include "c_api.h"

//
//	}{	Constructeur de la classe T_binary_op
//
T_binary_op::T_binary_op(T_item *new_op1,
								  T_item *new_op2,
								  T_binary_operator new_operator,
								  T_item **adr_first,
								  T_item **adr_last,
								  T_item *father,
								  T_betree *betree,
								  T_lexem *ref_lexem)
  : T_expr(NODE_BINARY_OP, adr_first, adr_last, father, betree, ref_lexem)
{
  TRACE9("T_binary_op(%x)::T_binary_op(%x, %x, %d, %x, %x, %x, %x, %x)",
		 this,
		 new_op1, new_op2, new_operator,
		 adr_first, adr_last, father, betree, ref_lexem) ;

  // Mise a jour des champs
  op1 = new_op1 ;
  op2 = new_op2 ;
  oper = new_operator ;

  // On met a jour les liens "father"
  op1->set_father(this) ;
  op2->set_father(this) ;

  // Calcul de la liste des operandes : lors du type check
  first_operand = last_operand = NULL ;


}

int T_binary_op::is_an_expr(void)
{
	//  TRACE2("T_binary_expr(%x oper %d)::is_an_expr", this, oper) ;
  switch (oper)
	{
	case BOP_PIPE :
	  {
		// Constructions temporaires qui ne sont pas des expressions
		return FALSE ;
	  }

	default :
	  {
		return TRUE ;
	  }
	}
}


void T_binary_op::fix_operand_list(void)
{
  // Champs first_operand, last_operand
  first_operand = last_operand = NULL ;
  fetch_operands() ;

  // Calcul du nombre d'operandes
  nb_operands = 0 ;
  T_item *cur = first_operand ;
  while (cur != NULL)
	{
	  nb_operands ++ ;
	  cur = cur->get_next() ;
	}
}

// Ajout des operandes propres et celles des fils
void T_binary_op::fetch_operands(void)
{
#ifdef FULL_TRACE
  TRACE1("T_binary_op(%x)::fetch_operands", this) ;
#endif

  T_binary_op *bop1 = (T_binary_op *)op1 ; // cast justifie quand necessaire apres
  T_binary_op *bop2 = (T_binary_op *)op2 ; // cast justifie quand necessaire apres

  if ( 	(op1->get_node_type() == NODE_BINARY_OP)
		&& (bop1->oper == oper) )
	{
	  // On recupere les operandes de op1
#ifdef FULL_TRACE
	  TRACE0("DEBUT recuperation des operandes de op1") ;
#endif
	  ENTER_TRACE ;
	  T_list_link *cur = bop1->first_operand ;
	  while (cur != NULL)
		{
		  (void)new T_list_link(cur->get_object(),
								LINK_BINOP_OPER_LIST,
								(T_item **)&first_operand,
								(T_item **)&last_operand,
								this,
								get_betree(),
								((T_item *)cur->get_object())->get_ref_lexem()) ;
		  cur = (T_list_link *)cur->get_next() ;
		}
	  EXIT_TRACE ;
#ifdef FULL_TRACE
	  TRACE0("FIN   recuperation des operandes de op1") ;
#endif
	}
  else
	{
	  // On ajoute op1 dans la liste des operandes
#ifdef FULL_TRACE
	  TRACE0("AJOUT DE OP1") ;
#endif
	  (void)new T_list_link(op1,
							LINK_BINOP_OPER_LIST,
							(T_item **)&first_operand,
							(T_item **)&last_operand,
							this,
							get_betree(),
							op1->get_ref_lexem()) ;
	}

  if ( 	(op2->get_node_type() == NODE_BINARY_OP)
		&& (bop2->oper == oper) )
	{
	  // On recupere les operandes de op2
#ifdef FULL_TRACE
	  TRACE0("DEBUT recuperation des operandes de op2") ;
#endif
	  ENTER_TRACE ;
	  T_list_link *cur = bop2->first_operand ;
	  while (cur != NULL)
		{
		  (void)new T_list_link(cur->get_object(),
								LINK_BINOP_OPER_LIST,
								(T_item **)&first_operand,
								(T_item **)&last_operand,
								this,
								get_betree(),
								((T_item *)cur->get_object())->get_ref_lexem()) ;
		  cur = (T_list_link *)cur->get_next() ;
		}
	  EXIT_TRACE ;
#ifdef FULL_TRACE
	  TRACE0("FIN   recuperation des operandes de op2") ;
#endif
	}
  else
	{
	  // On ajoute op2 dans la liste des operandes
#ifdef FULL_TRACE
	  TRACE0("AJOUT DE OP2") ;
#endif
	  (void)new T_list_link(op2,
							LINK_BINOP_OPER_LIST,
							(T_item **)&first_operand,
							(T_item **)&last_operand,
							this,
							get_betree(),
							op2->get_ref_lexem()) ;
	}

}

//
//	Destructeur
//
T_binary_op::~T_binary_op(void)
{
}

//
//	}{	Fonction qui fabrique un item a partir d'une item parenthesee
//
T_item *T_binary_op::new_paren_item(T_betree *betree,
											 T_lexem *opn,
											 T_lexem *clo)
{
  TRACE0("T_binary_op::new_paren_item") ;

  // On ne traite ici que les cas particuliers
  // on fait appel a T_expr::new_paren_item pour le cas general
  if (opn->get_lex_type() == L_OPEN_BRACES)
	{
	  if (oper == BOP_PIPE)
		{
		  TRACE0("on construit un comprehensive_set") ;
		  return new T_comprehensive_set(this, NULL, NULL, NULL, betree, opn) ;
		}
	  else
		{
		  TRACE0("on construit un extensive_set") ;
		  return new T_extensive_set(this, NULL, NULL, NULL, betree, opn) ;
		}
	}

  // Par defaut : Appel de la superclasse
  ENTER_TRACE ;
  return T_expr::new_paren_item(betree, opn, clo)  ;
  EXIT_TRACE ;
}


//
//	}{	Correction de l'arbre
//
T_item *T_binary_op::check_tree(T_item **ref_this)
{
#ifdef DEBUG_CHECK
  TRACE1(">> T_binary_op(%x)::check_tree", this) ;
  ENTER_TRACE ;
#endif

  if (op1 != NULL)
	{
	  op1->check_tree((T_item **)&op1) ;
	}

  if (op2 != NULL)
	{
	  op2->check_tree((T_item **)&op2) ;
	}

#ifdef DEBUG_CHECK
  TRACE1("<< T_binary_op(%x)::check_tree", this) ;
  EXIT_TRACE ;
#endif

  return this ;
  assert(ref_this) ; // pour le warning
}

//
//	}{	Fonction auxiliaire qui extrait une liste de parametres
//
void T_binary_op::extract_params(int separator,
										  T_item *father,
										  T_item **adr_first,
										  T_item **adr_last)
{
#ifdef TRACE_EXTRACT
  TRACE4("T_binary_op::extract_params(%x, %d, %x, %x)",
		 this, separator, adr_first, adr_last) ;
  ENTER_TRACE ;
#endif

  if (oper == separator)
	{
	  // On extrait les params des fils gauches et droits
#ifdef TRACE_EXTRACT
	  TRACE0("extract_params fils gauche") ;
#endif
	  op1->extract_params(separator, father, adr_first, adr_last) ;
#ifdef TRACE_EXTRACT
	  TRACE0("extract_params fils droit") ;
#endif
	  op2->extract_params(separator, father, adr_first, adr_last) ;

#ifdef FIX_MAPLETS
	  if (oper == BOP_COMMA)
		{
		  // On change l'oper en n'importe quoi sauf la virgule, pour ne
		  // pas qu'elle soit remplacee par un MAPLET dans la passe de
		  // reparation
		  oper = BOP_PIPE ;
		}
#endif // FIX_MAPLETS

	  // On pose un lien sur les fils car on n'en a plus la responsabilite,
	  // c'est la personne qui a demande l'extraction qui a cette tache
	  op1->link() ;
	  op2->link() ;
	}
  else
	{
	  T_item::extract_params(separator, father, adr_first, adr_last) ;
	}

#ifdef TRACE_EXTRACT
  EXIT_TRACE ;
  TRACE4("END :: T_binary_op::extract_params(%x, %x, %x)",
		 this, separator, adr_first, adr_last) ;
#endif
}

//
//	}{	Fin du fichier
//
