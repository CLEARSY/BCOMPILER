/******************************* CLEARSY **************************************
* Fichier : $Id: v_colli.cpp,v 2.0 2001-08-10 12:14:12 lv Exp $
* (C) 2008 CLEARSY
*
* Description :		Compilateur B
*					Analyse semantique : controles d'anti collisions
*
* Compilation : 	-DDEBUG_COLLI pour tracer les tests
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
RCS_ID("$Id: v_colli.cpp,v 1.28 2001-08-10 12:14:12 lv Exp $") ;

/* Includes systeme */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

/* Includes Composant Local */
#include "c_api.h"
#include "c_tstamp.h"

// Fonction auxiliaire d'ajout d'identifiant dans une liste
// avec controle de non-collision
// On gere une liste triee par ordre croissant de ptr name pour optimiser le
// traitement
// ^
// |------- A FAIRE !!! (pas immediat pour l'instant car on chaine des ident
// et pas des symbol)
// Prefix est le prefixe utilise, stocke dans le champ tmp
static void add_list_link(T_ident *new_item,
								   T_list_link **adr_first,
								   T_list_link **adr_last,
								   T_symbol *prefix = NULL)
{
#ifdef DEBUG_COLLI
  TRACE3("DEBUT add_list_link(%x:%s -> ref_glued = %x)",
		 new_item,
		 new_item->get_name()->get_value(),
		 new_item->get_ref_glued()) ;
  ENTER_TRACE ;
#endif
  T_list_link *cur = *adr_first ;
  T_list_link *prev = NULL ;
  size_t adr = (size_t)(new_item->get_name()) ;
  size_t cur_adr = 0 ;

  int encore = TRUE ;
  int found = FALSE ;

  while (encore == TRUE)
	{
	  if (cur == NULL)
		{
		  encore = FALSE ;
		}
	  else
		{
#ifdef DEBUG_COLLI
		  cur_adr = (size_t)(((T_ident *)cur->get_object())->get_name()) ;
		  TRACE6("cur %x cur->adr %x (%s) adr %x (%s) prefix %x",
				 cur,
				 cur_adr,
				 ((T_symbol *)cur_adr)->get_value(),
				 adr,
				 ((T_symbol *)adr)->get_value(),
				 cur->get_tmp()) ;
#endif
		  if ( ( (cur_adr = (size_t)(((T_ident *)cur->get_object())->get_name()))
				 == adr)
			   && (cur->get_tmp() == prefix) )
			{
			  encore = FALSE ;
			  found = TRUE ;
			}
		  else
			{
			  prev = cur ;
			  cur = (T_list_link *)cur->get_next() ;
			}
		}
	}

#ifdef DEBUG_COLLI
  if (cur != NULL)
	{
	  TRACE6("SORTIE cur %x cur->adr %x (%s) adr %x (%s) prefix %x",
			 cur,
			 cur_adr,
			 ((T_symbol *)cur_adr)->get_value(),
			 adr,
			 ((T_symbol *)adr)->get_value(),
			 cur->get_tmp()) ;
	}
  else
	{
	  TRACE0("SORTIE cur NULL") ;
	}
#endif

  if (found == TRUE)
	{
	  // Il y a un doublon collision, sauf s'il y a collage
	  // (lien par le champ ref_glued) ou inclusion
	  // (lien par le champ def)

	  T_ident *first = new_item ;
	  T_ident *second = (T_ident *)(cur->get_object()) ;

	  // Il faut aller au bout des def
	  T_ident *first_def = first ;
	  while (first_def->get_def() != NULL)
		{
		  first_def = first_def->get_def() ;
		}
	  T_ident *second_def = second ;
	  while (second_def->get_def() != NULL)
		{
		  second_def = second_def->get_def() ;
		}

	  TRACE5("first %x:%s -> ref_glued %x/ first_def %x -> ref_glued %x",
			 first,
			 first->get_name()->get_value(),
			 first->get_ref_glued(),
			 first_def,
			 first_def->get_ref_glued()) ;
	  TRACE5("second %x:%s -> ref_glued %x/ second_def %x -> ref_glued %x",
			 second,
			 second->get_name()->get_value(),
			 second->get_ref_glued(),
			 second_def,
			 second_def->get_ref_glued()) ;

	  // Utilisation de l'anneau de collage (glued_ring) � la place
	  if (first->is_in_glued_ring(second) == TRUE)
		{
		  // Pas de doublon car les identificateurs sont colles entre eux
		  TRACE6("GLUE :pas doublon entre %s:%x:%s et %s:%x:%s car ils sont colles",
				 first->get_ref_machine()
				 ->get_machine_name()->get_name()->get_value(),
				 first,
				 first->get_name()->get_value(),
				 second->get_ref_machine()
				 ->get_machine_name()->get_name()->get_value(),
				 second,
				 second->get_name()->get_value()) ;
		}
	  else
		{
		  // LV le 09/11/2000:
		  //
		  //  � terme, il faudrait permettant d'acc�der � chacun des
		  //  deux identificateurs depuis le Betree en cours
		  //  d'analyse.  Pour l'instant, on se contente de donner les
		  //  deux d�finitions in fine.
		  //
		  // cf. DEVOL 2649.

		  // Si l'un des deux est le betree en cours d'analyse, on le met en premier
		  // Le seul cas ou l'on veut mettre second avant first est si
		  // second appartient au betree courant, et pas first
		  T_betree *cur = get_betree_manager()->get_betrees() ;
		  if ( (first_def->get_betree() != cur) && (second_def->get_betree() == cur) )
			{
			  T_ident *tmp = first_def ;
			  first_def = second_def ;
			  second_def = tmp ;
			}
		  double_identifier_error(first_def, second_def) ;
		  TRACE6("ICI : doublon entre %s:%x:%s et %s:%x:%s",
				 first->get_ref_machine()
				 ->get_machine_name()->get_name()->get_value(),
				 first,
				 first->get_name()->get_value(),
				 second->get_ref_machine()
				 ->get_machine_name()->get_name()->get_value(),
				 second,
				 second->get_name()->get_value()) ;
		}
	}

  // On chaine !
  T_list_link *new_link = new T_list_link(new_item,
										  LINK_OTHER,
										  prev,
										  (T_item **)adr_first,
										  (T_item **)adr_last) ;

  new_link->set_tmp(prefix) ;
  // was  new_link->insert_after(prev, (T_item **)adr_first, (T_item **)adr_last) ;

#ifdef DEBUG_COLLI
  EXIT_TRACE ;
  TRACE1("FIN add_list_link(%s)", new_item->get_name()->get_value()) ;

  TRACE0("DEBUT DUMP LISTE") ;
  T_list_link *l = *adr_first ;
  while (l != NULL)
	{
	  TRACE4("l %x prev %x, next %x symb %s",
			 l,
			 l->get_prev(),
			 l->get_next(),
			 ((T_ident *)(l->get_object()))->get_name()->get_value()) ;
	  l = (T_list_link *)l->get_next() ;
	}
  TRACE0("FIN DUMP LISTE") ;
#endif
}


// Fonction auxiliaire qui dit si l'insertion des donnees
// du composant a deja ete fait en se basant sur le nom
// de machine (c'est le cas de i1.M et i2.M : on n'insere
// qu'une seule fois les donnees de M)
static int check_time(T_machine *machine)
{
  ASSERT(machine != NULL) ;
  T_symbol *sym = machine->get_machine_name()->get_name() ;
  TRACE3("check_time(%x:%s:%d)", machine, sym->get_value(), sym->get_tmp2()) ;

  if ((size_t)sym->get_tmp2() == get_timestamp())
	{
	  TRACE0("deja fait !") ;
	  return TRUE ;
	}

  TRACE0("pas encore fait") ;
  sym->set_tmp2(get_timestamp()) ;
  return FALSE ;
}

//GP 28/12/98
//Fonction auxiliaire qui ajoute � la liste les operations propres de la
//machine: c'est � dire les operations de la clause OPERATIONS
static void add_machine_operations(T_machine *root,
											T_list_link **adr_first,
											T_list_link **adr_last)
{
  TRACE2("add_machine_operations(%x:%s)",
		 root,
		 root->get_machine_name()->get_name()->get_value()) ;

  T_op* cur_op = root->get_operations() ;
  while(cur_op != NULL)
	{
	  add_list_link(cur_op->get_name(), adr_first, adr_last, NULL) ;
	  cur_op = (T_op*)cur_op->get_next() ;
	}
}

//GP 28/12/98
//Fonction auxiliaire qui ajoute � la liste, les operations d'une machine
//requise: c'est � dire toutes les operations locales+promues
static void add_requiered_machine_operations(T_machine *root,
											T_list_link **adr_first,
											T_list_link **adr_last)
{
  TRACE2("add_requiered_machine_operations(%x:%s)",
		 root,
		 root->get_machine_name()->get_name()->get_value()) ;

  T_symbol *prefix = (root->get_context() == NULL) ?
	(T_symbol *)NULL : root->get_context()->get_instance_name() ;

  T_list_link* cur_obj = root->get_op_list() ;

  while(cur_obj != NULL)
	{
	  T_generic_op* cur_op = (T_generic_op*)cur_obj->get_object() ;
	  ASSERT(cur_op != NULL) ;
	  add_list_link(cur_op->get_name(), adr_first, adr_last, prefix) ;
	  cur_obj = (T_list_link*)cur_obj->get_next() ;
	}
}

//GP 28/12/98
//Fonction auxiliaire qui ajoute � la liste, les operations propres d'un
//raffinement ou d'une implantation: c'est � dire toutes les operations
//de la specification moins les operations promues dans le raffinement
//resp l'abstraction.
static void add_imp_or_raf_operations(T_machine *root,
											   T_list_link **adr_first,
											   T_list_link **adr_last)
{
  TRACE2(" add_imp_or_raf_operations (%x:%s)",
		 root,
		 root->get_machine_name()->get_name()->get_value()) ;

  T_machine * specification = root->get_ref_specification() ;
  ASSERT(specification != NULL) ;

  T_list_link* cur_obj = specification->get_op_list() ;

  while(cur_obj != NULL)
	{
	  T_generic_op* cur_op = (T_generic_op*)cur_obj->get_object() ;
	  ASSERT(cur_op != NULL) ;

	  T_generic_op* local_op = root->find_op(cur_op->get_op_name()) ;
	  if (local_op == NULL)
		{
		  //L'operation n'est pas raffinee, mais c'est une operation
		  //propre au composant
		  add_list_link(cur_op->get_name(), adr_first, adr_last, NULL) ;
		}
	  else
		{
		  if (local_op->get_node_type() == NODE_OPERATION)
			{
			  //C'est une operation propre au composant
			  add_list_link(local_op->get_name(), adr_first, adr_last, NULL) ;
			}
		  else
			{
			  //Dans ce cas l'operation est une NODE_USED_OP et elle
			  //ne fait pas partie des operations propres au composant
			}
		}

	  cur_obj = (T_list_link*)cur_obj->get_next() ;
	}
  //En plus il faut verifier que le composant ne redefini pas deux fois
  //la meme operation
  T_op* cur_op = root->get_operations() ;
  while(cur_op != NULL)
	{
	  T_op* check_op=(T_op*)cur_op->get_next() ;
	  while(check_op != NULL)
		{
		  T_symbol* check_value = check_op->get_name()->get_name() ;
		  T_symbol* cur_value = cur_op->get_name()->get_name() ;
		  if(check_value->compare(cur_value))
			{
			  double_identifier_error(check_op->get_name(),cur_op->get_name());
			}
		  check_op=(T_op*)check_op->get_next() ;
		}
	  cur_op = (T_op*)cur_op->get_next() ;
	}
}



// Fonction auxiliaire qui ajoute les ensembles abstraits et enumeres,
// les valeurs enumerees, les constantes, les noms des variable et
// des operations d'une machine
// is_l_raf et is_l_imp permettent de savoir si l'on est dans la
// construiction de l_raf ou l_imp
static void basic_add_machine(T_machine *root,
									   int is_l_raf_or_l_imp,
									   T_list_link **adr_first,
									   T_list_link **adr_last)
{
  T_symbol *prefix = (root->get_context() == NULL) ?
	(T_symbol *)NULL : root->get_context()->get_instance_name() ;
  TRACE2("basic_add_machine(%s prefix=%s)",
		 root->get_machine_name()->get_name()->get_value(),
		 (prefix == NULL) ? "null" : prefix->get_value()) ;
  // Noms des ensembles abstraits et des ensembles enumeres de Mch
  // i.e. noms des sets
  // Plus :: Noms des elements enumeres de Mch
  ASSERT(root) ;
  T_ident *cur_set = root->get_sets() ;
  TRACE2("Ajout des SETS de %x:%s",
		 root,
		 root->get_machine_name()->get_name()->get_value()) ;
  ENTER_TRACE ;
  while (cur_set != NULL)
	{
	  TRACE4("cur_set %x(%s) father %x ref_glued %x",
			 cur_set,
			 cur_set->get_name()->get_value(),
			 cur_set->get_father(),
			 cur_set->get_ref_glued()) ;
	  TRACE8("cur_set %x(%s) father %x ref_glued %x ref_machine %x(%s) root %x(%s)",
			 cur_set,
			 cur_set->get_name()->get_value(),
			 cur_set->get_father(),
			 cur_set->get_ref_glued(),
			 cur_set->get_ref_machine(),
			 cur_set->get_ref_machine()->get_machine_name()->
			 	get_name()->get_value(),
			 root,
			 root->get_machine_name()->get_name()->get_value()) ;
	  if (cur_set->get_ref_glued() == NULL)
		{
		  // Test obligatoire pour les ensembles colles
		  add_list_link(cur_set, adr_first, adr_last) ;

		  if (cur_set->get_values() != NULL)
			{
			  // On ajoute les valeurs enumerees de cet ensemble
			  // Le cast de T_expr en T_ident est valide pour get_values
			  // car on est dans le cas d'un ensemble enumere, et pas
			  // d'une constante
			  T_ident *cur_value = (T_ident *)cur_set->get_values() ;
			  while (cur_value != NULL)
				{
				  TRACE2("ici cur_value %x:%s",
						 cur_value, cur_value->get_class_name());
				  ASSERT(cur_value->get_node_type() ==
						 NODE_LITERAL_ENUMERATED_VALUE) ;
				  add_list_link(cur_value, adr_first, adr_last) ;
				  cur_value = (T_ident *)cur_value->get_next() ;
				}
			}
		}

	  cur_set = (T_ident *)cur_set->get_next() ;
	}

  // Noms des constantes de Mch
  EXIT_TRACE ;
  TRACE2("Ajout des constantes concretes de %x:%s",
		 root,
		 root->get_machine_name()->get_name()->get_value()) ;
  ENTER_TRACE ;
  T_ident *cur_constant = root->get_concrete_constants() ;
  while (cur_constant != NULL)
	{
	  if (cur_constant->get_ref_glued() == NULL)
		{
		  TRACE1("Ajout de la constante concrete: %s",
				 cur_constant->get_name()->get_value() ) ;
		  // Test obligatoire pour les constantes collees
		  add_list_link(cur_constant, adr_first, adr_last) ;
		}
	  cur_constant = (T_ident *)cur_constant->get_next() ;
	}

  cur_constant = root->get_abstract_constants() ;
  EXIT_TRACE ;
  TRACE2("Ajout des constantes abstraites de %x:%s",
		 root,
		 root->get_machine_name()->get_name()->get_value()) ;
  ENTER_TRACE ;
  while (cur_constant != NULL)
	{
	  if (cur_constant->get_ref_glued() == NULL)
		{
		  TRACE1("Ajout de la constante abstraite: %s",
				 cur_constant->get_name()->get_value() ) ;
		  // Test obligatoire pour les constantes collees
		  add_list_link(cur_constant, adr_first, adr_last) ;
		}
	  cur_constant = (T_ident *)cur_constant->get_next() ;
	}


  // Noms des variables de Mch
  EXIT_TRACE ;
  TRACE2("Ajout des variables concretes de %x:%s",
		 root,
		 root->get_machine_name()->get_name()->get_value()) ;
  ENTER_TRACE ;
  T_ident *cur_variable = root->get_concrete_variables() ;
  while (cur_variable != NULL)
	{
	  if (cur_variable->get_ref_glued() == NULL)
		{
		  // Test obligatoire pour les variables collees
		  add_list_link(cur_variable, adr_first, adr_last, prefix) ;
		}
	  cur_variable = (T_ident *)cur_variable->get_next() ;
	}

  EXIT_TRACE ;
  TRACE2("Ajout des variables abstraites de %x:%s",
		 root,
		 root->get_machine_name()->get_name()->get_value()) ;
  ENTER_TRACE ;
  cur_variable = root->get_abstract_variables() ;
  while (cur_variable != NULL)
	{
	  if (cur_variable->get_ref_glued() == NULL)
		{
		  // Test obligatoire pour les variables collees
		  add_list_link(cur_variable, adr_first, adr_last, prefix) ;
		}
	  cur_variable = (T_ident *)cur_variable->get_next() ;
	}

  // Noms des operations de Mch
  EXIT_TRACE ;

  //GP 28/12/98: l'ajout des operation se fait maintenant par les
  //fonctions add_machine_operations, add_requiered_machine_operation
  // et add_raf_or_imp_operations, d'ou le #if 0
#if 0
  TRACE2("Ajout des operations de %x:%s",
		 root,
		 root->get_machine_name()->get_name()->get_value()) ;
  ENTER_TRACE ;
  T_op *cur_op = root->get_operations() ;
  while (cur_op != NULL)
	{
	  add_list_link(cur_op->get_name(), adr_first, adr_last, prefix) ;
	  cur_op = (T_op *)cur_op->get_next() ;
	}

  EXIT_TRACE ;
#endif

  if (is_l_raf_or_l_imp)
	{
	  //GP 28/12/98 nom de la specification
	  T_machine * specification = root->get_ref_specification() ;
	  ASSERT(specification != NULL) ;
	  add_list_link(specification->get_machine_name(),
					adr_first,
					adr_last,
					NULL) ;

	  // Nom des constantes abstraites et des variables abstraites de
	  // l'abstraction de raf (resp. imp) disparaissant dans raf (resp. imp)
	  if (root->get_ref_abstraction() != NULL)
		// normalement c'est toujours vrai
		// le test est la pour pouvoir continuer en cas d'erreur sans crash
		{
		  // Constantes ...
		  T_ident *cur_const ;
		  cur_const = root->get_ref_abstraction()->get_abstract_constants() ;

		  while (cur_const != NULL)
			{
			  // On recherche les variables de la spec qui sont 'collees'
			  // i.e. on les redonne dans la clause ABSTRACT_CONSTANTS
			  // Il faut alors mettre a jour leur DEF
			  // Sinon, elles disparaissent. C'est important pour
			  // les controles de collision
			  T_ident *root_const = root->find_constant(cur_const->get_name()) ;

			  if (root_const == NULL)
				{
				  TRACE1("la constante abstraite \"%s\" de l'abstraction disparait",
						 cur_const->get_name()->get_value()) ;
				  add_list_link(cur_const, adr_first, adr_last) ;
				}
			  else
				{
				  // Mise a jour du def ...
				  root_const->set_def(cur_const) ;
				}
			  cur_const = (T_ident *)cur_const->get_next() ;
			}

		  // ... et variables
		  T_ident *cur_var = root->get_ref_abstraction()->get_abstract_variables() ;

		  while (cur_var != NULL)
			{
			  // On recherche les variables de la spec qui sont 'collees'
			  // i.e. on les redonne dans la clause ABSTRACT_VARIABLES
			  // Il faut alors mettre a jour leur DEF
			  // Sinon, elles disparaissent. C'est important pour
			  // les controles de collision
			  T_ident *root_var = root->find_variable(cur_var->get_name()) ;
			  if (root_var == NULL)
				{
				  TRACE1("la variable abstraite \"%s\" de l'abstraction disparait",
						 cur_var->get_name()->get_value()) ;
				  add_list_link(cur_var, adr_first, adr_last) ;
				}
			  else
				{
				  // Mise a jour du def ...
				  root_var->set_def(cur_var) ;
				  TRACE8("implementation de %s:%d:%d \"%s\" detectee avec %s:%d:%d \"%s\"\n",
						 root_var->get_ref_lexem()->get_file_name()->get_value(),
						 root_var->get_ref_lexem()->get_file_line(),
						 root_var->get_ref_lexem()->get_file_column(),
						 root_var->get_name()->get_value(),
						 cur_var->get_ref_lexem()->get_file_name()->get_value(),
						 cur_var->get_ref_lexem()->get_file_line(),
						 cur_var->get_ref_lexem()->get_file_column(),
						 cur_var->get_name()->get_value()) ;

				}
			  cur_var = (T_ident *)cur_var->get_next() ;
			}
		}
	}
}


// Fonction auxiliaire qui ajoute L_MCH dans une liste
static void add_L_MCH(T_betree *betree,
							   T_list_link **adr_first,
							   T_list_link **adr_last)
{
  // CTRL Ref: ELMCH
  TRACE3("add_L_MCH(%x, %x, %x)", betree, adr_first, adr_last) ;
  T_machine *root = betree->get_root() ;

  // Nom de la machine
  add_list_link(root->get_machine_name(), adr_first, adr_last) ;

  //#if 0
  // 03/11/98 SL, GP : on enleve le #if 0 car on ne sait
  // pas a quoi il sert, et il empeche la detection d'erreurs
  // A FAIRE : enlever le #if 0 a la fin de la validation
  //

  // Noms des parametres de Mch
  T_ident *cur_param = root->get_params() ;
  TRACE1("parametres formels %x", cur_param) ;
  while (cur_param != NULL)
	{
	  add_list_link(cur_param, adr_first, adr_last) ;
	  cur_param = (T_ident *)cur_param->get_next() ;
	}
  //#endif

  // Le reste
  basic_add_machine(root, FALSE, adr_first, adr_last) ;
  //GP 28/12/98
  add_machine_operations(root,  adr_first, adr_last) ;
}

// Fonction auxiliaire qui ajoute L_RAF dans une liste
static void add_L_RAF(T_betree *betree,
							   T_list_link **adr_first,
							   T_list_link **adr_last)
{
  // CTRL Ref : ELRAF
  TRACE3("add_L_RAF(%x, %x, %x)", betree, adr_first, adr_last) ;
  T_machine *root = betree->get_root() ;

  // Nom de la machine
  add_list_link(root->get_machine_name(), adr_first, adr_last) ;

  //#if 0
  // 03/11/98 SL, GP : on enleve le #if 0 car on ne sait
  // pas a quoi il sert, et il empeche la detection d'erreurs
  // A FAIRE : enlever le #if 0 a la fin de la validation
  //

  // Noms des parametres de Mch
  T_ident *cur_param = root->get_params() ;
  while (cur_param != NULL)
	{
	  add_list_link(cur_param, adr_first, adr_last) ;
	  cur_param = (T_ident *)cur_param->get_next() ;
	}
  //#endif

  // Le reste
  basic_add_machine(root, TRUE, adr_first, adr_last) ;
  //GP 28/12/98
  add_imp_or_raf_operations(root, adr_first, adr_last) ;

}

// Fonction auxiliaire qui ajoute L_IMP dans une liste
// A FAIRE : finir et corriger
static void add_L_IMP(T_betree *betree,
							   T_list_link **adr_first,
							   T_list_link **adr_last)
{
  //CTRL Ref: ELIMP
  TRACE3("add_L_IMP(%x, %x, %x)", betree, adr_first, adr_last) ;
  T_machine *root = betree->get_root() ;

  // Nom de la machine
  add_list_link(root->get_machine_name(), adr_first, adr_last) ;

  //#if 0
  // 03/11/98 SL, GP : on enleve le #if 0 car on ne sait
  // pas a quoi il sert, et il empeche la detection d'erreurs
  // A FAIRE : enlever le #if 0 a la fin de la validation
  //

  // Noms des parametres de Mch
  T_ident *cur_param = root->get_params() ;
  while (cur_param != NULL)
	{
	  add_list_link(cur_param, adr_first, adr_last) ;
	  cur_param = (T_ident *)cur_param->get_next() ;
	}
  //#endif

  // Le reste
  basic_add_machine(root, TRUE, adr_first, adr_last) ;
  //GP 28/12/98
  add_imp_or_raf_operations(root, adr_first, adr_last) ;
}

// Fonction auxiliaire qui ajoute L_SEES dans une liste
static void add_L_SEES(T_betree *betree,
								T_list_link **adr_first,
								T_list_link **adr_last)
{
  //CTRL Ref: ELSEES
  TRACE3("add_L_SEES(%x, %x, %x)", betree, adr_first, adr_last) ;

  T_used_machine *cur_sees = betree->get_root()->get_sees() ;

  while (cur_sees != NULL)
	{
	  ASSERT(cur_sees->get_node_type() == NODE_USED_MACHINE) ;
	  TRACE4("cur_sees(%x,%s)->get_ref_machine(%x,%s)",
			 cur_sees,
			 cur_sees->get_name()->get_name()->get_value(),
			 cur_sees->get_ref_machine(),
			 (cur_sees->get_ref_machine() == NULL)
			 ? "null"
			 : cur_sees->get_ref_machine()->get_machine_name()
			 ->get_name()->get_value()) ;

	  T_machine *cur_root = cur_sees->get_ref_machine() ;

	  if (check_time(cur_root) == TRUE)
		{
		  TRACE0("insertion deja faite pour ce composant") ;
		}
	  else
		{
		  // Nom de la machine
		  add_list_link(cur_sees->get_name(), adr_first, adr_last) ;

		  // Le reste
		  basic_add_machine(cur_root, FALSE, adr_first, adr_last) ;
		  //GP 28/12/98
		  add_requiered_machine_operations(cur_root, adr_first, adr_last) ;
		}

	  // La suite
	  cur_sees = (T_used_machine *)cur_sees->get_next() ;
	}
}

// Fonction auxiliaire qui ajoute L_INC dans une liste
static void add_L_INC(T_betree *betree,
							   T_list_link **adr_first,
							   T_list_link **adr_last)
{
  //CTRL Ref: ELINC
  TRACE3("add_L_INC(%x, %x, %x)", betree, adr_first, adr_last) ;

  T_used_machine *cur_inc = betree->get_root()->get_includes() ;
  //GP 24/11/98, pour faire le travail avec les extends
  // Debut 04/11/98 GP, SL prendre en compte les machines EXTENDS
  int extends_done = FALSE ;
  if ( (cur_inc == NULL) && (extends_done == FALSE) )
	{
	  // On passe a la clause EXTENDS
	  extends_done = TRUE ;
	  cur_inc = betree->get_root()->get_extends() ;
	}
  //Fin GP 24/11/98

  while (cur_inc != NULL)
	{
	  T_machine *cur_root = cur_inc->get_ref_machine() ;


	  if (check_time(cur_root) == TRUE)
		{
		  TRACE0("insertion deja faite pour ce composant") ;
		}
	  else
		{
		  // Nom de la machine
		  add_list_link(cur_inc->get_name(), adr_first, adr_last) ;

		  // GP 25/11/98 Conformement a REF 7.25
		  //Les parametres des machines incluses
		  //ne font pas partis du test d'anticollision
#if 0
		  // Noms des parametres de Mch
		  T_ident *cur_param = cur_root->get_params() ;
		  while (cur_param != NULL)
			{
			  add_list_link(cur_param, adr_first, adr_last) ;
			  cur_param = (T_ident *)cur_param->get_next() ;
			}
#endif

		  // Le reste
		  basic_add_machine(cur_root, FALSE, adr_first, adr_last) ;
		  //GP 28/12/98
		  add_requiered_machine_operations(cur_root, adr_first, adr_last) ;
		}

	  // La suite
	  cur_inc = (T_used_machine *)cur_inc->get_next() ;

	  //GP 24/11/98, pour faire le travail avec les extends
	  // Debut 04/11/98 GP, SL prendre en compte les machines EXTENDS
 	  if ( (cur_inc == NULL) && (extends_done == FALSE) )
		{
		  // On passe a la clause EXTENDS
		  extends_done = TRUE ;
		  cur_inc = betree->get_root()->get_extends() ;
		}
	  //Fin GP 24/11/98
	}
}

// Fonction auxiliaire qui ajoute L_IMPORTS dans une liste
static void add_L_IMPORTS(T_betree *betree,
								   T_list_link **adr_first,
								   T_list_link **adr_last)
{
  //CTRL Ref: ELIMPRTS
  TRACE3("add_L_IMPORTS(%x, %x, %x)", betree, adr_first, adr_last) ;

  T_used_machine *cur_imports = betree->get_root()->get_imports() ;
  int extends_done = FALSE ;

  // Debut 04/11/98 GP, SL prendre en compte les machines EXTENDS
  if ( (cur_imports == NULL) && (extends_done == FALSE) )
	{
	  // On passe a la clause EXTENDS
	  extends_done = TRUE ;
	  cur_imports = betree->get_root()->get_extends() ;
	}

  while (cur_imports != NULL)
	{
	  T_machine *cur_root = cur_imports->get_ref_machine() ;

	  if (check_time(cur_root) == TRUE)
		{
		  TRACE0("insertion deja faite pour ce composant") ;
		}
	  else
		{
		  // Nom de la machine
		  add_list_link(cur_imports->get_name(), adr_first, adr_last) ;

		  // GP 25/11/98 Conformement a REF 7.25
		  //Les parametres des machines incluses
		  //ne font pas partis du test d'anticollision
#if 0
		  // Noms des parametres de Mch
		  T_ident *cur_param = cur_root->get_params() ;
		  while (cur_param != NULL)
			{
			  add_list_link(cur_param, adr_first, adr_last) ;
			  cur_param = (T_ident *)cur_param->get_next() ;
			}
#endif

		  // Le reste
		  basic_add_machine(cur_root, FALSE, adr_first, adr_last) ;
		  //GP 28/12/98
		  add_requiered_machine_operations(cur_root, adr_first, adr_last) ;
		}

	  // La suite
	  cur_imports = (T_used_machine *)cur_imports->get_next() ;

	  // Debut 04/11/98 GP, SL prendre en compte les machines EXTENDS
	  if ( (cur_imports == NULL) && (extends_done == FALSE) )
		{
		  // On passe a la clause EXTENDS
		  extends_done = TRUE ;
		  cur_imports = betree->get_root()->get_extends() ;
		}
	  // Fin 04/11/98
	}
}

// Fonction auxiliaire qui ajoute L_USES dans une liste
static void add_L_USES(T_betree *betree,
								T_list_link **adr_first,
								T_list_link **adr_last)
{
  //CTRL Ref: ELUSES
  TRACE3("add_L_USES(%x, %x, %x)", betree, adr_first, adr_last) ;

  T_used_machine *cur_uses = betree->get_root()->get_uses() ;

  while (cur_uses != NULL)
	{
	  T_machine *cur_root = cur_uses->get_ref_machine() ;

	  if (check_time(cur_root) == TRUE)
		{
		  TRACE0("insertion deja faite pour ce composant") ;
		}
	  else
		{
		  // Nom de la machine
		  add_list_link(cur_uses->get_name(), adr_first, adr_last) ;

		  //GP: 24/11/98 conformement a REF 7.25 L_USES
		  //Pb: il faudrait metre le prefixe de renommage
		  T_ident *cur_param = cur_root->get_params() ;
		  while (cur_param != NULL)
			{
			  add_list_link(cur_param, adr_first, adr_last) ;
			  cur_param = (T_ident *)cur_param->get_next() ;
			}
		  //Fin GP 24/11/98

		  // Le reste
		  basic_add_machine(cur_root, FALSE, adr_first, adr_last) ;
		  //GP 28/12/98
		  //Conformement � LUSES les operations de machines USES
		  //ne font pas parties de la liste d'anticollision.
		}

	  // La suite
	  cur_uses = (T_used_machine *)cur_uses->get_next() ;
	}
}

// Analyse d'anticollision
T_betree *collision_analysis(T_betree *betree)
{
  TRACE2("DEBUT COLLISION_ANALYSIS %s (%x)",
		 betree->get_betree_name()->get_value(),
		 betree) ;
  ENTER_TRACE ; ENTER_TRACE ;

  // On cree la liste du composant, en verifiant au fur et a mesure
  // qu'il n'y a pas de doublon

  // On gere une liste de machines deja inserees.
  // En cas de renommage, on ne fait le controle que sur l'une des instances
  // Cela suffit car les prefixes de renommage sont uniques
  next_timestamp() ;

  // Initialisation de la liste
  T_list_link *first_list = NULL ;
  T_list_link *last_list = NULL ;

  switch(betree->get_root()->get_machine_type())
	{
	case MTYPE_SPECIFICATION :
	  {
		// Ajout de L_MCH, L_SEES, L_INC, L_USES
		add_L_MCH(betree, &first_list, &last_list) ;
		add_L_INC(betree, &first_list, &last_list) ;
		add_L_SEES(betree, &first_list, &last_list) ;
		add_L_USES(betree, &first_list, &last_list) ;
		break ;
	  }
	case MTYPE_SYSTEM :
	  {
		// Ajout de L_MCH, L_SEES
		add_L_MCH(betree, &first_list, &last_list) ;
		add_L_SEES(betree, &first_list, &last_list) ;
		break ;
	  }

	case MTYPE_REFINEMENT :
	  {
		// Ajout de L_RAF, L_SEES, L_INC
		add_L_RAF(betree, &first_list, &last_list) ;
		add_L_INC(betree, &first_list, &last_list) ;
		add_L_SEES(betree, &first_list, &last_list) ;
		break ;
	  }

	case MTYPE_IMPLEMENTATION :
	  {
		// Ajout de L_IMP, L_IMPORTS, L_SEES
		add_L_IMP(betree, &first_list, &last_list) ;
		add_L_IMPORTS(betree, &first_list, &last_list) ;
		add_L_SEES(betree, &first_list, &last_list) ;

		// Il faut verifier qu'aucune operation locale n'a le meme nom
		// qu'une operation de la specification
		// CTRL Ref : SEMLOC 2
		T_machine *implementation = betree->get_root() ;
		T_machine *specification = implementation->get_specification() ;

		T_op *cur_local_op = implementation->get_local_operations() ;

		while (cur_local_op != NULL)
		  {
			T_generic_op *genop ;
			genop = specification->find_op(cur_local_op->get_op_name()) ;

			if (genop != NULL)
			  {
				double_identifier_error(cur_local_op->get_name(),
										genop->get_name()) ;
			  }

			cur_local_op = (T_op *)cur_local_op->get_next() ;
		  }

        // Verification de l'unicit� des sp�cifications des op�rations
        // locales dans la clause LOCAL_OPERATION.
        cur_local_op = implementation->get_local_operations() ;
		while (cur_local_op != NULL)
		  {
			T_op *cur_local_op2 = (T_op *)cur_local_op->get_next();
			while (cur_local_op2 != NULL)
			  {
				if (cur_local_op2->get_op_name()->
					compare(cur_local_op->get_op_name()))
				  {
					double_identifier_error(cur_local_op->get_name(),
											cur_local_op2->get_name());
				  }
				cur_local_op2 = (T_op *)cur_local_op2->get_next();
			  }

			cur_local_op = (T_op *)cur_local_op->get_next() ;
		  }
		break ;
	  }
	  // Pas de default pour que le compilateur nous previenne si l'on
	  // a oublie un cas
	}

  EXIT_TRACE ; EXIT_TRACE ;
  TRACE2(" FIN  COLLISION_ANALYSIS %s (%x)",
		 betree->get_betree_name()->get_value(),
		 betree) ;
  return betree ;
}
