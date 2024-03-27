/******************************** CLEARSY *************************************
$RCSfile: m_state.h,v $


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

*****************************************************************************/
#ifndef _M_STATE_H_
#define _M_STATE_H_

#include <ostream>
#include <istream>
#include "tinyxml.h"
using namespace std;

// 	Type des contextes
//
typedef enum
{
  CTYPE_MAIN,
  CTYPE_REQUIRED
} T_context_type;


//
//	}{	Classe T_operation_checksums
//
class T_operation_checksums : public T_item
{
  //
  // Structure de donnees

  // Nom de l'operation
  T_symbol                  *operation_name;

  // Nom du composant definissant l'operation
  T_symbol                  *component_name;

  // Checksums de la precondition
  T_symbol                  *precondition_checksum;

  // Checksums du corps de l'operation
  T_symbol					*body_checksum;

  // Checksums des operations appellees dans le corps
  T_operation_checksums		*first_called_operation_checksums;
  T_operation_checksums		*last_called_operation_checksums;

  // Checksums des operations raffinees
  T_operation_checksums		*first_refined_operation_checksums;
  T_operation_checksums		*last_refined_operation_checksums;

  // L'operation a-t-elle ete modifiee ?
  int                       modified;


public :

  // Constructeurs
  T_operation_checksums() : T_item(NODE_OPERATION_CHECKSUMS) {}
  T_operation_checksums(T_generic_op *oper,
						T_machine *comp,
						T_item **adr_first,
						T_item **adr_last,
						T_item *father);

  // Destructeurs
  virtual ~T_operation_checksums() ;

  // Methodes de lecture
  T_symbol *get_operation_name(void) { return operation_name;}
  T_symbol *get_component_name(void) { return component_name;}
  T_symbol *get_precondition_checksum(void) {
    return precondition_checksum;}
  T_symbol *get_body_checksum(void) { return body_checksum;}
  T_operation_checksums *get_called_operation_checksums(void) {
    return first_called_operation_checksums;}
  T_operation_checksums **
  get_adr_first_called_operation_checksums(void) {
    return &first_called_operation_checksums;}
  T_operation_checksums **
  get_adr_last_called_operation_checksums(void) {
    return &last_called_operation_checksums;}
  T_operation_checksums *get_refined_operation_checksums(void) {
    return first_refined_operation_checksums;}
  int is_modified(void) {return modified;}

  // Mise a jour de l'etat d'une operations
  void set_modified(int new_modified) {modified = new_modified;}

  // Comparaison de deux operations, sans comparer les operations raffinees
  // Retourne TRUE si ce sont les meme, FALSE sinon
  int local_compare(T_operation_checksums *oper);

  // Comparaison de deux operations
  // Retourne TRUE si ce sont les meme, FALSE sinon
  int compare(T_operation_checksums *oper);

  friend ostream &operator<<(ostream &os, const T_operation_checksums &sub);
  friend istream &operator>>(istream &is, T_operation_checksums &sub);

  void readOperation(TiXmlElement* operation);
};

//
//	}{	Classe T_component_checksums
//
class T_component_checksums : public T_item
{
  //
  // Structure de donnees

  // Nom du composant
  T_symbol                  *component_name;

  // Type de l'etat
  T_context_type             type;

  // Checksums des differentes clauses
  T_symbol                  *formal_params_checksum;
  T_symbol                  *constraints_clause_checksum;
  T_symbol					*sees_clause_checksum;
  T_symbol					*includes_clause_checksum;
  T_symbol					*imports_clause_checksum;
  T_symbol					*promotes_clause_checksum;
  T_symbol					*extends_clause_checksum;
  T_symbol					*uses_clause_checksum;
  T_symbol					*sets_clause_checksum;
  T_symbol					*concrete_variables_clause_checksum;
  T_symbol					*abstract_variables_clause_checksum;
  T_symbol					*concrete_constants_clause_checksum;
  T_symbol					*abstract_constants_clause_checksum;
  T_symbol					*properties_clause_checksum;
  T_symbol					*invariant_clause_checksum;
  T_symbol					*variant_clause_checksum;
  T_symbol					*assertions_clause_checksum;
  T_symbol					*values_clause_checksum;

  // Checksums des raffinements du composants
  T_component_checksums     *refined_checksums;

  // Checksums des machines requises
  T_component_checksums     *first_seen_checksums;
  T_component_checksums     *last_seen_checksums;
  T_component_checksums     *first_used_checksums;
  T_component_checksums     *last_used_checksums;
  T_component_checksums     *first_included_checksums;
  T_component_checksums     *last_included_checksums;
  T_component_checksums     *first_imported_checksums;
  T_component_checksums     *last_imported_checksums;
  T_component_checksums     *first_extended_checksums;
  T_component_checksums     *last_extended_checksums;

  // Checksums des operations
  T_operation_checksums     *first_operation_checksums;
  T_operation_checksums     *last_operation_checksums;

public :

  // Constructeurs
  T_component_checksums() : T_item(NODE_COMPONENT_CHECKSUMS) {}
  T_component_checksums(T_machine *machine,
						T_context_type new_type,
						T_item **adr_first,
						T_item **adr_last,
						T_item *father);

  // Destructeurs
  virtual ~T_component_checksums() ;

  // Methodes de lecture
  T_symbol *get_component_name(void) {
    return component_name;}
  T_context_type get_type(void) {return type;}
  T_symbol *get_formal_params_checksum(void) {
    return formal_params_checksum;}
  T_symbol *get_constraints_clause_checksum(void) {
    return constraints_clause_checksum;}
  T_symbol *get_sees_clause_checksum(void) {
    return sees_clause_checksum;}
  T_symbol *get_includes_clause_checksum(void) {
    return includes_clause_checksum;}
  T_symbol *get_imports_clause_checksum(void) {
    return imports_clause_checksum;}
  T_symbol *get_promotes_clause_checksum(void) {
    return promotes_clause_checksum;}
  T_symbol *get_extends_clause_checksum(void) {
    return extends_clause_checksum;}
  T_symbol *get_uses_clause_checksum(void) {
    return uses_clause_checksum;}
  T_symbol *get_sets_clause_checksum(void) {
    return sets_clause_checksum;}
  T_symbol *get_concrete_variables_clause_checksum(void) {
    return concrete_variables_clause_checksum;}
  T_symbol *get_abstract_variables_clause_checksum(void) {
    return abstract_variables_clause_checksum;}
  T_symbol *get_concrete_constants_clause_checksum(void) {
    return concrete_constants_clause_checksum;}
  T_symbol *get_abstract_constants_clause_checksum(void) {
    return abstract_constants_clause_checksum;}
  T_symbol *get_properties_clause_checksum(void) {
    return properties_clause_checksum;}
  T_symbol *get_invariant_clause_checksum(void) {
    return invariant_clause_checksum;}
  T_symbol *get_variant_clause_checksum(void) {
    return variant_clause_checksum;}
  T_symbol *get_assertions_clause_checksum(void) {
    return assertions_clause_checksum;}
  T_symbol *get_values_clause_checksum(void) {
    return values_clause_checksum;}
  T_component_checksums *get_refined_checksums(void) {
    return refined_checksums;}
  T_component_checksums *get_seen_checksums(void);
  T_component_checksums *get_used_checksums(void);
  T_component_checksums *get_included_checksums(void) {
    return first_included_checksums;}
  T_component_checksums *get_imported_checksums(void) {
    return first_imported_checksums;}
  T_component_checksums *get_extended_checksums(void) {
    return first_extended_checksums;}
  T_operation_checksums *get_operations_checksums(void) const;

  // Comparaison de deux contexte
  // Renvoie TRUE si les contextes sont les memes, FALSE sinon
  int has_same_context(T_component_checksums *comp_state, bool recurse);

  friend ostream &operator<<(ostream &os, const T_component_checksums &sub);
  friend istream &operator>>(istream &is, T_component_checksums &sub);

  void readXML(TiXmlElement* component);

  void writeBaseClauses(ostream &os) const;
  void readBaseClauses(istream &is);
  void readBaseClauses(TiXmlElement* base);
  void writeNotTransitiveClause(ostream &os, T_component_checksums *startElt, T_component_checksums *endElt) const;
  void readNotTransitiveClause(istream &is, T_component_checksums **startElt, T_component_checksums **endElt);
  void readNotTransitiveClause(TiXmlElement* clause, T_component_checksums **startElt, T_component_checksums **endElt);
};

extern void writeSymbol(ostream &os, const string& pName, T_symbol* pSymbol);
extern void readSymbol(istream &is, const string& pName, T_symbol** pSymbol);
extern void readSymbol(TiXmlElement* symbol, const string& pName, T_symbol** pSymbol);

// Fonctions de parcours des corps des operations pour creer la liste des
// operations appel�es
extern int add_called_op_substitution(T_substitution *subst,
										  T_operation_checksums *op_checksums);
extern int add_called_op_opcall(T_op_call *opcall,
										 T_operation_checksums *op_checksums);
extern int add_called_op_skip(T_substitution *subst,
									   T_operation_checksums *op_checksums);
extern int add_called_op_any(T_any *any,
									  T_operation_checksums *op_checksums);
extern int add_called_op_assert(T_assert *mon_assert,
										 T_operation_checksums *op_checksums);
extern int add_called_op_label(T_label *mon_label,
                                                                                 T_operation_checksums *op_checksums);
extern int add_called_op_jumpif(T_jumpif *mon_witness,
                                                                                 T_operation_checksums *op_checksums);
extern int add_called_op_witness(T_witness *mon_witness,
										 T_operation_checksums *op_checksums);
extern int add_called_op_begin(T_begin *b_begin,
										T_operation_checksums *op_checksums);
extern int add_called_op_case(T_case *b_case,
									   T_operation_checksums *op_checksums);
extern int add_called_op_choice(T_choice *b_choice,
										 T_operation_checksums *op_checksums);
extern int add_called_op_if(T_if *b_if,
									 T_operation_checksums *op_checksums);
extern int add_called_op_let(T_let *b_let,
									  T_operation_checksums *op_checksums);
extern int add_called_op_precond(T_precond *b_pre,
										  T_operation_checksums *op_checksums);
extern int add_called_op_select(T_select *b_select,
										 T_operation_checksums *op_checksums);
extern int add_called_op_var(T_var *b_var,
									  T_operation_checksums *op_checksums);
extern int add_called_op_while(T_while *b_while,
										T_operation_checksums *op_checksums);
#endif /* _M_STATE_H */
