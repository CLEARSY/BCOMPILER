/******************************* CLEARSY **************************************
* Fichier : $Id: d_misc.cpp,v 2.0 2007-07-25 09:58:51 arequet Exp $
* (C) 2008 CLEARSY
*
* Description :		Compilateur B
*					Betree persistant
*
* Compilation :		-DDEBUG_DUMP pour debugger le module
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
RCS_ID("$Id: d_misc.cpp,v 1.42 2007-07-25 09:58:51 arequet Exp $") ;

/* Includes systeme */
#include <iostream>
#include <fstream>
using namespace std;

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <map>

/* Includes Composant Local */
#include "c_api.h"
#include "d_dskman.h"
#include "d_dskobj.h"
#include "d_dskstr.h"
#include "c_chain2.h"
#include "c_md5.h"
#include "tinyxml.h"

// Fonction qui sauve les champs d'un objet connaissant sa classe
// La fonction s'appelle recursivement afin de remonter la hierarchie des
// classes afin de sauvegarder tout d'abord les champs de classes de base
void T_object::save_fields(T_metaclass *metaclass,
									T_disk_manager *disk_manager)
{
#ifdef DEBUG_DUMP
  TRACE4("save_fields(%x:\"%s\", metaclass %x:\"%s\")",
		 this,
		 get_class_name(),
		 metaclass,
		 metaclass->get_class_name()) ;
#endif

  T_metaclass *superclass = metaclass->get_superclass() ;
  if (superclass != NULL)
	{
	  // Appel recursif pour sauvegarder les champs de la superclasse
#ifdef DEBUG_DUMP
	  ENTER_TRACE ;
	  TRACE0("-> appel recursif pour la superclasse") ;
#endif
	  save_fields(superclass, disk_manager) ;
#ifdef DEBUG_DUMP
	  EXIT_TRACE ;
#endif
	}

  // Sauvegarde des champs specifiques
#ifdef DEBUG_DUMP
  TRACE1("DEBUT sauvegarde des champs specifiques de la classe %s",
		 metaclass->get_class_name()) ;
#endif

  T_field *cur_field = metaclass->get_fields() ;
  while (cur_field != NULL)
	{
#ifdef DEBUG_DUMP
	  TRACE4("sauvegarde du champ offset %s::%d : \"%s\" (instance %x)",
			 metaclass->get_class_name(),
			 cur_field->get_field_offset(),
			 cur_field->get_field_name(),
			 this) ;
#endif

	  // Adresse lue
	  size_t adr = (size_t)this + cur_field->get_field_offset() ;

	  switch(cur_field->get_field_type())
		{
		case FIELD_INTEGER :
		  {
			int value = *(int *)adr ;
			disk_manager->write(value) ;
			break ;
		  }

		case FIELD_OBJECT :
		  {
			T_object *value = *(T_object **)adr ;
#ifdef DEBUG_DUMP
			if (object_test(value) == FALSE)
			  {
				fprintf(stderr,
						"Panic : Instance %x de %s : champ %s = %x, pas un objet\n",
						(size_t)this,
						metaclass->get_class_name(),
						cur_field->get_field_name(),
						(size_t)value) ;
				TRACE4("Panic : Instance %x de %s : champ %s = %x, pas un objet\n",
					   this,
					   metaclass->get_class_name(),
					   cur_field->get_field_name(),
					   value) ;
				fflush(stderr) ;
				disk_manager->write((T_object *)NULL) ;
				break ;
			  }
#endif
			disk_manager->write(value) ;
			break ;
		  }

		case FIELD_STRING :
		  {
			const char *value = *(const char **)adr ;
			disk_manager->write(value) ;
			break ;
		  }

		case FIELD_POINTER :
		case FIELD_USER :
		  {
			void *value = *(void **)adr ;
			disk_manager->write(value) ;
			break ;
		  }

		case FIELD_FILLING :
		  {
			// Ne rien faire, les tables virtuelles sont traitees specialement.
			break ;
		  }
		case FIELD_RESET :
		  {
			disk_manager->write((int)0) ;
			break ;
		  }

		case FIELD_UPGRADE :
		  {
			// Ces champs ne sont pas sauves
			disk_manager->write((T_object *)NULL) ;
#ifdef DEBUG_DUMP
			TRACE0("champ sauve") ;
#endif
		  }

		  // Pas de default car on traite tous les cas
		  // Le compilateur nous previent si ce n'est pas le cas
		}

	  // Champ suivant
	  cur_field = cur_field->get_next() ;
	}

  // Sauvegarde eventuelle du champ vtbl a la fin de la classe
  T_field *first_field = metaclass->get_fields() ;
  T_field *last_field = metaclass->get_last_field() ;
  if ((first_field != NULL && first_field->get_field_type() == FIELD_FILLING) ||
	  ( last_field != NULL &&  last_field->get_field_type() == FIELD_FILLING))
	{
	  disk_manager->write((int)0) ;
	}

#ifdef DEBUG_DUMP
  TRACE0("FIN   sauvegarde des champs specifiques") ;
#endif
}

// Fonction qui sauve les champs d'un objet connaissant sa classe
// La fonction s'appelle recursivement afin de remonter la hierarchie des
// classes afin de sauvegarder tout d'abord les champs de classes de base
void T_object::load_fields(T_object *object,
									T_metaclass *metaclass)
{
#ifdef DEBUG_DUMP
  TRACE4("load_fields(%x:\"%s\", metaclass %x:\"%s\")",
		 this,
		 get_class_name(),
		 metaclass,
		 metaclass->get_class_name()) ;
#endif

  T_metaclass *superclass = metaclass->get_superclass() ;
  if (superclass != NULL)
	{
	  // Appel recursif pour sauvegarder les champs de la superclasse
#ifdef DEBUG_DUMP
	  ENTER_TRACE ;
	  TRACE0("-> appel recursif pour la superclasse") ;
#endif
	  load_fields(object, superclass) ;
#ifdef DEBUG_DUMP
	  EXIT_TRACE ;
#endif
	}

  // Lecture des champs specifiques
#ifdef DEBUG_DUMP
  TRACE1("DEBUT lecture des champs specifiques de %s", metaclass->get_class_name());
#endif

  T_field *cur_field = metaclass->get_fields() ;
  while (cur_field != NULL)
	{
#ifdef DEBUG_DUMP
	  TRACE3("champ \"%s\" : lecture offset %d,  ecriture offset %d",
			 cur_field->get_field_name(),
			 cur_field->get_field_offset_before_upgrade(),
			 cur_field->get_field_offset()) ;
#endif

	  // Adresse lue dans le buffer
	  size_t adr_load =
		((size_t)object + cur_field->get_field_offset_before_upgrade()) ;
	  // Adresse ou l'on sauvegarde
	  size_t adr_save = ((size_t)this + cur_field->get_field_offset()) ;

	  switch(cur_field->get_field_type())
		{
		case FIELD_INTEGER :
		  {
			int value = ntohl(*(int *)adr_load) ;
#ifdef DEBUG_DUMP
			TRACE1("FIELD_OBJECT -> lu valeur = %d", value) ;
#endif
			*(size_t *)adr_save = (size_t)value ;
			break ;
		  }

		case FIELD_OBJECT :
		case FIELD_STRING :
		case FIELD_USER :
		  {
			size_t value = ntohl(*(size_t *)adr_load) ;
#ifdef DEBUG_DUMP
			TRACE1("FIELD_OBJECT ou STRING -> lu index = %d", value) ;
#endif
			*(size_t *)adr_save = value ;
			break ;
		  }

		case FIELD_POINTER :
		case FIELD_FILLING :
		case FIELD_RESET :
		  {
			// Ces champs ne sont pas restaures
#ifdef DEBUG_DUMP
			TRACE0("champ non restaure") ;
#endif
			break ;
		  }

		case FIELD_UPGRADE :
		  {
			// Ces champs ne sont pas lus
#ifdef DEBUG_DUMP
			TRACE0("champ non lu") ;
#endif
		  }

		  // Pas de default car on traite tous les cas
		  // Le compilateur nous previent si ce n'est pas le cas
		}

	  // Champ suivant
	  cur_field = cur_field->get_next() ;
	}

#ifdef DEBUG_DUMP
  TRACE1("FIN   lecture des champs specifiques de %s", metaclass->get_class_name());
#endif
}

// Fonction qui resoud les champs d'un objet connaissant sa classe
// La fonction s'appelle recursivement afin de remonter la hierarchie des
// classes afin de restaurer tout d'abord les champs de classes de base
void T_object::solve_references(T_metaclass *metaclass)
{
#ifdef DEBUG_DUMP
  TRACE4("solve_references(%x:\"%s\", metaclass %x:\"%s\")",
		 this,
		 get_class_name(),
		 metaclass,
		 metaclass->get_class_name()) ;
#endif

  if (is_a_symbol() == FALSE)
	{
	  // Les symboles sont traites a part dans une passe precedente
	  T_metaclass *superclass = metaclass->get_superclass() ;
	  if (superclass != NULL)
		{
		  // Appel recursif pour sauvegarder les champs de la superclasse
		  ENTER_TRACE ;
#ifdef DEBUG_DUMP
		  TRACE0("-> appel recursif pour la superclasse") ;
#endif
		  solve_references(superclass) ;
		  EXIT_TRACE ;
		}

	  // Resolution des champs specifiques
#ifdef DEBUG_DUMP
	  TRACE0("DEBUT resolution des champs specifiques") ;
#endif
	  T_field *cur_field = metaclass->get_fields() ;
	  while (cur_field != NULL)
		{
#ifdef DEBUG_DUMP
		  TRACE4("this <%x> :: resolution du champ offset %d : \"%s:%s\"",
				 this,
				 cur_field->get_field_offset(),
				 metaclass->get_class_name(),
				 cur_field->get_field_name()) ;
#endif

		  // Addresse dans l'objet
		  size_t adr = (size_t)this + cur_field->get_field_offset() ;
#ifdef DEBUG_DUMP
		  TRACE2("on resoud l'index %d (type %d)",
				 *(size_t *)adr,
				 cur_field->get_field_type()) ;
#endif

		  switch(cur_field->get_field_type())
			{
			case FIELD_OBJECT :
			  {
				T_object *value = (T_object *)disk_object_find_address(*(size_t *)adr) ;
				*(T_object **)adr = value ;
#ifdef DEBUG_DUMP
				TRACE1("FIELD_OBJECT -> lu valeur = %x", value) ;
#endif
				break ;
			  }

			case FIELD_STRING :
			  {
				char *value =(char *)disk_string_find_string(*(size_t *)adr) ;
				*(char **)adr = value ;

#ifdef DEBUG_DUMP
				TRACE1("FIELD_STRING -> lu valeur = %s",
					   (value == NULL) ? NULL : value) ;
#endif
				break ;
			  }

			case FIELD_INTEGER :
			case FIELD_POINTER :
			case FIELD_FILLING :
			case FIELD_RESET :
			case FIELD_USER :
			case FIELD_UPGRADE :
			  {
				// Ces champs ne sont pas resolus
#ifdef DEBUG_DUMP
				TRACE0("champ non resolus") ;
#endif
				break ;
			  }

			  // Pas de default car on traite tous les cas
			  // Le compilateur nous previent si ce n'est pas le cas
			}

		  // Champ suivant
		  cur_field = cur_field->get_next() ;
		}
	}

#ifdef DEBUG_DUMP
  TRACE0("FIN   resolution des champs specifiques") ;
#endif
}

// Methode de dump sur disque
void T_object::dump_disk(T_disk_manager *disk_manager)
{
#ifdef DEBUG_DUMP
  TRACE4("T_object(%s:%x)::dump_disk cur_offset = %d disk_manager=%x",
		 get_class_name(),
		 this,
		 disk_manager->get_offset(),
		 disk_manager) ;
  ENTER_TRACE ;
#endif // DEBUG_DUMP
  size_t start = disk_manager->get_offset() ;

  // Boucle de sauvegarde de tous les champs
  T_metaclass *metaclass = get_metaclass(get_node_type()) ;
  ASSERT(metaclass != NULL) ;

  // Sauvegarde des champs
  save_fields(metaclass, disk_manager) ;

  size_t end = disk_manager->get_offset() ;
  assert(end - start == metaclass->get_size()) ;

#ifdef DEBUG_DUMP
  EXIT_TRACE ;
  TRACE1("fin de T_object(%x)::dump_disk()", this) ;
#endif // DEBUG_DUMP
}

// Creer un object vide d'un node_type donne

T_object *create_object_of_type(T_node_type node_type,
										 int &incr,
										 T_item **adr_first_read_symbol,
										 T_item **adr_last_read_symbol,
										 int cur_adr)
{
  //  TRACE1("create_object_of_type(%d)", node_type) ;

  // Malheureusement on ne peut pas ecrire incr = sizeof(*this) car
  // ca ne compile pas sous Windows95
  switch (node_type)
	{
	case NODE_MACHINE 	:	 /* 00 */
	  {
		incr = get_class_size_before_upgrade(NODE_MACHINE) ;
		return new T_machine() ;
	  }
	case NODE_BETREE   	: 	/* 01 */
	  {
		incr = get_class_size_before_upgrade(NODE_BETREE) ;
		return new T_betree() ;
	  }
	case NODE_SYMBOL   	: 	/* 02 */
	  {
		// On fabrique un T_list_link qui pointe sur le symbole
		// temporaire. On stocke dans cet objet l'adresse courante
		// pour pouvoir la corriger plus tard.
		incr = get_class_size_before_upgrade(NODE_SYMBOL) ;
		T_symbol *symbol = new T_symbol() ;
		// On verifie qu'on est en mode "lecture disque"
		ASSERT(cur_adr != 0) ;
		ASSERT(adr_first_read_symbol != NULL) ;
		ASSERT(adr_last_read_symbol != NULL) ;
		TRACE1("creation du symbole %x dans d_misc.cpp", symbol) ;
		T_list_link *res = new T_list_link(symbol,
										   LINK_READ_SYMBOL,
										   adr_first_read_symbol,
										   adr_last_read_symbol,
										   NULL, /* father */
										   NULL, /* betree */
										   NULL);/* lexem  */
		res->set_tmp2(cur_adr) ;
		// On renvoie le symbole pour que l'algo general continue
		// comme si tout etait normal
		return symbol ;
	  }
	case NODE_LEXEM		: 	/* 03 */
	  {
		incr = get_class_size_before_upgrade(NODE_LEXEM) ;
		return new T_lexem() ;
	  }
	case NODE_OPERATION	:	/* 04 */
	  {
		incr = get_class_size_before_upgrade(NODE_OPERATION) ;
		return new T_op() ;
	  }
	case NODE_SKIP	:		/* 05 */
	  {
		incr = get_class_size_before_upgrade(NODE_SKIP) ;
		return new T_skip() ;
	  }
	case NODE_BEGIN	:		/* 06 */
	  {
		incr = get_class_size_before_upgrade(NODE_BEGIN) ;
		return new T_begin() ;
	  }
	case NODE_USED_OP	:		/* 07 */
	  {
		incr = get_class_size_before_upgrade(NODE_USED_OP) ;
		return new T_used_op() ;
	  }
	case NODE_CHOICE	:	/* 08 */
	  {
		incr = get_class_size_before_upgrade(NODE_CHOICE) ;
		return new T_choice() ;
	  }
	case NODE_OR	:	/* 09 */
	  {
		incr = get_class_size_before_upgrade(NODE_OR) ;
		return new T_or() ;
	  }
	case NODE_PRECOND	:	/* 10 */
	  {
		incr = get_class_size_before_upgrade(NODE_PRECOND) ;
		return new T_precond() ;
	  }
	case NODE_ASSERT	:	/* 11 */
	  {
		incr = get_class_size_before_upgrade(NODE_ASSERT) ;
		return new T_assert() ;
	  }
        case NODE_IF	:	/* 12 */
	  {
		incr = get_class_size_before_upgrade(NODE_IF) ;
		return new T_if() ;
	  }
	case NODE_ELSE	:	/* 13 */
	  {
		incr = get_class_size_before_upgrade(NODE_ELSE) ;
		return new T_else() ;
	  }
	case NODE_SELECT	:	/* 14 */
	  {
		incr = get_class_size_before_upgrade(NODE_SELECT) ;
		return new T_select() ;
	  }
	case NODE_WHEN	:	/* 15 */
	  {
		incr = get_class_size_before_upgrade(NODE_WHEN) ;
		return new T_when() ;
	  }
	case NODE_PREDICATE	:	/* 16 */
	  {
		incr = get_class_size_before_upgrade(NODE_PREDICATE) ;
		return new T_predicate() ;
	  }
	case NODE_CASE	:		/* 18 */
	  {
		incr = get_class_size_before_upgrade(NODE_CASE) ;
		return new T_case() ;
	  }
	case NODE_CASE_ITEM	:	/* 19 */
	  {
		incr = get_class_size_before_upgrade(NODE_CASE_ITEM) ;
		return new T_case_item() ;
	  }
	case NODE_LIST_LINK	:	/* 20 */
	  {
		incr = get_class_size_before_upgrade(NODE_LIST_LINK) ;
		return new T_list_link() ;
	  }
	case NODE_LET	:		/* 21 */
	  {
		incr = get_class_size_before_upgrade(NODE_LET) ;
		return new T_let() ;
	  }
	case NODE_VALUATION	:	/* 22 */
	  {
		incr = get_class_size_before_upgrade(NODE_VALUATION) ;
		return new T_valuation() ;
	  }
	case NODE_IDENT	:		/* 23 */
	  {
		incr = get_class_size_before_upgrade(NODE_IDENT) ;
		return new T_ident() ;
	  }
	case NODE_VAR	:		/* 24 */
	  {
		incr = get_class_size_before_upgrade(NODE_VAR) ;
		return new T_var() ;
	  }
	case NODE_WHILE	:		/* 25 */
	  {
		incr = get_class_size_before_upgrade(NODE_WHILE) ;
		return new T_while() ;
	  }
	case NODE_ANY	:		/* 26 */
	  {
		incr = get_class_size_before_upgrade(NODE_ANY) ;
		return new T_any() ;
	  }
	case NODE_AFFECT	:		/* 27 */
	  {
		incr = get_class_size_before_upgrade(NODE_AFFECT) ;
		return new T_affect() ;
	  }
	case NODE_BECOMES_MEMBER_OF	: /* 28 */
	  {
		incr = get_class_size_before_upgrade(NODE_BECOMES_MEMBER_OF) ;
		return new T_becomes_member_of() ;
	  }
	case NODE_BECOMES_SUCH_THAT	: /* 29 */
	  {
		incr = get_class_size_before_upgrade(NODE_BECOMES_SUCH_THAT) ;
		return new T_becomes_such_that() ;
	  }
	case NODE_OP_CALL	: /* 30 */
	  {
		incr = get_class_size_before_upgrade(NODE_OP_CALL) ;
		return new T_op_call() ;
	  }
	case NODE_COMMENT	:	/* 31 */
	  {
		incr = get_class_size_before_upgrade(NODE_COMMENT) ;
		return new T_comment() ;
	  }
	case NODE_FLAG		:	/* 32 */
	  {
		incr = get_class_size_before_upgrade(NODE_FLAG) ;
		return new T_flag() ;
	  }
	case NODE_DEFINITION 	:	/* 33 */
	  {
		incr = get_class_size_before_upgrade(NODE_DEFINITION) ;
		return new T_definition() ;
	  }
	case NODE_USED_MACHINE 	:	/* 34 */
	  {
		incr = get_class_size_before_upgrade(NODE_USED_MACHINE) ;
		return new T_used_machine() ;
	  }
	case NODE_LITERAL_INTEGER :	/* 35 */
	  {
		incr = get_class_size_before_upgrade(NODE_LITERAL_INTEGER) ;
		return new T_literal_integer() ;
	  }
	case NODE_BINARY_OP :	/* 36 */
	  {
		incr = get_class_size_before_upgrade(NODE_BINARY_OP) ;
		return new T_binary_op() ;
	  }
	case NODE_EXPR_WITH_PARENTHESIS :	/* 37 */
	  {
		incr = get_class_size_before_upgrade(NODE_EXPR_WITH_PARENTHESIS) ;
		return new T_expr_with_parenthesis() ;
	  }
	case NODE_UNARY_OP :	/* 38 */
	  {
		incr = get_class_size_before_upgrade(NODE_UNARY_OP) ;
		return new T_unary_op() ;
	  }
	case NODE_ARRAY_ITEM :		   	/* 39 */
	  {
		incr = get_class_size_before_upgrade(NODE_ARRAY_ITEM) ;
		return new T_array_item() ;
	  }
	case NODE_CONVERSE :		   	/* 40 */
	  {
		incr = get_class_size_before_upgrade(NODE_CONVERSE) ;
		return new T_converse() ;
	  }
        case NODE_LITERAL_STRING :	/* 41 */
	  {
		incr = get_class_size_before_upgrade(NODE_LITERAL_STRING) ;
		return new T_literal_string() ;
	  }
	case NODE_KEYWORD   	: 	/* 42 */
	  {
		// On fabrique un T_list_link qui pointe sur le symbole
		// temporaire. On stocke dans cet objet l'adresse courante
		// pour pouvoir la corriger plus tard.
		incr = get_class_size_before_upgrade(NODE_KEYWORD) ;
		T_keyword *keyword = new T_keyword() ;
		// On verifie qu'on est en mode "lecture disque"
		ASSERT(cur_adr != 0) ;
		ASSERT(adr_first_read_symbol != NULL) ;
		ASSERT(adr_last_read_symbol != NULL) ;
		TRACE1("creation du symbole %x dans d_misc.cpp", keyword) ;
		T_list_link *res = new T_list_link(keyword,
										   LINK_READ_SYMBOL,
										   adr_first_read_symbol,
										   adr_last_read_symbol,
										   NULL, /* father */
										   NULL, /* betree */
										   NULL);/* lexem  */
		res->set_tmp2(cur_adr) ;
		// On renvoie le symbole pour que l'algo general continue
		// comme si tout etait normal
		return keyword ;
	  }
	case NODE_LAMBDA_EXPR :	/* 43 */
	  {
		incr = get_class_size_before_upgrade(NODE_LAMBDA_EXPR) ;
		return new T_lambda_expr() ;
	  }
	case NODE_BINARY_PREDICATE :	/* 44 */
	  {
		incr = get_class_size_before_upgrade(NODE_BINARY_PREDICATE) ;
		return new T_binary_predicate() ;
	  }
	case NODE_TYPING_PREDICATE :	/* 45 */
	  {
		incr = get_class_size_before_upgrade(NODE_TYPING_PREDICATE) ;
		return new T_typing_predicate() ;
	  }
	case NODE_COMPREHENSIVE_SET :	/* 46 */
	  {
		incr = get_class_size_before_upgrade(NODE_COMPREHENSIVE_SET) ;
		return new T_comprehensive_set() ;
	  }
	case NODE_IMAGE :	/* 47 */
	  {
		incr = get_class_size_before_upgrade(NODE_IMAGE) ;
		return new T_image() ;
	  }
	case NODE_PREDICATE_WITH_PARENTHESIS :	/* 48 */
	  {
		incr = get_class_size_before_upgrade(NODE_PREDICATE_WITH_PARENTHESIS) ;
		return new T_predicate_with_parenthesis() ;
	  }
	case NODE_EXTENSIVE_SET :	/* 49 */
	  {
		incr = get_class_size_before_upgrade(NODE_EXTENSIVE_SET) ;
		return new T_extensive_set() ;
	  }
	case NODE_NOT_PREDICATE :	/* 50 */
	  {
		incr = get_class_size_before_upgrade(NODE_NOT_PREDICATE) ;
		return new T_not_predicate() ;
	  }
	case NODE_EXPR_PREDICATE :	/* 51 */
	  {
		incr = get_class_size_before_upgrade(NODE_EXPR_PREDICATE) ;
		return new T_expr_predicate() ;
	  }

	case NODE_UNIVERSAL_PREDICATE :	/* 52 */
	  {
		incr = get_class_size_before_upgrade(NODE_UNIVERSAL_PREDICATE) ;
		return new T_universal_predicate() ;
	  }
	case NODE_EXISTENTIAL_PREDICATE :	/* 53 */
	  {
		incr = get_class_size_before_upgrade(NODE_EXISTENTIAL_PREDICATE) ;
		return new T_existential_predicate() ;
	  }
	case NODE_EMPTY_SEQ :	/* 54 */
	  {
		incr = get_class_size_before_upgrade(NODE_EMPTY_SEQ) ;
		return new T_empty_seq() ;
	  }
	case NODE_EMPTY_SET :	/* 55 */
	  {
		incr = get_class_size_before_upgrade(NODE_EMPTY_SET) ;
		return new T_empty_set() ;
	  }
	case NODE_SIGMA :	/* 56 */
	  {
		incr = get_class_size_before_upgrade(NODE_SIGMA) ;
		return new T_sigma() ;
	  }
	case NODE_PI :	/* 57 */
	  {
		incr = get_class_size_before_upgrade(NODE_PI) ;
		return new T_pi() ;
	  }
	case NODE_QUANTIFIED_UNION :	/* 58 */
	  {
		incr = get_class_size_before_upgrade(NODE_QUANTIFIED_UNION) ;
		return new T_quantified_union() ;
	  }
	case NODE_QUANTIFIED_INTERSECTION :	/* 59 */
	  {
		incr = get_class_size_before_upgrade(NODE_QUANTIFIED_INTERSECTION) ;
		return new T_quantified_intersection() ;
	  }
	case NODE_LITERAL_BOOLEAN :	/* 60 */
	  {
		incr = get_class_size_before_upgrade(NODE_LITERAL_BOOLEAN) ;
		return new T_literal_boolean() ;
	  }
	case NODE_LITERAL_ENUMERATED_VALUE :	/* 61 */
	  {
		incr = get_class_size_before_upgrade(NODE_LITERAL_ENUMERATED_VALUE) ;
		return new T_literal_enumerated_value() ;
	  }
	case NODE_RELATION :	/* 62 */
	  {
		incr = get_class_size_before_upgrade(NODE_RELATION) ;
		return new T_relation() ;
	  }
	case NODE_RENAMED_IDENT :	/* 63 */
	  {
		incr = get_class_size_before_upgrade(NODE_RENAMED_IDENT) ;
		return new T_renamed_ident() ;
	  }
	case NODE_EXTENSIVE_SEQ :	/* 64 */
	  {
		incr = get_class_size_before_upgrade(NODE_EXTENSIVE_SEQ) ;
		return new T_extensive_seq() ;
	  }

	case NODE_GENERALISED_UNION :	/* 65 */
	  {
		incr = get_class_size_before_upgrade(NODE_GENERALISED_UNION) ;
		return new T_generalised_union() ;
	  }
	case NODE_GENERALISED_INTERSECTION :	/* 66 */
	  {
		incr = get_class_size_before_upgrade(NODE_GENERALISED_INTERSECTION) ;
		return new T_generalised_intersection() ;
	  }
	  /*
		67 NODE_OBJECT : classe abstraite
		68 NODE_ITEM : classe_abstraite
		69 NODE_SUBSTITUTION : classe abstraite
	  */
	case NODE_INTEGER :	/* 70 */
	  {
		incr = get_class_size_before_upgrade(NODE_INTEGER) ;
		return new T_integer() ;
	  }

	case NODE_BETREE_MANAGER : /* 71 */
	  {
		incr = get_class_size_before_upgrade(NODE_BETREE_MANAGER) ;
		return new T_betree_manager() ;
	  }
	case NODE_BETREE_LIST : /* 72 */
	  {
		incr = get_class_size_before_upgrade(NODE_BETREE_LIST) ;
		return new T_betree_list() ;
	  }

	  /*
		NODE_TYPE 73 : classe abstraite
	  */

	case NODE_PRODUCT_TYPE : /* 74 */
	  {
		incr = get_class_size_before_upgrade(NODE_PRODUCT_TYPE) ;
		return new T_product_type() ;
	  }
	case NODE_SETOF_TYPE : /* 75 */
	  {
		incr = get_class_size_before_upgrade(NODE_SETOF_TYPE) ;
		return new T_setof_type() ;
	  }
	case NODE_ABSTRACT_TYPE : /* 76 */
	  {
		incr = get_class_size_before_upgrade(NODE_ABSTRACT_TYPE) ;
		return new T_abstract_type() ;
	  }
	case NODE_ENUMERATED_TYPE : /* 77 */
	  {
		incr = get_class_size_before_upgrade(NODE_ENUMERATED_TYPE) ;
		return new T_enumerated_type() ;
	  }
	case NODE_PREDEFINED_TYPE : /* 78 */
	  {
		incr = get_class_size_before_upgrade(NODE_PREDEFINED_TYPE) ;
		return new T_predefined_type() ;
	  }
	  /*
		NODE_BASE_TYPE : 79 classe abstraite
		NODE_CONSTRUCTOR_TYPE : 80 classe abstraite
	  */
	case NODE_GENERIC_TYPE : /* 81 */
	  {
		incr = get_class_size_before_upgrade(NODE_GENERIC_TYPE) ;
		return new T_generic_type() ;
	  }
	  /*
		NODE_GENERIC_OP : 82 classe abstraite
	  */
	case NODE_BOUND : /* 83 */
	  {
		incr = get_class_size_before_upgrade(NODE_BOUND) ;
		return new T_bound() ;
	  }
	case NODE_OP_RESULT :		   	/* 84 */
	  {
		incr = get_class_size_before_upgrade(NODE_OP_RESULT) ;
		return new T_op_result() ;
	  }
	case NODE_PRAGMA : /* 85 */
	  {
		incr = get_class_size_before_upgrade(NODE_PRAGMA) ;
		return new T_pragma() ;
	  }
	case NODE_PRAGMA_LEXEM : /* 86 */
	  {
		incr = get_class_size_before_upgrade(NODE_PRAGMA_LEXEM) ;
		return new T_pragma_lexem() ;
	  }
	  /*
		NODE_B0_TYPE : 87 classe abstraite
	  */
	case NODE_B0_BASE_TYPE : /* 88 */
	  {
		incr = get_class_size_before_upgrade(NODE_B0_BASE_TYPE) ;
		return new T_B0_base_type() ;
	  }
	case NODE_B0_ABSTRACT_TYPE : /* 89 */
	  {
		incr = get_class_size_before_upgrade(NODE_B0_ABSTRACT_TYPE) ;
		return new T_B0_abstract_type() ;
	  }
	case NODE_B0_ENUMERATED_TYPE : /* 90 */
	  {
		incr = get_class_size_before_upgrade(NODE_B0_ENUMERATED_TYPE) ;
		return new T_B0_enumerated_type() ;
	  }
	case NODE_B0_INTERVAL_TYPE : /* 91 */
	  {
		incr = get_class_size_before_upgrade(NODE_B0_INTERVAL_TYPE) ;
		return new T_B0_interval_type() ;
	  }
	case NODE_B0_ARRAY_TYPE : /* 92 */
	  {
		incr = get_class_size_before_upgrade(NODE_B0_ARRAY_TYPE) ;
		return new T_B0_array_type() ;
	  }
	case NODE_RECORD : /* 93 */
	  {
		incr = get_class_size_before_upgrade(NODE_RECORD) ;
		return new T_record() ;
	  }
	case NODE_RECORD_ITEM : /* 94 */
	  {
		incr = get_class_size_before_upgrade(NODE_RECORD_ITEM) ;
		return new T_record_item() ;
	  }
	case NODE_STRUCT : /* 95 */
	  {
		incr = get_class_size_before_upgrade(NODE_STRUCT) ;
		return new T_struct() ;
	  }
	case NODE_RECORD_ACCESS : /* 96 */
	  {
		incr = get_class_size_before_upgrade(NODE_RECORD_ACCESS) ;
		return new T_record_access() ;
	  }
	case NODE_RECORD_TYPE : /* 97 */
	  {
		incr = get_class_size_before_upgrade(NODE_RECORD_TYPE) ;
		return new T_record_type() ;
	  }
	case NODE_LABEL_TYPE : /* 98 */
	  {
		incr = get_class_size_before_upgrade(NODE_LABEL_TYPE) ;
		return new T_label_type() ;
	  }
	case NODE_B0_RECORD_TYPE : /* 99 */
	  {
		incr = get_class_size_before_upgrade(NODE_B0_RECORD_TYPE) ;
		return new T_B0_record_type() ;
	  }
	case NODE_B0_LABEL_TYPE : /* 100 */
	  {
		incr = get_class_size_before_upgrade(NODE_B0_LABEL_TYPE) ;
		return new T_B0_label_type() ;
	  }
	case NODE_MSG_LINE : /* 101 */
	  {
		incr = get_class_size_before_upgrade(NODE_MSG_LINE) ;
		return new T_msg_line() ;
	  }
	case NODE_MSG_LINE_MANAGER : /* 102 */
	  {
		incr = get_class_size_before_upgrade(NODE_MSG_LINE_MANAGER) ;
		return new T_msg_line_manager() ;
	  }
	case NODE_COMPONENT : /* 103 */
	  {
		incr = get_class_size_before_upgrade(NODE_COMPONENT) ;
		return new T_component() ;
	  }
	case NODE_FILE_COMPONENT : /* 104 */
	  {
		incr = get_class_size_before_upgrade(NODE_FILE_COMPONENT) ;
		return new T_file_component() ;
	  }
	case NODE_PROJECT : /* 105 */
	  {
		incr = get_class_size_before_upgrade(NODE_PROJECT) ;
		return new T_project() ;
	  }
	case NODE_PROJECT_MANAGER : /* 106 */
	  {
		incr = get_class_size_before_upgrade(NODE_PROJECT_MANAGER) ;
		return new T_project_manager() ;
	  }
	case NODE_FILE_DEFINITION : /* 107 */
	  {
		incr = get_class_size_before_upgrade(NODE_FILE_DEFINITION) ;
		return new T_file_definition() ;
	  }

	case NODE_COMPONENT_CHECKSUMS : /* 108 */
	  {
		incr = get_class_size_before_upgrade(NODE_COMPONENT_CHECKSUMS) ;
		return new T_component_checksums() ;
	  }
	case NODE_OPERATION_CHECKSUMS : /* 109 */
	  {
		incr = get_class_size_before_upgrade(NODE_OPERATION_CHECKSUMS) ;
		return new T_operation_checksums() ;
	  }
	case NODE_OP_CALL_TREE : /* 110 */
	  {
		incr = get_class_size_before_upgrade(NODE_OP_CALL_TREE) ;
		return new T_op_call_tree() ;
	  }
	case NODE_WITNESS	:	/* 111 */
	  {
		incr = get_class_size_before_upgrade(NODE_WITNESS) ;
		return new T_witness() ;
	  }
        case NODE_LITERAL_REAL :	/* 112 */
          {
                incr = get_class_size_before_upgrade(NODE_LITERAL_REAL) ;
                return new T_literal_real() ;
          }
        case NODE_LABEL	:	/* 113 */
          {
                incr = get_class_size_before_upgrade(NODE_LABEL) ;
                return new T_label() ;
          }
        case NODE_JUMPIF	:	/* 114 */
          {
                incr = get_class_size_before_upgrade(NODE_JUMPIF) ;
                return new T_jumpif() ;
          }

        case NODE_USER_DEFINED1 : /* 115 */
	case NODE_USER_DEFINED2 :
	case NODE_USER_DEFINED3 :
	case NODE_USER_DEFINED4 :
	case NODE_USER_DEFINED5 :
	case NODE_USER_DEFINED6 :
	case NODE_USER_DEFINED7 :
	case NODE_USER_DEFINED8 :
	case NODE_USER_DEFINED9 :
	case NODE_USER_DEFINED10 :
	case NODE_USER_DEFINED11 :
	case NODE_USER_DEFINED12 :
	case NODE_USER_DEFINED13 :
	case NODE_USER_DEFINED14 :
	case NODE_USER_DEFINED15 :
	case NODE_USER_DEFINED16 :
	case NODE_USER_DEFINED17 :
	case NODE_USER_DEFINED18 :
	case NODE_USER_DEFINED19 :
	case NODE_USER_DEFINED20 :
	  {
		// Appel de la fonction utilisateur
		return (get_user_create_method(node_type))(node_type, incr) ;
	  }

	default :
	  {
		fprintf(stderr,
				"%s::%d :: INTERNAL ERROR :: node type %d is unknown\n",
				__FILE__,
				__LINE__,
				node_type) ;
		assert(FALSE) ; // A FAIRE
	  }
	}

  return NULL ;
}

// Recuperation des champs a partir d'une image disque
void T_object::load_disk(T_object *object)
{
  TRACE2("T_object<%s>::load_disk(%x)", get_class_name(), object) ;

  // Boucle de lecture de tous les champs
  T_metaclass *metaclass = get_metaclass(get_node_type()) ;
  ASSERT(metaclass != NULL) ;

  load_fields(object, metaclass) ;

  EXIT_TRACE ;

  //  TRACE1("fin T_object::load_disk(%x)", object) ;
}


void T_object::solve_objects(void)
{
  // Boucle de resolution de tous les champs
  T_metaclass *metaclass = get_metaclass(get_node_type()) ;
  ASSERT(metaclass != NULL) ;

  solve_references(metaclass) ;
}

// Methode de sauvegarde sur disque
T_io_diagnostic T_object::save_object(const char *file_name,
											   T_betree_file_type file_type)
{
  TRACE2("T_object(%x)::save_object -> fichier \"%s\"", this, file_name) ;

  if (get_no_exit_mode() == TRUE)
	{
	  jmp_buf env ;			// Pour retour en cas d'erreur

	  if (setjmp(env) != 0)
		{
		  // On est en retour d'erreur ! */
		  TRACE0("retour d'erreur dans T_object::save_object") ;
		  return IO_ERROR ;
		}

	  set_exit_handler(&env) ;
	}

  // Initialisations
  TRACE0("creation disk manager") ;
  T_disk_manager *disk_manager = init_disk_object_write(NULL, file_name, file_type);
  TRACE0("init disk_string_write") ;
  init_disk_string_write(disk_manager) ;
  TRACE0("reset chain") ;
  reset_chain2(NULL) ;

  TRACE0("dump_disk(disk_manager)") ;
  //  dump_disk(disk_manager) ;

  // Amorcage
  chain2_add(this) ;
  disk_object_get_object_index(this) ;

  TRACE3("maj rank disk manager (%x) : this %x -> get_tmp %x",
		 get_tmp(),
		 this,
		 get_tmp()) ;
  ((T_disk_object *)get_tmp())->set_rank() ;

  TRACE0("DEBUT BOUCLE DE SAUVEGARDE DES ELEMENTS RESTANTS") ;
  // Boucle de sauvegarde des elements restants
  T_object *item ;

#ifdef DEBUG_DUMP
  // On compte les objets sauves
  static int nb_save = 0 ;
#endif // DEBUG_DUMP

  while ((item = chain2_get()) != NULL)
	{

#ifdef DEBUG_DUMP
		TRACE3(">>>>>>>>>>> SAUVEGARDE %x (%s) no save=%d",
			 item,
			 make_class_name(item),
			 ++nb_save) ;
			 ENTER_TRACE ; ENTER_TRACE ;
			 TRACE1("get->%x", item) ;
#endif // DEBUG_DUMP

			 size_t cur_offset = disk_manager->get_offset() ;
	  item->dump_disk(disk_manager) ;

#ifdef DEBUG_DUMP
	  TRACE2("item(%x)->get_tmp()= %x", item, item->get_tmp()) ;
#endif // DEBUG_DUMP

	  ((T_disk_object *)item->get_tmp())->set_offset(cur_offset) ;
	  ((T_disk_object *)item->get_tmp())->set_rank() ;

#ifdef DEBUG_DUMP
	  EXIT_TRACE ; EXIT_TRACE ;
	  TRACE2("<<<<<<<<<<< SAUVEGARDE %x (%s)", item, item->get_class_name()) ;
#endif // DEBUG_DUMP
	}

  TRACE0("FIN BOUCLE DE SAUVEGARDE DES ELEMENTS RESTANTS") ;

  // Ecriture de la table des offsets
  size_t object_offset = dump_disk_object() ;

  // Ecriture de la table des chaines
  size_t string_offset = dump_disk_string() ;

  // Ecriture des offsets des tables
  disk_manager->write((int)object_offset) ;
  disk_manager->write((int)string_offset) ;

  TRACE2("ECRIT offset de la table = %d = 0x%x", object_offset, object_offset) ;
  TRACE2("ECRIT offset de la table string = %d = 0x%x", string_offset, string_offset) ;

  // Fin de la sauvegarde
  exit_disk_object() ;

  return IO_OK ;
}


T_io_diagnostic T_component::save_checksums(const char *file_name,
                                    T_betree_file_type file_type)
{
    if (get_no_exit_mode() == TRUE)
      {
        jmp_buf env ;			// Pour retour en cas d'erreur

        if (setjmp(env) != 0)
          {
            // On est en retour d'erreur ! */
            TRACE0("retour d'erreur dans T_object::save_object") ;
            return IO_ERROR ;
          }

        set_exit_handler(&env) ;
      }

    // Initialisations
    TRACE0("creation disk manager") ;
    T_disk_manager *disk_manager = init_disk_object_write(NULL, file_name, file_type);
    TRACE0("init disk_string_write") ;
    init_disk_string_write(disk_manager) ;
    TRACE0("reset chain") ;
    reset_chain2(NULL) ;

    TRACE0("dump_disk(disk_manager)") ;
    //  dump_disk(disk_manager) ;

    // Amorcage
    chain2_add(this) ;
    disk_object_get_object_index(this) ;

    disk_manager->write_checksums(compo_checksums);

    // Fin de la sauvegarde
    exit_disk_object() ;

    return IO_OK ;
}

// Deplacement du champ vtbl pour le placer en tete de chaque objet.
void massage_load_area(void *start, void *end)
{
  size_t object_size = get_class_size_before_upgrade(NODE_OBJECT) ;

  void *cur_pos = start ;
  while (cur_pos < end)
	{
	  // Calcul de la taille de l'objet sur le disque
	  int disk_node_type = *(int *)((size_t) cur_pos + sizeof(int)) ;
	  T_node_type node_type = (T_node_type) ntohl(disk_node_type) ;
	  size_t size = get_class_size_before_upgrade(node_type) ;

	  // Deplacement de la fin de la partie T_object de l'objet sur disque
	  void *new_pos = (void *) ((size_t) cur_pos + sizeof(int)) ;
	  (void) memmove(new_pos, cur_pos, object_size - sizeof(int)) ;

	  // Vidage du premier champ
	  *(int *)cur_pos = 0 ;

	  cur_pos = (void *)((size_t) cur_pos + size) ;
	}
}


// Restauration disque
T_object *load_object(const char *file_name,
							   T_betree_file_type &betree_file_type,
							   T_io_diagnostic &io_diagnostic)
{
  TRACE2("--> load_object(%s, %d)", file_name, betree_file_type) ;
  ENTER_TRACE ; ENTER_TRACE ;

  if (get_no_exit_mode() == TRUE)
	{
	  TRACE0("mise en place mode retour erreur") ;
	  jmp_buf env ;			// Pour retour en cas d'erreur

	  if (setjmp(env) != 0)
		{
		  // On est en retour d'erreur ! */
		  TRACE0("retour d'erreur dans load_object") ;
		  io_diagnostic = IO_ERROR ;
		  return NULL ;
		}

	  set_exit_handler(&env) ;
	}

  io_diagnostic = IO_OK ;
  struct stat stats_ls ;

  FILE *fd_lip = fopen(file_name, "rb");

  // on se protege d'une eventuelle interruption par le kernel ou l'un des fils
  while (fd_lip == NULL && errno == EINTR)
	{
	  fd_lip = fopen(file_name, "rb");
	}

  TRACE1("Lecture du fichier %s", file_name) ;
  if ( (fd_lip == NULL) || (stat(file_name, &stats_ls) !=0) )
	{
	  TRACE0("retour d'erreur dans load_object") ;
	  io_diagnostic = IO_ERROR ;
	  toplevel_error(FATAL_ERROR,
					 get_error_msg(E_NO_SUCH_FILE),
					 file_name,
					 strerror(errno)) ;
	}

  size_t size = stats_ls.st_size ;
  TRACE2("Taille de %s = %d bytes", file_name, size) ;

  // PORTAGE :
  // On ne peut pas allouer par malloc un table "char" de size
  // octets puis le caster en void * car (par exemple sur
  // solaris 2.4) l'alignement n'est pas forcement respecte
  // i.e. on peut avoir un bloc dont le debut n'est pas
  // aligne sur sizeof(void *) -> ce qui est un mauvais
  // depart ! On va donc demander un bloc d'ints
  // afin d'etre sur d'avoir un bloc "aligne"
  ASSERT(sizeof(int) == sizeof(void *)) ;
  size_t nb_ints  = size / sizeof(int) ;
  if (nb_ints *sizeof(int) < size)
	{
	  // Il y a un reste, et on veut un majorant
	  nb_ints ++ ;
	}
  TRACE2("sizeof(int) = %d -> on veut %d ints", sizeof(int), nb_ints) ;

  void *load_sct = (void *)new int[nb_ints] ;

  if (fread(load_sct, 1, size,  fd_lip) <= 0)
	{
	  int res_close = fclose(fd_lip) ;
	  while (res_close == -1 && errno == EINTR)
		{
		  res_close = fclose(fd_lip) ;
		}
	  toplevel_error(FATAL_ERROR,
					 get_error_msg(E_PROBLEM_READING_FILE),
					 file_name,
					 strerror(errno)) ;
	}

  int res_close = fclose(fd_lip) ;
  while (res_close == -1 && errno == EINTR)
	{
	  res_close = fclose(fd_lip) ;
	}

  // Offset de la table des chaines en fin - sizeof(int)
  // Offset de la table des objets en fin - 2 * sizeof(int)
  TRACE1("load_sct = 0x%08x", load_sct) ;
  void *load_end = (void *)((size_t)load_sct + size) ;
  TRACE1("load_end = 0x%08x", load_end) ;

  size_t string_address = (size_t)(load_end) - sizeof(int) ;
  size_t object_address = string_address - sizeof(int) ;
  TRACE1("string_address = 0x%08x", string_address) ;
  size_t string_offset = *(size_t *)string_address ;
  size_t object_offset = *(size_t *)object_address ;

  TRACE1("load_end = %x", load_end) ;
  TRACE1("string_address = %x", string_address) ;
  TRACE1("object_address = %x", object_address) ;
  TRACE2("string_offset = 0x%08x, object_offset = 0x%08x",
		 string_offset,
		 object_offset) ;
  string_offset = ntohl(string_offset) ;
  object_offset = ntohl(object_offset) ;

  TRACE2("offset de la table = %d = 0x%x", object_offset, object_offset) ;
  TRACE2("offset de la table string = %d = 0x%x", string_offset, string_offset) ;
  size_t string_len = (size_t)
	((size_t)load_end - string_offset - (size_t)load_sct - 2 * sizeof(int)) ;
  TRACE1("longueur de la table des chaines = %d", string_len) ;
  ASSERT(string_len >= 0) ;
  size_t table_len = (size_t)(string_offset - object_offset) ;
  TRACE1("longueur de la table des objets = %d", table_len) ;
  TRACE1("longueur de la table des objets = %u", table_len) ;
  TRACE1("longueur de la table des objets = %ud", table_len) ;
  ASSERT(table_len >= 0) ;
  size_t max_rank = table_len / sizeof(void *) + 2 ; // 1 pour la
  // securite de l'arrondi, 1 pour le tableau qui demarre a 0
  TRACE1("max_rank %d", max_rank) ;

  // On reconstitue la table des chaines
  init_rank_to_object_table(max_rank) ;
  init_disk_string_read() ;
  void *string_start = (void *)((size_t)load_sct + string_offset) ;


  load_disk_string(string_start, string_len) ;

  // On reconstitue la table des objets
  init_disk_object_read(load_sct) ;

  void *real_start = load_sct ;
  size_t magic = (size_t)get_integer(real_start) ;

  if (betree_file_type == FILE_READ_JOKER)
	{
	  // lecture du type depuis le fichier
	  betree_file_type = magic2file_type(magic) ;
	}
  else if (magic != get_file_magic(betree_file_type))
	{
	  // Mauvais magic number
	  toplevel_error(FATAL_ERROR,
					 get_error_msg(E_BAD_MAGIC),
					 get_file_magic(betree_file_type),
					 magic,
					 file_name) ;

	}

  void *table_start = (void *)((size_t)load_sct + object_offset) ;

  size_t cpt = load_disk_object(table_start, table_len) ;
  TRACE1("on commence a <%d>", cpt) ;

  // Initialisation liste des symboles lus
  T_list_link *first_read_symbol = NULL ;
  T_list_link *last_read_symbol = NULL ;

  // On lit le betree
  T_object *res = NULL ;
  T_disk_object *betree_table = NULL ;

  load_sct = real_start ;
#ifdef DEBUG_DUMP
  size_t index = 0 ;
#endif

  T_field *vtbl_field = get_metaclass(NODE_OBJECT)->find_field("vtbl") ;
  if (vtbl_field->get_prev() == NULL)
	{
	  // Il faut modifier l'image disque pour la faire correspondre a
	  // l'image memoire, en deplacant le champ vtbl de chaque objet.
	  massage_load_area(load_sct, table_start) ;
	}

  // Recherche de l'offset du champ "node_type" qui sera utilise dans la boucle suivante.
  T_field *node_type_field = get_metaclass(NODE_OBJECT)->find_field("node_type") ;
  size_t node_type_offset = node_type_field->get_field_offset_before_upgrade() ;

  while (load_sct < table_start)
	{
	  // On trouve le node type
#ifdef DEBUG_DUMP
	  TRACE1("-- OBJET SUIVANT (index %d) --", ++index) ;
	  TRACE2("load_sct = 0x%08x, table_start = 0x%08x", load_sct, table_start) ;
#endif // DEBUG_DUMP
	  int disk_node_type = *(int *)((size_t) load_sct + node_type_offset) ;
	  T_node_type node_type = (T_node_type)ntohl(disk_node_type) ;

	  ENTER_TRACE ; ENTER_TRACE ;
	  int incr ; // taille du nouvel objet
	  T_object *new_object = create_object_of_type(node_type,
												   incr,
												   (T_item **)&first_read_symbol,
												   (T_item **)&last_read_symbol,
												   cpt) ;
#ifdef DEBUG_DUMP
	  TRACE3("nouvel objet -> @0x%x (classe %s) incr %d",
			 new_object,
			 new_object->get_class_name(),
			 incr) ;
#endif // DEBUG_DUMP
	  T_disk_object *table = disk_object_set_address(cpt++, new_object) ;

	  if (res == NULL)
		{
		  // Le premier betree est le betree propre ?
		  // assert(new_betree == NULL) ;
		  res = new_object ;
		  betree_table = table ;
#ifdef DEBUG_DUMP
		  TRACE2("UPDATE res=%x, betree_table=%x", res, betree_table) ;
#endif // DEBUG_DUMP
		}

	  new_object->load_disk((T_object *)load_sct) ;

#ifdef DEBUG_DUMP
	  TRACE2("node_type %d, expected %d", new_object->get_node_type(), node_type) ;
#endif // DEBUG_DUMP
	  assert(new_object->get_node_type() == node_type) ;

	  load_sct = (void *)((size_t) load_sct + incr) ;
	  EXIT_TRACE ; EXIT_TRACE ;
#ifdef DEBUG_DUMP
	  TRACE3("objet de la classe %d <%s> : on a avance de %d octets",
			 node_type,
			 get_metaclass(node_type)->get_class_name(),
			 incr) ;
#endif // DEBUG_DUMP
	}

  // "Reparation" des symboles lus
  TRACE2(">> DEBUT REPARATIONS SYMBOLES (first %x, last %x)",
		 first_read_symbol,
		 last_read_symbol) ;
  ENTER_TRACE ;

  T_list_link *cur_link = first_read_symbol ;

  while (cur_link != NULL)
	{
	  TRACE1("cur_link %x", cur_link) ;
	  T_list_link *next = (T_list_link*)cur_link->get_next() ;

	  // Recuperation du symbole lu
	  // Cast valide par construction
	  T_symbol *cur_read = (T_symbol *)cur_link->get_object() ;
	  TRACE1("cur_read = %x", cur_read) ;
	  ASSERT(cur_read->is_a_symbol() == TRUE) ;

	  size_t adr = (size_t)cur_read->get_value() ;
	  TRACE1("adr = %d", adr) ;
	  char *value = (char *)disk_string_find_string(adr) ;
	  int len = cur_read->get_len() ;

	  // Insertion du symbole dans la table des symboles s'il n'y est
	  // pas, ou recuperation du symbole correspondant le cas echeant
	  // A priori pas de probleme avec les keyword car ils sont
	  // persistants et predefinis donc le lookup_symbol ne creera pas
	  // de T_symbole, mais renverra le T_keyword
	  T_symbol *real_symbol = lookup_symbol(value, len) ;

	  TRACE5("cur_read %x \"%s\" <%d> adr %d -> symbole %x de la table %x",
			 cur_read,
			 value,
			 len,
			 cur_link->get_tmp2(),
			 cur_read) ;

	  // Correction de la table d'adressage
	  disk_object_set_address(cur_link->get_tmp2(), real_symbol) ;

	  // Liberation du T_list_link et du symbole
	  // On librere a la main car le destructeur n'existe pas (car on
	  // ne libere pas les symboles en principe !)
	  TRACE3("libe value %x symbole %x link %x",
			 value,
			 cur_read,
			 cur_link) ;
	  //	  delete [] cur_read->get_value() ;
	  // free(cur_read) ;
	  delete cur_link ;

	  // Au suivant !
	  cur_link = next ;
	}

  EXIT_TRACE ;
  TRACE2("<<  FIN  REPARATIONS SYMBOLES (first %x, last %)",
		 first_read_symbol,
		 last_read_symbol) ;

  // Boucle de secours : tous les symboles lus sont persistants, ainsi
  // que leurs valeurs
  fix_symbol_table() ;

  // On resoud les pointeurs
  TRACE1("ici betree_table %x", betree_table) ;

  if (betree_table != NULL)
	{
	  betree_table->set_solved(TRUE) ;
	}
  TRACE0("ici") ;

  if (res != NULL)
	{
	  res->solve_objects() ;
	}

  // Fin
  exit_disk_object() ;

  EXIT_TRACE ; EXIT_TRACE ;
  TRACE3("<-- load_object(%s, %d) -> %x", file_name, betree_file_type, res) ;
  return res ;
}

T_object *load_checksums(const char *file_name,
                               T_betree_file_type &betree_file_type,
                               T_io_diagnostic &io_diagnostic)
{
    T_component_checksums* ret = 0;
  TRACE2("--> load_checksums(%s, %d)", file_name, betree_file_type) ;
  ENTER_TRACE ; ENTER_TRACE ;

  if (get_no_exit_mode() == TRUE)
    {
      TRACE0("mise en place mode retour erreur") ;
      jmp_buf env ;			// Pour retour en cas d'erreur

      if (setjmp(env) != 0)
        {
          // On est en retour d'erreur ! */
          TRACE0("retour d'erreur dans load_object") ;
          io_diagnostic = IO_ERROR ;
          return NULL ;
        }

      set_exit_handler(&env) ;
    }

  io_diagnostic = IO_OK ;

  std::fstream lStream;
  lStream.open(file_name, ios_base::in);
  TiXmlDocument doc( file_name );
  if(doc.LoadFile())
  {
      //et on lit le checksum
      ret = new T_component_checksums();
      ret->readXML(doc.FirstChildElement());
  }


  EXIT_TRACE ; EXIT_TRACE ;
  TRACE3("<-- load_checksums(%s, %d) -> %x", file_name, betree_file_type, ret) ;
  return ret ;
}
