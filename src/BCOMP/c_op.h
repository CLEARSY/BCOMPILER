/******************************* CLEARSY **************************************
* Fichier : $Id: c_op.h,v 2.0 2002-09-24 09:37:06 cm Exp $
* (C) 2008 CLEARSY
*
* Description :		Compilateur B
*					Interfaces des operations
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
#ifndef _C_OP_H_
#define _C_OP_H_

#include "c_item.h"
#include "c_expr.h"
#include "c_pred.h"
#include "c_subst.h"
#include "c_genop.h"

class T_symbol ;
class T_ident ;


class T_op : public T_generic_op
{
#ifdef __BCOMP__
  // Des friends pour creer les squelettes
  friend T_betree *create_betree_refinement(T_betree *abstraction,
													 const char *new_name,
													 int create_imp) ;

  friend void add_op(T_generic_op *generic_op,
							  T_betree *betree,
							  T_machine *root,
							  int *cur_line,
							  int *cur_column,
							  T_symbol *sym_fname,
							  T_item **adr_first,
							  T_item **adr_last) ;
#endif // __BCOMP__

  // Nom de l'operation
  T_ident				*name ;

  // Checksum de la precondition de l'operation (NULL si pas de precondition)
  T_symbol 				*precond_checksum ;

  // Checksum du corps l'operation
  T_symbol 				*body_checksum ;

  // Operations raffinees
  T_ident				*first_ref_op ;
  T_ident				*last_ref_op ;

  // Parametres en entree
  T_ident				*first_in_param ;
  T_ident				*last_in_param ;

	// Parametres en sortie
  T_ident				*first_out_param ;
  T_ident				*last_out_param ;

  // Corps
  T_substitution		*first_body ;
  T_substitution		*last_body ;

  // Pour savoir si l'operation a deja ete type-checkee
  int 					type_checked ;

  // Pour savoir si l'operation a deja ete B0 checkee
  int                   B0_checked;

  // Flag de v�rification B0
  int                   B0_body_checked;
  int                   B0_header_checked;

  // op�ration inlin�e
  int                   is_inlined;
  // Pour savoir si l'operation est declaree dans la clause LOCAL_OPERATIONS
  int 					is_a_local_op ;

  // Fonction d'analyse syntaxique
  void syntax_analysis(T_item **adr_first,
								T_item **adr_last,
								T_item *new_father,
								T_betree *new_betree,
								T_lexem **adr_ref_lexem) ;
  // Fonction d'analyse syntaxique des op�rations �ventuellement avec
  // des param�tres typ�s
  void syntax_analysis_BOM_typed(T_item **adr_first,
										  T_item **adr_last,
										  T_item *new_father,
										  T_betree *new_betree,
										  T_lexem **adr_ref_lexem) ;

  // Types check des parametres d'entree/sortie
  void type_check_parameters(T_machine *root) ;

  // Types check de la clause ref d'un evenement
  void  type_check_ref(T_machine *root) ;

  // Fonction auxiliaire de verification de liste
  // On verifie que tous les elements de list1 sont dans list2
  void check_list(T_op *spec_op,
						   T_ident *list_ref,
						   T_ident *list_spec) ;

public :
  T_op() : T_generic_op(NODE_OPERATION) {} ;
  T_op(T_item **adr_first,
	   T_item **adr_last,
	   T_item *new_father,
	   T_betree *new_betree,
	   T_lexem **adr_cur_lexem,
	   int new_is_a_local_op = FALSE) ;
  // Constructeurs pour les clients
  T_op(T_symbol *new_op_name,
	   T_ident *new_first_in_param,
	   T_ident *new_last_in_param,
	   T_ident *new_first_out_param,
	   T_ident *new_last_out_param,
	   T_substitution *new_first_body,
	   T_substitution *new_last_body,
	   T_item **adr_first,
	   T_item **adr_last,
	   T_item *new_father,
	   T_betree *new_betree) ;
  T_op(T_ident *new_op_name,
	   T_ident *new_first_in_param,
	   T_ident *new_last_in_param,
	   T_ident *new_first_out_param,
	   T_ident *new_last_out_param,
	   T_substitution *new_first_body,
	   T_substitution *new_last_body,
	   T_item **adr_first,
	   T_item **adr_last,
	   T_item *new_father,
	   T_betree *new_betree) ;
  virtual ~T_op(void) ;

  // Fonction de typage et de verification de type
  virtual void type_check(void) ;

  // Fonction de verificatio B0
  virtual void B0_check(void);

  // Methode de dump HTML
  virtual FILE *dump_html(void) ;

  // Methodes de lecture
  virtual const char *get_class_name(void)	{ return "T_op" ; }  ;
  virtual T_ident *get_name(void) ;
  virtual T_symbol *get_op_name(void)	;
  virtual T_ident *get_in_params(void) ;
  virtual T_ident *get_out_params(void)	;
  T_substitution *get_body(void)		  	{ return first_body ; } ;
  T_symbol *get_precond_checksum(void)   	{ return precond_checksum ; } ;
  T_symbol *get_body_checksum(void)   		{ return body_checksum ; } ;
  T_ident *get_ref_operation(void)			{ return first_ref_op; } ;
  
  virtual int get_is_inlined(void) const {return is_inlined;}
  void set_is_inlined(int inlined){is_inlined = inlined;}

  int get_is_a_local_op(void) { return is_a_local_op ; } ;
  void set_is_a_local_op(int new_is_a_local_op)
	{ is_a_local_op = new_is_a_local_op ; } ;

  // Recherche recursive d'un identificateur
  virtual T_ident *find_ident(T_ident *name, int root_machine) ;

  // Cherche le T_op associe en parcourant les ref_op eventuels
  // Pour T_op : renvoie this
  virtual T_op *get_definition(void) ;

  // Pour une operation clonee depuis une autre operation, mise a jour
  // recursive des liens pour que les father, ... pointent sur
  // la machine new_machine
  virtual void fix_links(T_machine *new_machine) ;

  // Methode d'ecriture
  void set_precond_checksum(T_symbol *new_precond_checksum)
	{ precond_checksum = new_precond_checksum ; } ;
  void set_body_checksum(T_symbol *new_body_checksum)
	{ body_checksum = new_body_checksum ; } ;
#ifdef __BCOMP__
  void set_body(T_substitution *new_first_body,
						 T_substitution *new_last_body)
	{
	  first_body = new_first_body ;
	  last_body = new_last_body ;
	}
#endif

  //
  // Controles supplementaire pour les operations locales :
  //
  // a) pas de cycle
  // CTRL Ref : SEMLOC3
  void check_local_op_cycle(void) ;
  // b) pas d'appel en // d'op d'une meme instance de machine importee
  // dans une spec d'op loc
  // CTRL Ref : SEMLOC 5
  void check_local_op_calls(void) ;

  // Ajout dans la liste (adr_first, adr_last) des noms d'operations locales
  // appellees (redefinie dans T_op et dans les substitutions)
  // La liste est une liste de T_list_link qui pointent sur des symboles : les
  // noms d'operations
  // cur_op est l'operation depuis laquelle la verification a debute
  // la liste doit etre sans doublon (la methode T_op::add_local_op_call
  // s'occupe de la verification et de l'ajout)
  void build_local_op_call_tree(T_list_link * cur_op_call);

  // Ajout d'un appel d'op loc dans une liste sans doublon
  // la methode verifie l'unicite du nom et emet un message d'erreur le cas echeant
  // L'operation appelle est called_op_name et l'appel est localise en ref_lexem
  void add_local_op_call_list(T_symbol *called_op_name,
									   T_lexem *ref_lexem,
									   T_item **adr_first,
									   T_item **adr_last) ;

  // Ajout dans la liste (adr_first, adr_last) des noms de machines des
  // operations non locales appellees
  // (redefinie dans T_op et dans les substitutions)
  // On ne travaillle que sur les substitutions !!!
  // La liste est une liste de T_list_link qui pointent sur des symboles : les
  // noms de machines
  // cur_op est l'operation depuis laquelle la verification a debute
  // la liste doit etre sans doublon (la methode T_op::add_local_op_call_ref_machine
  // s'occupe de la verification et de l'ajout)
  virtual void build_op_call_ref_machine_list(T_op *ref_op,
													   T_item **adr_first,
													   T_item **adr_last) ;

  // Ajout d'un appel d'op de ref_machine_name dans une liste sans
  // doublon la methode verifie l'unicite du nom et emet un message
  // d'erreur le cas echeant L'operation appelle est provient de
  // ref_machine_name et l'appel est localise en ref_lexem
  void add_op_call_ref_machine_list(T_symbol *ref_machine_name,
											 T_lexem *ref_lexem,
											 T_item **adr_first,
											 T_item **adr_last) ;


  // Est-ce une formule typante ?? oui !
  virtual int is_a_typing_formula(void) ;

  // Calcul de la precondition et du corps de l'operation, avec
  // la semantique du GOP actuel (voir commentaire dans c_op.cpp)
  void get_precond_and_body(T_precond **adr_precond,
									 T_substitution **adr_body) ;

  // Methode qui essaie de trouver un end qui manque
  void check_for_missing_end(void) ;

  // M�thode qui v�rifie le type B0 des param�tres d'entr�e d'op�rations
  void entry_parameter_type_check(void) ;


  // Fonctionn de v�rification B0_OM
  virtual void B0_OM_check_header_decl(T_B0_OM_status expec_sta);
  virtual void B0_OM_check_body_decl();

  // Fonctions utilisateur
  virtual int user_defined_f1(void) ;
  virtual T_object *user_defined_f2(void) ;
  virtual int user_defined_f3(int arg) ;
  virtual T_object *user_defined_f4(T_object *arg) ;
  virtual int user_defined_f5(T_object *arg) ;
} ;

// Fonction qui parse les operations
extern void syntax_get_operations(T_item **adr_first,
										   T_item **adr_last,
										   T_item *father,
										   T_betree *betree,
										   T_lexem **adr_cur_lexem,
										   int parse_local_op = FALSE) ;

#endif /* _C_OP_H_ */


