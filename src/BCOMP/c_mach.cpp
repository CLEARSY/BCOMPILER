/***************************** CLEARSY **************************************
* Fichier : $Id: c_mach.cpp,v 2.0 2007-06-26 13:15:37 jburlando Exp $
* (C) 2008 CLEARSY
*
* Compilations :	-DDEBUG_FIND pour tracer les recherches de
*					variables/constantes
* 					-DDEBUG_FIND_OP pour tracer les recherches d'operations
*					-DDEBUG_CHECKSUM pour tracer les calculs de checksums
*
* Description :		Classe T_machine
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
RCS_ID("$Id: c_mach.cpp,v 1.60 2007-06-26 13:15:37 jburlando Exp $") ;

/* Includes systeme */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

/* Includes Composant Local */
#include "c_api.h"

static const char * const machine_type_name_sct[] =
{
  "SPECIFICATION",
  "REFINEMENT",
  "IMPLEMENTATION",
  "SYSTEM",
  "MULTI COMPONENT",
} ;

const char *const get_machine_type_name(T_machine_type machine_type)
{
  return machine_type_name_sct[machine_type] ;
}

static const char * const machine_suffix_name_sct[] =
{
  "mch",
  "ref",
  "imp",
#ifdef BALBULETTE
  "mode",
#else
  "sys",
#endif
} ;

const char *const get_machine_suffix_name(T_machine_type machine_type)
{
  return machine_suffix_name_sct[machine_type] ;
}

//
//	}{ Constructeur de la classe T_machine
//
static T_machine *cur_machine_si = NULL ;

T_machine::T_machine(T_machine_type new_machine_type,
							  const char *new_file_name,
							  T_betree *new_betree,
							  T_lexem **adr_ref_lexem,
							  int do_parse)
: T_item(NODE_MACHINE, NULL, NULL, new_betree, new_betree,
		 *adr_ref_lexem)
{
  T_lexem *new_ref_lexem = *adr_ref_lexem ;

  cur_machine_si = this ;
  TRACE6("T_machine(%x)::T_machine(%d : %x:%s, %x, %x)",
		 this,
		 new_machine_type,
		 new_file_name,
		 new_file_name,
		 new_betree,
		 new_ref_lexem) ;
  TRACE1("do_parse=%s", (do_parse == TRUE) ? "TRUE" : "FALSE") ;

  machine_type = new_machine_type ;
  context = NULL ;

  machine_clause = new T_flag(this, new_betree, new_ref_lexem) ;

  T_lexem *cur_lexem = NULL ;

  if (do_parse == TRUE)
	{
	  cur_lexem = new_ref_lexem->get_next_lexem() ;

	  if ( (cur_lexem == NULL) || (cur_lexem->get_lex_type() != L_IDENT) )
		{
		  syntax_unexpected_error(cur_lexem,
								  FATAL_ERROR,
								  get_catalog(C_MACHINE_NAME)) ;
		}

	  // On verifie que le nom du composant est identique
	  // a la racine du nom du fichier
	  char *new_machine_name ; // nom du fichier
	  char *new_machine_name_to_delete = NULL ; // non null si doit
	  // new_machine_name doit etre libere

	  size_t len = clone(&new_machine_name, new_file_name) ;
	  new_machine_name_to_delete = new_machine_name ;

	  char *r = (char *)((size_t)new_machine_name + len) ;

	  type_checked = FALSE ;
	  project_checked = FALSE ;
	  op_analysed = FALSE ;

	  // On cherche le point
#ifdef FULL_TRACE
	  TRACE1("nom complet = \"%s\"", new_machine_name) ;
#endif
	  while ( (r > new_machine_name) && (*r != '.') )
		{
		  r -- ;
		}

	  if (r == new_machine_name)
		{
		  // Nom de fichier sans suffixe !
		  toplevel_error(FATAL_ERROR,
						 get_error_msg(E_FILE_NAME_HAS_NO_SUFFIX),
						 new_machine_name) ;
		}

	  *r = '\0' ;
	  char *suffix = r + 1 ;
	  TRACE1("nom sans point = \"%s\"", new_machine_name) ;
	  TRACE1("suffixe = \"%s\"", suffix) ;

	  // Maintenant on cherche la fin du path
	  while ( (r > new_machine_name) && (*r != '/')
#ifdef WIN32
			  && (*r != '\\')
#endif
			  )
		{
		  r -- ;
		}

	  if (r != new_machine_name)
		{
		  new_machine_name = r + 1 ; // + 1 car on est sur le (anti)slash
		}

	  TRACE1("nom sans path = \"%s\"", new_machine_name) ;

	  // Teste correspondance suffixe de fichier <-> type de composant
	  if ( (strcmp(suffix, "mod") != 0)
		   &&(    ((machine_type==MTYPE_SPECIFICATION) && (strcmp(suffix, "mch") != 0))
				  || ((machine_type==MTYPE_SYSTEM) && (strcmp(suffix, "sys") != 0))
				  || ((machine_type==MTYPE_REFINEMENT) && (strcmp(suffix, "ref") != 0
						  && strcmp(suffix, "sys") != 0))
				  || ((machine_type==MTYPE_IMPLEMENTATION)&&(strcmp(suffix, "imp") != 0))))
		{
		  syntax_error(get_ref_lexem(),
					   CAN_CONTINUE,
					   get_error_msg(E_BAD_COMPONENT_SUFFIX),
					   machine_type_name_sct[machine_type],
					   suffix) ;
		}

	  // En mode multi-composant, il faut demander au lexeme le nom du composant
	  // car il ne peut pas etre deduit du nom de fichier.
	  // Les verifications de correspondance ont lieu au cours de
	  // l'analyse semantique
	  //
	  // On caste en (char *) car on sait qu'on n'ecrira pas
	  new_machine_name =  (char *)cur_lexem->get_value() ;;

	  // Ok, le nom du composant est correct
	  machine_name = new T_ident(new_machine_name,
								 ITYPE_MACHINE_NAME,
								 NULL,
								 NULL,
								 this,			/* father */
								 new_betree,
								 cur_lexem) ;

      free(new_machine_name_to_delete) ;
	}

  new_betree->set_root(this) ;

// On corrige le component_name du betree qui contient
// encore le nom du fichier
if (do_parse == TRUE)
{
new_betree->set_betree_name(machine_name->get_name()) ;
}

  //
  // ATTENTION : on ne peut pas ici mettre first/last pragma a NULL car
  // sinon on ecrase les eventuels pragmas accroches a la machine,
  // recuperes lors de l'appel du constructeur de la classe de base
  // T_item. On est donc oblige de faire confiance au constructeur de
  // T_object qui initialise tous les champs des objets a NULL
  //

abstraction_name = NULL ;
  ref_abstraction = NULL ;
  ref_refinement = NULL ;
  refines_clause = NULL ;
  first_param = last_param = NULL ;
  constraints_clause = NULL ;
  constraints = NULL ;
  sees_clause = NULL ;
  first_sees = last_sees = NULL ;
  includes_clause = NULL ;
  first_includes = last_includes = NULL ;
  imports_clause = NULL ;
  first_imports = last_imports = NULL ;
  promotes_clause = NULL ;
  first_promotes = last_promotes = NULL ;
  extends_clause = NULL ;
  first_extends = last_extends = NULL ;
  uses_clause = NULL ;
  first_uses = last_uses = NULL ;
  sets_clause = NULL ;
  first_set = last_set = NULL ;
  concrete_variables_clause = NULL ;
  first_concrete_variable = last_concrete_variable = NULL ;
  abstract_variables_clause = NULL ;
  first_abstract_variable = last_abstract_variable = NULL ;
  concrete_constants_clause = NULL ;
  first_concrete_constant = last_concrete_constant = NULL ;
  abstract_constants_clause = NULL ;
  first_abstract_constant = last_abstract_constant = NULL ;
  properties_clause = NULL ;
  properties = NULL ;
  invariant_clause = NULL ;
  invariant = NULL ;
  variant_clause = NULL ;
  variant = NULL ;
  assertions_clause = NULL ;
  first_assertion = last_assertion = NULL ;
  initialisation_clause = NULL ;
  initialisation_clause_operation = NULL ;
  initialisation_clause_ident = NULL ;
  first_initialisation = last_initialisation = NULL ;
  values_clause = NULL ;
  first_value = last_value = NULL ;
  operations_clause = NULL ;
  first_operation = last_operation = NULL ;
  end_machine = NULL ;
  first_end_comment = last_end_comment = NULL ;
  first_op_list = last_op_list = NULL ;
  first_dollar_zero_ident = last_dollar_zero_ident = NULL ;

  if (do_parse == FALSE)
	{
return ;
	}

  // Parse la machine
  cur_lexem = syntax_analysis(cur_lexem) ;

  // Verifications

  // Si la machine n'est pas une spec, elle doit avoir une clause
  // refines


// Mise a jour du parametre
*adr_ref_lexem = cur_lexem ;
}

// Destructeur
T_machine::~T_machine(void)
{
}

// Recherche d'une operation dans la liste des operations par son nom,
// avec mise a jour d'une reference sur le T_list_link correspondant
T_generic_op *T_machine::find_op(T_symbol *op_name,
										  T_list_link **adr_ref_link)
{
  TRACE3("T_machine(%x:%s)::find_op(%s)",
		 this, machine_name->get_name()->get_value(),
		 op_name->get_value()) ;

  T_list_link *cur_op = first_op_list ;

#ifdef DEBUG_FIND_OP
  TRACE3("%s(%x)->first_op_list = %x",
		 machine_name->get_name()->get_value(),
		 this,
		 cur_op) ;
#endif

  for (;;)
	{
#ifdef DEBUG_FIND_OP
	  TRACE1("cur_op = %x", cur_op) ;
#endif
	  if (cur_op == NULL)
		{
		  return NULL ;
		}

	  // Element courant et son nom
	  T_object *cur_object = cur_op->get_object() ;
	  ASSERT(    (cur_object->get_node_type() == NODE_OPERATION)
				 || (cur_object->get_node_type() == NODE_USED_OP) ) ;

	  T_symbol *cur_name = ((T_generic_op *)cur_object)->get_op_name() ;
#ifdef DEBUG_FIND_OP
	  TRACE2("cur_name %s op_name %s", cur_name->get_value(), op_name->get_value()) ;
#endif
	  if (cur_name->compare(op_name) == TRUE)
		{
		  // Trouve !
		  // Cast valide par construction
		  if (adr_ref_link != NULL)
			{
			  *adr_ref_link = cur_op ;
			}
		  return (T_generic_op *)cur_object ;
		}

	  cur_op = (T_list_link *)cur_op->get_next() ;
	}
}


// Recherche d'une operation dans la liste des operations par son nom
T_generic_op *T_machine::find_op(T_symbol *op_name)
{
  return find_op(op_name, NULL) ;
}

// Recherche d'une operation dans la liste des operations par son nom,
// avec mise a jour d'une reference sur le T_list_link correspondant
T_op *T_machine::find_local_op(T_symbol *op_name)
{
  TRACE3("T_machine(%x:%s)::find_local_op(%s)",
		 this, machine_name->get_name()->get_value(),
		 op_name->get_value()) ;

  T_op *cur_op = first_local_operation ;

  for (;;)
	{
	  if (cur_op == NULL)
		{
		  return NULL ;
		}

	  T_symbol *cur_name = cur_op->get_op_name() ;
	  if (cur_name->compare(op_name) == TRUE)
		{
		  // Trouve !
		  return cur_op ;
		}

	  cur_op = (T_op *)cur_op->get_next() ;
	}
}

// Recherche d'une constante abstraite dans la liste par son nom
T_ident *T_machine::find_in_list(T_symbol *name,
										  T_ident *list)
{
#ifdef DEBUG_FIND
  TRACE3("T_machine(%s)::find_in_list(%s, %x)",
		 machine_name->get_name()->get_value(),
		 name->get_value(),
		 list) ;
#endif

  T_ident *cur = list ;

  for (;;)
	{
#ifdef DEBUG_FIND
	  TRACE3("cur %x:%s ref %s",
			 cur,
			 (cur == NULL) ? "" : cur->get_name()->get_value(),
			 name->get_value()) ;
#endif

	  if (cur == NULL)
		{
		  // Pas trouve
#ifdef DEBUG_FIND
		  TRACE0("pas trouve") ;
#endif
		  return NULL ;
		}

	  if (cur->get_name()->compare(name) == TRUE)
		{
		  // Trouve
#ifdef DEBUG_FIND
		  TRACE0("trouve !") ;
#endif
		  return cur ;
		}

	  // Au suivant
	  cur = (T_ident *)cur->get_next() ;
	}
}

// Recherche d'une variable/constante abstraite/concrete dans la liste par son nom
T_ident *T_machine::find_abstract_variable(T_symbol *var_name)
{
  return find_in_list(var_name, first_abstract_variable) ;
}
T_ident *T_machine::find_concrete_variable(T_symbol *var_name)
{
  return find_in_list(var_name, first_concrete_variable) ;
}
T_ident *T_machine::find_abstract_constant(T_symbol *var_name)
{
  return find_in_list(var_name, first_abstract_constant) ;
}
T_ident *T_machine::find_concrete_constant(T_symbol *var_name)
{
  return find_in_list(var_name, first_concrete_constant) ;
}

// Fonction auxilaire pour voir si une machine est dans une liste
static int internal_check(T_used_machine *first, T_machine *ref)
{
  TRACE2("internal_check(%x, %x)", first, ref) ;
  T_used_machine *cur = first ;
#if 0
  T_symbol *ref_name = ref->get_machine_name()->get_name() ;
#endif

  for (;;)
	{
	  if (cur == NULL)
		{
		  // Pas trouve
		  TRACE0("pas trouve") ;
		  return FALSE ;
		}

#if 0
	  TRACE2("cur %s ref %s",
			 cur->get_name()->get_name()->get_value(),
			 ref->get_machine_name()->get_name()->get_value()) ;

	  if (cur->get_name()->get_name()->compare(ref_name) == TRUE)
		{
		  // Trouve
		  TRACE0("trouve") ;
		  return TRUE ;
		}
#endif
	  //	TRACE3("cur %x:%x ref %x", cur, cur->get_ref_machine(), ref) ;
	  if (cur->get_ref_machine() == ref)
		{
		  // Trouve
		  TRACE0("trouve") ;
		  return TRUE ;
		}

	  // Encore une fois
	  cur = (T_used_machine *)cur->get_next() ;
	}
}

// Pour savoir si une machine est vue, importee, ...
// rend TRUE si oui, FALSE sinon
int T_machine::get_sees(T_machine *mach)
{
  TRACE4("recherche de %x:%s dans les SEES de %x:%s",
		 mach,
		 mach->get_machine_name()->get_name()->get_value(),
		 this,
		 machine_name->get_name()->get_value()) ;
  return internal_check(first_sees, mach) ;
}

int T_machine::get_includes(T_machine *mach)
{
  TRACE1("recherche de %x dans INCLUDES",
		 mach->get_machine_name()->get_name()->get_value()) ;
  return internal_check(first_includes, mach) ;
}

int T_machine::get_extends(T_machine *mach)
{
  TRACE1("recherche de %x dans EXTENDS",
		 mach->get_machine_name()->get_name()->get_value()) ;
  return internal_check(first_extends, mach) ;
}

int T_machine::get_imports(T_machine *mach)
{
  TRACE1("recherche de %x dans IMPORTS",
		 mach->get_machine_name()->get_name()->get_value()) ;
  return internal_check(first_imports, mach) ;
}

int T_machine::get_uses(T_machine *mach)
{
  TRACE1("recherche de %x dans USES",
		 mach->get_machine_name()->get_name()->get_value()) ;
  return internal_check(first_uses, mach) ;
}

// Pour savoir si une machine definit un identificateur
// Recherche : dans les constantes, les variables et les SETS
T_ident	*T_machine::defines(T_ident *def_ident)
{
  TRACE4("T_machine(%x:%s)::defines(T_ident* (%s*)%s)",
		 this,
		 machine_name->get_name()->get_value(),
		 def_ident->get_class_name(),
		 def_ident->get_name()->get_value()) ;

  switch (def_ident->get_ident_type())
	{
	case ITYPE_CONCRETE_VARIABLE :
	case ITYPE_ABSTRACT_VARIABLE :
	  {
		return find_variable(def_ident->get_name()) ;
	  }

	case ITYPE_CONCRETE_CONSTANT :
	case ITYPE_ABSTRACT_CONSTANT :
	  {
		return find_constant(def_ident->get_name()) ;
	  }

	case ITYPE_ABSTRACT_SET :
	case ITYPE_ENUMERATED_SET :
	  {
	    return find_set(def_ident->get_name()) ;
	  }

	default :
	  {
		// Erreur : le client demande la recherche d'une entite illegale
		TRACE2("erreur: %s est un %s",
			   def_ident,
			   def_ident->get_ident_type_name()) ;
		assert(FALSE) ;
	  }
	}

  return NULL ; // Pour Visual-C++
}

//GP 20/11/98, pour la correction collage des idents renommes.
//Fait la meme chose que define(T_ident*),
//mais renvoi null si la T_renamed_machine associee a this
//n'a pas le meme prefixe que def_ident_renamed
T_ident	*T_machine::defines(T_renamed_ident *def_ident_renamed)
{
  TRACE4("T_machine(%x:%s)::defines(T_renamed_ident* (%s*)%s)",
		 this,
		 machine_name->get_name()->get_value(),
		 def_ident_renamed->get_class_name(),
		 def_ident_renamed->get_name()->get_value()) ;

  //Cette fonction doit etre appele si il existe une machine
  //requise associee
  ASSERT(context != NULL) ;
  T_symbol * machine_prefix = context->get_instance_name() ;

  TRACE6("Compare les prefixes de %s(prefixe %s[%x]) et %s(prefixe : %s[%x])",
		 context->get_name()->get_name()->get_value(),
		 (machine_prefix == NULL)? "null" : machine_prefix->get_value(),
		 (machine_prefix == NULL)? NULL : machine_prefix->get_value(),
		 def_ident_renamed->get_name()->get_value(),
		 def_ident_renamed->get_instance_name()->get_value(),
		 def_ident_renamed->get_instance_name()->get_value()) ;

  if((machine_prefix == NULL) ||
	 (machine_prefix->get_value() !=
		   def_ident_renamed->get_instance_name()->get_value()))
	{
	  //Les prefixes ne correspondent pas
	  return NULL ;
	}


  switch (def_ident_renamed->get_ident_type())
	{
	case ITYPE_CONCRETE_VARIABLE :
	case ITYPE_ABSTRACT_VARIABLE :
	  {

		return  find_variable(def_ident_renamed->get_component_name()) ;
	  }

	case ITYPE_CONCRETE_CONSTANT :
	case ITYPE_ABSTRACT_CONSTANT :
	  {
		//Conformement a EREN, les constantes ne doivent pas etres renommees
		return NULL ;
	  }

	case ITYPE_ABSTRACT_SET :
	case ITYPE_ENUMERATED_SET :
	  {
		//Conformement a EREN, les ensembles ne doivent pas etre renommes
		return NULL ;
	  }

	default :
	  {
		// Erreur : le client demande la recherche d'une entite illegale
		TRACE2("erreur: %s est un %s",
			   def_ident_renamed,
			   def_ident_renamed->get_ident_type_name()) ;
		assert(FALSE) ;
	  }
	}

  return NULL ; // Pour Visual-C++
}

// Obtention de la machine en cours de construction
T_machine *get_cur_machine(void)
{
  return cur_machine_si ;
}

// Recherche d'une variable/constante par son nom, independament
// de sa nature (concrete/abstraite)
T_ident *T_machine::find_constant(T_symbol *const_name)
{
  // On commence par chercher dans les constantes abstraites
#ifdef DEBUG_FIND
  TRACE2("T_machine(%s)::find_constant(%s) -> recherche dans les ctes abstraites",
		 machine_name->get_name()->get_value(),
		 const_name->get_value()) ;
  ENTER_TRACE ;
#endif
  T_ident *res = find_abstract_constant(const_name) ;

  if (res == NULL)
	{
	  // On cherche alors dans les constantes concretes
#ifdef DEBUG_FIND
	  TRACE2("T_machine(%s)::find_constant(%s) -> recherche dans les ctes concretes",
			 machine_name->get_name()->get_value(),
			 const_name->get_value()) ;
#endif
	  res = find_concrete_constant(const_name) ;
	}

#ifdef DEBUG_FIND
  EXIT_TRACE ;
  TRACE2("T_machine::find_constant(%s) -> %x", const_name->get_value(), res) ;
#endif
  return res ;
}

T_ident *T_machine::find_variable(T_symbol *var_name)
{
  // On commence par chercher dans les variables abstraites
#ifdef DEBUG_FIND
  TRACE2("T_machine(%s)::find_variable(%s) -> recherche dans les var abstraites",
		 machine_name->get_name()->get_value(),
		 var_name->get_value()) ;
  ENTER_TRACE ;
#endif
  T_ident *res = find_abstract_variable(var_name) ;

  if (res == NULL)
	{
	  // On cherche alors dans les variables concretes
#ifdef DEBUG_FIND
	  TRACE2("T_machine(%s)::find_variable(%s) -> recherche dans les var concretes",
			 machine_name->get_name()->get_value(),
			 var_name->get_value()) ;
#endif
	  res = find_concrete_variable(var_name) ;
	}

#ifdef DEBUG_FIND
  EXIT_TRACE ;
  TRACE2("T_machine::find_variable(%s) -> %x", var_name->get_value(), res) ;
#endif
  return res ;
}

T_ident *T_machine::find_set(T_symbol *set_name)
{
  // On commence par chercher dans les sets abstraites
#ifdef DEBUG_FIND
  TRACE2("T_machine(%s)::find_set(%s) -> recherche dans les sets",
		 machine_name->get_name()->get_value(),
		 set_name->get_value()) ;
  ENTER_TRACE ;
#endif
  return find_in_list(set_name, first_set) ;
}

// Pour savoir si this raffine spec
// rend TRUE si oui, FALSE sinon
int T_machine::refines(T_machine *spec)
{
  T_machine *cur = ref_abstraction ;

  for (;;)
	{
	  if (cur == NULL)
		{
		  return FALSE ;
		}

	  if (cur == spec)
		{
		  return TRUE ;
		}

	  cur = cur->ref_abstraction ;
	}
}

// Obtention du parametre formel scalaire apres cur_param
// Rend le premier de la liste si cur_param = NULL
T_ident *T_machine::get_scalar_formal_parameter_after(T_ident *cur_param)
{
  TRACE2("T_machine(%s)::get_scalar_formal_parameter_after(%s)",
		 machine_name->get_name()->get_value(),
		 (cur_param == NULL) ? "null" : cur_param->get_name()->get_value()) ;

  if (cur_param == NULL)
	{
	  cur_param = first_param ;
	}
  else
	{
	  cur_param = (T_ident *)cur_param->get_next() ;
	}

  for (;;)
	{
	  if (cur_param == NULL)
		{
		  TRACE0("--> NULL") ;
		  return cur_param ;
		}

	  // Les parametres scalaires ensemblistes aussi
	  T_B0_type *type = cur_param->get_B0_type() ;
	  if (    (type == NULL) // pour pas de crash lors des verifs
		   || (type->is_a_base_type() == TRUE) )
		/* was
	  if (    (cur_param->get_ident_type() == ITYPE_MACHINE_SCALAR_FORMAL_PARAM)
	  || (cur_param->get_B0_type()->is_a_base_type() == TRUE) )*/
		{
 		  TRACE1("--> %s", cur_param->get_name()->get_value()) ;
		  return cur_param ;
		}

	  // La suite
	  cur_param = (T_ident *)cur_param->get_next() ;
	}
}

// Obtention du parametre formel ensembliste apres cur_param
// Rend le premier de la liste si cur_param = NULL
T_ident *T_machine::get_set_formal_parameter_after (T_ident *cur_param)
{
  TRACE2("T_machine(%s)::get_set_formal_parameter_after(%s)",
		 machine_name->get_name()->get_value(),
		 (cur_param == NULL) ? "null" : cur_param->get_name()->get_value()) ;
  if (cur_param == NULL)
	{
	  cur_param = first_param ;
	}
  else
	{
	  cur_param = (T_ident *)cur_param->get_next() ;
	}

  for (;;)
	{
	  if (cur_param == NULL)
		{
		  return NULL ;
		}

	  T_B0_type *type = cur_param->get_B0_type() ;

	  if (    (cur_param == NULL)
		   || (   (cur_param->get_ident_type() == ITYPE_MACHINE_SET_FORMAL_PARAM) )
			   && (type != NULL)
			   && (type->is_a_base_type() == TRUE) )
		{
 		  TRACE1("--> %s",
				 (cur_param == NULL) ? "NULL":cur_param->get_name()->get_value()) ;
		  return cur_param ;
		}

	  // Les parametres scalaires ensemblistes aussi
	  if (type->is_a_base_type() == FALSE)
		{
 		  TRACE3("--> %s (type %s:%s)",
				 cur_param->get_name()->get_value(),
				 make_type_name(cur_param->get_B_type())->get_value(),
				 cur_param->get_B0_type()->get_class_name()) ;
		  return cur_param ;
		}

	  // La suite
	  cur_param = (T_ident *)cur_param->get_next() ;
	}

  return cur_param ;
}

// Ajout d'un identificateur avec $0
void T_machine::add_dollar_zero_ident(T_ident *ident)
{
  TRACE4("T_machine(%x:%s)::add_dollar_zero_ident(%x:%s)",
		 this,
		 machine_name->get_name()->get_value(),
		 ident,
		 ident->get_name()->get_value()) ;

  (void)new T_list_link(ident,
						LINK_DOLLAR_ZERO,
						(T_item **)&first_dollar_zero_ident,
						(T_item **)&last_dollar_zero_ident,
						this,
						get_betree(),
						ident->get_ref_lexem()) ;
}


// Nom de la spec (information semantique)
T_symbol *T_machine::get_spec_machine_symbol_name(void)
{
  return get_specification()->get_machine_name()->get_name() ;
}

// Nom de la spec (information semantique)
const char *T_machine::get_spec_machine_name(void)
{
  return get_spec_machine_symbol_name()->get_value() ;
}

int T_machine::get_spec_machine_name_len(void)
{
  return get_spec_machine_symbol_name()->get_len() ;
}

// Rend TRUE si ref_machine est requise (au premier niveau) par this
static int find_machine_in_list(T_used_machine *list,
										 T_symbol *ref_name)
{
  T_used_machine *cur = list ;
  while (cur != NULL)
	{
	  if (cur->get_name()->get_name() == ref_name)
		{
		  return TRUE ;
		}
	  cur = (T_used_machine *)cur->get_next() ;
	}
  return FALSE ;
}

// Returne un pragma de meme nom que le symbol
// NULL si non trouve
T_pragma *T_machine::lookup_pragma(T_symbol *pragma_name,
											T_pragma *after_pragma)
{
  T_pragma *cur_pragma;
  if (after_pragma == NULL)
	{
	  cur_pragma = get_global_pragmas();
	}
  else
	{
	  cur_pragma = after_pragma->get_next_pragma();
	}
  int found = FALSE;
  while (cur_pragma != NULL && found == FALSE)
    {
      if(cur_pragma->get_name()->compare(pragma_name) == TRUE )
		{
		  found = TRUE;
		}
	  else
		{
		  cur_pragma =  cur_pragma->get_next_pragma();
		}
	}
  return cur_pragma;
}


// Rend TRUE si ref_machine est requise (au premier niveau) par this
int T_machine::needs(T_machine *ref_machine)
{
  T_symbol *ref_name = ref_machine->get_machine_name()->get_name() ;

  if (ref_machine == this)
	{
	  return TRUE ;
	}

  if (refines(ref_machine) == TRUE)
	{
	  return TRUE ;
	}

  if (find_machine_in_list(first_sees, ref_name) == TRUE)
	{
	  return TRUE ;
	}
  if (find_machine_in_list(first_includes, ref_name) == TRUE)
	{
	  return TRUE ;
	}
  if (find_machine_in_list(first_imports, ref_name) == TRUE)
	{
	  return TRUE ;
	}
  if (find_machine_in_list(first_extends, ref_name) == TRUE)
	{
	  return TRUE ;
	}
  if (find_machine_in_list(first_uses, ref_name) == TRUE)
	{
	  return TRUE ;
	}
  return FALSE ;
}

// Calcul du checksum de start_lexem a end_lexem
// Les deux lexemes sont exclus
T_symbol *T_machine::internal_compute_checksum(T_item *start_item,
	bool include_first)
{
  if (start_item == NULL)
	{
	  return NULL ;
	}

  T_lexem *start_lexem = start_item->get_ref_lexem() ;
  if ( (start_lexem == NULL) || (start_lexem->get_next_lexem() == NULL) )
	{
	  return NULL ;
	}

  TRACE1(">> T_machine()::internal_compute_checksum(%x)", start_item) ;
  ENTER_TRACE ;

  // Initialisation du contexte
  MD5Context ctx ;
  MD5Init(&ctx) ;

  // Parcours du flux de lexemes expanses sans commentaires
  // T_lexem *cur_lexem = start_lexem->get_next_lexem() ;

  T_lexem *cur_lexem;

  /* Tue Jun 26 15:08:11 CEST 2007 - jburlando */
  /* Modification pour prendre en compte le premier lexem d'une op�ration */
  if(include_first && start_lexem != NULL)
  {
    #ifdef DEBUG_CHECKSUM
	  TRACE5("prise en compte de %x %s:%d:%d %s",
			 start_lexem,
			 start_lexem->get_file_name()->get_value(),
			 start_lexem->get_file_line(),
			 start_lexem->get_file_column(),
			 (start_lexem->get_value() == NULL)
			 ? start_lexem->get_lex_ascii() : start_lexem->get_value()) ;
#endif

	  // Mise a jour du checksum
	  if (start_lexem->get_value() != NULL)
		{
		  // identificateur
		  MD5Update(&ctx,
					(unsigned char *)start_lexem->get_value(),
					start_lexem->get_value_len()) ;
		}
	  else
		{
		  // Mot-cle
		  MD5Update(&ctx,
					(unsigned char *)start_lexem->get_lex_ascii(),
					start_lexem->get_lex_ascii_len()) ;
		}

  }
  cur_lexem = start_lexem->get_next_lexem() ;
  /* fin modif */

  while (    (cur_lexem != NULL)
		  && ((size_t)cur_lexem->get_tmp2() != get_timestamp()) )
	{
#ifdef DEBUG_CHECKSUM
	  TRACE5("prise en compte de %x %s:%d:%d %s",
			 cur_lexem,
			 cur_lexem->get_file_name()->get_value(),
			 cur_lexem->get_file_line(),
			 cur_lexem->get_file_column(),
			 (cur_lexem->get_value() == NULL)
			 ? cur_lexem->get_lex_ascii() : cur_lexem->get_value()) ;
#endif

	  // Mise a jour du checksum
	  if (cur_lexem->get_value() != NULL)
		{
		  // identificateur
		  MD5Update(&ctx,
					(unsigned char *)cur_lexem->get_value(),
					cur_lexem->get_value_len()) ;
		}
	  else
		{
		  // Mot-cle
		  MD5Update(&ctx,
					(unsigned char *)cur_lexem->get_lex_ascii(),
					cur_lexem->get_lex_ascii_len()) ;
		}

	  // Le suivant : on utilise get_next_lexem pour sauter les commentaires
	  // et parcourir le flux expanse des definitions
	  cur_lexem = cur_lexem->get_next_lexem() ;
	}


  // Finalisation du calcul de la cle
  char MD5_key[33] ;
  unsigned char MD5buf[17] ;
  MD5Final(MD5buf, &ctx) ;
  sprintf(MD5_key,
		  "%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",
		  MD5buf[0], MD5buf[1], MD5buf[2], MD5buf[3],
		  MD5buf[4], MD5buf[5], MD5buf[6], MD5buf[7],
		  MD5buf[8], MD5buf[9], MD5buf[10], MD5buf[11],
		  MD5buf[12], MD5buf[13], MD5buf[14], MD5buf[15]) ;

  T_symbol *checksum = lookup_symbol(MD5_key, 32) ;

  EXIT_TRACE ;
  TRACE2("<< T_machine()::internal_compute_checksum(%x) -> %s",
		 start_lexem,
		 checksum->get_value()) ;

  return checksum ;
}

void SET_TIMESTAMP(T_item *x)
{
  if (x != NULL)
	{
	  x->get_ref_lexem()->set_tmp2(get_timestamp()) ;


	  // Amelioration du 22/02/99 : dans le checksum d'une operation,
	  // ne pas prendre en compte l'eventuel ';' de fin. Pour cela,
	  // quand on analyse une operation, on regarde si elle est
	  // precedee d'un ';' que l'on marque le cas echeant
	  T_lexem *prev = x->get_ref_lexem()->get_prev_lexem() ;
	  if ( (prev != NULL) && (prev->get_lex_type() == L_SCOL) )
		{
		  prev->set_tmp2(get_timestamp()) ;
		}
	  // Fin amelioration 22/02/99


	}
}

// Calcul des checksums
void T_machine::compute_checksums(void)
{
  TRACE1(">> T_machine(%s)::compute_checksums",
		 machine_name->get_name()->get_value()) ;
  ENTER_TRACE ;

  // Mise a jour des tmp2 des lexemes de fin
  next_timestamp() ;

  SET_TIMESTAMP(constraints_clause) ;
  SET_TIMESTAMP(sees_clause) ;
  SET_TIMESTAMP(includes_clause) ;
  SET_TIMESTAMP(imports_clause) ;
  SET_TIMESTAMP(promotes_clause) ;
  SET_TIMESTAMP(extends_clause) ;
  SET_TIMESTAMP(uses_clause) ;
  SET_TIMESTAMP(sets_clause) ;
  SET_TIMESTAMP(concrete_variables_clause) ;
  SET_TIMESTAMP(abstract_variables_clause) ;
  SET_TIMESTAMP(concrete_constants_clause) ;
  SET_TIMESTAMP(abstract_constants_clause) ;
  SET_TIMESTAMP(properties_clause) ;
  SET_TIMESTAMP(invariant_clause) ;
  SET_TIMESTAMP(variant_clause) ;
  SET_TIMESTAMP(assertions_clause) ;
  SET_TIMESTAMP(initialisation_clause) ;
  SET_TIMESTAMP(values_clause) ;
  SET_TIMESTAMP(end_machine) ;

  T_op *cur_op = first_operation ;
  int local_op_done = FALSE ; // pour savoir si on a fait les local.op.

  if (    (cur_op == NULL) // on a fait toutes les operations
		  && (local_op_done == FALSE) ) // on n'a pas encore fait les local.op
	{
	  cur_op = first_local_operation ;
	  local_op_done = TRUE ;
	}

  while (cur_op != NULL)
	{
	  T_item *start = (cur_op->get_out_params() == NULL)
		? (T_item *)cur_op : cur_op->get_out_params() ;
	  SET_TIMESTAMP(start) ;

	  // Amelioration du 22/02/99 : dans le checksum d'une operation,
	  // ne pas prendre en compte l'eventuel ';' de fin. Pour cela,
	  // quand on analyse une operation, on regarde si elle est
	  // precedee d'un ';' que l'on marque le cas echeant
	  T_lexem *prev = start->get_ref_lexem()->get_prev_lexem() ;
	  if ( (prev != NULL) && (prev->get_lex_type() == L_SCOL) )
		{
		  prev->set_tmp2(get_timestamp()) ;
		}
	  // Fin amelioration 22/02/99

	  cur_op = (T_op *)cur_op->get_next() ;

	  if (    (cur_op == NULL) // on a fait toutes les operations
			&& (local_op_done == FALSE) ) // on n'a pas encore fait les local.op
		{
		  cur_op = first_local_operation ;
		  local_op_done = TRUE ;
		}
	}

  // Delimitation speciale des parametres formels
  if (last_param != NULL)
	{
	  T_lexem *end_lexem = last_param->get_ref_lexem()->get_next_lexem() ;
	  end_lexem->set_tmp2(get_timestamp()) ;
	}

  // Calcul des checksums
  formal_params_checksum = internal_compute_checksum(first_param, false) ;
  constraints_clause_checksum = internal_compute_checksum(constraints_clause, false) ;
  sees_clause_checksum = internal_compute_checksum(sees_clause, false) ;
  includes_clause_checksum = internal_compute_checksum(includes_clause, false) ;
  imports_clause_checksum = internal_compute_checksum(imports_clause, false) ;
  promotes_clause_checksum = internal_compute_checksum(promotes_clause, false) ;
  extends_clause_checksum = internal_compute_checksum(extends_clause, false) ;
  uses_clause_checksum = internal_compute_checksum(uses_clause, false) ;
  sets_clause_checksum = internal_compute_checksum(sets_clause, false) ;
  concrete_variables_clause_checksum =
	internal_compute_checksum(concrete_variables_clause, false) ;
  abstract_variables_clause_checksum =
	internal_compute_checksum(abstract_variables_clause, false) ;
  concrete_constants_clause_checksum =
	internal_compute_checksum(concrete_constants_clause, false) ;
  abstract_constants_clause_checksum =
	internal_compute_checksum(abstract_constants_clause, false) ;
  properties_clause_checksum =
	internal_compute_checksum(properties_clause, false) ;
  invariant_clause_checksum =
	internal_compute_checksum(invariant_clause, false) ;
  variant_clause_checksum =
	internal_compute_checksum(variant_clause, false) ;
  assertions_clause_checksum =
	internal_compute_checksum(assertions_clause, false) ;
  values_clause_checksum = internal_compute_checksum(values_clause, false) ;

  cur_op = first_operation ;
  int init_done = FALSE ;
  local_op_done = FALSE ;

  if (cur_op == NULL)
	{
	  cur_op = initialisation_clause_operation ;
	  init_done = TRUE ;

	  if (cur_op == NULL)
		{
		  cur_op = first_local_operation ;
		  local_op_done = TRUE ;
		}
	}

  while (cur_op != NULL)
	{
	  // SL 11/09/99 on n'encode plus l'operation mais juste son
	  // corps, en separant la precondition du reste du body. Cette
	  // methode n'encode plus les parametres d'entree et de sortie,
	  // mais CG dit que ce n'est pas grave car de toutes facons ces
	  // parametres se retrouvent dans l'operation (sauf dans un
	  // raffinement, mais dans ce cas on ne peut pas changer la liste
	  // des parametres sans provoquer d'erreur de type check)
	  //
	  /* was :

	  T_item *start = (cur_op->get_out_params() == NULL)
		? (T_item *)cur_op : cur_op->get_out_params() ;
	  cur_op->set_checksum(internal_compute_checksum(start)) ;
	  */
	  // Debut nouveau code
	  T_precond *precond = NULL ;
	  T_item *body = NULL ;
	  cur_op->get_precond_and_body(&precond, (T_substitution **)&body) ;

	  if (precond != NULL)
		{
#ifdef DEBUG_CHECKSUM
		  TRACE2("on arrete a body = %x %s", body, body->get_class_name()) ;
#endif // DEBUG_CHECKSUM

		  // pour arreter le calcul du checksum de pred
		  SET_TIMESTAMP(precond->get_then_keyword()) ;

		  // pour arreter le calcul du checksum de body
		  SET_TIMESTAMP(precond->get_end_precond()) ;

		  // On triche un peu : en fait internal_compute_checksum
		  // saute le lexeme de l'item en cours donc il faut retourner
		  // au "THEN". Donc dans ce cas la valeur retournee par
		  // get_precond_and_body() est ignoree !
		  body = precond->get_then_keyword() ;

#ifdef DEBUG_CHECKSUM
		  TRACE1(">> CHECKSUM PRECOND %s", cur_op->get_op_name()->get_value()) ;
#endif // DEBUG_CHECKSUM

		  cur_op->set_precond_checksum(internal_compute_checksum(precond, false)) ;

#ifdef DEBUG_CHECKSUM
		  TRACE0("><< CHECKSUM PRECOND") ;
#endif // DEBUG_CHECKSUM

		}
	  else
		{
		  cur_op->set_precond_checksum(NULL) ;
		}

	  // Calcul du checksum du corps
#ifdef DEBUG_CHECKSUM
	  TRACE1(">> CHECKSUM BODY %s", cur_op->get_op_name()->get_value()) ;
#endif // DEBUG_CHECKSUM

	  cur_op->set_body_checksum(internal_compute_checksum(body, true)) ;

#ifdef DEBUG_CHECKSUM
	  TRACE0("<< CHECKSUM BODY") ;
#endif // DEBUG_CHECKSUM

	  // Fin nouveau code
	  cur_op = (T_op *)cur_op->get_next() ;

	  if (cur_op == NULL)
		{
		  if (init_done == FALSE)
			{
			  cur_op = initialisation_clause_operation ;
			  init_done = TRUE ;
			}
		  if ( (cur_op == NULL) && (local_op_done == FALSE) )
			{
			  cur_op = first_local_operation ;
			  local_op_done = TRUE ;
			}
		}
	}

  EXIT_TRACE ;
  TRACE1("<< T_machine(%s)::compute_checksums",
		 machine_name->get_name()->get_value()) ;
}

// Fonction qui enleve les "liens sortant"
// Utile pour que recursive_set_block_state ne rende pas persistant
// des objets qu'on souhaite liberer avec s_memory_clean_up
void T_machine::make_standalone(void)
{
  TRACE0("T_machine::make_standalone(void)");

  T_used_machine *cur = NULL;

  // On coupe tous les liens vers un objet � partir du quel on peut atteindre
  // d'autres machines ou betree.
  ref_abstraction = NULL;
  ref_specification = NULL;
  ref_refinement = NULL;
  first_promotes = NULL;
  last_promotes = NULL;
  first_op_list = NULL;
  last_op_list = NULL;
  context = NULL;

  // On veut garder les T_used_machines qui sont utiles au MS,
  // mais on doit les d�connecter elles aussi
  cur = first_sees;
  while (cur != NULL)
	{
	  cur->make_standalone();
	  cur = (T_used_machine *)cur->get_next();
	}
  cur = first_includes;
  while (cur != NULL)
	{
	  cur->make_standalone();
	  cur = (T_used_machine *)cur->get_next();
	}
  cur = first_imports;
  while (cur != NULL)
	{
	  cur->make_standalone();
	  cur = (T_used_machine *)cur->get_next();
	}
  cur = first_extends;
  while (cur != NULL)
	{
	  cur->make_standalone();
	  cur = (T_used_machine *)cur->get_next();
	}
  cur = first_uses;
  while (cur != NULL)
	{
	  cur->make_standalone();
	  cur = (T_used_machine *)cur->get_next();
	}

  // On deconnecte le betree, car tous les constituants de la machines
  // font reference a lui
  if (this->get_betree() != NULL)
	{
	  this->get_betree()->make_standalone();
	}

  // On coupe les liens herites de T_item
  set_betree(NULL);
  set_father(NULL);
  set_next(NULL);
  set_prev(NULL);
  set_ref_lexem(NULL);
}

//
//	}{	Fin du fichier
//
