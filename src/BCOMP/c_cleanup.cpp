/******************************* CLEARSY **************************************
* Fichier : $Id: c_cleanup.cpp,v 2.0 2001-07-19 14:11:38 lv Exp $
*
* (C) 2008 CLEARSY
*
* Description :		Gestion du garbage collector du compilateur
*
* Compilation :		-DMEMORY_CLEANUP pour activer le garbage collector
*					-DTRACE_CLEANUP pour tracer les traitements
*					-DTRACE_CLEANUP_BLOCKS pour afficher les blocs
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
RCS_ID("$Id: c_cleanup.cpp,v 1.10 2001-07-19 14:11:38 lv Exp $") ;

// Includes systeme
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <set>

// Includes locaux
#include "c_api.h"

// Lancer le garbage collector
void memory_cleanup(void)
{
  std::vector<T_object *> objects_to_delete;
  objects_to_delete.reserve(get_object_manager()->get_nb_objects());
  for (T_object_tag *object_tag = get_object_manager()->get_tags(); object_tag; object_tag = object_tag->get_next_tag())
    {
      T_object *object = object_tag->get_object();
      if (!object->get_tag()) continue;
      if (!object->is_an_item()) continue;
      T_item *item = static_cast<T_item *>(object);
      T_item *root = item;
      for (; root->get_father() && root->get_father()->is_an_item(); root = root->get_father())
        {
        }
      for (T_betree *betree = get_betree_manager()->get_betrees(); betree; betree = betree->get_next_betree())
        {
          if (   root == betree
              || (   root->get_father() // Someone has lied.
                  && (   reinterpret_cast<T_betree_list *>(root->get_father()) == betree->get_seen_by()
                      || reinterpret_cast<T_betree_list *>(root->get_father()) == betree->get_included_by()
                      || reinterpret_cast<T_betree_list *>(root->get_father()) == betree->get_imported_by()
                      || reinterpret_cast<T_betree_list *>(root->get_father()) == betree->get_extended_by()
                      || reinterpret_cast<T_betree_list *>(root->get_father()) == betree->get_used_by())))
            {
              objects_to_delete.push_back(item);
              break;
            }
        }
    }
  for (T_betree *betree = get_betree_manager()->get_betrees(); betree; betree = betree->get_next_betree())
    {
      objects_to_delete.push_back(betree->get_seen_by());
      objects_to_delete.push_back(betree->get_included_by());
      objects_to_delete.push_back(betree->get_imported_by());
      objects_to_delete.push_back(betree->get_extended_by());
      objects_to_delete.push_back(betree->get_used_by());
    }
  objects_to_delete.push_back(get_betree_manager());
  for (std::vector<T_object *>::const_iterator it = objects_to_delete.begin(); it != objects_to_delete.end(); ++it)
    {
      delete *it;
    }

  // Reset du gestionnaire d'objets
  reset_object_manager_context() ;

  // Apres : reset variables globales
  reset_compiler() ;
}

//
// Fin du fichier
//
