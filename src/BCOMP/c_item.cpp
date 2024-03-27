/***************************** CLEARSY **************************************
* Fichier : $Id: c_item.cpp,v 2.0 2005-04-25 10:43:55 cc Exp $
* (C) 2008 CLEARSY
*
* Compilations :	* -DDEBUG_LINKS pour voir les liens sur les objets
*					* -DDEBUG_COMMENT pour voir les commentaires
*					* -DDEBUG_LIST pour voir les trace de gestion de liste
*					* -DDEBUG_ITEM pour les autres traces
*					* -DSTOP_AT_LINK pour faire un assert a un link donne
*
* Description :		Classe de base T_item
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
RCS_ID("$Id: c_item.cpp,v 1.65 2005-04-25 10:43:55 cc Exp $") ;

/* Includes systeme */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

/* Includes Composant Local */
#include "c_api.h"

// Initialisation des champs
void T_item::init(T_item *new_father,
						   T_betree *new_betree,
						   T_lexem *new_ref_lexem)
{
#ifdef DEBUG_ITEM
  TRACE3("T_item::init(%x, %x, %x)", new_father, new_betree, new_ref_lexem) ;
#endif

  // Autres champs
  father = new_father ;

  betree = new_betree ;
  ref_lexem = new_ref_lexem ;
  if (ref_lexem != NULL)
	{
	  // ref_lexem == NULL pour le betree par exemple
	  ref_lexem->set_tmp2(TRUE) ;
	}
  first_comment = last_comment = NULL ;
  first_pragma = last_pragma = NULL ;

  // Pose d'un lien
  links = 1 ;

  // Resolution des commentaires

  if ( 	(get_node_type() != NODE_COMMENT) // Skip to my loop ...
		//		&& (get_node_type() != NODE_PRAGMA)
		&& (ref_lexem != NULL) 	 // NULL pour le T_betree
		&& (get_solve_comment() == TRUE) ) // on verifie que c'est pas desactive
	{
	  T_lexem *cur_comment = get_object_manager()->get_unsolved_comments() ;

	  // Eventuellement, on evite des commentaires dans le cas de
	  // l'expansion des definitions. Cf commentaires dans
	  // le code de T_item::check_comment()
	  cur_comment = check_comment(cur_comment) ;

#ifdef DEBUG_ITEM
	  if (cur_comment != NULL)
		{
		  TRACE0("on accroche les commentaires") ;
		}
	  else
		{
		  TRACE0("pas de commentaire a accrocher") ;
		}
#endif

	  while (cur_comment != NULL)
		{
		  if (cur_comment->get_file_name() != get_ref_lexem()->get_file_name())
			{
			  TRACE4("le commentaire %x <%s> du fichier %s et pas %s est ignore",
					 cur_comment,
					 cur_comment->get_value(),
					 cur_comment->get_file_name()->get_value(),
					 get_ref_lexem()->get_file_name()->get_value()) ;
			}
		  else
			{
			  if (cur_comment->is_before(ref_lexem) == TRUE)
				{
#ifdef DEBUG_COMMENT
				  TRACE6("on accroche le commentaire %x <%s> a l'item %x %d L%dC%d",
						 cur_comment,
						 cur_comment->get_value(),
						 this,
						 get_node_type(),
						 ref_lexem->get_file_line(),
						 ref_lexem->get_file_column()) ;
				  TRACE5("commentaire %x:<%s> %s:%d:%d",
						 cur_comment,
						 cur_comment->get_value(),
						 cur_comment->get_file_name()->get_value(),
						 cur_comment->get_file_line(),
						 cur_comment->get_file_column()) ;
#endif

				  if (get_dep_mode() == FALSE)
					{
                      (void)new T_comment(cur_comment->get_value(),
                                          cur_comment->is_a_kept_comment(),
                                          TRUE,
                                          // on veut analyser les pragmas
                                          // Pour la machine on utilise get_cur_machine,
                                          // qui est valide sauf si on est en cours de
                                          // constrution d'une machine (c'est aussi le cas
                                          // pour les clauses DEFINITIONS (voir commentaire
                                          // dans check_comment)), auquel cas on
                                          // est ici dans le ctr de la classe de base
                                          // et la variable statique de c_mach.cpp n'a
                                          // pas encore ete initialisee
                                          // Pour les definitions, on ne pourra pas autoriser les
                                          // pragmas car la machine qui va les referencer
                                          // n'existe pas encore
                                          (get_node_type() == NODE_DEFINITION)
                                          ? NULL :((get_node_type() == NODE_MACHINE)
                                                   ? (T_machine *)this : get_cur_machine()),
                                          (T_item **)&first_comment,
                                          (T_item **)&last_comment,
                                          this,
                                          betree,
                                          cur_comment) ;
					  get_object_manager()->set_solved_comment(cur_comment) ;
					}
				}
			  else
				{
#ifdef DEBUG_COMMENT
				  TRACE0("fin de l'accrochage des commentaires") ;
#endif

				  return ;
				}

			}
		  // fin du else, pendant du if qui teste que le
		  // commentaire est dans le bon fichier

		  cur_comment = cur_comment->get_next_unsolved_comment() ;
		}
	}

}


T_lexem *T_item::check_comment(T_lexem *cur_comment)
{
#ifdef DEBUG_COMMENT
TRACE1("T_item(%x)::check_comment()", this) ;
#endif

// Il faut ici considerer le cas particulier des definitions.
// Comme les elements de cette clause provoquent la creation
// d'instance de T_definition au cours de la phase d'analyse
// lexicale, c'est a dire avant toute autre creation d'items
// (T_machine, ...), alors la premiere instance de T_definition
// "avale" tous les commentaires du debut du fichier, jusqu'a
// elle. Ce n'est bien entendu pas le comportement recherche.

// Il faut rechercher le L_DEFINTIONS situe avant le lexeme
// courant, et ne considerer que les commentaires situes
// apres ce lexeme

// IMPORTANT ::
// On ne peut pas, a ce niveau, utiliser de fonction virtuelle
// car on est en train de construire l'instance et les pointeurs
// de fonction virtuelle ne sont pas encore a jour

#ifdef DEBUG_COMMENT
TRACE1("ici get_node_type()->%d", get_node_type()) ;
#endif

 if ( (get_node_type() == NODE_DEFINITION)
	  || (get_node_type() == NODE_FILE_DEFINITION)
	  || ( (get_node_type() == NODE_FLAG)
		   && (get_ref_lexem()->get_lex_type() == L_DEFINITIONS))
	  || ( (get_father() != NULL)
		   && (get_father()->get_node_type() == NODE_DEFINITION) ) )
   {
	 T_lexem *def_lexem = get_definitions_clause() ;

	 // Ici def_lexem peut etre NULL pendant la phase de
	 // reorganisation des definitions. Dans ce cas, on
	 // peut sortit tout de suite
	 if (def_lexem == NULL)
	   {
		 TRACE0("def_lexem = NULL -> on sort (phase de reorg des def ?)") ;
		 return NULL ;
	   }

	 ASSERT(def_lexem != NULL) ;

	 TRACE2("... localisation de la clause definition -> de L%dC%d",
			(def_lexem == NULL) ? -1 : def_lexem->get_file_line(),
			(def_lexem == NULL) ? -1 : def_lexem->get_file_column()) ;


#ifdef DEBUG_COMMENT
	 TRACE1("def_lexem = %x", def_lexem) ;
#endif

	 if (def_lexem != NULL)
	   {
		 // On saute tous les commentaires situes avant def_lexem
#ifdef DEBUG_COMMENT
		 TRACE2("localisation de DEFINITIONS : L%dC%d",
				def_lexem->get_file_line(),
				def_lexem->get_file_column()) ;
#endif
		 while ( 	 (cur_comment != NULL)
					 && (cur_comment->is_before(def_lexem) == TRUE) )
		   {
#ifdef DEBUG_COMMENT
			 TRACE3("on saute le commentaire L%dC%d \n\"%s\"",
					cur_comment->get_file_line(),
					cur_comment->get_file_column(),
					cur_comment->get_value()) ;
#endif
			 cur_comment = cur_comment->get_next_unsolved_comment() ;
		   }
	   }

#ifdef DEBUG_COMMENT
	 if (cur_comment == NULL)
	   {
		 TRACE0("il ne reste pas de commentaire a accrocher") ;
	   }
	 else
	   {
		 TRACE3("on accroche a partir de L%dC%d \n\"%s\"",
				cur_comment->get_file_line(),
				cur_comment->get_file_column(),
				cur_comment->get_value()) ;

	   }
#endif
	 return cur_comment ;
   }
 else if (get_node_type() == NODE_FLAG)
   {
	 if (get_ref_lexem()->get_lex_type() == L_DEFINITIONS)
	   {
		 cur_comment = get_ref_lexem()->get_real_prev_lexem() ;

		 T_lexem *res = NULL ;
		 while (    (cur_comment->get_lex_type() == L_COMMENT)
		   		 || (cur_comment->get_lex_type() == L_KEPT_COMMENT)
				 || (cur_comment->get_lex_type() == L_CPP_COMMENT) 
				 || (cur_comment->get_lex_type() == L_CPP_KEPT_COMMENT)
				 )
		   {
			 res = cur_comment ;
			 cur_comment = cur_comment->get_real_prev_lexem() ;
		   }

		 return res ;
	   }

	 return cur_comment ;
   }

 // Pas de traitement par defaut
#ifdef DEBUG_COMMENT
 TRACE0("cas par defaut : on ne fait rien") ;
#endif
 return cur_comment ;
}


// Constructeur avec chainage en queue
T_item::T_item(T_node_type new_node_type,
						T_item **adr_first,
						T_item **adr_last,
						T_item *new_father,
						T_betree *new_betree,
						T_lexem *new_ref_lexem)
  : T_object(new_node_type)
{
#ifdef DEBUG_ITEM
  TRACE6("T_item::T_item(%d, %x, %x, %x, %x, %x)",
		 new_node_type,
		 adr_first,
		 adr_last,
		 new_father,
		 new_betree,
		 new_ref_lexem) ;
#endif

  tail_insert(adr_first, adr_last) ;
  init(new_father, new_betree, new_ref_lexem) ;
}

// Constructeur avec chainage apres un element
T_item::T_item(T_node_type new_node_type,
						T_item *after_this_item,
						T_item **adr_first,
						T_item **adr_last,
						T_item *new_father,
						T_betree *new_betree,
						T_lexem *new_ref_lexem)
  : T_object(new_node_type)
{
  TRACE7("T_item::T_item(%d, %x, %x, %x, %x, %x, %x)",
		 new_node_type,
		 after_this_item,
		 adr_first,
		 adr_last,
		 new_father,
		 new_betree,
		 new_ref_lexem) ;

  insert_after(after_this_item, adr_first, adr_last) ;
  init(new_father, new_betree, new_ref_lexem) ;
}

T_item::~T_item(void)
{
  TRACE1("T_item(%x)::~T_item()", this) ;
}

//
//	}{	Chainage en tete d'une liste
//
void T_item::head_insert(T_item **adr_first, T_item **adr_last)
{
#ifdef DEBUG_ITEM
  TRACE3("T_item(%x)::head_insert(%x, %x)", this, adr_first, adr_last) ;
#endif

  // Chainage en tete le cas echeant
  if (adr_first != NULL)
	{
	  ASSERT(adr_last != NULL) ;

	  if (*adr_first == NULL)
		{
		  *adr_last = this ;
		  next = NULL ;
		}
	  else
		{
		  // Chainage en queue
		  (*adr_first)->prev = this ;
		  next = *adr_first ;
		}

	  prev = NULL ;
	  *adr_first = this ;
	}
  else
	{
	  // Pas de chainage, on met les pointeurs next/prev a NULL
	  next = NULL ;
	  prev = NULL ;
	}

}


//
//	}{	Chainage en queue d'une liste
//
void T_item::tail_insert(T_item **adr_first, T_item **adr_last)
{
#ifdef DEBUG_LIST
  TRACE3("T_item(%x)::tail_insert(%x, %x)", this, adr_first, adr_last) ;
#endif

#ifdef DEBUG_LIST
  if (adr_first != NULL)
	{
	  TRACE0("DEBUT DUMP AVANT") ; ENTER_TRACE ;
	  T_item *cur = *adr_first ;
	  while (cur != NULL)
		{
		  TRACE3("%x <-- cur %x --> %x", cur->prev, cur, cur->next) ;
		  cur = cur->next ;
		}
	  EXIT_TRACE ; TRACE0("FIN  DUMP AVANT") ;
	}
#endif

  // Chainage en queue le cas echeant
  if (adr_first != NULL)
	{
	  ASSERT(adr_last != NULL) ;

	  if (*adr_last == NULL)
		{
		  //
#ifdef DEBUG_LIST
		  TRACE1("mise a jour de %x", adr_first) ;
#endif
		  *adr_first = this ;
		  prev = NULL ;
		}
	  else
		{
		  // Chainage en queue
		  (*adr_last)->next= this ;
		  prev = *adr_last ;
		}

	  next = NULL ;
#ifdef DEBUG_LIST
		  TRACE1("mise a jour de %x", adr_last) ;
#endif
	  *adr_last = this ;
	}
  else
	{
	  // Pas de chainage, on met les pointeurs next/prev a NULL
	  next = NULL ;
	  prev = NULL ;
	}

#ifdef DEBUG_LIST
  if (adr_first != NULL)
	{
	  TRACE0("DEBUT DUMP APRES") ; ENTER_TRACE ;
	  T_item *cur = *adr_first ;
	  while (cur != NULL)
		{
		  TRACE3("%x <-- cur %x --> %x", cur->prev, cur, cur->next) ;
		  cur = cur->next ;
		}
	  EXIT_TRACE ; TRACE0("FIN  DUMP APRES") ;
	}
#endif
}

//
//	}{	Chainage apres un element d'une liste
//
void T_item::insert_after(T_item *after_this_item,
								   T_item **adr_first,
								   T_item **adr_last)
{
#ifdef DEBUG_LIST
  TRACE4("T_item(%x)::insert_after(%x,%x, %x)",
		 this,
		 after_this_item,
		 adr_first, adr_last) ;

  TRACE0("DEBUT DUMP AVANT") ; ENTER_TRACE ;
  T_item *cur = *adr_first ;
  while (cur != NULL)
	{
	  TRACE3("%x <-- cur %x --> %x", cur->prev, cur, cur->next) ;
	  cur = cur->next ;
	}
  EXIT_TRACE ; TRACE0("FIN  DUMP AVANT") ;
#endif

  if (after_this_item == NULL)
	{
	  // On insere dans une liste vide
#ifdef DEBUG_LIST
	  TRACE0("on insere en tete de liste") ;
#endif
	  next = *adr_first ;
	  if (next == NULL)
		{
		  // C'est aussi le dernier element de la liste
		  *adr_last = this ;
		}
	  else
		{
		  next->prev = this ;
		}
	  *adr_first = this ;
	  prev = NULL ;
	}
  else
	{
	  // Insertion dans une liste non vide
#ifdef DEBUG_LIST
	  TRACE0("on insere pas en tete") ;
#endif
	  next = after_this_item->next ;
	  after_this_item->next = this ;
	  prev = after_this_item ;
	  if (next != NULL)
		{
		  next->prev = this ;
		}

	  if (*adr_last == after_this_item)
		{
		  *adr_last = this ;
		}
	}

#ifdef DEBUG_LIST
TRACE0("DEBUT DUMP APRES") ; ENTER_TRACE ;

cur = *adr_first ;
 while (cur != NULL)
   {
	 TRACE3("%x <-- cur %x --> %x", cur->prev, cur, cur->next) ;
	 cur = cur->next ;
   }
 EXIT_TRACE ; TRACE0("FIN  DUMP APRES") ;
#endif
}

//
//	}{ Chainage avant un element d'une liste. Convention :
//	insert_before(NULL) = tail_insert
//
void T_item::insert_before(T_item *before_this_item,
									T_item **adr_first,
									T_item **adr_last)
{
#ifdef DEBUG_LIST
  TRACE4("T_item(%x)::insert_before(%x,%x, %x)",
		 this,
		 before_this_item,
		 adr_first, adr_last) ;

  TRACE0("DEBUT DUMP AVANT") ; ENTER_TRACE ;
  T_item *cur = *adr_first ;
  while (cur != NULL)
	{
	  TRACE3("%x <-- cur %x --> %x", cur->prev, cur, cur->next) ;
	  cur = cur->next ;
	}
  EXIT_TRACE ; TRACE0("FIN  DUMP AVANT") ;
#endif

  if (before_this_item == NULL)
	{
#ifdef DEBUG_LIST
	  TRACE0("appel de la methode generique de chainage en queue") ;
#endif
	  T_item::tail_insert(adr_first, adr_last) ;
	}
  else
	{
	  // Insertion dans une liste non vide
#ifdef DEBUG_LIST
	  TRACE0("on insere pas en tete") ;
#endif
	  prev = before_this_item->prev ;
	  before_this_item->prev = this ;
	  next = before_this_item ;
	  if (prev != NULL)
		{
		  prev->next = this ;
		}

	  if (*adr_first == before_this_item)
		{
		  *adr_first = this ;
		}
	}

#ifdef DEBUG_LIST
TRACE0("DEBUT DUMP APRES") ; ENTER_TRACE ;

cur = *adr_first ;
 while (cur != NULL)
   {
	 TRACE3("%x <-- cur %x --> %x", cur->prev, cur, cur->next) ;
	 cur = cur->next ;
   }
 EXIT_TRACE ; TRACE0("FIN  DUMP APRES") ;
#endif
}

//
//	}{	Phase de correction
// ref_this contient l'adresse du champ du pere a mettre a jour
T_item *T_item::check_tree(T_item **ref_this)
{
#ifdef DEBUG_CHECK
  TRACE2("T_item(%x::%s)::check_tree :: on ne fait rien",
		 this,
		 get_class_name()) ;
#endif
  return this ;
  assert(ref_this) ; // pour le warning
}

// Fonction qui enleve un item d'une liste chainee
void T_item::remove_from_list(T_item **adr_first,
									   T_item **adr_last)
{
#ifdef DEBUG_LIST
  TRACE5("remove_item_from_list(%x, %x(*%x), %x(*%x))",
		 this, adr_first, *adr_first, adr_last, *adr_last) ;
#endif

#ifdef DEBUG_LIST
  TRACE0("DEBUT DUMP AVANT") ; ENTER_TRACE ;
  T_item *cur = *adr_first ;
  while (cur != NULL)
	{
	  TRACE3("%x <-- cur %x --> %x", cur->prev, cur, cur->next) ;
	  cur = cur->next ;
	}
  EXIT_TRACE ; TRACE0("FIN  DUMP AVANT") ;
#endif

  if (prev == NULL)
	{
	  *adr_first = next ;
	}
  else
	{
	  prev->next = next ;
	}

  if (next == NULL)
	{
	  *adr_last = prev ;
	}
  else
	{
	  next->prev = prev ;
	}

  next = prev = NULL ;
  father = NULL ;

#ifdef DEBUG_LIST
  TRACE0("DEBUT DUMP APRES") ; ENTER_TRACE ;
  cur = *adr_first ;
  while (cur != NULL)
	{
	  TRACE3("%x <-- cur %x --> %x", cur->prev, cur, cur->next) ;
	  cur = cur->next ;
	}
  EXIT_TRACE ; TRACE0("FIN  DUMP APRES") ;
#endif
}

//
//	}{	Fonction qui remplace par un autre item item d'une liste chainee
//
void T_item::exchange_in_list(T_item *new_item,
									   T_item **adr_first,
									   T_item **adr_last)
{
  TRACE4("T_item(%x)::exchange_in_list(%x, %x, %x)",
		 this, new_item, adr_first, adr_last) ;
  TRACE4("ON REMPLACE %x:%s par %x:%s",
		 this,
		 get_class_name(),
		 new_item,
		 new_item->get_class_name()) ;

#ifdef DEBUG_LIST
TRACE2("-- DEBUT DUMP AVANT EXCHANGE de %x par %x--", this, new_item) ;
ENTER_TRACE ;
T_item *cur = *adr_first ;
while (cur != NULL)
  {
	TRACE4("%x <-- %x:%s --> %x", cur->prev, cur, get_class_name(), next) ;
	cur = cur->next ;
  }
EXIT_TRACE ;
TRACE0("-- FIN DUMP AVANT EXCHANGE --") ;
#endif

  if (prev == NULL)
	{
	  *adr_first = new_item ;
	}
  else
	{
	  prev->next = new_item ;
	}

  if (next == NULL)
	{
	  *adr_last = new_item ;
	}
  else
	{
	  next->prev = new_item ;
	}

  new_item->prev = prev ;
  new_item->next = next ;

#ifdef DEBUG_LIST
TRACE2("-- DEBUT DUMP APRES EXCHANGE de %x par %x --", this, new_item) ;
ENTER_TRACE ;
cur = *adr_first ;
while (cur != NULL)
  {
	TRACE4("%x <-- %x:%s --> %x", cur->prev, cur, get_class_name(), next) ;
	cur = cur->next ;
  }
EXIT_TRACE ;
TRACE0("-- FIN DUMP APRES EXCHANGE --") ;
#endif
}

//
//	}{	Inversion d'une liste
// IL FAUT QUE this SOIT LE PREMIER OBJET DE LA LISTE
//
void T_item::reverse_list(T_item **adr_first,
								   T_item **adr_last)
{
#ifdef DEBUG_LIST
  TRACE3("DEBUT T_item::reverse_list(%x,%x,%x)", this, adr_first, adr_last) ;
#endif

#ifdef DEBUG_LIST
TRACE0("-- DEBUT DUMP AVANT REVERSE") ;
ENTER_TRACE ;
 {
   T_item *cur = *adr_first ;
   while (cur != NULL)
	 {
	   TRACE4("%x <-- %x:%s --> %x", cur->prev, cur, get_class_name(), next) ;
	   cur = cur->next ;
	 }
 }
 EXIT_TRACE ;
 TRACE0("-- FIN DUMP AVANT REVERSE") ;
#endif

 T_item *tmp_first = NULL ;
 T_item *tmp_last = NULL ;

 T_item *cur = *adr_last ;

 while (cur != NULL)
   {
	 T_item *prev = cur->prev ;
	 cur->tail_insert((T_item **)&tmp_first, (T_item **)&tmp_last) ;
	 cur = prev ;
   }

 *adr_first = tmp_first ;
 *adr_last = tmp_last ;

#ifdef DEBUG_LIST
TRACE0("-- DEBUT DUMP APRES REVERSE") ;
ENTER_TRACE ;
 {
   T_item *cur = *adr_first ;
   while (cur != NULL)
	 {
	   TRACE4("%x <-- %x:%s --> %x", cur->prev, cur, get_class_name(), next) ;
	   cur = cur->next ;
	 }
 }
 EXIT_TRACE ;
TRACE0("-- FIN DUMP APRES REVERSE") ;
#endif


#ifdef DEBUG_LIST
  TRACE3("FIN T_item::reverse_list(%x,%x,%x)", this, adr_first, adr_last) ;
#endif
}



//
//	}{	Fonction auxiliaire qui extrait une liste de parametres
//
void T_item::extract_params(int separator,
									 T_item *new_father,
									 T_item **adr_first,
									 T_item **adr_last)
{
#ifdef TRACE_EXTRACT
  TRACE4("T_item::extract_params(%x, %d, %x, %x)",
		 this, separator, adr_first, adr_last) ;
  ENTER_TRACE ;
#endif

  // On chaine l'expression courante en queue

  if (*adr_first == NULL)
	{
	  *adr_first = this ;
	}
  else
	{
	  (*adr_last)->next = this ;
	}

  next = NULL ;
  prev = *adr_last ;
  *adr_last = this ;
  father = new_father ;

#ifdef DEBUG_ITEM
  EXIT_TRACE ;
  TRACE4("END :: extract_params(%x, %x, %x)", this, separator, adr_first, adr_last) ;
#endif

  // Pour le warning
  return ;
  separator = separator ;
}

//
//	}{	Fonction qui fabrique un item (expr ou pred) binaire
//
T_item *new_binary_item(T_item *op1,
							     T_item *op2,
							     T_lexem *oper,
							     T_betree *betree)
{
#ifdef DEBUG_ITEM
  TRACE5("new_binary_item(%x, %x, %x, %x, %s)",
		 op1, op2, oper, betree,
		 (oper == NULL) ? "NULL" : oper->get_lex_name()) ;
#endif
  ASSERT(oper) ;

  T_lex_type lex_type = oper->get_lex_type() ;

  // Une petite macro pour simplifier l'appel des constructeurs
#define LA_SUITE NULL, NULL, NULL, betree, oper
  TRACE1("lex_type=%d", lex_type) ;

  switch (lex_type)
	{
	  //
	  //	}{	Tout d'abord les cas triviaux qui declenchent la creation
	  //		d'instances de T_binary_op
	  //
	case L_AFFECT :
	  { return new T_affect(op1, op2, LA_SUITE) ; }
	case L_ANTICORESTRICTION :
	  { return new T_binary_op(op1, op2, BOP_ANTICORESTRICT, LA_SUITE) ; }
	case L_ANTIRESTRICTION :
	  { return new T_binary_op(op1, op2, BOP_ANTIRESTRICT, LA_SUITE) ; }
	case L_CONCAT :
	  { return new T_binary_op(op1, op2, BOP_CONCAT, LA_SUITE) ; }
	case L_CORESTRICTION :
	  { return new T_binary_op(op1, op2, BOP_CORESTRICT, LA_SUITE) ; }
	case L_DIVIDES :
	  { return new T_binary_op(op1, op2, BOP_DIVIDES, LA_SUITE) ; }
        case L_DIRECT_PRODUCT :
	  { return new T_binary_op(op1, op2, BOP_DIRECT_PRODUCT, LA_SUITE) ; }
	case L_HEAD_INSERT :
	  { return new T_binary_op(op1, op2, BOP_HEAD_INSERT, LA_SUITE) ; }
	case L_HEAD_RESTRICT :
	  { return new T_binary_op(op1, op2, BOP_HEAD_RESTRICT, LA_SUITE) ; }
	case L_INTERSECT :
	  { return new T_binary_op(op1, op2, BOP_INTERSECTION, LA_SUITE) ; }
	case L_LEFT_OVERLOAD :
	  { return new T_binary_op(op1, op2, BOP_LEFT_OVERLOAD, LA_SUITE) ; }
	case L_INTERVAL :
	  { return new T_binary_op(op1, op2, BOP_INTERVAL, LA_SUITE) ; }
	case L_MAPLET :{ return new T_binary_op(op1, op2, BOP_MAPLET, LA_SUITE) ; }
	case L_MINUS : { return new T_binary_op(op1, op2, BOP_MINUS, LA_SUITE) ; }
    case L_MOD :   { return new T_binary_op(op1, op2, BOP_MOD, LA_SUITE) ; }
	case L_LR_PARALLEL :
	  { return new T_binary_op(op1, op2, BOP_PARALLEL_PRODUCT, LA_SUITE) ; }
	case L_PIPE :  { return new T_binary_op(op1, op2, BOP_PIPE, LA_SUITE) ; }
	case L_PLUS :  { return new T_binary_op(op1, op2, BOP_PLUS, LA_SUITE) ; }
    case L_POWER : { return new T_binary_op(op1, op2, BOP_POWER, LA_SUITE) ; }
	case L_RESTRICTION :
	  { return new T_binary_op(op1, op2, BOP_RESTRICT, LA_SUITE) ; }
	case L_LR_SCOL :
	  { return new T_binary_op(op1, op2, BOP_COMPOSITION, LA_SUITE) ; }
	case L_SET_RELATION :
	  { return new T_binary_op(op1, op2, BOP_SET_RELATION, LA_SUITE) ; }
	case L_SURJ_RELATION :
	  { return new T_binary_op(op1, op2, BOP_SURJ_RELATION, LA_SUITE) ; }
	case L_TOTAL_RELATION :
	  { return new T_binary_op(op1, op2, BOP_TOTAL_RELATION, LA_SUITE) ; }
	case L_TOTAL_SURJ_RELATION :
	  { return new T_binary_op(op1, op2, BOP_TOTAL_SURJ_RELATION, LA_SUITE) ; }
	case L_TAIL_INSERT :
	  { return new T_binary_op(op1, op2, BOP_TAIL_INSERT, LA_SUITE) ; }
	case L_TAIL_RESTRICT :
	  { return new T_binary_op(op1, op2, BOP_TAIL_RESTRICT, LA_SUITE) ; }
	case L_TIMES :
	  {
		return new T_binary_op(op1, op2, BOP_TIMES, LA_SUITE) ;
	  }
        case L_UNION : { return new T_binary_op(op1, op2, BOP_UNION, LA_SUITE) ; }

	//
	//	}{	Tout d'abord les cas triviaux qui declenchent la creation
	//		d'instances de T_relation
	//
	case L_BIJECTION :
	  { return new T_relation(op1, op2, RTYPE_BIJECTION, LA_SUITE) ; }
	case L_PARTIAL_FUNCTION :
	  { return new T_relation(op1, op2, RTYPE_PARTIAL_FUNCTION, LA_SUITE) ; }
	case L_PARTIAL_INJECTION :
	  { return new T_relation(op1, op2, RTYPE_PARTIAL_INJECTION, LA_SUITE) ; }
	case L_PARTIAL_SURJECTION :
	  { return new T_relation(op1, op2, RTYPE_PARTIAL_SURJECTION, LA_SUITE) ; }
	case L_TOTAL_FUNCTION :
	  { return new T_relation(op1, op2, RTYPE_TOTAL_FUNCTION, LA_SUITE) ; }
	case L_TOTAL_INJECTION :
	  { return new T_relation(op1, op2, RTYPE_TOTAL_INJECTION, LA_SUITE) ; }
	case L_TOTAL_SURJECTION :
	  { return new T_relation(op1, op2, RTYPE_TOTAL_SURJECTION, LA_SUITE) ; }

	//
	//	}{	Ensuite les cas triviaux qui declenchent la creation
	//		d'instances de T_binary_predicate
	//
	case L_AND :
	  { return new T_binary_predicate(op1, op2, BPRED_CONJONCTION, LA_SUITE) ; }

	case L_IMPLIES :
	  { return new T_binary_predicate(op1, op2, BPRED_IMPLIES, LA_SUITE) ; }

	case L_EQUIVALENT :
	  { return new T_binary_predicate(op1, op2, BPRED_EQUIVALENT, LA_SUITE) ; }

	case L_L_OR :
	  { return new T_binary_predicate(op1, op2, BPRED_OR, LA_SUITE) ; }

	//
	//	}{	Ensuite les cas qui declenchent la creation d'instances
	//		de T_expr_predicate
	//
	case L_DIFFERENT :
	  { return new T_expr_predicate(op1, op2, EPRED_DIFFERENT, LA_SUITE) ; }

	case L_NOT_BELONGS :
	  { return new T_expr_predicate(op1, op2, EPRED_NOT_BELONGS, LA_SUITE) ; }

	case L_NOT_INCLUDED :
	  {
		return new T_expr_predicate(op1, op2, EPRED_NOT_INCLUDED, LA_SUITE) ;
	  }

	case L_NOT_STRICT_INCLUDED :
	  {
		return new T_expr_predicate(op1, op2, EPRED_NOT_STRICT_INCLUDED,
									LA_SUITE) ;
	  }

	case L_LESS_THAN :
	  { return new T_expr_predicate(op1, op2, EPRED_LESS_THAN, LA_SUITE) ; }

	case L_GREATER_THAN :
	  { return new T_expr_predicate(op1, op2, EPRED_GREATER_THAN, LA_SUITE) ; }

	case L_LESS_THAN_OR_EQUAL :
	  {
		return new T_expr_predicate(op1, op2, EPRED_LESS_THAN_OR_EQUAL,
									LA_SUITE) ;
	  }

	case L_GREATER_THAN_OR_EQUAL :
	  {
		return new T_expr_predicate(op1, op2, EPRED_GREATER_THAN_OR_EQUAL,
									LA_SUITE) ;
      }
	//
	//	}{	Ensuite les cas triviaux qui declenchent la creation
	//		d'instances de T_typing_predicate
	//
	case L_EQUAL :
          { T_node_type node_type = (T_node_type)(op1->get_node_type()) ;

              if (node_type == NODE_LITERAL_STRING) {
            return op2;
              }
              else {
                  return new T_typing_predicate(op1, op2, TPRED_EQUAL, LA_SUITE) ; }
          }

	case L_BELONGS :
	  { return new T_typing_predicate(op1, op2, TPRED_BELONGS, LA_SUITE) ; }

	case L_INCLUDED :
	  { return new T_typing_predicate(op1, op2, TPRED_INCLUDED, LA_SUITE) ; }

	case L_STRICT_INCLUDED :
	  { return new T_typing_predicate(op1, op2,TPRED_STRICT_INCLUDED,LA_SUITE) ; }

	//
	//	}{	Ensuite les cas plus complexes
	//
	case L_DOT :
	  {
		// Si op1 est % on construit une lambda-expression
		// Si op1 est ! on construit un predicat universel
		// Si op1 est SIGMA on construit un sigma
		// Si op1 est PI on construit un pi
		// Si op1 est UNION on construit un quantified union
		// Si op1 est INTER on construit un quantified intersection
		// Pour le savoir il faut tester le champ tmp de op1 qui vaut NODE_xxx
		T_node_type node_type = (T_node_type)(op1->get_tmp2()) ;

		switch(node_type)
		  {
                  case NODE_LITERAL_INTEGER :
                        { return new T_binary_op(op1, op2, BOP_PLUS, LA_SUITE) ;
                            //T_literal_real(op1 , op2, LA_SUITE) ;
                        }
		  case NODE_LAMBDA_EXPR :
			{ return new T_lambda_expr(op1 , op2, LA_SUITE) ; }

		  case NODE_UNIVERSAL_PREDICATE :
			{ return new T_universal_predicate(op1 , op2, LA_SUITE) ; }

		  case NODE_EXISTENTIAL_PREDICATE :
			{ return new T_existential_predicate(op1 , op2, LA_SUITE) ; }

		  case NODE_SIGMA :
                        { return new T_sigma(NODE_SIGMA,op1 , op2, LA_SUITE) ; }

		  case NODE_PI :
                        { return new T_pi(NODE_PI,op1 , op2, LA_SUITE) ; }

		  case NODE_QUANTIFIED_UNION :
			{ return new T_quantified_union(op1 , op2, LA_SUITE) ; }

		  case NODE_QUANTIFIED_INTERSECTION :
			{ return new T_quantified_intersection(op1 , op2, LA_SUITE) ; }

		  default :
			{
			  // ERREUR : construction avec un point inconnue
                          TRACE1("pb node_type %d", node_type) ;
			  syntax_error(oper,
						   CAN_CONTINUE,
						   get_error_msg(E_BAD_DOT_CONSTRUCT)) ;

			  // Si op1 est un ident, on essaye d'etre plus precis
			  if (op1->is_an_ident() == TRUE)
				{
				  syntax_error(op1->get_ref_lexem(),
							   CAN_CONTINUE,
							   get_error_msg(E_BAD_DOT_EXPLAIN)) ;
				}

			  stop() ;
			}
		  }
	  }

	case L_BECOMES_MEMBER_OF :
	  {
		T_substitution *res = new T_becomes_member_of(op1, op2, LA_SUITE) ;

		if ( (betree->get_root()->get_machine_type() == MTYPE_IMPLEMENTATION)
			 && (get_B0_allow_becomes_member_of() == FALSE) )
		  {
			// Cette substitution est interdite en implementation
			// CTRL Ref : CSYN 3-10
			syntax_error(res->get_ref_lexem(),
						 CAN_CONTINUE,
						 get_error_msg(E_BECOMES_MEMBER_OF_FORBIDDEN_IN_IMP)) ;
		  }

		return res ;
	  }

#ifdef BALBULETTE
	case L_BECOMES_SUCH_THAT :
	  {
		// On change le type du lexème pour la décompilation
		oper->set_lex_type(L_BELONGS) ;

		T_typing_predicate *pred = new T_typing_predicate(op1, op2, TPRED_BELONGS, LA_SUITE) ;
		T_substitution *res = new T_becomes_such_that(pred, NULL, betree) ;

		if ( (betree->get_root()->get_machine_type() == MTYPE_IMPLEMENTATION)
			 && (get_B0_allow_becomes_such_that() == FALSE) )
		  {
			// Cette substitution est interdite en implementation
			// CTRL Ref : CSYN 3-11
			syntax_error(res->get_ref_lexem(),
						 CAN_CONTINUE,
						 get_error_msg(E_BECOMES_SUCH_THAT_FORBIDDEN_IN_IMP)) ;
		  }

		return res ;
	  }
#endif

	case L_COMMA :
	  {
		/* Cas general : on construit un T_binary_op
		   Cas particulier : predicat de typagage du type xx, yy :
		   On a alors l'arbre :
		    ,
		   / \
		  x   :         <-- op2 (T_typing_predicate)
		  ^  / \
		  | y  []
		  |---------------- op1 (T_ident)

		  Dans ce cas, il faut ajouter x a la liste des parametres de op2 */

		if (op2->get_node_type() == NODE_TYPING_PREDICATE)
		  {
			if (    (op1->is_an_ident() == TRUE)
					|| (op1->get_node_type() == NODE_BINARY_OP) )
			  {
				T_item *first = NULL ;
				T_item *last = NULL ;
				op1->extract_params((int)BOP_COMMA, NULL, &first, &last) ;

				TRACE0("on enrichit op2 qui est un typing predicate") ;
				T_typing_predicate *tpred = (T_typing_predicate *)op2 ;

				// On parcourt la liste a l'envers car on
				// insere les parametres en tete de liste
				T_item *cur = last ;
				while (cur != NULL)
				  {
					T_ident *next = (T_ident *)cur->get_prev() ;
					if (cur->get_node_type() != NODE_IDENT)
					  {
						syntax_ident_expected_error(cur, CAN_CONTINUE) ;
					  }
					else
					  {
						TRACE1("on ajoute %s",
							   ((T_ident *)cur)->get_name()->get_value()) ;
					  }
					tpred->insert_param((T_ident *)cur) ;
					cur = next ;
				  }

				TRACE0("fin enrichissement") ;
				return op2 ;
			  }

			// Fall into
		  }
		return new T_binary_op(op1, op2, BOP_COMMA, LA_SUITE) ;
	  }
	case L_EVL :
	  {
		/*
		// A priori, on construit un T_op_res.
		// Cas particulier : op2 est un T_image
		// Dans ce cas, on enrichit ce T_image :
		//				EVL
		//             /   \
		//          Expr1  [ ]      <---- T_image deja construit = op2
		//            ||    |
		//			  op1  Expr2
		// Il faut construire un T_image en retournant a EVL pour trouver Expr1
		*/

		// Dans la construction des SIGMA, PI, ..., si on oublie le '.'
		// on tombe dans la creation d'un appel de fonction avec le
		// EVL des arguments. Ce n'est pas bon du tout, et c'est ici
		// que l'on peut le detecter : le champ tmp2 de op1 n'est
		// paS 0, il contient le node_type du SIGMA, PI, ... en
		// cours de creation.
		if (op1->get_tmp2() != 0)
		  {
			// Construction du style "sigma"
			syntax_error(op1->get_ref_lexem(),
						 CAN_CONTINUE,
						 get_error_msg(E_MISSING_DOT),
						 make_class_name((T_node_type)op1->get_tmp2())) ;
		  }

		if (op2->get_node_type() == NODE_IMAGE)
		  {
			TRACE1("on enrichit le T_image %x", op2) ;
			((T_image *)op2)->end_constructor(op1) ;
			return op2 ;
		  }
		else
		  {
			// Cas general : on construit un T_op_result
			// Le cas echeant, il sera transforme plus tard en T_op_call
			TRACE0("on construit un T_op_result") ;
			return new T_op_result(op1, op2, NULL, LA_SUITE) ;
		  }
	  }

	case L_RETURNS :
	  {
		/*
		// Cas d'un appel x, y <-- f(z, t)
		//
		// On a ::             <--
		//                 /       \
		//				T_ident	  T_op_res
		//
		// Il faut construire un T_op_call
		*/
		return new T_op_call(op1, op2, LA_SUITE) ;
	  }

	case L_LABEL_COL :
	  {
		return new T_record_item(op1, op2, LA_SUITE) ;
	  }

	case L_RECORD_ACCESS :
	  {
		return new T_record_access(op1, op2, LA_SUITE) ;
	  }

	default :
	  {
		// Erreur ! op non attendu ou pas encore implemente
		TRACE1("type %d pas implemente", lex_type) ;
		internal_error(oper,
					   __FILE__,
					   __LINE__,
					   get_error_msg(E_BINOP_TYPE_NOT_YET_IMPLEMENTED),
					   oper->get_lex_name(),
					   oper->get_lex_ascii()) ;
	  }
	}

  return NULL ;
}


// Une macro pour raccourcir le code des builtins
// A FAIRE : corriger ce code qui appelle link referencer l'objet
// ici ecrire tous les get_builtin necessaires pour eviter ce
// carnage !!!
T_op_result *MAKE_BUILTIN(T_list_link *link,
								   T_item *operand,
								   T_lexem *oper,
								   T_betree *betree)
{
  // Cast justifie par construction et verifie a posteriori
  T_ident *builtin = (T_ident *)link->get_object() ;
  ASSERT(builtin->is_an_ident() == TRUE) ;
  T_op_result *res = new T_op_result(NULL,
									 operand,
									 builtin,
									 NULL, NULL, NULL,
									 betree,
									 oper) ;
  link->unset_do_free() ;
  object_unlink(link) ;
  //  builtin->link() ; NON fait lors de l'appel a link_builtin...
  return res ;
}

//
//	}{	Fonction qui fabrique un item unaire
//
T_item *new_unary_item(T_item *operand,
								T_lexem *oper,
								T_betree *betree)
{
#ifdef DEBUG_ITEM
  TRACE4("new_unary_item(%x, %x, %x, %s)",
		 operand, oper, betree,
		 (oper == NULL) ? "NULL" : oper->get_lex_name()) ;
#endif
  ASSERT(oper) ;

  T_lex_type lex_type = oper->get_lex_type() ;

  TRACE2("ici oper %d:%s", lex_type, oper->get_lex_name()) ;

  switch (lex_type)
	{
	  //
	  //	Tout d'abord les operateurs unaires
	  //
	case L_UMINUS :
	  {
		// Cas particulier "- entier"
		if (operand->get_node_type() == NODE_LITERAL_INTEGER)
		  {
			T_literal_integer *litin = (T_literal_integer *)operand ;
			// On passe en oppose
			litin->set_opposite() ;
			return litin ;
		  }

		return new T_unary_op(operand,
							  UOP_MINUS,
							  NULL,
							  NULL,
							  NULL,
							  betree,
							  oper) ;
	  }
        case L_LAMBDA :
	  {
		// On ne renvoie que l'operande. La lambda-expression est construite
		// lorsque le L_DOT est rencontre
		//		TRACE0("ON MARQUE LE LAMBDA") ;
		operand->set_tmp2((int)NODE_LAMBDA_EXPR) ;
		return operand ;
	  } ;

	case L_SIGMA :
	  {
		// On ne renvoie que l'operande. Le sigma est construit
		// lorsque le L_DOT est rencontre.
                operand->set_tmp2((int)NODE_SIGMA) ;
		return operand ;
	  } ;

	case L_PI :
	  {
		// On ne renvoie que l'operande. Le pi est construit
		// lorsque le L_DOT est rencontre.
                operand->set_tmp2((int)NODE_PI) ;
		return operand ;
	  } ;

	case L_QUNION :
	  {
		// On ne renvoie que l'operande. Le quantified_union est construit
		// lorsque le L_DOT est rencontre.
		operand->set_tmp2((int)NODE_QUANTIFIED_UNION) ;
		return operand ;
	  } ;

	case L_QINTER :
	  {
		// On ne renvoie que l'operande. Le quantified_intersection est construit
		// lorsque le L_DOT est rencontre.
		operand->set_tmp2((int)NODE_QUANTIFIED_INTERSECTION) ;
		return operand ;
	  } ;

	case L_FORALL :
	  {
		// On ne renvoie que l'operande. Le predicat universel est construit
		// lorsque le L_DOT est rencontre
		operand->set_tmp2((int)NODE_UNIVERSAL_PREDICATE) ;
		return operand ;
	  } ;

	case L_EXISTS :
	  {
		// On ne renvoie que l'operande. Le predicat existentiel est construite
		// lorsque le L_DOT est rencontre
		operand->set_tmp2((int)NODE_EXISTENTIAL_PREDICATE) ;
		return operand ;
	  } ;

	case L_L_NOT :
	  {
		// On fait toujours un not_predicate
		// Car a l'interieur on a peut etre un futur predicat qui n'en est
		// pas encore un
		return new T_not_predicate((T_predicate *)operand,
								   NULL,
								   NULL,
								   NULL,
								   betree,
								   oper) ;
	  } ;

	case L_REC :
	  {
		return new T_record(operand, NULL, betree, oper) ;
	  }

	case L_STRUCT :
	  {
		return new T_struct(operand, NULL, betree, oper) ;
	  }

	// Reste :: les fonctions builtin
	case L_BOOL :
	  { return MAKE_BUILTIN(link_builtin_bool(oper, betree),
							operand,
							oper,
							betree) ; } ;
	case L_SUCC :
	  { return MAKE_BUILTIN(link_builtin_succ(oper, betree),
							operand,
							oper,
							betree) ; } ;
	case L_PRED :
	  { return MAKE_BUILTIN(link_builtin_pred(oper, betree),
							operand,
							oper,
							betree) ; } ;
	case L_MAX :
	  { return MAKE_BUILTIN(link_builtin_max(oper, betree),
							operand,
							oper,
							betree) ; } ;
	case L_MIN :
	  { return MAKE_BUILTIN(link_builtin_min(oper, betree),
							operand,
							oper,
							betree) ; } ;

        case L_REALOP :
          { return MAKE_BUILTIN(link_builtin_real(oper, betree),
                                                        operand,
                                                        oper,
                                                        betree) ; } ;
        case L_FLOOR :
          { return MAKE_BUILTIN(link_builtin_floor(oper, betree),
                                                        operand,
                                                        oper,
                                                        betree) ; } ;
        case L_CEILING :
          { return MAKE_BUILTIN(link_builtin_ceiling(oper, betree),
                                                        operand,
                                                        oper,
                                                        betree) ; } ;
	case L_CARD :
	  { return MAKE_BUILTIN(link_builtin_card(oper, betree),
							operand,
							oper,
							betree) ; } ;
	case L_GUNION :
	  { return MAKE_BUILTIN(link_builtin_gunion(oper, betree),
							operand,
							oper,
							betree) ; } ;
	case L_GINTER :
	  { return MAKE_BUILTIN(link_builtin_ginter(oper, betree),
							operand,
							oper,
							betree) ; } ;
	case L_ID :
	  { return MAKE_BUILTIN(link_builtin_id(oper, betree),
							operand,
							oper,
							betree) ; } ;
	case L_PRJ1 :
	  { return MAKE_BUILTIN(link_builtin_prj1(oper, betree),
							operand,
							oper,
							betree) ; } ;
	case L_PRJ2 :
	  { return MAKE_BUILTIN(link_builtin_prj2(oper, betree),
							operand,
							oper,
							betree) ; } ;
	case L_DOM :
	  { return MAKE_BUILTIN(link_builtin_dom(oper, betree),
							operand,
							oper,
							betree) ; } ;
	case L_RAN :
	  { return MAKE_BUILTIN(link_builtin_ran(oper, betree),
							operand,
							oper,
							betree) ; } ;
	case L_SIZE :
	  { return MAKE_BUILTIN(link_builtin_size(oper, betree),
							operand,
							oper,
							betree) ; } ;
	case L_FIRST :
	  { return MAKE_BUILTIN(link_builtin_first(oper, betree),
							operand,
							oper,
							betree) ; } ;
	case L_LAST :
	  { return MAKE_BUILTIN(link_builtin_last(oper, betree),
							operand,
							oper,
							betree) ; } ;
	case L_FRONT :
	  { return MAKE_BUILTIN(link_builtin_front(oper, betree),
							operand,
							oper,
							betree) ; } ;
	case L_TAIL :
	  { return MAKE_BUILTIN(link_builtin_tail(oper, betree),
							operand,
							oper,
							betree) ; } ;
	case L_REV :
	  { return MAKE_BUILTIN(link_builtin_rev(oper, betree),
							operand,
							oper,
							betree) ; } ;
	case L_POW :
	  { return MAKE_BUILTIN(link_builtin_pow(oper, betree),
							operand,
							oper,
							betree) ; } ;
	case L_POW1 :
	  { return MAKE_BUILTIN(link_builtin_pow1(oper, betree),
							operand,
							oper,
							betree) ; } ;
	case L_FIN :
	  { return MAKE_BUILTIN(link_builtin_fin(oper, betree),
							operand,
							oper,
							betree) ; } ;
	case L_FIN1 :
	  { return MAKE_BUILTIN(link_builtin_fin1(oper, betree),
							operand,
							oper,
							betree) ; } ;
	case L_TRANS_FUNC :
	  { return MAKE_BUILTIN(link_builtin_trans_func(oper, betree),
							operand,
							oper,
							betree) ; } ;
	case L_TRANS_REL :
	  { return MAKE_BUILTIN(link_builtin_trans_rel(oper, betree),
							operand,
							oper,
							betree) ; } ;
	case L_SEQUENCE :
	  { return MAKE_BUILTIN(link_builtin_sequence(oper, betree),
							operand,
							oper,
							betree) ; } ;
	case L_NON_EMPTY_SEQUENCE :
	  { return MAKE_BUILTIN(link_builtin_non_empty_sequence(oper, betree),
							operand,
							oper,
							betree) ; } ;
	case L_INJECTIVE_SEQ :
	  { return MAKE_BUILTIN(link_builtin_injective_seq(oper, betree),
							operand,
							oper,
							betree) ; } ;
	case L_NON_EMPTY_INJECTIVE_SEQ :
	  { return MAKE_BUILTIN(
			link_builtin_non_empty_injective_seq(oper, betree),
			operand,
			oper,
			betree) ; } ;
	case L_PERMUTATION :
	  { return MAKE_BUILTIN(link_builtin_permutation(oper, betree),
							operand,
							oper,
							betree) ; } ;
	case L_GENERALISED_CONCAT :
	  { return MAKE_BUILTIN(link_builtin_generalised_concat(oper, betree),
							operand,
							oper,
							betree) ; };
	case L_ITERATE :
	  { return MAKE_BUILTIN(link_builtin_iterate(oper, betree),
							operand,
							oper,
							betree) ; } ;
	case L_CLOSURE :
	  { return MAKE_BUILTIN(link_builtin_closure(oper, betree),
							operand,
							oper,
							betree) ; } ;
	case L_CLOSURE1 :
	  { return MAKE_BUILTIN(link_builtin_closure1(oper, betree),
							operand,
							oper,
							betree) ; } ;

	case L_TREE :
	  { return MAKE_BUILTIN(link_builtin_tree(oper, betree),
							operand,
							oper,
							betree) ; } ;

	case L_BTREE :
	  { return MAKE_BUILTIN(link_builtin_btree(oper, betree),
							operand,
							oper,
							betree) ; } ;

	case L_CONST :
	  { return MAKE_BUILTIN(link_builtin_const(oper, betree),
							operand,
							oper,
							betree) ; } ;

	case L_TOP :
	  { return MAKE_BUILTIN(link_builtin_top(oper, betree),
							operand,
							oper,
							betree) ; } ;

	case L_SONS :
	  { return MAKE_BUILTIN(link_builtin_sons(oper, betree),
							operand,
							oper,
							betree) ; } ;

	case L_PREFIX :
	  { return MAKE_BUILTIN(link_builtin_prefix(oper, betree),
							operand,
							oper,
							betree) ; } ;

	case L_POSTFIX :
	  { return MAKE_BUILTIN(link_builtin_postfix(oper, betree),
							operand,
							oper,
							betree) ; } ;

	case L_SIZET :
	  { return MAKE_BUILTIN(link_builtin_sizet(oper, betree),
							operand,
							oper,
							betree) ; } ;

	case L_MIRROR :
	  { return MAKE_BUILTIN(link_builtin_mirror(oper, betree),
							operand,
							oper,
							betree) ; } ;

	case L_RANK :
	  { return MAKE_BUILTIN(link_builtin_rank(oper, betree),
							operand,
							oper,
							betree) ; } ;

	case L_FATHER :
	  { return MAKE_BUILTIN(link_builtin_father(oper, betree),
							operand,
							oper,
							betree) ; } ;

	case L_SON :
	  { return MAKE_BUILTIN(link_builtin_son(oper, betree),
							operand,
							oper,
							betree) ; } ;

	case L_SUBTREE :
	  { return MAKE_BUILTIN(link_builtin_subtree(oper, betree),
							operand,
							oper,
							betree) ; } ;

	case L_ARITY :
	  { return MAKE_BUILTIN(link_builtin_arity(oper, betree),
							operand,
							oper,
							betree) ; } ;

	case L_BIN :
	  { return MAKE_BUILTIN(link_builtin_bin(oper, betree),
							operand,
							oper,
							betree) ; } ;

	case L_LEFT :
	  { return MAKE_BUILTIN(link_builtin_left(oper, betree),
							operand,
							oper,
							betree) ; } ;

	case L_RIGHT :
	  { return MAKE_BUILTIN(link_builtin_right(oper, betree),
							operand,
							oper,
							betree) ; } ;

	case L_INFIX :
	  { return MAKE_BUILTIN(link_builtin_infix(oper, betree),
							operand,
							oper,
							betree) ; } ;
  case L_REF :
    { return MAKE_BUILTIN(link_builtin_ref(oper, betree),
                                                  operand,
                                                  oper,
                                                  betree) ; } ;

	default :
	  {
		internal_error(oper,
					   __FILE__,
					   __LINE__,
					   get_error_msg(E_UNOP_TYPE_NOT_YET_IMPLEMENTED),
					   oper->get_lex_ascii()) ;
	  }
	}

  return NULL ;
}

//
//	}{	Fonction qui fabrique un item a partir d'une item parenthesee
//
T_item *T_item::new_paren_item(T_betree *betree,
										T_lexem *opn,
										T_lexem *clo)
{
  TRACE2("T_item(%x:%s)::new_paren_item", this, get_class_name()) ;

  syntax_unexpected_error(get_ref_lexem(),
						  FATAL_ERROR,
						  get_catalog(C_EXPR_OR_PRED)) ;
  // pour eviter les warnings
  assert(opn) ;
  assert(clo) ;
  assert(betree) ;
  return NULL ;
}

// Pour savoir si un objet est un item
int T_item::is_an_item(void)
{
  return TRUE ;
}

// Pour savoir si un item  est une expression
int T_item::is_an_expr(void)
{
  return FALSE ;
}

// Pour savoir si un item  est une operation
int T_item::is_an_operation(void)
{
  return FALSE ;
}

// Pour savoir si un item  est une substitution
int T_item::is_a_substitution(void)
{
  return FALSE ;
}

// Pour savoir si un item  est un predicate
int T_item::is_a_predicate(void)
{
  TRACE2("T_item(%x:%s)::is_a_predicate() -> FALSE", this, get_class_name()) ;
  return FALSE ;
}

// Pour savoir si un item  est un identificateur
int T_item::is_an_ident(void)
{
  return FALSE ;
}

// Pour savoir si un predicat est un predicat de typage de
// parametre de machine - ce n'est pas le cas par defaut !
int T_item::is_a_machine_param_typing_predicate(void)
{
  return FALSE ;
}

// Pour savoir si un predicat est un predicat de typage de
// constante - ce n'est pas le cas par defaut !
int T_item::is_a_constant_typing_predicate(void)
{
  return FALSE ;
}

// Pour savoir si un predicat est un predicat de typage de
// variable - ce n'est pas le cas par defaut !
int T_item::is_a_variable_typing_predicate(void)
{
  return FALSE ;
}

// Acces au lexeme precedent dans le fichier
// Pas en inline dans le .h car pb de references cycliques
T_lexem *T_item::get_before_ref_lexem(void)
{
  return ref_lexem->get_prev_lexem() ;
}

// Acces au pragma non traduits
T_pragma *T_item::get_pragmas(void)
{
  T_pragma *res = first_pragma ;
  while ( (res != NULL) && (res->get_translated() == TRUE) )
	{
	  res = res->get_next_pragma() ;
	}
  return res ;
}

#ifdef STOP_AT_LINK // pour faire un assert a un link done
static int cpt_links_si = 0 ;
static int stop_at_cpt_links_si = 0 ;

void set_stop_link_at_number(int new_number)
{
  printf("set_stop_at_link_number(%d)\n", new_number) ;
  stop_at_cpt_links_si = new_number ;
}
#endif // STOP_AT_LINK

#ifdef STOP_AT_UNLINK // pour faire un assert a un link done
static int cpt_unlinks_si = 0 ;
static int stop_at_cpt_unlinks_si = 0 ;
void set_stop_unlink_at_number(int new_number)
{
  printf("set_stop_at_unlink_number(%d)\n", new_number) ;
  stop_at_cpt_unlinks_si = new_number ;
}
#endif // STOP_AT_UNLINK

// Poser, retirer un lien
T_item *T_item::link(void)
{
#ifdef DEBUG_LINKS
  TRACE2("T_item::link(%x) : il y a maintenant %d liens", this, links + 1) ;
#ifdef STOP_AT_LINK
  TRACE2("c'est le link numero %d (stop_at_cpt_links_si %d)",
		 ++cpt_links_si,
		 stop_at_cpt_links_si) ;
  assert(cpt_links_si != stop_at_cpt_links_si) ;
#endif
#endif
  links ++ ;
  return this ;
} ;

void T_item::unlink(void)
{
#ifdef DEBUG_LINKS
  TRACE2("T_item::unlink(%x) : il y a maintenant %d liens", this, links - 1) ;
  ASSERT(links > 0) ;
#endif
#ifdef STOP_AT_UNLINK
  TRACE2("c'est le unlink numero %d (stop_at_cpt_unlinks_si %d)",
		 ++cpt_unlinks_si,
		 stop_at_cpt_unlinks_si) ;
  assert(cpt_unlinks_si != stop_at_cpt_unlinks_si) ;
#endif

}

// Recherche de l'operation et de la machine de definition d'un item
void T_item::find_machine_and_op_def(T_machine **adr_ref_machine,
											  T_op **adr_ref_op)
{
#ifdef DEBUG_FIND_MACHINE_AND_OP_DEF
  TRACE3("T_item(%x:%s:%d)::find_machine_and_op_def",
		 this, get_class_name(), get_node_type()) ;
#endif

  T_item *cur = this ; // plus get_father() (cas de la machine)
  if (adr_ref_op != NULL)
	{
	  *adr_ref_op = NULL ;
	}
  *adr_ref_machine = NULL ;

  // Ce code serait naif si on ne savait pas que le B interdit d'emboiter
  // les operations, et que la structure du betree, vue des feuilles
  // en le remontant par le get_father, est soit
  // betree->machine->operation->substitution ou betree->machine->clause
  while (cur != NULL)
	{
#ifdef DEBUG_FIND_MACHINE_AND_OP_DEF
	  TRACE2("ici cur = %x : %s", cur, cur->get_class_name()) ;
#endif
	  if (cur->get_node_type() == NODE_MACHINE)
		{
#ifdef DEBUG_FIND_MACHINE_AND_OP_DEF
		  TRACE1("maj de machine avec %x", cur) ;
#endif
		  *adr_ref_machine = (T_machine *)cur ;
		}
	  else if (cur->get_node_type() == NODE_OPERATION)
		{
#ifdef DEBUG_FIND_MACHINE_AND_OP_DEF
		  TRACE1("maj de op avec %x", cur) ;
#endif
		  if (adr_ref_op != NULL)
			{
			  *adr_ref_op = (T_op *)cur ;
			}
		}

#ifdef DEBUG_FIND_MACHINE_AND_OP_DEF
	  TRACE2("suivant : cur %x, next_cur = get_father %x", cur, cur->get_father()) ;
#endif
	  cur = cur->get_father() ;
	}

#ifdef DEBUG_FIND_MACHINE_AND_OP_DEF
  TRACE4("on renvoie mach %x:%s op %x:%s",
		 *adr_ref_machine,
		 (*adr_ref_machine == NULL) ? "" : (*adr_ref_machine)->get_class_name(),
		 (adr_ref_op == NULL) ? NULL : *adr_ref_op,
		 ( (adr_ref_op == NULL) || (*adr_ref_op == NULL)) ? "" : (*adr_ref_op)->get_class_name()) ;
#endif
}

// Demande de l'acces necessaire pour le fils ref
// Ex: si this est un T_affect et que ref est en partie gauche,
// alors la fonction renvoie AUTH_READ_WRITE
// Ex: si this est un predicat, renvoie AUTH_READ
T_access_authorisation T_item::get_auth_request(T_item *ref)
{
  TRACE3("T_item(%s)::get_auth_request(%x, %s) -> AUTH_READ",
		 get_class_name(),
		 ref,
		 ref->get_class_name()) ;

  return AUTH_READ ;
}

T_type *T_item::get_B_type(void)
{
  TRACE2("T_item(%x:%s)::get_B_type() PAS NORMAL !", this, get_class_name()) ;

  semantic_error(this,
				 CAN_CONTINUE,
				 get_error_msg(E_NOT_AN_EXPRESSION),
				 make_class_name(this)) ;
  return NULL ;	// pour le warning
} ;

void T_item::set_B_type(T_type *, T_lexem *)
{
#ifdef DEBUG_ITEM
  TRACE3("T_item(%x:%s)::set_B_type(%x) on ne fait rien !",
		 this, get_class_name(), new_B_type) ;
#endif
  return ;
}


// On verifie que la methode n'est pas appelee avec un type !
#ifndef NO_CHECKS
#define CHECK_TYPE
//T_node_type type = get_node_type() ;
//ASSERT(type != NODE_TYPE) ;
//ASSERT(type != NODE_CONSTRUCTOR_TYPE) ;
//ASSERT(type != NODE_SETOF_TYPE) ;
//ASSERT(type != NODE_PRODUCT_TYPE) ;
//ASSERT(type != NODE_GENERIC_TYPE) ;
//ASSERT(type != NODE_BASE_TYPE) ;
//ASSERT(type != NODE_PREDEFINED_TYPE) ;
//ASSERT(type != NODE_ABSTRACT_TYPE) ;
//ASSERT(type != NODE_ENUMERATED_TYPE) ;
#else /* NO_CHECKS */
#define CHECK_TYPE
#endif /* !NO_CHECKS */




// Est-ce un ensemble ?
int T_item::is_a_set(void)
{
  TRACE2("T_item(%x::%s)::is_a_set ?", this, get_class_name()) ;
  //  CHECK_TYPE ;

  return FALSE ;
}

int T_item::is_a_relation(void)
{
  TRACE2("T_item(%x::%s)::is_a_relation ?", this, get_class_name()) ;
  CHECK_TYPE ;

  return FALSE ;
}

// Est-ce un ensemble ?
int T_item::is_a_seq(void)
{
  TRACE2("T_item(%x::%s)::is_a_seq ?", this, get_class_name()) ;

  // CHECK_TYPE ;

  return FALSE ;
}

// Est-ce un ensemble ?
int T_item::is_a_non_empty_seq(void)
{
  TRACE2("T_item(%x::%s)::is_a_seq ?", this, get_class_name()) ;
  CHECK_TYPE ;

  return FALSE ;
}

// Est-ce un ensemble d'entiers ?
int T_item::is_an_integer_set(void)
{
  TRACE2("T_item(%x::%s)::is_an_integer_set ?", this, get_class_name()) ;
  CHECK_TYPE ;

  return FALSE ;
}

// Est-ce un ensemble de réels ?
int T_item::is_a_real_set(void)
{
  TRACE2("T_item(%x::%s)::is_a_real_set ?", this, get_class_name()) ;
  CHECK_TYPE ;

  return FALSE ;
}

// Est-ce un ensemble ?
void T_item::make_type(void)
{
#ifdef DEBUG_ITEM
  TRACE2("T_item(%x::%s)::make_type : ON NE FAIT RIEN", this, get_class_name()) ;
#endif
}

// Verification de typage
// Utilise pour verifier qu'un identificateur est type avant d'etre
// accede. Ne fait rien dans le cas general
void T_item::check_type()
{
#ifdef DEBUG_ITEM
  TRACE2("T_item(%x:%s)::check_type : on ne fait rien", this, get_class_name()) ;
#endif
}

// Fonction qui fabrique le type d'un item
T_symbol *make_type_name(T_item *item)
{
  if (item == NULL)
	{
	  static T_symbol *null_type = lookup_symbol("null type") ;
	  return null_type ;
	}
  //  TRACE2("make_type_name(%x, class %s)", item, item->get_class_name()) ;

  if (item->is_an_expr() == TRUE)
	{
	  T_type *type = item->get_B_type() ;

	  if (type == NULL)
		{
		  static T_symbol *no_type = lookup_symbol(get_catalog(C_NO_TYPE_GIVEN)) ;
		  return no_type ;
		}
	  else
		{
		  return type->make_type_name() ;
		}
	}
  else if (item->is_a_type() == TRUE)
	{
	  return ((T_type *)item)->make_type_name() ;
	}
  else
	{
	  return lookup_symbol(make_class_name(item)) ;
	}
}

int T_item::is_a_constant(int ask_def)
{
  TRACE2("T_item(%x::%s)::is_a_constant ?", this, get_class_name()) ;

  assert(FALSE) ; // A FAIRE
  return FALSE ; // pour le warning
  ask_def = ask_def ;
}
int T_item::is_a_variable(int ask_def)
{
  TRACE2("T_item(%x::%s)::is_a_variable ?", this, get_class_name()) ;

  assert(FALSE) ; // A FAIRE
  return FALSE ; // pour le warning
  ask_def = ask_def ;
}

// Renvoie l'identificateur "contenu" dans this
// i.e. this pour la classe T_ident et sous-classes
//      this->object pour T_list_link qui contient un ident, ...
// prerequis : is_an_ident == TRUE
T_ident *T_item::make_ident(void)
{
  assert(FALSE) ;
  return NULL ; // pour Visual-C++
}

// Renvoie l'expression "contenue" dans this
// i.e. this pour la classe T_expr et sous-classes
//      this->object pour T_list_link qui contient une expr, ...
// prerequis : is_an_expr == TRUE
T_expr *T_item::make_expr(void)
{
  if (get_error_count() != 0)
	{
	  // ... c'est certainement une erreur precedente
	  // qui nous a envoye sur cette mauvaise voie ...
	  TRACE2("T_item(%x:%s)::make_expr -> INTERNAL ERROR reprise sur erreur",
			 this,
			 get_class_name()) ;
	  // ... dont on ne sait pas sortir proprement
	  toplevel_error(FATAL_ERROR,
					 get_error_msg(E_CONFUSED),
					 get_error_count()) ;
	}

#ifdef TRACE
  TRACE2("T_item(%x:%s)::make_expr -> INTERNAL ERROR", this, get_class_name()) ;
  T_lexem *lex = get_ref_lexem() ;
  TRACE3("provenance : %s:%d:%d",
		 lex->get_file_name()->get_value(),
		 lex->get_file_line(),
		 lex->get_file_column()) ;
#endif //TRACE

  internal_error(get_ref_lexem(),
				 __FILE__,
				 __LINE__,
				 "T_item::make_expr() : %x:%s is not an expression",
				 this,
				 get_class_name()) ;
  return NULL ; // pour Visual-C++
}

// Fonction qui transfere vole les commentaires de ref_item pour
// les placer dans this
void T_item::fetch_comments(T_item *ref_item)
{
  TRACE4("T_item(%x)::fetch_comments(%x : <%x, %x>",
		 this,
		 ref_item,
		 ref_item->first_comment,
		 ref_item->last_comment) ;

  T_item *cur = ref_item->get_comments() ;

  while (cur != NULL)
	{
	  T_item *next = (T_item *)cur->get_next() ;
	  cur->set_father(this) ;
	  cur->tail_insert((T_item **)&first_comment, (T_item **)&last_comment) ;
	  cur = next ;
	}

  ref_item->first_comment = NULL ;
  ref_item->last_comment = NULL ;
}

/*
  La methode set_B_type_rec est utilisee dans le traitement du typage.
  Comme les sous-expressions dans les classes T_array_item et
  T_binary_op sont de la classe T_item, il est necessaire de definir
  la methode sur cette classe.  L'implementation par defaut ne fait
  rien.
*/
void T_item::set_B_type_rec(T_type*)
{
}

//
//
//	}{	Fin du fichier
//
