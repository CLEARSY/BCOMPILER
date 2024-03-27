/******************************* CLEARSY **************************************
* Fichier : $Id: i_type.cpp,v 2.0 2002-07-16 07:57:02 cm Exp $
*
* (C) 2008 CLEARSY
*
* Description :		B0 Check
*					Conversion des types B en types B0
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
RCS_ID("$Id: i_type.cpp,v 1.24 2002-07-16 07:57:02 cm Exp $") ;

/* Includes systeme */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

/* Includes Composant Local */
#include "c_api.h"

//
//	}{	Fonctions de fabrication de types B0
//

// Classe T_predefined_type
T_B0_type *T_type::make_B0_type(T_ident *ident,
										 int use_valuation_type,
										 T_item **adr_first,
										 T_item **adr_last,
										 int force_creation)
{
  T_B0_type *res = get_B0_type() ;
  if ( (res != NULL) && (force_creation == FALSE) )
	{
	  // Deja calcule
	  TRACE1("deja fait (%x)", res) ;
	  // TMP ...
	  res->tail_insert(adr_first, adr_last) ;
	  // ... TMP
	  return res ;
	}

  return internal_make_B0_type(ident,
							   use_valuation_type,
							   adr_first,
							   adr_last) ;

}

// Classe T_predefined_type
T_B0_type *T_predefined_type::internal_make_B0_type(T_ident *ident,
															 int use_valuation_type,
															 T_item **adr_first,
															 T_item **adr_last)
{
  TRACE1("T_predefined_type(%x)::internal_make_B0_type()", this) ;

  T_B0_type *res = new T_B0_base_type(this,
									  ref_interval,
									  adr_first,
									  adr_last,
									  this,
									  get_betree(),
									  get_ref_lexem()) ;
  set_B0_type(res) ;
  object_unlink(res) ;
  return res ;

  // Pour le warning
  ident = ident ;
  use_valuation_type = use_valuation_type ;
}


// Fonction auxilaire de fabrication d'un type abstrait
T_B0_type *T_setof_type::make_B0_abstract_type(T_abstract_type *ref_type,
														T_ident *ident,
														T_enumerated_type **adr_ref_enum,
														T_item **adr_first,
														T_item **adr_last)
{
  TRACE7("DEBUT T_setof_type(%x:%s):make_B0_atype(rtype = %x:%s:%s, ident = %x:%s",
		 this,
		 make_type_name()->get_value(),
		 ref_type,
		 ref_type->get_class_name(),
		 ref_type->make_type_name()->get_value(),
		 ident,
		 ident->get_name()->get_value()) ;


  ENTER_TRACE ;

  // Cas general : on fabrique un type abstrait
  // Sauf: si on est autre chose qu'un abstract_set
  // et que le atype a ete value par un intervalle
  // auquel cas on prefere construire un interval_type

  T_B0_interval_type *ref_interval = NULL ;
  T_ident *ref_decla = NULL ;
  T_type *set = NULL ;

  // Mise a jour eventuelle de ref_decla ou ref_interval
  if (ref_type->get_after_valuation_type() != NULL)
	{

	  T_type *set1 = ref_type->get_after_valuation_type() ;

	  if (set1->get_node_type() == NODE_ABSTRACT_TYPE)
		{
		  // Valuation avec un ensemble abstrait
		  T_abstract_type *aset1 = (T_abstract_type *)set1 ;
		  ref_decla = aset1->get_set() ;
		}
	  else
		{
		  // Valuation avec un intervalle

		  set = (T_type *)set1->get_father() ;

		  if (set->is_a_type() == FALSE)
			{
			  // Rien a recuperer
			  TRACE2("ici set %x %s : rien a recuperer",
					 set,
					 set->get_class_name()) ;
			}
		  else
			{
			  TRACE2("DEBUT FABRICATION TYPE B0 APRES VALUATION (set=%x:%s)",
					 set,
					 set->make_type_name()->get_value()) ;

			  ENTER_TRACE ;
			  T_B0_type *ref = make_B0_type(ident, TRUE, NULL, NULL) ;
			  EXIT_TRACE ;
			  TRACE2(" FIN FABRICATION TYPE B0 APRES VALUATION -> %x:%s",
					 ref, ref->get_class_name()) ;

			  if (ref != NULL)
				{
				  ref->set_B_type(set) ;

				  // Comme un ens. abstrait ne peut etre value que par un intervalle
				  // ou un autre ensemble abstrait, on peut faire un switch avec
				  // les deux cas, on sait qu'on ne passe pas dans le default
				  switch(ref->get_node_type())
					{
					case NODE_B0_ABSTRACT_TYPE :
					  {
						// Il faut recuperer le T_ident qui decrit le set.
						// Cette information n'est presente que dans le T_type dual
						// qui est, par construction, un Setof(atype)
						T_B0_abstract_type *b0atype = (T_B0_abstract_type *)ref ;
						ASSERT(b0atype->get_node_type() == NODE_B0_ABSTRACT_TYPE) ;

						T_type *cur_type = b0atype->get_B_type() ;
						TRACE2("ici cur_type %x:%s",
							   cur_type, cur_type->get_class_name()) ;
						T_abstract_type *atype = NULL ;

						if (cur_type->is_a_set() == TRUE)
						  {
							T_setof_type *stype = (T_setof_type *)cur_type ;
							TRACE2("stype %x:%s", stype, stype->get_class_name()) ;

							atype = (T_abstract_type *)stype->get_spec_base_type();
							ASSERT(atype->get_node_type() == NODE_ABSTRACT_TYPE) ;
						  }
						else if (cur_type->get_node_type() == NODE_ABSTRACT_TYPE)
						  {
							atype = (T_abstract_type *)cur_type  ;
						  }
						else
						  {
							assert(FALSE) ;
						  }


						ref_decla = atype->get_set() ;
						break ;
					  }
					case NODE_B0_INTERVAL_TYPE :
					  {
						ref_interval = (T_B0_interval_type *)ref ;
						break ;
					  }

					  // Debut SL 28/10/98
					  // Bug 2019
					  // Un parametre formel ensembliste de machine peut
					  // etre value avec un enumere
					case NODE_B0_ENUMERATED_TYPE :
					  {
						// Il faut recuperer le T_ident qui decrit le set.
						// Cette information n'est presente que dans le T_type dual
						// qui est, par construction, un Setof(atype)
						T_B0_enumerated_type *b0etype =
						  (T_B0_enumerated_type *)ref ;

						// On recupere le Setof(etype(xx))
						// Cast valide par construction
						T_setof_type *set = (T_setof_type *)b0etype->get_B_type() ;

						// On recupere le etype(xx)
						// Cast valide par construction
						*adr_ref_enum = (T_enumerated_type *)set->get_base_type() ;

						// Et c'est fini
						return NULL ;
					  }
					  // Fin SL 28/10/98

					default :
					  {
						TRACE2("panic ref %x %s", ref, ref->get_class_name()) ;
						TRACE1("panic ref %x->lexem", ref->get_ref_lexem()) ;
						TRACE3("panic ref %s:%d:%d",
							   ref->get_ref_lexem()->get_file_name()->get_value(),
							   ref->get_ref_lexem()->get_file_line(),
							   ref->get_ref_lexem()->get_file_column()) ;
						assert(FALSE) ; // A FAIRE
					  }
					}
				}
			}
		}
	}


  if ( (ident != NULL)
	   && (ident->get_name()->compare(ref_type->get_set()->get_name()) == FALSE) )
	{
	  if (ref_interval != NULL)
		{
		  set_B0_type(ref_interval) ;
		  ref_interval->unlink() ;
		  TRACE3("mise a jour du name de %x avec %x:%s",
				 ref_interval,
				 ref_type->get_set(),
				 ref_type->get_set()->get_name()->get_value()) ;

		  ref_interval->set_name(ref_type->get_set()) ;
		  return ref_interval ;
		}

	}

  TRACE0("ICI FAIT UN ATYPE") ;
  T_B0_type *res = new T_B0_abstract_type(ref_type,
										  ref_interval,
										  ref_decla,
										  adr_first,
										  adr_last,
										  this,
										  get_betree(),
										  get_ref_lexem()) ;

  set_B0_type(res) ;
  object_unlink(res) ;

  EXIT_TRACE ;
  TRACE7(" FIN  T_setof_type(%x:%s):make_B0_atype(rtype = %x:%s:%s, ident = %x:%s",
		 this,
		 make_type_name()->get_value(),
		 ref_type,
		 ref_type->get_class_name(),
		 ref_type->make_type_name()->get_value(),
		 ident,
		 ident->get_name()->get_value()) ;

  TRACE1("resultat = %x", res) ;
  return res ;

}




// Classe T_setof_type
T_B0_type *T_setof_type::internal_make_B0_type(T_ident *ident,
														int use_valuation_type,
														T_item **adr_first,
														T_item **adr_last)
{
#if 0
  TRACE2("DEBUT T_setof_type(%x)::internal_make_B0_type(%s)",
		 this,
		 ident->get_name()->get_value()) ;
#endif

  ENTER_TRACE ; ENTER_TRACE ;

  // A FAIRE : REECRIRE AVEC DES METHODES VIRTUELLES !
  T_setof_type *this_type = this ;
  int is_a_type_definition = FALSE ;
  T_type *ref_type = (use_valuation_type == TRUE) ? get_base_type() : base_type ;
  T_B0_type *res ;

  TRACE3("ref_type = %x \"%s\" <%s>",
		 ref_type,
		 ref_type->get_class_name(),
		 ref_type->make_type_name()->get_value()) ;

  if (get_typing_ident() != NULL)
	{
	  // On est sur une définition de type seulement si le type
	  // courant est un type tableau, record ou intervalle et que
	  // l'identificateur est une constante concrète.
	  //
	  // Un test équivalent (compte-tenu des restrictions sur les
	  // constantes) est de dire que la constante ne doit pas etre une
	  // valeur tableau.
	  if (ident->get_ident_type() == ITYPE_CONCRETE_CONSTANT &&
		  ref_type->get_node_type() != NODE_PRODUCT_TYPE)
		{
		  is_a_type_definition = TRUE ;
		}

	  // Fall into : on continue
	}


  if (ref_type->get_node_type() == NODE_SETOF_TYPE)
	{
	  // C'est un type tableau ou record ! traitement different (rentrer dans le
	  // premier Setof)
	  TRACE1("... type tableau ou record ... old_ref_type %s",
			 ref_type->make_type_name()->get_value()) ;
	  this_type = (T_setof_type *)ref_type ;
	  ref_type = this_type->get_base_type() ;
	  TRACE1("... type tableau ou record ... new_ref_type %s",
			 ref_type->make_type_name()->get_value()) ;

	  // Fall into : on continue
	}

  if (ref_type->get_node_type() == NODE_PRODUCT_TYPE)
	{
	  // C'est une declaration de tableau !
	  // On appelle le constructeur des tableaux
	  TRACE0("fabrication d'un tableau") ;
	  res = new T_B0_array_type(this_type,
								ident,
								adr_first,
								adr_last,
								this,
								get_betree(),
								get_ref_lexem()) ;
	}
  else if (ref_type->get_node_type() == NODE_RECORD_TYPE)
	{
	  // Evolution pour typing_ident
	  // C'est une declaration de record !
	  // On appelle le constructeur des records
	  TRACE0("fabrication d'un record") ;
	  res = new T_B0_record_type((T_record_type *)ref_type,
								 ident,
								 adr_first,
								 adr_last,
								 this,
								 get_betree(),
								 get_ref_lexem()) ;
	}
  else
	{
#if 0
	  TRACE3("ici : setof %x, ref_type %x %s",
			 this,
			 ref_type,
			 ref_type->get_class_name()) ;
#endif

	  if ( (ref_type->is_a_base_type() == FALSE)
		   // AB le 05/03 : ajout ligne suivante car constructeur de
		   // T_B0_interval_type utilise get_base_type() dans tous les cas
		   // etude a creuser!...BUG de la machine
		   // /TESTS_LANGAGE/IDENT/TEID2/part30/MchTestLangMU4_3005.mch
		   ||
		   (get_base_type()->is_a_base_type() == FALSE) )
		{
		  T_item *cur = get_father() ;

		  while ( (cur != NULL) && (cur->is_an_ident() == FALSE) )
			{
			  cur = cur->get_father() ;
			}

		  // Propriete de bonne construction : on est arrive ici car
		  // on etudie le type d'un T_ident. Donc le pere est un
		  // T_ident ... sauf en cas d'erreur
		  if (cur == NULL)
			{
			  TRACE1("PANIC arg ! %x n'a pas pour pere un ident", this) ;
			  return NULL ;
			}

		  // Cast justifie car on est sorti du while
		  T_ident *ident = (T_ident *)cur ;

		  // On n'emet un warning que si on est dans la machine principale
		  if (ident->get_ref_machine() ==
			  get_betree_manager()->get_betrees()->get_root())
			{
			  B0_semantic_warning(this,
								  BASE_WARNING,
								  get_warning_msg(W_UNIMPLEMENTABLE_TYPE_FOR_IDENT),
								  make_type_name()->get_value(),
								  ident->get_ident_type_name(),
								  ident->get_name()->get_value()) ;
			}

		  return NULL ;
		}

	  // Sinon :
	  // C'est une declaration d'intervalle ou de type enumere ou abstrait
	  // On appelle le constructeur associe
#if 0
	  TRACE5("ici ident %x:%s:%s, ref_type %x %s",
			 ident,
			 (ident == NULL) ? "" : ident->get_name()->get_value(),
			 (ident == NULL) ? "" : ident->get_ident_type_name(),
			 ref_type, ref_type->get_class_name()) ;
#endif

	  //CT le 11.07.97 : un sous_ensemble d'un ensemble abstrait
	  //comme d'un ensemble predefini est un intervalle....
	  switch(ref_type->get_node_type())
		{

		case NODE_ABSTRACT_TYPE :
		  {
			//CT Le 11.07/97
			//		    TRACE0("Fabrication d'un type abstrait") ;

			ASSERT( ref_type != NULL ) ;
			//CT le 15.07.97 le champ name de ref_type n'est pas a jour....
			((T_abstract_type *)ref_type)->set_name( ((T_abstract_type *)ref_type)->get_set() ) ;

			T_enumerated_type *ref_enum = NULL ;
			T_B0_type *abs_type =
			  make_B0_abstract_type((T_abstract_type *)ref_type,
									((T_abstract_type *)ref_type)->get_set(),
									&ref_enum,
									adr_first,
									adr_last) ;

			if (ref_enum == NULL)
			  {
				TRACE0("Fabrication d'un type intervalle") ;

				res = new T_B0_interval_type(this,
											 abs_type,
											 ident,
											 adr_first,
											 adr_last,
											 this,
											 get_betree(),
											 get_ref_lexem()) ;
			  }
			// Debut SL 28/10/98
			// Bug 2019
			// Un parametre formel ensembliste de machine peut
			// etre value avec un enumere
			else
			  {
				// C'etait un enumere
				res = new T_B0_enumerated_type(ref_enum,
											   adr_first,
											   adr_last,
											   this,
											   get_betree(),
											   get_ref_lexem()) ;
			  }
			// Fin SL 28/10/98
			break ;
		  }
		case NODE_ENUMERATED_TYPE :
		  {
			TRACE0("fabrication d'un type enumere") ;
			res = new T_B0_enumerated_type((T_enumerated_type *)ref_type,
										   adr_first,
										   adr_last,
										   this,
										   get_betree(),
										   get_ref_lexem()) ;
			break ;
		  }
		default : // Ensemble abstrait borne ou intervalle
		  {
			TRACE0("fabrication d'un intervalle") ;
			res = new T_B0_interval_type(this,
										 NULL,
										 NULL,
										 adr_first,
										 adr_last,
										 this,
										 get_betree(),
										 get_ref_lexem()) ;
		  }

		}
	}

  set_B0_type(res) ;
  res->set_is_a_type_definition(is_a_type_definition) ;
  res->unlink() ;

  EXIT_TRACE ; EXIT_TRACE ;
#if 0
  TRACE2(" FIN  T_setof_type(%x)::internal_make_B0_type(%s)",
		 this,
		 ident->get_name()->get_value()) ;
#endif

  return res ;
}


// Classe T_enumerated_type
T_B0_type *T_enumerated_type::internal_make_B0_type(T_ident *ident,
															 int use_valuation_type,
															 T_item **adr_first,
															 T_item **adr_last)
{
  TRACE1("T_enumerated_type(%x)::internal_make_B0_type()", this) ;
  T_B0_type *res =  new T_B0_enumerated_type(this,
											 adr_first,
											 adr_last,
											 this,
											 get_betree(),
											 get_ref_lexem()) ;
  set_B0_type(res) ;
  object_unlink(res) ;
  return res ;

  // Pour le warning
  ident = ident ;
  use_valuation_type = use_valuation_type ;
}

// Classe T_abstract_type
T_B0_type *T_abstract_type::internal_make_B0_type(T_ident *ident,
														   int use_valuation_type,
														   T_item **adr_first,
														   T_item **adr_last)
{
  TRACE1("T_abstract_type(%x)::internal_make_B0_type()", this) ;
  T_B0_type *res ;

  // Cast justifie par construction
  TRACE2("set %x %s", set, set->get_name()->get_value()) ;
  T_setof_type *setof = (T_setof_type *)set->get_B_type() ;
  TRACE2("setof %x %s", setof, setof->get_class_name()) ;
  ASSERT(setof->get_node_type() == NODE_SETOF_TYPE) ;

  // Cast justifie a posteriori si besoin est
  T_abstract_type *ref_atype = (T_abstract_type *)setof->get_spec_base_type() ;

  T_B0_interval_type *ref_interval = NULL ;
  T_ident *ref_decla = NULL ;

  TRACE2("ici ref_atype %x:%s",
		 ref_atype,
		 (ref_atype == NULL) ? "NULL" : ref_atype->get_class_name()) ;

  if (    (ref_atype != NULL)
		  && (ref_atype->get_node_type() == NODE_ABSTRACT_TYPE) )
	{
	  // Cast valide par construction, justifie a posteriori
	  T_B0_abstract_type *b0atype =
		(T_B0_abstract_type *)ref_atype->get_B0_type() ;

	  if (b0atype != NULL)
		{
		  ASSERT(b0atype->get_node_type() == NODE_B0_ABSTRACT_TYPE) ;

		  ref_interval = b0atype->get_ref_interval() ;
		  ref_decla = b0atype->get_ref_decla() ;
		  TRACE4("ref_interval %x:%s, ref_decla %x:%s",
				 ref_interval,
				 (ref_interval == NULL) ? "" : ref_interval->get_class_name(),
				 ref_decla,
				 (ref_decla == NULL) ? "" : ref_decla->get_name()->get_value()) ;
		}


	}


  res = new T_B0_abstract_type(this,
							   ref_interval,
							   ref_decla,
							   adr_first,
							   adr_last,
							   this,
							   get_betree(),
							   get_ref_lexem()) ;


  set_B0_type(res) ;
  object_unlink(res) ;
  return res ;

  // Pour le warning
  ident = ident ;
  use_valuation_type = use_valuation_type ;
}

// Classe T_product_type
T_B0_type *T_product_type::internal_make_B0_type(T_ident *ident,
														  int use_valuation_type,
														  T_item **adr_first,
														  T_item **adr_last)
{
  TRACE1("T_product_type(%x)::internal_make_B0_type()", this) ;

  // Type non implementable
  return NULL ;

  // Pour le warning :
  adr_first = adr_last ;
  ident = ident ;
  use_valuation_type = use_valuation_type ;
}

// Classe T_generic_type
T_B0_type *T_generic_type::internal_make_B0_type(T_ident *ident,
														  int use_valuation_type,
														  T_item **adr_first,
														  T_item **adr_last)
{
  TRACE1("T_generic_type(%x)::internal_make_B0_type()", this) ;
  // ce cas ne doit pas arriver car on ne fait que les types B0
  // des identificateurs, et seuls {} et [] on des types qui
  // sont des instances de T_generic_type
  fprintf(stderr, "%s:%d PANIC\n", __FILE__, __LINE__) ;
  TRACE0("PANIC") ;
  return NULL ;
  assert(FALSE) ; // A FAIRE
  // Pour le warning :
  adr_first = adr_last ;
  ident = ident ;
  use_valuation_type = use_valuation_type ;
}


T_B0_type *T_record_type::internal_make_B0_type(T_ident *ident,
														 int use_valuation_type,
														 T_item **adr_first,
														 T_item **adr_last)
{
  TRACE1("T_record_type(%x)::internal_make_B0_type()", this) ;
  T_B0_record_type *res = new T_B0_record_type(this,
											   ident,
											   adr_first,
											   adr_last,
											   ident,
											   get_betree(),
											   get_ref_lexem()) ;

  set_B0_type(res) ;
  res->unlink() ;
  return res ;

  // Pour les warnings
  use_valuation_type = use_valuation_type ;
}

T_B0_type *T_label_type::internal_make_B0_type(T_ident *ident,
														int use_valuation_type,
														T_item **adr_first,
														T_item **adr_last)
{
  T_B0_label_type *res = new T_B0_label_type(this,
											 ident,
											 adr_first,
											 adr_last,
											 ident,
											 get_betree(),
											 get_ref_lexem()) ;
  set_B0_type(res) ;
  // NON  res->unlink() ; car c'est le lien vers le pere (B0_record_type)
  return res ;

  // Pour les warnings
  use_valuation_type = use_valuation_type ;
  adr_first = adr_last ;
}



//
// Fonction de manipulation de type B0_OM
//
//
// Fonction qui donne le type "suivant" pour un type donné Si le type
// est renomme un autre type on renvoie le type renommé NULL sinon
//
T_ident *get_next_homonym_type(T_ident *type)
{
  TRACE2("get_next_homonym_type(%s, %p)",
		 type->get_name()->get_value(),
		 type);
  T_ident *result = NULL;
  T_ident *def = type;
  while (def->get_def() != NULL)
	{
	  def = def->get_def();
	}
  if (def->get_BOM_type() != NULL
	  &&
	  def->get_BOM_type()->is_an_ident() == TRUE)
	{

	  result = def->get_BOM_type()->make_ident();
	}


  TRACE1("%s<--get_next_homonym_type",
		 result != NULL ? result->get_name()->get_value() : "null");
  return result;
}


//
// Fonction de comparaison de deux types nommés
//
int ident_type_semantic_equal(T_ident *type,
									   T_ident *expected_type)
{
  ASSERT(type != NULL
		 &&
		 expected_type != NULL);
  TRACE4("ident_type_semantic_equal (%s, %p, %s, %p)",
		 type->get_name()->get_value(),
		 type,
		 expected_type->get_name()->get_value(),
		 expected_type);

  int result = FALSE;

  T_ident *cur_type = type;
  while(cur_type != NULL && result == FALSE)
	{
	  T_ident *cur_exp_type = expected_type;
	  while (cur_exp_type != NULL && result == FALSE)
		{
		  if (cur_type->get_name()->
			  compare(cur_exp_type->get_name()) == TRUE)
			{
			  result = TRUE;
			}
		  else
			{
			  cur_exp_type = get_next_homonym_type(cur_exp_type);
			}
		}
	  if (result == FALSE)
		{
		  cur_type = get_next_homonym_type(cur_type);
		}
	}
  TRACE1("%s<--ident_type_semantic_equal", result == TRUE ? "true" :"false");
  return result;
}
//
// Fonction pour comparer deux types sémantiquement
// Seuls cas:
//     ident
//     a..b-->ident a et b ident ou entier litéral
//

int expr_type_semantic_equal(T_expr *expr,
									  T_expr *ref_expr)
{
  TRACE0("expr_type_semantic_equal");
  int result = FALSE;
  if (expr->get_node_type() != ref_expr->get_node_type())
	{
	  result  = FALSE;
	  TRACE0("pas meme type");
	}
  else
	{
	  switch(expr->get_node_type())
		{
		case NODE_RELATION:
		  {
			T_relation *rela_expr = (T_relation *)expr;
			T_relation *rela_ref_expr = (T_relation *)ref_expr;
			result = (expr_type_semantic_equal(rela_expr->get_src_set(),
								  rela_ref_expr->get_src_set()) == TRUE
					  &&
					  expr_type_semantic_equal(rela_expr->get_dst_set(),
								  rela_ref_expr->get_dst_set()) == TRUE
					  &&
					  rela_expr->get_relation_type()
					  == rela_ref_expr->get_relation_type());
			break;
		  }
		case NODE_BINARY_OP:
		  {
			ASSERT(((T_binary_op *)expr)->get_op1()->is_an_expr() == TRUE
				   &&
				   ((T_binary_op *)expr)->get_op2()->is_an_expr() == TRUE
				   &&
				   ((T_binary_op *)ref_expr)->get_op1()->is_an_expr() == TRUE
				   &&
				   ((T_binary_op *)ref_expr)->get_op2()->is_an_expr() == TRUE);

			T_expr *expr_op1 = ((T_binary_op *)expr)->get_op1()->make_expr();
			T_expr *expr_op2 = ((T_binary_op *)expr)->get_op2()->make_expr();
			T_expr *ref_expr_op1 = ((T_binary_op *)ref_expr)->get_op1()->make_expr();
			T_expr *ref_expr_op2 = ((T_binary_op *)ref_expr)->get_op2()->make_expr();

			result = (expr_type_semantic_equal(expr_op1,ref_expr_op1) == TRUE
					  &&
					  expr_type_semantic_equal(expr_op2,ref_expr_op2) == TRUE
					  &&
					  ((T_binary_op *)expr)->get_operator() ==
					  ((T_binary_op *)ref_expr)->get_operator());
			break;
		  }
		case NODE_IDENT:
		  {
			result = ident_type_semantic_equal((T_ident *)expr,(T_ident *)ref_expr);
			break;
		  }
		case NODE_LITERAL_INTEGER:
		  {
			int val_expr;
			int val_ref_expr;
			val_expr = ((T_literal_integer *)expr)->get_value()->
			  get_int_value();
			val_ref_expr = ((T_literal_integer *)ref_expr)->get_value()->
			  get_int_value();
			result = (val_expr == val_ref_expr);
			break;
		  }
		default:
		  // Reprise sur erreur on les déclare incompatible alors
		  // qu'un des types au moins est n'est pas un type B0
		  TRACE1("type du noeud: %d",expr->get_node_type());
		  result = FALSE;
		}
	}
  TRACE3("%s<--expr_type_semantic_equal(%p,%p)",
		 result == TRUE ? "true" :"false",
		 expr,
		 ref_expr);

  return result;
}

//
// Function de test pour savoir si un type est complet
//
int T_ident::is_a_BOM_complete_type()
{
  int result = FALSE;
  if (ident_type == ITYPE_UNKNOWN)
	{
	  result = def->is_a_BOM_complete_type();
	}
  else
	{
	  switch(ident_type)
		{
		case ITYPE_BUILTIN:
		  ASSERT(name == get_builtin_BOOL()->get_name());
		  result = FALSE;
		  break;
		case ITYPE_ABSTRACT_SET:
		  result = TRUE;
		  break;
		case ITYPE_ENUMERATED_SET:
		  result = FALSE;
		  break;
		case ITYPE_CONCRETE_CONSTANT:
		  if (is_an_array_type() == TRUE)
			{
			  T_ident *ident_dst = get_dst_type();
			  result = ident_dst->is_a_BOM_complete_type();
			}
		  else
			{
			  if (BOM_type->is_an_ident() == TRUE)
				{
				  result = BOM_type->make_ident()->is_a_BOM_complete_type();
				}
			  else
				{
				  // Une constante concrete qui n'est pas un tableau
				  // et qui n'est pas valuée par un ident est un
				  // intervalle d'entier donc dans la machine de base
				  // donc est complet par définition
				  result = TRUE;
				}
			}
		  break;

		default: ASSERT(0);
		}
	}

  return result;
}

