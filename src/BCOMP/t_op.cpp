/******************************* CLEARSY **************************************
* Fichier : $Id: t_op.cpp,v 2.0 2002-09-24 09:47:30 cm Exp $
*
* (C) 2008 CLEARSY
*
* Description :		Compilateur B
*					Type check pour les operations
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
RCS_ID("$Id: t_op.cpp,v 1.19 2002-09-24 09:47:30 cm Exp $") ;

/* Includes systeme */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

/* Includes Composant Local */
#include "c_api.h"
#include "v_ldreq.h"
#include "v_toplvl.h"
#include "c_tstamp.h"

// Classe T_op : on type !
void T_op::type_check(void)
{
    TRACE3("DEBUT TYPE CHECK OPERATION %x (%s:%s)",
           this,
           get_betree()->get_file_name()->get_value(),
           name->get_name()->get_value()) ;
    ENTER_TRACE ; ENTER_TRACE ;

    if (type_checked == TRUE)
    {
        TRACE0("deja fait") ;
        return ;
    }
    type_checked = TRUE ;

    // On verifie que tous les parametres d'netree/sortie sont
    // specifies. On en profite pour recuperer leur type
    // Le corps
    // Les types des parametres d'entree proviennent de la specification
    // ident pour les parametres de sortie
    T_machine *root = (T_machine *)get_father() ;
    ASSERT(root->get_node_type() == NODE_MACHINE) ;
    if (root->get_machine_type() != MTYPE_SPECIFICATION &&
            root->get_machine_type() != MTYPE_SYSTEM)
    {
        type_check_parameters(root) ;
    }

    if (root->get_machine_type() != MTYPE_SPECIFICATION &&
            root->get_machine_type() != MTYPE_SYSTEM)
    {
        type_check_ref(root);
    }

    if (get_Use_B0_Declaration() == TRUE)
    {
        // On type check les param�tres d'entr�e pour obtenir
        // leurs types
        T_ident *cur_ident = first_in_param ;
        while (cur_ident != NULL)
        {
            cur_ident->type_check();
            cur_ident = (T_ident *)cur_ident->get_next() ;
        }
        // On type check les param�tres de sortie
        cur_ident = first_out_param ;
        while (cur_ident != NULL)
        {
            cur_ident->type_check();
            cur_ident = (T_ident *)cur_ident->get_next() ;
        }
    }

    T_substitution *body;
    T_precond *precond;
    get_precond_and_body(&precond,&body);

    if(precond != NULL)
    {
        TRACE0("type check PRE");
        precond->get_predicate()->type_check();
    }

    if ( (root->get_machine_type() == MTYPE_SPECIFICATION)
         ||
         (is_a_local_op == TRUE) )
    {
        // Tous les parametres d'entree doivent etre types ...  le
        // typage � lieu au niveau de la spec de l'operation ie soit par
        // ce que op est dans une spec ou alors op est une spec
        // d'operation locale
        T_ident *cur_ident = first_in_param ;
        while (cur_ident != NULL)
        {
            if (cur_ident->get_B_type() == NULL)
            {
                TRACE2("pb %x %s pas type",
                       cur_ident,
                       cur_ident->get_name()->get_value()) ;
                semantic_error(cur_ident,
                               CAN_CONTINUE,
                               get_error_msg(E_NO_TYPE_FOR_IDENT),
                               cur_ident->get_ident_type_name(),
                               cur_ident->get_name()->get_value()) ;
            }
            else
            {
                TRACE2("Type du param�tre d'entr�e %s : %s",
                       cur_ident->get_name()->get_value(),
                       cur_ident->get_ident_type_name()) ;
            }
            cur_ident = (T_ident *)cur_ident->get_next() ;
        }

    }


    // Puis type check du corps
    TRACE0("type check du corps") ;
    type_check_substitution_list(body) ;

    TRACE0("fin type check corps");
    if ( (root->get_machine_type() == MTYPE_SPECIFICATION)
         ||
         (is_a_local_op == TRUE) )
    {
        // on doit verifier que tous les parametres de sortie sont typ�s
        // au niveau de la sec de l'operation ie soit par ce que op est
        // dans une spec ou alors op est une spec d'operation locale
        T_ident *cur_ident = first_out_param ;
        while (cur_ident != NULL)
        {
            if (cur_ident->get_B_type() == NULL)
            {
                semantic_error(cur_ident,
                               CAN_CONTINUE,
                               get_error_msg(E_NO_TYPE_FOR_IDENT),
                               cur_ident->get_ident_type_name(),
                               cur_ident->get_name()->get_value()) ;
            }
            cur_ident = (T_ident *)cur_ident->get_next() ;
        }

    }// fin de la verification des parametres de sorties d'op�ration

    EXIT_TRACE ; EXIT_TRACE ;
    TRACE3("FIN TYPE CHECK OPERATION %x:%s",
           this,
           get_betree()->get_file_name()->get_value(),
           name->get_name()->get_value()) ;
}


// Types check des parametres d'entree/sortie
void T_op::type_check_parameters(T_machine *root)
{
    T_machine *spec = root->get_ref_specification() ;
    ASSERT(spec) ;
    ASSERT(spec->get_node_type() == NODE_MACHINE) ;

    // Les parametres d'entree sont les memes que dans la specification
    // CTRL Ref : PROJ 2-9

    // Recherche de l'operation (propre ou promue)
    T_generic_op *used_spec_op = NULL ;
    if (is_a_local_op == TRUE)
    {
        // On est dans une implementation, dans la clause
        // LOCAL_OPERATIONS i.e. dans la specification de l'operation.
        used_spec_op = this ;
    }

    if (used_spec_op == NULL)
    {
        used_spec_op = root->find_local_op(name->get_name()) ;
    }

    if (used_spec_op == NULL)
    {
        // Cas general
        used_spec_op = spec->find_op(name->get_name()) ;
    }

    if (used_spec_op == NULL)
    {
        if (root->get_specification()->get_machine_type() != MTYPE_SYSTEM) {// Impossible : Garanti par PROJ 2-3 et PROJ 2-4
            internal_error(get_ref_lexem(),
                           __FILE__,
                           __LINE__,
                           "Unable to find specification of operation %s\n",
                           name->get_name()->get_value()) ;
        }
        else {
            return;
        }
    }

    // On cherche la definition
    while (used_spec_op->get_node_type() == NODE_USED_OP)
    {
        used_spec_op = ((T_used_op *)used_spec_op)->get_ref_op() ;
        ASSERT(used_spec_op != NULL) ;
    }
    T_op *spec_op = (T_op *)used_spec_op ; // Cast justifie par le while
    ASSERT(spec_op != NULL) ;
    ASSERT(spec_op->get_node_type() == NODE_OPERATION) ;


    // On v�rifie que les signature sont homog�ne entre
    // op�rations raffin�es
    if (get_Use_B0_Declaration() == TRUE
            &&
            spec_op->get_status() != get_status())
    {
        semantic_error(this,
                       CAN_CONTINUE,
                       get_error_msg(E_INVALID_OP_REFINEMENT),
                       name->get_name()->get_value());
    }

    // On verifie que tous les parametres d'entree sont specifies/raffines
    check_list(spec_op, first_in_param, spec_op->first_in_param) ;

    // Idem avec ceux de sortie
    check_list(spec_op, first_out_param, spec_op->first_out_param) ;

}


// Types check de la clause ref d'une operation
void  T_op::type_check_ref(T_machine *root)
{
    T_machine *abstract = root->get_ref_abstraction() ;
    ASSERT(abstract) ;

    T_ident *cur_list_ref = first_ref_op;

    // On verifie qu'un evenement qui a une clause ref n'est pas deja declare dans l'abstraction
    if (cur_list_ref != NULL)
    {
        while (abstract != NULL) {
            T_generic_op *used_spec_op = abstract->find_op(name->get_name());
            if (used_spec_op != NULL) {
                semantic_error(this,
                               CAN_CONTINUE,
                               get_error_msg(E_REFINED_EVENT_CANNOT_REF),
                               name->get_name()->get_value());
            }
            abstract = abstract->get_ref_abstraction() ;
        }
    }


    // Suppression de la contrainte selon laquelle on peut pas merger et splitter en meme temps un evenement
//    if (cur_list_ref != NULL && cur_list_ref->get_next() != NULL)
//    {
//        while (cur_list_ref != NULL)
//        {
//            T_list_link *cur_op_link = (T_list_link*) root->get_op_list();
//            while (cur_op_link != NULL) {
//                T_op *cur_op = (T_op *)cur_op_link->get_object();
//                T_ident *op_list_ref = cur_op->get_ref_operation();
//                while (op_list_ref != NULL) {
//                    if (cur_op != this && cur_list_ref->get_name()->compare(op_list_ref->get_name()) ==TRUE) {
//                        semantic_error(this,
//                                       CAN_CONTINUE,
//                                       get_error_msg(E_REFINE_AND_MERGE_EVENT),
//                                       name->get_name()->get_value(),
//                                       cur_list_ref->get_name()->get_value());
//                    }
//                    op_list_ref = (T_ident *)op_list_ref->get_next() ;
//                }
//                cur_op_link = (T_list_link*) cur_op_link->get_next();
//            }
//            cur_list_ref = (T_ident *)cur_list_ref->get_next() ;
//        }
//    }

    cur_list_ref = first_ref_op;
    while (cur_list_ref != NULL)
    {

        // On verifie que les evenements de la clause ref sont declares dans l'abstraction
        abstract = root->get_ref_abstraction() ;
        bool found = false;

        while (abstract != NULL && found == false) {
            T_generic_op *used_spec_op = abstract->find_op(cur_list_ref->get_name());

            if (used_spec_op != NULL) {
                found = true;
            }
            abstract = abstract->get_ref_abstraction();
        }

        if (found == false) {
            semantic_error(this,
                           CAN_CONTINUE,
                           get_error_msg(E_INVALID_EVENT_REF),
                           cur_list_ref->get_name()->get_value());
        }

        // On verifie que les evenements de la clause ref ne sont pas declares en double
        T_ident *double_list_ref = (T_ident *)cur_list_ref->get_next() ;
        while (double_list_ref != NULL)
        {
            if (cur_list_ref->get_name()->compare(double_list_ref->get_name()) ==TRUE) {
                semantic_error(this,
                               CAN_CONTINUE,
                               get_error_msg(E_DOUBLE_EVENT_REF),
                               cur_list_ref->get_name()->get_value());
            }
            double_list_ref = (T_ident *)double_list_ref->get_next() ;
        }

        cur_list_ref = (T_ident *)cur_list_ref->get_next() ;
    }


    // On verifie que les evenements de la clause ref sont aussi présents localement, sinon Warning
    cur_list_ref = first_ref_op;
    while (cur_list_ref != NULL)
    {
        T_generic_op *used_spec_op = root->find_op(cur_list_ref->get_name());
        if (used_spec_op == NULL) {
            semantic_warning(this,
                           BASE_WARNING,
                           get_warning_msg(W_REFINED_EVENT_NOT_CLOSED),
                           cur_list_ref->get_name()->get_value(),
                             name->get_name()->get_value());
        }
        cur_list_ref = (T_ident *)cur_list_ref->get_next() ;
    }

    // On verifie qu'une clause VARIANT est presente quand on genere les POs de divergence
    // et qu'il y a de nouveaux evenements.
    if (get_B0_generate_eventb_non_divergence_po() == TRUE
            && first_ref_op == NULL) {
        bool found = false;
        abstract = root->get_ref_abstraction() ;
        while (abstract != NULL) {
            T_generic_op *used_spec_op = abstract->find_op(name->get_name());
            if (used_spec_op != NULL) {
                found = true;
                break;
            }
            abstract = abstract->get_ref_abstraction() ;
        }
        if (!found && root->get_variant() == NULL) {
            semantic_error(this,
                           CAN_CONTINUE,
                           get_error_msg(E_VARIANT_CLAUSE_MANDATORY),
                           name->get_name()->get_value());
        }
    }
}


// Fonction auxiliaire de verification de liste
// On verifie que tous les elements de list1 sont dans list2
void T_op::check_list(T_op *spec_op,
                               T_ident *list_ref,
                               T_ident *list_spec)
{
    T_ident *cur_list_ref = list_ref ;
    T_ident *cur_list_spec = list_spec ;
    for (;;)
    {
        if (cur_list_spec == NULL)
        {
            if (cur_list_ref == NULL)
            {
                // Les deux listes se finissent en meme temps :
                // pas de probleme
                return ;
            }
            else
                // Ici : erreur. Il a y des parametres en trop par rapport
                // a la spec
                while (cur_list_ref != NULL)
                {
                    TRACE0("Trop de param�tres!") ;
                    semantic_error(this,
                                   CAN_CONTINUE,
                                   get_error_msg(E_TOO_MANY_PARAM_IN_REF_OP),
                                   this->get_op_name()->get_value() ) ;
                    semantic_error_details(spec_op,
                                           get_error_msg(E_LOCATION_OF_OP_SPEC)) ;
                    cur_list_ref = (T_ident *)cur_list_ref->get_next() ;
                }
            return ;
        }

        if (cur_list_ref == NULL)
        {
            // Forcement cur_list_ref != NULL et donc il y a erreur
            // Ici : erreur. Il manque des parametres par rapport
            // a la spec
            while (cur_list_spec != NULL)
            {
                TRACE0("Pas assez de param�tres!") ;
                semantic_error(this,
                               CAN_CONTINUE,
                               get_error_msg(E_TOO_FEW_PARAM_IN_REF_OP),
                               this->get_op_name()->get_value() ) ;
                semantic_error_details(spec_op,
                                       get_error_msg(E_LOCATION_OF_OP_SPEC)) ;
                cur_list_spec = (T_ident *)cur_list_spec->get_next() ;
            }
            return ;
        }

        // Ici, cas general : comparaison de cur_list_spec et de
        // cur_list_ref

        if (cur_list_ref->get_name()->compare(cur_list_spec->get_name()) ==FALSE)
        {
            semantic_error(this,
                           CAN_CONTINUE,
                           get_error_msg(E_PARAM_NAME_DIFF_1),
                           cur_list_spec->get_name()->get_value(),
                           cur_list_ref->get_name()->get_value() ) ;
            semantic_error_details(this,
                                   get_error_msg(E_PARAM_NAME_DIFF_2),
                                   this->get_op_name()->get_value() ) ;
            semantic_error_details(spec_op,
                                   get_error_msg(E_LOCATION_OF_OP_SPEC)) ;
        }
        else
        {
            // C'est bon, on recupere le type
            TRACE2("OUT Param %s :  C'est bon, on recupere le type %x",
                   cur_list_ref->get_name()->get_value(),
                   cur_list_spec->get_B_type()) ;
            T_type *spec_type = cur_list_spec->get_original_B_type() ;
            if (spec_type != NULL)// peut etre null en cas de reprise sur erreur
            {
                T_type *type = spec_type->clone_type(cur_list_ref,
                                                     get_betree(),
                                                     NULL) ;
                cur_list_ref->set_B_type(type, cur_list_ref->get_ref_lexem()) ;
                type->unlink() ;
            }
        }


        // La suite !
        cur_list_spec = (T_ident *)cur_list_spec->get_next() ;
        cur_list_ref = (T_ident *)cur_list_ref->get_next() ;
    }
}

//
//	}{	Fin du type check des operations
//
