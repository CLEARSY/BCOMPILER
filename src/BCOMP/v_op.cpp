/******************************* CLEARSY **************************************
* Fichier : $Id: v_op.cpp,v 2.0 2002-11-25 17:01:43 lv Exp $
* (C) 2008 CLEARSY
*
* Description :		Compilateur B
*					Verifications sur les operations
*
* Compilations :	-DTRACE_OP pour tracer la construction de la liste des
*					operations
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
RCS_ID("$Id: v_op.cpp,v 1.29 2002-11-25 17:01:43 lv Exp $") ;

/* Includes systeme */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

/* Includes Composant Local */
#include "c_api.h"
#include "v_ldreq.h"
#include "v_toplvl.h"


static void set_explicitly_declared( T_generic_op *op, int explicitl);

static void set_homonym_in_abstraction( T_generic_op *op, T_generic_op *op_abs);

static void set_homonym_in_required_mach( T_generic_op *op, T_generic_op *op_abs);

static void set_implemented_by( T_generic_op *op, T_generic_op *op_req);

static void add_op_list(T_op *list,
								 T_item **adr_first,
								 T_item **adr_last,
								 T_machine *machine,
								 T_betree *betree)
{
#ifdef TRACE_OP
  TRACE1("add_op_list(%x)", list) ;
#endif

T_op *cur = list ;

while (cur != NULL)
	{
#ifdef TRACE_OP
	  TRACE1("ajout de %s", cur->get_name()->get_name()->get_value()) ;
#endif
	(void)new T_list_link(cur,
						  LINK_OP_LIST,
						  adr_first,
						  adr_last,
						  machine,
						  betree,
						  cur->get_ref_lexem()) ;

	cur = (T_op *)cur->get_next() ;
	}
}

// Fonction auxiliaire
// Construction de la liste de toutes les operations d'une machine
static void build_op_list(T_machine *root)
{
TRACE2("build_op_list(%x:%s)",
	   root,
	   root->get_machine_name()->get_name()->get_value()) ;

ENTER_TRACE ; ENTER_TRACE ;

T_list_link *first = NULL ;
T_list_link *last = NULL ;

// On commence par les operations propres
TRACE0("DEBUT AJOUT OPERATIONS PROPRES") ;
add_op_list(root->get_operations(),
			(T_item **)&first,
			(T_item **)&last,
			root,
			root->get_betree()) ;
TRACE0("FIN   AJOUT OPERATIONS PROPRES") ;


// Puis on ajoute toutes les operations des machines promues
TRACE0("DEBUT AJOUT OPERATIONS PROMUES") ;
T_used_op *cur_promotes = root->get_promotes() ;

while (cur_promotes != NULL)
	{
	TRACE1("cur_promotes = %x", cur_promotes) ;
	TRACE1("ajout operation %s", cur_promotes->get_op_name()->get_value()) ;
	(void)new T_list_link(cur_promotes,
						  LINK_OP_LIST,
						  (T_item **)&first,
						  (T_item **)&last,
						  root,
						  root->get_betree(),
						  cur_promotes->get_ref_lexem()) ;

	cur_promotes = (T_used_op *)cur_promotes->get_next() ;
	}

TRACE0("FIN   AJOUT OPERATIONS PROMUES") ;

// Puis ajout de toutes les operations de toutes les machines etendues
TRACE0("DEBUT AJOUT OPERATIONS DES MACHINES ETENDUES") ;
T_used_machine *cur_ext = root->get_extends() ;

while (cur_ext != NULL)
  {
	TRACE1("-> machine etendue %s", cur_ext->get_component_name()->get_value()) ;

	T_machine *mach = cur_ext->get_ref_machine() ;
 	TRACE1("mach = %x", mach) ;

	if (mach != NULL) // NULL en cas d'erreur ?
	  {
		T_list_link *cur_lnk = mach->get_op_list() ;
		TRACE1("cur_lnk = %x", cur_lnk) ;

		while (cur_lnk != NULL)
		  {
			TRACE1("cur_lnk = %x", cur_lnk) ;

			// Cast valide par construction
			T_generic_op *genop = (T_generic_op *)(cur_lnk->get_object()) ;
			ASSERT(genop->is_an_operation() == TRUE) ;

			// A FAIRE : premier parametre faux en cas de renommage
			T_used_op *cur_used_op = new T_used_op(genop->get_name()->get_name(),
												   cur_ext->get_instance_name(),
												   genop->get_name(),
												   genop,
												   NULL,
												   NULL,
												   NULL,
												   root->get_betree(),
												   cur_ext->get_ref_lexem()) ;

#if 0
			// Mise a jour du father,... pour que les recherches de visibilite
			// se passent bien
			clone_op->fix_links(root) ;
#endif

			T_list_link *new_link = new T_list_link(cur_used_op,
													LINK_OP_LIST,
													(T_item **)&first,
													(T_item **)&last,
													root,
													root->get_betree(),
													cur_ext->get_ref_lexem()) ;

			cur_used_op->set_father(new_link) ;
			set_explicitly_declared(cur_used_op, FALSE);
			set_homonym_in_required_mach(cur_used_op, genop);

			// positionnement de impl�mented by dans le cas ou on est
			// sur une impl�me
			if (root->get_machine_type() == MTYPE_IMPLEMENTATION)
			  {
				set_implemented_by(cur_used_op,genop);
			  }

			new_link->set_do_free() ; // new_link responsable de cur_used_op
			cur_lnk = (T_list_link *)cur_lnk->get_next() ;
		  }
	  }

	cur_ext = (T_used_machine *)cur_ext->get_next() ;
  }

TRACE0("FIN   AJOUT OPERATIONS DES MACHINES ETENDUES") ;

// Et on met a jour les pointeurs de la machine
root->set_op_list(first, last) ;

EXIT_TRACE ; EXIT_TRACE ;
}

// Fonction auxilaire
// Verifier que toutes les operations sont specifiees
static void check_op_in_spec(T_machine *root, T_machine *abstract)
{
TRACE2("check_op_in_spec(%x, %x)", root, abstract) ;
T_list_link *cur_link = root->get_op_list() ;

while (cur_link != NULL)
	{
	T_item *cur_op = (T_item *)cur_link->get_object() ;
	T_symbol *name ;

	TRACE2("cur_op %x %s",
		   cur_op, (cur_op == NULL)
		   ? "null" : cur_op->get_class_name()) ;
	ASSERT(    (cur_op->get_node_type() == NODE_OPERATION)
		    || (cur_op->get_node_type() == NODE_USED_OP) ) ; // A FAIRE

	name = ((T_generic_op *)cur_op)->get_op_name() ;

	TRACE1("name = %s", name->get_value()) ;
	T_list_link *spec_link = NULL ;
	// was : T_item *spec_op = abstract->find_op(name, &spec_link) ;
	// erreur : Il faut remonter dans tous les raffinements car
	// raffinement ne raffine pas forcement toutes les operations (Bug
	// 1958). Donc autant aller chercher directement dans la spec
	TRACE2("abstract = %x (%s)",
		   abstract,
		   abstract->get_machine_name()->get_name()->get_value()) ;
	TRACE2("spec = %x (%s)",
		   abstract->get_specification(),
		   abstract->get_specification()->get_machine_name()->get_name()->get_value()) ;
	T_item *spec_op = abstract->get_specification()->find_op(name, &spec_link) ;
	TRACE1("node type(%d)",
		   (spec_link == NULL) ? 0 : spec_link->get_node_type()) ;
	// L'operation peut etre locale
	if ( (spec_op == NULL) && (root->get_machine_type() == MTYPE_IMPLEMENTATION) )
	  {
		spec_op = root->find_local_op(name) ;
	  }

	if (spec_op == NULL && abstract->get_specification()->get_machine_type() != MTYPE_SYSTEM)
	  {
		// L'operation n'est pas specifiee !
		semantic_error(cur_link,
					   CAN_CONTINUE,
					   get_error_msg(E_OPERATION_IS_NOT_SPECIFIED),
					   name->get_value(),
					   abstract->get_machine_name()->get_name()->get_value()) ;
	  }
	else if (spec_link != NULL) // Null pour EVOL_LOCAL_OP
	  {
		TRACE2("operation %s : mise a jour du tmp de %x a TRUE",
			   name->get_value(),
			   spec_link) ;
		assert(spec_link->get_node_type() == NODE_LIST_LINK) ;
		spec_link->set_tmp((void *)TRUE) ;
	  }

	// Mise � jour de l'anneau de collage pour les nom des op�rations
	if (spec_op != NULL)
	  {
		// R�cup�ration des noms des op�rations.
		ASSERT(cur_op->is_an_operation());
		ASSERT(spec_op->is_an_operation());
		T_ident *spec_id = ((T_generic_op *) spec_op)->get_name();
		T_ident *cur_id  = ((T_generic_op *)  cur_op)->get_name();

		spec_id->insert_in_glued_ring(cur_id) ;
	  }

	cur_link = (T_list_link *)cur_link->get_next() ;
	}
}

// Fonction auxilaire
// Verifier que toutes les operations sont raffinees
static void check_op_in_imp(T_machine *root, T_machine *abstract)
{
  TRACE2("check_op_in_imp(%x, %x)", root, abstract) ;
  // was : T_list_link *cur_link = abstract->get_op_list() ;
  // Cf bug 1958 : seule la  spec a la liste complete des operations,
  // le raffinement peut n'en avoir qu'une partie
  T_list_link *cur_link = abstract->get_specification()->get_op_list() ;

  while (cur_link != NULL)
	{
          if (cur_link->get_tmp_int() == FALSE)
		{
		  // L'operation n'est pas raffinee (car sinon on aurait TRUE)
		  T_object *cur_op = cur_link->get_object() ;
		  T_symbol *name ;

		  ASSERT(    (cur_op->get_node_type() == NODE_OPERATION)
					 || (cur_op->get_node_type() == NODE_USED_OP) ) ;
		  name = ((T_generic_op *)cur_op)->get_op_name() ;

		  TRACE2("operation %s : le a jour du tmp de %x vaut FALSE",
				 name->get_value(),
				 cur_link) ;
		  semantic_error((T_item *)cur_op, // cast valide par construction
						 CAN_CONTINUE,
						 get_error_msg(E_OPERATION_IS_NOT_REFINED),
						 name->get_value(),
						 root->get_machine_name()->get_name()->get_value()) ;
		}

	  cur_link = (T_list_link *)cur_link->get_next() ;
	}
}

// Fonction auxilaire
// Verifier que toutes les operations sont raffinees
static void check_local_op_in_imp(T_machine *root)
{
  TRACE1("check_local_op_in_imp(%x)", root) ;
  T_op *cur_local_op = root->get_local_operations() ;

  while (cur_local_op != NULL)
	{
	  T_generic_op *cur_impl = root->find_op(cur_local_op->get_op_name(), NULL) ;

	  if (cur_impl == NULL)
		{
		  // L'operation n'est pas implementee
		  semantic_error(cur_local_op,
						 CAN_CONTINUE,
						 get_error_msg(E_LOCAL_OPERATION_IS_NOT_IMPLEMENTED),
						 cur_local_op->get_op_name()->get_value(),
						 root->get_machine_name()->get_name()->get_value()) ;
		}
	  else
		{
		  //mise � jours du champs homonym_in_abstraction pour l'op�ration
		  set_homonym_in_abstraction(cur_impl, cur_local_op);
		}
	  cur_local_op = (T_op *)cur_local_op->get_next() ;
	}
}

//GP 30/12/98
//Fonction auxiliare qui recherche un operateur dans une liste
//de machines requises.
T_generic_op* find_op_in_list(T_used_machine *cur_list,
									   T_used_op* operation)
{
  T_symbol* op_instance_name = operation->get_instance_name() ;
  T_symbol* op_name = operation->get_real_op_name() ;
  T_symbol* op_full_name = operation->get_name()->get_name() ;

   for (;;)
	 {
	  if (cur_list == NULL)
		{
		  return NULL ;
		}

	  T_generic_op *res = NULL ;
	  ASSERT(cur_list->get_ref_machine() != NULL) ;
	  if (cur_list->get_instance_name() != NULL)
		{
		  // Machine renomm�e
		  if (op_instance_name != NULL &&
			  cur_list->get_instance_name()->compare(op_instance_name) == TRUE)
			{
			  // M�me pr�fixe que l'op�ration promue.
			  res = cur_list->get_ref_machine()->find_op(op_name, NULL) ;
			}
		}
	  else
		{
		  // Machine non renomm�e, on cherche avec le nom complet.
		  res = cur_list->get_ref_machine()->find_op(op_full_name) ;
		}

	  if (res != NULL)
		{
		  TRACE0("trouve !") ;
		  return res ;
		}

	  cur_list = (T_used_machine *)cur_list->get_next() ;
	 }
}

//GP 30/12/98
//Verification sur les operations promues
//(1) qu'elles proviennent de machines includes ou imports
//(2) une operation ne peut pas etre promue plusieurs fois
static void check_promotes_op(T_machine* root)
{
  T_used_op * promote_op = root->get_promotes() ;

  T_generic_op* res = NULL ;

  while(promote_op != NULL)
	{

	  res = find_op_in_list(root->get_includes(), promote_op) ;
	  if(res == NULL)
		{
		  res = find_op_in_list(root->get_imports(),promote_op) ;
		  if (res == NULL)
			{
			  //(1) une operation promue doit provenir d'une machine incluse
			  //ou importee
			 semantic_error(promote_op,
					 CAN_CONTINUE,
					 get_error_msg(E_UNKNOWN_OPERATION_CAN_NOT_BE_PROMOTED),
					 promote_op->get_op_name()->get_value()) ;
			}
		}

	  if(res != NULL)
		{
		  set_homonym_in_required_mach(promote_op, res);
		  if (root->get_machine_type() == MTYPE_IMPLEMENTATION)
			{
			  set_implemented_by(promote_op, res);
			}
		}

	  //(2) On ne peut pas promouvoir deux fois une operation
	  T_used_op *check_op = (T_used_op*)promote_op->get_next() ;
	  while(check_op != NULL)
		{
		  if(promote_op->get_op_name()->compare(check_op->get_op_name()))
			{
			  double_identifier_error(check_op->get_name(),
									  promote_op->get_name());
			}
		  check_op = (T_used_op*)check_op->get_next() ;
		}

	  //Cast vrai par construction
	  promote_op = (T_used_op*)promote_op->get_next() ;
	}
}





// Fonction auxilaire qui appelle build_op_list sur une liste de
// T_used_machine
static void aux_build_op_list(T_used_machine *list)
{
#ifdef TRACE
  // En mode TRACE, on sort tout de suite pour ne pas polluer les traces
  if (list == NULL)
	{
	  return ;
	}
#endif
  TRACE1("aux_build_op_list(%x)", list) ;
  ENTER_TRACE ;

  T_used_machine *cur = list ;

  while (cur != NULL)
	{
	  TRACE2("CONSTRUCTION DE LA LISTE POUR %s (%x)",
			 cur->get_component_name()->get_value(),
			 cur) ;
	  if (cur->get_ref_machine() != NULL)
		{
		  // Pour la recursivite, il faut appeler op_analysis et
		  // pas build_op_list. Ainsi, si la machine courante utilise
		  // AA et que AA utilise BB, alors on construit la liste pour
		  // BB. Donc de AA on peut promovoir des operations de BB, etc.
		  // Ce qui n'est pas le cas si on appelle simplement build_op_list
		  op_analysis(cur->get_ref_machine()) ;
		  //		  build_op_list(cur->get_ref_machine()) ;
		}
#ifdef TRACE
	  else
		{
		  TRACE1("?????????????? %x->ref_machine = NULL", cur) ;
		}
#endif
	  cur = (T_used_machine *)cur->get_next() ;
	}

  EXIT_TRACE ;
  TRACE1("fin de aux_build_op_list(%x)", list) ;
}

// Analyse semantique : controles au niveau composant sur les operations
void op_analysis(T_machine *root)
{
  if (root->get_op_analysed() == TRUE)
	{
	  TRACE2("Op�rations d�j� analys�es (%x : %s)",
			 root,
			 root->get_machine_name()->get_name()->get_value()) ;
	}
  else
	{
	  root->set_op_analysed(TRUE) ;

	  TRACE2("DEBUT op_analysis(%x : %s)",
			 root,
			 root->get_machine_name()->get_name()->get_value()) ;
	  ENTER_TRACE ;
	  T_machine *abstract = root->get_ref_abstraction() ;

	  // Pour toutes les machines : Construction de la liste de toutes
	  // les operations On fait les machines requises tout d'abord
	  // afin de pouvoir se baser sur leur op_list

	  // Machines requises ...
	  aux_build_op_list(root->get_sees()) ;
	  aux_build_op_list(root->get_includes()) ;
	  aux_build_op_list(root->get_imports()) ;
	  aux_build_op_list(root->get_extends()) ;
	  aux_build_op_list(root->get_uses()) ;

	  // Abstractions ...
	  T_machine *abs = root->get_ref_specification() ;

	  while (abs != NULL)
		{
		  TRACE1("Debut Fabrication de la liste pour %s",
				 abs->get_machine_name()->get_name()->get_value()) ;
		  ENTER_TRACE ;

		  //	build_op_list(abs) ;
		  op_analysis(abs) ;
		  EXIT_TRACE ;

#if 0
		  // SL 24/03/98 ca sert a rien car ca se fait eclater ci-apres ...
		  //GP 28/12/98 attention si on le rajoute ca casse
		  //l'algo d'anti-collision des operateurs.
		  add_abstract_op(abs,
						  (T_item **)abs->get_adr_first_op_list(),
						  (T_item **)abs->get_adr_last_op_list()) ;
#endif

		  TRACE1("Fin Fabrication de la liste pour %s",
				 abs->get_machine_name()->get_name()->get_value()) ;
		  abs = abs->get_ref_refinement() ;
		}

	  // Composant en cours d'analyse
	  build_op_list(root) ;

	  if (abstract != NULL)
		{
		  // Mise a jour des timestamps pour la recherche
		  // i.e. tmp a FALSE dans les op de la spec
		  // Permet de detecter en Passe 2 les operations dont on
		  // est sur qu'elles sont implementees car
		  // on leur a mis tmp a TRUE dans la Passe 1
		  T_list_link *cur = abstract->get_op_list() ;
		  while (cur != NULL)
			{
			  cur->set_tmp((void *)FALSE) ;
			  cur = (T_list_link *)cur->get_next() ;
			}

		  // Passe 1
		  // Verifier que toutes les operations sont specifiees
		  // CTRL Ref : CSEM 1-1
		  // CTRL Ref : PROJ 2-4
		  check_op_in_spec(root, abstract) ;

		  // Passe 2 : cas d'une implementation
		  // Verifier que toutes les operations specifees sont implementees
		  // CTRL Ref : CSEM 1-1
		  // CTRL Ref : PROJ 2-3
		  if (root->get_machine_type() == MTYPE_IMPLEMENTATION
				&& abstract->get_specification()->get_machine_type() != MTYPE_SYSTEM)
			{
			  check_op_in_imp(root, abstract) ;
			}

		  // Passe 3 : cas d'une implementation
		  // Verifier que toute operation locale est implantee
		  // CTRL Ref : SEMLOC 1
		  if (root->get_machine_type() == MTYPE_IMPLEMENTATION
				&& abstract->get_specification()->get_machine_type() != MTYPE_SYSTEM)
			{
			  check_local_op_in_imp(root) ;
			}

		}
	  // GP 30/12/98
	  //Passe 3 : v�rification sur les operations promues
	  // CTRL Ref : CSEM 2
	  // CTRL Ref : CSEM 3
	  check_promotes_op(root) ;

	  EXIT_TRACE ;
	  TRACE1("FIN   op_analysis(%x)", root) ;
	}
}

static void set_homonym_in_abstraction( T_generic_op *op, T_generic_op *op_abs)
{
  T_ident *op_name = op->get_name();
  T_ident *op_abs_name = op_abs->get_name();
  op_name->set_homonym_in_abstraction(op_abs_name);
}

static void set_homonym_in_required_mach( T_generic_op *op, T_generic_op *op_req)
{
  T_ident *op_name = op->get_name();
  T_ident *op_req_name = op_req->get_name();
  op_name->set_homonym_in_required_mach(op_req_name);
}

static void set_implemented_by( T_generic_op *op, T_generic_op *op_req)
{
  T_ident *op_name = op->get_name();
  T_ident *op_req_name = op_req->get_name();
  op_name->set_implemented_by(op_req_name);
}


static void set_explicitly_declared( T_generic_op *op, int explicitl)
{
  T_ident *op_name = op->get_name();

  op_name->set_explicitly_declared(explicitl);
}

// la fonction parcours la liste des op�rations  et cherche un homonyme
// dans l'abstraction, dans le cas d'un succes, elle cr�e les liens
// d'homonymie sur l'abstraction.
void solve_homonym_in_abstraction_op(T_machine *root)
{
  TRACE1("--> solve_homonym_in_abstraction_op(%s)",
		 root->get_machine_name()->get_name()->get_value());
  T_machine *abstract = root->get_ref_abstraction();
  TRACE1("abstraction: %s",abstract?abstract->get_machine_name()->get_name()->get_value():"null");
  if (abstract != NULL)
	{
	  T_list_link *cur_link = root->get_op_list();
	  TRACE1("cur_link = %s",cur_link?"non null":"null");
	  while (cur_link != NULL)
		{
		  	T_item *cur_op = (T_item *)cur_link->get_object() ;

			ASSERT(    (cur_op->get_node_type() == NODE_OPERATION)
					   || (cur_op->get_node_type() == NODE_USED_OP) ) ;
			T_symbol *name ;
			name = ((T_generic_op *)cur_op)->get_op_name() ;

			T_generic_op *abs_op = abstract->find_op(name) ;
			TRACE1("homonyum trouv�: %x",abs_op);
			if (abs_op != NULL)
			  {
				set_homonym_in_abstraction((T_generic_op *)cur_op, abs_op);
			  }
			cur_link = (T_list_link *)cur_link->get_next() ;
		}

	}
  TRACE0("<--solve_homonym_in_abstraction_op");

}
