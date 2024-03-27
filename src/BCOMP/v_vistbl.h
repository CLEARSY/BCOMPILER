/******************************* CLEARSY **************************************
* Fichier : $Id: v_vistbl.h,v 2.0 2002-05-29 09:09:07 cm Exp $
* (C) 2008 CLEARSY
*
* Description :		Compilateur B
*					Interface des tables de visibilite
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
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA******************************************************************************/
#ifndef _V_VISTBL_H_
#define _V_VISTBL_H_

// Modelisation des entites pouvant etre accedees depuis
// un source B : T_access_entity_type
typedef enum
{
  /* 00 */
  ACCESS_FORMAL_PARAMETER,
  /* 01 */
  ACCESS_ABSTRACT_SET,
  /* 02 */
  ACCESS_ENUMERATED_SET,
  /* 03 */
  ACCESS_ENUMERATED_VALUE,
  /* 04 */
  ACCESS_CONCRETE_CONSTANT,
  /* 05 */
  ACCESS_ABSTRACT_CONSTANT,
  /* 06 */
  ACCESS_CONCRETE_VARIABLE,
  /* 07 */
  ACCESS_ABSTRACT_VARIABLE,
  /* 08 */
  ACCESS_UNREFD_ABSTRACT_CONSTANT_FROM_ABS,
  /* 09 */
  ACCESS_UNREFD_ABSTRACT_VARIABLE_FROM_ABS,
  /* 10 */
  ACCESS_OPERATION,
  /* 11 */
  ACCESS_LOCAL_OPERATION,
  /* 12 */
  ACCESS_IN_PARAMETER,
  /* 13 */
  ACCESS_ANY_QUANTIFIER,
  /*
	Cas particuler : acces que l'on n'a pas besoin de verifier

	CETTE VALEUR DOIT ETRE LA DERNIERE DE LA TABLE !!!
	SINON LES ALGORITHMES NE FONCTIONNENT PLUS

   */
    /* 14 */
  OTHER_ACCESS
} T_access_entity_type ;

// Modelisation de la localisation des entites accedees :
// T_access_entity_location
typedef enum
{
  /* 00 */
  LOC_SAME_SPECIFICATION,
  /* 01 */
  LOC_SAME_REFINEMENT,
  /* 02 */
  LOC_SAME_IMPLEMENTATION,
  /* 03 */
  LOC_REFINEMENT_ABSTRACTION,
  /* 04 */
  LOC_IMPLEMENTATION_ABSTRACTION,
  /* 05 */
  LOC_SPEC_SEEN_BY_SPEC_OR_REF,
  /* 06 */
  LOC_SPEC_SEEN_BY_IMPLEMENTATION,
  /* 07 */
  LOC_SPEC_INCLUDED,
  /* 08 */
  LOC_SPEC_IMPORTED,
  /* 09 */
  LOC_SPEC_USED,
  /* 10 */
  LOC_NON_VISIBLE // entite non visible - code d'erreur
} T_access_entity_location ;

// Contexte des compoants B qui accedent � des identificateurs
// definis ailleurs
typedef enum
{
  /* 00 */
  FROM_CONSTRAINTS,
  /* 01 */
  FROM_INCL_OR_EXT_OR_IMP,
  /* 02 */
  FROM_PROPERTIES,
  /* 03 */
  FROM_VALUES,
  /* 04 */
  FROM_VALUES_AFTER_VALUATION,
  /* 05 */
  FROM_INVARIANT_OR_ASSERTIONS,
  /* 06 */
  FROM_ASSERT_PREDICATE,
  /* 07 */
  FROM_VARIANT_OR_INVARIANT,
  /* 08 */
  FROM_SUBSTITUTION,
  /* 09 */
  FROM_CONCRETE_CONSTANTS,
  /* 10 */
  FROM_CONCRETE_VARIABLES,
  /* 11 */
  FROM_WITNESS_PREDICATE,
  /* 12 */
  FROM_LABEL_PREDICATE,
  /* 13 */
  /*
	CETTE VALEUR DOIT ETRE LA DERNIERE DE LA TABLE !!!
	SINON LES ALGORITHMES NE FONCTIONNENT PLUS

   */
  FROM_LOCAL_OPERATION
} T_access_from_context ;

// Modelisation des autorisations d'acces
typedef enum
{
  /* 00 */
  AUTH_DENIED,
  /* 01 */
  AUTH_READ,
  /* 02 */
  AUTH_READ_WRITE,
  /* 03 */
  AUTH_WRITE,
  /* 04 */
  AUTH_READ_OP,
  /* 05 */
  AUTH_ALL_OP
} T_access_authorisation ;

// Initialisation des tables de visibilite
extern void init_visibility_tables(void) ;

// Lecture des table de visibilite
extern T_access_authorisation get_auth(T_access_entity_type type,
												T_access_entity_location location,
												T_access_from_context context) ;

// Lecture des table de visibilite, avec en retour une chaine qui
// decrit l'autorisation
extern
T_access_authorisation get_auth_with_details(T_access_entity_type type,
											 T_access_entity_location location,
											 T_access_from_context context,
											 char **adr_string) ;

// Dump des tables au format LaTeX 2e
extern void latex2e_dump_visibility_tables(const char *file_name,
													const char *output_path) ;

// Dump des tables au format HTML
extern void html_dump_visibility_tables(const char *file_name,
												 const char *output_path) ;

// Obtention du nom d'un T_access_entity_type
extern const char *get_access_entity_type_name(T_access_entity_type type);

// Obtention du nom d'un T_access_from_context
extern const char *get_access_from_context_name(T_access_from_context ct);

// Obtention du message utilisateur d'un T_access_entity_type
extern const char *get_access_entity_type_msg(T_access_entity_type type);

// Obtention du message utilisateur d'un T_access_authorisation
extern const char *get_access_authorisation_msg(T_access_authorisation a);

// Obtention du message utilisateur d'un T_access_entity_location
extern const char *
get_access_entity_location_msg(T_access_entity_location location);

// Obtention du message utilisateur d'un T_access_from_context
extern const char *get_access_from_context_msg(T_access_from_context ct);

#endif /* _V_VISTBL_H_ */
