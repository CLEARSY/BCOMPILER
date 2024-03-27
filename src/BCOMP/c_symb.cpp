/***************************** CLEARSY **************************************
* Fichier : $Id: c_symb.cpp,v 2.0 2001-07-19 16:24:12 lv Exp $
* (C) 2008 CLEARSY
*
* Description :		Table des symboles
*
* Compilations : 	-DDEBUG_SYMB pour avoir des traces
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
RCS_ID("$Id: c_symb.cpp,v 1.24 2001-07-19 16:24:12 lv Exp $") ;

/* Includes systeme */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

/* Includes Composant Local */
#include "c_api.h"

/* Variables statiques locales */
const int SYMBOL_HASH_SIZE = 211 ;
static T_symbol *symbol_table_sop[SYMBOL_HASH_SIZE] ;
static int init_symbtab_done_si = FALSE ;


ostream &operator<<(ostream &os, const T_symbol &sub)
{
    os << sub.value;
	return os;
}

istream &operator>>(istream &is, T_symbol &sub)
{
    char buffer[2000];
    is >> buffer;
    char * token;
    token = strtok (buffer,":\n");
    sscanf (token,"%zu",&sub.len);
    token = strtok (NULL, "\n");
    sub.value = new char[sub.len+1];
    sscanf (token,"%s",sub.value);

	return is;
}



//
//	}{	Fonction d'initialisation
//
void init_symbol_table(void)
{
  TRACE0(">> init_symbol_table") ;
  ENTER_TRACE ; ENTER_TRACE ;

  if (init_symbtab_done_si == FALSE)
	{
	  TRACE1("-- initialisation des %d lignes", SYMBOL_HASH_SIZE) ;
	  int i ;
	  for (i = 0 ; i < SYMBOL_HASH_SIZE ; i++)
		{
		  symbol_table_sop[i] = NULL ;
		}

	  init_symbtab_done_si = TRUE ;

	  // On met les builtins dans la table des sym
	  add_keywords() ;
	}

  // On ne veut pas que la table des symboles soit liberee. Mais
  // attention, il ne faut pas marquer le bloc car c'est un tableau
  // statique, donc non alloue par new, donc ne possede pas de
  // T_memory_info en entete
  // Il ne faut donc surtout pas ecrire :
  // set_block_state(symbol_table_sop, MINFO_PERSISTENT) ;

  EXIT_TRACE ; EXIT_TRACE ;
  TRACE1("<< init_symbol_table symbol_table = %x", symbol_table_sop) ;
}

//
//	}{ Fonction de repararation de la persistance (pour les symboles
//	charges depuis le disque)
//
void fix_symbol_table(void)
{
  TRACE0(">> fix_symbol_table") ;
  ENTER_TRACE ; ENTER_TRACE ;

  TRACE1("-- fixialisation des %d lignes", SYMBOL_HASH_SIZE) ;
  int i ;
  for (i = 0 ; i < SYMBOL_HASH_SIZE ; i++)
	{
	  T_symbol *cur_symbol = symbol_table_sop[i] ;

	  while (cur_symbol != NULL)
		{
		  cur_symbol->set_persistent();
		  cur_symbol = cur_symbol->get_next_symb() ;
		}
	}

  EXIT_TRACE ; EXIT_TRACE ;
  TRACE1("<< fix_symbol_table symbol_table = %x", symbol_table_sop) ;
}


void unlink_symbol_table(void)
{
  TRACE0("unlink_symbol_table") ;
}


/* Fonction de hachage */
/* Extrait du "Dragon Book" des compilateurs */
static int HPJW(const char *name)
{
  ASSERT(name != NULL) ;
  const char *p ;
  size_t h = 0 ;
  size_t g ;

  for (p = name ; *p != '\0' ; p++ )
	{
	  h = (h << 4) + *p ;
	  if ( (g = h & 0xf0000000) != 0)
		{
		  h = h ^ (g >> 24) ;
		  h = h ^ g ;
		}
	}

#ifdef DEBUG_SYMB
  TRACE2("HPJW(%s)->%d", name, h % SYMBOL_HASH_SIZE ) ;
#endif

  return h % SYMBOL_HASH_SIZE ;
}

//
// Recherche d'une chaine avec creation ssi new_create = TRUE
//
static T_symbol *internal_lookup_symbol(const char *new_value,
												 size_t new_len,
												 int is_a_keyword,
												 T_lex_type new_lex_type,
												 int new_create)
{
#ifdef DEBUG_SYMB
  TRACE4("internal_lookup_symbol(%s <%d>) is_a_keyword=%s symbol_table %x",
		 new_value,
		 new_len,
		 (is_a_keyword == TRUE) ? "TRUE" : "FALSE",
		 symbol_table_sop) ;
#endif
  int h = HPJW(new_value) ;

  T_symbol *cur_symbol = symbol_table_sop[h] ;
  T_symbol *prev_symbol = NULL ;

  // Recherche rapide : HCODE + ligne triee par taille croissante
  for (;;)
	{
#ifdef DEBUG_SYMB
  /*	  TRACE4("SYMB_SEARCH ref %x:%s::%d, cur %x",
			 new_value,
			 new_value,
			 new_len,
			 cur_symbol) ;*/
	  TRACE7("SYMB_SEARCH ref %x:%s::%d, cur %x value %x:<%s> len %d",
			 new_value,
			 new_value,
			 new_len,
			 cur_symbol,
			 (cur_symbol == NULL) ? 0 : cur_symbol->get_value(),
			 (cur_symbol == NULL) ? "NULL" : cur_symbol->get_value(),
			 (cur_symbol == NULL) ? 0 : cur_symbol->get_len()) ;
#endif
	  if ( (cur_symbol == NULL) || (cur_symbol->get_len() > new_len) )
		{
		  if (new_create == TRUE)
			{
			  // Il faut creer la chaine !
			  if (is_a_keyword == FALSE)
				{
#ifdef DEBUG_SYMB
				  TRACE1("on cree la chaine \"%s\"", new_value) ;
#endif
				  T_symbol *res = new T_symbol(new_value,
											   new_len,
											   h,
											   cur_symbol,
											   prev_symbol);
				  return res ;
				}
			  else
				{
#ifdef DEBUG_SYMB
				  TRACE3("on cree le MOT-CLE \"%s\" entre %s et %s",
						 new_value,
						 (prev_symbol == NULL) ? "NULL" : prev_symbol->get_value(),
						 (cur_symbol == NULL) ? "NULL" : cur_symbol->get_value()) ;
#endif
				  T_keyword *res = new T_keyword(new_value,
												 new_len,
												 h,
												 cur_symbol,
												 prev_symbol,
												 new_lex_type) ;
				  return res ;
				}
			}
		  else // new_create == FALSE : on ne cree pas la chaine
			{
#ifdef DEBUG_SYMB
			  TRACE1("ici :: new_create == FALSE, on ne cree pas %s", new_value) ;
#endif
			  return NULL ;
			}
 		}

	  if ( 	(cur_symbol->get_len() == new_len)
			&& (strcmp(cur_symbol->get_value(), new_value) == 0) )
		{
		  ASSERT(is_a_keyword == FALSE) ;
		  // Trouve !
#ifdef DEBUG_SYMB
		  TRACE2("on reutilise la chaine %s (%x)",
				 cur_symbol->get_value(),
				 cur_symbol) ;
#endif
		  return cur_symbol ;
		}

	  prev_symbol = cur_symbol ;
	  cur_symbol = cur_symbol->get_next_symb() ;
	}
}

//
//	}{	Recherche d'une chaine avec eventuellement creation
T_symbol *lookup_symbol(const char *new_value,
								 size_t new_len)
{
  ASSERT(new_value != NULL) ;
  return internal_lookup_symbol(new_value, new_len, FALSE, L_IDENT, TRUE) ;
}

//
//	}{	Ajout d'un mot cle
//
T_keyword *insert_builtin(T_lex_type new_lex_type,
								   const char *new_value)
{
  // Le cast est licite par construction
  return (T_keyword *)internal_lookup_symbol(new_value,
											 strlen(new_value),
											 TRUE,
											 new_lex_type,
											 TRUE) ;
}

//
//	}{	Fonction qui donne renvoie TRUE ssi une chaine est un mot-cle, FALSE
//		sinon. Si le resultat est true, lex_type est rempli avec le lex_type
//		du mot-cle
int lookup_builtin(const char *string,
							int string_len ,
							T_lex_type &lex_type)
{
#ifdef DEBUG_SYMB
  TRACE2("lookup_builtin(%s, %d)", string, string_len) ;
#endif
  T_symbol *res = internal_lookup_symbol(string, string_len, FALSE, L_IDENT, FALSE) ;

  if ( (res == NULL) || (res->get_node_type() != NODE_KEYWORD) )
	{
	  // Ce n'est pas un mot cle
#ifdef DEBUG_SYMB
	  TRACE1("%s n'est pas un mot cle", string) ;
#endif
	  return FALSE ;
	}
  else
	{
	  // C'est un mot cle
#ifdef DEBUG_SYMB
	  TRACE1("%s est un mot cle", string) ;
#endif
	  lex_type = ((T_keyword *)res)->get_lex_type() ;
	  return TRUE ;
	}
}


// Constructeur
T_symbol::T_symbol(const char *new_value,
							size_t new_len,
							int h,
							T_symbol *next_symbol,
							T_symbol *prev_symbol)
  : T_object(NODE_SYMBOL)
{
#ifdef DEBUG_SYMB
  TRACE3("T_symbol::T_symbol(%s) %x <----> %x", new_value, prev_symbol, next_symbol) ;
#endif
  value = new char[new_len + 1] ;
#ifdef STATS_ALLOC
  add_alloc(value, new_len + 1, __FILE__, __LINE__) ;
#endif
  strncpy(value, new_value, new_len) ;
  *(char *)((size_t)value + (new_len)) = '\0' ;
  len = new_len ;
  next_symb = next_symbol ;

  if (prev_symbol == NULL)
	{
	  symbol_table_sop[h] = this ;
	}
  else
	{
	  prev_symbol->next_symb = this ;
	}

  set_persistent() ;

}

// Passage en mode persistant
void T_symbol::set_persistent(void)
{
}

T_symbol::~T_symbol()
{
  TRACE2("T_symbol::~T_symbol(%x) \"%s\"", this, value) ;
}

T_symbol *lookup_symbol(const char *new_value)
{
  ASSERT(new_value != NULL) ;
  return lookup_symbol(new_value, strlen(new_value)) ;
}

#ifdef DEBUG_SYMB
//
//	}{	Dump complet de la table des symboles
//
void dump_symtab(void)
{
  TRACE0("dump_symtab DEBUT") ;
  ENTER_TRACE ;

  int i ;

  for (i = 0 ; i < SYMBOL_HASH_SIZE ; i++)
	{
	  T_symbol *cur = symbol_table_sop[i] ;

	  if (cur != NULL)
		{
		  TRACE1("-- Ligne %03d", i) ;
		  ENTER_TRACE ; ENTER_TRACE ;
		  while (cur != NULL)
			{
			  TRACE2("%s::%d", cur->get_value(), cur->get_len()) ;
			  cur = (T_symbol *)cur->get_next_symb() ;
			}
		  EXIT_TRACE ; EXIT_TRACE ;
		}
	}

  EXIT_TRACE ;
  TRACE0("dump_symtab FIN") ;
}
#endif

//
// Methodes auxilaires de gestion d'une liste de noms sans doublons
//
// Recherche un element dans une liste avec ajout si absent
T_symbol *list_lookup(T_symbol *new_symbol,
							   T_list_link **adr_first,
							   T_list_link **adr_last)
{
  TRACE4("list_lookup(%x:%s, %x, %x)",
		 new_symbol,
		 new_symbol->get_value(),
		 adr_first,
		 adr_last) ;

  T_list_link *cur = *adr_first ;

  while (cur != NULL)
	{
	  // Cast valide par construction
	  T_symbol *cur_symbol = (T_symbol *)cur->get_object() ;
	  ASSERT(cur_symbol->is_a_symbol() == TRUE) ;

	  if (cur_symbol == new_symbol)
		{
		  TRACE0("trouve dans la liste") ;
		  return cur_symbol ;
		}

	  cur = (T_list_link *)cur->get_next() ;
	}

  TRACE0("pas trouve : on cree une entree") ;
  (void) new T_list_link(new_symbol,
						 LINK_OTHER,
						 (T_item **)adr_first,
						 (T_item **)adr_last) ;

  return new_symbol ;
}

// C-Wrapper pour list_lookup
EXTERN char *C_list_lookup(char *new_char,
									void **adr_first,
									void **adr_last)
{
  TRACE3("C_list_lookup(%s, %x, %x)", new_char, adr_first, adr_last) ;

#ifndef NO_CHECKS
  // Verification des parametres
  assert(adr_first != NULL) ;
  assert(adr_last != NULL) ;

  if (*adr_first == NULL)
	{
	  assert(*adr_last == NULL) ;
	}
  else
	{
	  // Cast verifie a posteriori
	  T_list_link *cur = *(T_list_link **)adr_first ;

	  while (cur != NULL)
		{
		  assert(object_test(cur) == TRUE) ;
		  assert(cur->get_node_type() == NODE_LIST_LINK) ;
		  cur = (T_list_link *)cur->get_next() ;
		}

	}
#endif

  // On peut appeller la fonction C++
  T_symbol *new_symbol = lookup_symbol(new_char) ;

  // Cast valides par construction et verifies ci-dessus
  T_symbol *res = list_lookup(new_symbol,
							  (T_list_link **)adr_first,
							  (T_list_link **)adr_last) ;

  ASSERT(res != NULL) ;
  // On prie tres fort pour que le client ne modifie pas la chaine ...
  return (char *)res->get_value() ;
}

//
// Fin du fichier
//

