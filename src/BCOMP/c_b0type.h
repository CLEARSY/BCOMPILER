/******************************* CLEARSY **************************************
* Fichier : $Id: c_b0type.h,v 2.0 1999-07-22 15:17:38 sl Exp $
* (C) 2008 CLEARSY
*
* Description :		Compilateur B
*					Description des types "B0"
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
#ifndef _C_B0_TYPE_H_
#define _C_B0_TYPE_H_

class T_expr ;
class T_B0_interval_type ;
class T_B0_enumerated_type ;

// Classe generique, qui modelise un type B0
class T_B0_type : public T_item
{
  // Type B de provenance
  T_type 	*B_type ;

  // Evolution typing_ident : le champ is_a_type_definition vaut TRUE
  // si on est en presence de la definition d'un type
  int		is_a_type_definition ;

public :
  T_B0_type(T_node_type new_node_type) : T_item(new_node_type) {} ;
  T_B0_type(T_node_type new_node_type,
			T_item **adr_first,
			T_item **adr_last,
			T_item *father,
			T_betree *betree,
			T_lexem *ref_lexem) ;
  virtual ~T_B0_type() ;

  // Methodes de lecture
  virtual const char *get_class_name(void) { return "T_B0_type" ; } ;
  T_type *get_B_type(void) { return B_type ; } ;
  int get_is_a_type_definition(void) { return is_a_type_definition ; } ;

  // Methodes d'ecriture
  void set_B_type(T_type *new_B_type) { B_type = new_B_type ; } ;
  void set_is_a_type_definition(int new_is_a_type_definition)
	{ is_a_type_definition = new_is_a_type_definition ; } ;

  // Obtention de la machine de definition du type
  // Non valide pour les T_B0_abstract_type. Pour ce type,
  // appeler la methode recursivement sur les src_types puis
  // sur le dst_type
  virtual T_machine *get_machine_def(void) ;

  // Pour savoir si un type est un type "de base"
  // toujours FALSE sauf pour les T_B0_base_type/T_B0_abstract_type
  virtual int is_a_base_type(void) ;

#ifdef B0C
  // Controle si check_ident est une donne concrete, dans le cas ou le type B0
  // courrant type une donnee concrete
  void check_if_only_concrete_data(T_ident *check_ident);
#endif // B0C

  // Dump HTML
  virtual FILE *dump_html(void) ;

  // Acces au typing_ident
  // En fait les infos sont stockees dans le type, ces methodes sont des raccourcis
  T_ident *get_typing_ident(void) ;
  void set_typing_ident(T_ident *new_typing_ident) ;

  // Fonctions utilisateur
  virtual int user_defined_f1(void) ;
  virtual T_object *user_defined_f2(void) ;
  virtual int user_defined_f3(int arg) ;
  virtual T_object *user_defined_f4(T_object *arg) ;
  virtual int user_defined_f5(T_object *arg) ;
} ;

class T_B0_base_type : public T_B0_type
{
  // Nom
  T_ident				*name ;

  // Type de base
  T_builtin_type		type ;

  // A-t-on un type borne ?
  int					is_bound ;

  // Bornes inferieures et superieurs (si le type est borne)
  // i.e. valables ssi is_bound == TRUE
  T_expr				*min ;
  T_expr				*max ;

  // Cas d'une appartenance a une constante intervalle : ref_type pointe
  // sur la definition type intervalle
  // L'information min/max sera obtenue via cette indirection
  T_B0_interval_type   	*ref_type ;

public :
  T_B0_base_type() : T_B0_type(NODE_B0_BASE_TYPE) {} ;
  T_B0_base_type(T_predefined_type *ref_type,
				 T_ident *new_name,
				 T_item **adr_first,
				 T_item **adr_last,
				 T_item *father,
				 T_betree *betree,
				 T_lexem *ref_lexem) ;
  virtual ~T_B0_base_type() ;

  // Dump HTML
  virtual FILE *dump_html(void) ;

  // Methodes d'acces '
  virtual const char *get_class_name(void) { return "T_B0_base_type" ; } ;
  T_builtin_type get_type(void) { return type ; } ;
  int get_is_bound(void) { return is_bound ; } ;
  T_expr *get_min(void) { return min ; } ;
  T_expr *get_max(void) { return max ; } ;
  T_ident *get_name(void) { return name ; } ;
  T_B0_interval_type *get_ref_type(void) { return ref_type ; } ;

  // Reset des bornes
  void reset_bounds(void) ;

  // Obtention de la machine de definition du type
  virtual T_machine *get_machine_def(void) ;

  // Pour savoir si un type est un type "de base"
  // toujours FALSE sauf pour les T_B0_base_type
  virtual int is_a_base_type(void) ;

  // Fonctions utilisateur
  virtual int user_defined_f1(void) ;
  virtual T_object *user_defined_f2(void) ;
  virtual int user_defined_f3(int arg) ;
  virtual T_object *user_defined_f4(T_object *arg) ;
  virtual int user_defined_f5(T_object *arg) ;
} ;

class T_B0_abstract_type : public T_B0_type
{
  // Nom de l'ensemble abstrait
  T_ident				*name ;

  // Machine de definition de l'ensemble abstrait
  T_machine				*ref_machine ;

  // Dans le cas de la valuation avec un ensemble abstrait
  // ou un ensemble bultin, nom de cet ensemble
  T_ident				*valuation_name ;

  // Dans le cas d'un intervalle, definition de cet intervalle
  T_B0_interval_type   	*ref_interval ;

  // Dans le cas d'un autre ensemble abstrait, definition de cet ensemble
  T_ident				*ref_decla ;

public :
  T_B0_abstract_type() : T_B0_type(NODE_B0_ABSTRACT_TYPE) {} ;
  T_B0_abstract_type(T_abstract_type *ref_type,
					 T_B0_interval_type *new_ref_interval,
					 T_ident *new_ident,
					 T_item **adr_first,
					 T_item **adr_last,
					 T_item *father,
					 T_betree *betree,
					 T_lexem *ref_lexem) ;

  virtual ~T_B0_abstract_type(void) ;

  // Dump HTML
  virtual FILE *dump_html(void) ;

  // Pour savoir si un type est un type "de base"
  // toujours FALSE sauf pour les T_B0_base_type/T_B0_abstract_type
  virtual int is_a_base_type(void) ;

  // Methodes d'acces '
  virtual const char *get_class_name(void)
	{ return "T_B0_abstract_type" ; } ;
  T_ident *get_name(void) { return name ; } ;
  T_machine *get_ref_machine(void) { return ref_machine ; } ;
  T_B0_interval_type *get_ref_interval(void) { return ref_interval ; } ;
  T_ident *get_ref_decla(void) { return ref_decla ; } ;
  T_ident *get_valuation_name(void) { return valuation_name ; } ;

  // Methodes d'ecriture
  void set_ref_machine(T_machine *new_ref_machine)
	{ ref_machine = new_ref_machine ; } ;

  // Obtention de la machine de definition du type
  virtual T_machine *get_machine_def(void) ;

  // Fonctions utilisateur
  virtual int user_defined_f1(void) ;
  virtual T_object *user_defined_f2(void) ;
  virtual int user_defined_f3(int arg) ;
  virtual T_object *user_defined_f4(T_object *arg) ;
  virtual int user_defined_f5(T_object *arg) ;
} ;

class T_B0_enumerated_type : public T_B0_type
{
  // Nom
  T_ident				*name ;

  // Machine de definition de l'ensemble abstrait
  T_machine				*ref_machine ;

  // Valeur enumerees
  T_literal_enumerated_value		*first_enumerated_value ;
  T_literal_enumerated_value		*last_enumerated_value ;

public :
  T_B0_enumerated_type() : T_B0_type(NODE_B0_ENUMERATED_TYPE) {} ;
  T_B0_enumerated_type(T_enumerated_type *ref_type,
					   T_item **adr_first,
					   T_item **adr_last,
					   T_item *father,
					   T_betree *betree,
					   T_lexem *ref_lexem) ;

  virtual ~T_B0_enumerated_type(void) ;

  // Dump HTML
  virtual FILE *dump_html(void) ;

  // Methodes d'acces '
  virtual const char *get_class_name(void)
	{ return "T_B0_enumerated_type" ; } ;
  T_ident *get_name(void) { return name ; } ;
  T_machine *get_ref_machine(void) { return ref_machine ; } ;
  T_literal_enumerated_value *get_values(void)
	{ return first_enumerated_value ; } ;

  // Obtention de la machine de definition du type
  virtual T_machine *get_machine_def(void) ;

  // Fonctions utilisateur
  virtual int user_defined_f1(void) ;
  virtual T_object *user_defined_f2(void) ;
  virtual int user_defined_f3(int arg) ;
  virtual T_object *user_defined_f4(T_object *arg) ;
  virtual int user_defined_f5(T_object *arg) ;
} ;

class T_B0_interval_type : public T_B0_type
{
  // Chainage des interval_type
  T_B0_interval_type   	*prev_itype ;
  T_B0_interval_type   	*next_itype ;

  // Nom de l'intervalle
  T_ident				*name ;

  // Borne inferieure
  T_expr				*min ;
  T_bound				*min_bound ;

  // Borne superieure
  T_expr				*max ;
  T_bound				*max_bound ;

  // reference au type abstrait associe pour le cas
  //d'un sous-ensemble d'ensemble abstrait...
  //CT le 11.07.97
  T_B0_type             *abstract_type ;

public :
  T_B0_interval_type() : T_B0_type(NODE_B0_INTERVAL_TYPE) {} ;


  //constructeur pour set_of( type abstrait ou type predefini)
  T_B0_interval_type(T_setof_type *ref_type,
					 T_B0_type *abst_type,
					 T_ident *name_of_setof,
					 T_item **adr_first,
					 T_item **adr_last,
					 T_item *father,
					 T_betree *betree,
					 T_lexem *ref_lexem) ;

  virtual ~T_B0_interval_type(void) ;

  // Dump HTML
  virtual FILE *dump_html(void) ;

  // Methodes d'acces '
  virtual const char *get_class_name(void)
	{ return "T_B0_interval_type" ; } ;
  T_B0_interval_type *get_prev_itype(void) { return prev_itype ; }  ;
  T_B0_interval_type *get_next_itype(void) { return next_itype ; }  ;
  T_ident *get_name(void) { return name ; }  ;
  T_expr *get_min(void) { return min ; }  ;
  T_bound *get_min_bound(void) { return min_bound ; }  ;
  T_expr *get_max(void) { return max ; }  ;
  T_bound *get_max_bound(void) { return max_bound ; }  ;

  T_B0_type *get_abstract_type(void) { return abstract_type ; }  ;

  // Obtention de la machine de definition du type
  virtual T_machine *get_machine_def(void) ;

  // Methodes d'ecriture
  void set_name(T_ident *new_name) { name = new_name ; } ;

  // Fonctions utilisateur
  virtual int user_defined_f1(void) ;
  virtual T_object *user_defined_f2(void) ;
  virtual int user_defined_f3(int arg) ;
  virtual T_object *user_defined_f4(T_object *arg) ;
  virtual int user_defined_f5(T_object *arg) ;
} ;

class T_B0_array_type : public T_B0_type
{
  // Types sources
  T_B0_type 	   	*first_src_type ;
  T_B0_type		   	*last_src_type ;

  // Type destination
  T_B0_type		   	*dst_type ;

  // Machine
  T_machine			*ref_machine ;

  // Nom unique affecte a la creation
  T_symbol			*name ;

  // Chainage au niveau de la machine
  T_B0_array_type  	*next_decla ;
  T_B0_array_type  	*prev_decla ;

public :
  T_B0_array_type() : T_B0_type(NODE_B0_ARRAY_TYPE) {} ;
  T_B0_array_type(T_setof_type *ref_type,
				  T_ident *ident,
				  T_item **adr_first,
				  T_item **adr_last,
				  T_item *father,
				  T_betree *betree,
				  T_lexem *ref_lexem) ;
  virtual ~T_B0_array_type() ;

  // Dump HTML
  virtual FILE *dump_html(void) ;

  // Methodes d'acces '
  virtual const char *get_class_name(void)
	{ return "T_B0_array_type" ; } ;
  T_B0_type *get_src_types(void) { return first_src_type ; }  ;
  T_B0_type *get_first_src_type(void) { return first_src_type ; }  ;
  T_B0_type	*get_last_src_type(void) { return last_src_type ; }  ;
  T_B0_type	*get_dst_type(void) { return dst_type ; }  ;
  T_machine	*get_ref_machine(void) { return ref_machine ; }  ;
  T_symbol *get_name(void) { return name ; }  ;
  T_B0_array_type *get_next_decla(void) { return next_decla ; }  ;
  T_B0_array_type *get_prev_decla(void) { return prev_decla ; }  ;

  // Fonctions utilisateur
  virtual int user_defined_f1(void) ;
  virtual T_object *user_defined_f2(void) ;
  virtual int user_defined_f3(int arg) ;
  virtual T_object *user_defined_f4(T_object *arg) ;
  virtual int user_defined_f5(T_object *arg) ;
} ;

// Type record
class T_B0_label_type : public T_B0_type
{
  // Nom du label
  T_ident	*name ;

  // Type associe
  T_B0_type	*type ;

public:
  T_B0_label_type(void) : T_B0_type(NODE_B0_LABEL_TYPE) {} ;
  T_B0_label_type(T_label_type *ref_type,
				  T_ident *ref_ident,
				  T_item **adr_first,
				  T_item **adr_last,
				  T_item *father,
				  T_betree *betree,
				  T_lexem *ref_lexem) ;
  virtual ~T_B0_label_type(void) ;

  // Dump HTML
  virtual FILE *dump_html(void) ;

  // Methodes d'acces '
  virtual const char *get_class_name(void) { return "T_B0_label_type" ;};
  T_ident *get_name(void) { return name ; } ;
  T_B0_type *get_type(void) { return type ; } ;

  // Fonctions utilisateur
  virtual int user_defined_f1(void) ;
  virtual T_object *user_defined_f2(void) ;
  virtual int user_defined_f3(int arg) ;
  virtual T_object *user_defined_f4(T_object *arg) ;
  virtual int user_defined_f5(T_object *arg) ;
} ;

// Type record en B0
class T_B0_record_type : public T_B0_type
{
  // Les types
  T_B0_label_type		*first_label ;
  T_B0_label_type		*last_label ;

  // Machine
  T_machine			*ref_machine ;

  // Nom unique affecte a la creation
  T_symbol			*name ;

  // Chainage au niveau de la machine
  T_B0_record_type  	*next_decla ;
  T_B0_record_type  	*prev_decla ;

public:
  T_B0_record_type(void) : T_B0_type(NODE_B0_RECORD_TYPE) {} ;
  T_B0_record_type(T_record_type *ref_type,
				   T_ident *ref_ident,
				   T_item **adr_first,
				   T_item **adr_last,
				   T_item *father,
				   T_betree *betree,
				   T_lexem *ref_lexem) ;
  virtual ~T_B0_record_type(void) ;

  // Dump HTML
  virtual FILE *dump_html(void) ;

  // Methodes d'acces '
  virtual const char *get_class_name(void) { return "T_B0_record_type" ;};
  T_B0_label_type *get_labels(void) { return first_label ; }  ;
  T_machine	*get_ref_machine(void) { return ref_machine ; }  ;
  T_symbol *get_name(void) { return name ; }  ;
  T_B0_record_type *get_next_decla(void) { return next_decla ; }  ;
  T_B0_record_type *get_prev_decla(void) { return prev_decla ; }  ;

  // Fonctions utilisateur
  virtual int user_defined_f1(void) ;
  virtual T_object *user_defined_f2(void) ;
  virtual int user_defined_f3(int arg) ;
  virtual T_object *user_defined_f4(T_object *arg) ;
  virtual int user_defined_f5(T_object *arg) ;
} ;

#endif /* _C_B0_TYPE_H_ */

