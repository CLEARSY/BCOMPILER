/******************************* CLEARSY **************************************
* Fichier : $Id: c_bmana.cpp,v 2.0 2000-02-25 17:21:28 lv Exp $
* (C) 2008 CLEARSY
*
* Description :		Compilateur B
*					Gestionnaire de Betree
*
* Compilations :	-DDEBUG_BMANA pour tracer les recherches de machines
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
RCS_ID("$Id: c_bmana.cpp,v 1.24 2000-02-25 17:21:28 lv Exp $") ;

/* Includes systeme */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

/* Includes Composant Local */
#include "c_api.h"

// Gestionnaire de Betree
static T_betree_manager *manager_sop = NULL ;

T_betree_manager *get_betree_manager(void)
{
  if (manager_sop == NULL)
	{
	  manager_sop = new T_betree_manager() ;
	}

return manager_sop ;
}

// Reset gestionnaire de betree
void reset_betree_manager(void)
{
  manager_sop = NULL ;
}

// Constructeur
T_betree_manager::T_betree_manager(void)
: T_object(NODE_BETREE_MANAGER)
{
TRACE1("T_betree_manager::T_betree_manager(%x)", this) ;
first_betree = NULL ;
last_betree = NULL ;
}

// Destructeur
T_betree_manager::~T_betree_manager(void)
{
TRACE1("T_betree_manager::~T_betree_manager(%x)", this) ;
}

// Ajout d'un betree
void T_betree_manager::add_betree(T_betree *new_betree)
{
TRACE3("T_betree_manager(%x)::add_betree(%x:%s)",
	   this, new_betree, new_betree->get_file_name()->get_value());

#ifdef DEBUG_BMANA
TRACE0("DEBUT DUMP AVANT ADD") ;
T_betree *cur = first_betree ;
while (cur != NULL)
  {
	TRACE4("%x <-- %x:%s --> %x",
		   cur->get_prev_betree(),
		   cur,
		   cur->get_file_name()->get_value(),
		   cur->get_next_betree()) ;
	cur = cur->get_next_betree() ;
  }
TRACE0(" FIN  DUMP AVANT ADD") ;
#endif

if (last_betree == NULL)
	{
	first_betree = new_betree ;
	new_betree->set_prev_betree(NULL) ;
	}
else
	{
	// Chainage en queue
	last_betree->set_next_betree(new_betree) ;
	new_betree->set_prev_betree(last_betree) ;
	}

new_betree->set_next_betree(NULL) ;
last_betree = new_betree ;

#ifdef DEBUG_BMANA
TRACE0("DEBUT DUMP APRES ADD") ;
cur = first_betree ;
while (cur != NULL)
  {
	TRACE4("%x <-- %x:%s --> %x",
		   cur->get_prev_betree(),
		   cur,
		   cur->get_file_name()->get_value(),
		   cur->get_next_betree()) ;
	cur = cur->get_next_betree() ;
  }
 TRACE0(" FIN  DUMP APRES ADD") ;
#endif
}

// Ajout d'un betree
void T_betree_manager::delete_betree(T_betree *new_betree)
{
  TRACE3("T_betree_manager(%x)::delete_betree(%x:%s)",
		 this, new_betree, new_betree->get_file_name()->get_value());

#ifdef DEBUG_BMANA
  TRACE0("DEBUT DUMP AVANT DELETE") ;
  T_betree *cur = first_betree ;
  while (cur != NULL)
	{
	  TRACE4("%x <-- %x:%s --> %x",
			 cur->get_prev_betree(),
			 cur,
			 cur->get_file_name()->get_value(),
			 cur->get_next_betree()) ;
	  cur = cur->get_next_betree() ;
	}
  TRACE0(" FIN  DUMP AVANT DELETE") ;
#endif

  T_betree *prev = new_betree->get_prev_betree() ;
  T_betree *next = new_betree->get_next_betree() ;

  if (new_betree == first_betree)
	{
	  first_betree = next ;
	}
  else
	{
	  prev->set_next_betree(next) ;
	}

  if (new_betree == last_betree)
	{
	  last_betree = prev ;
	}
  else
	{
	  next->set_prev_betree(prev) ;
	}

#ifdef DEBUG_BMANA
  TRACE0("DEBUT DUMP APRES DELETE") ;
  cur = first_betree ;
  while (cur != NULL)
	{
	  TRACE4("%x <-- %x:%s --> %x",
			 cur->get_prev_betree(),
			 cur,
			 cur->get_file_name()->get_value(),
			 cur->get_next_betree()) ;
	  cur = cur->get_next_betree() ;
	}
  TRACE0(" FIN  DUMP APRES DELETE") ;
#endif
}

// Recherche d'un Betree en memoire par son nom
// Renvoie NULL si le Betree n'est pas en memoire
T_betree *betree_manager_get_betree(T_symbol *betree_name)
{
TRACE2("betree_manager_get_betree(%x) : look for betree \"%s\"",
	   manager_sop,
	   betree_name->get_value()) ;

ASSERT(manager_sop != NULL) ;

T_betree *cur = manager_sop->get_betrees() ;

for (;;)
	{
	if (cur == NULL)
		{
#ifdef DEBUG_BMANA
		TRACE0("pas trouve") ;
#endif // DEBUG_BMANA
		return NULL ;
		}

	T_symbol *cur_name = cur->get_betree_name() ;

#ifdef DEBUG_BMANA
	TRACE2("betree_name %s cur_name %s",
		   betree_name->get_value(),
		   cur_name->get_value()) ;
#endif // DEBUG_BMANA

	if (betree_name->compare(cur_name) == TRUE)
		{
#ifdef DEBUG_BMANA
		  TRACE0("trouve !") ;
#endif // DEBUG_BMANA
		  return cur ;
		}

	cur = cur->get_next_betree() ;
	}
}

T_betree *T_betree_manager::get_betrees(void)
{
  TRACE2("T_betree_manager_get_betrees(%x)->%x", this, first_betree) ;
  return first_betree ;
}

static char *next_path(char *path)
{
	while(*path != '\0')
	{
		switch(*path)
		{
		case '/':
		case '\\':
			return path;
		default:
			++path;
		}
	}

	return 0;
}

// En mode multi-compo on verifie en plus l'integrite du fichier source
// i.e. que les composants qui sont la ont bien le droit d'y etre !
void T_betree_manager::check_file_integrity(const char *file_name)
{
  TRACE2("T_betree_manager(%x)::check_file_integrity(%s)", this, file_name) ;

  // Ici on sait que file_name = xxx.mod ou xxx.mch ou xxx.ref ou xxx.imp
  // car on est appele alors que get_error_count() = 0
  ASSERT(get_error_count() == 0) ;

  char *base_name ;
  int len = clone(&base_name, file_name) ;
  char *start_base_name = base_name ;
  // position des 'm' 'c' 'h' ou equivalents
  const char *first = (const char *)((size_t) base_name + len - 3) ;
  const char *second = (const char *)((size_t) first + 1) ;
  char *dot = (char *)((size_t) first - 1) ;
#ifndef NO_CHECKS
  ASSERT(*dot == '.') ;
  ASSERT(    (strcmp(first, "mch") == 0)
		  || (strcmp(first, "mod") == 0)
		  || (strcmp(first, "ref") == 0)
		  || (strcmp(first, "imp") == 0)
		  || (strcmp(first, "sys") == 0) ) ;
#endif
  *dot = '\0' ;

  char *p ;

  // Il faut ensuite enlever tous les '/' (unix) ou les '\' (windows)
  // du nom du fichier
  char *clone_file_name ;
  clone(&clone_file_name, file_name) ;
  char *start_clone = clone_file_name ;
  TRACE2("clone_file_name %x, start_clone %x", clone_file_name, start_clone) ;
  while ((p = next_path(clone_file_name)) != NULL)
	{
	  clone_file_name = (char *)((size_t)p + 1) ;
	  TRACE2("un %c trouve, on avance -> <%s>", *p, clone_file_name) ;
	}

  T_symbol *ref_file_name = lookup_symbol(clone_file_name) ;
  T_symbol *ref_path_name = lookup_symbol(file_name, len) ;
  TRACE2("clone_file_name %x, start_clone %x", clone_file_name, start_clone) ;
  free(start_clone) ;

  // Il faut ensuite enlever tous les '/' (unix) ou les '\' (windows)

  while ((p = next_path(base_name)) != NULL)
	{
	  base_name = (char *)((size_t)p + 1) ;
	  TRACE2("un %c trouve, on avance -> <%s>", *p, base_name) ;
	}

  T_symbol *ref_base_name = lookup_symbol(base_name) ;

  TRACE4("ref_file_name(%x \"%s\") ref_base_name(%x \"%s\")",
		 ref_file_name,
		 ref_file_name->get_value(),
		 ref_base_name,
		 ref_base_name->get_value()) ;

  TRACE2("<%c><%c>", *first, *second) ;
  if (*first == 'i')
	{
	  // Implementation : il faut qu'un seul Betree, l'implementation
	  // qui a le meme nom
	  TRACE0("-- CAS D'UNE IMPLEMENTATION --") ;
	  single_check(ref_file_name, ref_path_name, ref_base_name) ;
	}
  else if (*first == 'r')
	{
	  // Raffinement : il faut qu'un seul Betree, l'implementation
	  // qui a le meme nom
	  TRACE0("-- CAS D'UN RAFFINEMENT --") ;
	  single_check(ref_file_name, ref_path_name, ref_base_name) ;
	}
  else if (*first == 's')
	{
	  // System : il faut qu'un seul Betree, l'implementation
	  // qui a le meme nom
	  TRACE0("-- CAS D'UN SYSTEM --") ;
	  single_check(ref_file_name, ref_path_name, ref_base_name) ;
	}
  // Ici on sait que *first = 'm' : on regarde second pour discriminer
  // mch et mod
  else if (*second == 'c')
	{
	  // Specificiation : il faut qu'un seul Betree, l'implementation
	  // qui a le meme nom
	  TRACE0("-- CAS D'UNE SPEC --") ;
	  single_check(ref_file_name, ref_path_name, ref_base_name) ;
	}
  else
	{
	  // Ici on sait qu'on est dans un .mod
	  TRACE0("-- CAS D'UN MULTI-COMPO") ;
	  multi_check(ref_file_name, ref_path_name, ref_base_name) ;
	}

  free(start_base_name) ;
}

// On verifie que tous les Betree dont file_name vaut ref_file_name
// sont du type ref_mtype
void T_betree_manager::single_check(T_symbol *ref_file_name,
											T_symbol *ref_path_name,
											 T_symbol *ref_mach_name)
{
  TRACE4("--> T_betree_manager(%x)::single_check(%s, %s)",
		 this,
		 ref_file_name->get_value(),
		 ref_path_name->get_value(),
		 ref_mach_name->get_value()) ;
  ENTER_TRACE ;

  T_betree *cur = first_betree ;
  int one_found = FALSE ;

  while (cur != NULL)
	{
	  // On utilise le champ integrity_check_done de T_betree pour dire qu'il a ete
	  // visite. C'est important car si BB.mch utilise i1.AA et i2.AA
	  // alors lors de la verif i2.AA/AA.mch, il ne faut pas prendre
	  // en compte le AA de i1/AA
	  if (    (cur->get_integrity_check_done() == FALSE)
		   && (cur->get_file_name()->compare(ref_path_name) == TRUE) )
		{
		  T_symbol *mach_name = cur->get_root()->get_machine_name()->get_name() ;
		  if (one_found == TRUE)
			{
			  syntax_error(cur->get_root()->get_ref_lexem(),
						   CAN_CONTINUE,
						   get_error_msg(E_ONLY_ONE_COMPONENT_CAN_BE_STORED_IN_FILE),
						   mach_name->get_value(),
						   ref_file_name->get_value()) ;
			}
		  else
			{
			  one_found = TRUE ;

			  if (mach_name->compare(ref_mach_name) == FALSE)
				{
				  // Erreur !
				  syntax_error(cur->get_root()->get_ref_lexem(),
							   CAN_CONTINUE,
							   get_error_msg(E_BAD_COMPONENT_NAME),
							   ref_file_name->get_value(),
							   mach_name->get_value()) ;
				}

			  cur->set_integrity_check_done(TRUE) ; // pour dire qu'on l'a verifie
			}
		}

	  cur = cur->get_next_betree() ;
	}

  EXIT_TRACE ;
  TRACE3("<-- T_betree_manager(%x)::single_check(%s, %s)",
		 this,
		 ref_file_name->get_value(),
		 ref_mach_name->get_value()) ;
}


// Fonction auxiliaire qui cherche si un composant est deja present dans la liste
static void check_component_unicity(T_betree *betree,
											 T_list_link *compo_list)
{
  TRACE3("check_component_unicity(%x (%s), %x",
		 betree,
		 betree->get_root()->get_machine_name()->get_name()->get_value(),
		 compo_list) ;

  T_list_link *cur = compo_list ;
  T_symbol *name = betree->get_root()->get_machine_name()->get_name() ;

  while (cur != NULL)
	{
	  T_betree *cur_betree = (T_betree *)cur->get_object() ;
	  T_symbol *cur_name = cur_betree->get_root()->get_machine_name()->get_name() ;

	  if (name == cur_name)
		{
		  syntax_error(betree->get_root()->get_ref_lexem(),
					   CAN_CONTINUE,
					   get_error_msg(E_COMPO_CLASH),
					   name->get_value()) ;
		  syntax_error(cur_betree->get_root()->get_ref_lexem(),
					   MULTI_LINE,
					   get_error_msg(E_IDENT_CLASH_OTHER_LOCATION)) ;
		}
	  cur = (T_list_link *)cur->get_next() ;
	}
}

// On verifie que tous les Betree dont file_name vaut ref_file_name
// sont du type ref_mtype
void T_betree_manager::multi_check(T_symbol *ref_file_name,
											T_symbol *ref_path_name,
											T_symbol *ref_mach_name)
{
  TRACE4("--> T_betree_manager(%x)::multi_check(file <%s> path <%s>, mach <%s>)",
		 this,
		 ref_file_name->get_value(),
		 ref_path_name->get_value(),
		 ref_mach_name->get_value()) ;
  ENTER_TRACE ;

  // 1) On commence par recenser tous les Betree de ce fichier
  // au passage on cherche le point d'entree i.e. le composant
  // de meme nom que le fichier
  T_item *first_solved = NULL ;
  T_item *last_solved = NULL ;

  T_item *first_unsolved = NULL ;
  T_item *last_unsolved = NULL ;

  T_betree *entry = NULL ;
  T_betree *one_betree = NULL ;

  T_betree *cur = first_betree ;
  int nb_betrees = 0 ;

  TRACE0("--> RECENSEMENT") ;
  ENTER_TRACE ;
  while (cur != NULL)
	{
	  TRACE2("cur->get_file_name <%s>, ref_path_name <%s>",
			 cur->get_file_name()->get_value(),
			 ref_path_name->get_value()) ;
	  if (cur->get_file_name()->compare(ref_path_name) == TRUE)
		{
		  one_betree = cur ;
		  nb_betrees++ ;
		  TRACE2("le composant %x:%s est dans ce fichier",
				 cur,
				 cur->get_betree_name()->get_value()) ;
		  cur->set_father(new T_list_link(cur,
										  LINK_BETREE_LIST,
										  (T_item **)&first_unsolved,
										  (T_item **)&last_unsolved,
										  NULL,
										  NULL,
										  NULL)) ;

		  TRACE4("INIT LOOP :: solved (%x, %x) unsolved (%x, %x)",
				 first_solved,
				 last_solved,
				 first_unsolved,
				 last_unsolved) ;

		  T_symbol *mach_name = cur->get_root()->get_machine_name()->get_name() ;
		  TRACE2("!!mach_name <%s> ref_mach_name <%s>",
				 mach_name->get_value(), ref_mach_name->get_value()) ;
		  if (mach_name->compare(ref_mach_name) == TRUE)
			{
			  TRACE1("ok entry=%x", entry) ;
			  entry = cur ;
			}

		}

	  cur = cur->get_next_betree() ;
	}
  EXIT_TRACE ;
  TRACE0("<-- RECENSEMENT") ;

  // sinon le fichier etait vide ! ca a ete verifie avant
  ASSERT(one_betree != NULL) ;
  T_symbol *mach_name = one_betree->get_root()->get_machine_name()->get_name() ;
  TRACE1("entry=%x", entry) ;
  if (entry == NULL)
	{
	  // Pas de composant du meme nom que le fichier : erreur !!!

	  if (nb_betrees == 1)
		{
		  syntax_error(one_betree->get_root()->get_ref_lexem(),
					   CAN_CONTINUE,
					   get_error_msg(E_BAD_COMPONENT_NAME),
					   ref_file_name->get_value(),
					   mach_name->get_value()) ;
		}
	  else
		{
		  // On prend le premier
		  toplevel_error(CAN_CONTINUE,
						 get_error_msg(E_NO_COMPONENT_MATCH_IN_MULTI_COMPO_FILE),
						 ref_mach_name->get_value(),
						 ref_file_name->get_value()) ;

		  // On liste les betree, cast valide par construction
		  T_list_link *cur = (T_list_link *)first_unsolved ;

		  while (cur != NULL)
			{
			  // Cast valide par construction
			  T_betree *betree = (T_betree *)cur->get_object() ;

			  syntax_error(betree->get_root()->get_ref_lexem(),
						   MULTI_LINE,
						   get_error_msg(E_LOCATION_OF_ITEM_DEF),
						   betree->get_betree_name()->get_value()) ;

			  cur = (T_list_link *)cur->get_next() ;
			}
		}

	  return ;
	}

  TRACE2("ok point d'entree = %x:%s",
		 entry,
		 entry->get_betree_name()->get_value()) ;

  // Boucle de raccrochage
  int nb_unsolved = nb_betrees - 1 ;

  TRACE4("AVANT MOVE ENTRY  :: solved (%x, %x) unsolved (%x, %x)",
		 first_solved,
		 last_solved,
		 first_unsolved,
		 last_unsolved) ;

  entry->get_father()->remove_from_list(&first_unsolved, &last_unsolved) ;
  entry->get_father()->tail_insert(&first_solved, &last_solved) ;

  TRACE4("INIT LOOP :: solved (%x, %x) unsolved (%x, %x)",
		 first_solved,
		 last_solved,
		 first_unsolved,
		 last_unsolved) ;

  while (first_unsolved != NULL)
	{
	  // On parcourt la liste
	  // Les composants non raccroches sont caracterises par :
	  // get_tmp2() != get_timestamp()
	  // On regarde s'ils sont apparentes a un
	  TRACE1(":: LOOP >%d unsolved betrees<", nb_unsolved) ;
	  int save_unsolved = nb_unsolved ;

	  // Cast valide par construction
	  T_list_link *cur_link = (T_list_link *)first_unsolved ;

	  while (cur_link != NULL)
		{
		  T_list_link *next = (T_list_link *)cur_link->get_next() ;
		  T_betree *betree = (T_betree *)cur_link->get_object() ;
		  TRACE2("betree %x:%s is unsolved",
				 betree, betree->get_betree_name()->get_value()) ;

		  T_machine *machine = betree->get_root() ;

		  switch (machine->get_machine_type())
			{
			case MTYPE_SPECIFICATION :
			  {
				// Est-ce qu'on est importe/extends ?
				if (lookup_importer(betree, (T_list_link *)first_solved) != NULL)
				  {
					TRACE0("on est importe/etendu par un betree resolu !!!") ;
					// Reste a verifier qu'un composant de meme nom
					// n'existe pas deja
					check_component_unicity(betree, (T_list_link *)first_solved) ;
					betree->get_father()->remove_from_list(&first_unsolved,
														   &last_unsolved) ;
					betree->get_father()->tail_insert(&first_solved, &last_solved) ;
					nb_unsolved -- ;
				  }
				break ;
			  }

			case MTYPE_REFINEMENT :
			case MTYPE_IMPLEMENTATION :
			  {
				// Est-ce qu'on raffine un composant resolu ???
				if (lookup_abstraction(betree, (T_list_link *)first_solved) != NULL)
				  {
					TRACE0("on raffine un betree resolu !!!") ;
					// Reste a verifier qu'un composant de meme nom
					// n'existe pas deja
					check_component_unicity(betree, (T_list_link *)first_solved) ;

					betree->get_father()->remove_from_list(&first_unsolved,
														   &last_unsolved) ;
					betree->get_father()->tail_insert(&first_solved, &last_solved) ;
					nb_unsolved -- ;
				  }
				break ;
			  }

			  // Pas de default pour que le compilateur nous previenne
			  // en cas d'oubli
			}

		  cur_link = next ;
		}

	  if (save_unsolved == nb_unsolved)
		{
		  // Un tour complet sans resoudre
		  // Donc tous les arbres qui sont unsolved n'ont rien a faire la
		  // On liste les betree
		  T_list_link *cur = (T_list_link *)first_unsolved ;

		  while (cur != NULL)
			{
			  // Cast valide par construction
			  T_betree *betree = (T_betree *)cur->get_object() ;

			  syntax_error(betree->get_root()->get_ref_lexem(),
						   CAN_CONTINUE,
						   get_error_msg(E_COMPONENT_CAN_NOT_BE_IN_THIS_FILE),
						   betree->get_betree_name()->get_value()) ;

			  cur = (T_list_link *)cur->get_next() ;
			}

		  EXIT_TRACE ;
		  TRACE3("<-- T_betree_manager(%x)::multi_check(%s, %s)",
				 this,
				 ref_file_name->get_value(),
				 ref_mach_name->get_value()) ;

		  return ;
		}
	}

  EXIT_TRACE ;
  TRACE3("<-- T_betree_manager(%x)::multi_check(%s, %s)",
		 this,
		 ref_file_name->get_value(),
		 ref_mach_name->get_value()) ;
}


// Fonction auxiliaire que regarde si un betree est un raffinement
// d'un betree de la liste. Renvoie ce betree si oui, NULL sinon
T_betree *T_betree_manager::lookup_abstraction(T_betree *betree,
														T_list_link *list)
{
  TRACE3("T_betree_manager::lookup_abstraction(%x:%s, %x)",
		 betree, betree->get_betree_name()->get_value(), list) ;

  if (betree->get_root()->get_abstraction_name() == NULL)
	{
	  // Erreur de syntaxe : le composant n'a pas de clause REFINES
	  TRACE0("ERREUR : le composant n'a pas de clause REFINES") ;
	  syntax_error(betree->get_root()->get_ref_lexem(),
				   CAN_CONTINUE,
				   get_error_msg(E_COMPONENT_HAS_NO_REFINES_CLAUSE)) ;
	  return NULL ;
	}

  T_symbol *abs_name = betree->get_root()->get_abstraction_name()->get_name() ;

  T_list_link *cur = list ;

  while (cur != NULL)
	{
	  // Cast valide par construction
	  T_betree *cur_betree = (T_betree *)cur->get_object() ;

	  if (cur_betree->get_betree_name()->compare(abs_name) == TRUE)
		{
		  return cur_betree ;
		}

	  cur = (T_list_link *)cur->get_next() ;

	}

  return NULL ;
}

// Fonction auxiliaire que regarde si un betree est un raffinement
// d'un betree de la liste. Renvoie ce betree si oui, NULL sinon
T_betree *T_betree_manager::lookup_importer(T_betree *betree,
													 T_list_link *list)
{
  TRACE3("T_betree_manager::lookup_importer(%x:%s, %x)",
		 betree, betree->get_betree_name()->get_value(), list) ;
  ENTER_TRACE ; ENTER_TRACE ;

  T_symbol *imported_name = betree->get_betree_name() ;

  T_list_link *cur = list ;

  while (cur != NULL)
	{
	  // Cast valide par construction
	  T_betree *cur_betree = (T_betree *)cur->get_object() ;
	  T_machine *cur_machine = cur_betree->get_root() ;

	  TRACE3("cur_betree %x:%s machine %s",
			 cur_betree, cur_betree->get_betree_name()->get_value(),
			 get_machine_type_name(cur_machine->get_machine_type())) ;

	  if (cur_machine->get_machine_type() == MTYPE_IMPLEMENTATION)
		{
		  TRACE4("OK %x:%s est une implementation imports=%x extends=%x",
				 cur_machine,
				 cur_machine->get_machine_name()->get_name()->get_value(),
				 cur_machine->get_imports(),
				 cur_machine->get_extends()) ;
		  T_used_machine *cur_umach = cur_machine->get_imports() ;
		  int extends_done = FALSE ;

		  while (cur_umach != NULL)
			{
			  TRACE2("cur_umach %x %s",
					 cur_umach, cur_umach->get_component_name()->get_value()) ;
			  if (cur_umach->get_component_name()->compare(imported_name) == TRUE)
				{
				  EXIT_TRACE ; EXIT_TRACE ;
				  return cur_betree ;
				}

			  cur_umach = (T_used_machine *)cur_umach->get_next() ;

			  if ( (cur_umach == NULL) && (extends_done == FALSE) )
				{
				  extends_done = TRUE ;
				  cur_umach = cur_machine->get_extends() ;
				}
			}
		}

	  cur = (T_list_link *)cur->get_next() ;

	}

  EXIT_TRACE ; EXIT_TRACE ;
  return NULL ;
}


//
// }{ Fin du fichier
//


