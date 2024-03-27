/******************************* CLEARSY **************************************
* Fichier : $Id: c_record.cpp,v 2.0 2000-07-11 16:24:30 fl Exp $
* (C) 2008 CLEARSY
*
* Description :		Compilateur B
*					Interfaces des records
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
RCS_ID("$Id: c_record.cpp,v 1.21 2000-07-11 16:24:30 fl Exp $") ;

/* Includes systeme */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

/* Includes Composant Local */

#include "c_api.h"

//
//	}{	Classe T_record
//
T_record::T_record(T_item *args,
							T_item *new_father,
							T_betree *new_betree,
							T_lexem *new_ref_lexem)
  : T_expr(NODE_RECORD, NULL, NULL, new_father, new_betree, new_ref_lexem)
{
  TRACE3("T_record::T_record(%x, %x, %x)", new_father, new_betree, new_ref_lexem) ;

  TRACE2("args = %x:%s", args, args->get_class_name()) ;

  TRACE2("args = %x:%s", args, args->get_class_name()) ;
  if (args->get_node_type() != NODE_EXPR_WITH_PARENTHESIS)
	{
	  syntax_error(args->get_ref_lexem(),
				   CAN_CONTINUE,
				   get_error_msg(E_IN_EXTENSIVE_REC_PAREN_EXPR_EXPECTED),
				   make_class_name(args)) ;
	  return ;
	}

  // Suppression des '()' inutiles
  // La methode strip_parenthesis fait le menage toute seule
  // (T_expr_with_parenthesis)
  T_expr *expr = args->make_expr()->strip_parenthesis() ;

  first_record_item = last_record_item = NULL ;

  // Recuperation des record_item
  // La methode fait le menage toute seule (T_binary_op avec op = BOP_COMA)
  expr->extract_record_items(this,
							 TRUE,
							 (T_item **)&first_record_item,
							 (T_item **)&last_record_item) ;
}

T_record::~T_record(void)
{
}

void T_record::add_record_item(T_record_item *item)
{
  item->tail_insert((T_item **)&first_record_item,
					(T_item **)&last_record_item) ;
  item->set_father(this);
}

//
//	}{	Phase de correction
// ref_this contient l'adresse du champ du pere a mettre a jour
T_item *T_record::check_tree(T_item **ref_this)
{
    T_record_item *cur = first_record_item ;

    while (cur != NULL)
      {
        T_record_item *next = (T_record_item *) cur->get_next();
        cur->check_tree((T_item **)&cur);
        cur = next;
    }
    return this;
}

//
//	}{	Classe T_struct
//
T_struct::T_struct(T_item *args,
							T_item *new_father,
							T_betree *new_betree,
							T_lexem *new_ref_lexem)
  : T_expr(NODE_STRUCT, NULL, NULL, new_father, new_betree, new_ref_lexem)
{
  TRACE3("T_struct::T_struct(%x, %x, %x)", new_father, new_betree, new_ref_lexem) ;

  TRACE2("args = %x:%s", args, args->get_class_name()) ;

  if (args->get_node_type() != NODE_EXPR_WITH_PARENTHESIS)
	{
	  char tmp[128] ;
	  sprintf(tmp, "pas expr_with_paren mais %s", args->get_class_name()) ;
	  syntax_error(args->get_ref_lexem(),
				   CAN_CONTINUE,
				   tmp) ;
	  return ;
	}

  T_expr *expr = args->make_expr()->strip_parenthesis() ;

  first_record_item = last_record_item = NULL ;

  // Recuperation des record_item
  expr->extract_record_items(this,
							 FALSE,
							 (T_item **)&first_record_item,
							 (T_item **)&last_record_item) ;

}

T_struct::~T_struct(void)
{
}

//
//	}{	Classe T_record_item
//
T_record_item::T_record_item(T_item *new_label,
									  T_item *new_value,
									  T_item **adr_first,
									  T_item **adr_last,
									  T_item *father,
									  T_betree *betree,
									  T_lexem *new_ref_lexem)
  : T_expr(NODE_RECORD_ITEM, adr_first, adr_last, father, betree, new_ref_lexem)
{
  TRACE7("T_record_item::T_record_item(%x, %x, %x, %x, %x, %x, %x)",
		 new_label, new_value, adr_first, adr_last, father, betree, new_ref_lexem) ;

  if (new_label == NULL)
	{
	  TRACE0("pas de label") ;
	  label = NULL ;
	}
  else
	{
	  TRACE2("new_label(%x:%s)", new_label, new_label->get_class_name()) ;
	  if (new_label->is_an_ident() == FALSE)
		{
		  syntax_ident_expected_error(new_label, CAN_CONTINUE) ;
		}
	  else
		{
		  // Cast justifie par le test
		  label = (T_ident *)new_label ;
		  label->set_father(this) ;
		  label->set_ident_type(ITYPE_RECORD_LABEL) ;
		}
	}

  TRACE2("new_value(%x:%s)", new_value, new_value->get_class_name()) ;
  if (new_value->is_an_expr() == FALSE)
	{
	  syntax_expr_expected_error(new_value, CAN_CONTINUE) ;
	}
  else
	{
	  // Cast justifie par le test ci-dessus
	  value = new_value->make_expr() ;
	  value->set_father(this) ;
	  value->post_check() ;
	}

}

T_record_item::~T_record_item(void)
{
}

T_access_authorisation T_record_item::get_auth_request(T_item *ref)
{
  if (ref == this->get_label())
	{
	  return AUTH_READ ;
	}
  else
	// si l'appel vient du fils gauche : demande au père ce qu'il veut faire
	{
	  return get_father()->get_auth_request(this) ;
	}
}

//
//	}{	Phase de correction
// ref_this contient l'adresse du champ du pere a mettre a jour
T_item *T_record_item::check_tree(T_item **ref_this)
{
    value->check_tree((T_item **)&value);
    return this;
}

//
//	}{	Classe T_record_access
//
T_record_access::T_record_access(T_item *new_record,
										  T_item *new_label,
										  T_item **adr_first,
										  T_item **adr_last,
										  T_item *father,
										  T_betree *betree,
										  T_lexem *new_ref_lexem)
  : T_expr(NODE_RECORD_ACCESS, adr_first, adr_last, father, betree, new_ref_lexem)
{
  TRACE7("T_record_access::T_record_access(%x, %x, %x, %x, %x, %x, %x)",
		 new_record, new_label, adr_first, adr_last, father, betree, new_ref_lexem);

  TRACE2("new_record(%x:%s)", new_record, new_record->get_class_name()) ;
  if (new_record->is_an_expr() == FALSE)
	{
	  syntax_expr_expected_error(new_record, CAN_CONTINUE) ;
	}
  else
	{
	  // Cast justifie par le test ci-dessus
	  record = new_record->make_expr() ;
	  record->set_father(this) ;
	  record->post_check() ;
	}

  TRACE2("new_label(%x:%s)", new_label, new_label->get_class_name()) ;
  if (new_label->is_an_expr() == FALSE)
	{
	  syntax_ident_expected_error(new_label, CAN_CONTINUE) ;
	}
  else
	{
	  // Cast justifie par le test ci-dessus
	  label = (T_ident *)new_label ;
	  label->set_father(this) ;
	  label->set_ident_type(ITYPE_RECORD_LABEL) ;
	}

}

T_record_access::~T_record_access(void)
{
}

//
//	}{	Correction de l'arbre
//

void T_record_item::set_label(T_ident *new_label)
{
  label = new_label;

  // Reprise du chaînage
  label->set_father(this);
  label->set_prev(NULL);
  label->set_next(NULL);
}


//
//	}{	Methodes d'extraction des record_items
//
void T_item::extract_record_items(T_item *new_father,
										   int allow_empty_label,
										   T_item **adr_first,
										   T_item **adr_last)
{
  TRACE3("T_item::extract_record_items(%x, %x, %x)",
		 this, adr_first, adr_last) ;

  syntax_error(get_ref_lexem(),
			   CAN_CONTINUE,
			   get_error_msg(E_RECORD_LABEL_EXPECTED),
			   make_class_name(this)) ;
  // Pour le warning
  new_father = new_father ;
  allow_empty_label = allow_empty_label ;
}


void T_record_item::extract_record_items(T_item *new_father,
												  int allow_empty_label,
												  T_item **adr_first,
												  T_item **adr_last)
{
  TRACE3("T_item::extract_record_items(%x, %x, %x)",
		 this, adr_first, adr_last) ;

  // C'est bon, on se chaine tout seul
  set_father(new_father) ;
  tail_insert(adr_first, adr_last) ;

  return ;
  // Pour le warning
  allow_empty_label = allow_empty_label ;
}

void T_expr::extract_record_items(T_item *new_father,
										   int allow_empty_label,
										   T_item **adr_first,
										   T_item **adr_last)
{
  TRACE3("T_expr::extract_record_items(%x, %x, %x)",
		 this, adr_first, adr_last) ;

  // Si allow_empty_label = TRUE :
  // Il faut fabriquer le record_item correspondant :
  // pas de label, value = this
  if (allow_empty_label == TRUE)
	{
	  (void)new T_record_item(NULL,
							  this,
							  adr_first,
							  adr_last,
							  new_father,
							  get_betree(),
							  get_ref_lexem()) ;
	}
  else
	{
	  syntax_error(get_ref_lexem(),
				   CAN_CONTINUE,
				   get_error_msg(E_RECORD_LABEL_EXPECTED),
				   make_class_name(this)) ;
	}
}

//
//	}{	Fonction auxiliaire qui extrait une liste de parametres
//
void T_binary_op::extract_record_items(T_item *father,
												int allow_empty_label,
												T_item **adr_first,
												T_item **adr_last)
{
  TRACE3("T_binary_op::extract_record_items(%x, %x, %x)",
		 this, adr_first, adr_last) ;

  if (oper == BOP_COMMA)
	{
	  // On extrait les record_items des fils gauches et droits
	  TRACE0("extract_record_items fils gauche") ;
	  op1->extract_record_items(father, allow_empty_label, adr_first, adr_last) ;
	  op1 = NULL ;

	  TRACE0("extract_record_items fils droit") ;
	  op2->extract_record_items(father, allow_empty_label, adr_first, adr_last) ;
	  op2 = NULL ;

	}
  else
	{
	  // On appelle la methode generique
	  T_expr::extract_record_items(father, allow_empty_label, adr_first, adr_last) ;
	}
}

//
// Fonctions "joker" (type record avec un label non defini)
//
int T_base_type::has_jokers(void)
{
  TRACE1("T_base_type(%x)::has_jokers()->FALSE", this) ;
  return FALSE ;
}

int T_product_type::has_jokers(void)
{
  TRACE1("T_product_type(%x)::has_jokers() ->> ", this) ;
  if (type1->has_jokers() == TRUE)
	{
	  return TRUE ;
	}

  return type2->has_jokers() ;
}

int T_setof_type::has_jokers(void)
{
  TRACE1("T_setof_type(%x)::has_jokers() ->>", this) ;
  return base_type->has_jokers() ;
}

int T_abstract_type::has_jokers(void)
{
  TRACE1("T_abstract_type(%x)::has_jokers()->FALSE", this) ;
  return FALSE ;
}

int T_enumerated_type::has_jokers(void)
{
  TRACE1("T_enumerated_type(%x)::has_jokers()->FALSE", this) ;
  return FALSE ;
}

int T_predefined_type::has_jokers(void)
{
  TRACE1("T_predefined_type(%x)::has_jokers()->FALSE", this) ;
  return FALSE ;
}

int T_generic_type::has_jokers(void)
{
  TRACE1("T_generic_type(%x)::has_jokers()->FALSE", this) ;
  return FALSE ;
}

int T_record_type::has_jokers(void)
{
  TRACE2("T_record_type(%x)::has_jokers() ??? first_label %x",
		 this,
		 first_label) ;
  ENTER_TRACE ;
  T_label_type *cur = first_label ;

  while (cur != NULL)
	{
	  if (cur->has_jokers() == TRUE)
		{
		  EXIT_TRACE ;
		  TRACE1("T_record_type(%x)::has_jokers() ->> TRUE", this) ;
		  return TRUE ;
		}

	  cur = (T_label_type *)cur->get_next() ;
	}

  EXIT_TRACE ;
  TRACE1("T_record_type(%x)::has_jokers() ->> FALSE", this) ;
  return FALSE ;
}

int T_label_type::has_jokers(void)
{
  TRACE1("T_label_type(%x)::has_jokers", this) ;
  if (name == NULL)
	{
	  TRACE0("name = NULL -> TRUE") ;
	  return TRUE ;
	}

  int res = type->has_jokers() ;
  TRACE1("type -> %s", (res == TRUE) ? "TRUE" : "FALSE") ;
  return res ;
}

//
//	}{	Fin du fichier
//
