/***************************** CLEARSY **************************************
* Fichier : $Id: c_object.cpp,v 2.0 2005-02-11 10:11:55 cc Exp $
* (C) 2008 CLEARSY
*
* Compilations :	* -DDEBUG_LINK pour voir les liens sur les objets
*					* -DSTATS_ALLOC permet d'avoir les stats d'allocation par
*					  classe (il faut alors aussi compiler c_secure.cpp avec
*					  ce flag)
*
* Description :		Classe de base T_object
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
RCS_ID("$Id: c_object.cpp,v 1.39 2005-02-11 10:11:55 cc Exp $") ;

/* Includes systeme */
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <sys/types.h>
#include <sys/stat.h>

/* Includes Composant Local */
#include "c_api.h"

const int OBJECT_MAGIC = 0xFABCCBA ;

static char *type_str(char *, T_type *) ;
static char *bound_str(char *, T_bound *) ;
static char *strcat_1ko(char *, const char *) ;

// Constructeur
T_object::T_object(T_node_type new_node_type)
{
  //  TRACE2("T_object(%x)::T_object(node_type=%d)", this, node_type) ;
  T_metaclass *meta = get_metaclass(new_node_type) ;

  if (meta != NULL)
	{
	  // Mise a NULL de tous les champs, si on connait la taille de la
	  // structure.
	  memset(this, 0, meta->get_size()) ;
	}
  else
	{
          usr1.usr1 = usr2.usr2 = NULL ;
          tmp.tmp = NULL ;
	  tmp2 = 0 ;
	}

  node_type = new_node_type ;
  magic = OBJECT_MAGIC ;

  // Ajout dans le gestionnaire d'objets
  get_object_manager()->add_object(this) ;

#ifdef STATS_ALLOC
  // Prise en compte dans les stats
  add_alloc(this) ;
#endif
}

void T_object::check_field_offset(T_metaclass *meta, const char *field_name, int offset)
{
    T_field *f = meta->get_fields();
    while(f)
    {
        if(strcmp(f->get_field_name(), field_name) == 0)
        {
            int f_offset = f->get_field_offset();
            if(f_offset != offset)
            {
                fprintf(stderr, "Warning: Field %s.%s should be at offset %d instead of computed %d\n",
                        meta->get_class_name(),
                        field_name,
                        offset,
                        f_offset);
                fflush(stderr);
            }
            return;
        }
        f = f->get_next();
    }

    fprintf(stderr, "Warning: Field not found in class %s: %s\n", meta->get_class_name(), field_name);
    fflush(stderr);
}

// Constructeur
T_object::T_object()
{
}

void T_object::check_field_offsets()
{
    T_metaclass *meta = get_metaclass("T_object");

    check_field_offset(meta, "magic", offsetof(T_object, magic));
  check_field_offset(meta, "node_type", offsetof(T_object, node_type));
  check_field_offset(meta, "tag", offsetof(T_object, tag)) ;
  check_field_offset(meta, "rank", offsetof(T_object, rank)) ;
  check_field_offset(meta, "tmp",	offsetof(T_object, tmp)) ;
  check_field_offset(meta, "tmp2",	offsetof(T_object, tmp2)) ;
  check_field_offset(meta, "usr1", offsetof(T_object, usr1.usr1)) ;
  check_field_offset(meta, "usr2", offsetof(T_object, usr2.usr2)) ;
}

// Destructeur
T_object::~T_object(void)
{
#ifdef FULL_TRACE
  TRACE1("T_object(%x)::~T_object", this) ;
#endif
  get_object_manager()->delete_object(this) ;
}

// Pour savoir si un objet est une instance de T_object
int object_test(void *adr, int no_msg)
{
  // SURTOUT PAS METTRE SHOW_WATCH ICI SINON CA BOUCLE !!!
#ifdef TEST_TRACE
  TRACE1("object_test(%x)", adr) ;

#endif

  if (adr == NULL)
	{
	  return TRUE ; // pourquoi pas !
	}

  if ( ((size_t)adr % sizeof(void *)) != 0)
	{
	  if (no_msg == FALSE)
		{
		  TRACE1("l'adresse %x n'est pas alignee sur sizeof(void *)", adr) ;
		  //		  assert(FALSE) ;
		  toplevel_error(CAN_CONTINUE,
						 get_error_msg(E_MISALIGNED_ADDRESS),
						 (size_t)adr) ;
		}
	  return FALSE ;
	}

  if ((size_t)adr < get_object_manager()->get_min_address())
	{
	  // Pas un objet car l'adresse est < a la plus petite adresse
	  // allouee pour un T_object
#ifdef TEST_TRACE
	  TRACE0("pas obj cat < min adr") ;
#endif
	  return FALSE ;
	}

#ifdef FULL_TRACE
  TRACE3("object_test(%x) : magic = %d, OBJECT_MAGIC = %d",
		 adr,
		 *((size_t *)adr),
		 OBJECT_MAGIC) ;
#endif

  return (((T_object *)adr)->get_magic() == OBJECT_MAGIC) ;
}

// Dump d'un objet "memoire"
void dump_object(void *adr, int size)
{
  TRACE2("DEBUT DUMP de %x sur %d octets", adr, size) ; ENTER_TRACE ;

  void *p = adr ;
  void *end = (void *)(((unsigned char*)adr) + size) ;


  while (p < end)
	{
	  TRACE2("(0x%08x) : 0x%08x", p, *(int *)p) ;
	  p = (void *)((size_t)p + sizeof(int)) ;
	}

  EXIT_TRACE ; TRACE2("FIN   DUMP de %x sur %d octets", adr, size) ;
}

// Recuperer un int dans le buffer, et avancer d'un cran
size_t get_integer(void *&base)
{
  int res = ntohl(*(int *)base) ;
  TRACE2("get_integer(%x) -> %d", base, res) ;
  base = (void *)((size_t)base + sizeof(int)) ;
  return res ;
}

/* Generation nom pour un pointeur */
#ifdef FULL_TRACE
const char *_ptr_ref(const char *file, int line, void *adr)
#else
  const char *ptr_ref(void *adr)
#endif
{
#ifdef FULL_TRACE
  TRACE3("ptr_ref(%x) appele depuis %s::%d", adr, file, line) ;
#endif
  char *ptr_res = get_1Ko_string() ;

  if (adr == NULL)
	{
	  sprintf(ptr_res, "NULL") ;
	  /* TRACE0("ptr_ref->NULL") ; */
	}
  else
	{
#ifdef FULL_TRACE
	  TRACE1("ptr_ref(%x)", adr) ;
#endif
	  if ( (size_t)adr >= 0xF0000000)
		{
		  //		betree_warning(dump_id, "ILLEGAL ADDRESS 0x%x\n", (int)adr) ;
                  sprintf(ptr_res, "<kbd> ILLEGAL ADDRESS 0x%p</kbd>\n", adr) ;
		}
	  else
		{
		  if (object_test(adr) == TRUE)
			{
			  sprintf(ptr_res,
                                          "<kbd><A HREF=\"%p.html\">0x%p</A></kbd> <em>(class %s)</em>\n",
                                          adr,
                                          adr,
					  ((T_object *)adr)->get_class_name()) ;
			  T_node_type node_type = ((T_object *)adr)->get_node_type() ;

			  // Informations supplementaires
			  switch(node_type)
				{
 				case NODE_IDENT :
				case NODE_RENAMED_IDENT :
				  {
					T_symbol *name = ((T_ident *)adr)->get_name() ;
					ptr_res = strcat_1ko(ptr_res, sym_ref(name)) ;
					break ;
				  }
				case NODE_MACHINE :
				  {
					T_symbol *name = ((T_machine *)adr)->
					  get_machine_name()->get_name() ;
					ptr_res = strcat_1ko(ptr_res, sym_ref(name)) ;
					break ;
				  }
				case NODE_LITERAL_STRING :
				  {
					T_symbol *name = ((T_literal_string *)adr)->get_value() ;
					ptr_res = strcat_1ko(ptr_res, sym_ref(name)) ;
					break ;
				  }
                                case NODE_LITERAL_REAL :
                                  {
                                        T_symbol *name = ((T_literal_real *)adr)->get_value() ;
                                        ptr_res = strcat_1ko(ptr_res, sym_ref(name)) ;
                                        break ;
                                  }
                                case NODE_LITERAL_BOOLEAN :
				  {
					const char *value ;
					value = (((T_literal_boolean *)adr)->get_value() == TRUE)
					  ? "TRUE" : "FALSE" ;
					ptr_res = strcat_1ko(ptr_res, value) ;
					break ;
				  }
				case NODE_LITERAL_INTEGER :
				  {
					T_literal_integer *litin = (T_literal_integer *)adr ;
					ptr_res = strcat_1ko(ptr_res, ptr_ref(litin->get_value())) ;
					T_symbol *name = litin->get_value()->get_value() ;
					if (litin->get_value()->get_is_positive() == FALSE)
					  {
						ptr_res = strcat_1ko(ptr_res, "<FONT COLOR=\"red\">-") ;
					  }
					ptr_res = strcat_1ko(ptr_res, name->get_value()) ;
					if (litin->get_value()->get_is_positive() == FALSE)
					  {
						ptr_res = strcat_1ko(ptr_res, "</FONT>") ;
					  }
					break ;
				  }
				case NODE_SYMBOL :
				case NODE_KEYWORD :
				  {
					T_symbol *name = (T_symbol *)adr ;
					ptr_res = strcat_1ko(ptr_res, sym_ref(name)) ;
					break ;
				  }
				case NODE_OP_RESULT :
				  {
					T_item *name = ((T_op_result *)adr)->get_op_name() ;
					if (name->is_a_symbol())
					  {
						ptr_res = strcat_1ko(ptr_res, sym_ref((T_symbol *)name)) ;
					  }
					break ;
				  }
				case NODE_LEXEM :
				  {
					T_lexem *lex = (T_lexem *)adr ;
					const char *lex_value =  (lex->get_value() == NULL)
					  ? lex->get_lex_ascii() : lex->get_value() ;
					int lex_len =  (lex->get_value() == NULL)
					  ? lex->get_lex_ascii_len() : lex->get_value_len() ;

					// voir decompte ci-dessous, le 6 vient du %06d
					int tmp_len = 5 + 2*6 + lex_len ;
					char *tmp = new char[tmp_len + 1] ;
					//            1  2  3 4   5
					sprintf(tmp, "L%6dC%6d \"%s\"",
							lex->get_file_line(),
							lex->get_file_column(),
							lex_value) ;

					char *save_ptr_res = ptr_res ;
					// cette chaine ne sera libere que par appel a cleanup
					size_t ptr_res_len = tmp_len + strlen(save_ptr_res)  ;
					ptr_res = new char[ptr_res_len + 1 ] ;
					sprintf(ptr_res, "%s%s", save_ptr_res, tmp) ;
					delete [] tmp ;
					break ;
				  }
				case NODE_BETREE :
				  {
					T_symbol *name = ((T_betree *)adr)->get_betree_name() ;
					ptr_res = strcat_1ko(ptr_res, sym_ref(name)) ;
					break ;
				  }
				case NODE_TYPE :
				case NODE_BASE_TYPE :
				case NODE_PREDEFINED_TYPE :
				case NODE_GENERIC_TYPE :
				case NODE_SETOF_TYPE :
				case NODE_PRODUCT_TYPE :
				case NODE_ABSTRACT_TYPE :
				case NODE_ENUMERATED_TYPE :
				case NODE_LABEL_TYPE :
				case NODE_RECORD_TYPE :
				  {
					ptr_res = type_str(ptr_res, (T_type *)adr) ;
					break ;
				  }
				case NODE_BOUND :
				  {
					ptr_res = bound_str(ptr_res, (T_bound *)adr) ;
					break ;
				  }
				default :
				  {
					// Rien par defaut
				  }
				}
			  chain_add((T_item *)adr) ;
			}
		  else
			{
			  TRACE1("%x pas un objet", (size_t)adr) ;
			  sprintf(ptr_res,
                                          "0x%p <kbd><BLINK>(not an object)</BLINK></kbd>\n", adr) ;

			}
		}
	}

  TRACE1("-> ptr_res=%s", ptr_res) ;
  return ptr_res ;
}


/* Generation nom pour un symbole */
#ifndef FULL_TRACE
const char *sym_ref(T_symbol *adr)
#else
  const char *_sym_ref(const char *file, int line, T_symbol *adr)
#endif
{
#ifdef FULL_TRACE
  TRACE3("sym_ref(%x) appele depuis %s:%d", adr, file, line) ;
#endif
  char *sym_res = get_1Ko_string() ;

  if (adr == NULL)
	{
	  sprintf(sym_res, "NULL") ;
	}
  else
	{
	  if (object_test(adr) == FALSE)
		{
                  sprintf(sym_res, "<BLINK>%p NOT AN OBJECT</BLINK>", adr) ;
		}
	  else if (adr->is_a_symbol() == FALSE)
		{
                  TRACE2("<BLINK>%p IS AN OBJECT BUT NOT A SYMBOL</BLINK> : class %s",
                                 adr,
				 adr->get_class_name()) ;
		  sprintf(sym_res,
                                  "<BLINK>%p IS AN OBJECT BUT NOT A SYMBOL</BLINK> : class %s",
                                  adr,
				  adr->get_class_name()) ;
		  fprintf(stdout,
                                  "sym_ref() : %p IS AN OBJECT BUT NOT A SYMBOL : class %s\n",
                                  adr,
				  adr->get_class_name()) ;
		}
	  else
		{
		  T_symbol *sym = (T_symbol *)adr ;
		  const char * class_name = sym->get_class_name() ;
		  // Longueur de la chaîne ci-dessous + longueur des paramètres.
		  size_t new_len = 79 + 2*8 + sym->get_len() + strlen(class_name) ;
		  if (new_len >= 1024) {
			sym_res = new char[new_len + 1] ;
		  }
		  sprintf(sym_res,
                                  "<A HREF=\"%p.html\"> 0x%p</A>"
				  "<FONT COLOR=\"green\">\"%s\"</FONT>"
				  "<em>(class %s)</em>\n",
                                  adr,
                                  adr,
				  sym->get_value(),
				  class_name) ;
		  chain_add(adr) ;
		}
	}

  return sym_res ;
}

/* Generation nom pour une chaine */
const char *str_ref(const char *adr)
{
  if (adr == NULL)
	{
	  return "NULL" ;
	}
  else
	{
	  return adr ;
	}
}

/* Generation nom pour un booleen */
const char *bool_ref(int new_bool)
{
  if (new_bool == TRUE)
	{
	  return "<em>TRUE</em>" ;
	}
  else
	{
	  return "<em>FALSE</em>" ;
	}
}

/* Generation nom pour un type B */
//
// Le buffer passé en paramètre est alloué pour 1024 octets exactement.
//
static char *type_str(char *buf, T_type *type)
{
  char *res = buf ;
  size_t buf_len = strlen(buf);

  T_symbol *name = type->make_type_name() ;
  if (name != NULL)// null en mode FREE_MEMORY par exemple
	{
	  if (type->get_typing_ident() == NULL)
		{
		  size_t new_len = buf_len + name->get_len() ;
		  if (new_len >= 1024)
			{
			  res = new char[new_len + 1] ;
			  sprintf(res, "%s%s", buf, name->get_value()) ;
			}
		  else
			{
			  strcpy(buf + buf_len, name->get_value()) ;
			}
		}
	  else
		{
		  T_symbol *tname = type->get_typing_ident()->get_name() ;
		  size_t new_len = buf_len + name->get_len() + tname->get_len() + 3;
		  if (new_len >= 1024)
			{
			  res = new char[new_len + 1] ;
			  sprintf(res, "%s%s (%s)",
					  buf, name->get_value(), tname->get_value()) ;
			}
		  else
			{
			  sprintf(buf + buf_len, "%s (%s)",
					  name->get_value(), tname->get_value()) ;
			}
		}
	}

  return res ;
}

/* Generation nom pour une borne d'intervalle */
//
// Le buffer passé en paramètre est alloué pour 1024 octets exactement.
//
static char *bound_str(char *buf, T_bound *bound)
{
  char *res = buf ;
  size_t buf_len = strlen(buf) ;

  char *buf2 = get_1Ko_string() ;
  size_t buf2_len = sprintf(buf2,
							"<FONT COLOR=\"red\">%s%c</FONT>",
							bound->get_ref_ident()->get_name()->get_value(),
							(bound->is_upper()) ? '+' : '-') ;
  const char *buf3 ;
  size_t buf3_len ;
  if (bound->get_value() != NULL)
	{
	  buf3 = ptr_ref(bound->get_value()) ;
      buf3_len = strlen(buf3) ;
	}
  else
	{
	  buf3 = "" ;
	  buf3_len = 0 ;
	}

  size_t new_len = buf_len + buf2_len + buf3_len ;
  if (new_len >= 1024)
	{
	  res = new char[new_len + 1] ;
	  sprintf(res, "%s%s%s", buf, buf2, buf3) ;
	}
  else
	{
	  sprintf(buf + buf_len, "%s%s", buf2, buf3) ;
	}
  return res ;
}

// Concaténation de deux chaînes de caractères.
//
//  Le premier paramètre est le buffer de 1 ko dans lequel on veut
//  écrire, le second est la longueur de la chaîne qu'il contient
//  déjà.  Le troisième paramètre est la chaîne qu'on veut lui
//  ajouter.
static char *strcat_1ko(char *buf, const char *str)
{
  char *res = buf ;
  size_t buf_len = strlen(buf) ;
  size_t str_len = strlen(str) ;
  size_t new_len = buf_len + str_len ;
  if (new_len >= 1024)
	{
	  res = new char[new_len + 1] ;
	  sprintf(res, "%s%s", buf, str) ;
	}
  else
	{
	  strcpy(buf + buf_len, str) ;
	}
  return res ;
}

// Liberation d'une adresse (si elle est nulle, on ne fait rien)
// Si l'objet est un item, on decremente le compteur de liens. La liberation
// n'a lieu que si le compteur est nul
// J'avais essaye une fonction surchargee mais ce ne marche pas bien
// dynamiquement donc j'ai fait une fonction "a la C"
void object_unlink(T_object *object)
{
	/*
  TRACE3("object_unlink(%x object_test=%s node_type %d)",
		 object,
		 (object_test(object) == TRUE) ? "TRUE" : "FALSE",
		 ((object != NULL) && (object_test(object) == TRUE))
		 	? ((T_object *)object)->get_node_type() : -1) ;
	*/

  if (object == NULL)
	{
	  return ;
	}

  if (object_test(object) == TRUE)
	{
	  if (object->is_an_item() == TRUE)
		{
			//		  TRACE0("appel de unlink") ;
		  ((T_item *)object)->unlink() ;
		  return ;
		}
	  else if (object->is_a_symbol() == TRUE)
		{
		  // TRACE0("on ne libère pas un symbole") ;
		  return ;
		}
	  else
		{
		  //	    TRACE0("liberation d'object") ;
		  delete object ;
		  return ;
		}
	}
  else
	{
	  // Erreur : on ne sait pas liberer cet objet !
	  TRACE1("PANIC : %x n'est pas un objet", object) ;
	}
}

// Liberation d'une liste d'items commenceant a first_item
void list_unlink(T_item *first_item)
{
#ifdef DEBUG_LINKS
  TRACE1("debut list_unlink(%x)", first_item) ;
#endif
  while (first_item != NULL)
	{
#ifdef DEBUG_LINKS
	  TRACE1(">> item_unlink(%x)", first_item) ;
	  ENTER_TRACE ;
#endif
	  T_item *next = first_item->get_next() ;
	  object_unlink(first_item) ;
#ifdef DEBUG_LINKS
	  EXIT_TRACE ;
	  TRACE1("<< item_unlink(%x)", first_item) ;
#endif
	  first_item = next ;
	}

#ifdef DEBUG_LINKS
  TRACE0("fin list_unlink()") ;
#endif
}

int T_object::is_a_type(void)
{
  return FALSE ;
}

// Pour savoir si un objet est un symbole
int T_object::is_a_symbol(void)
{
  return FALSE ;
}

