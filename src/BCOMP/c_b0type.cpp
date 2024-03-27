/******************************* CLEARSY **************************************
* Fichier : $Id: c_b0type.cpp,v 2.0 2000-02-22 13:43:19 lv Exp $
* (C) 2008 CLEARSY
*
* Description :			Interface des types B0
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
RCS_ID("$Id: c_b0type.cpp,v 1.29 2000-02-22 13:43:19 lv Exp $") ;

/* Includes systeme */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

/* Includes Composant Local */
#include "c_api.h"

//
//	}{	Classe T_B0_type
//
T_B0_type::T_B0_type(T_node_type new_node_type,
							  T_item **adr_first,
							  T_item **adr_last,
							  T_item *new_father,
							  T_betree *new_betree,
							  T_lexem *new_ref_lexem)
  : T_item(new_node_type, adr_first,adr_last, new_father, new_betree, new_ref_lexem)
{
  is_a_type_definition = FALSE ;
}

T_B0_type::~T_B0_type()
{
}

// Pour savoir si un type est un type "de base"
// toujours FALSE sauf pour les T_B0_base_type/T_B0_abstract_type
int T_B0_type::is_a_base_type(void)
{
  return FALSE ;
}

#ifdef B0C
// Controle si check_ident est une donne concrete, dans le cas ou le type B0
// courrant type une donnee concrete
void T_B0_type::check_if_only_concrete_data(T_ident *check_ident)
{
  if (check_ident == NULL)
	{
	  return;
	}

  // Par construction, on type "B0" les identificateurs
  // On retrouve donc l'ident type
  T_item *cur_item = get_father();
  while ( (cur_item != NULL) &&
		  (cur_item->is_an_ident() == FALSE) )
	{
	  cur_item = cur_item->get_father();
	}
  if (cur_item == NULL)
	{
	  return;
	}

  ASSERT (cur_item->is_an_ident() == TRUE);
  // cast justifie par ASSERT
  T_ident *cur_ident = (T_ident *) cur_item->make_expr();

  // On teste si l'ident type est une donne concrete pouvant etre
  // potentiellememt typee "B" avec une donnee non concrete (non implementable)
  // C'est-a-dire : Constante concrete, variable concrete,
  // parametre d'entree/sortie d'operation
  TRACE2("cur_ident %x %s", cur_ident, cur_ident->get_name()->get_value()) ;
  TRACE2("cur_ident->def %x %s",
		 cur_ident->get_definition(),
		 cur_ident->get_ident_type_name()) ;

  // Booléen indiquant si on doit effectivement faire la vérification,
  // suivant la valeur de la ressource de suppression des contrôles B0
  // correspondante.
  int do_check;

  switch (cur_ident->get_definition()->get_ident_type())
	{
	case ITYPE_CONCRETE_VARIABLE :
	  {
		do_check =
		  (get_B0_disable_concrete_variables_type_check() == FALSE);
		break;
	  }
	case ITYPE_OP_IN_PARAM :
	  {
		do_check =
		  (get_B0_disable_operation_input_parameters_type_check() == FALSE);
		break;
	  }
	case ITYPE_OP_OUT_PARAM :
	  {
		do_check =
		  (get_B0_disable_operation_output_parameters_type_check() == FALSE);
		break;
	  }
	case ITYPE_CONCRETE_CONSTANT :
	  {
		do_check =
		  (get_B0_disable_concrete_constants_type_check() == FALSE);
		break;
	  }
	default:
	  {
		do_check = FALSE;
		break;
	  }
	}

  if (do_check == TRUE)
	{
	  // Dans le cas d'un parametre de sortie d'operation,
	  // On attend de se positionner dans la machine de definition
	  if ( (cur_ident->get_definition()->get_ident_type()
			== ITYPE_OP_OUT_PARAM) &&
		   (cur_ident->get_betree()->get_root()->get_machine_type()
			!= MTYPE_SPECIFICATION) )
		{
		  return;
		}

	  switch (check_ident->get_definition()->get_ident_type())
		{
		case ITYPE_ABSTRACT_VARIABLE :
		case ITYPE_ABSTRACT_CONSTANT :
		case ITYPE_LOCAL_VARIABLE :
		  {
			// Il faut crier, sinon le traducteur ne s'en sortira pas...
			if (get_error_count() == 0)
			  {
				T_ident *ident_def = cur_ident->get_definition();
				T_ident *check_ident_def = check_ident->get_definition();
				B0_semantic_error(cur_ident,
								  CAN_CONTINUE,
								  get_error_msg(E_B0_DATA_IS_TYPED_WITH_WRONG_IDENT_TYPE),
								  ident_def->get_ident_type_name(),
								  ident_def->get_name()->get_value(),
								  check_ident_def->get_ident_type_name(),
								  check_ident_def->get_name()->get_value());
			  }
			break;
		  }
		default :
		  {
			// Les autres cas sont autorises
		  }
		}
	  return;
	}
}
#endif //B0C

//
//	}{ Classe T_B0_base_type
//
T_B0_base_type::T_B0_base_type(T_predefined_type *ref_type,
										T_ident *new_name,
										T_item **adr_first,
										T_item **adr_last,
										T_item *new_father,
										T_betree *new_betree,
										T_lexem *new_ref_lexem)
  : T_B0_type(NODE_B0_BASE_TYPE,
			  adr_first,
			  adr_last,
			  new_father,
			  new_betree,
			  new_ref_lexem)
{
  TRACE2("T_B0_base_type::T_B0_base_type(%x), ref_type = %x", this, ref_type) ;

  name = new_name ;

#ifdef B0C
  // Controle le champ name (potentiellement critique pour la traduction)
  check_if_only_concrete_data(name);
#endif // B0C
  type = ref_type->get_type() ;
  min = ref_type->get_lower_bound() ;
  max = ref_type->get_upper_bound() ;
  is_bound = (min == NULL) ? FALSE : TRUE ;
  ASSERT( ( (is_bound == TRUE) && (max != NULL) )
		  || ( (is_bound == FALSE) && (max == NULL) ) ) ;

  ref_type = NULL ; // A FAIRE
}

T_B0_base_type::~T_B0_base_type()
{
}

// Reset des bornes
void T_B0_base_type::reset_bounds(void)
{
  min = NULL ;
  max = NULL ;
  is_bound = FALSE ;
}

// Pour savoir si un type est un type "de base"
// toujours FALSE sauf pour les T_B0_base_type/T_B0_abstract_type
int T_B0_base_type::is_a_base_type(void)
{
  return TRUE ;
}



//
//	}{ Classe T_B0_array_type
//
T_B0_array_type::T_B0_array_type(T_setof_type *ref_type,
										  T_ident *ident,
										  T_item **adr_first,
										  T_item **adr_last,
										  T_item *new_father,
										  T_betree *new_betree,
										  T_lexem *new_ref_lexem)
  : T_B0_type(NODE_B0_ARRAY_TYPE,
			  adr_first,
			  adr_last,
			  new_father,
			  new_betree,
			  new_ref_lexem)
{
  TRACE2("T_B0_array_type::T_B0_array_type(%x), ref_type = %x", this, ref_type) ;
  ENTER_TRACE ;
  ASSERT(get_node_type() == NODE_B0_ARRAY_TYPE) ;

  set_B_type(ref_type) ;

  T_product_type *ptype = (T_product_type *)(ref_type->get_base_type()) ;
  // Garanti par l'appelant
  ASSERT(ptype->get_node_type() == NODE_PRODUCT_TYPE) ;

  // On veut les types "spec" i.e. avant plongement, car ce sont
  // eux seuls qui sont relies
  TRACE2("ici ptype %x:%s", ptype, ptype->make_type_name()->get_value()) ;
  T_type *cur_type = ptype->get_spec_types() ;

  TRACE2("ici cur_type %x:%s", cur_type, cur_type->make_type_name()->get_value()) ;
  T_type *next_type = cur_type->get_next_spec_type() ;

  TRACE2("ici next_type %x:%s",next_type, make_type_name(next_type)->get_value());
  first_src_type = last_src_type = NULL ;

  TRACE1("ici next_type->get_next_spec_type() %x:",
		 next_type->get_next_spec_type()) ;
  TRACE2("ici next_type->get_next_spec_type() %x:%s",
		 next_type->get_next_spec_type(),
		 (next_type->get_next_spec_type() == NULL)
		  ? "null"
		 : next_type->get_next_spec_type()->make_type_name()->get_value()) ;

  TRACE2("CHECK INIT first_src_type = %x, last_src_type = %x",
		 first_src_type,
		 last_src_type) ;
  while (next_type != NULL)
	{
	  TRACE1("-> ajout src_type %x", cur_type) ;
	  cur_type->make_B0_type(ident,
							 FALSE,
							 (T_item **)&first_src_type,
							 (T_item **)&last_src_type,
							 TRUE) ; // on force la creation d'une copie privee
	  TRACE2("CHECK first_src_type = %x, last_src_type = %x",
			 first_src_type,
			 last_src_type) ;

	  // Type suivant
	  next_type = cur_type->get_next_spec_type() ;
	  cur_type = next_type ;
	}

  dst_type = last_src_type ;
  TRACE2("CHECK first_src_type = %x, last_src_type = %x",
		 first_src_type,
		 last_src_type) ;
  TRACE1("dst_type = %x" ,dst_type) ;
  last_src_type->remove_from_list((T_item **)&first_src_type,
								  (T_item **)&last_src_type) ;
  TRACE1("mise en place dst_type = %x", dst_type) ;

  ref_machine = NULL ;
  T_op *ref_op = NULL ;
  find_machine_and_op_def(&ref_machine, &ref_op) ;

  if (ref_machine == NULL)
	{
	  // Reprise sur erreur
	  TRACE0("reprise sur erreur") ;
	  return ;
	}
  next_decla = NULL ;
  prev_decla = NULL ;

  char *buf = get_1Ko_string() ;
  int number = ref_machine->get_specification()->get_tmp2() + 1 ;
  sprintf(buf, "T_%d", number) ;
  ref_machine->get_specification()->set_tmp2(number) ;
  name = lookup_symbol(buf) ;

  EXIT_TRACE ;
  TRACE2("fin de T_B0_array_type::T_B0_array_type(%x), ref_type = %x",
		 this, ref_type) ;
}

T_B0_array_type::~T_B0_array_type()
{
}

//
//	}{ Classe T_B0_interval_type
//
//CT le 15.07.97 : ajout parametres abst_type et name_of_setof.
//ceci pour les cas bb : setof( atype(BB))
//abst_type est le type BB abstrait associe a BB et name_of_setof est bb.
//En effet on souhaite creer dans ce cas un type intervalle de nom bb
//faisant reference a un type abstrait B0 de nom BB.
T_B0_interval_type::T_B0_interval_type(T_setof_type *ref_type,
												T_B0_type *abst_type,
												T_ident *name_of_setof,
												T_item **adr_first,
												T_item **adr_last,
												T_item *new_father,
												T_betree *new_betree,
												T_lexem *new_ref_lexem)
  : T_B0_type(NODE_B0_INTERVAL_TYPE,
			  adr_first,
			  adr_last,
			  new_father,
			  new_betree,
			  new_ref_lexem)
{
  TRACE2("T_B0_interval_type::T_B0_interval_type(%x), ref_type=%x",this,ref_type) ;


  // Cast verifie par l'appelant
  T_base_type *base_type = (T_base_type *)(ref_type->get_base_type()) ;
  ASSERT(base_type->is_a_base_type() == TRUE) ;

  min = base_type->get_lower_bound() ;
  max = base_type->get_upper_bound() ;

  // Valeurs par defaut pour l'instant
  min_bound = NULL ;
  max_bound = NULL ;

  TRACE2("min %x %s", min, (min == NULL) ? "" : min->get_class_name()) ;
  TRACE2("max %x %s", max, (max == NULL) ? "" : max->get_class_name()) ;

  if (min != NULL)
	{
	  ASSERT(max != NULL) ; // par construction
	  min->link() ;
	  max->link() ;

	  // On recupere les valuations mises dans les T_bound s'il y a lieu
	  if (min->get_node_type() == NODE_BOUND)
		{
		  // On met les valuations dans min/max et les bound
		  // dans min_bound/max_bound
		  ASSERT(max->get_node_type() == NODE_BOUND) ;
		  min_bound = (T_bound *)min ;
		  min = min_bound->get_value() ;
		  if (min != NULL)
			{
			  min->link() ;
			}
		  max_bound = (T_bound *)max ;
		  max = max_bound->get_value() ;
		  if (max != NULL)
			{
			  max->link() ;
			}
		}
	}

  //CT le 11.07.97
  //pour le cas des sous-ens d'ens abstrait
  abstract_type = abst_type ;

  if ( abst_type != NULL )
	{
	  //cas bb : setof(atype(EE)) : ss-ens d'ens abstrait
	  //le nom n'est pas EE mais bb
	  name = name_of_setof ;
	  /* SL supprime le 18/12/97 - voir si pas de pb
	  min =  NULL;
	  max =  NULL;
	  min_bound = NULL ;
	  max_bound = NULL ;
	  */
	}
  else
	{
	  name = base_type->get_name() ;

	}//end   if ( abst_typ != NULL )

#ifdef B0C
  // Controle le champ name (potentiellement critique pour la traduction)
  check_if_only_concrete_data(name);
#endif // B0C

}

T_B0_interval_type::~T_B0_interval_type()
{
  TRACE1("T_B0_interval_type::~T_B0_interval_type", this) ;
}

//
//	}{ Classe T_B0_enumerated_type
//
T_B0_enumerated_type::T_B0_enumerated_type(T_enumerated_type *ref_type,
													T_item **adr_first,
													T_item **adr_last,
													T_item *new_father,
													T_betree *new_betree,
													T_lexem *new_ref_lexem)
  : T_B0_type(NODE_B0_ENUMERATED_TYPE,
			  adr_first,
			  adr_last,
			  new_father,
			  new_betree,
			  new_ref_lexem)
{
  TRACE2("T_B0_enum_type::T_B0_enum_type(%x),ref_type=%x",this,ref_type) ;

  ASSERT(ref_type != NULL) ;

  T_ident *def = ref_type->get_type_definition() ;

  ASSERT(def != NULL) ;
  ASSERT(def->get_ident_type() == ITYPE_ENUMERATED_SET);
  ASSERT(def->get_first_value()->get_node_type() == NODE_LITERAL_ENUMERATED_VALUE);
  ASSERT(def->get_last_value()->get_node_type() == NODE_LITERAL_ENUMERATED_VALUE) ;

  // Cast valide par construction, verifie par les assertions ci-dessus
  first_enumerated_value = (T_literal_enumerated_value *)def->get_first_value() ;
  last_enumerated_value = (T_literal_enumerated_value *)def->get_last_value() ;

  name = ref_type->get_type_definition() ;

  // Machine de definition de l'ensemble abstrait
  T_ident *cur = name ;
  while (cur->get_def() != NULL)
	{
	  cur = cur->get_def() ;
	}

  ref_machine = cur->get_ref_machine() ;

}

T_B0_enumerated_type::~T_B0_enumerated_type()
{
}

//
//	}{ Classe T_B0_abstract_type
//
T_B0_abstract_type::T_B0_abstract_type(T_abstract_type *ref_type,
												T_B0_interval_type *new_ref_interval,
												T_ident *new_ref_decla,
												T_item **adr_first,
												T_item **adr_last,
												T_item *new_father,
												T_betree *new_betree,
												T_lexem *new_ref_lexem)
  : T_B0_type(NODE_B0_ABSTRACT_TYPE,
			  adr_first,
			  adr_last,
			  new_father,
			  new_betree,
			  new_ref_lexem)
{
  TRACE2("T_B0_abs_type::T_B0_abs_type(%x),ref_type=%x",this,ref_type) ;

  ASSERT(ref_type != NULL) ;

  // Nom du type
  name = ref_type->get_name() ;
#ifdef B0C
  // Controle le champ name (potentiellement critique pour la traduction)
  check_if_only_concrete_data(name);
#endif // B0C

  // Nom de l'ensemble avec lequel on est value
  T_ident *ref_name = ref_type->get_valuation_ident() ;
  valuation_name = ref_name ;
#ifdef B0C
  // Controle le champ valuation_name (potentiellement critique pour la
  // traduction)
  check_if_only_concrete_data(valuation_name);
#endif // B0C

  // Machine de definition de l'ensemble abstrait
  ref_machine = ref_type->get_set()->get_ref_machine() ;

  // Dans le cas d'un intervalle, definition de cet intervalle
  // Inutilise maintenant : dans ce cas, on provoque la creation
  // d'une instance de T_B0_Interval_type
  ref_interval = NULL ;
  ref_interval = new_ref_interval ;

  // Dans le cas d'un autre ensemble abstrait, definition de cet ensemble
  ref_decla = new_ref_decla ;
  #ifdef B0C
  // Controle le champ ref_decla (potentiellement critique pour la traduction)
  check_if_only_concrete_data(ref_decla);
#endif // B0C
  if (ref_decla != NULL)
	{
	  ref_decla->link() ;
	}

}

T_B0_abstract_type::~T_B0_abstract_type()
{
  TRACE1("T_B0_abstract_type(%x)::~T_B0_abstract_type", this) ;
}

// Pour savoir si un type est un type "de base"
// toujours FALSE sauf pour les T_B0_base_type/T_B0_abstract_type
int T_B0_abstract_type::is_a_base_type(void)
{
  return TRUE ;
}

// Fonction auxiliaire qui renvoie la
// machine de definition d'un identificateur
static T_machine *find_machine_decla(T_ident *ident)
{
  if (ident == NULL)
	{
	  return NULL ;
	}
  T_ident *def = ident ;

  while (def->get_def() != NULL)
	{
	  def = def->get_def() ;
	}

  return def->get_ref_machine() ;
}

T_machine *T_B0_base_type::get_machine_def(void)
{
return find_machine_decla(name) ;
}

T_machine *T_B0_abstract_type::get_machine_def(void)
{
return find_machine_decla(name) ;
}

T_machine *T_B0_enumerated_type::get_machine_def(void)
{
return find_machine_decla(name) ;
}

T_machine *T_B0_interval_type::get_machine_def(void)
{
return find_machine_decla(name) ;
}

// Obtention de la machine de definition du type
// Non valide pour les T_B0_abstract_type. Pour ce type,
// appeler la methode recursivement sur les src_types puis
// sur le dst_type
T_machine *T_B0_type::get_machine_def(void)
{
  TRACE2("T_B0_type(%x:%s::get_machine_def)", this, get_class_name()) ;
  assert(FALSE) ;
  return NULL ; // Pour Visual-C++
}

//
//	}{ Classe T_B0_record_type
//
T_B0_record_type::T_B0_record_type(T_record_type *ref_type,
											T_ident *ref_ident,
											T_item **adr_first,
											T_item **adr_last,
											T_item *new_father,
											T_betree *new_betree,
											T_lexem *new_ref_lexem)
  : T_B0_type(NODE_B0_RECORD_TYPE,
			  adr_first,
			  adr_last,
			  new_father,
			  new_betree,
			  new_ref_lexem)
{
  TRACE2("T_B0_record_type::T_B0_record_type(%x), ref_type = %x", this, ref_type) ;
  ENTER_TRACE ;

  // On met le B_type a la main car dans le cas de records imbriques
  // le traducteur C++ a besoin du ref_type des types records "a
  // l'interieur". Or la boucle principale ne le met a jour que pour
  // le record "a l'exterieur"
  set_B_type(ref_type) ;

  first_label = last_label = NULL ;
  T_label_type *cur_label = ref_type->get_labels() ;

  while (cur_label != NULL)
	{
	  cur_label->make_B0_type(ref_ident,
							  FALSE,
							  (T_item **)&first_label,
							  (T_item **)&last_label) ;

	  cur_label = (T_label_type *)cur_label->get_next() ;
	}

  ref_machine = ref_ident->get_ref_machine() ;

  next_decla = NULL ;
  prev_decla = NULL ;

  char *buf = get_1Ko_string() ;
  int number = ref_machine->get_specification()->get_tmp2() + 1 ;
  sprintf(buf, "R_%d", number) ;
  ref_machine->get_specification()->set_tmp2(number) ;
  name = lookup_symbol(buf) ;

  EXIT_TRACE ;
  TRACE2("fin de T_B0_record_type::T_B0_record_type(%x), ref_type = %x",
		 this, ref_type) ;
}

T_B0_record_type::~T_B0_record_type(void)
{
  TRACE1("T_B0_record_type(%x)::~T_B0_record_type", this) ;
}

//
//	}{ Classe T_B0_label_type
//
T_B0_label_type::T_B0_label_type(T_label_type *ref_type,
										  T_ident *ref_ident,
										  T_item **adr_first,
										  T_item **adr_last,
										  T_item *new_father,
										  T_betree *new_betree,
										  T_lexem *new_ref_lexem)
  : T_B0_type(NODE_B0_LABEL_TYPE,
			  adr_first,
			  adr_last,
			  new_father,
			  new_betree,
			  new_ref_lexem)
{
  TRACE2("T_B0_label_type::T_B0_label_type(%x), ref_type = %x", this, ref_type) ;
  ENTER_TRACE ;

  name = ref_type->get_name() ;
  TRACE2("ici avant make_B0_type de ref_type=%x %s",
		 ref_type,
		 ref_type->make_type_name()->get_value()) ;
  type = ref_type->get_type()->make_B0_type(ref_ident,
											FALSE,
											NULL,
											NULL,
											// on force la creation
											// d'une copie privee
											TRUE) ;
  type->link() ;
  TRACE2("ici apres make_B0_type de ref_type=%x %s",
		 ref_type,
		 ref_type->make_type_name()->get_value()) ;

  EXIT_TRACE ;
  TRACE2("fin de T_B0_label_type::T_B0_label_type(%x), ref_type = %x",
		 this, ref_type) ;
}

T_B0_label_type::~T_B0_label_type()
{
  TRACE1("T_B0_label_type(%x)::~T_B0_label_type", this) ;
}

//
//	}{ Fin du fichier
//
