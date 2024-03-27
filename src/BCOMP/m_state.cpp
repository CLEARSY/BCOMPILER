/******************************* CLEARSY **************************************
* Fichier : $Id: m_state.cpp,v 2.0 2001-09-19 15:07:53 lv Exp $
* (C) 2008 CLEARSY
*
* Description :		Parcours de la liste des operations appelees
*					Checksums des parametres formels de la machine
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
/* Includes systeme */
#include "c_port.h"

/* Includes bibliotheques */
#include "c_api.h"

/* Includes locaux */
#include "m_state.h"
#include "m_msg.h"
#include "tinystr.h"

#include <iostream>
#include <sstream>
#include <string>

//
//	}{	Classe T_operation_checksums
//


T_operation_checksums::T_operation_checksums(T_generic_op *oper,
													  T_machine *comp,
													  T_item **adr_first,
													  T_item **adr_last,
													  T_item *father)
  : T_item(NODE_OPERATION_CHECKSUMS, adr_first, adr_last, father, NULL, NULL)
{
  TRACE5("T_operation_checksums(%x)::T_operation_checksums(%x:%s, %x:%s)",
		 this, oper, oper->get_op_name()->get_value(),
		 comp, comp->get_machine_name()->get_name()->get_value());
  TRACE1("L'operation est %s",
		 (oper->get_node_type() == NODE_OPERATION ? "propre" : "promue"));

  // Parcours des abstractions du composant
  T_machine *abst_mach = NULL;
  // Abstraction de l'operation
  T_generic_op *abst_op = NULL;
  // Definition de l'operation
  T_op *def_op = oper->get_definition();
  // Corps de la definition de l'operation
  T_substitution *body = def_op->get_body();
  // est-ce la clause INITILISATION ?
  int is_initialisation = FALSE;
  // est-ce une op�ration locale ?
  int is_local = FALSE;
  // Parcours des machines incluses ou importees par le composant de def
  T_used_machine *u_mach = NULL;

  operation_name = oper->get_op_name();
  component_name = comp->get_machine_name()->get_name();

  // Est-ce la clause INITIALISATION ?
  is_initialisation = operation_name->compare("INITIALISATION", 14);

  //CLB : Est-ce une op�ration locale ?
  is_local = (comp->find_local_op(operation_name) == NULL) ? FALSE : TRUE;

  // Par defaut on initialise a TRUE :
  // Lors du premier traitement d'un composant, toutes ses operations sont
  // modifiees. La methode set_modified_op_list met ce champs a jour
  modified = TRUE;

  // checksum de la precondition
  precondition_checksum = def_op->get_precond_checksum();
  TRACE1("precondition_checksum = %x", precondition_checksum);

  // checksum du corps
  body_checksum = def_op->get_body_checksum();
  TRACE1("body_checksum = %x", body_checksum);

  // checksum des operations appelees
  // Parcours du corps de l'operation pour ajouter les operations
  // appel�es
  first_called_operation_checksums = NULL;
  last_called_operation_checksums = NULL;
  first_refined_operation_checksums = NULL;
  last_refined_operation_checksums = NULL;
  while (body != NULL)
	{
	  body->user_defined_f5((T_object *)this);
	  body = (T_substitution *)body->get_next();
	}

  // Cas de l'initialisation : On doit ajouter les initialisations des
  // machines incluses ou importees
  if (is_initialisation == TRUE)
	{
	  u_mach = comp->get_includes();
	  while (u_mach != NULL)
		{
		  if (u_mach->get_ref_machine()
			  ->get_initialisation_clause_operation() != NULL)
			{
			  new T_operation_checksums(
								 u_mach->get_ref_machine()->
								        get_initialisation_clause_operation(),
								 u_mach->get_ref_machine(),
								 (T_item **)&first_called_operation_checksums,
								 (T_item **)&last_called_operation_checksums,
								 this);
			}
		  u_mach = (T_used_machine *)u_mach->get_next();
		}
	  u_mach = comp->get_imports();
	  while (u_mach != NULL)
		{
		  if (u_mach->get_ref_machine()
			  ->get_initialisation_clause_operation() != NULL)
			{
			  new T_operation_checksums(
								 u_mach->get_ref_machine()->
										get_initialisation_clause_operation(),
								 u_mach->get_ref_machine(),
								 (T_item **)&first_called_operation_checksums,
								 (T_item **)&last_called_operation_checksums,
								 this);
			}
		  u_mach = (T_used_machine *)u_mach->get_next();
		}
	}

  // Checksums de l'abstraction
  TRACE0("Recuperation de l'abstraction eventuelle");
  //CLB : pour une op�ration locale
  if ( is_local == TRUE )
	{
	  if( def_op->get_is_a_local_op() )
		{
		  //pour l'abstraction
		  abst_op = NULL;
		}
	  else
		{
		  //pour l'impl�mentation
		  abst_op = comp->find_local_op(operation_name);
		  abst_mach = comp;
		}
	}
   else
	{
	  abst_mach = comp->get_ref_abstraction();
	  if ( abst_mach != NULL )
		{
		  TRACE2("Abstraction : %x:%s",
				 abst_mach,
				 abst_mach->get_machine_name()->get_name()->get_value());
		  if ( is_initialisation == TRUE )
			{
			  abst_op = abst_mach->get_initialisation_clause_operation();
			}
		  else
			{
			  while ( abst_mach != NULL && abst_op == NULL )
				{
				  // Premier passage : on recupere l'abstraction de l'operation
				  // Si celle ci est nulle, c'est qu'implicitement celle de
				  // l'etape precedente a ete recopiee. On doit donc
				  // la recuperer.
				  abst_op = abst_mach->find_op(operation_name);
				  TRACE1("Abstraction de l'operation : %x:",abst_op);
				  if ( abst_op == NULL )
					{
					  abst_mach = abst_mach->get_ref_abstraction();
					}
				}
			}
		}
	}
  TRACE1("Son abstraction est : %x", abst_op);
  if ( abst_op != NULL )
	{
	  TRACE1("La definition de son abstraction est : %x", abst_op);
	  new T_operation_checksums(abst_op,
			  abst_mach,
			  (T_item **)&first_refined_operation_checksums,
			  (T_item **)&last_refined_operation_checksums,
			  this);
	}
//  else
//	{
//	  TRACE0("Pas d'abstraction");
//	  refined_operation_checksums = NULL;
//	}
  if ( is_local == FALSE &&  is_initialisation == FALSE) {
	  T_ident *ref_op = def_op->get_ref_operation();
	  while (ref_op) {
		  TRACE1("Abstraction de l'operation ref : %s",ref_op->get_name()->get_value());
		  abst_op = NULL;
		  abst_mach = comp->get_ref_abstraction();
		  TRACE2("Abstraction de l'operation ref : %s:%x",ref_op->get_name()->get_value(),
				  abst_mach);
		 		  if ( abst_mach != NULL )
		  {
			  TRACE2("Abstraction ref: %x:%s",
					  abst_mach,
					  abst_mach->get_machine_name()->get_name()->get_value());
			  while ( abst_op == NULL )
			  {
				  // Premier passage : on recupere l'abstraction de l'operation
				  // Si celle ci est nulle, c'est qu'implicitement celle de
				  // l'etape precedente a ete recopiee. On doit donc
				  // la recuperer.
				  abst_op = abst_mach->find_op(ref_op->get_name());
				  TRACE1("Abstraction de l'operation : %x:",abst_op);
				  if ( abst_op == NULL )
				  {
					  abst_mach = abst_mach->get_ref_abstraction();
				  }

			  }
		  }
		  if ( abst_op != NULL )
		  {
			  TRACE1("La definition de son abstraction est : %x", abst_op);
			  new T_operation_checksums(abst_op,
					  abst_mach,
					  (T_item **)&first_refined_operation_checksums,
					  (T_item **)&last_refined_operation_checksums,
					  this);
		  }

		  ref_op = (T_ident *) ref_op->get_next();
	  }
  }
}

// Destructeurs
T_operation_checksums::~T_operation_checksums(void)
{
  object_unlink(operation_name);
  object_unlink(component_name);
  object_unlink(body_checksum);
  list_unlink(first_called_operation_checksums);
  list_unlink(first_refined_operation_checksums);
}

// Comparaison de deux symbols
static int symbol_compare(T_symbol *symb1, T_symbol *symb2)
{
  TRACE2("symbol_compare(%s, %s)",
		 symb1 == NULL ? "(null)" : symb1->get_value(),
		 symb2 == NULL ? "(null)" : symb2->get_value());
  if (symb1 == NULL)
	{
	  return (symb2 == NULL);
	}
  if (symb2 == NULL)
	{
	  return FALSE;
	}
  return symb1->compare(symb2);
}

// Comparaison de deux listes de checksums
static int list_op_checksums_compare(T_operation_checksums *check1,
									 T_operation_checksums *check2)
{
  TRACE2("list_op_checksum_compare(%x, %x)", check1, check2);

  T_operation_checksums *cur_check1 = check1;
  T_operation_checksums *cur_check2 = check2;
  int res = TRUE;

  if (check1 == NULL)
	{
		TRACE1("check2 :(%x)", check2) ;
		TRACE2("retour :(%d), true :(%d)", (check2 == NULL), TRUE) ;
		return (check2 == NULL);
	}

  while ((res == TRUE) && (cur_check1 != NULL) && (cur_check2 != NULL))
	{
	  res = cur_check1->compare(cur_check2);
	  TRACE2("Comparaison de %s  --> %d",
			 cur_check1->get_operation_name()->get_value(), res);
	  cur_check1 = (T_operation_checksums *)cur_check1->get_next();
	  cur_check2 = (T_operation_checksums *)cur_check2->get_next();
	}

  if ((res == TRUE) && (cur_check1 == NULL) && (cur_check2 == NULL))
	{
	  // Les deux listes avaient le meme nombre d'elements qui sont
	  //tous identiques
	  return TRUE;
	}
  // Sinon soit on a trouve une difference (res == FALSE) soit les deux listes
  // n'ont pas le meme d'elements (cur_check1 != NULL ou cur_check2 != NULL)
  return FALSE;
}

// Comparaison locale de deux listes de checksums
static int list_op_checksums_local_compare(T_operation_checksums *check1,
									 T_operation_checksums *check2)
{
  TRACE2("list_op_checksum_compare(%x, %x)", check1, check2);

  T_operation_checksums *cur_check1 = check1;
  T_operation_checksums *cur_check2 = check2;
  int res = TRUE;

  if (check1 == NULL)
	{
		TRACE1("check2 :(%x)", check2) ;
		TRACE2("retour :(%d), true :(%d)", (check2 == NULL), TRUE) ;
		return (check2 == NULL);
	}

  while ((res == TRUE) && (cur_check1 != NULL) && (cur_check2 != NULL))
	{
	  res = cur_check1->local_compare(cur_check2);
	  TRACE2("Comparaison de %s  --> %d",
			 cur_check1->get_operation_name()->get_value(), res);

	  cur_check1 = (T_operation_checksums *)cur_check1->get_next();
	  cur_check2 = (T_operation_checksums *)cur_check2->get_next();
	}

  if ((res == TRUE) && (cur_check1 == NULL) && (cur_check2 == NULL))
	{
	  // Les deux listes avaient le meme nombre d'elements qui sont
	  //tous identiques
	  return TRUE;
	}
  // Sinon soit on a trouve une difference (res == FALSE) soit les deux listes
  // n'ont pas le meme d'elements (cur_check1 != NULL ou cur_check2 != NULL)
  return FALSE;
}

// Comparaison de deux operations, sans comparer les operations raffinees
// Retourne TRUE si ce sont les meme, FALSE sinon
int T_operation_checksums::local_compare(T_operation_checksums *oper)
{
  TRACE2("T_operation_checksums(%x)::local_compare(%x)", this, oper);

  if ((symbol_compare(operation_name, oper->get_operation_name()) == FALSE) ||
	  (symbol_compare(component_name, oper->get_component_name()) == FALSE) ||
	  (symbol_compare(precondition_checksum, oper->get_precondition_checksum())
	   == FALSE) ||
	  (symbol_compare(body_checksum, oper->get_body_checksum()) == FALSE) ||
	  (list_op_checksums_compare(first_called_operation_checksums,
								 oper->get_called_operation_checksums())
	   == FALSE))
	{
	  TRACE1("T_operation_checksums(%s)::local_compare--> FALSE",
			  oper->get_operation_name()->get_value());
	  return FALSE;
	}
  TRACE0("T_operation_checksums::local_compare--> TRUE");
  return TRUE;
}

// Comparaison de deux operations
// Retourne TRUE si ce sont les meme, FALSE sinon
int T_operation_checksums::compare(T_operation_checksums *oper)
{
  TRACE2("T_operation_checksums(%x)::compare(%x)", this, oper);

  ASSERT(oper != NULL);
   T_operation_checksums *abst_op = NULL;
  T_operation_checksums *abst_op2 = oper->get_refined_operation_checksums();

  if (local_compare(oper) == FALSE)
	{
	  TRACE1("T_operation_checksums(%s)::compare--> FALSE",
			 operation_name->get_value());
	  return FALSE;
	}
  if (list_op_checksums_local_compare(first_refined_operation_checksums,
 								 oper->get_refined_operation_checksums()) == FALSE) {
	  TRACE1("T_operation_checksums(%s)::compare--> FALSE",
			 operation_name->get_value());
	  return FALSE;

   }

//  if ((refined_operation_checksums == NULL) && (abst_op2 == NULL))
//	{
//	  // Pas de machine raffinee, on a fini
//	  TRACE1("T_operation_checksums(%s)::compare--> TRUE",
//			 operation_name->get_value());
//	  return TRUE;
//	}
//  if ((refined_operation_checksums == NULL) || (abst_op2 == NULL))
//	{
//	  TRACE0("On n'a pas la meme architecture");
//	  TRACE1("T_operation_checksums(%s)::compare--> FALSE",
//			 operation_name->get_value());
//	  return FALSE;
//	}
//
//  // On compare l'operation raffinee
//  if (refined_operation_checksums->local_compare(abst_op2) == FALSE)
//	{
//	  // L'operation raffinee est differente
//	  TRACE1("T_operation_checksums(%s)::compare--> FALSE",
//			 operation_name->get_value());
//	  return FALSE;
//	}
  if ((first_refined_operation_checksums != NULL) && (abst_op2 != NULL)) {
  // On compare les preconditions des autres abstractions
  abst_op = first_refined_operation_checksums->get_refined_operation_checksums();
  abst_op2 = abst_op2->get_refined_operation_checksums();

  while(abst_op != NULL)
	{
	  if (abst_op2 == NULL)
		{
		  TRACE0("On n'a pas la meme architecture de l'abstraction");
		  return FALSE;
		}
	  if (symbol_compare(abst_op->get_precondition_checksum(),
						 abst_op2->get_precondition_checksum()) == FALSE)
		{
		  // Ils n'ont pas la meme precondition
		  TRACE1("T_operation_checksums(%s)::compare--> FALSE",
				 operation_name->get_value());
		  return FALSE;
		}
	  abst_op = abst_op->get_refined_operation_checksums();
	  abst_op2 = abst_op2->get_refined_operation_checksums();
	}
  }
  TRACE0("T_operation_checksums::compare--> TRUE");
  return TRUE;
}


ostream &operator<<(ostream &os, const T_operation_checksums &sub)
{
    os << "<operation>" << endl;
    writeSymbol(os, "NAME", sub.operation_name);
    writeSymbol(os, "COMPONENT", sub.component_name);
    writeSymbol(os, "PRECONDITION", sub.precondition_checksum);
    writeSymbol(os, "BODY", sub.body_checksum);
    os << "<called>" << endl;
    T_operation_checksums* lOpeCheck = sub.first_called_operation_checksums;
    while(lOpeCheck && (lOpeCheck != sub.last_called_operation_checksums))
    {
        os << *lOpeCheck << endl;
        lOpeCheck = (T_operation_checksums*)lOpeCheck->get_next();
    }
    if(sub.last_called_operation_checksums)
    {
        os << *sub.last_called_operation_checksums << endl;
    }
    os << "</called>" << endl;
    os << "<refined>" << endl;
    lOpeCheck = sub.first_refined_operation_checksums;
    while(lOpeCheck && (lOpeCheck != sub.last_refined_operation_checksums))
    {
        os << *lOpeCheck << endl;
        lOpeCheck = (T_operation_checksums*)lOpeCheck->get_next();
    }
    if(sub.last_refined_operation_checksums)
    {
        os << *sub.last_refined_operation_checksums << endl;
    }
    os << "</refined>" << endl;
    os << "</operation>" << endl;

    return os;
}

/** DO NOT USE: bug with mingw, function tellg move the stream position**/
istream &operator>>(istream &is, T_operation_checksums &sub)
{
    string lCurStr;
    istream::pos_type lPos = is.tellg();
    is >> lCurStr;
    if(lCurStr == "<operation>")
    {
        readSymbol(is, "NAME", &sub.operation_name);
        readSymbol(is, "COMPONENT", &sub.component_name);
        readSymbol(is, "PRECONDITION", &sub.precondition_checksum);
        readSymbol(is, "BODY", &sub.body_checksum);
        T_operation_checksums* lCurrOp;
        is >> lCurStr; // "<called>"
        lPos = is.tellg();
        is >> lCurStr; // "</called>" ou <operation>
        while(lCurStr != "</called>")
        {
            is.seekg(lPos);
            lCurrOp = new T_operation_checksums();
            is >> *lCurrOp;
            lCurrOp->tail_insert((T_item**) &sub.first_called_operation_checksums, (T_item**) &sub.last_called_operation_checksums);
            lPos = is.tellg();
            is >> lCurStr; // "</called>" ou <operation>
        }
        is >> lCurStr; // "<refined>"
        lPos = is.tellg();
        is >> lCurStr; // "</refined>" ou <operation>
        while(lCurStr != "</refined>")
        {
            is.seekg(lPos);
            lCurrOp = new T_operation_checksums();
            is >> *lCurrOp;
            lCurrOp->tail_insert((T_item**) &sub.first_refined_operation_checksums, (T_item**) &sub.last_refined_operation_checksums);
            lPos = is.tellg();
            is >> lCurStr; // "</refined>" ou <operation>
        }
        is >> lCurStr; // "</operation>"
    }
    else
    {
        is.seekg(lPos);
    }

    return is;
}

void T_operation_checksums::readOperation(TiXmlElement* operation) {

    if(operation)
    {
        TiXmlElement* symbol = operation->FirstChildElement("symbol");
        readSymbol(symbol, "NAME", &operation_name);
        symbol = symbol->NextSiblingElement("symbol");
        readSymbol(symbol, "COMPONENT", &component_name);
        symbol = symbol->NextSiblingElement("symbol");
        readSymbol(symbol, "PRECONDITION", &precondition_checksum);
        symbol = symbol->NextSiblingElement("symbol");
        readSymbol(symbol, "BODY", &body_checksum);

        T_operation_checksums* lCurrOp;
        if(operation->FirstChildElement("called"))
        {
            TiXmlElement* inop = operation->FirstChildElement("called")->FirstChildElement("operation");
            while(inop) {
                lCurrOp = new T_operation_checksums();
                lCurrOp->readOperation(inop);
                lCurrOp->tail_insert((T_item**) &first_called_operation_checksums, (T_item**) &last_called_operation_checksums);
                inop = inop->NextSiblingElement("operation");
            }
        }
        if(operation->FirstChildElement("refined"))
        {
            TiXmlElement* inop = operation->FirstChildElement("refined")->FirstChildElement("operation");
            while(inop) {
                lCurrOp = new T_operation_checksums();
                lCurrOp->readOperation(inop);
                lCurrOp->tail_insert((T_item**) &first_refined_operation_checksums, (T_item**) &last_refined_operation_checksums);
                inop = inop->NextSiblingElement("operation");
            }
        }
    }
}

//
//	}{	Classe T_component_checksums
//
T_component_checksums::T_component_checksums(T_machine *machine,
													  T_context_type new_type,
													  T_item **adr_first,
													  T_item **adr_last,
													  T_item *father)
  : T_item(NODE_COMPONENT_CHECKSUMS, adr_first, adr_last, father, NULL, NULL)
{
  TRACE4("T_component_checksums(%x)::T_component_checksums(%x:%s, %d)",
		 this, machine, machine->get_machine_name()->get_name()->get_value(),
		 type);

  // parcours des machines requises
  T_used_machine *tmp_mach = NULL;
  // abstraction de machine
  T_machine *abst_mach = machine->get_ref_abstraction();
   //parcours des operations propres et promues
  T_list_link *elem_list = NULL;
  T_generic_op *cur_op = NULL;
  T_generic_op *cur_op1 = NULL;
  T_symbol *op_name = NULL;
  T_machine *tmp1_mach = NULL;
  T_list_link *tmp_elem_list = NULL;
 int ajout = FALSE;

  component_name = machine->get_machine_name()->get_name();
  type = new_type;

 // initialisation des champs contenant les checksums des clauses
  TRACE0("initialisation des clauses");
  formal_params_checksum = machine->get_formal_params_checksum();
  constraints_clause_checksum = machine->get_constraints_clause_checksum();
  sees_clause_checksum = machine->get_sees_clause_checksum();
  includes_clause_checksum = machine->get_includes_clause_checksum();
  imports_clause_checksum = machine->get_imports_clause_checksum();
  promotes_clause_checksum = machine->get_promotes_clause_checksum();
  extends_clause_checksum = machine->get_extends_clause_checksum();
  uses_clause_checksum = machine->get_uses_clause_checksum();
  sets_clause_checksum = machine->get_sets_clause_checksum();
  concrete_variables_clause_checksum = machine->
	get_concrete_variables_clause_checksum();
  abstract_variables_clause_checksum = machine->
	get_abstract_variables_clause_checksum();
  concrete_constants_clause_checksum = machine->
	get_concrete_constants_clause_checksum();
  abstract_constants_clause_checksum = machine->
	get_abstract_constants_clause_checksum();
  properties_clause_checksum = machine->get_properties_clause_checksum();
  invariant_clause_checksum = machine->get_invariant_clause_checksum();
  TRACE2("invariant_clause_checksum = %x:%s",
		 invariant_clause_checksum,
		 (invariant_clause_checksum != NULL ?
		  invariant_clause_checksum->get_value() : "null"));
  variant_clause_checksum = machine->get_variant_clause_checksum();
 assertions_clause_checksum = machine->get_assertions_clause_checksum();
  values_clause_checksum = machine->get_values_clause_checksum();

  abst_mach = machine->get_ref_abstraction();
  TRACE1("initialisation de l'abstraction : %x", abst_mach);
  if (abst_mach != NULL)
	{
	  refined_checksums = new T_component_checksums(abst_mach, CTYPE_REQUIRED,
													NULL, NULL, this);
	}
  else
	{
	  refined_checksums = NULL;
	}

  // Mise a jour de la liste des checksums des operations
  first_operation_checksums = NULL;
  last_operation_checksums = NULL;

  if (type == CTYPE_MAIN)
	{
	  // On ajoute la clause INITIALISATION
	  TRACE0("Clause Initialisation");
	  cur_op =machine->get_initialisation_clause_operation();
	  if (cur_op != NULL)
		{
		  TRACE1("On ajoute %x", cur_op);
		  TRACE1("de nom : %s", cur_op->get_op_name()->get_value());
		  new T_operation_checksums((T_op *)cur_op,
									machine,
									(T_item **)&first_operation_checksums,
									(T_item **)&last_operation_checksums,
									this);
		}

	  //CLB : on ajoute les op�rations locales
	  TRACE0("Clause LOCAL_OPERATIONS");
	  T_op *local_op = machine->get_local_operations();
	  while(local_op != NULL)
		{
		  //r�cup�ration de l'impl�mentation de l'op�ration locale
		  cur_op = machine->find_op(local_op->get_op_name());
		  TRACE1("On ajoute %x", cur_op);
		  TRACE1("de nom : %s", cur_op->get_op_name()->get_value());
		  new T_operation_checksums((T_op *)cur_op,
									machine,
									(T_item **)&first_operation_checksums,
									(T_item **)&last_operation_checksums,
									this);
		  local_op = (T_op *) local_op->get_next();
		}

	  // On parcourt la liste des operations (propres et promues)
	  // Pour avoir TOUTES les operations (y compris celles qui ne sont
	  // pas raffinees), on parcourt la liste des operations de la spec
	  TRACE0("Initialisation des operations");
	  if (!(machine->get_machine_type() == MTYPE_SYSTEM
			  || (machine->get_ref_specification() != NULL
					  && machine->get_ref_specification()->get_machine_type() == MTYPE_SYSTEM))) {
		  if (machine->get_ref_specification() == NULL)
		  {
			  // Cas d'une specification
			  elem_list = machine->get_op_list();
		  }
		  else
		  {
			  // Cas d'un raffinement ou d'une implantation
			  elem_list = machine->get_ref_specification()->get_op_list();
			  TRACE1("T_component_checksums::raffinement: %s", machine->get_ref_specification()->get_machine_name()->get_name()->get_value());
		  }
		  while (elem_list != NULL)
		  {
			  // On recupere le nom de l'operation dans la spec
			  op_name = ((T_generic_op *)elem_list->get_object())->get_op_name();

			  // On cherche l'operation dans le composant
			  cur_op = machine->find_op(op_name);
			  TRACE3("%s: resultat loop1 : %x,%s", machine->get_machine_name()->get_name()->get_value(),cur_op,op_name->get_value());

			  // Si l'operation n'est pas raffinn�e, on cherche sa precedente
			  // definition car c'est elle qui servira � faire les PO
			  abst_mach = machine;
			  while (cur_op == NULL)
			  {
				  abst_mach = abst_mach->get_ref_abstraction();
				  TRACE1("abst_mach = %x", abst_mach);
				  cur_op = abst_mach->find_op(op_name);
				  TRACE1("resultat : %x", cur_op);
			  }

			  TRACE2("On ajoute loop1 %x:%s", cur_op, op_name->get_value());
			  new T_operation_checksums(cur_op,
					  abst_mach,
					  (T_item **)&first_operation_checksums,
					  (T_item **)&last_operation_checksums,
					  this);
			  elem_list = (T_list_link *)elem_list->get_next();
		  }
	  }
	  else
	  {
		  abst_mach = machine;
		  while (abst_mach != NULL) {
			  TRACE2("%s: loop machine : %s", machine->get_machine_name()->get_name()->get_value(),
					  abst_mach->get_machine_name()->get_name()->get_value());
			  elem_list = abst_mach->get_op_list();
			  while (elem_list != NULL)
			  {
				  // On recupere le nom de l'operation dans la spec
				  op_name = ((T_generic_op *)elem_list->get_object())->get_op_name();

				  // On cherche l'operation dans le composant
				  cur_op = abst_mach->find_op(op_name);

				  ajout = TRUE;
				  // On vérifie que c'est la premiere occurence de l'événement en repartant
				  // du composant initial
				  tmp1_mach = machine;
				  while (tmp1_mach != abst_mach) {
					  if (tmp1_mach->find_op(op_name) != NULL) {
						  ajout = FALSE;
					  }
					  tmp1_mach = tmp1_mach->get_ref_abstraction();
				  }
				  // On vérifie que l'événement n'a pas été raffiné par un autre événement
				  tmp1_mach = machine;
				  while (tmp1_mach != abst_mach) {
					  tmp_elem_list = tmp1_mach->get_op_list();
					  while (tmp_elem_list != NULL) {
						  T_ident *ref_op = ((T_generic_op *)tmp_elem_list->get_object())->get_definition()->get_ref_operation();
						  while (ref_op != NULL) {
							  if (strcmp(ref_op->get_name()->get_value(),op_name->get_value()) == 0) {
								  ajout = FALSE;
							  }
							  ref_op = (T_ident *) ref_op->get_next();
						  }
						  tmp_elem_list = (T_list_link *)tmp_elem_list->get_next();
					  }
					  tmp1_mach = tmp1_mach->get_ref_abstraction();
				  }
				  if (ajout) {
					  TRACE2("On ajoute loop2 %x:%s", cur_op, op_name->get_value());
					  new T_operation_checksums(cur_op,
							  abst_mach,
							  (T_item **)&first_operation_checksums,
							  (T_item **)&last_operation_checksums,
							  this);
				  }
				  elem_list = (T_list_link *)elem_list->get_next();
			  }
			  abst_mach = abst_mach->get_ref_abstraction();
		  }
	  }
	  //	  //Pour le B événementiel, on traite aussi les opérations seulement présentes dans le raffinement
	  //	  if (machine->get_ref_specification() != NULL)
	  //	  {
	  //		  // Cas d'un raffinement
	  //		  elem_list = machine->get_op_list();
	  //		  while (elem_list != NULL)
	  //		  {
	  //			  // On recupere le nom de l'operation dans la spec
	  //			  op_name = ((T_generic_op *)elem_list->get_object())->get_op_name();
	  //
	  //			  // On cherche l'operation dans le composant
	  //			  cur_op = machine->find_op(op_name);
	  //			  TRACE3("%s: resultat loop2 : %x,%s", machine->get_machine_name()->get_name()->get_value(),cur_op,op_name->get_value());
	  //
	  //			  // Si l'opération est dans le composant, on vérifie qu'elle n'est pas dans une de ses
	  //			  // abstractions
	  //			  if (cur_op != NULL) {
	  //				  abst_mach = machine;
	  //				  cur_op1 = NULL;
	  //				  while (cur_op1 == NULL)
	  //				  {
	  //					  if (abst_mach->get_ref_specification() != NULL) {
	  //						  abst_mach = abst_mach->get_ref_abstraction();
	  //						  TRACE1("abst_mach = %x", abst_mach);
	  //						  cur_op1 = abst_mach->find_op(op_name);
	  //						  TRACE1("resultat : %x", cur_op);
	  //					  }
	  //					  else
	  //						  break;
	  //				  }
	  //				  if (cur_op1 == NULL) {
	  //					  TRACE2("On ajoute loop2 %x:%s", cur_op, op_name->get_value());
	  //					  new T_operation_checksums(cur_op,
	  //							  machine,
	  //							  (T_item **)&first_operation_checksums,
	  //							  (T_item **)&last_operation_checksums,
	  //							  this);
	  //				  }
	  //			  }
	  //			  elem_list = (T_list_link *)elem_list->get_next();
	  //		  }
	  //	  }
	}
  // sinon on ne le met pas a jour car ce n'est pas utile

  TRACE0("initialisation des machines requises");
  first_seen_checksums = NULL;
  last_seen_checksums = NULL;
  first_used_checksums = NULL;
  last_used_checksums = NULL;
  if (type == CTYPE_MAIN)
	{
	  // On met a jour les checksums des machines vues
	  tmp_mach = machine->get_sees();
	  while(tmp_mach != NULL)
		{
		  ASSERT(tmp_mach->get_ref_machine() != NULL);
		  new T_component_checksums(tmp_mach->get_ref_machine(),
									CTYPE_REQUIRED,
									(T_item **)&first_seen_checksums,
									(T_item **)&last_seen_checksums,
									this);
		  tmp_mach = (T_used_machine *)tmp_mach->get_next();
		}
	  // On met a jour les checksums des machines utilisees
	  tmp_mach = machine->get_uses();
	  while(tmp_mach != NULL)
		{
		  ASSERT(tmp_mach->get_ref_machine() != NULL);
		  new T_component_checksums(tmp_mach->get_ref_machine(),
									CTYPE_REQUIRED,
									(T_item **)&first_used_checksums,
									(T_item **)&last_used_checksums,
									this);
		  tmp_mach = (T_used_machine *)tmp_mach->get_next();
		}
	}

  // On met a jour les checksums des machines vues
  first_included_checksums = NULL;
  last_included_checksums = NULL;
  tmp_mach = machine->get_includes();
  while(tmp_mach != NULL)
	{
	  ASSERT(tmp_mach->get_ref_machine() != NULL);
	  new T_component_checksums(tmp_mach->get_ref_machine(),
								CTYPE_REQUIRED,
								(T_item **)&first_included_checksums,
								(T_item **)&last_included_checksums,
								this);
	  tmp_mach = (T_used_machine *)tmp_mach->get_next();
	}

  first_imported_checksums = NULL;
  last_imported_checksums = NULL;
  tmp_mach = machine->get_imports();
  while(tmp_mach != NULL)
	{
	  ASSERT(tmp_mach->get_ref_machine() != NULL);
	  new T_component_checksums(tmp_mach->get_ref_machine(),
								CTYPE_REQUIRED,
								(T_item **)&first_imported_checksums,
								(T_item **)&last_imported_checksums,
								this);
	  tmp_mach = (T_used_machine *)tmp_mach->get_next();
	}

  first_extended_checksums = NULL;
  last_extended_checksums = NULL;
  tmp_mach = machine->get_extends();
  while(tmp_mach != NULL)
	{
	  ASSERT(tmp_mach->get_ref_machine() != NULL);
	  new T_component_checksums(tmp_mach->get_ref_machine(),
								CTYPE_REQUIRED,
								(T_item **)&first_extended_checksums,
								(T_item **)&last_extended_checksums,
								this);
	  tmp_mach = (T_used_machine *)tmp_mach->get_next();
	}
  TRACE0("T_component_checksums::T_component_checksums : OK");
}


// Destructeur
T_component_checksums::~T_component_checksums(void)
{
  TRACE1("T_component_checksums(%x)::~T_component_checksums()", this);

  object_unlink(component_name);
  object_unlink(formal_params_checksum);
  object_unlink(constraints_clause_checksum);
  object_unlink(sees_clause_checksum);
  object_unlink(includes_clause_checksum);
  object_unlink(imports_clause_checksum);
  object_unlink(promotes_clause_checksum);
  object_unlink(extends_clause_checksum);
  object_unlink(uses_clause_checksum);
  object_unlink(sets_clause_checksum);
  object_unlink(concrete_variables_clause_checksum);
  object_unlink(abstract_variables_clause_checksum);
  object_unlink(concrete_constants_clause_checksum);
  object_unlink(abstract_constants_clause_checksum);
  object_unlink(properties_clause_checksum);
  object_unlink(invariant_clause_checksum);
  object_unlink(variant_clause_checksum);
  object_unlink(assertions_clause_checksum);
  object_unlink(values_clause_checksum);

  // Checksums des raffinements du composants
  TRACE0("Liberation des checksums des composants raffines");
  object_unlink(refined_checksums);

  // Checksums des machines requises
  TRACE0("Liberation des checksums des composants requis");
  list_unlink(first_seen_checksums);
  list_unlink(first_used_checksums);
  list_unlink(first_included_checksums);
  list_unlink(first_imported_checksums);
  list_unlink(first_extended_checksums);

  // Checksums des operations
  TRACE0("Liberation des checksums des operations");
  list_unlink(first_operation_checksums);

  TRACE0("T_component_checksums()::~T_component_checksums()->OK");
}

T_component_checksums *T_component_checksums::get_seen_checksums()
{
  if (type == CTYPE_REQUIRED)
	{
	  ASSERT((first_seen_checksums == NULL) && (last_seen_checksums == NULL));
	  return NULL;
	}
  return first_seen_checksums;
}

T_component_checksums *T_component_checksums::get_used_checksums()
{
  if (type == CTYPE_REQUIRED)
	{
	  ASSERT((first_used_checksums == NULL) && (last_used_checksums == NULL));
	  return NULL;
	}
  return first_used_checksums;
}

T_operation_checksums *T_component_checksums::get_operations_checksums(void) const
{
  if (type == CTYPE_REQUIRED)
	{
	  ASSERT((first_operation_checksums == NULL) &&
			 (last_operation_checksums == NULL));
	  return NULL;
	}
  return first_operation_checksums;
}

// Comparaison de deux listes de checksums
static int list_checksum_compare(T_component_checksums *check1,
                                 T_component_checksums *check2,
                                 bool recurse)
{
  TRACE2("list_checksum_compare(%x, %x)", check1, check2);

  T_component_checksums *cur_check1 = check1;
  T_component_checksums *cur_check2 = check2;
  int res = TRUE;

  if (check1 == NULL)
	{
	  return (check2 == NULL);
	}

  while ((res == TRUE) && (cur_check1 != NULL) && (cur_check2 != NULL))
	{
      res = cur_check1->has_same_context(cur_check2, recurse);
	  cur_check1 = (T_component_checksums *)cur_check1->get_next();
	  cur_check2 = (T_component_checksums *)cur_check2->get_next();
	}

  if ((res == TRUE) && (cur_check1 == NULL) && (cur_check2 == NULL))
	{
	  // Les deux listes avaient le meme nombre d'elements qui sont
	  //tous identiques
	  return TRUE;
	}
  // Sinon soit on a trouve une difference (res == FALSE) soit les deux listes
  // n'ont pas le meme d'elements (cur_check1 != NULL ou cur_check2 != NULL)
  return FALSE;
}

// Comparaison de deux contexte
// Renvoie TRUE si les contextes sont les memes, FALSE sinon
int T_component_checksums::has_same_context(
                                             T_component_checksums *comp_state, bool recurse)

{
  TRACE2("T_component_checksums(%x)::has_same_context(%x)", this, comp_state);

  ASSERT(comp_state != NULL);

  int ret = TRUE;
  ret = ret && (symbol_compare(component_name, comp_state->get_component_name()) == TRUE);
  ret = ret && (type == comp_state->get_type());
  ret = ret && (symbol_compare(formal_params_checksum, comp_state->get_formal_params_checksum()) == TRUE);
  ret = ret && (symbol_compare(constraints_clause_checksum, comp_state->get_constraints_clause_checksum()) == TRUE);
  ret = ret && (symbol_compare(sees_clause_checksum, comp_state->get_sees_clause_checksum()) == TRUE);
  ret = ret && (symbol_compare(includes_clause_checksum, comp_state->get_includes_clause_checksum()) == TRUE);
  ret = ret && (symbol_compare(imports_clause_checksum, comp_state->get_imports_clause_checksum()) == TRUE);
  ret = ret && (symbol_compare(extends_clause_checksum, comp_state->get_extends_clause_checksum()) == TRUE);
  ret = ret && (symbol_compare(uses_clause_checksum, comp_state->get_uses_clause_checksum()) == TRUE);
  ret = ret && (symbol_compare(sets_clause_checksum, comp_state->get_sets_clause_checksum()) == TRUE);
  ret = ret && (symbol_compare(concrete_variables_clause_checksum, comp_state->get_concrete_variables_clause_checksum()) == TRUE);
  ret = ret && (symbol_compare(abstract_variables_clause_checksum, comp_state->get_abstract_variables_clause_checksum()) == TRUE);
  ret = ret && (symbol_compare(concrete_constants_clause_checksum, comp_state->get_concrete_constants_clause_checksum()) == TRUE);
  ret = ret && (symbol_compare(abstract_constants_clause_checksum, comp_state->get_abstract_constants_clause_checksum()) == TRUE);
  ret = ret && (symbol_compare(properties_clause_checksum, comp_state->get_properties_clause_checksum()) == TRUE);
  ret = ret && (symbol_compare(invariant_clause_checksum, comp_state->get_invariant_clause_checksum()) == TRUE);
  ret = ret && (symbol_compare(variant_clause_checksum, comp_state->get_variant_clause_checksum()) == TRUE);
  ret = ret && (symbol_compare(assertions_clause_checksum, comp_state->get_assertions_clause_checksum()) == TRUE);
  ret = ret && (symbol_compare(values_clause_checksum, comp_state->get_values_clause_checksum()) == TRUE);
  // Comparaison des checksums des differentes machines requises
  ret = ret && (list_checksum_compare(refined_checksums, comp_state->get_refined_checksums(), true /*Refined tjrs récursif*/) == TRUE);
  if(recurse)
  {
      //ATTENTION, ceux-la ne sont pas transitifs
      ret = ret && (list_checksum_compare(first_seen_checksums, comp_state->get_seen_checksums(), false) == TRUE);
      ret = ret && (list_checksum_compare(first_used_checksums, comp_state->get_used_checksums(), false) == TRUE);
      ret = ret && (list_checksum_compare(first_included_checksums, comp_state->get_included_checksums(), false) == TRUE);
      ret = ret && (list_checksum_compare(first_imported_checksums, comp_state->get_imported_checksums(), false) == TRUE);
      ret = ret && (list_checksum_compare(first_extended_checksums, comp_state->get_extended_checksums(), false) == TRUE);
  }

  return ret;

}

ostream &operator<<(ostream &os, const T_component_checksums &sub)
{
    os << "<component_checksums>" << endl;
    sub.writeBaseClauses(os);

    //clauses non transitives il ne faut pas inclure les clauses SEES, etc...
    os << "<seen>" << endl;
    sub.writeNotTransitiveClause(os, sub.first_seen_checksums, sub.last_seen_checksums);
    os << "</seen>" << endl;
    os << "<used>" << endl;
    sub.writeNotTransitiveClause(os, sub.first_used_checksums, sub.last_used_checksums);
    os << "</used>" << endl;
    os << "<included>" << endl;
    sub.writeNotTransitiveClause(os, sub.first_included_checksums, sub.last_included_checksums);
    os << "</included>" << endl;
    os << "<imported>" << endl;
    sub.writeNotTransitiveClause(os, sub.first_imported_checksums, sub.last_imported_checksums);
    os << "</imported>" << endl;
    os << "<extended>" << endl;
    sub.writeNotTransitiveClause(os, sub.first_extended_checksums, sub.last_extended_checksums);
    os << "</extended>" << endl;

    //La clause REFINES est transitive, donc on la met telle quelle dans le fichier
    os << "<refined>" << endl;
    if(sub.refined_checksums)
        os << *(sub.refined_checksums) << endl;
    os << "</refined>" << endl;

    os << "</component_checksums>" << endl;
    return os;
}

/** DO NOT USE: bug with mingw, function tellg move the stream position**/
istream &operator>>(istream &is, T_component_checksums &sub)
{
    string lCurStr;
    is >> lCurStr;

    if(lCurStr == "<component_checksums>")
    {
        sub.readBaseClauses(is);
        //clauses non transitives
        is >> lCurStr;
        if(lCurStr == "<seen>")
        {
            sub.readNotTransitiveClause(is, &sub.first_seen_checksums, &sub.last_seen_checksums);
            is >> lCurStr; //</seen>
        }
        is >> lCurStr;
        if(lCurStr == "<used>")
        {
            sub.readNotTransitiveClause(is, &sub.first_used_checksums, &sub.last_used_checksums);
            is >> lCurStr; //</used>
        }
        is >> lCurStr;
        if(lCurStr == "<included>")
        {
            sub.readNotTransitiveClause(is, &sub.first_included_checksums, &sub.last_included_checksums);
            is >> lCurStr; //</included>
        }
        is >> lCurStr;
        if(lCurStr == "<imported>")
        {
            sub.readNotTransitiveClause(is, &sub.first_imported_checksums, &sub.last_imported_checksums);
            is >> lCurStr; //</imported>
        }
        is >> lCurStr;
        if(lCurStr == "<extended>")
        {
            sub.readNotTransitiveClause(is, &sub.first_extended_checksums, &sub.last_extended_checksums);
            is >> lCurStr; //</extended>
        }
        //clause refine => transitive
        is >> lCurStr;
        if(lCurStr == "<refined>")
        {
            istream::pos_type lIsPos(is.tellg());
            is >> lCurStr; // <component_checksums> ou </refined>
            if(lCurStr != "</refined>")
            {
                is.seekg(lIsPos);
                sub.refined_checksums = new T_component_checksums();
                is >> *sub.refined_checksums;
                is >> lCurStr; //</refined>
            }
        }

    }

    return is;
}

void T_component_checksums::readXML(TiXmlElement* component) {

    if(component)
    {
        readBaseClauses(component->FirstChildElement("base"));
        //clauses non transitives
        if(component->FirstChildElement("seen"))
            readNotTransitiveClause(component->FirstChildElement("seen"), &first_seen_checksums, &last_seen_checksums);
        if(component->FirstChildElement("used"))
            readNotTransitiveClause(component->FirstChildElement("used"), &first_used_checksums, &last_used_checksums);
        if(component->FirstChildElement("included"))
            readNotTransitiveClause(component->FirstChildElement("included"), &first_included_checksums, &last_included_checksums);
        if(component->FirstChildElement("imported"))
            readNotTransitiveClause(component->FirstChildElement("imported"), &first_imported_checksums, &last_imported_checksums);
        if(component->FirstChildElement("extended"))
            readNotTransitiveClause(component->FirstChildElement("extended"), &first_extended_checksums, &last_extended_checksums);
        //clause refine => transitive

        if(component->FirstChildElement("refined"))
        {
            TiXmlElement* content = component->FirstChildElement("refined")->FirstChildElement();
            if(content)
            {
                refined_checksums = new T_component_checksums();
                refined_checksums->readXML(content);
            }
        }

    }
}

void T_component_checksums::readBaseClauses(istream &is)
{
    string lCurStr;
    is >> lCurStr;

    if(lCurStr == "<base>")
    {
        readSymbol(is,"NAME",&component_name);
        int typeInt;
        char line[2048];
        line[0]='\0';
        while(strlen(line) == 0)
        {
            is.getline(line, 2048);
            if(sscanf(line,"<symbol name=\"TYPE\" value=\"%d\" />",&typeInt) == 1)
            {
                type = (T_context_type) typeInt;
            }
        }
        readSymbol(is,"PARAM",&formal_params_checksum);
        readSymbol(is,"CONSTRAINTS",&constraints_clause_checksum);
        readSymbol(is,"SEES",&sees_clause_checksum);
        readSymbol(is,"INCLUDES",&includes_clause_checksum);
        readSymbol(is,"IMPORTS",&imports_clause_checksum);
        readSymbol(is,"PROMOTES",&promotes_clause_checksum);
        readSymbol(is,"EXTENDS",&extends_clause_checksum);
        readSymbol(is,"USES",&uses_clause_checksum);
        readSymbol(is,"SETS",&sets_clause_checksum);
        readSymbol(is,"CONCRETE_VAR",&concrete_variables_clause_checksum);
        readSymbol(is,"ABSTRACT_VAR",&abstract_variables_clause_checksum);
        readSymbol(is,"CONCRETE_CONST",&concrete_constants_clause_checksum);
        readSymbol(is,"ABSTRACT_CONST",&abstract_constants_clause_checksum);
        readSymbol(is,"PROPERTIES",&properties_clause_checksum);
        readSymbol(is,"INVARIANT",&invariant_clause_checksum);
        readSymbol(is,"VARIANT",&variant_clause_checksum);
        readSymbol(is,"ASSERTIONS",&assertions_clause_checksum);
        readSymbol(is,"VALUES",&values_clause_checksum);

        //Traitement des opérations
        is >> lCurStr;
        if(lCurStr == "<operations>")
        {
            istream::pos_type lIsPos(is.tellg());
            is >> lCurStr;
            while(lCurStr != "</operations>")
            {
                is.seekg(lIsPos);
                T_operation_checksums* lCurOperationChecksum = new T_operation_checksums();
                is >> *lCurOperationChecksum;
                lCurOperationChecksum->tail_insert((T_item**) &first_operation_checksums, (T_item**) &last_operation_checksums);
                lIsPos = is.tellg();
                is >> lCurStr; // </operations> ou operation
            }
        }
        is >> lCurStr; // </base>
    }
}

void T_component_checksums::readBaseClauses(TiXmlElement* base)
{
    if(base)
    {
        TiXmlElement* symbol = base->FirstChildElement("symbol");
        readSymbol(symbol,"NAME",&component_name);
        symbol = symbol->NextSiblingElement("symbol");
        if(symbol)
            if(symbol->Attribute("name"))
                if(strcmp(symbol->Attribute("name"), "TYPE") == 0)
                    type = (T_context_type) atoi(symbol->Attribute("value"));
        symbol = symbol->NextSiblingElement("symbol");
        readSymbol(symbol,"PARAM",&formal_params_checksum);
        symbol = symbol->NextSiblingElement("symbol");
        readSymbol(symbol,"CONSTRAINTS",&constraints_clause_checksum);
        symbol = symbol->NextSiblingElement("symbol");
        readSymbol(symbol,"SEES",&sees_clause_checksum);
        symbol = symbol->NextSiblingElement("symbol");
        readSymbol(symbol,"INCLUDES",&includes_clause_checksum);
        symbol = symbol->NextSiblingElement("symbol");
        readSymbol(symbol,"IMPORTS",&imports_clause_checksum);
        symbol = symbol->NextSiblingElement("symbol");
        readSymbol(symbol,"PROMOTES",&promotes_clause_checksum);
        symbol = symbol->NextSiblingElement("symbol");
        readSymbol(symbol,"EXTENDS",&extends_clause_checksum);
        symbol = symbol->NextSiblingElement("symbol");
        readSymbol(symbol,"USES",&uses_clause_checksum);
        symbol = symbol->NextSiblingElement("symbol");
        readSymbol(symbol,"SETS",&sets_clause_checksum);
        symbol = symbol->NextSiblingElement("symbol");
        readSymbol(symbol,"CONCRETE_VAR",&concrete_variables_clause_checksum);
        symbol = symbol->NextSiblingElement("symbol");
        readSymbol(symbol,"ABSTRACT_VAR",&abstract_variables_clause_checksum);
        symbol = symbol->NextSiblingElement("symbol");
        readSymbol(symbol,"CONCRETE_CONST",&concrete_constants_clause_checksum);
        symbol = symbol->NextSiblingElement("symbol");
        readSymbol(symbol,"ABSTRACT_CONST",&abstract_constants_clause_checksum);
        symbol = symbol->NextSiblingElement("symbol");
        readSymbol(symbol,"PROPERTIES",&properties_clause_checksum);
        symbol = symbol->NextSiblingElement("symbol");
        readSymbol(symbol,"INVARIANT",&invariant_clause_checksum);
        symbol = symbol->NextSiblingElement("symbol");
        readSymbol(symbol,"VARIANT",&variant_clause_checksum);
        symbol = symbol->NextSiblingElement("symbol");
        readSymbol(symbol,"ASSERTIONS",&assertions_clause_checksum);
        symbol = symbol->NextSiblingElement("symbol");
        readSymbol(symbol,"VALUES",&values_clause_checksum);

        //Traitement des opération

        if(base->FirstChildElement("operations"))
        {
            TiXmlElement* operation = base->FirstChildElement("operations")->FirstChildElement("operation");
            while(operation)
            {
                T_operation_checksums* lCurOperationChecksum = new T_operation_checksums();
                lCurOperationChecksum->readOperation(operation);
                lCurOperationChecksum->tail_insert((T_item**) &first_operation_checksums, (T_item**) &last_operation_checksums);
                operation = operation->NextSiblingElement("operation");
            }
        }
    }
}

void writeSymbol(ostream &os, const string& pName, T_symbol* pSymbol)
{
    os << "<symbol name=\"" << pName << "\"";
    if(pSymbol)
    {
        os << " value=\"" << *pSymbol << "\"";
    }
    os << " />" << endl;
}

void readSymbol(istream &is, const string& pName, T_symbol** pSymbol)
{
    string lCurStr;
    is >> lCurStr; //<symbol
    is >> lCurStr; //name="xxx"
    if((pName.size() == 0) || (lCurStr == "name=\""+pName+"\""))
    {
        is >> lCurStr; //value="xxx" ou "/>"
        if(lCurStr != "/>")
        {
            lCurStr = lCurStr.substr(7, lCurStr.size() - 7 - 1);
            *pSymbol = new T_symbol(lCurStr.c_str(), lCurStr.length(), 0, 0, 0);
            is >> lCurStr; // "/>";
        }
    }
}

void readSymbol(TiXmlElement* symbolElem, const string& pName, T_symbol** pSymbol)
{
    std::string readAttr = symbolElem->Attribute("name");
    if(pName.size() == 0 || readAttr == pName)
        if(symbolElem->Attribute("value"))
            *pSymbol = new T_symbol(symbolElem->Attribute("value"), strlen(symbolElem->Attribute("value")), 0, 0, 0);
}

void T_component_checksums::writeBaseClauses(ostream &os) const
{
    os << "<base>" << endl;
    writeSymbol(os,"NAME",component_name);
    //writeSymbol(os,"TYPE",&type);
    os << "<symbol name=\"TYPE\" value=\"" << (int)type << "\" />" << endl;
    writeSymbol(os,"PARAM",formal_params_checksum);
    writeSymbol(os,"CONSTRAINTS",constraints_clause_checksum);
    writeSymbol(os,"SEES",sees_clause_checksum);
    writeSymbol(os,"INCLUDES",includes_clause_checksum);
    writeSymbol(os,"IMPORTS",imports_clause_checksum);
    writeSymbol(os,"PROMOTES",promotes_clause_checksum);
    writeSymbol(os,"EXTENDS",extends_clause_checksum);
    writeSymbol(os,"USES",uses_clause_checksum);
    writeSymbol(os,"SETS",sets_clause_checksum);
    writeSymbol(os,"CONCRETE_VAR",concrete_variables_clause_checksum);
    writeSymbol(os,"ABSTRACT_VAR",abstract_variables_clause_checksum);
    writeSymbol(os,"CONCRETE_CONST",concrete_constants_clause_checksum);
    writeSymbol(os,"ABSTRACT_CONST",abstract_constants_clause_checksum);
    writeSymbol(os,"PROPERTIES",properties_clause_checksum);
    writeSymbol(os,"INVARIANT",invariant_clause_checksum);
    writeSymbol(os,"VARIANT",variant_clause_checksum);
    writeSymbol(os,"ASSERTIONS",assertions_clause_checksum);
    writeSymbol(os,"VALUES",values_clause_checksum);

    os << "<operations>" << endl;
    char operationName[2000];
    T_operation_checksums* lOpeCheck = get_operations_checksums();
    while(lOpeCheck)
    {
        os << *lOpeCheck << endl;
        lOpeCheck = (T_operation_checksums*)lOpeCheck->get_next();
    }
    os << "</operations>" << endl;

    os << "</base>" << endl;
}

void T_component_checksums::writeNotTransitiveClause(ostream &os, T_component_checksums *startElt, T_component_checksums *endElt) const
{
    if(startElt && endElt)
    {
        T_component_checksums * currentElt = startElt;
        while(currentElt != endElt)
        {
            currentElt->writeBaseClauses(os);
            currentElt = (T_component_checksums*) currentElt->get_next();
        }
        currentElt->writeBaseClauses(os);
    }
}

void T_component_checksums::readNotTransitiveClause(istream &is, T_component_checksums **startElt, T_component_checksums **endElt)
{
    string lCurStr;
    istream::pos_type lPos = is.tellg();
    is >> lCurStr;
    while(lCurStr == "<base>")
    {
        T_component_checksums* lCurrentComp = new T_component_checksums();
        is.seekg(lPos);
        lCurrentComp->readBaseClauses(is);
        lCurrentComp->tail_insert((T_item**)startElt , (T_item**)endElt);
        lPos = is.tellg();
        is >> lCurStr;
    }

    is.seekg(lPos);
}

void T_component_checksums::readNotTransitiveClause(TiXmlElement* clause, T_component_checksums **startElt, T_component_checksums **endElt)
{
    TiXmlElement* base = clause->FirstChildElement("base");
    while(base)
    {
        T_component_checksums* lCurrentComp = new T_component_checksums();
        lCurrentComp->readBaseClauses(base);
        lCurrentComp->tail_insert((T_item**)startElt , (T_item**)endElt);
        base = base->NextSiblingElement("base");
    }
}

// Fonctions de parcours des corps des operations pour creer la liste des
// operations appel�es

static int internal_add_called_op(T_substitution *subst,
										  T_operation_checksums *op_checksums)
{
  int res = 0;
  T_substitution *tmp = subst;

  while (tmp != NULL)
	{
	  res = res + tmp->user_defined_f5((T_object *)op_checksums);
	  tmp = (T_substitution *)tmp->get_next();
	}
  return res;
}

int add_called_op_substitution(T_substitution *subst,
										T_operation_checksums *op_checksums)
{
  TRACE2("add_called_op_substitution(%x, %x)", subst, op_checksums);

  user_warning(BASE_WARNING,
			   get_user_warning_msg(U_ATB_DIFF_NOT_IMPL),
			   subst->get_node_type());
  return 1;
}

int add_called_op_opcall(T_op_call *opcall,
								  T_operation_checksums *op_checksums)
{
  TRACE2("-->add_called_op_opcall(%x, %x)", opcall, op_checksums);

  T_generic_op *ref_op = opcall->get_ref_op();

  //  On recherche la machine � laquelle appartient le composant
  //  appel� en remontant les p�res.  On acc�de � la machine du
  //  premier coup s'il s'agit d'une op�ration propre, mais seulement
  //  au deuxi�me coup pour une op�ration promue.
  T_item *father = ref_op->get_father();
  while (father != NULL && father->get_node_type() != NODE_MACHINE)
	{
	  father = father->get_father();
	}
  ASSERT(father->get_node_type() == NODE_MACHINE);

  // On ajoute l'appel d'operation
  new T_operation_checksums(ref_op,
							(T_machine *)father,
							(T_item **)op_checksums->
							  get_adr_first_called_operation_checksums(),
							(T_item **)op_checksums->
							  get_adr_last_called_operation_checksums(),
							op_checksums->get_father());

TRACE2("<--add_called_op_opcall(%x, %x)", opcall, op_checksums);
  return 0;
}

int add_called_op_skip(T_substitution *subst,
								T_operation_checksums *op_checksums)
{
  TRACE2("add_called_op_skip(%x, %x)", subst, op_checksums);
  TRACE0("Substitution terminale, sans appel d'operation");
  return 0;
}

int add_called_op_any(T_any *any,
							   T_operation_checksums *op_checksums)
{
  TRACE2("add_called_op_any(%x, %x)", any, op_checksums);

  return internal_add_called_op(any->get_body(), op_checksums);
}

int add_called_op_assert(T_assert *b_assert,
								  T_operation_checksums *op_checksums)
{
  TRACE2("add_called_op_assert(%x, %x)", b_assert, op_checksums);

  return internal_add_called_op(b_assert->get_body(), op_checksums);
}

int add_called_op_label(T_label *b_label,
                                                                  T_operation_checksums *op_checksums)
{
  TRACE2("add_called_op_assert(%x, %x)", b_label, op_checksums);

  return internal_add_called_op(b_label->get_body(), op_checksums);
}

int add_called_op_jumpif(T_jumpif *b_witness,
                                                                  T_operation_checksums *op_checksums)
{
  TRACE2("add_called_op_jumpif(%x, %x)", b_witness, op_checksums);

  return 0;
}

int add_called_op_witness(T_witness *b_witness,
								  T_operation_checksums *op_checksums)
{
  TRACE2("add_called_op_witness(%x, %x)", b_witness, op_checksums);

  return internal_add_called_op(b_witness->get_body(), op_checksums);
}

int add_called_op_begin(T_begin *b_begin,
								 T_operation_checksums *op_checksums)
{
  TRACE2("add_called_op_begin(%x, %x)", b_begin, op_checksums);

  return internal_add_called_op(b_begin->get_body(), op_checksums);
}

int add_called_op_case(T_case *b_case,
								T_operation_checksums *op_checksums)
{
  TRACE2("add_called_op_case(%x, %x)", b_case, op_checksums);

  T_case_item *case_item = b_case->get_case_items();
  int res = 0;

  while (case_item != NULL)
	{
	  res = res + internal_add_called_op(case_item->get_body(), op_checksums);
	  case_item = (T_case_item *)case_item->get_next();
	}
  // On retourne 1 si il y a eu une erreur
  return res;
}

int add_called_op_choice(T_choice *b_choice,
								  T_operation_checksums *op_checksums)
{
  TRACE2("add_called_op_choice(%x, %x)", b_choice, op_checksums);

  T_or *choice_or = b_choice->get_or();
  int res = internal_add_called_op(b_choice->get_body(), op_checksums);

  while (choice_or != NULL)
	{
	  res = res + internal_add_called_op(choice_or->get_body(), op_checksums);
	  choice_or = (T_or *)choice_or->get_next();
	}
  // On retourne 1 si il y a eu une erreur
  return res;
}

int add_called_op_if(T_if *b_if,
							  T_operation_checksums *op_checksums)
{
  TRACE2("add_called_op_if(%x, %x)", b_if, op_checksums);

  int res = internal_add_called_op(b_if->get_then_body(), op_checksums);

  T_else *b_else = b_if->get_else();

  while (b_else != NULL)
	{
	  res = res + internal_add_called_op(b_else->get_body(), op_checksums);
	  b_else = (T_else *)b_else->get_next();
	}
  // On retourne 1 si il y a eu une erreur
  return res;
}

int add_called_op_let(T_let *b_let,
							   T_operation_checksums *op_checksums)
{
  TRACE2("add_called_op_let(%x, %x)", b_let, op_checksums);

  return internal_add_called_op(b_let->get_body(), op_checksums);
}

int add_called_op_precond(T_precond *b_pre,
								   T_operation_checksums *op_checksums)
{
  TRACE2("add_called_op_pre(%x, %x)", b_pre, op_checksums);

  return internal_add_called_op(b_pre->get_body(), op_checksums);
}

int add_called_op_select(T_select *b_select,
								  T_operation_checksums *op_checksums)
{
  TRACE2("add_called_op_select(%x, %x)", b_select, op_checksums);

  int res = internal_add_called_op(b_select->get_then_body(), op_checksums);

  T_when *b_when = b_select->get_when();

  while (b_when != NULL)
	{
	  res = res + internal_add_called_op(b_when->get_body(), op_checksums);
	  b_when = (T_when *)b_when->get_next();
	}
  // On retourne 1 si il y a eu une erreur
  return res;
}

int add_called_op_var(T_var *b_var,
							   T_operation_checksums *op_checksums)
{
  TRACE2("add_called_op_var(%x, %x)", b_var, op_checksums);

  return internal_add_called_op(b_var->get_body(), op_checksums);
}

int add_called_op_while(T_while *b_while,
								 T_operation_checksums *op_checksums)
{
  TRACE2("add_called_op_while(%x, %x)", b_while, op_checksums);

  return internal_add_called_op(b_while->get_body(), op_checksums);
}

//
// Fin du fichier
//
