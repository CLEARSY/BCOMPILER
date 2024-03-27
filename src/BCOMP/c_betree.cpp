/******************************* CLEARSY **************************************
* Fichier : $Id: c_betree.cpp,v 2.0 1999-07-19 15:58:06 sl Exp $
* (C) 2008 CLEARSY
*
* Description :		Compilateur B
*					Betree
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
RCS_ID("$Id: c_betree.cpp,v 1.24 1999-07-19 15:58:06 sl Exp $") ;

/* Includes systeme */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>

/* Includes Composant Local */
#include "c_api.h"

// Constructeur
T_betree::T_betree(const char *new_betree_name) :
  T_item(NODE_BETREE, NULL, NULL, NULL, NULL, NULL)
{
  TRACE2("T_betree(%x)::T_betree(%s)", this, new_betree_name) ;
  status = BST_PARSING ;
  root = NULL ;
  file_name = lookup_symbol(new_betree_name) ;
  checksum = NULL ;
  expanded_checksum = NULL ;
  revision_ID = NULL ;
  betree_name = lookup_symbol(new_betree_name) ;

  manager = NULL ;
  seen_by = new T_betree_list() ;
  included_by = new T_betree_list() ;
  imported_by = new T_betree_list() ;
  extended_by = new T_betree_list() ;
  used_by = new T_betree_list() ;

  collision_analysed = false;

  get_betree_manager()->add_betree(this) ;
  set_manager(get_betree_manager()) ;

}

// Destructeur
T_betree::~T_betree(void)
{
  TRACE1("T_betree::~T_betree(%x)", this) ;
}

// Ajout d'une reference sur un Betree chargeur
void T_betree::add_list(T_use_type use_type,
								 T_betree *loader_betree,
								 T_lexem *new_ref_lexem)
{
  TRACE4("T_betree(%s)::add_list(%d, %x)",
		 betree_name->get_value(),
		 use_type,
		 loader_betree,
		 loader_betree->betree_name->get_value()) ;

  TRACE2("Le betree %s charge le betree betree(%s)",
		 loader_betree->betree_name->get_value(),
		 betree_name->get_value()) ;


  T_betree_list **adr_list = NULL ;

  switch (use_type)
	{
	case USE_SEES :
	  {
		adr_list = &seen_by ;
		break ;
	  }
	case USE_INCLUDES :
	  {
		adr_list = &included_by ;
		break ;
	  }
	case USE_IMPORTS :
	  {
		adr_list = &imported_by ;
		break ;
	  }
	case USE_EXTENDS :
	  {
		adr_list = &extended_by ;
		break ;
	  }
	case USE_USES :
	  {
		adr_list = &used_by ;
		break ;
	  }
	  // Pas de default expres pour que le compilateur verifie que l'on
	  // a bien traite tous les cas
	}

  ASSERT(*adr_list != NULL) ; // garanti par le constructeur

  // Ajout dans la liste, avec verification de non redefinition
  T_list_link *prev_req = (*adr_list)->add_betree(loader_betree, new_ref_lexem) ;

  if (prev_req != NULL)
	{
	  // Il y a redefinition !!
	  semantic_error(new_ref_lexem,
					 CAN_CONTINUE,
					 get_error_msg(E_COMPONENT_MULTIPLY_REFERENCED),
					 get_betree_name()->get_value()) ;
	  already_location(prev_req->get_ref_lexem()) ;
	}

  // Il faut ensuite verifier que le chargeur n'est dans aucune autre liste !
  // On utilise la propriete suivante de la classe (cf le .h)
  // 5 listes de composants qui utilisent
  //
  // ATTENTION CES CINQ LISTES DOIVENT ETRE CONSECUTIVES DANS
  // LA CLASSE, seen_by DOIT ETRE LE PREMIER, ET used_by LE DERNIER
  //
  // CERTAINES METHODES UTILISENT CETTE PROPRIETE
  //
  T_betree_list **adr_first = &(this->seen_by) ;
  T_betree_list **adr_last = &(this->used_by) ;
  T_betree_list **cur_list = adr_first ;

  while ((size_t)cur_list <= (size_t)adr_last)
	{
#ifdef FULL_TRACE
	  TRACE2("cur_list %x, adr_list %x", cur_list, adr_list) ;
#endif
	  if (cur_list != adr_list)
		{
		  T_list_link *prev_req = (*cur_list)->search_betree(loader_betree) ;

		  if (prev_req != NULL)
			{
			  // Il y a redefinition !!
			  // Attention, il faut essayer de garder l'ordre du source !
			  // Ordre trouve dans le betree :
			  T_lexem *new_lexem = new_ref_lexem ;
			  T_lexem *old_lexem = prev_req->get_ref_lexem() ;

			  // Ordre dans le source :
			  size_t new_line = new_lexem->get_file_line() ;
			  size_t old_line = old_lexem->get_file_line() ;
			  size_t new_column = new_lexem->get_file_column() ;
			  size_t old_column = old_lexem->get_file_column() ;

			  // Ordre pour le message
			  T_lexem *first = NULL ;
			  T_lexem *second = NULL ;

			  if (    (new_line > old_line)
					  || ( (new_line == old_line) && (new_column > old_column) ) )
				{
				  // Ok, l'ordre trouve est celui du source
				  first = old_lexem ;
				  second = new_lexem ;
				}
			  else
				{
				  // Tout faux, l'ordre trouve est l'inverse du source !
				  first = new_lexem ;
				  second = old_lexem ;
				}


			  TRACE6("AAA new(%x) :: L%dC%d old(%x) :: L%dC%d",
					 new_lexem, new_line, new_column,
					 old_lexem, old_line, old_column) ;
			  TRACE6("AAA first(%x) :: L%dC%d second(%x) :: L%dC%d",
					 first,
					 first->get_file_line(),
					 first->get_file_column(),
					 second,
					 second->get_file_line(),
					 second->get_file_column()) ;
			  semantic_error(second,
							 CAN_CONTINUE,
							 get_error_msg(E_COMPONENT_MULTIPLY_REFERENCED),
							 get_betree_name()->get_value()) ;
			  already_location(first) ;
			}

		}

	  // Au suivant !
	  // Cette partie de code exige que les listes soient contigues dans
	  // la classe
	  cur_list = (T_betree_list **)((size_t)cur_list + sizeof(T_betree_list **)) ;
	}
}


// Acces au pathname (via T_used_machine associe ou this si racine)
T_symbol *T_betree::get_pathname(void)
{
  T_used_machine *context = root->get_context() ;

  if (context == NULL)
	{
	  // Racine
	  return root->get_machine_name()->get_name() ;
	}
  else
	{
	  context->make_pathname() ;
	  return context->get_pathname() ;
	}
}

// Fonction qui enleve les "liens sortant"
// Utile pour que recursive_set_block_state ne rende pas persistant
// des objets qu'on souhaite liberer avec s_memory_clean_up
void T_betree::make_standalone(void)
{
  TRACE0("T_betree::make_standalone(void)");

  // On coupe tous les liens vers un objet à partir du quel on peut atteindre
  // d'autres machines ou betree.
  // On coupe egalement tout ce qui ne sert a rien au MS et que l'on prefere
  // liberer
  root = NULL;
  file_name = NULL;
  path = NULL;
  manager = NULL;
  first_lexem = NULL;
  last_lexem = NULL;
  last_code_lexem = NULL;
  definitions_clause = NULL;
  first_definition = NULL;
  last_definition = NULL;
  first_file_definition = NULL;
  last_file_definition = NULL;
  next_betree = NULL;
  prev_betree = NULL;
  seen_by = NULL;
  included_by = NULL;
  imported_by = NULL;
  extended_by = NULL;
  used_by = NULL;
  // On coupe les liens herites de T_item
  set_betree(NULL);
  set_father(NULL);
  set_next(NULL);
  set_prev(NULL);
  set_ref_lexem(NULL);
}

//
//	}{ Fin du fichier
//
