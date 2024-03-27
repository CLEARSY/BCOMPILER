/******************************* CLEARSY **************************************
* Fichier : $Id: c_lstlnk.cpp,v 2.0 1999-06-09 12:38:19 sl Exp $
* (C) 2008 CLEARSY
*
* Description :		Compilateur B
*					Elements auxiliaires de chainage
*
* Compilations : 	-DDEBUG_LSTLNK pour avoir des traces
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
RCS_ID("$Id: c_lstlnk.cpp,v 1.20 1999-06-09 12:38:19 sl Exp $") ;

/* Includes systeme */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

/* Includes Composant Local */
#include "c_api.h"

// Constructeur avec chainage en queue
T_list_link::T_list_link(T_object *new_object,
								  T_list_link_type new_list_link_type,
								  T_item **adr_first,
								  T_item **adr_last,
								  T_item *new_father,
								  T_betree *new_betree,
								  T_lexem *new_ref_lexem)
  : T_item(NODE_LIST_LINK,
		   adr_first,
		   adr_last,
		   new_father,
		   new_betree,
		   new_ref_lexem)
{
#ifdef DEBUG_LSTLNK
  TRACE8("T_list_link(%x)::T_list_link(%x, %d, %x, %x, %x, %x, %x)",
		 this,
		 new_object,
		 new_list_link_type,
		 adr_first,
		 adr_last,
		 new_father,
		 new_betree,
		 new_ref_lexem) ;
#endif

  object = new_object ;
  list_link_type = new_list_link_type ;
  do_free = FALSE ;


}

// Constructeur avec chainage apres un element
T_list_link::T_list_link(T_object *new_object,
								  T_list_link_type new_list_link_type,
								  T_item *after_this_item,
								  T_item **adr_first,
								  T_item **adr_last,
								  T_item *new_father,
								  T_betree *new_betree,
								  T_lexem *new_ref_lexem)
  : T_item(NODE_LIST_LINK,
		   after_this_item,
		   adr_first,
		   adr_last,
		   new_father,
		   new_betree,
		   new_ref_lexem)
{
  TRACE8("T_list_link::T_list_link(%x, %d, %x, %x, %x, %x, %x)",
		 new_object,
		 new_list_link_type,
		 after_this_item,
		 adr_first,
		 adr_last,
		 new_father,
		 new_betree,
		 new_ref_lexem) ;

  object = new_object ;
  list_link_type = new_list_link_type ;
  do_free = FALSE ;


}

// Constructeur par copie
T_list_link::T_list_link(T_list_link *ref_list_link,
								  T_item **adr_first,
								  T_item **adr_last,
								  T_item *new_father,
								  T_betree *new_betree,
								  T_lexem *new_ref_lexem)
  : T_item(NODE_LIST_LINK, adr_first, adr_last, new_father, new_betree, new_ref_lexem)
{
#ifdef DEBUG_LSTLNK
  TRACE6("T_list_link::T_list_link(%x, %x, %x, %x, %x, %x)",
		 ref_list_link,
		 adr_first,
		 adr_last,
		 new_father,
		 new_betree,
		 new_ref_lexem) ;
#endif

  object = ref_list_link->object ;
  TRACE1("clone :: object = %s", ((T_lexem *)object)->get_lex_name()) ;
  list_link_type = ref_list_link->list_link_type ;
  do_free = ref_list_link->do_free ;


}

// Destructeur
T_list_link::~T_list_link(void)
{
}

//
//	}{	Fonction qui fabrique un item a partir d'une item parenthesee
//
T_item *T_list_link::new_paren_item(T_betree *betree,
											 T_lexem *opn,
											 T_lexem *clo)
{
  TRACE0("T_list_link::new_paren_item -> indirection") ;
  if (object->is_an_item() == TRUE)
	{
	  T_item *item = (T_item *)object ; // Cast valide cf le if

	  // Calcul du resultat
	  T_item *res = ((T_item *)object)->new_paren_item(betree, opn, clo) ;

	  // Pose d'un lien sur item car il va survivre seul
	  item->link() ;

	  // Calcul du resultat
	  return res ;
	}

  syntax_unexpected_error(this, FATAL_ERROR, get_catalog(C_EXPR_OR_PRED)) ;

  // Pour le warning
  return NULL ;
}

T_type *T_list_link::get_B_type(void)
{
#ifdef DUMP_TYPES
  TRACE2("T_list_link(%x:%s)::get_B_type() on renvoie NULL !",
		 this, get_class_name()) ;
#endif
  ASSERT(is_an_item() == TRUE) ; // A FAIRE
  return ((T_item *)object)->get_B_type() ;
} ;

void T_list_link::set_B_type(T_type *new_B_type,
									  T_lexem *typing_location)
{
  TRACE3("T_list_link(%x:%s)::set_B_type(%x) on suit le lien",
		 this, get_class_name(), new_B_type) ;
  ASSERT(is_an_item() == TRUE) ; // A FAIRE
  ((T_item *)object)->set_B_type(new_B_type, typing_location) ;
}

int T_list_link::is_a_set(void)
{
  TRACE2("T_list_link(%x::%s)::is_a_set ?", this, get_class_name()) ;

  ASSERT(is_an_item() == TRUE) ; // A FAIRE
  return ((T_item *)object)->is_a_set() ; ;
}

int T_list_link::is_a_relation(void)
{
  TRACE2("T_list_link(%x::%s)::is_a_relation ?", this, get_class_name()) ;

  ASSERT(is_an_item() == TRUE) ; // A FAIRE
  return ((T_item *)object)->is_a_relation() ; ;
}

int T_list_link::is_a_seq(void)
{
  TRACE2("T_list_link(%x::%s)::is_a_seq ?", this, get_class_name()) ;

  ASSERT(is_an_item() == TRUE) ; // A FAIRE
  return ((T_item *)object)->is_a_seq() ; ;
}

int T_list_link::is_a_non_empty_seq(void)
{
  TRACE2("T_list_link(%x::%s)::is_a_non_empty_seq ?", this, get_class_name()) ;

  ASSERT(is_an_item() == TRUE) ; // A FAIRE
  return ((T_item *)object)->is_a_non_empty_seq() ; ;
}

int T_list_link::is_an_integer_set(void)
{
  TRACE2("T_list_link(%x::%s)::is_an_integer_set ?", this, get_class_name()) ;

  ASSERT(is_an_item() == TRUE) ; // A FAIRE
  return ((T_item *)object)->is_an_integer_set() ; ;
}

int T_list_link::is_an_ident(void)
{
  TRACE2("T_list_link(%x::%s)::is_an_ident ?", this, get_class_name()) ;

  ASSERT(is_an_item() == TRUE) ; // A FAIRE
  return ((T_item *)object)->is_an_ident() ;
}

// Est-ce un ensemble ?
void T_list_link::make_type(void)
{
#ifdef DEBUG_LSTLNK
  TRACE2("T_list_link(%x::%s)::make_type : on suit le lien", this, get_class_name()) ;
#endif

  ASSERT(is_an_item() == TRUE) ; // A FAIRE
  ((T_item *)object)->make_type() ;
}

//GP 07/01/99
// Pour pouvoir applique la fonction check_type à une T_list_link
void T_list_link::check_type(void)
{
  TRACE2("T_list_link(%x::%s)::check_type() ?", this, get_class_name()) ;
  ASSERT(is_an_item() == TRUE) ; // A FAIRE
  ((T_item *)object)->check_type() ;
}

int T_list_link::is_a_constant(int ask_def)
{
  TRACE2("T_list_link(%x::%s)::is_a_constant ?", this, get_class_name()) ;

  ASSERT(is_an_item() == TRUE) ; // A FAIRE
  return ((T_item *)object)->is_a_constant(ask_def) ; ;
}
int T_list_link::is_a_variable(int ask_def)
{
  TRACE2("T_list_link(%x::%s)::is_a_variable ?", this, get_class_name()) ;

  ASSERT(is_an_item() == TRUE) ; // A FAIRE
  return ((T_item *)object)->is_a_variable(ask_def) ;
}

// Renvoie l'identificateur "contenu" dans this
// i.e. this pour la classe T_ident et sous-classes
//      this->object pour T_list_link qui contient un ident, ...
// prerequis : is_an_ident == TRUE
T_ident *T_list_link::make_ident(void)
{
  TRACE2("T_list_link(%x::%s)::make_ident ?", this, get_class_name()) ;

  ASSERT(is_an_ident() == TRUE) ; // A FAIRE
  return ((T_item *)object)->make_ident() ; ;
}

// Renvoie l'expression "contenu" dans this
// i.e. this pour la classe T_expr et sous-classes
//      this->object pour T_list_link qui contient une expr, ...
// prerequis : is_an_expr == TRUE
T_expr *T_list_link::make_expr(void)
{

  TRACE2("T_list_link(%x::%s)::make_expr ?", this, get_class_name()) ;

  ASSERT(is_an_expr() == TRUE) ; // A FAIRE
  return ((T_item *)object)->make_expr() ; ;
}

//
//	}{	Correction de l'arbre
//
T_item *T_list_link::check_tree(T_item **ref_this)
{
#ifdef DEBUG_CHECK
  TRACE1("T_list_link(%x)::check_tree", this) ;
#endif

  if ( (object != NULL) && (object->is_an_item() == TRUE) )
	{
	  ((T_item *)object)->check_tree((T_item **)&object) ;
	}

  return this ;
  assert(ref_this) ; // pour le warning
}

// Methode qui suit une indirection (T_list_link) ssi elle est
// d'un type donne, et rend l'objet pointe, ou this sinon
// Attention il faut aussi accrocher les commentaires
T_object *T_list_link::follow_indirection(T_list_link_type ref_type)
{
  TRACE4("T_list_link(%x)::follow_indirection(%d ref %d) : ras",
		 this, list_link_type, ref_type,
		 (list_link_type == ref_type) ? object : this) ;
  T_object *res = NULL ;

  if (list_link_type == ref_type)
	{
	  TRACE2("ok bon type object %x:%s", object, object->get_class_name()) ;
	  res = object ;

	  // Accrocher les eventuels commentaires
	  if (res->is_an_item() == TRUE)
		{
		  ((T_item *)res)->fetch_comments(this) ;
		}
	}
  else
	{
	  res = this ;
	}

  return res ;
}


//
//	}{	Fin du fichier
//
