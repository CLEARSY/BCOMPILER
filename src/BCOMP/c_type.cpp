/***************************** CLEARSY **************************************
* Fichier : $Id: c_type.cpp,v 2.0 2000-11-06 17:53:51 lv Exp $
* (C) 2008 CLEARSY
*
* Compilations :	-DDEBUG_TYPE pour les traces generales
*
* Description :	Modelisation des types B
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
****************************************************************************/
#include "c_port.h"
RCS_ID("$Id: c_type.cpp,v 1.69 2000-11-06 17:53:51 lv Exp $") ;

/* Includes systeme */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

/* Includes Composant Local */
#include "c_api.h"

//
//	}{	Classe T_type
//
// Constructeur
T_type::T_type(T_node_type new_node_type,
						T_item *new_father,
						T_betree *new_betree,
						T_lexem *ref_lexem)
  : T_item(new_node_type, NULL, NULL, new_father, new_betree, ref_lexem)
{
  //  static int cpt_si = 0 ;
  //  TRACE2("T_type(%x)::T_type (%d)", this, ++cpt_si) ;
	//  TRACE1("T_type(%x)::T_type", this) ;
  //  assert(cpt_si != 66) ;
  //  original_type = NULL ;
  type_name = NULL ;
  before_valuation_type = NULL ;
}

// Destructeur
T_type::~T_type(void)
{
}

int T_type::is_a_type(void)
{
  return TRUE ;
}

// Methodes d'ecriture
void T_type::set_B0_type(T_B0_type *new_B0_type)
{
  TRACE4("T_type(%x:%s)::set_B0_type(%x:%s)",
		 this,
		 make_type_name()->get_value(),
		 new_B0_type,
		 (new_B0_type == NULL) ? "(null)" : new_B0_type->get_class_name()) ;
  B0_type = new_B0_type ;
  if (B0_type != NULL)
	{
	  TRACE2("%x pose un lien sur %x", this, B0_type) ;
	  B0_type->link() ;
	}
}

// Calcul du nombre de types
int T_type::get_nb_types(void)
{
#ifdef DEBUG_TYPE
  TRACE2("T_type(%x:%s)::get_nb_types() -> 1", this, make_type_name()->get_value());
#endif // DEBUG_TYPE

  return 1 ;
}

// Surcharge get_next/get_prev
T_type *T_type::get_next(void)
{
  T_type *next_type = (T_type *)(T_item::get_next()) ;
  return (next_type == NULL) ? (T_type *)NULL : next_type->get_real_type() ;
}
T_type *T_type::get_prev(void)
{
  T_type *prev_type = (T_type *)(T_item::get_prev()) ;
  return (prev_type == NULL) ? (T_type *)NULL : prev_type->get_real_type() ;
}

T_type *T_type::get_next_spec_type(void)
{
  ASSERT(this != NULL) ;
  TRACE2("T_type(%x:%s)::get_next_spec_type", this, get_class_name()) ;
  TRACE2("T_type(%x:%s)::get_next_spec_type", this, make_type_name()->get_value()) ;
  TRACE2("T_type(%x) father %x", this, get_father()) ;
  TRACE4("T_type(%x:%s)::get_next_spec_type(father %x:%s)",
		 this,
		 make_type_name()->get_value(),
		 get_father(),
		 (get_father() == NULL) ? "null" : get_father()->get_class_name()) ;

  if (    (get_father() == NULL)
	   || (get_father()->get_node_type() != NODE_PRODUCT_TYPE) )
	{
	  TRACE0("on renvoie NULL (arrive a la racine ?)") ;
	  return NULL ;
	}

  ASSERT(get_father() != NULL) ;
  ASSERT(get_father()->get_node_type() == NODE_PRODUCT_TYPE) ;

  T_product_type *prod = (T_product_type *)get_father() ;
  ASSERT( (prod->get_spec_type1() == this) || (prod->get_spec_type2() == this) ) ;

  if (prod->get_spec_type1() == this)
	{
	  // Renvoyer le fils droit
	  TRACE0("renvoyer fils droit") ;
	  return (prod->get_spec_type2()->get_node_type() == NODE_PRODUCT_TYPE)
		? ((T_product_type *)prod->get_spec_type2())->get_types()
		: prod->get_spec_type2() ;
	}

  TRACE0("demander au pere") ;
  return ((T_type *)get_father())->get_next_spec_type() ;
}

T_type *T_type::get_next_type(void)
{
  TRACE4("T_type(%x:%s)::get_next_type(father %x:%s)",
		 this,
		 make_type_name()->get_value(),
		 get_father(),
		 (get_father() == NULL) ? "null" : get_father()->get_class_name()) ;

  if (    (get_father() == NULL)
	   || (get_father()->get_node_type() != NODE_PRODUCT_TYPE) )
	{
	  TRACE0("on renvoie NULL (arrive a la racine ?)") ;
	  return NULL ;
	}

  ASSERT(get_father() != NULL) ;
  ASSERT(get_father()->get_node_type() == NODE_PRODUCT_TYPE) ;

  T_product_type *prod = (T_product_type *)get_father() ;
  ASSERT( (prod->get_type1() == this) || (prod->get_type2() == this) ) ;

  if (prod->get_type1() == this)
	{
	  // Renvoyer le fils droit
	  TRACE0("renvoyer fils droit") ;
	  return (prod->get_type2()->get_node_type() == NODE_PRODUCT_TYPE)
		? ((T_product_type *)prod->get_type2())->get_types()
		: prod->get_type2() ;
	}

  TRACE0("demander au pere") ;
  return ((T_type *)get_father())->get_next_type() ;
}

// Remise a zero des bornes
void T_type::reset_bounds(void)
{
#ifdef DEBUG_TYPE
  TRACE2("T_type_(%x:%s)::reset_bounds -> rien a faire", this, get_class_name()) ;
#endif

}

// Pour savoir si un type est un type entier
int T_type::is_an_integer(void)
{
#ifdef DEBUG_TYPE
  TRACE2("T_type(%x:%s)::is_an_integer -> FALSE", this, get_class_name()) ;
#endif

  return FALSE ;
}

// Pour savoir si un type est un type réel
int T_type::is_a_real(void)
{
#ifdef DEBUG_TYPE
  TRACE2("T_type(%x:%s)::is_a_real -> FALSE", this, get_class_name()) ;
#endif

  return FALSE ;
}

// Pour savoir si un type est un type float
int T_type::is_a_float(void)
{
#ifdef DEBUG_TYPE
  TRACE2("T_type(%x:%s)::is_a_float -> FALSE", this, get_class_name()) ;
#endif

  return FALSE ;
}

// Est-ce un type de base ?
int T_type::is_a_base_type(void)
{
  return FALSE ;
}

// Est-ce le type P(P(Z*Z)*T) (T quelconque)
int T_type::is_a_tree_type(void)
{
  return FALSE ;
}

// Est-ce le type P(Z*Z)*T (T quelconque)
int T_type::is_a_tree_node_type(void)
{
  return FALSE ;
}

// Obtention des types apres valuation
T_type *T_type::get_real_type(void)
{
#ifdef DEBUG_TYPE
  TRACE3("T_type(%x:%s)::get_real_type() -> %x", this, get_class_name(), this) ;
#endif // DEBUG_TYPE
  return this ;
}

// Pour savoir si un type est un ensemble index large
// Dans le cas general : non
int T_type::is_an_extended_index_set(void)
{
#ifdef DEBUG_TYPE
  TRACE2("T_type(%x)::is_an_extended_index_set() cas general (%s) : FALSE",
		 this,
		 get_class_name()) ;
#endif

  return FALSE ;
}

//
// Classe T_constructor_type
//
// Constructeur
T_constructor_type::T_constructor_type(T_node_type new_node_type,
												T_item *new_father,
												T_betree *new_betree,
												T_lexem *ref_lexem)
  : T_type(new_node_type, new_father, new_betree, ref_lexem)
{
}

//
// Classe T_base_type
//
// Constructeur
T_base_type::T_base_type(T_node_type new_node_type,
								  T_ident *new_name,
								  T_expr *new_lower_bound,
								  T_expr *new_upper_bound,
								  T_item *new_father,
								  T_betree *new_betree,
								  T_lexem *ref_lexem)
  : T_type(new_node_type, new_father, new_betree, ref_lexem)
{
#ifdef DEBUG_TYPE
  TRACE5("T_base_type(%x, new_father %x)::T_btype(\"%s\", lbnd %x,ubnd %x)",
		 this,
		 new_father,
		 new_name->get_name()->get_value(),
		 lower_bound,
		 upper_bound) ;
#endif


  name = new_name ;
  lower_bound = new_lower_bound ;
  if (lower_bound != NULL)
	{
	  lower_bound->link() ;
	  if (lower_bound->get_father() == NULL)
		{
		  // On ne touche pas au pere s'il est deja positionne
		  lower_bound->set_father(this) ;
		}
	}
  upper_bound = new_upper_bound ;
  if (upper_bound != NULL)
	{
	  upper_bound->link() ;
	  if (lower_bound->get_father() == NULL)
		{
		  // On ne touche pas au pere s'il est deja positionne
		  upper_bound->set_father(this) ;
		}
	}
}

// Destructeur
T_base_type::~T_base_type(void)
{
}

// Remise a zero des bornes
void T_base_type::reset_bounds(void)
{
#ifdef DEBUG_TYPE
  TRACE1("T_base_type(%x)::reset_bounds -> RESET BORNES", this) ;
#endif

  lower_bound = NULL ;
  upper_bound = NULL ;
}

// Mise a jour des bornes, methodes generae
void T_base_type::set_bounds(T_expr *new_lower_bound,
									  T_expr *new_upper_bound)
{
  TRACE5("T_base_type(%x)::set_bounds(%x, %x) was (%x,%x)",
		 this,
		 new_lower_bound,
		 new_upper_bound,
		 lower_bound,
		 upper_bound) ;

  lower_bound = new_lower_bound ;
  upper_bound = new_upper_bound ;
}

// Est-ce un type de base ?
int T_base_type::is_a_base_type(void)
{
  return TRUE ;
}


// Fonction qui dit si les bornes sont positionnes a une valeur literale
int T_base_type::are_bounds_set(void)
{
if ( (lower_bound == NULL) || (lower_bound->get_node_type() == NODE_BOUND) )
  {
	ASSERT( (upper_bound == NULL) || (upper_bound->get_node_type() == NODE_BOUND) );
#ifdef DEBUG_TYPE
	TRACE1("T_base_type(%x)::are_bounds_set() -> FALSE", this) ;
#endif

	return FALSE ;
  }

#ifdef DEBUG_TYPE
 TRACE1("T_base_type(%x)::are_bounds_set() -> TRUE", this) ;
#endif

 return TRUE ;
}


//
//	}{	Classe T_product_type
//
// Constructeur
T_product_type::T_product_type(T_type *new_type1,
										T_type *new_type2,
										T_item *new_father,
										T_betree *new_betree,
										T_lexem *ref_lexem)
  : T_constructor_type(NODE_PRODUCT_TYPE,
					   new_father,
					   new_betree,
					   ref_lexem)
{
#ifdef DEBUG_TYPE
  TRACE5("T_product_type(%x)::T_product_type(%x, %x, %x, %x, %x)",
		 this, new_type1, new_type2, new_father, new_betree) ;
#endif // DEBUG_TYPE


  type1 = new_type1 ;
  type2 = new_type2 ;

  if (type1 != NULL)
	{
	  type1->link() ;
	  type1->set_father(this) ;

#ifdef DEBUG_TYPE
	  TRACE2("type1 %x father %x", type1, type1->get_father()) ;
#endif // DEBUG_TYPE
	}

  if (type2 != NULL)
	{
	  type2->link() ;
	  type2->set_father(this) ;

#ifdef DEBUG_TYPE
	  TRACE2("type2 %x father %x", type2, type2->get_father()) ;
#endif // DEBUG_TYPE
	}
}

// Destructeur
T_product_type::~T_product_type(void)
{
}

// Calcul du nombre de types
int T_product_type::get_nb_types(void)
{
#ifdef DEBUG_TYPE
  TRACE2("T_product_type(%x:%s)::get_nb_types() -> ...",
		 this,
		 make_type_name()->get_value()) ;
#endif // DEBUG_TYPE

  int res = type1->get_nb_types() + type2->get_nb_types() ;

#ifdef DEBUG_TYPE
  TRACE3("T_product_type(%x:%s)::get_nb_types() -> %d",
		 this, make_type_name()->get_value(), res) ;
#endif // DEBUG_TYPE

  return res ;
}


// Calcul du nombre de types
int T_product_type::get_nb_src_types(void)
{
  return type1->get_nb_types() ;
}


// Calcul du nombre de types
int T_product_type::get_nb_dst_types(void)
{
  return type1->get_nb_types() ;
}


// Obtention de la liste des types
T_type *T_product_type::get_spec_types(void)
{
  TRACE2("T_product_type(%x:%s)::get_spec_types",
		 this,
		 make_type_name()->get_value()) ;
  if (type1->get_node_type() == NODE_PRODUCT_TYPE)
	{
	  TRACE0("--> appel recursif ...") ;
	  ENTER_TRACE ;
	  return ((T_product_type *)type1)->get_spec_types() ;
	  TRACE0("<-- appel recursif ...") ;
	  EXIT_TRACE ;
	}

  TRACE2("--> %x:%s", type1, type1->make_type_name()->get_value()) ;
  return type1 ;
}

T_type *T_product_type::get_types(void)
{
  TRACE2("T_product_type(%x:%s)::get_spec_types",
		 this,
		 make_type_name()->get_value()) ;
  return get_spec_types()->get_real_type() ;
}

void T_product_type::reset_bounds(void)
{
  type1->reset_bounds() ;
  type2->reset_bounds() ;
}


T_type *T_product_type::get_type1(void)
{
#ifdef DEBUG_TYPE
TRACE2("T_product_type(%x)::get_type1 type1 = %x", this, type1) ;
TRACE2("T_product_type(%x)::get_type1 type1->get_real_type = %x",
	   this,
	   type1->get_real_type()) ;
#endif // DEBUG_TYPE
  return type1->get_real_type() ;
}

T_type *T_product_type::get_type2(void)
{
#ifdef DEBUG_TYPE
TRACE2("T_product_type(%x)::get_type2 type2 = %x", this, type2) ;
TRACE2("T_product_type(%x)::get_type2 type2->get_real_type = %x",
	   this,
	   type2->get_real_type()) ;
#endif // DEBUG_TYPE
  return type2->get_real_type() ;
}

// Est-ce le type P(Z*Z)*T (T quelconque)
//retourne TRUE si type1 = P(Z*Z)
int T_product_type::is_a_tree_node_type(void)
{
  if((type1 == NULL) || (type2 == NULL))
	{
	  //Reprise sur erreur
	  return FALSE ;
	}


  // type1 doit etre:
  //(1) une sequence de la forme P(Z*T)
  //(2) T doit etre du type Z


  //(1) est-ce une sequence de la forme P(Z*T)
  if (type1->is_a_seq() == FALSE)
	{
	  //NON !!
	  return FALSE ;
	}

  //(2) est-ce une sequence vide ?
  if(type1->is_a_non_empty_seq() == FALSE)
	{
	  //Oui c'est une sequence vide
	  return TRUE ;
	}

  // !!! ici on sait que type1 est de la form P(Z*T)  !!!

  //(3) dans P(Z*T), SI T est du type Z retourne TRUE sinon FALSE
  //cast justifie par le test ci-dessus


  return ((T_setof_type*)type1)->get_seq_base_type()->is_an_integer() ;


}

//
//	}{	Classe T_setof_type
//
T_setof_type::T_setof_type(T_type *new_base_type,
									T_item *new_father,
									T_betree *new_betree,
									T_lexem *ref_lexem)
  : T_constructor_type(NODE_SETOF_TYPE,
					   new_father,
					   new_betree,
					   ref_lexem)
{
  ASSERT(new_base_type != NULL) ;
#ifdef DEBUG_TYPE
  TRACE5("T_setof_type(%x)::T_setof_type(base_type = %x (%s), %x, %x, %x, %x)",
		 this,
		 new_base_type,
		 new_base_type->make_type_name()->get_value(),
		 new_father,
		  new_betree) ;
#endif

  base_type = new_base_type ;
  base_type->set_father(this) ;
  if (base_type != NULL) // Peut etre NULL pour {}
	{
	  base_type->link() ;
	}
}

//
// Constructeur pour les sous-classes
//
// RAPPEL : base_type peut val
//
T_setof_type::T_setof_type(T_node_type new_node_type,
									T_type *new_base_type,
									T_item *new_father,
									T_betree *new_betree,
									T_lexem *ref_lexem)
  : T_constructor_type(new_node_type,
					   new_father,
					   new_betree,
					   ref_lexem)
{
#ifdef DEBUG_TYPE
  TRACE5("T_setof_type(%x)::T_setof_type(base_type = %x (%s), %x, %x, %x, %x)",
		 this,
		 new_base_type,
		 (new_base_type == NULL)
		 ? "" : new_base_type->make_type_name()->get_value(),
		 new_father,
		  new_betree) ;
#endif

  base_type = new_base_type ;
  if (base_type != NULL) // Peut etre NULL pour {}
	{
	  base_type->set_father(this) ;
	  base_type->link() ;
	}
}

// Destructeur
T_setof_type::~T_setof_type(void)
{
}

T_type *T_setof_type::get_base_type(void)
{
  TRACE1("this = %x", this) ;
  TRACE1("base_type = %x", base_type) ;
  return (base_type == NULL) ? (T_type *)NULL : base_type->get_real_type() ;
}

void T_setof_type::set_base_type(T_type *new_base_type)
{
  TRACE3("T_setof_type(%x)::set_base_type(%x, was %x)",
		 this,
		 new_base_type,
		 base_type) ;

  base_type = new_base_type ;
}

void T_setof_type::reset_bounds(void)
{
  base_type->reset_bounds() ;
}

// Pour obtenir le type des elements d'une sequence
// Attention le type doit etre une sequence i.e. l'appelant
// doit le verifier avant d'appeler cette fonction
// (avec type->is_a_seq() par exemple)
T_type *T_setof_type::get_seq_base_type(void)
{
#ifdef DEBUG_TYPE
  TRACE2("T_setof_type(%x)::get_seq_base_type(%s)",
		 this,
		 make_type_name()->get_value()) ;
#endif

  // Le type est de la forme : Setof(INT * Type(e))
  // Les casts sont justifies par l'appelant, et a posteriori
  // avec des ASSERT
  T_product_type *prod = (T_product_type *)base_type ;
  ASSERT(prod->get_node_type() == NODE_PRODUCT_TYPE) ;
  return prod->get_type2() ;
}

// Pour obtenir l'ensemble de depart (resp. arrivee) d'une relation
// Attention le type doit etre une relation (a verifier
// par l'appelant)
T_type *T_setof_type::get_relation_src_type(void)
{
#ifdef DEBUG_TYPE
  TRACE3("T_setof_type(%x:%s) <%s>::get_relation_src_type",
		 this,
		 get_class_name(),
		 make_type_name()->get_value()) ;
#endif

  // Le type est de la forme : Setof(SRC * DST)
  // Les casts sont justifies par l'appelant, et a posteriori
  // avec des ASSERT
  T_product_type *inside = (T_product_type *)get_base_type() ;
  TRACE2("inside = %x %s", inside, inside->get_class_name()) ;

#ifndef NO_CHECKS
  if (inside->get_ref_lexem() != NULL)
	{
	  TRACE3("inside = %s:%d:%d",
			 inside->get_ref_lexem()->get_file_name()->get_value(),
			 inside->get_ref_lexem()->get_file_line(),
			 inside->get_ref_lexem()->get_file_column()) ;
	}
  ASSERT(inside->get_node_type() == NODE_PRODUCT_TYPE) ;
#endif // NODE_PRODUCT_TYPE

  return inside->get_type1() ;
}

T_type *T_setof_type::get_relation_dst_type(void)
{
#ifdef DEBUG_TYPE
  TRACE1("T_setof_type(%x)::get_relation_dst_type", this) ;
#endif

  // Le type est de la forme : Setof(T1 * T2 * .. Tn)
  // Les casts sont justifies par l'appelant, et a posteriori
  // avec des ASSERT
  T_product_type *inside = (T_product_type *)get_base_type() ;
  ASSERT(inside->get_node_type() == NODE_PRODUCT_TYPE) ;
  return inside->get_type2() ;
}


// Est-ce un ensemble ?
int T_setof_type::is_a_set(void)
{
return TRUE ;
}

// Pour savoir si un type est un ensemble index large
// Dans le cas general : non
// TRUE ssi ensemble de base, ensemble en extension ou identificateur,
// i.e. si le type est Setof(K(T, U, V))
int T_setof_type::is_an_extended_index_set(void)
{
#ifdef DEBUG_TYPE
  TRACE2("T_setof_type(%x)::is_an_extended_index_set() type = %s",
		 this,
		 make_type_name()->get_value()) ;
#endif


  if ( (get_base_type() == NULL) || (get_base_type()->is_a_base_type() == FALSE) )
	{
#ifdef DEBUG_TYPE
	  TRACE1("pas type de base mais %s -> FALSE",
			 (base_type == NULL) ? "<null>" : base_type->get_class_name()) ;
#endif

	  return FALSE ;
	}

  T_base_type *predef = (T_base_type *)get_base_type() ;

  if ( (predef->get_upper_bound() == NULL)
	   || (predef->get_upper_bound() == NULL) )
	{
#ifdef DEBUG_TYPE
	  TRACE0("bornes non etablies -> FALSE") ;
#endif

	  return FALSE ;
	}

#ifdef DEBUG_TYPE
  TRACE0("OK !") ;
#endif

  return TRUE ;
}

// Est-ce une sequence ?
int T_setof_type::is_a_seq(void)
{
  TRACE2("T_setof_type(%x::%s)::is_a_seq()",
		 this,
		 make_type_name()->get_value()) ;

  // Il faut que le type soit :Setof(btype(INTEGER) * T))
  if (get_base_type()->get_node_type() != NODE_PRODUCT_TYPE)
	{
	  TRACE0("pas Setof(Setof(Product ->pas seq") ;
	  return FALSE ;
	}
  T_product_type *prod = (T_product_type *)get_base_type() ;

  if (prod->get_types()->is_an_integer() == FALSE)
	{
	  TRACE0("1er type pas un type entier -> pas seq") ;
	  return FALSE ;
	}

  TRACE0("c'est un seq !!!") ;
  return TRUE ;
}

// Est-ce une sequence non vide
int T_setof_type::is_a_non_empty_seq(void)
{
  //rq: redefinie pour T_generic_type
  return is_a_seq() ;
}


//
//	}{	Classe T_abstract_type
//
// Constructeur
T_abstract_type::T_abstract_type(T_ident *new_set,
										  T_expr *new_lower_bound,
										  T_expr *new_upper_bound,
										  T_item *new_father,
										  T_betree *new_betree,
										  T_lexem *ref_lexem)
  : T_base_type(NODE_ABSTRACT_TYPE,
				new_set,
				new_lower_bound,
				new_upper_bound,
				new_father,
				new_betree,
				ref_lexem)
{
#ifdef DEBUG_TYPE
  TRACE3("T_abstract_type(%x)::T_abstract_type(%d, %x, %x, %x, %x)",
		 this, new_father, new_betree) ;
#endif


  set = new_set ;
  after_valuation_type = NULL ;

  // On s'ajoute a la liste des types abstraits
  get_object_manager()->add_abstract_type(this) ;
}

// Destructeur
T_abstract_type::~T_abstract_type(void)
{
  // On se supprime du gestionnaire d'objets
  get_object_manager()->delete_abstract_type(this) ;
}


// Mise a jour du type apres valuation
void T_abstract_type::set_after_valuation_type(T_type *new_after)
{
  TRACE2("T_abstract_type(%x)::set_after_valuation_type(%x)",
		 this,
		 new_after) ;
  ASSERT(new_after != this) ;
  if (after_valuation_type != NULL)
	{
 	  after_valuation_type->unlink() ;
			  // TMP_STATE
	  //	  set_block_state(after_valuation_type, MINFO_ALWAYS_PERSISTENT) ;
			  // TMP_STATE
	}
  // TMP_STATE
  // set_block_state(this, MINFO_ALWAYS_PERSISTENT) ;
  // TMP_STATE
  after_valuation_type = new_after ;
  new_after->set_before_valuation_type(this) ;
  new_after->link() ;
}

// Mise a jour des bornes, classe T_abstract_type (valable pour un ensemble value !)
void T_abstract_type::set_bounds(T_expr *new_lower_bound,
										  T_expr *new_upper_bound)
{
  TRACE4("T_abstract_type(%x:%s)::set_bounds(%x, %x)",
		 this,
		 make_type_name()->get_value(),
		 new_lower_bound,
		 new_upper_bound) ;

  if (after_valuation_type == NULL)
	{
	  T_base_type::set_bounds(new_lower_bound, new_upper_bound) ;
	  return ;
	}

  ASSERT(after_valuation_type != NULL) ;

 if (after_valuation_type->is_a_base_type() == TRUE)
   {
	 T_base_type *btype = (T_base_type *)after_valuation_type ;
	 btype->set_bounds(new_lower_bound, new_upper_bound) ;
	 return ;
   }

 // Ici par construction on est value avec un autre ensemble abstrait
 ASSERT(after_valuation_type->get_node_type() == NODE_ABSTRACT_TYPE) ;
 T_abstract_type *atype = (T_abstract_type *)after_valuation_type ;

 atype->set_bounds(new_lower_bound, new_upper_bound) ;
}

T_type *T_abstract_type::get_real_type(void)
{
#ifdef DEBUG_TYPE
  TRACE3("T_abstract_type(%x)::get_real_type() -> %x/%x",
		 this,
		 get_after_valuation_type(),
		 this) ;
#endif // DEBUG_TYPE
  if (get_after_valuation_type() != NULL)
	{
	  return get_after_valuation_type()->get_real_type() ;
	}
  else
	{
	  return this ;
	}
}

//
//	}{	Classe T_enumerated_type
//
// Constructeur
T_enumerated_type::T_enumerated_type(T_ident *new_type_definition,
											  T_expr *new_lower_bound,
											  T_expr *new_upper_bound,
											  T_item *new_father,
											  T_betree *new_betree,
											  T_lexem *ref_lexem)
  : T_base_type(NODE_ENUMERATED_TYPE,
				new_type_definition,
				new_lower_bound,
				new_upper_bound,
				new_father,
				new_betree,
				ref_lexem)
{
#ifdef DEBUG_TYPE
  TRACE3("T_enumerated_type(%x)::T_enumerated_type(%d, %x, %x, %x, %x)",
		 this, new_father, new_betree) ;
#endif


  type_definition = new_type_definition ;
}



// Destructeur
T_enumerated_type::~T_enumerated_type(void)
{
}

//
//	}{	Classe T_predefined_type
//
// Constructeur
T_predefined_type::T_predefined_type(T_builtin_type new_type,
											  T_expr *new_lower_bound,
											  T_expr *new_upper_bound,
											  T_item *new_father,
											  T_betree *new_betree,
											  T_lexem *ref_lexem)
  : T_base_type(NODE_PREDEFINED_TYPE,
				(new_type == BTYPE_INTEGER)
				? get_builtin_INT()
				: (new_type == BTYPE_BOOL)
				? get_builtin_BOOL()
                                : (new_type == BTYPE_REAL)
                                ? get_builtin_REAL()
                                    : (new_type == BTYPE_FLOAT)
                                    ? get_builtin_FLOAT()
                                        : get_builtin_STRING(),
				new_lower_bound,
				new_upper_bound,
				new_father,
				new_betree,
				ref_lexem)
{
#ifdef DEBUG_TYPE
  TRACE5("T_predefined_type(%x)::T_ptype(%d, %x, %x, %x, %x) ref_lexem %x",
		 this, new_type, new_father, new_betree, ref_lexem) ;
#endif

  type = new_type ;
  ref_interval = NULL ;
}

// Destructeur
T_predefined_type::~T_predefined_type(void)
{
}

int T_predefined_type::is_an_integer(void)
{
#ifdef DEBUG_TYPE
  TRACE2("T_predefined_type(%x)::is_an_integer -> %s",
                 this,
                 (type == BTYPE_INTEGER) ? "TRUE" : "FALSE") ;
#endif

  return (type == BTYPE_INTEGER) ? TRUE : FALSE ;
}

int T_predefined_type::is_a_real(void)
{
#ifdef DEBUG_TYPE
  TRACE2("T_predefined_type(%x)::is_a_real -> %s",
                 this,
                 (type == BTYPE_REAL) ? "TRUE" : "FALSE") ;
#endif

  return (type == BTYPE_REAL) ? TRUE : FALSE ;
}

int T_predefined_type::is_a_float(void)
{
#ifdef DEBUG_TYPE
  TRACE2("T_predefined_type(%x)::is_a_float -> %s",
                 this,
                 (type == BTYPE_FLOAT) ? "TRUE" : "FALSE") ;
#endif

  return (type == BTYPE_FLOAT) ? TRUE : FALSE ;
}
void T_predefined_type::set_ref_interval(T_ident *new_ref_interval)
{
  TRACE3("T_predefined_type(%x)::set_ref_interval(%x, was %x)",
		 this,
		 new_ref_interval,
		 ref_interval) ;

  if (ref_interval != NULL)
	{
	  ref_interval->unlink() ;
	}

  ref_interval = new_ref_interval ;
  ref_interval->link() ;

  // En plus, si ref_interval != NULL et que les bornes sont NULL,
  // on les positionnes
  if (    (ref_interval != NULL)
	   && (get_lower_bound() == NULL)
	   && (get_upper_bound() == NULL) )
	{
	  TRACE0("creation de bornes") ;

	  // Creation des bornes
	  T_bound *lower_bound = new T_bound(ref_interval,
										 FALSE,
										 NULL,
										 NULL,
										 this,
										 get_betree(),
										 get_ref_lexem()) ;
	  T_bound *upper_bound = new T_bound(ref_interval,
										 TRUE,
										 NULL,
										 NULL,
										 this,
										 get_betree(),
										 get_ref_lexem()) ;

	  // Positionnement
	  set_bounds(lower_bound, upper_bound) ;
	}
}


//
//	}{ Classe T_generic_type
//
// Constructeur
T_generic_type::T_generic_type(T_item *new_father,
										T_betree *new_betree,
										T_lexem *ref_lexem)
  : T_type(NODE_GENERIC_TYPE, new_father, new_betree, ref_lexem)
{
  TRACE1("T_generic_type(%x)::T_generic_type", this) ;
  type = NULL ;
}

// Destructeur
T_generic_type::~T_generic_type(void)
{
}

// Obtention des types apres valuation
T_type *T_generic_type::get_real_type(void)
{
#ifdef DEBUG_TYPE
  TRACE3("T_generic_type(%x)::get_real_type() -> %x/%x",
		 this,
		 type,
		 this) ;
#endif // DEBUG_TYPE
  return (type == NULL) ? this : type ;
}

int T_generic_type::is_an_integer(void)
{
#ifdef DEBUG_TYPE
  TRACE2("T_generic_type(%x)::is_an_integer(type = %s)",
		 this,
		 ::make_type_name(type)->get_value()) ;
#endif

  if (type == NULL)
	{
	  // On renvoie TRUE et on instancie le joker

	  type = new T_predefined_type(BTYPE_INTEGER,
								   NULL,
								   NULL,
								   this,
								   get_betree(),
								   get_ref_lexem()) ;

	  return TRUE ;
	}
  else
	{
	  return type->is_an_integer() ;
	}

}

int T_generic_type::is_a_real(void)
{
#ifdef DEBUG_TYPE
  TRACE2("T_generic_type(%x)::is_a_real(type = %s)",
                 this,
                 ::make_type_name(type)->get_value()) ;
#endif

  if (type == NULL)
        {
          // On renvoie TRUE et on instancie le joker

          type = new T_predefined_type(BTYPE_REAL,
                                                                   NULL,
                                                                   NULL,
                                                                   this,
                                                                   get_betree(),
                                                                   get_ref_lexem()) ;

          return TRUE ;
        }
  else
        {
          return type->is_a_real() ;
        }

}


int T_generic_type::is_a_float(void)
{
#ifdef DEBUG_TYPE
  TRACE2("T_generic_type(%x)::is_a_float(type = %s)",
                 this,
                 ::make_type_name(type)->get_value()) ;
#endif

  if (type == NULL)
        {
          // On renvoie TRUE et on instancie le joker

          type = new T_predefined_type(BTYPE_FLOAT,
                                                                   NULL,
                                                                   NULL,
                                                                   this,
                                                                   get_betree(),
                                                                   get_ref_lexem()) ;

          return TRUE ;
        }
  else
        {
          return type->is_a_float() ;
        }

}


//
//	}{ Classe T_label_type
//

// Constructeur
T_label_type::T_label_type(T_ident *new_name,
									T_setof_type *ref_setof,
									T_type *new_type,
									T_item **adr_first,
									T_item **adr_last,
									T_item *new_father,
									T_betree *new_betree,
									T_lexem *new_ref_lexem)
  : T_type(NODE_LABEL_TYPE,
		   new_father,
		   new_betree,
		   new_ref_lexem)
{
  TRACE1("T_label_type(%x)::T_label_type", this) ;
  name = new_name ;
  if (name != NULL)
	{
	  name->link() ;
	}

  if (new_type != NULL)
	{
	  type = new_type->clone_type(this, new_betree, new_ref_lexem) ;
	  type->link() ;

	  if (ref_setof != NULL)
		{
		  // Propagation du typing ident
		  // ex : struct(t1 : type_article)
		  type->set_typing_ident(ref_setof->get_typing_ident()) ;
		}
	}
  else
	{
	  new_type = NULL ;
	}

  // On s'insere a la main car on ne passe pas adr_first, adr_last
  // a T_type::T_type
  tail_insert(adr_first, adr_last) ;
}

// Destructeur
T_label_type::~T_label_type(void)
{
}

// Classe T_record_type
T_record_type::T_record_type(T_item *father,
									  T_betree *betree,
									  T_lexem *ref_lexem)
  : T_constructor_type(NODE_RECORD_TYPE, father, betree, ref_lexem)
{
  TRACE1("T_record_type(%x)::T_record_type", this) ;
  first_label = last_label = NULL ;
}

// Destructeur
T_record_type::~T_record_type(void)
{
}

// ref_type est-il une valuation de T_type (plongement)
// Si oui, remplit **adr_atype avec le type abstrait a plonger
// cas par defaut : non
int T_type::can_be_valuated_by(T_type *ref_type,
										T_ident *ident,
										T_abstract_type **adr_atype)
{
  TRACE6("T_type(%x:%s)::can_be_valuated_by(%x:%s, ident %x:%s) -> FALSE",
		 this,
		 make_type_name()->get_value(),
		 ref_type,
		 ref_type->make_type_name()->get_value(),
		 ident,
		 ident->get_name()->get_value()) ;

  return FALSE ;
}

// ref_type est-il une valuation de T_type (plongement)
// Si oui, remplit **adr_atype avec le type abstrait a plonger
int T_setof_type::can_be_valuated_by(T_type *ref_type,
											  T_ident *ident,
											  T_abstract_type **adr_atype)
{
  TRACE6(">> T_setof_type(%x:%s)::can_be_valuated_by(%x:%s, ident %x:%s)",
		 this,
		 make_type_name()->get_value(),
		 ref_type,
		 ref_type->make_type_name()->get_value(),
		 ident,
		 ident->get_name()->get_value()) ;
  ENTER_TRACE ;

  // Ici : on value le type abstrait ENS si et seulement si
  // on est dans la ligne ENS = ...
  // Pour cela il faut :
  // 1) que le type precedent de l'identificateur soit un type abstrait
  // 2) que l'identificateur a valuer soit de meme nom que le type abstrait

  // Le type precedent etait-il un type abstrait ?
  // Cas justifie a posteriori le cas echant
  T_abstract_type *atype = (T_abstract_type *)get_base_type() ;

  if (atype->get_node_type() == NODE_ABSTRACT_TYPE)
	{
	  if (atype->get_set()->get_name()->compare(ident->get_name()) == TRUE)
		{
		  // On est bien dans la valuation de l'ensemble
#ifdef DUMP_TYPES
		  T_lexem *ref_lexem = ident->get_ref_lexem() ;
		  TRACE4("ON est bien dans la valuation de %s (%s:%d:%d)",
				 ident->get_name()->get_value(),
				 (ref_lexem == NULL)
				 	? "(null)" : ref_lexem->get_file_name()->get_value(),
				 (ref_lexem == NULL) ? 0 : ref_lexem->get_file_line(),
				 (ref_lexem == NULL) ? 0 : ref_lexem->get_file_column()) ;
#endif

		  // Il faut verifier que l'on value avec un ensemble
		  if (ref_type->is_a_set() == FALSE)
			{
			  TRACE1("ici erreur ref_type %x", ref_type) ;
			  semantic_error(ref_type,
							 CAN_CONTINUE,
							 get_error_msg(E_OPERAND_SHOULD_BE_A_SET),
							 ref_type->make_type_name()->get_value()) ;
			  TRACE0("can_be_valuated_by -> FALSE") ;
			  return FALSE ;
			}

		  TRACE0("can_be_valuated_by -> TRUE") ;

		  if (adr_atype != NULL)
			{
			  *adr_atype = atype ;
			}
		  return TRUE ;
		}
	}

  // Si on arrive ici, c'est que ce n'etait pas une valuation
  EXIT_TRACE ;
  TRACE0("can_be_valuated_by -> FALSE") ;
  return FALSE ;
}

//GP 22/01/99
//Ensemble de fonctions qui permettent de savoir si un type
//est construit à base de STRING
int T_type::is_based_on_string()
{
  return FALSE ;
}

//GP 11/02/99
//Fonction qui retourne TRUE si le type est construit
//à partir d'un type généric
int T_type::is_based_on_generic_type(void)
{
  return FALSE ;
}

int T_product_type::is_based_on_string()
{
  if((type1 != NULL) && (type1->is_based_on_string() == TRUE))
	{
	  return TRUE ;
	}
  else if ((type2 != NULL) && (type2->is_based_on_string() == TRUE))
	{
	  return TRUE ;
	}

  if((type1 == NULL) || (type2 == NULL))
	{
	  TRACE1("REPRISE SUR ERREUR: T_product_type(%x)::is_based_on_string",
			 this);
	}

  return FALSE ;

}

//GP 11/02/99
//Fonction qui retourne TRUE si le type est construit
//à partir d'un type généric
int T_product_type::is_based_on_generic_type(void)
{
  if((type1 != NULL) && (type1->is_based_on_generic_type() == TRUE))
	{
	  return TRUE ;
	}
  else if ((type2 != NULL) && (type2->is_based_on_generic_type() == TRUE))
	{
	  return TRUE ;
	}

  if((type1 == NULL) || (type2 == NULL))
	{
	  TRACE1("REPRISE SUR ERREUR:T_product_type(%x)::is_based_on_generic_type",
			 this);
	}

  return FALSE ;
}

int T_setof_type::is_based_on_string()
{
  if(base_type == NULL)
	{
	  //Ensemble vide
	  return FALSE ;
	}
  if(base_type->is_based_on_string()==TRUE)
	{
	  return TRUE ;
	}

  return FALSE  ;
}

//GP 11/02/99
//Fonction qui retourne TRUE si le type est construit
//à partir d'un type généric
int T_setof_type::is_based_on_generic_type(void)
{
  if(base_type->is_based_on_generic_type()==TRUE)
	{
	  return TRUE ;
	}

  return FALSE  ;
}

// Est-ce le type P(P(Z*Z)*T) (T quelconque)
// FALSE par défaut
int T_setof_type::is_a_tree_type(void)
{
  TRACE1("T_setof_type(%x)::is_a_tree_type", this) ;

  if(base_type==NULL)
	{
	  TRACE1("REPRISE SUR ERREUR: T_setof_type(%x)::is_a_tree_type", this) ;
	  return FALSE ;
	}

  //retourne TRUE si base_type est de la forme P(Z*Z)*T
  return base_type->is_a_tree_node_type() ;
}

int T_predefined_type::is_based_on_string()
{
  if(type == BTYPE_STRING)
	{
	  return TRUE ;
	}

  return FALSE ;
}

//
// Fonctions "wildcards" (type generique non instancie)
//
int T_base_type::has_wildcards(int ignore_valuation)
{
	//  TRACE1("T_base_type(%x)::has_wildcards()->FALSE", this) ;
  return FALSE ;
}

int T_product_type::has_wildcards(int ignore_valuation)
{
	//  TRACE1("T_product_type(%x)::has_wildcards() ->> ", this) ;
  if (type1->has_wildcards(ignore_valuation) == TRUE)
	{
	  return TRUE ;
	}

  return type2->has_wildcards(ignore_valuation) ;
}

int T_setof_type::has_wildcards(int ignore_valuation)
{
	//  TRACE1("T_setof_type(%x)::has_wildcards() ->>", this) ;
  return base_type->has_wildcards(ignore_valuation) ;
}

int T_abstract_type::has_wildcards(int ignore_valuation)
{
	//  TRACE1("T_abstract_type(%x)::has_wildcards()->FALSE", this) ;
  return (after_valuation_type == NULL)
	? FALSE : after_valuation_type->has_wildcards(ignore_valuation) ;
}

int T_enumerated_type::has_wildcards(int ignore_valuation)
{
	//  TRACE1("T_enumerated_type(%x)::has_wildcards()->FALSE", this) ;
  return FALSE ;
}

int T_predefined_type::has_wildcards(int ignore_valuation)
{
	//  TRACE1("T_predefined_type(%x)::has_wildcards()->FALSE", this) ;
  return FALSE ;
}

int T_generic_type::has_wildcards(int ignore_valuation)
{
  if (ignore_valuation == TRUE)
	{
		/*
	  TRACE2("T_generic_type(%x type=%x)::has_wildcards(ignore_valuation)->TRUE",
			 this,
			 type) ;
		*/
	  return TRUE ;
	}
  else
	{
		/*
	  TRACE3("T_generic_type(%x type=%x)::has_wildcards()->%s",
			 this,
			 type,
			 (type == NULL) ? "TRUE" : "FALSE") ;
		*/
	  return (type == NULL) ? TRUE : FALSE ;
	}
}

int T_record_type::has_wildcards(int ignore_valuation)
{
	//  TRACE1("T_record_type(%x)::has_wildcards() -> FALSE", this) ;
  return FALSE ;
}

int T_label_type::has_wildcards(int ignore_valuation)
{
	//  TRACE1("T_label_type(%x)::has_wildcards() -> FALSE", this) ;
  return FALSE ;
}

// Pour le typage par identificateur :
// est-ce une def de type tableau ?
// est-ce une def de type record ?
// est-ce une def de type entier ?
int T_type::is_an_array_type_definition(void)
{
	/*
  TRACE2("T_type(%x:%s)::is_an_array_type_definition() -> FALSE",
		 this,
		 make_type_name()->get_value()) ;
	*/
  return FALSE ;
}

int T_type::is_a_record_type_definition(void)
{
	/*
  TRACE2("T_type(%x:%s)::is_a_record_type_definition() -> FALSE",
		 this,
		 make_type_name()->get_value()) ;
	*/
  return FALSE ;
}

int T_type::is_an_integer_type_definition(void)
{
	/*
  TRACE2("T_type(%x:%s)::is_an_integer_type_definition() -> FALSE",
		 this,
		 make_type_name()->get_value()) ;
	*/
  return FALSE ;
}

int T_setof_type::is_an_array_type_definition(void)
{
	/*
  TRACE2("T_setof_type(%x:%s)::is_an_array_type_definition() -> FALSE",
		 this,
		 make_type_name()->get_value()) ;
	*/

  // Une definition de type tableau est du type : Setof(Setof(a * b ...))
  if (get_base_type()->get_node_type() != NODE_SETOF_TYPE)
	{
		//	  TRACE0("pas du type S(S ...-> FALSE") ;
	  return FALSE ;
	}

  // Cast valide d'apres le test
  T_setof_type *set = (T_setof_type *)get_base_type() ;

  if (set->get_base_type()->get_node_type() != NODE_PRODUCT_TYPE)
	{
		//	  TRACE0("pas du type S(S(a*b...)-> FALSE") ;
	  return FALSE ;
	}

  TRACE0("ok !") ;
  return TRUE ;
}

int T_setof_type::is_a_record_type_definition(void)
{
	/*
  TRACE2("T_setof_type(%x:%s)::is_a_record_type_definition() -> FALSE",
		 this,
		 make_type_name()->get_value()) ;
	*/

  // Une definition de type record est du type : Setof(struct...
  if (get_base_type()->get_node_type() != NODE_RECORD_TYPE)
	{
		//	  TRACE0("pas du type S(struct ...-> FALSE") ;
	  return FALSE ;
	}

  //  TRACE0("ok !") ;
  return TRUE ;
}

int T_setof_type::is_an_integer_type_definition(void)
{
  int result = base_type->is_a_concrete_integer() ;

	/*
  TRACE2("T_setof_type(%x:%s)::is_an_integer_type_definition() -> %s",
		 this,
		 make_type_name()->get_value()
		 result == TRUE ? "TRUE" : "FALSE") ;
	*/

  return result ;
}

//
// Fin du fichier
//



