/***************************** CLEARSY **************************************
* Fichier : $Id: c_namtyp.cpp,v 2.0 2005-02-11 10:11:10 cc Exp $
* (C) 2008 CLEARSY
*
* Compilations :	-DDEBUG_TYPE_NAME pour debugger make_type_name
*					-DDEBUG_PROD_TYPE_NAME pour visulaliser les "limites" des
*					T_product_type quand on fait make_type_name
*					-DDEBUG_BOUNDS pour afficher les bornes dans le
*					debugger HTML et les messages d'erreur
*					-DDEBUG_TYPE pour les traces generales
*					-DDEBUG_HTML_TYPE pour voir dans le debugger HTML les src/dst
*					des relations
*
* Description :	Nommage des types B
*				Provient de c_type.cpp 1.11
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
RCS_ID("$Id: c_namtyp.cpp,v 1.24 2005-02-11 10:11:10 cc Exp $") ;

/* Includes systeme */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

/* Includes Composant Local */
#include "c_api.h"

// Taille memoire d'un pointeur ecrit en string
static const int PTR_SIZE = 24 ;

// Pas de reallocation
// La valeur 85 est obtenue a partir de considerations statistiques
static const size_t alloc_step_si = 85 ;

static void make_space(char *&value,
								size_t needed,
								size_t &used,
								size_t &allocated);

//
// Fabrication des noms
//
// Fonction de haut niveau
T_symbol *T_type::make_type_name(int bounds)
{
#ifdef DEBUG_TYPE_NAME
  TRACE2("T_type(%x)::internal_make_type_name(bounded %s)",
		 this,
		 (bounds == TRUE) ? "TRUE" : "FALSE") ;
  TRACE1("node_type = %d", get_node_type()) ;
  TRACE1("class_name = %s", get_class_name()) ;
#endif // DEBUG_TYPE_NAME
  assert(is_a_type() == TRUE) ;

  // Il faut calculer le nom !
  char *value = NULL ;
  size_t value_len = 0 ;
  size_t allocated = 0 ;
  size_t used = 0 ;
  // On alloue la place pour une chaîne vide.
  make_space(value, 1, used, allocated);
  //internal_make_type_name(value, allocated, used, bounds) ;
  internal_make_readable_type_name(value, allocated, used, bounds) ;
  value_len = strlen(value) ;

#ifdef DEBUG_TYPE_NAME
  TRACE1("make_type_name -> %s", value) ;
#endif // DEBUG_TYPE_NAME
  type_name = lookup_symbol(value, value_len) ;

#ifdef STATS_ALLOC
  add_type_name(value) ;
#endif

  return type_name ;
}

// Fonction accessoire qui fait de la place
static void make_space(char *&value,
								size_t needed,
								size_t &used,
								size_t &allocated)
{
#ifdef DEBUG_TYPE_NAME
  TRACE4("make_space(%s, needed %d, used %d, allocated %d",
		 value,
		 needed,
		 used,
		 allocated) ;
#endif

  if (used + needed >= allocated)
	{
	  size_t alloc = allocated ;
	  while (used + needed >= alloc)
		{
		  alloc += alloc_step_si ;
		}

#ifdef DEBUG_TYPE_NAME
	  TRACE1("on realloue un bloc de %d", alloc) ;
#endif
	  allocated = alloc ;
	  char *new_value = new char[alloc] ;
#ifdef STATS_ALLOC
	  add_alloc(new_value, alloc, __FILE__, __LINE__) ;
#endif
	  if (value != NULL)
		{
		  strcpy(new_value, value) ;
#ifdef DEBUG_TYPE_NAME
		  TRACE1("avant liberation %x", value) ;
#endif

		  delete [] value ;
#ifdef DEBUG_TYPE_NAME
		  TRACE1("apres liberation %x", value) ;
#endif

		}
	  else
		{
		  *new_value = '\0' ;
		}
	  value = new_value ;
	}

  used += needed ;
}

// Fonction qui fait un strcat/strcpy selon les cas
static void my_strcat(char *x, const char *y)
{
#ifdef DEBUG_TYPE_NAME
  TRACE2("my_strcat(%s, %s)",
		 (x == NULL) ? "" : x,
		 y) ;
#endif

  if (*x == '\0')
	{
	  strcpy(x, y) ;
	}
  else
	{
	  strcat(x,y) ;
	}
}


// Produit cartesien
void T_product_type::internal_make_type_name(char *&value,
													  size_t &used,
													  size_t &allocated,
													  int bounds)
{
#ifdef DEBUG_TYPE_NAME
  TRACE1("T_product_type(%x)::internal_make_type_name", this) ;
#endif // DEBUG_TYPE_NAME

  make_space(value, 3, used, allocated) ; // '[' + '*' + ']'
  my_strcat(value, "[") ;
  if (type1 != NULL)
	{
	  type1->internal_make_type_name(value, used, allocated, bounds) ;
	}
  else
	{
	  make_space(value, 1, used, allocated) ;
	  my_strcat(value, "?") ;
	}
  my_strcat(value, "*") ;
  if (type2 != NULL)
	{
	  type2->internal_make_type_name(value, used, allocated, bounds) ;
	}
  else
	{
	  make_space(value, 1, used, allocated) ;
	  my_strcat(value, "?") ;
	}
  my_strcat(value, "]") ;

#ifdef DEBUG_TYPE_NAME
  TRACE1("T_product_type::internal_make_type_name -> %s", value) ;
#endif // DEBUG_TYPE_NAME
}

// Produit cartesien
void T_product_type::internal_make_readable_type_name(char *&value,
                                                      size_t &used,
                                                      size_t &allocated,
                                                      int bounds)
{

  make_space(value, 3, used, allocated) ; // '(' + '*' + ')'
  my_strcat(value, "(") ;
  if (type1 != NULL)
    {
      type1->internal_make_readable_type_name(value, used, allocated, bounds) ;
    }
  else
    {
      make_space(value, 1, used, allocated) ;
      my_strcat(value, "?") ;
    }
  my_strcat(value, "*") ;
  if (type2 != NULL)
    {
      type2->internal_make_readable_type_name(value, used, allocated, bounds) ;
    }
  else
    {
      make_space(value, 1, used, allocated) ;
      my_strcat(value, "?") ;
    }
  my_strcat(value, ")") ;
}

// Ensemble
void T_setof_type::internal_make_type_name(char *&value,
													size_t &used,
													size_t &allocated,
													int bounds)
{
  make_space(value, 6, used, allocated) ;
  my_strcat(value, "SetOf(") ;
#ifndef NO_CHECKS
  if (base_type != NULL)
	{
	  base_type->internal_make_type_name(value, used, allocated, bounds) ;
	}
  else
	{
	  make_space(value, 10, used,allocated) ;
	  my_strcat(value, "PANIC") ;
	  TRACE1("PANIC T_setof_type(%x) base_type= NULL", this) ;
	}
#else // NO_CHECKS
	  base_type->internal_make_type_name(value, used, allocated, bounds) ;
#endif // !NO_CHECKS
  make_space(value, 1, used, allocated) ;
  my_strcat(value, ")") ;
#ifdef DEBUG_TYPE_NAME
  TRACE1("T_Setof_type::internal_make_type_name -> %s", value) ;
#endif
}

// Ensemble
void T_setof_type::internal_make_readable_type_name(char *&value,
                                                    size_t &used,
                                                    size_t &allocated,
                                                    int bounds)
{
  make_space(value, 4, used, allocated) ;
  my_strcat(value, "POW(") ;
#ifndef NO_CHECKS
  if (base_type != NULL)
    {
      base_type->internal_make_readable_type_name(value, used, allocated, bounds) ;
    }
  else
    {
      make_space(value, 10, used,allocated) ;
      my_strcat(value, "PANIC") ;
      TRACE1("PANIC T_setof_type(%x) base_type= NULL", this) ;
    }
#else // NO_CHECKS
      base_type->internal_make_readable_type_name(value, used, allocated, bounds) ;
#endif // !NO_CHECKS
  make_space(value, 1, used, allocated) ;
  my_strcat(value, ")") ;
#ifdef DEBUG_TYPE_NAME
  TRACE1("T_Setof_type::internal_make_readable_type_name -> %s", value) ;
#endif
}

// Ensemble abstrait
void T_abstract_type::internal_make_type_name(char *&value,
													   size_t &used,
													   size_t &allocated,
													   int bounds)
{
#ifdef DEBUG_TYPE_NAME
  TRACE1("T_abstract_type(%x)::internal_make_type_name", this) ;
#endif
  int extra = 0 ;
  if (get_after_valuation_type() != NULL)
	{
	  extra = 1 ;
	}

#ifdef DEBUG_HTML_TYPE
  if (get_set_type() == REL_SRC_SET)
	{
	  make_space(value, 4, used, allocated) ;
	  my_strcat(value, "<U>") ;
	}
#endif
  // A FAIRE
  make_space(value,
			 11 + 2*PTR_SIZE + extra + set->get_name()->get_len(),
			 used,
			 allocated) ;
  //                123456 + 1 pour ')' + 4 pour ','
  if (extra == 1)
	{
	  my_strcat(value, "[") ;
	}

  my_strcat(value, "atype(") ;
  my_strcat(value, set->get_name()->get_value()) ;

  if (bounds == TRUE)
	{
	  char buf[PTR_SIZE] ;
          sprintf(buf, ", 0x%p", get_lower_bound()) ;
	  my_strcat(value, buf) ;
          sprintf(buf, ", 0x%p", get_upper_bound()) ;
	  my_strcat(value, buf) ;
	}

  my_strcat(value, ")") ;

  if (get_after_valuation_type() != NULL)
	{
#ifdef DEBUG_TYPE_NAME
	  TRACE3("ici %x, get_after_valuation_type %x %s",
			 this,
			 get_after_valuation_type(),
			 (get_after_valuation_type() == NULL)
			 	? "(null)" : get_after_valuation_type()->get_class_name()) ;
#endif


#ifndef NO_CHECKS
	  if (get_after_valuation_type() == this)
		{
#ifdef DEBUG_TYPE
		  TRACE2("Panic this %x == after_valuation_type %x",
				 this,
				 get_after_valuation_type()) ;
#endif

		  internal_error(get_ref_lexem(),
						 __FILE__,
						 __LINE__,
						 "Panic this %x == after_valuation_type %x",
						 get_after_valuation_type()->make_type_name()) ;
		  make_space(value, 6, used, allocated) ;
		  //                12345
		  my_strcat(value, "PANIC") ;
		  return ;
		}
#endif
	  make_space(value, 4, used, allocated) ;
	  my_strcat(value, "-->") ;
	  get_after_valuation_type()->internal_make_type_name(value,
														  used,
														  allocated,
														  bounds) ;
	  my_strcat(value, "]") ;
	}

#ifdef DEBUG_HTML_TYPE
  if (get_set_type() == REL_SRC_SET)
	{
	  make_space(value, 4, used, allocated) ;
	  my_strcat(value, "</U>") ;
	}
#endif
#ifdef DEBUG_TYPE_NAME
  TRACE1("T_abstract_type::internal_make_type_name -> %s", value) ;
#endif
}

// Ensemble abstrait
void T_abstract_type::internal_make_readable_type_name(char *&value,
                                                       size_t &used,
                                                       size_t &allocated,
                                                       int bounds)
{
#ifdef DEBUG_TYPE_NAME
  TRACE1("T_abstract_type(%x)::internal_make_readable_type_name", this) ;
#endif
  int extra = 0 ;
  if (get_after_valuation_type() != NULL)
    {
      extra = 1 ;
    }

#ifdef DEBUG_HTML_TYPE
  if (get_set_type() == REL_SRC_SET)
    {
      make_space(value, 4, used, allocated) ;
      my_strcat(value, "<U>") ;
    }
#endif
  // A FAIRE
  make_space(value,
             4 + 2*PTR_SIZE + extra + set->get_name()->get_len(),
             used,
             allocated) ;
  //                4 pour ','
  if (extra == 1)
    {
      my_strcat(value, "[") ;
    }

  my_strcat(value, set->get_name()->get_value()) ;

  if (bounds == TRUE)
    {
      char buf[PTR_SIZE] ;
          sprintf(buf, ", 0x%p", get_lower_bound()) ;
      my_strcat(value, buf) ;
          sprintf(buf, ", 0x%p", get_upper_bound()) ;
      my_strcat(value, buf) ;
    }

  if (get_after_valuation_type() != NULL)
    {
#ifdef DEBUG_TYPE_NAME
      TRACE3("ici %x, get_after_valuation_type %x %s",
             this,
             get_after_valuation_type(),
             (get_after_valuation_type() == NULL)
                ? "(null)" : get_after_valuation_type()->get_class_name()) ;
#endif


#ifndef NO_CHECKS
      if (get_after_valuation_type() == this)
        {
#ifdef DEBUG_TYPE
          TRACE2("Panic this %x == after_valuation_type %x",
                 this,
                 get_after_valuation_type()) ;
#endif

          internal_error(get_ref_lexem(),
                         __FILE__,
                         __LINE__,
                         "Panic this %x == after_valuation_type %x",
                         get_after_valuation_type()->make_type_name()) ;
          make_space(value, 6, used, allocated) ;
          //                12345
          my_strcat(value, "PANIC") ;
          return ;
        }
#endif
      make_space(value, 4, used, allocated) ;
      my_strcat(value, "-->") ;
      get_after_valuation_type()->internal_make_readable_type_name(value,
                                                          used,
                                                          allocated,
                                                          bounds) ;
      my_strcat(value, "]") ;
    }

#ifdef DEBUG_HTML_TYPE
  if (get_set_type() == REL_SRC_SET)
    {
      make_space(value, 4, used, allocated) ;
      my_strcat(value, "</U>") ;
    }
#endif
#ifdef DEBUG_TYPE_NAME
  TRACE1("T_abstract_type::internal_make_type_name -> %s", value) ;
#endif
}

// Type enumere
void T_enumerated_type::internal_make_type_name(char *&value,
														 size_t &used,
														 size_t &allocated,
														 int bounds)
{
#ifdef DEBUG_TYPE_NAME
  TRACE1("T_enumerated_type(%x)::internal_make_type_name", this) ;
#endif
  // A FAIRE
  // 123456      7 -- 1 et 2 sont des pointeurs
  // etype(xx,1,2)
#ifdef DEBUG_HTML_TYPE
  if (get_set_type() == REL_SRC_SET)
	{
	  make_space(value, 4, used, allocated) ;
	  my_strcat(value, "<U>") ;
	}
#endif

  make_space(value,
			 type_definition->get_name()->get_len() + 11 + 2*PTR_SIZE ,
			 used, allocated) ;
  my_strcat(value, "etype(") ;
  my_strcat(value, type_definition->get_name()->get_value()) ;

  if (bounds == TRUE)
	{
	  char buf[PTR_SIZE] ;
          sprintf(buf, ", 0x%p", get_lower_bound()) ;
	  my_strcat(value, buf) ;
          sprintf(buf, ", 0x%p", get_upper_bound()) ;
	  my_strcat(value, buf) ;
	}

  my_strcat(value, ")") ;
#ifdef DEBUG_HTML_TYPE
  if (get_set_type() == REL_SRC_SET)
	{
	  make_space(value, 4, used, allocated) ;
	  my_strcat(value, "</U>") ;
	}
#endif

#ifdef DEBUG_TYPE_NAME
  TRACE1("T_enumerated_type::internal_make_type_name -> %s", value) ;
#endif
}

// Type enumere
void T_enumerated_type::internal_make_readable_type_name(char *&value,
                                                         size_t &used,
                                                         size_t &allocated,
                                                         int bounds)
{
#ifdef DEBUG_TYPE_NAME
  TRACE1("T_enumerated_type(%x)::internal_make_readable_type_name", this) ;
#endif
  // A FAIRE
  // 123456      7 -- 1 et 2 sont des pointeurs
  // etype(xx,1,2)
#ifdef DEBUG_HTML_TYPE
  if (get_set_type() == REL_SRC_SET)
    {
      make_space(value, 4, used, allocated) ;
      my_strcat(value, "<U>") ;
    }
#endif

  make_space(value,
             type_definition->get_name()->get_len() + 4 + 2*PTR_SIZE ,
             used, allocated) ;
  my_strcat(value, type_definition->get_name()->get_value()) ;

  if (bounds == TRUE)
    {
      char buf[PTR_SIZE] ;
          sprintf(buf, ", 0x%p", get_lower_bound()) ;
      my_strcat(value, buf) ;
          sprintf(buf, ", 0x%p", get_upper_bound()) ;
      my_strcat(value, buf) ;
    }

#ifdef DEBUG_HTML_TYPE
  if (get_set_type() == REL_SRC_SET)
    {
      make_space(value, 4, used, allocated) ;
      my_strcat(value, "</U>") ;
    }
#endif

#ifdef DEBUG_TYPE_NAME
  TRACE1("T_enumerated_type::internal_make_readable_type_name -> %s", value) ;
#endif
}

// Type predefini
static const char *const predefined_type_name_sct[] =
{
  "INTEGER",
  "BOOL",
  "STRING",
  "REAL",
  "FLOAT"
} ;

void T_predefined_type::internal_make_type_name(char *&value,
														 size_t &used,
														 size_t &allocated,
														 int bounds)
{
#ifdef DEBUG_TYPE_NAME
  TRACE1("T_predefined_type(%x)::internal_make_type_name", this) ;
#endif // DEBUG_TYPE_NAME
  size_t size = strlen(predefined_type_name_sct[type]) ;
  // 123456        7
  // btype(xx,yy,zz)
#ifdef DEBUG_HTML_TYPE
  if (get_set_type() == REL_SRC_SET)
	{
	  make_space(value, 4, used, allocated) ;
	  my_strcat(value, "<U>") ;
	}
#endif
  make_space(value, size + 11 + 2*PTR_SIZE, used, allocated) ;
  my_strcat(value, "btype(") ;
  my_strcat(value, predefined_type_name_sct[type]) ;
  if (ref_interval != NULL)
	{
	  make_space(value, ref_interval->get_name()->get_len() +2, used, allocated) ;
	  my_strcat(value, ":") ;
	  my_strcat(value, ref_interval->get_name()->get_value()) ;
	}

  if (bounds == TRUE)
	{
	  char buf[PTR_SIZE] ;
          sprintf(buf, ", 0x%p", get_lower_bound()) ;
	  my_strcat(value, buf) ;
          sprintf(buf, ", 0x%p", get_upper_bound()) ;
	  my_strcat(value, buf) ;
	}

  my_strcat(value, ")") ;
#ifdef DEBUG_HTML_TYPE
  if (get_set_type() == REL_SRC_SET)
	{
	  make_space(value, 4, used, allocated) ;
	  my_strcat(value, "</U>") ;
	}
#endif
#ifdef DEBUG_TYPE_NAME
  TRACE1("T_predefined_type::internal_make_type_name -> %s", value) ;
#endif
}

void T_predefined_type::internal_make_readable_type_name(char *&value,
                                                         size_t &used,
                                                         size_t &allocated,
                                                         int bounds)
{
#ifdef DEBUG_TYPE_NAME
  TRACE1("T_predefined_type(%x)::internal_make_readable_type_name", this) ;
#endif // DEBUG_TYPE_NAME
  size_t size = strlen(predefined_type_name_sct[type]) ;
  // 123456        7
  // btype(xx,yy,zz)
#ifdef DEBUG_HTML_TYPE
  if (get_set_type() == REL_SRC_SET)
    {
      make_space(value, 4, used, allocated) ;
      my_strcat(value, "<U>") ;
    }
#endif
  make_space(value, size + 4 + 2*PTR_SIZE, used, allocated) ;
  my_strcat(value, predefined_type_name_sct[type]) ;

  if (bounds == TRUE)
    {
      char buf[PTR_SIZE] ;
          sprintf(buf, ", 0x%p", get_lower_bound()) ;
      my_strcat(value, buf) ;
          sprintf(buf, ", 0x%p", get_upper_bound()) ;
      my_strcat(value, buf) ;
    }

#ifdef DEBUG_HTML_TYPE
  if (get_set_type() == REL_SRC_SET)
    {
      make_space(value, 4, used, allocated) ;
      my_strcat(value, "</U>") ;
    }
#endif
#ifdef DEBUG_TYPE_NAME
  TRACE1("T_predefined_type::internal_make_readable_type_name -> %s", value) ;
#endif
}

// Type generique
void T_generic_type::internal_make_type_name(char *&value,
													  size_t &used,
													  size_t &allocated,
													  int bounds)
{
#ifdef DEBUG_TYPE_NAME
  TRACE1("T_generic_type(%x)::internal_make_type_name", this) ;
#endif

  if (type == NULL)
	{
	  make_space(value, 2, used, allocated) ;
	  my_strcat(value, "?") ;
	}
  else // Joker instancie
	{
	  make_space(value, 4, used, allocated) ;
	  my_strcat(value, "?->") ;
	  type->internal_make_type_name(value, used, allocated, bounds) ;
	}

#ifdef DEBUG_TYPE_NAME
  TRACE1("T_generic_type::internal_make_type_name -> %s", value) ;
#endif
}

// Type generique
void T_generic_type::internal_make_readable_type_name(char *&value,
                                                      size_t &used,
                                                      size_t &allocated,
                                                      int bounds)
{
#ifdef DEBUG_TYPE_NAME
  TRACE1("T_generic_type(%x)::internal_make_readable_type_name", this) ;
#endif

  if (type == NULL)
    {
      make_space(value, 2, used, allocated) ;
      my_strcat(value, "?") ;
    }
  else // Joker instancie
    {
      make_space(value, 4, used, allocated) ;
      my_strcat(value, "?->") ;
      type->internal_make_readable_type_name(value, used, allocated, bounds) ;
    }

#ifdef DEBUG_TYPE_NAME
  TRACE1("T_generic_type::internal_make_type_name -> %s", value) ;
#endif
}

//
//	}{ Classe T_label_type
//
void T_label_type::internal_make_type_name(char *&value,
													size_t &used,
													size_t &allocated,
													int bounds)
{
#ifdef DEBUG_TYPE_NAME
  TRACE1("T_label_type(%x)::internal_make_type_name", this) ;
#endif
  if (name != NULL)
	{
	  // 'Ident'':'<type>
	  make_space(value, 1 + name->get_name()->get_len(), used, allocated) ;
	  my_strcat(value, name->get_name()->get_value()) ;
	  my_strcat(value, ":") ;
	}
  else
	{
	  make_space(value, 2, used, allocated) ;
	  my_strcat(value, "?:") ;
	}
  if (type != NULL)
	{
	  type->internal_make_type_name(value, used, allocated, bounds) ;
	}
  else
	{
	  make_space(value, 1, used, allocated) ;
	  my_strcat(value, "?") ;
	}
}

//
//	}{ Classe T_label_type
//
void T_label_type::internal_make_readable_type_name(char *&value,
                                                    size_t &used,
                                                    size_t &allocated,
                                                    int bounds)
{
#ifdef DEBUG_TYPE_NAME
  TRACE1("T_label_type(%x)::internal_make_readable_type_name", this) ;
#endif
  if (name != NULL)
    {
      // 'Ident'':'<type>
      make_space(value, 1 + name->get_name()->get_len(), used, allocated) ;
      my_strcat(value, name->get_name()->get_value()) ;
      my_strcat(value, ":") ;
    }
  else
    {
      make_space(value, 2, used, allocated) ;
      my_strcat(value, "?:") ;
    }
  if (type != NULL)
    {
      type->internal_make_readable_type_name(value, used, allocated, bounds) ;
    }
  else
    {
      make_space(value, 1, used, allocated) ;
      my_strcat(value, "?") ;
    }
}



//
//	}{ Classe T_record_type
//
void T_record_type::internal_make_type_name(char *&value,
													 size_t &used,
													 size_t &allocated,
													 int bounds)
{
#ifdef DEBUG_TYPE_NAME
  TRACE1("T_record_type(%x)::internal_make_type_name", this) ;
#endif
  // Struct()
  // 12345678
  make_space(value, 8, used, allocated) ;
  my_strcat(value, "Struct(") ;
  T_label_type *cur = first_label ;
  while (cur != NULL)
	{
	  cur->internal_make_type_name(value, used, allocated, bounds) ;
	  cur = (T_label_type *)cur->get_next() ;
	  if (cur != NULL)
		{
		  make_space(value, 2, used, allocated) ;
		  my_strcat(value, ", ") ;
		}
	}
  my_strcat(value, ")") ; // La place a deja ete reservee au debut
}

//
//	}{ Classe T_record_type
//
void T_record_type::internal_make_readable_type_name(char *&value,
                                                     size_t &used,
                                                     size_t &allocated,
                                                     int bounds)
{
#ifdef DEBUG_TYPE_NAME
  TRACE1("T_record_type(%x)::internal_make_type_name", this) ;
#endif
  // Struct()
  // 12345678
  make_space(value, 8, used, allocated) ;
  my_strcat(value, "Struct(") ;
  T_label_type *cur = first_label ;
  while (cur != NULL)
    {
      cur->internal_make_readable_type_name(value, used, allocated, bounds) ;
      cur = (T_label_type *)cur->get_next() ;
      if (cur != NULL)
        {
          make_space(value, 2, used, allocated) ;
          my_strcat(value, ", ") ;
        }
    }
  my_strcat(value, ")") ; // La place a deja ete reservee au debut
}

//
//	}{ Fin du fichier
//
