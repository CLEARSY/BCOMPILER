/******************************* CLEARSY **************************************
* Fichier : $Id: c_meta.cpp,v 2.0 2007-07-25 14:07:45 arequet Exp $
* (C) 2008 CLEARSY
*
* Description :		Compilateur B
*					Interface des metaclasses
*
* Compilation :		-DDEBUG_META pour avoir les traces de construction
*					des metaclasses
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
RCS_ID("$Id: c_meta.cpp,v 1.103 2007-07-25 14:07:45 arequet Exp $") ;

/* Includes systeme */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

/* Includes Composant Local */
#include "c_api.h"
#include "c_nodtyp.h"

// Aiguillage NODE_TYPE->METACLASS
// ATTENTION : l'index du tableau doit etre la derniere valeur systeme du
// type enumere T_node_type + 1, ie NODE_USER_DEFINED_20
static T_metaclass* metaclasses_sot[NODE_USER_DEFINED20] = { nullptr };
static size_t array_size_si = sizeof(metaclasses_sot) / sizeof(T_metaclass *) ;

static const char* field_type_name[] =
{
  /* 00 FIELD_INTEGER */
  "int",
  /* 01 FIELD_OBJECT */
  "T_object *",
  /* 02 FIELD_STRING */
  "char *",
  /* 03 FIELD_POINTER */
  "void *",
  /* 04 FIELD_FILLING */
  "filling",
  /* 05 FIELD_USER */
  "user field",
  /* 06 FIELD_RESET */
  "reset",
  /* 07 FIELD_UPGRADE */
  "upgrade",
  /* 08 FIELD_TIME_T */
  "time_t",
};


// Taille des champs sur le systeme courant (mode avant upgrade)
static size_t field_size_sit[] =
{
  /* 00 FIELD_INTEGER */
  sizeof(int),
  /* 01 FIELD_OBJECT */
  sizeof(T_object *),
  /* 02 FIELD_STRING */
  sizeof(char *),
  /* 03 FIELD_POINTER */
  sizeof(void *),
  /* 04 FIELD_FILLING */
  sizeof(void *),
  /* 05 FIELD_USER */
  sizeof(void *),
  /* 06 FIELD_RESET */
  sizeof(void *),
  /* 07 FIELD_UPGRADE */
  sizeof(void *),
  /* 08 FIELD_TIME_T */
  sizeof(time_t),
} ;

// Taille des champs sur le systeme courant (mode apres upgrade)
static size_t before_upgrade_field_size_sit[] =
{
  /* 00 FIELD_INTEGER */
  sizeof(int),
  /* 01 FIELD_OBJECT */
  sizeof(T_object *),
  /* 02 FIELD_STRING */
  sizeof(char *),
  /* 03 FIELD_POINTER */
  sizeof(void *),
  /* 04 FIELD_FILLING */
  sizeof(void *),
  /* 05 FIELD_USER */
  sizeof(void *),
  /* 06 FIELD_RESET */
  sizeof(void *),
  /* 07 FIELD_UPGRADE */
  0,
  /* 08 FIELD_TIME_T */
  sizeof(time_t),
} ;


#ifdef DEBUG_META
static const char *const debug_ref_type_name_sct[] =
{
  "REF_SHARED",
  "REF_PRIVATE",
  "REF_PRIVATE_LIST",
  "REF_NO_COPY",
} ;
#endif

static size_t internal_get_field_size(T_field_type field_type)
{
  return field_size_sit[field_type] ;
}

static size_t
internal_get_field_size_before_upgrade(T_field_type field_type)
{
  return before_upgrade_field_size_sit[field_type] ;
}

//
//  Fabrication d'une macro permettant de tester ais�ment la version
//  de GCC utilis�e pour compiler ce fichier.
//
#ifdef __GNUC__
#define GCC_VERSION ( __GNUC__ * 10000 \
                    + __GNUC_MINOR__ * 100 \
                    + __GNUC_PATCHLEVEL__)
#endif  // defined(__GNUC__)

// NODE_OBJECT
static void init_meta_object(void)
{
  // Classe T_object
  T_metaclass *cur = new T_metaclass("T_object", sizeof(T_object), NULL) ;
#if defined GCC_VERSION && GCC_VERSION >= 30200
  // GCC >= 3.2.0 has the virtual table as implicit first field
  cur->add_field("vtbl",				FIELD_FILLING) ;
#endif
  cur->add_field("magic", 				FIELD_INTEGER) ;
  cur->add_field("node_type", 			FIELD_INTEGER) ;
  cur->add_field("tag", 				FIELD_RESET) ;
  cur->add_field("rank", 				FIELD_INTEGER) ;
  cur->add_field("tmp",					FIELD_RESET) ; // was : FIELD_POINTER
  cur->add_field("tmp2",				FIELD_INTEGER) ;
  cur->add_field("usr1",				FIELD_USER) ;
  cur->add_field("usr2",				FIELD_USER) ;
#if ! (defined GCC_VERSION && GCC_VERSION >= 30200)
  // Here, the virtual table is in the last field
  cur->add_field("vtbl",				FIELD_FILLING) ;
#endif
  metaclasses_sot[NODE_OBJECT] = cur ;

  T_object::check_field_offsets();
}

static void init_meta_lexem(void)
{
  T_metaclass *cur = new T_metaclass("T_lexem",
									 sizeof(T_lexem),
									 metaclasses_sot[NODE_OBJECT]) ;

  cur->add_field("lex_type",				FIELD_INTEGER) ;
  cur->add_field("value",					FIELD_STRING) ;
  cur->add_field("value_len",				FIELD_INTEGER) ;
  cur->add_field("file_name",				FIELD_OBJECT) ;
  cur->add_field("file_line",				FIELD_INTEGER) ;
  cur->add_field("file_column",			FIELD_INTEGER) ;
  cur->add_field("next_lexem", 			FIELD_OBJECT) ;
  cur->add_field("prev_lexem", 			FIELD_OBJECT) ;
  cur->add_field("next_expanded_lexem", 	FIELD_OBJECT) ;
  cur->add_field("prev_expanded_lexem", 	FIELD_OBJECT) ;
  cur->add_field("original_lexem",		FIELD_OBJECT) ;
  cur->add_field("rewrite_rule_lexem",	  	FIELD_OBJECT) ;
  cur->add_field("next_line", 			FIELD_OBJECT) ;
  cur->add_field("prev_line", 			FIELD_OBJECT) ;
  cur->add_field("start_of_line",			FIELD_OBJECT) ;
  cur->add_field("expanded",				FIELD_INTEGER) ;
  cur->add_field("position",				FIELD_INTEGER) ;
  cur->add_field("next_list_lexem",		FIELD_OBJECT) ;
  cur->add_field("next_unsolved_comment",	FIELD_OBJECT) ;
  cur->add_field("prev_unsolved_comment",	FIELD_OBJECT) ;
  // On ne stocke pas les variables de classe
  metaclasses_sot[NODE_LEXEM] = cur ;
}

static void init_meta_item(void)
{
  T_metaclass *cur = new T_metaclass("T_item",
									 sizeof(T_item),
									 metaclasses_sot[NODE_OBJECT]) ;
  cur->add_field("betree",	 			FIELD_OBJECT) ;
  cur->add_field("father",	 			FIELD_OBJECT) ;
  cur->add_field("next", 				FIELD_OBJECT) ;
  cur->add_field("prev", 				FIELD_OBJECT) ;
  cur->add_field("links",				FIELD_INTEGER) ;
  cur->add_field("ref_lexem",			FIELD_OBJECT) ;
  cur->add_field("first_comment",		FIELD_OBJECT, REF_PRIVATE_LIST) ;
  cur->add_field("last_comment", 		FIELD_OBJECT, REF_NO_COPY) ;
  cur->add_field("first_pragma",		FIELD_OBJECT, REF_PRIVATE_LIST) ;
  cur->add_field("last_pragma", 		FIELD_OBJECT, REF_NO_COPY) ;
  // On ne stocke pas les variables de classe
  metaclasses_sot[NODE_ITEM] = cur ;
}

static void init_meta_betree(void)
{
  T_metaclass *cur = new T_metaclass("T_betree",
									 sizeof(T_betree),
									 metaclasses_sot[NODE_ITEM]) ;
  cur->add_field("root",		 		FIELD_OBJECT, REF_PRIVATE) ;
  cur->add_field("file_name",	 		FIELD_OBJECT) ;
  cur->add_field("path",		 		FIELD_OBJECT) ;
  cur->add_field("manager",	 		FIELD_OBJECT) ;
  cur->add_field("status",   			FIELD_INTEGER) ;
  cur->add_field("checksum", 			FIELD_OBJECT) ;
  cur->add_field("expanded_checksum",  	FIELD_OBJECT) ;
  cur->add_field("revision_ID",  		FIELD_STRING) ;
  cur->add_field("betree_name",   	FIELD_OBJECT) ;
  cur->add_field("first_lexem",	  	FIELD_OBJECT) ;
  cur->add_field("last_lexem",  		FIELD_OBJECT) ;
  cur->add_field("last_code_lexem",  	FIELD_OBJECT) ;
  cur->add_field("definitions_clause",FIELD_OBJECT) ;
  cur->add_field("first_definition",	FIELD_OBJECT) ;
  cur->add_field("last_definition", 	FIELD_OBJECT) ;
  cur->add_field("first_file_definition",	FIELD_OBJECT) ;
  cur->add_field("last_file_definition", 	FIELD_OBJECT) ;
  cur->add_field("next_betree",			FIELD_OBJECT) ;
  cur->add_field("prev_betree", 		FIELD_OBJECT) ;
  cur->add_field("integrity_check_done",FIELD_INTEGER) ;
  cur->add_field("seen_by",	 			FIELD_OBJECT) ;
  cur->add_field("included_by", 		FIELD_OBJECT) ;
  cur->add_field("imported_by", 		FIELD_OBJECT) ;
  cur->add_field("extended_by", 		FIELD_OBJECT) ;
  cur->add_field("used_by",	 			FIELD_OBJECT) ;
  cur->add_field("collision_analysed",	FIELD_INTEGER) ;

  metaclasses_sot[NODE_BETREE] = cur ;
}

static void init_meta_case_item(void)
{
  T_metaclass *cur = new T_metaclass("T_case_item",
									 sizeof(T_case_item),
									 metaclasses_sot[NODE_ITEM]) ;
  cur->add_field("first_literal_value",	FIELD_OBJECT, REF_PRIVATE_LIST) ;
  cur->add_field("last_literal_value",	FIELD_OBJECT, REF_NO_COPY) ;
  cur->add_field("first_body",    		FIELD_OBJECT, REF_PRIVATE_LIST) ;
  cur->add_field("last_body",    			FIELD_OBJECT, REF_NO_COPY) ;

  metaclasses_sot[NODE_CASE_ITEM] = cur ;
}

static void init_meta_comment(void)
{
  T_metaclass *cur = new T_metaclass("T_comment",
									 sizeof(T_comment),
									 metaclasses_sot[NODE_ITEM]) ;
  cur->add_field("comment",			FIELD_OBJECT) ;
  cur->add_field("keep_comment",		FIELD_INTEGER) ;
  cur->add_field("ref_machine",		FIELD_OBJECT) ;

  metaclasses_sot[NODE_COMMENT] = cur ;
}

static void init_meta_definition(void)
{
  T_metaclass *cur = new T_metaclass("T_definition",
									 sizeof(T_definition),
									 metaclasses_sot[NODE_ITEM]) ;
  cur->add_field("name",					FIELD_OBJECT) ;
  cur->add_field("nb_params",				FIELD_INTEGER) ;
  cur->add_field("first_param",			FIELD_OBJECT) ;
  cur->add_field("last_param",			FIELD_OBJECT) ;
  cur->add_field("first_rewrite_rule",   	FIELD_OBJECT) ;
  cur->add_field("last_rewrite_rule",	   	FIELD_OBJECT) ;
  cur->add_field("ref_scol_lexem",		   	FIELD_OBJECT) ;

  metaclasses_sot[NODE_DEFINITION] = cur ;
}

static void init_meta_file_definition(void)
{
  T_metaclass *cur = new T_metaclass("T_file_definition",
									 sizeof(T_file_definition),
									 metaclasses_sot[NODE_ITEM]) ;
  cur->add_field("name",				FIELD_OBJECT) ;
  cur->add_field("path",				FIELD_OBJECT) ;
  cur->add_field("checksum",			FIELD_OBJECT) ;

  metaclasses_sot[NODE_FILE_DEFINITION] = cur ;
}

static void init_meta_else(void)
{
  T_metaclass *cur = new T_metaclass("T_else",
									 sizeof(T_else),
									 metaclasses_sot[NODE_ITEM]) ;
  cur->add_field("cond",					FIELD_OBJECT, REF_PRIVATE) ;
  cur->add_field("first_body",			FIELD_OBJECT, REF_PRIVATE_LIST) ;
  cur->add_field("last_body",				FIELD_OBJECT, REF_NO_COPY) ;

  metaclasses_sot[NODE_ELSE] = cur ;
}

static void init_meta_expr(void)
{
  T_metaclass *cur = new T_metaclass("T_expr",
									 sizeof(T_expr),
									 metaclasses_sot[NODE_ITEM]) ;
  // Pas de champ propre
  cur->add_field("B_type",   		FIELD_OBJECT, REF_PRIVATE) ;
  cur->add_field("type_inferred",  	FIELD_INTEGER) ;  
  metaclasses_sot[NODE_EXPR] = cur ;

  T_expr::check_field_offsets();
}

static void init_meta_flag(void)
{
  T_metaclass *cur = new T_metaclass("T_flag",
									 sizeof(T_flag),
									 metaclasses_sot[NODE_ITEM]) ;
  // Pas de champ propre
  metaclasses_sot[NODE_FLAG] = cur ;
}

static void init_meta_list_link(void)
{
  T_metaclass *cur = new T_metaclass("T_list_link",
									 sizeof(T_list_link),
									 metaclasses_sot[NODE_ITEM]) ;
  cur->add_field("object",   		FIELD_OBJECT, REF_PRIVATE) ;
  cur->add_field("do_free",			FIELD_INTEGER) ;
  cur->add_field("list_link_type",  FIELD_INTEGER) ;

  metaclasses_sot[NODE_LIST_LINK] = cur ;
}

static void init_meta_machine(void)
{
  T_metaclass *cur = new T_metaclass("T_machine",
									 sizeof(T_machine),
									 metaclasses_sot[NODE_ITEM]) ;
  cur->add_field("machine_type", 			FIELD_INTEGER) ;
  cur->add_field("machine_name",   		FIELD_OBJECT) ;
  cur->add_field("abstraction_name", 		FIELD_OBJECT) ;
  cur->add_field("ref_abstraction", 		FIELD_OBJECT) ;
  cur->add_field("ref_specification",    	FIELD_OBJECT) ;
  cur->add_field("ref_refinement",    	FIELD_OBJECT) ;

  cur->add_field("machine_clause", 		FIELD_OBJECT, REF_PRIVATE) ;
  cur->add_field("refines_clause", 		FIELD_OBJECT, REF_PRIVATE) ;

  cur->add_field("first_param",   		FIELD_OBJECT, REF_PRIVATE_LIST) ;
  cur->add_field("last_param",   			FIELD_OBJECT, REF_NO_COPY) ;
  cur->add_field("formal_params_checksum",  FIELD_OBJECT, REF_PRIVATE) ;

  cur->add_field("first_global_pragma",   		FIELD_OBJECT, REF_PRIVATE_LIST) ;
  cur->add_field("last_global_pragma",FIELD_OBJECT, REF_NO_COPY) ;

  cur->add_field("constraints_clause",   	FIELD_OBJECT, REF_PRIVATE) ;
  cur->add_field("constraints_clause_checksum",   	FIELD_OBJECT, REF_PRIVATE) ;
  cur->add_field("constraints",			FIELD_OBJECT, REF_PRIVATE) ;

  cur->add_field("sees_clause",			FIELD_OBJECT, REF_PRIVATE) ;
  cur->add_field("sees_clause_checksum",			FIELD_OBJECT, REF_PRIVATE) ;
  cur->add_field("first_sees",			FIELD_OBJECT, REF_PRIVATE_LIST) ;
  cur->add_field("last_sees",				FIELD_OBJECT, REF_NO_COPY) ;

  cur->add_field("includes_clause",		FIELD_OBJECT, REF_PRIVATE) ;
  cur->add_field("includes_clause_checksum",		FIELD_OBJECT, REF_PRIVATE) ;
  cur->add_field("first_includes",		FIELD_OBJECT, REF_PRIVATE_LIST) ;
  cur->add_field("last_includes",			FIELD_OBJECT, REF_NO_COPY) ;

  cur->add_field("imports_clause",		FIELD_OBJECT, REF_PRIVATE) ;
  cur->add_field("imports_clause_checksum",		FIELD_OBJECT, REF_PRIVATE) ;
  cur->add_field("first_imports",			FIELD_OBJECT, REF_PRIVATE_LIST) ;
  cur->add_field("last_imports",			FIELD_OBJECT, REF_NO_COPY) ;

  cur->add_field("promotes_clause",		FIELD_OBJECT, REF_PRIVATE) ;
  cur->add_field("promotes_clause_checksum",		FIELD_OBJECT, REF_PRIVATE) ;
  cur->add_field("first_promotes",		FIELD_OBJECT, REF_PRIVATE_LIST) ;
  cur->add_field("last_promotes",			FIELD_OBJECT, REF_NO_COPY) ;

  cur->add_field("extends_clause",		FIELD_OBJECT, REF_PRIVATE) ;
  cur->add_field("extends_clause_checksum",		FIELD_OBJECT, REF_PRIVATE) ;
  cur->add_field("first_extends",			FIELD_OBJECT, REF_PRIVATE_LIST) ;
  cur->add_field("last_extends",			FIELD_OBJECT, REF_NO_COPY) ;

  cur->add_field("uses_clause",			FIELD_OBJECT, REF_PRIVATE) ;
  cur->add_field("uses_clause_checksum",			FIELD_OBJECT, REF_PRIVATE) ;
  cur->add_field("first_uses",			FIELD_OBJECT, REF_PRIVATE_LIST) ;
  cur->add_field("last_uses",				FIELD_OBJECT, REF_NO_COPY) ;

  cur->add_field("sets_clause",			FIELD_OBJECT, REF_PRIVATE) ;
  cur->add_field("sets_clause_checksum",			FIELD_OBJECT, REF_PRIVATE) ;
  cur->add_field("first_set",				FIELD_OBJECT, REF_PRIVATE_LIST) ;
  cur->add_field("last_set",				FIELD_OBJECT, REF_NO_COPY) ;

  cur->add_field("concrete_variables_clause",	FIELD_OBJECT, REF_PRIVATE) ;
  cur->add_field("concrete_variables_clause_checksum",	FIELD_OBJECT, REF_PRIVATE) ;
  cur->add_field("first_concrete_variable", 	FIELD_OBJECT, REF_PRIVATE_LIST) ;
  cur->add_field("last_concrete_variable",  	FIELD_OBJECT, REF_NO_COPY) ;

  cur->add_field("abstract_variables_clause",	FIELD_OBJECT, REF_PRIVATE) ;
  cur->add_field("abstract_variables_clause_checksum",	FIELD_OBJECT, REF_PRIVATE) ;
  cur->add_field("first_abstract_variable", 	FIELD_OBJECT, REF_PRIVATE_LIST) ;
  cur->add_field("last_abstract_variable",  	FIELD_OBJECT, REF_NO_COPY) ;

  cur->add_field("concrete_constants_clause",	FIELD_OBJECT, REF_PRIVATE) ;
  cur->add_field("concrete_constants_clause_checksum",	FIELD_OBJECT, REF_PRIVATE) ;
  cur->add_field("first_concrete_constant", 	FIELD_OBJECT, REF_PRIVATE_LIST) ;
  cur->add_field("last_concrete_constant",  	FIELD_OBJECT, REF_NO_COPY) ;

  cur->add_field("abstract_constants_clause",	FIELD_OBJECT, REF_PRIVATE) ;
  cur->add_field("abstract_constants_clause_checksum",	FIELD_OBJECT, REF_PRIVATE) ;
  cur->add_field("first_abstract_constant", 	FIELD_OBJECT, REF_PRIVATE_LIST) ;
  cur->add_field("last_abstract_constant",  	FIELD_OBJECT, REF_NO_COPY) ;

  cur->add_field("properties_clause",		FIELD_OBJECT, REF_PRIVATE) ;
  cur->add_field("properties_clause_checksum",		FIELD_OBJECT, REF_PRIVATE) ;
  cur->add_field("properties",			FIELD_OBJECT, REF_PRIVATE) ;

  cur->add_field("invariant_clause",		FIELD_OBJECT, REF_PRIVATE) ;
  cur->add_field("invariant_clause_checksum",		FIELD_OBJECT, REF_PRIVATE) ;
  cur->add_field("invariant",				FIELD_OBJECT, REF_PRIVATE) ;

  cur->add_field("variant_clause",		FIELD_OBJECT, REF_PRIVATE) ;
  cur->add_field("variant_clause_checksum",		FIELD_OBJECT, REF_PRIVATE) ;
  cur->add_field("variant",				FIELD_OBJECT, REF_PRIVATE) ;

  cur->add_field("assertions_clause",		FIELD_OBJECT, REF_PRIVATE) ;
  cur->add_field("assertions_clause_checksum",		FIELD_OBJECT, REF_PRIVATE) ;
  cur->add_field("first_assertion",			FIELD_OBJECT, REF_PRIVATE_LIST) ;
  cur->add_field("last_assertion",			FIELD_OBJECT, REF_NO_COPY) ;

  cur->add_field("initialisation_clause",	FIELD_OBJECT, REF_PRIVATE) ;
  cur->add_field("initialisation_clause_operation",	FIELD_OBJECT, REF_PRIVATE) ;
  cur->add_field("initialisation_clause_ident",	FIELD_OBJECT, REF_PRIVATE) ;
  cur->add_field("first_initialisation", 	FIELD_OBJECT, REF_PRIVATE_LIST) ;
  cur->add_field("last_initialisation",  	FIELD_OBJECT, REF_NO_COPY) ;

  cur->add_field("values_clause",			FIELD_OBJECT, REF_PRIVATE) ;
  cur->add_field("values_clause_checksum", 	FIELD_OBJECT, REF_PRIVATE) ;
  cur->add_field("first_value",				FIELD_OBJECT, REF_PRIVATE_LIST) ;
  cur->add_field("last_value",				FIELD_OBJECT, REF_NO_COPY) ;

  cur->add_field("operations_clause",		FIELD_OBJECT, REF_PRIVATE) ;
  cur->add_field("first_operation",			FIELD_OBJECT, REF_PRIVATE_LIST) ;
  cur->add_field("last_operation",			FIELD_OBJECT, REF_NO_COPY) ;

  cur->add_field("local_operations_clause",		FIELD_OBJECT, REF_PRIVATE) ;
  cur->add_field("first_local_operation",			FIELD_OBJECT, REF_PRIVATE_LIST) ;
  cur->add_field("last_local_operation",			FIELD_OBJECT, REF_NO_COPY) ;

  cur->add_field("end_machine",				FIELD_OBJECT, REF_PRIVATE) ;
  cur->add_field("first_end_comment",		FIELD_OBJECT, REF_PRIVATE_LIST) ;
  cur->add_field("last_end_comment",		FIELD_OBJECT, REF_NO_COPY) ;

  cur->add_field("first_op_list",		    FIELD_OBJECT, REF_PRIVATE_LIST) ;
  cur->add_field("last_op_list",			FIELD_OBJECT, REF_NO_COPY) ;

  cur->add_field("first_dollar_zero_ident",	FIELD_OBJECT, REF_PRIVATE_LIST) ;
  cur->add_field("last_dollar_zero_ident", 	FIELD_OBJECT, REF_NO_COPY) ;

  cur->add_field("context",					FIELD_OBJECT, REF_SHARED) ;
  cur->add_field("type_checked",		   	FIELD_INTEGER) ;
  cur->add_field("project_checked",		   	FIELD_INTEGER) ;
  cur->add_field("op_analysed",		   		FIELD_INTEGER) ;

  metaclasses_sot[NODE_MACHINE] = cur ;
}

static void init_meta_generic_operation(void)
{
  T_metaclass *cur = new T_metaclass("T_generic_op",
									 sizeof(T_generic_op),
									 metaclasses_sot[NODE_ITEM]) ;

  metaclasses_sot[NODE_GENERIC_OP] = cur ;
}

static void init_meta_operation(void)
{
  T_metaclass *cur = new T_metaclass("T_op",
									 sizeof(T_op),
									 metaclasses_sot[NODE_GENERIC_OP]) ;

  cur->add_field("name",   					FIELD_OBJECT, REF_PRIVATE) ;

  cur->add_field("precond_checksum", 	   	FIELD_OBJECT) ;
  cur->add_field("body_checksum", 		    FIELD_OBJECT) ;

  cur->add_field("first_ref_op",   		FIELD_OBJECT, REF_PRIVATE_LIST) ;
  cur->add_field("last_ref_op",    		FIELD_OBJECT, REF_NO_COPY) ;
  cur->add_field("first_in_param",   		FIELD_OBJECT, REF_PRIVATE_LIST) ;
  cur->add_field("last_in_param",    		FIELD_OBJECT, REF_NO_COPY) ;
  cur->add_field("first_out_param",  		FIELD_OBJECT, REF_PRIVATE_LIST) ;
  cur->add_field("last_out_param",   		FIELD_OBJECT, REF_NO_COPY) ;

  cur->add_field("first_body",				FIELD_OBJECT, REF_PRIVATE_LIST) ;
  cur->add_field("last_body",				FIELD_OBJECT, REF_NO_COPY) ;
  cur->add_field("type_checked",		   	FIELD_INTEGER) ;
  cur->add_field("B0_checked",		   	    FIELD_INTEGER) ;
  cur->add_field("B0_header_checked",		FIELD_INTEGER) ;
  cur->add_field("B0_body_checked",		   	FIELD_INTEGER) ;
  cur->add_field("is_a_local_op",		   	FIELD_INTEGER) ;
  cur->add_field("is_inlined", 		   	    FIELD_INTEGER) ;

  metaclasses_sot[NODE_OPERATION] = cur ;
}

static void init_meta_or(void)
{
  T_metaclass *cur = new T_metaclass("T_or",
									 sizeof(T_or),
									 metaclasses_sot[NODE_ITEM]) ;

  cur->add_field("first_body",			FIELD_OBJECT, REF_PRIVATE_LIST) ;
  cur->add_field("last_body",				FIELD_OBJECT, REF_NO_COPY) ;

  metaclasses_sot[NODE_OR] = cur ;
}

static void init_meta_predicate(void)
{
  T_metaclass *cur = new T_metaclass("T_predicate",
									 sizeof(T_predicate),
									 metaclasses_sot[NODE_ITEM]) ;
  // Pas de champ propre
  metaclasses_sot[NODE_PREDICATE] = cur ;
}

static void init_meta_substitution(void)
{
  T_metaclass *cur = new T_metaclass("T_substitution",
									 sizeof(T_substitution),
									 metaclasses_sot[NODE_ITEM]) ;


  cur->add_field("link_type",				FIELD_INTEGER) ;

  metaclasses_sot[NODE_SUBSTITUTION] = cur ;
}

static void init_meta_symbol(void)
{
  T_metaclass *cur = new T_metaclass("T_symbol",
									 sizeof(T_symbol),
									 metaclasses_sot[NODE_OBJECT]) ;


  cur->add_field("value",				FIELD_STRING) ;
  cur->add_field("len",				FIELD_INTEGER) ;

  // LV le 21/10/1999: Bug 2310
  //
  // Il ne faut pas sauvegarder le lien vers le successeur dans la
  // table des symboles, puisque le symbole sera reconstruit lors du
  // chargement depuis un fichier.
  cur->add_field("next_symb",		   	FIELD_RESET) ;

  metaclasses_sot[NODE_SYMBOL] = cur ;
}

static void init_meta_used_op(void)
{
  T_metaclass *cur = new T_metaclass("T_used_op",
									 sizeof(T_used_op),
									 metaclasses_sot[NODE_GENERIC_OP]) ;


  cur->add_field("real_op_name",   	FIELD_OBJECT) ;
  cur->add_field("instance_name",   	FIELD_OBJECT) ;
  cur->add_field("name", 			  	FIELD_OBJECT) ;
  cur->add_field("ref_op",   			FIELD_OBJECT) ;
  cur->add_field("type_checked",   		FIELD_INTEGER) ;

  metaclasses_sot[NODE_USED_OP] = cur ;
}

static void init_meta_used_machine(void)
{
  T_metaclass *cur = new T_metaclass("T_used_machine",
									 sizeof(T_used_machine),
									 metaclasses_sot[NODE_ITEM]) ;

  cur->add_field("component_name",   	FIELD_OBJECT) ;
  cur->add_field("instance_name",   	FIELD_OBJECT) ;
  cur->add_field("name", 			  	FIELD_OBJECT) ;
  cur->add_field("pathname", 			FIELD_OBJECT) ;
  cur->add_field("ref_machine",   		FIELD_OBJECT) ;
  cur->add_field("first_use_param",  	FIELD_OBJECT, REF_PRIVATE_LIST) ;
  cur->add_field("last_use_param",   	FIELD_OBJECT, REF_NO_COPY) ;
  cur->add_field("use_tpe",		   	FIELD_INTEGER) ;

  metaclasses_sot[NODE_USED_MACHINE] = cur ;
}

static void init_meta_valuation(void)
{
  T_metaclass *cur = new T_metaclass("T_valuation",
									 sizeof(T_valuation),
									 metaclasses_sot[NODE_ITEM]) ;


  cur->add_field("ident",   	FIELD_OBJECT, REF_PRIVATE) ;
  cur->add_field("value",   	FIELD_OBJECT, REF_PRIVATE) ;


  metaclasses_sot[NODE_VALUATION] = cur ;
}

static void init_meta_when(void)
{
  T_metaclass *cur = new T_metaclass("T_when",
									 sizeof(T_when),
									 metaclasses_sot[NODE_ITEM]) ;


  cur->add_field("cond",   		FIELD_OBJECT, REF_PRIVATE) ;
  cur->add_field("first_body",   	FIELD_OBJECT, REF_PRIVATE_LIST) ;
  cur->add_field("last_body",   	FIELD_OBJECT, REF_NO_COPY) ;

  metaclasses_sot[NODE_WHEN] = cur ;
}

static void init_meta_ident(void)
{
  T_metaclass *cur = new T_metaclass("T_ident",
									 sizeof(T_ident),
									 metaclasses_sot[NODE_EXPR]) ;


  cur->add_field("name",   					FIELD_OBJECT) ;
  cur->add_field("prefix",   				FIELD_OBJECT) ;
  cur->add_field("suffix_number",   	    FIELD_INTEGER) ;
  cur->add_field("ident_type",   			FIELD_INTEGER) ;
  cur->add_field("def",   					FIELD_OBJECT) ;
  cur->add_field("ref_machine",  			FIELD_OBJECT) ;
  cur->add_field("ref_op",   				FIELD_OBJECT) ;
  cur->add_field("ref_glued",   	   		FIELD_OBJECT) ;
  cur->add_field("ref_inherited",  	   		FIELD_OBJECT) ;
  cur->add_field("ref_glued_ring", 	   		FIELD_OBJECT) ;
  cur->add_field("implemented_by",          FIELD_OBJECT, REF_NO_COPY);
  cur->add_field("homonym_in_abstraction",  FIELD_OBJECT, REF_NO_COPY);
  cur->add_field("homonym_in_required_mach",FIELD_OBJECT, REF_NO_COPY);
  cur->add_field("explicitly_declared",     FIELD_INTEGER, REF_NO_COPY);
  cur->add_field("first_value",  			FIELD_OBJECT, REF_PRIVATE_LIST) ;
  cur->add_field("last_value",   			FIELD_OBJECT, REF_NO_COPY) ;
  cur->add_field("next_ident",  			FIELD_OBJECT) ;
  cur->add_field("prev_ident",   			FIELD_OBJECT) ;
  cur->add_field("next_unsolved_ident",   	FIELD_OBJECT, REF_NO_COPY) ;
  cur->add_field("typing_location",   		FIELD_OBJECT) ;
  cur->add_field("original_typing_location",FIELD_OBJECT) ;
  cur->add_field("B0_type",   				FIELD_OBJECT) ;
  cur->add_field("typing_pred_or_subst",    FIELD_OBJECT, REF_NO_COPY) ;
  cur->add_field("access_entity_location",	FIELD_INTEGER) ;
  cur->add_field("inherited",				FIELD_INTEGER) ;
  cur->add_field("B0_type_checked",			FIELD_INTEGER) ;
  cur->add_field("BOM_lexem_type",          FIELD_OBJECT);
  cur->add_field("BOM_type",                FIELD_OBJECT);
  cur->add_field("BOM_lexem_init_value",    FIELD_OBJECT);
  cur->add_field("BOM_init_value",          FIELD_OBJECT);
  metaclasses_sot[NODE_IDENT] = cur ;
}

static void init_meta_literal_integer(void)
{
  T_metaclass *cur = new T_metaclass("T_literal_integer",
									 sizeof(T_literal_integer),
									 metaclasses_sot[NODE_EXPR]) ;

  cur->add_field("value",   	FIELD_OBJECT, REF_PRIVATE) ;

  metaclasses_sot[NODE_LITERAL_INTEGER] = cur ;
}

static void init_meta_binary_op(void)
{
  T_metaclass *cur = new T_metaclass("T_binary_op",
									 sizeof(T_binary_op),
									 metaclasses_sot[NODE_EXPR]) ;

  cur->add_field("op1",   			FIELD_OBJECT, REF_PRIVATE) ;
  cur->add_field("op2",   			FIELD_OBJECT, REF_PRIVATE) ;
  // firs_operand private_list : bof, car on clone op1/op2 deux fois
  cur->add_field("first_operand",   	FIELD_OBJECT, REF_PRIVATE_LIST) ;
  cur->add_field("last_operand",   	FIELD_OBJECT, REF_NO_COPY) ;
  cur->add_field("nb_operands",   	FIELD_INTEGER) ;
  cur->add_field("oper",   			FIELD_INTEGER) ;

  metaclasses_sot[NODE_BINARY_OP] = cur ;
}

static void init_meta_expr_with_parenthesis(void)
{
  T_metaclass *cur = new T_metaclass("T_expr_with_parenthesis",
									 sizeof(T_expr_with_parenthesis),
									 metaclasses_sot[NODE_EXPR]) ;

  cur->add_field("expr",   	FIELD_OBJECT, REF_PRIVATE) ;

  metaclasses_sot[NODE_EXPR_WITH_PARENTHESIS] = cur ;
}

static void init_meta_unary_op(void)
{
  T_metaclass *cur = new T_metaclass("T_unary_op",
									 sizeof(T_unary_op),
									 metaclasses_sot[NODE_EXPR]) ;

  cur->add_field("operand",  	FIELD_OBJECT, REF_PRIVATE) ;
  cur->add_field("oper",   	FIELD_INTEGER) ;

  metaclasses_sot[NODE_UNARY_OP] = cur ;
}

static void init_meta_op_result(void)
{
  T_metaclass *cur = new T_metaclass("T_op_result",
									 sizeof(T_op_result),
									 metaclasses_sot[NODE_EXPR]) ;

  cur->add_field("op_name",  			FIELD_OBJECT) ;
  cur->add_field("ref_builtin", 	 	FIELD_OBJECT) ;
  cur->add_field("first_in_param",  	FIELD_OBJECT, REF_PRIVATE_LIST) ;
  cur->add_field("last_in_param",  	FIELD_OBJECT, REF_NO_COPY) ;
  cur->add_field("nb_in_params",  	FIELD_INTEGER) ;
  cur->add_field("ref_op",  			FIELD_OBJECT) ;
  cur->add_field("next_op_result",  	FIELD_OBJECT) ;
  cur->add_field("ref_array_item",  	FIELD_OBJECT) ;

  metaclasses_sot[NODE_OP_RESULT] = cur ;
}

static void init_meta_array_item(void)
{
  T_metaclass *cur = new T_metaclass("T_array_item",
									 sizeof(T_array_item),
									 metaclasses_sot[NODE_EXPR]) ;

  cur->add_field("ref_array",    	FIELD_OBJECT) ;
  cur->add_field("separator",  	    FIELD_INTEGER) ;
  cur->add_field("first_index",  	FIELD_OBJECT, REF_PRIVATE_LIST) ;
  cur->add_field("last_index",  	FIELD_OBJECT, REF_NO_COPY) ;
  cur->add_field("nb_indexes",  	FIELD_INTEGER) ;

  metaclasses_sot[NODE_ARRAY_ITEM] = cur ;
}

static void init_meta_converse(void)
{
  T_metaclass *cur = new T_metaclass("T_converse",
									 sizeof(T_converse),
									 metaclasses_sot[NODE_EXPR]) ;

  cur->add_field("expr",   	FIELD_OBJECT, REF_PRIVATE) ;

  metaclasses_sot[NODE_CONVERSE] = cur ;
}

static void init_meta_literal_string(void)
{
  T_metaclass *cur = new T_metaclass("T_literal_string",
									 sizeof(T_literal_string),
									 metaclasses_sot[NODE_EXPR]) ;

  cur->add_field("value",   	FIELD_OBJECT) ;

  metaclasses_sot[NODE_LITERAL_STRING] = cur ;
}

static void init_meta_literal_real(void)
{
  T_metaclass *cur = new T_metaclass("T_literal_real",
                                                                         sizeof(T_literal_real),
                                                                         metaclasses_sot[NODE_EXPR]) ;

  cur->add_field("value",   	FIELD_OBJECT) ;

  metaclasses_sot[NODE_LITERAL_REAL] = cur ;
}

static void init_meta_comprehensive_set(void)
{
  T_metaclass *cur = new T_metaclass("T_comprehensive_set",
									 sizeof(T_comprehensive_set),
									 metaclasses_sot[NODE_EXPR]) ;

  cur->add_field("first_ident",  	FIELD_OBJECT, REF_PRIVATE_LIST) ;
  cur->add_field("last_ident",   	FIELD_OBJECT, REF_NO_COPY) ;
  cur->add_field("predicate",   	FIELD_OBJECT, REF_PRIVATE) ;

  metaclasses_sot[NODE_COMPREHENSIVE_SET] = cur ;
}

static void init_meta_image(void)
{
  T_metaclass *cur = new T_metaclass("T_image",
									 sizeof(T_image),
									 metaclasses_sot[NODE_EXPR]) ;

  cur->add_field("relation",  	FIELD_OBJECT, REF_PRIVATE) ;
  cur->add_field("expr",		   	FIELD_OBJECT, REF_PRIVATE) ;

  metaclasses_sot[NODE_IMAGE] = cur ;
}

static void init_meta_empty_seq(void)
{
  T_metaclass *cur = new T_metaclass("T_empty_seq",
									 sizeof(T_empty_seq),
									 metaclasses_sot[NODE_EXPR]) ;
  // Pas de champ propre
  metaclasses_sot[NODE_EMPTY_SEQ] = cur ;
}

static void init_meta_empty_set(void)
{
  T_metaclass *cur = new T_metaclass("T_empty_set",
									 sizeof(T_empty_set),
									 metaclasses_sot[NODE_EXPR]) ;
  // Pas de champ propre
  metaclasses_sot[NODE_EMPTY_SET] = cur ;
}

static void init_meta_sigma(void)
{
  T_metaclass *cur = new T_metaclass("T_sigma",
									 sizeof(T_sigma),
									 metaclasses_sot[NODE_EXPR]) ;

  cur->add_field("first_variable",  	FIELD_OBJECT, REF_PRIVATE_LIST) ;
  cur->add_field("last_variable",  	FIELD_OBJECT, REF_NO_COPY) ;
  cur->add_field("predicate",		  	FIELD_OBJECT, REF_PRIVATE) ;
  cur->add_field("expr",		   		FIELD_OBJECT, REF_PRIVATE) ;

  metaclasses_sot[NODE_SIGMA] = cur ;
}


static void init_meta_pi(void)
{
  T_metaclass *cur = new T_metaclass("T_pi",
									 sizeof(T_pi),
									 metaclasses_sot[NODE_EXPR]) ;

  cur->add_field("first_variable",  	FIELD_OBJECT, REF_PRIVATE_LIST) ;
  cur->add_field("last_variable",  	FIELD_OBJECT, REF_NO_COPY) ;
  cur->add_field("predicate",		  	FIELD_OBJECT, REF_PRIVATE) ;
  cur->add_field("expr",		   		FIELD_OBJECT, REF_PRIVATE) ;

  metaclasses_sot[NODE_PI] = cur ;
}

static void init_meta_quantified_union(void)
{
  T_metaclass *cur = new T_metaclass("T_quantified_union",
									 sizeof(T_quantified_union),
									 metaclasses_sot[NODE_EXPR]) ;

  cur->add_field("first_variable",  	FIELD_OBJECT, REF_PRIVATE_LIST) ;
  cur->add_field("last_variable",  	FIELD_OBJECT, REF_NO_COPY) ;
  cur->add_field("predicate",		  	FIELD_OBJECT, REF_PRIVATE) ;
  cur->add_field("expr",		   		FIELD_OBJECT, REF_PRIVATE) ;

  metaclasses_sot[NODE_QUANTIFIED_UNION] = cur ;
}

static void init_meta_quantified_intersection(void)
{
  T_metaclass *cur = new T_metaclass("T_quantified_intersection",
									 sizeof(T_quantified_intersection),
									 metaclasses_sot[NODE_EXPR]) ;

  cur->add_field("first_variable",  	FIELD_OBJECT, REF_PRIVATE_LIST) ;
  cur->add_field("last_variable",  	FIELD_OBJECT, REF_NO_COPY) ;
  cur->add_field("predicate",		  	FIELD_OBJECT, REF_PRIVATE) ;
  cur->add_field("expr",		   		FIELD_OBJECT, REF_PRIVATE) ;

  metaclasses_sot[NODE_QUANTIFIED_INTERSECTION] = cur ;
}

static void init_meta_literal_boolean(void)
{
  T_metaclass *cur = new T_metaclass("T_literal_boolean",
									 sizeof(T_literal_boolean),
									 metaclasses_sot[NODE_EXPR]) ;

  cur->add_field("value",   	FIELD_INTEGER) ;

  metaclasses_sot[NODE_LITERAL_BOOLEAN] = cur ;

  T_literal_boolean::check_field_offsets();
}

static void init_meta_relation(void)
{
  T_metaclass *cur = new T_metaclass("T_relation",
									 sizeof(T_relation),
									 metaclasses_sot[NODE_EXPR]) ;

  cur->add_field("relation",   	FIELD_INTEGER) ;
  cur->add_field("src_set",  	FIELD_OBJECT, REF_PRIVATE) ;
  cur->add_field("dst_set",  	FIELD_OBJECT, REF_PRIVATE) ;
  cur->add_field("first_set", FIELD_OBJECT, REF_PRIVATE_LIST) ;
  cur->add_field("last_set",  FIELD_OBJECT, REF_NO_COPY) ;
  cur->add_field("nb_sets",   FIELD_INTEGER) ;

  metaclasses_sot[NODE_RELATION] = cur ;
}

static void init_meta_renamed_ident(void)
{
  T_metaclass *cur = new T_metaclass("T_renamed_ident",
									 sizeof(T_renamed_ident),
									 metaclasses_sot[NODE_IDENT]) ;


  cur->add_field("instance_name",    	FIELD_OBJECT) ;
  cur->add_field("component_name",   	FIELD_OBJECT) ;

  metaclasses_sot[NODE_RENAMED_IDENT] = cur ;
}

static void init_meta_literal_enumerated_value(void)
{
  T_metaclass *cur = new T_metaclass("T_literal_enumerated_value",
									 sizeof(T_literal_enumerated_value),
									 metaclasses_sot[NODE_IDENT]) ;

  cur->add_field("value",   	FIELD_INTEGER) ;

  metaclasses_sot[NODE_LITERAL_ENUMERATED_VALUE] = cur ;
}

static void init_meta_extensive_seq(void)
{
  T_metaclass *cur = new T_metaclass("T_extensive_seq",
									 sizeof(T_extensive_seq),
									 metaclasses_sot[NODE_EXPR]) ;

  cur->add_field("first_item",   	FIELD_OBJECT, REF_PRIVATE_LIST) ;
  cur->add_field("last_item",   	FIELD_OBJECT, REF_NO_COPY) ;

  metaclasses_sot[NODE_EXTENSIVE_SEQ] = cur ;
}

static void init_meta_generalised_union(void)
{
  T_metaclass *cur = new T_metaclass("T_generalised_union",
									 sizeof(T_generalised_union),
									 metaclasses_sot[NODE_EXPR]) ;

  cur->add_field("first_expr",   	FIELD_OBJECT, REF_PRIVATE_LIST) ;
  cur->add_field("last_expr",   	FIELD_OBJECT, REF_NO_COPY) ;

  metaclasses_sot[NODE_GENERALISED_UNION] = cur ;
}

static void init_meta_generalised_intersection(void)
{
  T_metaclass *cur = new T_metaclass("T_generalised_intersection",
									 sizeof(T_generalised_intersection),
									 metaclasses_sot[NODE_EXPR]) ;

  cur->add_field("first_expr",   	FIELD_OBJECT, REF_PRIVATE_LIST) ;
  cur->add_field("last_expr",   	FIELD_OBJECT, REF_NO_COPY) ;

  metaclasses_sot[NODE_GENERALISED_INTERSECTION] = cur ;
}

static void init_meta_extensive_set(void)
{
  T_metaclass *cur = new T_metaclass("T_extensive_set",
									 sizeof(T_extensive_set),
									 metaclasses_sot[NODE_EXPR]) ;

  cur->add_field("first_item",   	FIELD_OBJECT, REF_PRIVATE_LIST) ;
  cur->add_field("last_item",   	FIELD_OBJECT, REF_NO_COPY) ;

  metaclasses_sot[NODE_EXTENSIVE_SET] = cur ;
}

static void init_meta_lambda_expr(void)
{
  T_metaclass *cur = new T_metaclass("T_lambda_expr",
									 sizeof(T_lambda_expr),
									 metaclasses_sot[NODE_EXPR]) ;

  cur->add_field("first_variable",   	FIELD_OBJECT, REF_PRIVATE_LIST) ;
  cur->add_field("last_variable",   	FIELD_OBJECT, REF_NO_COPY) ;
  cur->add_field("predicate",   		FIELD_OBJECT, REF_PRIVATE) ;
  cur->add_field("expr",   			FIELD_OBJECT, REF_PRIVATE) ;


  metaclasses_sot[NODE_LAMBDA_EXPR] = cur ;
}

static void init_meta_binary_predicate(void)
{
  T_metaclass *cur = new T_metaclass("T_binary_predicate",
									 sizeof(T_binary_predicate),
									 metaclasses_sot[NODE_PREDICATE]) ;

  cur->add_field("pred1",   		FIELD_OBJECT, REF_PRIVATE) ;
  cur->add_field("pred2",   		FIELD_OBJECT, REF_PRIVATE) ;
  cur->add_field("predicate_type",FIELD_INTEGER) ;

  metaclasses_sot[NODE_BINARY_PREDICATE] = cur ;
}

static void init_meta_typing_predicate(void)
{
  T_metaclass *cur = new T_metaclass("T_typing_predicate",
									 sizeof(T_typing_predicate),
									 metaclasses_sot[NODE_PREDICATE]) ;

  cur->add_field("first_ident",   		FIELD_OBJECT, REF_PRIVATE_LIST) ;
  cur->add_field("last_ident",   			FIELD_OBJECT, REF_NO_COPY) ;
  cur->add_field("type",   				FIELD_OBJECT, REF_PRIVATE) ;
  cur->add_field("separator",	FIELD_INTEGER) ;
  cur->add_field("typing_predicate_type",	FIELD_INTEGER) ;

  metaclasses_sot[NODE_TYPING_PREDICATE] = cur ;
}

static void init_meta_predicate_with_parenthesis(void)
{
  T_metaclass *cur = new T_metaclass("T_predicate_with_parenthesis",
									 sizeof(T_predicate_with_parenthesis),
									 metaclasses_sot[NODE_PREDICATE]) ;

  cur->add_field("predicate",   		FIELD_OBJECT, REF_PRIVATE) ;

  metaclasses_sot[NODE_PREDICATE_WITH_PARENTHESIS] = cur ;
}

static void init_meta_not_predicate(void)
{
  T_metaclass *cur = new T_metaclass("T_not_predicate",
									 sizeof(T_not_predicate),
									 metaclasses_sot[NODE_PREDICATE]) ;

  cur->add_field("predicate",   		FIELD_OBJECT, REF_PRIVATE) ;

  metaclasses_sot[NODE_NOT_PREDICATE] = cur ;
}

static void init_meta_expr_predicate(void)
{
  T_metaclass *cur = new T_metaclass("T_expr_predicate",
									 sizeof(T_expr_predicate),
									 metaclasses_sot[NODE_PREDICATE]) ;

  cur->add_field("expr1",   		FIELD_OBJECT, REF_PRIVATE) ;
  cur->add_field("expr2",   		FIELD_OBJECT, REF_PRIVATE) ;
  cur->add_field("predicate_type",FIELD_INTEGER) ;

  metaclasses_sot[NODE_EXPR_PREDICATE] = cur ;
}

static void init_meta_universal_predicate(void)
{
  T_metaclass *cur = new T_metaclass("T_universal_predicate",
									 sizeof(T_universal_predicate),
									 metaclasses_sot[NODE_PREDICATE]) ;

  cur->add_field("first_variable",   	FIELD_OBJECT, REF_PRIVATE_LIST) ;
  cur->add_field("last_variable",    	FIELD_OBJECT, REF_NO_COPY) ;
  cur->add_field("predicate",			FIELD_OBJECT, REF_PRIVATE) ;

  metaclasses_sot[NODE_UNIVERSAL_PREDICATE] = cur ;
}

static void init_meta_existential_predicate(void)
{
  T_metaclass *cur = new T_metaclass("T_existential_predicate",
									 sizeof(T_existential_predicate),
									 metaclasses_sot[NODE_PREDICATE]) ;

  cur->add_field("first_variable",   	FIELD_OBJECT, REF_PRIVATE_LIST) ;
  cur->add_field("last_variable",    	FIELD_OBJECT, REF_NO_COPY) ;
  cur->add_field("predicate",			FIELD_OBJECT, REF_PRIVATE) ;

  metaclasses_sot[NODE_EXISTENTIAL_PREDICATE] = cur ;
}

static void init_meta_skip(void)
{
  T_metaclass *cur = new T_metaclass("T_skip",
									 sizeof(T_skip),
									 metaclasses_sot[NODE_SUBSTITUTION]) ;
  // Pas de champ propre
  metaclasses_sot[NODE_SKIP] = cur ;
}

static void init_meta_begin(void)
{
  T_metaclass *cur = new T_metaclass("T_begin",
									 sizeof(T_begin),
									 metaclasses_sot[NODE_SUBSTITUTION]) ;

  cur->add_field("first_body",   	FIELD_OBJECT, REF_PRIVATE_LIST) ;
  cur->add_field("last_body",    	FIELD_OBJECT, REF_NO_COPY) ;
  cur->add_field("end_begin",    	FIELD_OBJECT, REF_PRIVATE) ;

  metaclasses_sot[NODE_BEGIN] = cur ;
}

static void init_meta_choice(void)
{
  T_metaclass *cur = new T_metaclass("T_choice",
									 sizeof(T_choice),
									 metaclasses_sot[NODE_SUBSTITUTION]) ;

  cur->add_field("first_body",   	FIELD_OBJECT, REF_PRIVATE_LIST) ;
  cur->add_field("last_body",    	FIELD_OBJECT, REF_NO_COPY) ;
  cur->add_field("first_or",   	FIELD_OBJECT, REF_PRIVATE_LIST) ;
  cur->add_field("last_or",    	FIELD_OBJECT, REF_NO_COPY) ;
  cur->add_field("end_begin",    	FIELD_OBJECT, REF_PRIVATE) ;

  metaclasses_sot[NODE_CHOICE] = cur ;
}

static void init_meta_precond(void)
{
  T_metaclass *cur = new T_metaclass("T_precond",
									 sizeof(T_precond),
									 metaclasses_sot[NODE_SUBSTITUTION]) ;

  cur->add_field("predicate",   	FIELD_OBJECT, REF_PRIVATE) ;
  cur->add_field("then_keyword",   FIELD_OBJECT, REF_PRIVATE) ;
  cur->add_field("first_body",   	FIELD_OBJECT, REF_PRIVATE_LIST) ;
  cur->add_field("last_body",    	FIELD_OBJECT, REF_NO_COPY) ;
  cur->add_field("end_precond",   FIELD_OBJECT, REF_PRIVATE) ;

  metaclasses_sot[NODE_PRECOND] = cur ;
}

static void init_meta_assert(void)
{
  T_metaclass *cur = new T_metaclass("T_assert",
									 sizeof(T_assert),
									 metaclasses_sot[NODE_SUBSTITUTION]) ;

  cur->add_field("predicate",   	FIELD_OBJECT, REF_PRIVATE) ;
  cur->add_field("then",	 	  	FIELD_OBJECT) ;
  cur->add_field("first_body",   	FIELD_OBJECT, REF_PRIVATE_LIST) ;
  cur->add_field("last_body",    	FIELD_OBJECT, REF_NO_COPY) ;
  cur->add_field("end_assert",    FIELD_OBJECT, REF_PRIVATE) ;

  metaclasses_sot[NODE_ASSERT] = cur ;
}

static void init_meta_label(void)
{
  T_metaclass *cur = new T_metaclass("T_label",
                                                                         sizeof(T_label),
                                                                         metaclasses_sot[NODE_SUBSTITUTION]) ;

  cur->add_field("expression",   	FIELD_OBJECT, REF_PRIVATE) ;
  cur->add_field("then",	 	  	FIELD_OBJECT) ;
  cur->add_field("first_body",   	FIELD_OBJECT, REF_PRIVATE_LIST) ;
  cur->add_field("last_body",    	FIELD_OBJECT, REF_NO_COPY) ;
  cur->add_field("end_assert",    FIELD_OBJECT, REF_PRIVATE) ;

  metaclasses_sot[NODE_LABEL] = cur ;
}

static void init_meta_jumpif(void)
{
  T_metaclass *cur = new T_metaclass("T_jumpif",
                                                                         sizeof(T_jumpif),
                                                                         metaclasses_sot[NODE_SUBSTITUTION]) ;

  cur->add_field("cond",   	FIELD_OBJECT, REF_PRIVATE) ;
  cur->add_field("to",	 	  	FIELD_OBJECT) ;
  cur->add_field("label",   	FIELD_OBJECT, REF_PRIVATE) ;
  cur->add_field("end_jumpif",    FIELD_OBJECT, REF_PRIVATE) ;

  metaclasses_sot[NODE_JUMPIF] = cur ;
}

static void init_meta_witness(void)
{
  T_metaclass *cur = new T_metaclass("T_witness",
									 sizeof(T_witness),
									 metaclasses_sot[NODE_SUBSTITUTION]) ;

  cur->add_field("predicate",   	FIELD_OBJECT, REF_PRIVATE) ;
  cur->add_field("then",	 	  	FIELD_OBJECT) ;
  cur->add_field("first_body",   	FIELD_OBJECT, REF_PRIVATE_LIST) ;
  cur->add_field("last_body",    	FIELD_OBJECT, REF_NO_COPY) ;
  cur->add_field("end_witness",    FIELD_OBJECT, REF_PRIVATE) ;

  metaclasses_sot[NODE_WITNESS] = cur ;
}

static void init_meta_if(void)
{
  T_metaclass *cur = new T_metaclass("T_if",
									 sizeof(T_if),
									 metaclasses_sot[NODE_SUBSTITUTION]) ;

  cur->add_field("cond",   			FIELD_OBJECT, REF_PRIVATE) ;
//  cur->add_field("label",   			FIELD_OBJECT, REF_PRIVATE) ;
  cur->add_field("first_then_body",  	FIELD_OBJECT, REF_PRIVATE_LIST) ;
  cur->add_field("last_then_body",   	FIELD_OBJECT, REF_NO_COPY) ;
  cur->add_field("first_else",   		FIELD_OBJECT, REF_PRIVATE_LIST) ;
  cur->add_field("last_else",    		FIELD_OBJECT, REF_NO_COPY) ;
  cur->add_field("end_if",	    	FIELD_OBJECT, REF_PRIVATE) ;

  metaclasses_sot[NODE_IF] = cur ;
}

static void init_meta_select(void)
{
  T_metaclass *cur = new T_metaclass("T_select",
									 sizeof(T_select),
									 metaclasses_sot[NODE_SUBSTITUTION]) ;

  cur->add_field("cond",   			FIELD_OBJECT, REF_PRIVATE) ;
  cur->add_field("first_then_body",  	FIELD_OBJECT, REF_PRIVATE_LIST) ;
  cur->add_field("last_then_body",   	FIELD_OBJECT, REF_NO_COPY) ;
  cur->add_field("first_when",   		FIELD_OBJECT, REF_PRIVATE_LIST) ;
  cur->add_field("last_when",    		FIELD_OBJECT, REF_NO_COPY) ;
  cur->add_field("end_select",       	FIELD_OBJECT, REF_PRIVATE) ;

  metaclasses_sot[NODE_SELECT] = cur ;
}

static void init_meta_case(void)
{
  T_metaclass *cur = new T_metaclass("T_case",
									 sizeof(T_case),
									 metaclasses_sot[NODE_SUBSTITUTION]) ;

  cur->add_field("case_select",  		FIELD_OBJECT, REF_PRIVATE) ;
  cur->add_field("first_case_item",  	FIELD_OBJECT, REF_PRIVATE_LIST) ;
  cur->add_field("last_case_item",   	FIELD_OBJECT, REF_NO_COPY) ;
  cur->add_field("end_either",       	FIELD_OBJECT, REF_PRIVATE) ;
  cur->add_field("end_case",       	FIELD_OBJECT, REF_PRIVATE) ;

  metaclasses_sot[NODE_CASE] = cur ;
}

static void init_meta_let(void)
{
  T_metaclass *cur = new T_metaclass("T_let",
									 sizeof(T_let),
									 metaclasses_sot[NODE_SUBSTITUTION]) ;

  cur->add_field("first_ident",  		FIELD_OBJECT, REF_PRIVATE_LIST) ;
  cur->add_field("last_ident",   		FIELD_OBJECT, REF_NO_COPY) ;
  cur->add_field("first_valuation",  	FIELD_OBJECT, REF_PRIVATE_LIST) ;
  cur->add_field("last_valuation",   	FIELD_OBJECT, REF_NO_COPY) ;
  cur->add_field("first_body",  		FIELD_OBJECT, REF_PRIVATE_LIST) ;
  cur->add_field("last_body", 	  	FIELD_OBJECT, REF_NO_COPY) ;
  cur->add_field("end_let",       	FIELD_OBJECT, REF_PRIVATE) ;

  metaclasses_sot[NODE_LET] = cur ;
}

static void init_meta_var(void)
{
  T_metaclass *cur = new T_metaclass("T_var",
									 sizeof(T_var),
									 metaclasses_sot[NODE_SUBSTITUTION]) ;

  cur->add_field("first_ident",   	FIELD_OBJECT, REF_PRIVATE_LIST) ;
  cur->add_field("last_ident",    	FIELD_OBJECT, REF_NO_COPY) ;
  cur->add_field("first_body",   		FIELD_OBJECT, REF_PRIVATE_LIST) ;
  cur->add_field("last_body",    		FIELD_OBJECT, REF_NO_COPY) ;
  cur->add_field("end_var",    		FIELD_OBJECT, REF_PRIVATE) ;

  metaclasses_sot[NODE_VAR] = cur ;
}

static void init_meta_while(void)
{
  T_metaclass *cur = new T_metaclass("T_while",
									 sizeof(T_while),
									 metaclasses_sot[NODE_SUBSTITUTION]) ;

  cur->add_field("cond",   				FIELD_OBJECT, REF_PRIVATE) ;
  cur->add_field("first_body",   			FIELD_OBJECT, REF_PRIVATE_LIST) ;
  cur->add_field("last_body",    			FIELD_OBJECT, REF_NO_COPY) ;
  cur->add_field("invariant_keyword",    	FIELD_OBJECT, REF_PRIVATE) ;
  cur->add_field("invariant",    			FIELD_OBJECT, REF_PRIVATE) ;
  cur->add_field("variant_keyword",    	FIELD_OBJECT, REF_PRIVATE) ;
  cur->add_field("variant",    			FIELD_OBJECT, REF_PRIVATE) ;
  cur->add_field("end_while",    			FIELD_OBJECT, REF_PRIVATE) ;

  metaclasses_sot[NODE_WHILE] = cur ;
}

static void init_meta_any(void)
{
  T_metaclass *cur = new T_metaclass("T_any",
									 sizeof(T_any),
									 metaclasses_sot[NODE_SUBSTITUTION]) ;

  cur->add_field("first_ident",  	FIELD_OBJECT, REF_PRIVATE_LIST) ;
  cur->add_field("last_ident",   	FIELD_OBJECT, REF_NO_COPY) ;
  cur->add_field("where",   		FIELD_OBJECT, REF_PRIVATE) ;
  cur->add_field("first_body",   	FIELD_OBJECT, REF_PRIVATE_LIST) ;
  cur->add_field("last_body",    	FIELD_OBJECT, REF_NO_COPY) ;
  cur->add_field("end_any",    	FIELD_OBJECT, REF_PRIVATE) ;

  metaclasses_sot[NODE_ANY] = cur ;
}

static void init_meta_affect(void)
{
  T_metaclass *cur = new T_metaclass("T_affect",
									 sizeof(T_affect),
									 metaclasses_sot[NODE_SUBSTITUTION]) ;

  cur->add_field("first_name",  	FIELD_OBJECT, REF_PRIVATE_LIST) ;
  cur->add_field("last_name",   	FIELD_OBJECT, REF_NO_COPY) ;
  cur->add_field("first_value",  	FIELD_OBJECT, REF_PRIVATE_LIST) ;
  cur->add_field("last_value",   	FIELD_OBJECT, REF_NO_COPY) ;

  metaclasses_sot[NODE_AFFECT] = cur ;
}

static void init_meta_becomes_member_of(void)
{
  T_metaclass *cur = new T_metaclass("T_becomes_member_of",
									 sizeof(T_becomes_member_of),
									 metaclasses_sot[NODE_SUBSTITUTION]) ;

  cur->add_field("first_ident",  	FIELD_OBJECT, REF_PRIVATE_LIST) ;
  cur->add_field("last_ident",   	FIELD_OBJECT, REF_NO_COPY) ;
  cur->add_field("set",  			FIELD_OBJECT, REF_PRIVATE) ;

  metaclasses_sot[NODE_BECOMES_MEMBER_OF] = cur ;
}

static void init_meta_becomes_such_that(void)
{
  T_metaclass *cur = new T_metaclass("T_becomes_such_that",
									 sizeof(T_becomes_such_that),
									 metaclasses_sot[NODE_SUBSTITUTION]) ;

  cur->add_field("first_ident",  	FIELD_OBJECT, REF_PRIVATE_LIST) ;
  cur->add_field("last_ident",   	FIELD_OBJECT, REF_NO_COPY) ;
  cur->add_field("pred",     		FIELD_OBJECT, REF_PRIVATE) ;

  metaclasses_sot[NODE_BECOMES_SUCH_THAT] = cur ;
}

static void init_meta_op_call(void)
{
  T_metaclass *cur = new T_metaclass("T_op_call",
									 sizeof(T_op_call),
									 metaclasses_sot[NODE_SUBSTITUTION]) ;

  cur->add_field("op_name",  			FIELD_OBJECT) ;
  cur->add_field("first_in_param",  	FIELD_OBJECT, REF_PRIVATE_LIST) ;
  cur->add_field("last_in_param",  	FIELD_OBJECT, REF_NO_COPY) ;
  cur->add_field("first_out_param",  	FIELD_OBJECT, REF_PRIVATE_LIST) ;
  cur->add_field("last_out_param",  	FIELD_OBJECT, REF_NO_COPY) ;
  cur->add_field("ref_op",  			FIELD_OBJECT) ;

  metaclasses_sot[NODE_OP_CALL] = cur ;
}

static void init_meta_keyword(void)
{
  T_metaclass *cur = new T_metaclass("T_keyword",
									 sizeof(T_keyword),
									 metaclasses_sot[NODE_SYMBOL]) ;

  cur->add_field("lex_type",	   FIELD_INTEGER) ;

  metaclasses_sot[NODE_KEYWORD] = cur ;
}

static void init_meta_sequence(void)
{
  // Type de noeud inutilise
}

static void init_meta_integer(void)
{
  T_metaclass *cur = new T_metaclass("T_integer",
									 sizeof(T_integer),
									 metaclasses_sot[NODE_OBJECT]) ;
  cur->add_field("value",				FIELD_OBJECT) ;
  cur->add_field("is_positive", 		FIELD_INTEGER, REF_PRIVATE) ;
  metaclasses_sot[NODE_INTEGER] = cur ;
}


static void init_meta_betree_manager(void)
{
  T_metaclass *cur = new T_metaclass("T_betree_manager",
									 sizeof(T_betree_manager),
									 metaclasses_sot[NODE_OBJECT]) ;

  cur->add_field("first_betree",				FIELD_OBJECT) ;
  cur->add_field("last_betree",				FIELD_OBJECT) ;

  metaclasses_sot[NODE_BETREE_MANAGER] = cur ;
}


static void init_meta_betree_list(void)
{
  T_metaclass *cur = new T_metaclass("T_betree_list",
									 sizeof(T_betree_list),
									 metaclasses_sot[NODE_OBJECT]) ;

  cur->add_field("first_link",				FIELD_OBJECT) ;
  cur->add_field("last_link",					FIELD_OBJECT) ;

  metaclasses_sot[NODE_BETREE_LIST] = cur ;
}

static void init_meta_bound(void)
{
  T_metaclass *cur = new T_metaclass("T_bound",
									 sizeof(T_bound),
									 metaclasses_sot[NODE_EXPR]) ;

  cur->add_field("ref_type_decla",   			FIELD_OBJECT) ;
  cur->add_field("value", 		  			FIELD_OBJECT, REF_PRIVATE) ;
  cur->add_field("is_upper",					FIELD_INTEGER) ;

  metaclasses_sot[NODE_BOUND] = cur ;
}

static void init_meta_pragma(void)
{
  T_metaclass *cur = new T_metaclass("T_pragma",
									 sizeof(T_pragma),
									 metaclasses_sot[NODE_ITEM]) ;

  cur->add_field("name",				FIELD_OBJECT) ;
  cur->add_field("first_in_param",		FIELD_OBJECT, REF_PRIVATE_LIST) ;
  cur->add_field("last_in_param",		FIELD_OBJECT, REF_NO_COPY) ;
  cur->add_field("translated",			FIELD_INTEGER) ;
  cur->add_field("ref_comment",			FIELD_OBJECT) ;
  cur->add_field("ref_machine",			FIELD_OBJECT) ;
  cur->add_field("next_pragma",			FIELD_OBJECT) ;
  cur->add_field("prev_pragma",			FIELD_OBJECT) ;

  metaclasses_sot[NODE_PRAGMA] = cur ;
}

static void init_meta_pragma_lexem(void)
{
  T_metaclass *cur = new T_metaclass("T_pragma_lexem",
									 sizeof(T_pragma_lexem),
									 metaclasses_sot[NODE_OBJECT]) ;

  cur->add_field("lex_type",		   	FIELD_INTEGER) ;
  cur->add_field("value",				FIELD_STRING) ;
  cur->add_field("value_len",		   	FIELD_INTEGER) ;
  cur->add_field("ref_comment",	   	FIELD_OBJECT) ;
  cur->add_field("file_line",		   	FIELD_INTEGER) ;
  cur->add_field("file_column",	   	FIELD_INTEGER) ;
  cur->add_field("next_lexem",	   	FIELD_OBJECT) ;
  cur->add_field("prev_lexem",	   	FIELD_OBJECT) ;

  // On ne stocke pas les variables de classe

  metaclasses_sot[NODE_PRAGMA_LEXEM] = cur ;
}

static void init_meta_type(void)
{
  T_metaclass *cur = new T_metaclass("T_type",
									 sizeof(T_type),
									 metaclasses_sot[NODE_ITEM]) ;

  cur->add_field("B0_type",	  				FIELD_OBJECT, REF_PRIVATE) ;
  cur->add_field("type_name",	  			FIELD_OBJECT) ;
  // Le champ before_valuation_type est shared sinon on a une boucle infinie
  // entre T_type::before_valuation_type et T_abstract_type::after_valuation_type
  // On utilise le mecanisme du shared comme pour T_item::father
  cur->add_field("before_valuation_type",	FIELD_OBJECT, REF_SHARED) ;
  cur->add_field("typing_ident",			FIELD_OBJECT) ;

  metaclasses_sot[NODE_TYPE] = cur ;
}

static void init_meta_base_type(void)
{
  T_metaclass *cur = new T_metaclass("T_base_type",
									 sizeof(T_base_type),
									 metaclasses_sot[NODE_TYPE]) ;

  cur->add_field("name",	   			FIELD_OBJECT) ;
  cur->add_field("lower_bound",  		FIELD_OBJECT, REF_PRIVATE_LIST) ;
  cur->add_field("upper_bound",  		FIELD_OBJECT, REF_NO_COPY) ;

  metaclasses_sot[NODE_BASE_TYPE] = cur ;
}

static void init_meta_constructor_type(void)
{
  T_metaclass *cur = new T_metaclass("T_constructor_type",
									 sizeof(T_constructor_type),
									 metaclasses_sot[NODE_TYPE]) ;

  metaclasses_sot[NODE_CONSTRUCTOR_TYPE] = cur ;
}

static void init_meta_generic_type(void)
{
  T_metaclass *cur = new T_metaclass("T_generic_type",
									 sizeof(T_generic_type),
									 metaclasses_sot[NODE_TYPE]) ;

  cur->add_field("type",	   FIELD_OBJECT) ;

  metaclasses_sot[NODE_GENERIC_TYPE] = cur ;
}

static void init_meta_product_type(void)
{
  T_metaclass *cur = new T_metaclass("T_product_type",
									 sizeof(T_product_type),
									 metaclasses_sot[NODE_CONSTRUCTOR_TYPE]) ;

  cur->add_field("type1",	   	FIELD_OBJECT, REF_PRIVATE) ;
  cur->add_field("type2",	   	FIELD_OBJECT, REF_PRIVATE) ;
  metaclasses_sot[NODE_PRODUCT_TYPE] = cur ;
}

static void init_meta_setof_type(void)
{
  T_metaclass *cur = new T_metaclass("T_setof_type",
									 sizeof(T_setof_type),
									 metaclasses_sot[NODE_CONSTRUCTOR_TYPE]) ;

  cur->add_field("base_type",				FIELD_OBJECT, REF_PRIVATE) ;

  metaclasses_sot[NODE_SETOF_TYPE] = cur ;
}

static void init_meta_abstract_type(void)
{
  T_metaclass *cur = new T_metaclass("T_abstract_type",
									 sizeof(T_abstract_type),
									 metaclasses_sot[NODE_BASE_TYPE]) ;

  cur->add_field("set",					FIELD_OBJECT) ;//, REF_PRIVATE) ;
  cur->add_field("prev_abstract_type",	FIELD_OBJECT) ;
  cur->add_field("next_abstract_type",	FIELD_OBJECT) ;
  cur->add_field("after_valuation_type",FIELD_OBJECT, REF_PRIVATE) ;
  cur->add_field("valuation_ident", 	FIELD_OBJECT) ; //, REF_PRIVATE) ;
  metaclasses_sot[NODE_ABSTRACT_TYPE] = cur ;
}

static void init_meta_enumerated_type(void)
{
  T_metaclass *cur = new T_metaclass("T_enumerated_type",
									 sizeof(T_enumerated_type),
									 metaclasses_sot[NODE_BASE_TYPE]) ;

  cur->add_field("type_definition",			FIELD_OBJECT) ;
  metaclasses_sot[NODE_ENUMERATED_TYPE] = cur ;
}

static void init_meta_predefined_type(void)
{
  T_metaclass *cur = new T_metaclass("T_predefined_type",
									 sizeof(T_predefined_type),
									 metaclasses_sot[NODE_BASE_TYPE]) ;

  cur->add_field("type",				FIELD_INTEGER) ;
  cur->add_field("ref_interval",		    FIELD_OBJECT) ;

  metaclasses_sot[NODE_PREDEFINED_TYPE] = cur ;
}

static void init_meta_B0_type(void)
{
  T_metaclass *cur = new T_metaclass("T_B0_type",
									 sizeof(T_B0_type),
									 metaclasses_sot[NODE_ITEM]) ;

  cur->add_field("B_type",	  				FIELD_OBJECT) ;
  cur->add_field("is_a_type_definition",	FIELD_INTEGER) ;

  metaclasses_sot[NODE_B0_TYPE] = cur ;
}

static void init_meta_B0_base_type(void)
{
  T_metaclass *cur = new T_metaclass("T_B0_base_type",
									 sizeof(T_B0_base_type),
									 metaclasses_sot[NODE_B0_TYPE]) ;

  cur->add_field("name",		FIELD_OBJECT) ;
  cur->add_field("type",		FIELD_INTEGER) ;
  cur->add_field("is_bound",  FIELD_INTEGER) ;
  cur->add_field("min",		FIELD_OBJECT, REF_PRIVATE) ;
  cur->add_field("max",		FIELD_OBJECT, REF_PRIVATE) ;
  cur->add_field("ref_type",  FIELD_OBJECT) ;

  metaclasses_sot[NODE_B0_BASE_TYPE] = cur ;
}

static void init_meta_B0_abstract_type(void)
{
  T_metaclass *cur = new T_metaclass("T_B0_abstract_type",
									 sizeof(T_B0_abstract_type),
									 metaclasses_sot[NODE_B0_TYPE]) ;

  cur->add_field("name",			FIELD_OBJECT) ;
  cur->add_field("ref_machine",  	FIELD_OBJECT) ;
  cur->add_field("valuation_name", 		FIELD_OBJECT) ; //, REF_PRIVATE) ;
  cur->add_field("ref_interval",  FIELD_OBJECT) ;
  cur->add_field("ref_decla",  	FIELD_OBJECT) ;

  metaclasses_sot[NODE_B0_ABSTRACT_TYPE] = cur ;
}

static void init_meta_B0_enumerated_type(void)
{
  T_metaclass *cur = new T_metaclass("T_B0_enumerated_type",
									 sizeof(T_B0_enumerated_type),
									 metaclasses_sot[NODE_B0_TYPE]) ;

  // Tous les champs de type ident sont REF_SHARED car sinon il y
  // a bouclage leur de leur clonage :
  // ident -> type = T_enumerated_type -> B0_type = this -> ident ...
  cur->add_field("name",			  		FIELD_OBJECT, REF_SHARED) ;
  cur->add_field("ref_machine",			   	FIELD_OBJECT) ;
  cur->add_field("first_enumerated_value", 	FIELD_OBJECT, REF_SHARED) ;
  cur->add_field("last_enumerated_value",  	FIELD_OBJECT, REF_SHARED) ;

  metaclasses_sot[NODE_B0_ENUMERATED_TYPE] = cur ;
}

static void init_meta_B0_interval_type(void)
{
  T_metaclass *cur = new T_metaclass("T_B0_interval_type",
									 sizeof(T_B0_interval_type),
									 metaclasses_sot[NODE_B0_TYPE]) ;

  cur->add_field("prev_itype",		FIELD_OBJECT) ;
  cur->add_field("next_type",  		FIELD_OBJECT) ;
  cur->add_field("name",		  	FIELD_OBJECT) ; // pas REF_PRIVATE sinon boucle
  cur->add_field("min",  			FIELD_OBJECT, REF_PRIVATE) ;
  cur->add_field("min_bound",  		FIELD_OBJECT, REF_PRIVATE) ;
  cur->add_field("max",  			FIELD_OBJECT, REF_PRIVATE) ;
  cur->add_field("max_bound",  		FIELD_OBJECT, REF_PRIVATE) ;
  cur->add_field("abstract_type",  	FIELD_OBJECT, REF_PRIVATE) ;

  metaclasses_sot[NODE_B0_INTERVAL_TYPE] = cur ;
}

static void init_meta_B0_array_type(void)
{
  T_metaclass *cur = new T_metaclass("T_B0_array_type",
									 sizeof(T_B0_array_type),
									 metaclasses_sot[NODE_B0_TYPE]) ;

  cur->add_field("first_src_type",FIELD_OBJECT, REF_PRIVATE_LIST) ;
  cur->add_field("last_src_type", FIELD_OBJECT, REF_NO_COPY) ;
  cur->add_field("dst_type",     	FIELD_OBJECT, REF_PRIVATE) ;
  cur->add_field("ref_machine",  	FIELD_OBJECT) ;
  cur->add_field("name",  	   	FIELD_OBJECT, REF_SHARED) ;
  cur->add_field("next_decla",  	FIELD_OBJECT) ;
  cur->add_field("prev_decla",  	FIELD_OBJECT) ;

  metaclasses_sot[NODE_B0_ARRAY_TYPE] = cur ;
}


static void init_meta_record(void)
{
  T_metaclass *cur = new T_metaclass("T_record",
									 sizeof(T_record),
									 metaclasses_sot[NODE_EXPR]) ;

  cur->add_field("first_record_item",FIELD_OBJECT, REF_PRIVATE_LIST) ;
  cur->add_field("last_record_item", FIELD_OBJECT, REF_NO_COPY) ;

  metaclasses_sot[NODE_RECORD] = cur ;
}

static void init_meta_struct(void)
{
  T_metaclass *cur = new T_metaclass("T_struct",
									 sizeof(T_struct),
									 metaclasses_sot[NODE_EXPR]) ;

  cur->add_field("first_record_item",FIELD_OBJECT, REF_PRIVATE_LIST) ;
  cur->add_field("last_record_item", FIELD_OBJECT, REF_NO_COPY) ;

  metaclasses_sot[NODE_STRUCT] = cur ;
}

static void init_meta_record_item(void)
{
  T_metaclass *cur = new T_metaclass("T_record_item",
									 sizeof(T_record_item),
									 metaclasses_sot[NODE_EXPR]) ;

  cur->add_field("label", FIELD_OBJECT, REF_PRIVATE) ;
  cur->add_field("value", FIELD_OBJECT, REF_PRIVATE) ;

  metaclasses_sot[NODE_RECORD_ITEM] = cur ;
}

static void init_meta_record_access(void)
{
  T_metaclass *cur = new T_metaclass("T_record_access",
									 sizeof(T_record_access),
									 metaclasses_sot[NODE_EXPR]) ;

  cur->add_field("record", FIELD_OBJECT, REF_PRIVATE) ;
  cur->add_field("label", FIELD_OBJECT, REF_PRIVATE) ;

  metaclasses_sot[NODE_RECORD_ACCESS] = cur ;
}

static void init_meta_record_type(void)
{
  T_metaclass *cur = new T_metaclass("T_record_type",
									 sizeof(T_record_type),
									 metaclasses_sot[NODE_CONSTRUCTOR_TYPE]) ;

  cur->add_field("first_label", FIELD_OBJECT, REF_PRIVATE_LIST) ;
  cur->add_field("last_label", FIELD_OBJECT, REF_NO_COPY) ;

  metaclasses_sot[NODE_RECORD_TYPE] = cur ;
}

static void init_meta_label_type(void)
{
  T_metaclass *cur = new T_metaclass("T_label_type",
									 sizeof(T_label_type),
									 metaclasses_sot[NODE_TYPE]) ;

  cur->add_field("name", FIELD_OBJECT, REF_PRIVATE) ;
  cur->add_field("type", FIELD_OBJECT, REF_PRIVATE) ;

  metaclasses_sot[NODE_LABEL_TYPE] = cur ;
}


static void init_meta_B0_record_type(void)
{
  T_metaclass *cur = new T_metaclass("T_B0_record_type",
									 sizeof(T_B0_record_type),
									 metaclasses_sot[NODE_B0_TYPE]) ;

  cur->add_field("first_label", FIELD_OBJECT, REF_PRIVATE_LIST) ;
  cur->add_field("last_label", FIELD_OBJECT, REF_NO_COPY) ;
  cur->add_field("ref_machine",  	FIELD_OBJECT) ;
  cur->add_field("name",  	   	FIELD_OBJECT, REF_SHARED) ;
  cur->add_field("next_decla",  	FIELD_OBJECT) ;
  cur->add_field("prev_decla",  	FIELD_OBJECT) ;

  metaclasses_sot[NODE_B0_RECORD_TYPE] = cur ;
}

static void init_meta_B0_label_type(void)
{
  T_metaclass *cur = new T_metaclass("T_B0_label_type",
									 sizeof(T_B0_label_type),
									 metaclasses_sot[NODE_B0_TYPE]) ;

  cur->add_field("name", FIELD_OBJECT, REF_PRIVATE) ;
  cur->add_field("type", FIELD_OBJECT, REF_PRIVATE) ;

  metaclasses_sot[NODE_B0_LABEL_TYPE] = cur ;
}

static void init_meta_msg_line(void)
{
  T_metaclass *cur = new T_metaclass("T_msg_line",
									 sizeof(T_msg_line),
									 metaclasses_sot[NODE_ITEM]) ;

  cur->add_field("msg_stream", FIELD_INTEGER) ;
  cur->add_field("file_name", FIELD_OBJECT, REF_PRIVATE) ;
  cur->add_field("file_line", FIELD_INTEGER) ;
  cur->add_field("file_column", FIELD_INTEGER) ;
  cur->add_field("contents", FIELD_OBJECT, REF_PRIVATE) ;

  metaclasses_sot[NODE_MSG_LINE] = cur ;
}

static void init_meta_msg_line_manager(void)
{
  T_metaclass *cur = new T_metaclass("T_msg_line_manager",
									 sizeof(T_msg_line_manager),
									 metaclasses_sot[NODE_ITEM]) ;

  cur->add_field("first_msg", FIELD_OBJECT, REF_PRIVATE_LIST) ;
  cur->add_field("last_msg", FIELD_OBJECT, REF_NO_COPY) ;

  metaclasses_sot[NODE_MSG_LINE_MANAGER] = cur ;
}

static void init_meta_component(void)
{
  // creation de la metaclasse associee
  T_metaclass *meta = new T_metaclass("T_component",
									  sizeof(T_component),
									  get_metaclass(NODE_ITEM));

  // description des champs
  meta->add_field("name", FIELD_OBJECT);
  meta->add_field("machine_type", FIELD_INTEGER);
  meta->add_field("type", FIELD_INTEGER);
  meta->add_field("file", FIELD_OBJECT);
  meta->add_field("checksum", FIELD_OBJECT);
  meta->add_field("index", FIELD_INTEGER);
  meta->add_field("stamp", FIELD_INTEGER);
  meta->add_field("machine", FIELD_OBJECT);
  meta->add_field("compo_checksums", FIELD_OBJECT);

  metaclasses_sot[NODE_COMPONENT] = meta ;
}

static void init_meta_file_component(void)
{
  // creation de la metaclasse associee
  T_metaclass *meta = new T_metaclass("T_file_component",
									  sizeof(T_file_component),
									  get_metaclass(NODE_ITEM));

  // description des champs
  meta->add_field("path", FIELD_OBJECT);
  meta->add_field("name", FIELD_OBJECT);
  meta->add_field("suffix", FIELD_OBJECT);
  meta->add_field("user", FIELD_OBJECT);
  meta->add_field("date", FIELD_TIME_T);
  meta->add_field("first_component", FIELD_OBJECT, REF_PRIVATE_LIST);
  meta->add_field("last_component", FIELD_OBJECT, REF_NO_COPY);
  meta->add_field("first_file_definition", FIELD_OBJECT, REF_PRIVATE_LIST);
  meta->add_field("last_file_definition", FIELD_OBJECT, REF_NO_COPY);
  meta->add_field("generated_from_component", FIELD_OBJECT);

  metaclasses_sot[NODE_FILE_COMPONENT] = meta ;
}

static void init_meta_project(void)
{
  // creation de la metaclasse associee
  T_metaclass *meta = new T_metaclass("T_project",
									  sizeof(T_project),
									  get_metaclass(NODE_ITEM));

  // description des champs
  meta->add_field("index", FIELD_INTEGER);
  meta->add_field("old_tools_decomp_dir", FIELD_OBJECT);
  meta->add_field("decomp_dir", FIELD_OBJECT);
  meta->add_field("bdp_dir", FIELD_OBJECT);
  meta->add_field("timestamp", FIELD_INTEGER);
  meta->add_field("first_source_path", FIELD_OBJECT, REF_PRIVATE_LIST);
  meta->add_field("last_source_path", FIELD_OBJECT, REF_NO_COPY);
  meta->add_field("first_file", FIELD_OBJECT, REF_PRIVATE_LIST);
  meta->add_field("last_file", FIELD_OBJECT, REF_NO_COPY);
  meta->add_field("first_component", FIELD_OBJECT, REF_PRIVATE_LIST);
  meta->add_field("last_component", FIELD_OBJECT, REF_NO_COPY);
  meta->add_field("project_type", FIELD_INTEGER);
  meta->add_field("first_proof_mechanism", FIELD_OBJECT, REF_PRIVATE_LIST);
  meta->add_field("last_proof_mechanism", FIELD_OBJECT, REF_NO_COPY);

  metaclasses_sot[NODE_PROJECT] = meta ;
}

static void init_meta_project_manager(void)
{
  // creation de la metaclasse associee
  T_metaclass *meta = new T_metaclass("T_project_manager",
									  sizeof(T_project_manager),
									  get_metaclass(NODE_OBJECT));

  // description des champs
  meta->add_field("current", FIELD_OBJECT);
  meta->add_field("save_flag", FIELD_INTEGER) ;
  meta->add_field("modified_flag", FIELD_INTEGER) ;
  meta->add_field("file", FIELD_OBJECT);
  meta->add_field("file_date", FIELD_TIME_T);
  meta->add_field("first_library", FIELD_OBJECT);
  meta->add_field("last_library", FIELD_OBJECT);
  meta->add_field("first_missing", FIELD_OBJECT);
  meta->add_field("last_missing", FIELD_OBJECT);
  meta->add_field("first_removed", FIELD_OBJECT);
  meta->add_field("last_removed", FIELD_OBJECT);
  meta->add_field("first_added", FIELD_OBJECT);
  meta->add_field("last_added", FIELD_OBJECT);
  meta->add_field("first_file_def", FIELD_OBJECT);
  meta->add_field("last_file_def", FIELD_OBJECT);

  metaclasses_sot[NODE_PROJECT_MANAGER] = meta ;
}

static void init_meta_component_checksums(void)
{
  // creation de la metaclasse associee
  T_metaclass *meta = new T_metaclass("T_component_checksums",
									  sizeof(T_component_checksums),
									  get_metaclass(NODE_ITEM));

  // description des champs
  meta->add_field("component_name", FIELD_OBJECT);
  meta->add_field("type", FIELD_INTEGER);
  meta->add_field("formal_params_checksum", FIELD_OBJECT);
  meta->add_field("constraints_clause_checksum", FIELD_OBJECT);
  meta->add_field("sees_clause_checksum", FIELD_OBJECT);
  meta->add_field("includes_clause_checksum", FIELD_OBJECT);
  meta->add_field("imports_clause_checksum", FIELD_OBJECT);
  meta->add_field("promotes_clause_checksum", FIELD_OBJECT);
  meta->add_field("extends_clause_checksum", FIELD_OBJECT);
  meta->add_field("uses_clause_checksum", FIELD_OBJECT);
  meta->add_field("sets_clause_checksum", FIELD_OBJECT);
  meta->add_field("concrete_variables_clause_checksum", FIELD_OBJECT);
  meta->add_field("abstract_variables_clause_checksum", FIELD_OBJECT);
  meta->add_field("concrete_constants_clause_checksum", FIELD_OBJECT);
  meta->add_field("abstract_constants_clause_checksum", FIELD_OBJECT);
  meta->add_field("properties_clause_checksum", FIELD_OBJECT);
  meta->add_field("invariant_clause_checksum", FIELD_OBJECT);
  meta->add_field("variant_clause_checksum", FIELD_OBJECT);
  meta->add_field("assertions_clause_checksum", FIELD_OBJECT);
  meta->add_field("values_clause_checksum", FIELD_OBJECT);
  meta->add_field("refined_checksums", FIELD_OBJECT);
  meta->add_field("first_seen_checksums", FIELD_OBJECT);
  meta->add_field("last_seen_checksums", FIELD_OBJECT);
  meta->add_field("first_used_checksums", FIELD_OBJECT);
  meta->add_field("last_used_checksums", FIELD_OBJECT);
  meta->add_field("first_included_checksums", FIELD_OBJECT);
  meta->add_field("last_included_checksums", FIELD_OBJECT);
  meta->add_field("first_imported_checksums", FIELD_OBJECT);
  meta->add_field("last_imported_checksums", FIELD_OBJECT);
  meta->add_field("first_extended_checksums", FIELD_OBJECT);
  meta->add_field("last_extended_checksums", FIELD_OBJECT);
  meta->add_field("first_operation_checksums", FIELD_OBJECT);
  meta->add_field("last_operation_checksums", FIELD_OBJECT);

  metaclasses_sot[NODE_COMPONENT_CHECKSUMS] = meta;
}

static void init_meta_operation_checksums(void)
{
  // creation de la metaclasse associee
  T_metaclass *meta = new T_metaclass("T_operation_checksums",
									  sizeof(T_operation_checksums),
									  get_metaclass(NODE_ITEM));

  // description des champs
  meta->add_field("operation_name", FIELD_OBJECT);
  meta->add_field("component_name", FIELD_OBJECT);
  meta->add_field("precondition_checksum", FIELD_OBJECT);
  meta->add_field("body_checksum", FIELD_OBJECT);
  meta->add_field("first_called_operation_checksums", FIELD_OBJECT);
  meta->add_field("last_called_operation_checksums", FIELD_OBJECT);
  meta->add_field("first_refined_operation_checksums", FIELD_OBJECT);
  meta->add_field("last_refined_operation_checksums", FIELD_OBJECT);
  meta->add_field("modified", FIELD_INTEGER);

  metaclasses_sot[NODE_OPERATION_CHECKSUMS] = meta;
}

static void init_meta_op_call_tree(void)
{
  // creation de la metaclasse
  T_metaclass *meta = new T_metaclass("T_op_call_tree",
									  sizeof(T_op_call_tree),
									  get_metaclass(NODE_ITEM));
  //les champs
  meta->add_field("first_son", FIELD_OBJECT);
  meta->add_field("last_son", FIELD_OBJECT);

  metaclasses_sot[NODE_OP_CALL_TREE]=meta;
}

// Initialisation des metaclasses
static int init_metaclasses_done_si = FALSE ;
void init_metaclasses(void)
{
  if (init_metaclasses_done_si == TRUE)
	{
	  // Deja fait
	  return ;
	}

  TRACE0("DEBUT init_metaclasses") ;
  init_metaclasses_done_si = TRUE ;

  for (size_t i = 0 ; i < array_size_si ; i++)
      metaclasses_sot[i] = nullptr ;

      //
  // Initialisation des metaclasses
  // ATTENTION !!!
  // Il faut initialiser les metaclasses dans l'ordre de hierachie des classes
  //

  // Niveau 0
  init_meta_object() ;

  // Sous-classes de T_object
  init_meta_lexem() ;
  init_meta_item() ;

  // Sous-classes de T_item
  init_meta_betree() ;
  init_meta_case_item() ;
  init_meta_comment() ;
  init_meta_definition() ;
  init_meta_file_definition() ;
  init_meta_else() ;
  init_meta_expr() ;
  init_meta_flag() ;
  init_meta_list_link() ;
  init_meta_machine() ;
  init_meta_generic_operation() ;
  init_meta_operation() ;
  init_meta_or() ;
  init_meta_predicate() ;
  init_meta_substitution() ;
  init_meta_symbol() ;
  init_meta_used_op() ;
  init_meta_used_machine() ;
  init_meta_valuation() ;
  init_meta_when() ;
  init_meta_op_call_tree() ;

  // Sous-classes de T_expr
  init_meta_ident() ;
  init_meta_literal_integer() ;
  init_meta_binary_op() ;
  init_meta_expr_with_parenthesis() ;
  init_meta_unary_op() ;
  init_meta_op_result() ;
  init_meta_array_item() ;
  init_meta_converse() ;
  init_meta_literal_string() ;
  init_meta_literal_real() ;
  init_meta_comprehensive_set() ;
  init_meta_image() ;
  init_meta_empty_seq() ;
  init_meta_empty_set() ;
  init_meta_sigma() ;
  init_meta_pi() ;
  init_meta_quantified_union() ;
  init_meta_quantified_intersection() ;
  init_meta_literal_boolean() ;
  init_meta_literal_enumerated_value() ;
  init_meta_relation() ;
  init_meta_renamed_ident() ;
  init_meta_extensive_seq() ;
  init_meta_generalised_union() ;
  init_meta_generalised_intersection() ;
  init_meta_extensive_set() ;

  // Sous-classes de T_predicate
  init_meta_lambda_expr() ;
  init_meta_binary_predicate() ;
  init_meta_typing_predicate() ;
  init_meta_predicate_with_parenthesis() ;
  init_meta_not_predicate() ;
  init_meta_expr_predicate() ;
  init_meta_universal_predicate() ;
  init_meta_existential_predicate() ;

  // Sous-classes de T_substitution
  init_meta_skip() ;
  init_meta_begin() ;
  init_meta_choice() ;
  init_meta_precond() ;
  init_meta_assert() ;
  init_meta_label() ;
  init_meta_witness() ;
  init_meta_jumpif() ;
  init_meta_if() ;
  init_meta_select() ;
  init_meta_case() ;
  init_meta_let() ;
  init_meta_var() ;
  init_meta_while() ;
  init_meta_any() ;
  init_meta_affect() ;
  init_meta_becomes_member_of() ;
  init_meta_becomes_such_that() ;
  init_meta_op_call() ;

  // Sous-classes de T_symbol
  init_meta_keyword() ;

  // Types
  init_meta_type() ;

  init_meta_base_type() ;
  init_meta_predefined_type() ;
  init_meta_abstract_type() ;
  init_meta_enumerated_type() ;

  init_meta_constructor_type() ;
  init_meta_product_type() ;
  init_meta_setof_type() ;
  init_meta_generic_type() ;

  // Types B0
  init_meta_B0_type() ;
  init_meta_B0_base_type() ;
  init_meta_B0_enumerated_type() ;
  init_meta_B0_abstract_type() ;
  init_meta_B0_interval_type() ;
  init_meta_B0_array_type() ;

  // Autres
  init_meta_integer() ;
  init_meta_betree_manager() ;
  init_meta_betree_list() ;
  init_meta_bound() ;
  init_meta_pragma() ;
  init_meta_pragma_lexem() ;

  init_meta_record() ;
  init_meta_struct() ;
  init_meta_record_item() ;
  init_meta_record_access() ;
  init_meta_record_type() ;
  init_meta_label_type() ;
  init_meta_B0_record_type() ;
  init_meta_B0_label_type() ;

  init_meta_msg_line() ;
  init_meta_msg_line_manager() ;
  // Inutilises
  init_meta_sequence() ;

  // MS
  init_meta_component() ;
  init_meta_file_component() ;
  init_meta_project() ;
  init_meta_project_manager() ;
  init_meta_component_checksums() ;
  init_meta_operation_checksums() ;

#ifndef NO_CHECKS
  // Verification : que le tableau est rempli, que les tailles correspondent
  for (int i = 0 ; i < NODE_USER_DEFINED1 ; i++)
	{
	  if (metaclasses_sot[i] == NULL)
		{
		  fprintf(stderr,
				  "!!! pas de description de metaclasse pour node_type=%d\n",
                                  (int)i) ;
		}
	  else
		{
		  T_field *field = metaclasses_sot[i]->get_last_field() ;
		  size_t size ;
		  if (field != NULL)
			{
			  size = 	 field->get_field_offset()
				+ internal_get_field_size(field->get_field_type());

			  // The size of the class must be a multiple of the maximum size
			  // that can have a field of the metaclass.
			  size_t max_field_size = metaclasses_sot[i]->get_max_field_size();
			  while (size % max_field_size) ++size;

#ifdef __x86_64__
                  // On x64, the size of the class is a multiple of 8 bytes
          while(size % 8)
          {
              ++size;
          }
#endif // __x86_64__
			}
		  else
			{
			  if (metaclasses_sot[i]->get_superclass() != NULL)
				{
				  size = metaclasses_sot[i]->get_superclass()->get_size() ;
				}
			  else
				{
				  size = 0 ;
				}
			}


		  if (size != metaclasses_sot[i]->get_size())
			{
			  fprintf(stderr,
					  "! node_type=%d \"%s\" : sizeof=%d octets!=%d octets decrits\n",
                                          (int)i,
					  metaclasses_sot[i]->get_class_name(),
                                          (int)metaclasses_sot[i]->get_size(),
                                          (int)size) ;
			}

#ifdef DEBUG_META
		  // Production de warnings pour les elements de la table qui semble
		  // louches
		  T_field *cur_field = metaclasses_sot[i]->get_fields() ;

		  while (cur_field != NULL)
			{
			  if (cur_field->get_field_type() != FIELD_OBJECT)
				{
				  if (cur_field->get_ref_type() != REF_SHARED)
					{
					  fprintf(stderr, "! field %s:%s is not shared\n",
							  metaclasses_sot[i]->get_class_name(),
							  cur_field->get_field_name()) ;
					}
				}
			  else
				{
				  // Champ de type objet
				  const char *name = cur_field->get_field_name() ;
				  TRACE4("name %s /first %d /last %d  /ref %d",
						 name,
						 strncmp(name, "first", 5),
						 strncmp(name, "last", 4),
						 strncmp(name, "ref", 3)) ;
				  if (strncmp(name, "first", 5) == 0)
					{
					  if (cur_field->get_ref_type() != REF_PRIVATE_LIST)
						{
						  fprintf(stderr,
								  "! field %s:%s is not a private list\n",
								  metaclasses_sot[i]->get_class_name(),
								  cur_field->get_field_name()) ;

						}
					}
				  else if (strncmp(name, "last", 4) == 0)
					{
					  if (cur_field->get_ref_type() != REF_NO_COPY)
						{
						  fprintf(stderr, "! field %s:%s is not a no_copy\n",
								  metaclasses_sot[i]->get_class_name(),
								  cur_field->get_field_name()) ;
						}
					}
				  else if (strncmp(name, "ref", 3) != 0)
					{
					  if (cur_field->get_ref_type() != REF_PRIVATE)
						{
						  fprintf(stderr, "! field %s:%s is not private\n",
								  metaclasses_sot[i]->get_class_name(),
								  cur_field->get_field_name()) ;
						}
					}

				}
			  cur_field = cur_field->get_next() ;
			}
#endif // DEBUG_META
		}
	}
#endif // NO_CHECKS

  TRACE0("FIN init_metaclasses") ;
}

void unlink_metaclasses(void)
{
  if (init_metaclasses_done_si == FALSE)
    {
      return ;
    }

  init_metaclasses_done_si = FALSE ;

  for (size_t i = 0 ; i < array_size_si ; i++)
    {
      delete metaclasses_sot[i] ;
      metaclasses_sot[i] = NULL ;
    }
}

// Obtention de la metclasse pour un node_type donne
T_metaclass *get_metaclass(T_node_type node_type)
{
#ifdef DEBUG_META
  TRACE1("get_metaclass(%d)", node_type) ;
#endif
  ASSERT((size_t) node_type < (size_t) (sizeof(metaclasses_sot)/sizeof(void *))) ;
#ifdef DEBUG_META
  TRACE2("get_metaclass(%d) -> %x", node_type, metaclasses_sot[node_type]) ;
#endif
  return metaclasses_sot[node_type] ;
}

// Obtention de la taille de la metclasse pour un node_type donne
size_t get_class_size(T_node_type node_type)
{
  T_metaclass *meta = get_metaclass(node_type) ;
  size_t res = meta->get_size() ;
  TRACE3("get_class_size(%d:%s) -> %d", node_type, meta->get_class_name(), res) ;
  return res ;
}

// Obtention de la taille de la metclasse pour un node_type donne
size_t get_class_size_before_upgrade(T_node_type node_type)
{
  T_metaclass *meta = get_metaclass(node_type) ;
  size_t res = meta->get_size_before_upgrade() ;
  TRACE3("get_class_size_before_upgrade(%d:%s) -> %d", node_type, meta->get_class_name(), res) ;
  return res ;
}


#if (defined(__BCOMP__) || defined(__INTERFACE__) )
// Mise a jour de la metclasse pour un node_type donne
void set_metaclass(T_node_type node_type, T_metaclass *metaclass)
{
#ifdef DEBUG_META
  TRACE2("set_metaclass(%d,%x)", node_type, metaclass) ;
#endif
  ASSERT((size_t) node_type < (size_t)(sizeof(metaclasses_sot)/sizeof(void *))) ;
  metaclasses_sot[node_type] = metaclass ;
}
#endif

// Constructeur de la classe T_metaclass
T_metaclass::T_metaclass(const char *new_class_name,
								  size_t new_size,
								  T_metaclass *new_superclass)
{
#ifdef DEBUG_META
  TRACE5("T_metaclass::T_metaclass(\"%s\", %d, %x (\"%s\")) -> %x",
		 new_class_name,
		 new_size,
		 new_superclass,
		 (new_superclass == NULL) ? "NULL" : new_superclass->class_name,
		 this) ;
#endif
  class_name = new_class_name ;
  size = new_size ;
  size_before_upgrade = new_size ;
  superclass = new_superclass ;
  depth = 0 ;
  first_derived = NULL ;
  next_derived = NULL ;
  first_field = last_field = NULL ;
}

T_metaclass::~T_metaclass()
{
  for (T_field *field = first_field; field;)
    {
      T_field *field_to_delete = field ;
      field = field->get_next() ;
      delete field_to_delete ;
    }
}

// Ajout d'un champ
void T_metaclass::add_field(const char *new_field_name,
                                     T_field_type new_field_type,
                                     T_ref_type new_ref_type) // = REF_SHARED)
{
#ifdef DEBUG_META
  TRACE4("T_metaclass(%x)::add_field(\"%s\", %d ref_type %s)",
		 this,
		 new_field_name,
		 new_field_type,
		 debug_ref_type_name_sct[new_ref_type]) ;
#endif
  (void)new T_field(new_field_name,
			  new_field_type,
			  new_ref_type,
			  &first_field,
			  &last_field,
			  this) ;
}

// Constructeur de la classe T_field
T_field::T_field(const char *new_field_name,
						  T_field_type new_field_type,
						  T_ref_type new_ref_type,
						  T_field **adr_first,
						  T_field **adr_last,
						  T_metaclass *new_metaclass)
{
#ifdef DEBUG_META
  TRACE7("T_field::T_field(\"%s\", %d, %d, %x, %x, %x) -> %x",
		 new_field_name,
		 new_field_type,
		 new_ref_type,
		 adr_first,
		 adr_last,
		 new_metaclass,
		 this) ;
#endif

  field_name = new_field_name ;
  field_type = new_field_type ;
  ref_type = new_ref_type ;
  metaclass = new_metaclass ;

  // Chainage
  // Chainage en queue le cas echeant
  if (adr_first != NULL)
	{
	  ASSERT(adr_last != NULL) ;

	  if (*adr_last == NULL)
		{
		  //
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
	  *adr_last = this ;
	}
  else
	{
	  // Pas de chainage, on met les pointeurs next/prev a NULL
	  next = NULL ;
	  prev = NULL ;
	}

  // Calcul de l'offset
  if (prev != NULL)
	{
	  // Il y a deja un champ dans la metaclasse
#ifdef DEBUG_META
	  TRACE4("classe %s : l'offset du champ %s est %d (before upgrade : %d)",
			 metaclass->get_class_name(),
			 prev->field_name,
			 prev->field_offset,
			 prev->field_offset_before_upgrade) ;
	  TRACE4("classe %s : la taille du champ %s est %d (before upgrade : %d)",
			 metaclass->get_class_name(),
			 prev->field_name,
			 internal_get_field_size(prev->field_type),
			 internal_get_field_size_before_upgrade(prev->field_type)) ;
#endif
	  field_offset = prev->field_offset + internal_get_field_size(prev->field_type);
	  field_offset_before_upgrade =
		prev->field_offset_before_upgrade
		+ internal_get_field_size_before_upgrade(prev->field_type);
	}
  else if (metaclass->get_superclass() != NULL)
	{
	  // Premier champ apres les champs de la superclasse
          // Search for the last field of the superclass
          T_metaclass *super_class = metaclass->get_superclass();
          T_field *last_field = super_class->get_last_field();
          while(super_class && !last_field)
          {
              super_class = super_class->get_superclass();
              if(super_class)
              {
                  last_field = super_class->get_last_field();
              }
          }
          if(last_field)
          {
              field_offset = last_field->get_field_offset() + internal_get_field_size(last_field->get_field_type());
              field_offset_before_upgrade = last_field->get_field_offset_before_upgrade()
                                            + internal_get_field_size_before_upgrade(last_field->get_field_type());
          }
          else
          {
              // Ne devrait jamais arriver, car T_object contient des champs
              field_offset = field_offset_before_upgrade = 0;
          }
	}
  else
	{
	  // Premier champ de la premiere classe
	  field_offset = field_offset_before_upgrade = 0 ;
	}

  // On aligne le champ sur sa taille (nécessaire pour champs > 4 octets)
  int field_size = internal_get_field_size(field_type);
  while(field_offset % field_size)
  {
      ++field_offset;
  }

  while(field_offset_before_upgrade % field_size)
  {
      ++ field_offset_before_upgrade;
  }



#ifdef DEBUG_META
  TRACE4("classe %s : l'offset du champ %s est %d (before upgrade : %d)",
		 metaclass->get_class_name(),
		 field_name,
		 field_offset,
		 field_offset_before_upgrade) ;
#endif
}

// Taille d'un champ
size_t T_field::get_field_size(void)
{
  return internal_get_field_size(field_type) ;
}

// Obtention d'une metaclasse d'un nom donne, ou NULL si elle n'existe pas
T_metaclass *get_metaclass(const char *class_name)
{
  int i = 0 ;

  for (i = 0 ; i < NODE_USER_DEFINED20 ; i++)
	{
	  T_metaclass *cur_meta = metaclasses_sot[i] ;
	  // peut etre NULL pour user defined classes
	  const char *cur_name = (cur_meta == NULL) ? NULL : cur_meta->get_class_name();
	  if ( (cur_name != NULL) && (strcmp(class_name, cur_name) == 0) )
	  // pas tres optimise mais non critique, alors ...
		{
		  return cur_meta ;
		}
	}

  // Classe non trouvee
  return NULL ;
}

// Recalculer l'offset
void T_field::fix_offsets(void)
{
  // Calcul de l'offset
  if (prev != NULL)
	{
	  // Il y a deja un champ dans la metaclasse
	  field_offset = prev->field_offset + internal_get_field_size(prev->field_type) ;
	  field_offset_before_upgrade = prev->field_offset_before_upgrade + internal_get_field_size_before_upgrade(prev->field_type) ;
	}
  else if (metaclass->get_superclass() != NULL)
	{
	  // Premier champ apres les champs de la superclasse
	  field_offset = metaclass->get_superclass()->get_size() ;
	  field_offset_before_upgrade = metaclass->get_superclass()->get_size_before_upgrade() ;

//#ifdef __x86_64__
//          // On x86_64, class size is aligned to 8 bytes
//          while(field_offset % 8)
//          {
//              ++field_offset;
//          }
//          while(field_offset_before_upgrade % 8)
//          {
//              ++field_offset_before_upgrade;
//          }
//#endif // __x86_64__
	}
  else
	{
	  // Premier champ de la premiere classe
	  field_offset = 0 ;
	  field_offset_before_upgrade = 0 ;
	}

  // Realigne les champs si nécessaire
  int field_size = internal_get_field_size(field_type);
  while(field_offset % field_size)
  {
      ++field_offset;
  }

  field_size = internal_get_field_size_before_upgrade(field_type);
  while(field_offset_before_upgrade % field_size)
  {
      ++field_offset_before_upgrade;
  }

  TRACE4("class <%s> field <%s> offset <%d> before_upgrade <%d>",
		 metaclass->get_class_name(),
		 field_name,
		 field_offset,
		 field_offset_before_upgrade) ;
}


// Recherche d'un champ par son nom (renvoie NULL si n'existe pas)
T_field *T_metaclass::find_field(const char *field_name)
{
  T_field *cur_field = first_field ;

  while (cur_field != NULL)
	{
	  if (strcmp(cur_field->get_field_name(), field_name) == 0)
		{
		  return cur_field ;
		}

	  cur_field = cur_field->get_next() ;
	}

  return NULL ;
}

// Recalcul de tous les offsets de tous les champs de toutes les metaclasses
static void fix_single_metaclass_offsets(T_metaclass *cur_meta)
{
  // On stocke dans tmp2 l'heure du fix pour savoir s'il a deja ete fait
  if (cur_meta->get_timestamp() == get_timestamp())
	{
	  // Deja fait
	  //
	  return ;
	}

  // On y va ...
  // Il faut d'abord recalculer recursivement les champs de la superclasse
  if (cur_meta->get_superclass() != NULL)
	{
	  fix_single_metaclass_offsets(cur_meta->get_superclass()) ;
	}


  // On s'occupe maintenant de la classe courante
  T_field *cur_field = cur_meta->get_fields() ;

  while (cur_field != NULL)
	{
	  cur_field->fix_offsets() ;
	  cur_field = cur_field->get_next() ;
	}

  T_field *field = cur_meta->get_last_field() ;
  size_t size ;
  size_t size_before_upgrade ;
  if (field != NULL)
	{
	  size = 	 field->get_field_offset()
		+ internal_get_field_size(field->get_field_type());
	  size_before_upgrade = 	 field->get_field_offset_before_upgrade()
		+ internal_get_field_size_before_upgrade(field->get_field_type());
	}
  else
	{
	  if (cur_meta->get_superclass() != NULL)
		{
		  size = cur_meta->get_superclass()->get_size() ;
		  size_before_upgrade = cur_meta->get_superclass()->get_size_before_upgrade() ;
		}
	  else
		{
		  size = 0 ;
		  size_before_upgrade = 0 ;
		}
	}

  cur_meta->set_size(size) ;
  cur_meta->set_size_before_upgrade(size_before_upgrade) ;

  // On note que le travail a ete realise
  cur_meta->set_timestamp(get_timestamp()) ;
  TRACE4("FIX cur_meta %d %s size %d before upgrade %d",
		 cur_meta,
		 cur_meta->get_class_name(),
		 size,
		 size_before_upgrade) ;

}

// Recalcul de tous les offsets de tous les champs de toutes les metaclasses
void fix_metaclasses_offsets(void)
{
  int i = 0 ;

  // Obtention timestamp suivant
  // On stocke dans tmp2 l'heure du fix pour savoir s'il a deja ete fait
  next_timestamp() ;

  for (i = 0 ; i < NODE_USER_DEFINED20 ; i++)
	{
	  T_metaclass *cur_meta = metaclasses_sot[i] ;

	  if (cur_meta != NULL)
		{
		  fix_single_metaclass_offsets(cur_meta) ;
		}
	}

}

//
// Methodes pour la production automatique de la liste des classes
//

// Calcule la profondeur d'une classe en appelant recursivement la
// methode pour la superclasse si besoin est.
// Au passage, met a jour la liste des classes derivees de la superclasse
void T_metaclass::make_depth(void)
{
  // Si depth != 0 le calcul a deja ete fait
  if (depth != 0)
	{
	  return ;
	}

  // Il faut calculer la profondeur

  if (superclass == NULL)
	{
	  // Si pas de superclasse,  on est au niveau 1
	  depth = 1 ;
	  return ;
	}

  // On a une supercalsse
  // le niveau de this est le niveau de superclass +1
  superclass->make_depth() ;
  depth = superclass->get_depth() + 1 ;

  // On s'ajoute dans la liste des classes filles de superclass
  superclass->add_derived_class(this) ;
}

// Ajout d'une sous-classe
// Les sous-classes sont triees par ordre alphabetique
void T_metaclass::add_derived_class(T_metaclass *derived)
{
  T_metaclass *cur = first_derived ;
  T_metaclass *prev = NULL ;

  while (    (cur != NULL)
		  && (strcmp(cur->get_class_name(), derived->get_class_name()) < 0) )
	{
	  prev = cur ;
	  cur = cur->get_next_derived() ;
	}

  // Ici on sait qu'il faut s'inserer entre prev et cur
  if (prev == NULL)
	{
	  // Ajout en tete
	  first_derived = derived ;
	}
  else
	{
	  // Ajout apres prev
	  prev->set_next_derived(derived) ;
	}

  derived->set_next_derived(cur) ;
}

// Cree dans output_path une page decrivant la classe
// Ajoute dans index une entree pour la classe
void T_metaclass::print_class_hierarchy(const char *output_path,
												 size_t output_path_len,
												 FILE *index)
{
  TRACE1(">> T_metaclass(%s)::print_class_hierarchy", class_name) ;
  ENTER_TRACE ;

  //longueur du nom de fichier +1 pour / +5 pour le .html
  size_t len = output_path_len + strlen(class_name) + 6;

  //pour le nom de fichier ou dumper les champ
  char* file_name = new char[len + 1];
  //                   1  23456
  sprintf(file_name,"%s/%s.html",output_path,class_name);

  FILE *class_members = fopen (file_name, "w");
  if (class_members == NULL)
	{
	  toplevel_error(FATAL_ERROR,
					 get_error_msg(E_PROBLEM_WRITING_FILE),
					 file_name,
					 strerror(errno)) ;
	}

  //ecriture des champs de la classe les &nbsp; sont des " " html
  s_fprintf(class_members,
			"<html>\n<head>\n<title>B Compiler version %s - Classes %s</title>\n</head>\n<body>\n",
			get_bcomp_version(),
			class_name);
  s_fprintf(class_members,
			"<h1><pre>class %s : %s</pre></h1>\n",
			class_name,
			(superclass != NULL) ? superclass->get_class_name() : "");
  s_fprintf(class_members, "<h2><pre>{</pre></h2>\n");

  T_field *cur = get_fields();
  s_fprintf(class_members,"<ul>\n");
  while (cur != NULL)
	{
	  s_fprintf(class_members,
				"<li><pre>%s %s ;</pre></li>\n",
				field_type_name[cur->get_field_type()],
				cur->get_field_name());
	  cur = cur->get_next();
	}

  s_fprintf(class_members,"</ul>\n");
  s_fprintf(class_members, "<h2><pre>};</pre></h2>\n");
  s_fprintf(class_members, "</body>\n</html>\n");

  if (fclose(class_members) != 0)
	{
	  toplevel_error(FATAL_ERROR,
					 get_error_msg(E_PROBLEM_CLOSING_FILE),
					 file_name,
					 strerror(errno)) ;
	}

  // suppression du repertoire dans le nom du fichier pour le lien hypertexte
  // On reutilise file_name qui est assez grand par construction
  sprintf(file_name, "%s.html", class_name);

  // je m'affiche comme un grand ;)
  //format :   <a NAME="1"></a> positionne une marque "1"
  //           <a href="#2">1</a> lien vers la marque "2"
  //           <a href="toto.html">toto</a> lien vers fichier
  s_fprintf(index,
			"<li><a NAME=\"%s\"></a><a href=\"#%s\">%d</a>&nbsp; <a href=\"%s\">%s</a></li>\n",
			class_name,
			(superclass != NULL) ? superclass->get_class_name() : "",
			depth,
			file_name,
			class_name);

  // Affichage recursif de toutes les classes derivees
  T_metaclass *cur_meta = first_derived;
  s_fprintf(index,"<ul>\n");

  while (cur_meta != NULL)
	{
	  cur_meta->print_class_hierarchy(output_path, output_path_len, index) ;
	  cur_meta = cur_meta->get_next_derived() ;
	}

  s_fprintf(index,"</ul>\n");

  // C'est fini
  delete [] file_name ;

  EXIT_TRACE ;
  TRACE1("<< T_metaclass(%s)::print_class_hierarchy", class_name) ;
}

// Retourne la taille la plus grande que peut avoir un champ de la métaclasse
size_t T_metaclass::get_max_field_size(void)
{
  size_t max_field_size = 0;
  for (T_field* field = get_fields(); field != NULL; field = field->get_next()) {
    if (field->get_field_size() > max_field_size) {
      max_field_size = field->get_field_size();
    }
  }
  if (get_superclass() && get_superclass()->get_max_field_size() > max_field_size) {
    max_field_size = get_superclass()->get_max_field_size();
  }
  return max_field_size;
}

// Affichage de la hierarchie des classes du BCOMP
void dump_classes_hierarchy(const char *output_path)
{
  //nom du repertoire + len "index.html"=10 + 1 pour / +1 pour '\0'
  size_t output_path_len = strlen(output_path) ;
  char *path = new char[output_path_len + 11 + 1] ;

  //                        11
  //               12345678901

  sprintf(path, "%s/index.html", output_path);

  // ecriture de l'entete html
  FILE *index = fopen (path, "w");
  if (index == NULL)
	{
	  toplevel_error(FATAL_ERROR,
					 get_error_msg(E_PROBLEM_WRITING_FILE),
					 path,
					 strerror(errno)) ;
	}

  s_fprintf(index,
			"<html>\n<head>\n<title>B Compiler version %s - Classes hierarchy</title>\n</head>\n<body>\n",
			get_bcomp_version());

  for (unsigned int i = 0 ; i < NODE_USER_DEFINED1 ; i++)
	{
	  metaclasses_sot[i]->make_depth() ;
	}

  printf("Creating HTML files in directory %s, entry point is index.html\n",
		 output_path);

  // Affichage a partir de la racine, NODE_OBJECT
  metaclasses_sot[NODE_OBJECT]->print_class_hierarchy(output_path,
													  output_path_len,
													  index);

  s_fprintf(index,"</body>\n</html>");

  if (fclose(index) != 0)
	{
	  toplevel_error(FATAL_ERROR,
					 get_error_msg(E_PROBLEM_CLOSING_FILE),
					 path,
					 strerror(errno)) ;
	}

  delete [] path ;
}

//
// Fin du fichier
//
