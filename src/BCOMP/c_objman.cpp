/******************************* CLEARSY **************************************
* Fichier : $Id: c_objman.cpp,v 2.0 2000-10-20 12:12:40 fl Exp $
* (C) 2008 CLEARSY
*
* Compilation :		* -DDUMP_UNSOLVED pour tracer les identificateurs non resolus
* 				 	* -DDEBUG_COMMENT pour voir les commentaires
*					* -DSTATS_ALLOC permet d'avoir les stats d'allocation par
*					  classe (il faut alors aussi compiler c_secure.cpp avec
*					  ce flag)
*					* -DDEBUG_OP_RESULT pour tracer la gestion des T_op_result
*
* Description :		Compilateur B
*					Gestionnaire d'objets (classe T_object_manager)
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
RCS_ID("$Id: c_objman.cpp,v 1.52 2000-10-20 12:12:40 fl Exp $") ;

/* Includes systeme */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>

/* Includes Composant Local */
#include "c_api.h"

//
//	}{	Classe T_object_tag
//

//
// Constructeur
//
T_object_tag::T_object_tag(void)
{
#ifdef TRACE_OBJMAN
  TRACE1("T_object_tag(%x)::T_object_tag", this) ;
  static int cpt_si = 0 ;
  TRACE1("c'est le %d", ++cpt_si) ;
  if (cpt_si == 672)
	{
	  next_tag = 0 ;
	  watch((void **)&next_tag) ;
	}
#endif

#ifdef STATS_ALLOC
  // Prise en compte dans les stats
  add_alloc(this) ;
#endif

  object = NULL ;
  context = 0 ;
  next_tag = prev_tag = NULL ;
}

//
// Destructeur
//
T_object_tag::~T_object_tag(void)
{
  //  TRACE0("T_object_tag::~T_object_tag -- on ne fait rien") ;
}

// Initialisation i.e. prise en compte d'un objet
void T_object_tag::init(T_object *new_object, size_t new_context)
{
  object = new_object ;
  context = new_context ;
}

//
//	}{	Classe T_object_manager
//

// Plus petite adresse d'objet allouee
static size_t min_addr_si = INT_MAX ;

//
// Constructeur
//
T_object_manager::T_object_manager(void)
{
  TRACE0("T_object_manager::T_object_manager") ;

  first_tag = last_tag = NULL ;
  first_unsolved_ident = last_unsolved_ident = NULL ;
  first_ident = last_ident = NULL ;
  first_unsolved_comment = last_unsolved_comment = NULL ;
  first_lexem = last_lexem = NULL ;
  first_op_result = last_op_result = NULL ;
  first_abstract_type = last_abstract_type = NULL ;

  cur_rank = cur_context = 0 ;
  nb_objects = 0 ;
}

//
// Destructeur
//
T_object_manager::~T_object_manager(void)
{
  TRACE0("T_object_manager::~T_object_manager") ;
}

// Ajout d'un objet
void T_object_manager::add_object(T_object *new_object)
{
#ifdef TRACE_OBJMAN
  TRACE2("T_object_manager(%x)::add_object(%x)",
		 this,
		 new_object) ;
#endif

  // Un objet de plus !
  nb_objects ++ ;

  // Creation et initialisation du tag
  T_object_tag *new_tag = new T_object_tag ;	// A FAIRE
#ifdef TRACE_OBJMAN
  TRACE3("on a cree le tag %x pour referencer l'objet %x:%d",
		 new_tag,
		 new_object,
		 new_object->get_node_type()) ;
#endif

  new_tag->init(new_object, get_context()) ;

  new_object->set_tag(new_tag) ;

  // Chainage en queue du tag dans le gestionnaire d'objets
#ifdef TRACE_OBJMAN
  TRACE3("chain new_tag %x first_tag %x last_tag %x",
		 new_tag, first_tag, last_tag) ;
#endif
  if (last_tag == NULL)
	{
	  // Chainage dans une liste vide
	  first_tag = new_tag ;
	}
  else
	{
	  // Chainage en queue
	  last_tag->next_tag = new_tag ;
	}

  new_tag->next_tag = NULL ;
  new_tag->prev_tag = last_tag ;
  last_tag = new_tag ;

#if 0 // TRACE_OBJMAN
  TRACE3(">> DEBUT DUMP TAGS objman %x first %x last %x",
		 this,
		 first_tag,
		 last_tag) ;
  ENTER_TRACE ;
  T_object_tag *cur_tag = first_tag ;
  while (cur_tag != NULL)
	{
	  TRACE3("%x <-- cur %x --> %x",
			 cur_tag->get_prev_tag(),
			 cur_tag,
			 cur_tag->get_next_tag()) ;
	  cur_tag = cur_tag->get_next_tag() ;
	}

  EXIT_TRACE ;
  TRACE3("<<  FIN  DUMP TAGS objman %x first %x last %x",
		 this,
		 first_tag,
		 last_tag) ;
#endif // TAGS

  // Attribution du rang
  new_object->set_rank(get_unique_rank()) ;

  // Mises a jour de la plus petite adresse, le cas echeant
  if ((size_t)new_object < min_addr_si)
	{
	  min_addr_si = (size_t)new_object ;
	}
}

// Fonction permettant de supprimer un objet ainsi que le tag qui lui
// est associé.
void T_object_manager::delete_object(T_object *object_to_delete)
{
#ifdef TRACE_OBJMAN
  TRACE2("--> T_object_manager(%x)::delete_object(%x)", this, object_to_delete) ;
#endif

  ASSERT(object_to_delete != NULL) ;

  T_object_tag *tag_to_delete = object_to_delete->get_tag() ;

  if (!tag_to_delete) return;

  // Mise à neuf du chainage
  if (tag_to_delete == first_tag && tag_to_delete == last_tag)
	{
	  // Cas où tag_to_delete est le seul tag de la liste
	  ASSERT(tag_to_delete->get_next_tag() == NULL) ;
	  ASSERT(tag_to_delete->get_prev_tag() == NULL) ;
	  first_tag = NULL ;
	  last_tag = NULL ;
	}
  else
	{
      if (tag_to_delete != first_tag)
        {
          if (tag_to_delete->get_prev_tag())
            {
              tag_to_delete->get_prev_tag()->set_next_tag(tag_to_delete->get_next_tag()) ;
            }
        }
      else
        {
          if (tag_to_delete->get_next_tag())
            {
              tag_to_delete->get_next_tag()->set_prev_tag(NULL) ;
            }
          first_tag = tag_to_delete->get_next_tag() ;
        }
	  if (tag_to_delete != last_tag)
		{
          if (tag_to_delete->get_next_tag())
            {
              tag_to_delete->get_next_tag()->set_prev_tag(tag_to_delete->get_prev_tag()) ;
            }
		}
	  else
		{
          if (tag_to_delete->get_prev_tag())
            {
              tag_to_delete->get_prev_tag()->set_next_tag(NULL) ;
            }
		  last_tag = tag_to_delete->get_prev_tag() ;
		}
	}
  // Fin de mise à neuf du chainage

  object_to_delete->set_tag(NULL) ;
  delete tag_to_delete ;
  --nb_objects ;
#ifdef TRACE_OBJMAN
  TRACE2("<-- T_object_manager(%x)::delete_object(%x)", this, object_to_delete) ;
#endif
}

// Ajout d'un identificateur non resolu
void T_object_manager::add_unsolved_ident(T_ident *new_unsolved_ident)
{
#ifdef DUMP_UNSOLVED
  TRACE4("T_object_manager(%x)::add_unsolved_ident(%x) (first %x, last %x)",
		 this,
		 new_unsolved_ident,
		 first_unsolved_ident,
		 last_unsolved_ident) ;
#endif
#ifdef DUMP_UNSOLVED
  dump("add") ;
#endif /* DUMP_UNSOLVED */

  ASSERT(new_unsolved_ident->get_ident_type() == ITYPE_UNKNOWN) ;

  // Chainage en queue de l'ident dans le gestionnaire d'objets
  if (last_unsolved_ident == NULL)
	{
	  // Chainage dans une liste vide
	  first_unsolved_ident = new_unsolved_ident ;
	}
  else
	{
	  // Chainage en queue
	  last_unsolved_ident->set_next_unsolved_ident(new_unsolved_ident) ;
	}

  new_unsolved_ident->set_next_unsolved_ident(NULL) ;
  last_unsolved_ident = new_unsolved_ident ;

}

// Suppresion d'un identificateur non resolu si il est
// dans la liste (methode qui fait le test)
void T_object_manager::delete_unsolved_ident(T_ident *new_ident)
{
#ifdef DUMP_UNSOLVED
  TRACE4("T_object_manager(%x)::delete_unsolved_ident(%x) (first %x, last %x)",
		 this,
		 new_unsolved_ident,
		 first_unsolved_ident,
		 last_unsolved_ident) ;
#endif
#ifdef DUMP_UNSOLVED
  dump("add") ;
#endif /* DUMP_UNSOLVED */

  T_ident *cur = first_unsolved_ident ;
  T_ident *prev = NULL ;

  while (cur != new_ident)
	{
	  if (cur == NULL)
		{
		  // pas dans la liste
		  return ;
		}

	  prev = cur ;
	  cur = cur->get_next_unsolved_ident() ;
	}

  // Ici cur = new_ident
  TRACE0("suppression effective de la liste ...") ;
  T_ident *next = new_ident->get_next_unsolved_ident() ;
  if (prev == NULL)
	{
	  first_unsolved_ident = next ;
	}
  else
	{
	  prev->set_next_unsolved_ident(next) ;
	}

  if (next == NULL)
	{
	  last_unsolved_ident = prev ;
	}
}

// Obtention de la plus petite addresse memoire d'objet allouee
size_t T_object_manager::get_min_address(void)
{
  return min_addr_si ;
}

// Ajout d'un lexemes
void T_object_manager::add_lexem(T_lexem *new_lexem)
{
#ifdef TRACE_OBJMAN
  TRACE2("T_object_manager(%x)::add_lexem(%x)",
		 this,
		 new_lexem) ;
#endif

  // Chainage en queue du lexeme dans le gestionnaire d'objets
  if (last_lexem == NULL)
	{
	  // Chainage dans une liste vide
	  first_lexem = new_lexem ;
	}
  else
	{
	  // Chainage en queue
	  last_lexem->set_next_list_lexem(new_lexem) ;
	}

  new_lexem->set_next_list_lexem(NULL) ;
  last_lexem = new_lexem ;

}


#ifdef DUMP_UNSOLVED
static int inside = FALSE ;
void T_object_manager::dump(const char *msg)
{
  if (inside == TRUE)
	{
	  return ;
	}
  inside = TRUE ;
  fprintf(stdout, "<%s> DEBUT LISTE UNSOLVED IDENT\n", msg) ;
  TRACE0("DEBUT LISTE UNSOLVED IDENT\n") ;
  T_ident *cur = first_unsolved_ident ;
  while (cur != NULL)
	{
	  TRACE7("UNSOLVED ----> %s:%d:%d: %s(%x:%s) unsolved (next = %x)\n",
			 cur->get_ref_lexem()->get_file_name()->get_value(),
			 cur->get_ref_lexem()->get_file_line(),
			 cur->get_ref_lexem()->get_file_column(),
			 cur->get_name()->get_value(),
			 (size_t)cur,
			 cur->get_class_name(),
			 (size_t)cur->get_next_unsolved_ident()) ;
	  fprintf(stdout,
			  "%s:%d:%d: %s(%x:%s) unsolved (next = %x)\n",
			  cur->get_ref_lexem()->get_file_name()->get_value(),
			  cur->get_ref_lexem()->get_file_line(),
			  cur->get_ref_lexem()->get_file_column(),
			  cur->get_name()->get_value(),
			  (size_t)cur,
			  cur->get_class_name(),
			  (size_t)cur->get_next_unsolved_ident()) ;
	  cur = cur->get_next_unsolved_ident() ;
	}
  fprintf(stdout, "<%s> FIN LISTE UNSOLVED IDENT\n", msg) ;
  TRACE0("FIN   LISTE UNSOLVED IDENT\n") ;
  inside = FALSE ;
}
#endif /* DUMP_UNSOLVED */

#if 0
// On dit qu'un identificateur est resolu
// Ne marche que pour le premier !!!
void T_object_manager::delete_unsolved_ident(T_ident *new_ident)
{
#ifdef DUMP_UNSOLVED
  TRACE2("T_object_manager(%x)::delete_unsolved_ident(%x)", this, new_ident) ;
#endif
  first_unsolved_ident = new_ident->get_next_unsolved_ident() ;

  if (last_unsolved_ident == new_ident)
	{
	  // Cas de la suppression du dernier ident de la liste
	  last_unsolved_ident = first_unsolved_ident ;
	}
}
#endif

void T_object_manager::reset(void)
{
  TRACE1("T_object_manager(%x)::reset", this) ;

  first_tag = last_tag = NULL ;
  first_unsolved_ident = last_unsolved_ident = NULL ;
  first_ident = last_ident = NULL ;
  first_unsolved_comment = last_unsolved_comment = NULL ;
  first_lexem = last_lexem = NULL ;
  first_op_result = last_op_result = NULL ;
  first_abstract_type = last_abstract_type = NULL ;

  cur_rank = cur_context = 0 ;
  nb_objects = 0 ;
}

void T_object_manager::reset_context(void)
{
  cur_rank = cur_context = 0 ;
  first_unsolved_ident = last_unsolved_ident = NULL ;
  first_ident = last_ident = NULL ;
  first_unsolved_comment = last_unsolved_comment = NULL ;
  first_op_result = last_op_result = NULL ;
  first_abstract_type = last_abstract_type = NULL ;
}

//
//	}{	API du gestionnaire d'objets
//

// Gestionnaire d'objets
static T_object_manager *object_manager_sop = NULL ;

// Creation du gestionnaire d'objets
void new_object_manager(void)
{
  object_manager_sop = new T_object_manager() ;
}

// Liberation du gestionnaire d'objets
void unlink_object_manager(void)
{
  TRACE1("unlink_object_manager(%x)", object_manager_sop) ;
}

// Obtention du gestionnaire d'objets
T_object_manager *get_object_manager(void)
{
  return object_manager_sop ;
}

// Reset du gestionnaire d'objets (pour le mode cleanup)
void reset_object_manager(void)
{
  TRACE1("reset_object_manager(%x)", object_manager_sop) ;

  if (object_manager_sop != NULL)
	{
	  object_manager_sop->reset() ;
	}
}

void reset_object_manager_context(void)
{
  if (object_manager_sop != NULL)
    {
      object_manager_sop->reset_context();
    }
}

// Obtention de la liste des identificateurs non resolus
T_ident *T_object_manager::get_unsolved_idents(void)
{
#ifdef DUMP_UNSOLVED
  TRACE1("T_object_manager(%x)::get_unsolved_idents()", this) ;
#endif
#ifdef DUMP_UNSOLVED
  dump("get") ;
#endif /* DUMP_UNSOLVED */
  // Commentaire : cf T_ident::get_next_unsolved_ident()
  // Variable intermediaire utilisee pour la trace
  TRACE2("first_unsolved_ident %x (ident_type %d)",
		 first_unsolved_ident,
		 (first_unsolved_ident == NULL)
		 ? -1 : first_unsolved_ident->get_ident_type()) ;
  T_ident *res = ( 	(first_unsolved_ident == NULL)
					|| (first_unsolved_ident->get_ident_type() == ITYPE_UNKNOWN) )
	? first_unsolved_ident : first_unsolved_ident->get_next_unsolved_ident() ;

  TRACE2("T_object_manager(%x)::get_unsolved_idents()->%x", this, res) ;
  return res ;
}

// Obtention des commentaires non encore accroches dans l'arbre
T_lexem *T_object_manager::get_unsolved_comments(void)
{
#ifdef DEBUG_COMMENT
  TRACE2("T_object_manager(%x)::get_unsolved_comments()->%x",
		 this,
		 first_unsolved_comment) ;
#endif
  return first_unsolved_comment ;
}

// Prevenir que le premier commentaire de la liste a ete accroche
// ATTENTION ne marche que pour le premier commentaire non accroche
void T_object_manager::set_solved_comment(T_lexem *solved_comment)
{
#ifdef DEBUG_COMMENT
  TRACE5("T_object_manager(%x)::set_solved_comment()->%x (L%dC%d \"%s\")",
		 this,
		 solved_comment,
		 solved_comment->get_file_line(),
		 solved_comment->get_file_column(),
		 solved_comment->get_value()) ;
#endif


  if (first_unsolved_comment == NULL)
	{
	  // En mode multi-composants on a un reset apres l'analyse
	  // syntaxique de la machine. Si la machine a des commentaires en
	  // fin, on dit "set_solved_comment" alors que
	  // first_unsolved_comment = last_unsolved_comment = NULL. Dans
	  // ce cas, on ne fait rien
	  ASSERT(last_unsolved_comment == NULL) ;
	  TRACE0("commentaire de fin de machine - rien a faire") ;
	  return ;
	}

  TRACE2("ici first_unsolved_comment %x, solved_comment %x",
		 first_unsolved_comment, solved_comment) ;
  T_lexem *next_comment = solved_comment->get_next_unsolved_comment() ;
  T_lexem *prev_comment = solved_comment->get_prev_unsolved_comment() ;

  if (first_unsolved_comment == solved_comment)
	{
	  TRACE0("on passe au suivant") ;
	  first_unsolved_comment = next_comment ;
	}
  else
	{
	  T_lexem *prev_comment = solved_comment->get_prev_unsolved_comment() ;
	  TRACE3("ON DEROUTE ! prev_comment %x (expected NULL) solved_comment %x first_unsolved_comment %x",
			 prev_comment,
			 solved_comment,
			 first_unsolved_comment) ;
	  ASSERT(prev_comment != NULL) ; // garanti par le IF
	  prev_comment->set_next_unsolved_comment(next_comment) ;
	}

  TRACE2("ici last_unsolved_comment %x, solved_comment %x",
		 last_unsolved_comment, solved_comment) ;
  if (last_unsolved_comment == solved_comment)
	{
	  last_unsolved_comment = prev_comment ;
	}
  else
	{
	  T_lexem *next_comment = solved_comment->get_next_unsolved_comment() ;
	  ASSERT(next_comment != NULL) ; // garanti par le IF
	  next_comment->set_prev_unsolved_comment(prev_comment) ;
	}

}

// Ajouter un commentaire non resolu en fin de liste
void T_object_manager::add_unsolved_comment(T_lexem *comment)
{
#ifdef DEBUG_COMMENT
  TRACE6("T_object_manager(%x)::add_unsolved_comment()->%x %x %s:%d:%d",
		 this,
		 comment,
		 comment->get_value(),
		 (comment->get_file_name()->get_value() == NULL)
		  ? "null" : comment->get_file_name()->get_value(),
		 comment->get_file_line(),
		 comment->get_file_column()) ;
#endif

  TRACE2("first_unsolved_comment = %x last_unsolved_comment %x!",
		 first_unsolved_comment, last_unsolved_comment) ;
  if (first_unsolved_comment == NULL)
	{
	  TRACE0("maj de first_unsolved_comment") ;
	  first_unsolved_comment = comment ;
	}
  else
	{
	  last_unsolved_comment->set_next_unsolved_comment(comment) ;
	}
  comment->set_prev_unsolved_comment(last_unsolved_comment) ;
  last_unsolved_comment = comment ;

  TRACE3("ici first_unsolved_comment %x (adr %x), last_unsolved_comment %x",
		 first_unsolved_comment,
		 &first_unsolved_comment,
		 last_unsolved_comment) ;
}

// Ajouter un T_op_result
void T_object_manager::add_op_result(T_op_result *op_result)
{
#ifdef DEBUG_OP_RESULT
  TRACE3("T_object_manager(%x)::add_op_result()->%x : %s!",
		 this,
		 op_result,
		 op_result->get_class_name()) ;
#endif
  ASSERT(op_result->get_node_type() == NODE_OP_RESULT) ;
  if (first_op_result == NULL)
	{
	  first_op_result = op_result ;
	}
  else
	{
      // On teste que op_result n'est pas déjà dans la liste. Correction #28957
      T_op_result *cur = first_op_result ;
      do {
          if (cur == op_result)
              return;
          cur = cur->get_next_op_result();
      } while (cur != NULL);
	  last_op_result->set_next_op_result(op_result) ;
	}
  last_op_result = op_result ;
}

// Supprimeer un T_op_result
void T_object_manager::remove_op_result(T_op_result *op_result)
{
#ifdef DEBUG_OP_RESULT
  TRACE2("T_object_manager(%x)::remove_op_result()->%x", this, op_result) ;
#endif

  T_op_result *cur = first_op_result ;

  if (cur == NULL)
	{
	  // Rien a faire - il y a deja eu un reset
	  return ;
	}

  T_op_result *prev = NULL ;
  T_op_result *next = cur->get_next_op_result() ;

  while (cur != op_result)
	{
	  prev = cur ;
	  cur = next ;

#if 0
	  ASSERT(cur != NULL) ; // garanti par l'appelant
#else
	  // louche ... pas dans la liste
	  if (cur == NULL)
		{
		  return ;
		}
#endif
	  next = cur->get_next_op_result() ;
	}


  if (prev == NULL)
	{
	  first_op_result = next ;
	}
  else
	{
	  prev->set_next_op_result(next) ;
	}

  if (next == NULL)
	{
	  last_op_result = prev ;
	}

}

// Ajouter un identificateur en fin de liste
void T_object_manager::add_ident(T_ident *ident)
{
#ifdef DEBUG_IDENT
  TRACE3("T_object_manager(%x)::add_ident()->%x:%s",
		 this, ident, ident->get_name()->get_value()) ;
#endif

#ifdef DEBUG_IDENT
  TRACE2("objman_ident -- DEBUT DUMP AVANT AJOUT first %x last %x--", first_ident, last_ident) ;
  T_ident *cur = first_ident ;
  while (cur != NULL)
	{
	  TRACE4("%x <-- %x (%s) --> %x",
			 cur->get_prev_ident(),
			 cur, "???",
			 //			 cur->get_name()->get_value(),
			 cur->get_next_ident()) ;
	  cur = (T_ident *)cur->get_next_ident() ;
	}
  TRACE0("objman_ident --  FIN  DUMP AVANT AJOUT --") ;
#endif /* DEBUG_IDENT */

  if (first_ident == NULL)
	{
	  first_ident = ident ;
	}
  else
	{
	  last_ident->set_next_ident(ident) ;
	}
  ident->set_prev_ident(last_ident) ;
  ident->set_next_ident(NULL) ;
  last_ident = ident ;

#ifdef DEBUG_IDENT
  TRACE2("objman_ident -- DEBUT DUMP APRES AJOUT first %x last %x--", first_ident, last_ident) ;
  T_ident *after = first_ident ;
  while (after != NULL)
	{
	  TRACE4("%x <-- %x (%s) --> %x",
			 after->get_prev_ident(),
			 after,
			 after->get_name()->get_value(),
			 after->get_next_ident()) ;
	  after = (T_ident *)after->get_next_ident() ;
	}
  TRACE0("objman_ident --  FIN  DUMP APRES AJOUT --") ;
#endif /* DEBUG_IDENT */
}

// Supprimier un identificateur
void T_object_manager::delete_ident(T_ident *ident)
{
#ifdef DEBUG_IDENT
  TRACE3("T_object_manager(%x)::delete_ident()->%x:%s",
		 this, ident, ident->get_name()->get_value()) ;
#endif

#ifdef DEBUG_IDENT
  TRACE2("objman_ident -- DEBUT DUMP AVANT SUPPR %x last %x--", first_ident, last_ident) ;
  T_ident *cur = first_ident ;
  while (cur != NULL)
	{
	  TRACE4("%x <-- %x (%s) --> %x",
			 cur->get_prev_ident(),
			 cur,
			 cur->get_name()->get_value(),
			 cur->get_next_ident()) ;
	  cur = (T_ident *)cur->get_next_ident() ;
	}
  TRACE0("objman_ident --  FIN  DUMP AVANT SUPPR --") ;
#endif /* DEBUG_IDENT */

  T_ident *prev_ident = ident->get_prev_ident() ;
  T_ident *next_ident = ident->get_next_ident() ;

  if (first_ident == ident)
	{
	  first_ident = next_ident ;
	}
  else
	{
	  TRACE1("ici prev_ident %x", prev_ident) ;
	  TRACE1("next_ident %x", next_ident) ;
      if (prev_ident)
        {
          prev_ident->set_next_ident(next_ident) ;
        }
	}

  if (last_ident == ident)
	{
	  last_ident = prev_ident ;
	}
  else
	{
      if (next_ident)
        {
          next_ident->set_prev_ident(prev_ident) ;
        }
	}

  ident->set_prev_ident(NULL) ;
  ident->set_next_ident(NULL) ;

#ifdef DEBUG_IDENT
  TRACE2("objman_ident -- DEBUT DUMP APRES SUPPR first %x last %x--", first_ident, last_ident) ;
  T_ident *after = first_ident ;
  while (after != NULL)
	{
	  TRACE4("%x <-- %x (%s) --> %x",
			 after->get_prev_ident(),
			 after,
			 after->get_name()->get_value(),
			 after->get_next_ident()) ;
	  after = (T_ident *)after->get_next_ident() ;
	}
  TRACE0("objman_ident --  FIN  DUMP APRES SUPPR --") ;
#endif /* DEBUG_IDENT */
}

// Ajout d'un identificateur non resolu
void T_object_manager::add_abstract_type(T_abstract_type *abstract_type)
{
  ASSERT(abstract_type->get_node_type() == NODE_ABSTRACT_TYPE) ;
#ifdef DUMP_ABSTRACT
  TRACE2(">> T_object_manager(%x)::add_abstract_type(%x)",
		 this,
		 abstract_type) ;
  ENTER_TRACE ;
  TRACE3("@@ DEBUT DUMP AVANT AJOUT item %x first %x last %x", abstract_type, first_abstract_type, last_abstract_type) ;
  ENTER_TRACE ;
  T_abstract_type *cur = first_abstract_type ;
  while (cur != NULL)
	{
	  TRACE3("%08x <-- %08x --> %08x",
			 cur->get_prev_abstract_type(),
			 cur,
			 cur->get_next_abstract_type()) ;
	  cur = cur->get_next_abstract_type() ;
	}
  EXIT_TRACE ;
  TRACE3("@@  FIN  DUMP AVANT AJOUT item %x first %x last %x", abstract_type, first_abstract_type, last_abstract_type) ;
#endif /* DUMP_ABSTRACT */

  // Chainage en queue de l'abstract_type dans le gestionnaire d'objets
  if (first_abstract_type == NULL)
	{
	  first_abstract_type = abstract_type ;
	}
  else
	{
	  last_abstract_type->set_next_abstract_type(abstract_type) ;
	}
  abstract_type->set_prev_abstract_type(last_abstract_type) ;
  abstract_type->set_next_abstract_type(NULL) ;
  last_abstract_type = abstract_type ;

#ifdef DUMP_ABSTRACT
  TRACE3("@@ DEBUT DUMP APRES AJOUT item %x first %x last %x",
		 abstract_type, first_abstract_type, last_abstract_type) ;
  ENTER_TRACE ;
  cur = first_abstract_type ;
  while (cur != NULL)
	{
	  TRACE3("%08x <-- %08x --> %08x",
			 cur->get_prev_abstract_type(),
			 cur,
			 cur->get_next_abstract_type()) ;
	  cur = cur->get_next_abstract_type() ;
	}
  EXIT_TRACE ;
  TRACE3("@@  FIN  DUMP APRES AJOUT item %x first %x last %x",
		 abstract_type, first_abstract_type, last_abstract_type) ;
  EXIT_TRACE ;
  TRACE2("<< T_object_manager(%x)::add_abstract_type(%x)",
		 this,
		 add_abstract_type) ;
#endif /* DUMP_ABSTRACT */
}

// Supprimer un type abstrait
void T_object_manager::delete_abstract_type(T_abstract_type *abstract_type)
{
#ifdef DEBUG_ABSTRACT_TYPE
  TRACE3("T_object_manager(%x)::delete_abstract_type()->%x:%s",
		 this, abstract_type, abstract_type->get_name()->get_name()->get_value()) ;
#endif

#ifdef DUMP_ABSTRACT
  TRACE2(">> T_object_manager(%x)::delete_abstract_type(%x)",
		 this,
		 abstract_type) ;
  ENTER_TRACE ;
  TRACE3("@@ DEBUT DUMP AVANT SUPPRESSION item %x first %x last %x",
		 abstract_type, first_abstract_type, last_abstract_type) ;
  ENTER_TRACE ;
  T_abstract_type *cur = first_abstract_type ;
  while (cur != NULL)
	{
	  TRACE3("%08x <-- %08x --> %08x",
			 cur->get_prev_abstract_type(),
			 cur,
			 cur->get_next_abstract_type()) ;
	  cur = cur->get_next_abstract_type() ;
	}
  EXIT_TRACE ;
  TRACE3("@@  FIN  DUMP AVANT SUPPRESSION item %x first %x last %x",
		 abstract_type, first_abstract_type, last_abstract_type) ;
#endif /* DUMP_ABSTRACT */

  T_abstract_type *prev_abstract_type = abstract_type->get_prev_abstract_type() ;
  T_abstract_type *next_abstract_type = abstract_type->get_next_abstract_type() ;

  if (first_abstract_type == abstract_type)
	{
	  first_abstract_type = next_abstract_type ;
	}
  else
	{
	  prev_abstract_type->set_next_abstract_type(next_abstract_type) ;
	}

  if (last_abstract_type == abstract_type)
	{
	  last_abstract_type = prev_abstract_type ;
	}
  else
	{
	  next_abstract_type->set_prev_abstract_type(prev_abstract_type) ;
	}

#ifdef DUMP_ABSTRACT
  TRACE3("@@ DEBUT DUMP APRES SUPPRESSION item %x first %x last %x",
		 abstract_type, first_abstract_type, last_abstract_type) ;
  ENTER_TRACE ;
  cur = first_abstract_type ;
  while (cur != NULL)
	{
	  TRACE3("%08x <-- %08x --> %08x",
			 cur->get_prev_abstract_type(),
			 cur,
			 cur->get_next_abstract_type()) ;
	  cur = cur->get_next_abstract_type() ;
	}
  EXIT_TRACE ;
  TRACE3("@@  FIN  DUMP APRES SUPPRESSION item %x first %x last %x",
		 abstract_type, first_abstract_type, last_abstract_type) ;
#endif /* DUMP_ABSTRACT */
}



#ifdef WATCH
//
//	Services d'observation d'adresse memoire
//

static void **adr_watch_sop = NULL ;
static void *prev_value = NULL ;

// Affichage de la montre
extern void _show_watch(const char *file, int line)
{
  if (adr_watch_sop != NULL)
	{
	  if (*adr_watch_sop != prev_value)
		{
		  TRACE3("%s:%d ALARM ! watch change detected (was %x)",
				 (file == NULL) ? "NULL" : file,
				 line,
				 prev_value) ;
		}
	  prev_value = *adr_watch_sop ;
	}

  TRACE4("%s:%d watch -> %x * = %x",
		 file,
		 line,
		 adr_watch_sop,
		 (adr_watch_sop == NULL) ? 0 : *adr_watch_sop) ;
}

// Demande d'observation d'une adresse memoire
void watch(void **adr)
{
  TRACE1("--> watch(%x)", adr) ;
  adr_watch_sop = adr ;
  show_watch ;
}

// Obtention de l'adresse observee
void **get_watch(void)
{
  return adr_watch_sop ;
}

#endif

#ifdef DUMP_TYPES
void T_object_manager::dump_atype_list(void)
{
  TRACE0("DEBUT DUMP ABSTRACT_TYPES") ;
  ENTER_TRACE ;

  T_abstract_type *cur = first_abstract_type ;

  while (cur != NULL)
	{
	  TRACE2("%x -> %x", cur, cur->get_next_abstract_type()) ;
	  cur = cur->get_next_abstract_type() ;
	}

  EXIT_TRACE ;
  TRACE0(" FIN  DUMP ABSTRACT_TYPES") ;
}
#endif

// Mise a jour du dernier non resolu (avec troncage de la liste apres)
void
	T_object_manager::set_last_unsolved_ident(T_ident *new_last_unsolved_ident)
{
  TRACE3("T_object_manager(%x)::set_last_unsolved_ident(%x, was %x)",
		 this,
		 new_last_unsolved_ident,
		 last_unsolved_ident) ;
  if (new_last_unsolved_ident != NULL)
	{
	  last_unsolved_ident = new_last_unsolved_ident ;
	  last_unsolved_ident->set_next_unsolved_ident(NULL) ;
	}
}

// Mise a jour du dernier (avec troncage de la liste apres)
void
	T_object_manager::set_last_ident(T_ident *new_last_ident)
{
  TRACE2("T_object_manager::set_last_ident(%x:%s)",
		 new_last_ident,
		 new_last_ident->get_name()->get_value()) ;
  last_ident = new_last_ident ;
  last_ident->set_next_ident(NULL) ;
}


// Suppression de tous les lexems
void T_object_manager::unlink_lexems(void)
{
  TRACE0(">> T_object_manager::unlink_lexems") ;
  ENTER_TRACE ;

  T_object_tag *tag = first_tag ;

  while (tag != NULL)
	{
	  T_object_tag *next = tag->get_next_tag() ;
	  if (tag->get_object()->get_node_type() == NODE_LEXEM)
		{
		  delete tag->get_object() ;
		}

	  tag = next ;
	}

  EXIT_TRACE ;
  TRACE0("<< T_object_manager::unlink_lexems") ;
}
//
//	}{	Fin du fichier
//
