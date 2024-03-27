/******************************* CLEARSY **************************************
* Fichier : $Id: s_subst.cpp,v 2.0 2005-04-25 10:46:10 cc Exp $
* (C) 2008 CLEARSY
*
* Description :		Compilateur B
*					Substitutions
*
* Compilations:		-DENABLE_PAREN_SUBST pour autoriser le parenthesage des
*					substitutions
*					-DDEBUG_SYNTAX pour les traces de l'analyseur
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
RCS_ID("$Id: s_subst.cpp,v 1.23 2005-04-25 10:46:10 cc Exp $") ;

/* Includes systeme */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

/* Includes Composant Local */
#include "c_api.h"
#include "s_synta.h"
#include "s_lr.h"

//
//	}{ Fonction qui parse les substitutions "atomiques" :
//		Substitution_devient_�gal
//		Substitution_devient_elt
//		Substitution_devient_tel_que
//		Substitution_appel
//
// Fonction qui parse les substitutions
T_substitution *syntax_get_atomic_substitution(T_item **adr_first,
														T_item **adr_last,
														T_item *father,
														T_betree *betree,
														T_lexem **adr_cur_lexem)
{
#ifdef DEBUG_SYNTAX
  TRACE1("debut syntax_get_atomic_substitution cur_lexem %s",
		 (*adr_cur_lexem)->get_lex_ascii()) ;

  TRACE0("Appel parseur LR") ;
#endif

  T_item *res = syntax_call_LR_parser(adr_first,
									  adr_last,
									  father,
									  betree,
									  adr_cur_lexem) ;

#ifdef DEBUG_SYNTAX
  if (res->get_node_type() == NODE_OP_RESULT)
	{
	  TRACE1("op_result name = %s", ptr_ref(((T_op_result *)res)->get_op_name())) ;
	}
#endif

  return (T_substitution *)res ;
}

//
//	}{	Fonction generale qui parse les substitutions
//
T_substitution *syntax_get_substitution(T_item **adr_first,
												 T_item **adr_last,
												 T_item *father,
												 T_betree *betree,
												 T_lexem **adr_cur_lexem
#ifdef ENABLE_PAREN_IDENT
												 // Parametre supplementaire
												 // Si TRUE pas produire
												 // de msg d'erreur en cas
												 // de pb
												 , int no_error_msg
#endif
												 )
{
  TRACE0("debut syntax_get_substitution") ;

  T_lexem *cur_lexem = *adr_cur_lexem ;

  //	Substitution ::=
  //	-- Premier groupe : reconnaissable au premier lexeme -> c_subblk.cpp
  //		Substitution_bloc			(BEGIN)					OK
  //		Substitution_nulle			(skip)					OK
  //		Substitution_pr�condition	(PRE)					OK
  //		Substitution_assertion		(ASSERT)				OK
  //		Substitution_witness		(WITNESS)				OK
    //		Substitution_choix_born�	(CHOICE)				OK
  //		Substitution_if				(IF)					OK
  //		Substitution_select			(SELECT)				OK
  //		Substitution_case			(CASE)					OK
  //		Substitution_any			(ANY)					OK
  //		Substitution_let			(LET)					OK
  //		Substitution_var			(VAR)					OK
  //		Substitution_while			(WHILE)					OK
  //	-- Deuxieme groupe :: commencent toutes par un identifiant	-> c_subato.cpp
  //		Substitution_devient_�gal
  //		Substitution_devient_elt
  //		Substitution_devient_tel_que
  //		Substitution_appel
  //	-- Dernier groupe : detectable si on a un L_SCOL ou un L_PARALLEL
  //	   apres une substitution
  //		Substitution_s�quencement							OK
  //		Substitution_simultan�e								OK

  syntax_check_eof(cur_lexem) ;

  T_substitution *res = NULL ;

  T_machine_type mtype = betree->get_root()->get_machine_type() ;
  TRACE1("cur_lexem = %s", cur_lexem->get_lex_name()) ;

  // Initialisation du gestionnaire de projets, si ce n'est deja fait
  init_project_manager() ;

  // Chargement du projet
  T_project *project = get_project_manager()->get_current() ;

  int isValidationProject = 0;

  if (project != NULL) {
      isValidationProject = project->isValidation() ;
  }
  else
  {
      isValidationProject = (T_project::get_default_project_type() == PROJECT_VALIDATION);
  }

  switch(cur_lexem->get_lex_type())
	{
	case L_SKIP :
	  {
		res = new T_skip(adr_first, adr_last, father, betree, &cur_lexem) ;
		break ;
	  }

	case L_BEGIN :
	  {
		res = new T_begin(adr_first, adr_last, father, betree, &cur_lexem) ;
		break ;
	  }

	case L_CHOICE :
	  {
		// Cette clause n'est pas valide dans les implementations
		// CTRL Ref : CSYN 3-6
		if ( (mtype == MTYPE_IMPLEMENTATION)
			 && (get_B0_allow_choice() == FALSE) )
		  {
			syntax_error(cur_lexem,
						 CAN_CONTINUE,
						 get_error_msg(E_CHOICE_FORBIDDEN_IN_IMP)) ;
		  }
		res = new T_choice(adr_first, adr_last, father, betree, &cur_lexem) ;
		break ;
	  }

	case L_PRE :
	  {
		// Cette clause n'est pas valide dans les implementations
		// CTRL Ref : CSYN 3-5
		if ( (mtype == MTYPE_IMPLEMENTATION)
			 && (get_B0_allow_pre() == FALSE) )
		  {
			syntax_error(cur_lexem,
						 CAN_CONTINUE,
						 get_error_msg(E_PRE_FORBIDDEN_IN_IMP)) ;
		  }
		res = new T_precond(adr_first, adr_last, father, betree, &cur_lexem) ;
		break ;
	  }

	case L_ASSERT :
	  {
		res = new T_assert(adr_first, adr_last, father, betree, &cur_lexem) ;
		break ;
	  }

        case L_LABEL :
          {
                res = new T_label(adr_first, adr_last, father, betree, &cur_lexem) ;
                break ;
          }

        case L_JUMPIF :
          {
                res = new T_jumpif(adr_first, adr_last, father, betree, &cur_lexem) ;
                break ;
          }

        case L_WITNESS :
	  {
		res = new T_witness(adr_first, adr_last, father, betree, &cur_lexem) ;
		break ;
	  }

	case L_IF :
	  {
		res = new T_if(adr_first, adr_last, father, betree, &cur_lexem) ;
		break ;
	  }

#ifdef BALBULETTE
	case L_WHEN :
	  {
		cur_lexem->set_lex_type(L_SELECT) ;
	  }
	  // fall through
#endif

	case L_SELECT :
	  {
		// Cette clause n'est pas valide dans les implementations
		// CTRL Ref : CSYN 3-7
		if ( (mtype == MTYPE_IMPLEMENTATION)
             && (get_B0_allow_select() == FALSE) )
		  {
			syntax_error(cur_lexem,
						 CAN_CONTINUE,
						 get_error_msg(E_SELECT_FORBIDDEN_IN_IMP)) ;
		  }
		res = new T_select(adr_first, adr_last, father, betree, &cur_lexem) ;
		break ;
	  }

	case L_CASE :
	  {
		res = new T_case(adr_first, adr_last, father, betree, &cur_lexem) ;
		break ;
	  }

	case L_LET :
	  {
		// Cette clause n'est pas valide dans les implementations
		// CTRL Ref : CSYN 3-9
		if ( (mtype == MTYPE_IMPLEMENTATION)
			 && (get_B0_allow_let() == FALSE) )

		  {
			syntax_error(cur_lexem,
						 CAN_CONTINUE,
						 get_error_msg(E_LET_FORBIDDEN_IN_IMP)) ;
		  }
		res = new T_let(adr_first, adr_last, father, betree, &cur_lexem) ;
		break ;
	  }

	case L_VAR :
	  {
		if (mtype == MTYPE_SPECIFICATION)
		  {
			// Cette clause est interdite en specification,
			// CTRL Ref : CSYN 3-1
                if (!isValidationProject && (get_allow_var_sub_in_machine() == FALSE))  // clause permise pour les projets de type VALIDATION
			syntax_error(cur_lexem,
						 CAN_CONTINUE,
						 get_error_msg(E_VAR_FORBIDDEN_IN_SPEC)) ;
		  }

		res = new T_var(adr_first, adr_last, father, betree, &cur_lexem) ;
		break ;
	  }

	case L_WHILE :
	  {
		switch(mtype)
		  {
			// Cette clause n'est valide que dans les implementations
			// CTRL Ref : CSYN 3-2
		  case MTYPE_SPECIFICATION :
			{
                          if ((!isValidationProject) && (get_allow_while_sub_everywhere() == FALSE))// clause permise pour les projets de type VALIDATION
			  syntax_error(cur_lexem,
						   CAN_CONTINUE,
						   get_error_msg(E_WHILE_FORBIDDEN_IN_SPEC)) ;
			  break ;
			}

		  case MTYPE_REFINEMENT :
			{
            if (get_allow_while_sub_everywhere() == FALSE)
                syntax_error(cur_lexem,
                             CAN_CONTINUE,
                             get_error_msg(E_WHILE_FORBIDDEN_IN_REF)) ;
                break ;
			}

		  default :
			{
			  // On ne fait rien
			}
		  }

		res = new T_while(adr_first, adr_last, father, betree, &cur_lexem) ;
		break ;
	  }

	case L_ANY :
	  {
		// Cette clause n'est pas valide dans les implementations
		// CTRL Ref : CSYN 3-8
		if ( (mtype == MTYPE_IMPLEMENTATION)
			 && (get_B0_allow_any() == FALSE) )
		  {
			syntax_error(cur_lexem,
						 CAN_CONTINUE,
						 get_error_msg(E_ANY_FORBIDDEN_IN_IMP)) ;
		  }
		res = new T_any(adr_first, adr_last, father, betree, &cur_lexem) ;
		break ;
	  }

	case L_IDENT :
	case L_RENAMED_IDENT :
#ifdef ALLOW_PAREN_AROUND_IDENT
    case L_OPEN_PAREN :
#endif
	  {
		// Quatre possibilit�s "atomiques" restent:
		//		Substitution_devient_�gal
		//		Substitution_devient_elt
		//		Substitution_devient_tel_que
		//		Substitution_appel
		res = syntax_get_atomic_substitution(adr_first,
											 adr_last,
											 father,
											 betree,
											 &cur_lexem) ;
		break ;
	  }


#ifdef ENABLE_PAREN_SUBST
	  // On autorise le parenthesage des substitutions
	case L_OPEN_PAREN :
	  {
		T_lexem *open_lexem = *adr_cur_lexem ;
		TRACE0("PARENTHESE OUVRANTE DETECTEE") ;
		ENTER_TRACE ;

		// On avance d'un cran
		T_lexem *save_lexem = *adr_cur_lexem ;

#ifdef COMPAT
		// Memorisation de '('
		// En fait on pourrait se servir de save_lexem mais on introduit
		// une variable dediee pour plus de lisibilite
		T_lexem *open_paren = save_lexem ;
#endif // COMPAT

		*adr_cur_lexem = (*adr_cur_lexem)->get_next_lexem() ;

		// Avant l'appel recursif, on memorise le dernier unsolved_ident
		// en cas d'echec, le bout d'abre construit sera detruit et tous
		// les unsolved_ident supplementaires crees "supprimes"
		// Pour cela, le plus simple est de memoriser le dernier
		// element de la liste
		// Idem pour la liste des idents
		T_ident *last_unsolved_ident ;
		T_ident *last_ident ;
		last_unsolved_ident = get_object_manager()->get_last_unsolved_ident() ;
		last_ident = get_object_manager()->get_last_ident() ;

		// On s'appelle recursivement
		res = syntax_get_substitution(adr_first,
									  adr_last,
									  father,
									  betree,
									  adr_cur_lexem,
									  TRUE) ; // pas de msg d'erreur en cas de pb

		if (res == NULL)
		  {
			// L'analyse a echoue .. on essaie une derniere fois, en
			// remettant la parenthese dans le flux de lexemes
			// C'est le cas de l'analyse de "(x) : (x : ENS)" :
			// pour le premier essai, la boucle generale mange la
			// premiere parenthese et on essaie d'analyser
			// "x) : (x : ENS)
			TRACE0("PB ANALYSE ! On reessaie sans la parenthese") ;

			TRACE1("on commence par supprimer le mauvais noeud %x", *adr_last) ;
			(*adr_last)->remove_from_list(adr_first, adr_last) ;

			TRACE0("on supprimer tous les idents parasites") ;
			get_object_manager()->set_last_ident(last_ident) ;

			TRACE0("on supprimer tous les unsolved_idents parasites") ;
			get_object_manager()->set_last_unsolved_ident(last_unsolved_ident) ;

			TRACE0("et on essaie une substitution atomique") ;

			// Quatre possibilit�s "atomiques" restent:
			//		Substitution_devient_�gal
			//		Substitution_devient_elt
			//		Substitution_devient_tel_que
			//		Substitution_appel
			cur_lexem = save_lexem ;
			res = syntax_get_atomic_substitution(adr_first,
												 adr_last,
												 father,
												 betree,
												 &cur_lexem) ;
			TRACE0("on peut maintenant analyser le resultat sans parenthese") ;
			break ;
		  }
		else
		  {
			// Il doit y avoir une parenthese fermante
			syntax_check_eof(*adr_cur_lexem) ;

			if ((*adr_cur_lexem)->get_lex_type() != L_CLOSE_PAREN)
			  {
				syntax_error(*adr_cur_lexem,
							 CAN_CONTINUE,
							 get_error_msg(E_NO_CLOSE_PAREN_MATCHING_OPEN_PAREN)) ;
				syntax_error(open_lexem,
							 CAN_CONTINUE,
							 get_error_msg(E_OPEN_PAREN_LOCATION)) ;
			  }


			EXIT_TRACE ;
			TRACE0("PARENTHESE FERMANT DETECTEE") ;

#ifdef COMPAT
			// On marque le '(' et le ')' comme L_VOID c'est a dire
			// des lexemes qu'on ne decompile pas
			open_paren->mark_as_void() ;
			(*adr_cur_lexem)->mark_as_void() ;
#endif // COMPAT


			*adr_cur_lexem = (*adr_cur_lexem)->get_next_lexem() ;
			cur_lexem = *adr_cur_lexem ;
			TRACE1("on continue avec cur_lexem %x", cur_lexem) ;
			break ;
			//		return res ;
		  }
	  }
#endif

	default :
	  {
		// Cas general : erreur
		// Cas particulier : on est apres un ';' qui  ';' etait superflu
		T_lexem *prev_lexem = cur_lexem->get_prev_lexem() ;
		if (prev_lexem->get_lex_type() == L_SCOL)
		  {
			// Ici on a un ';' superflu. On n'en tient pas compte
			if (get_strict_B_demanded() == TRUE)
			  {
				// En mode strict, on n'accepte pas les ';' superflus
				syntax_warning(prev_lexem,
                               BASE_WARNING,
							   get_warning_msg(W_EXTRA_SCOL_NOT_STRICT_B)) ;
			  }
			TRACE0("fin de la sequence ...") ;
			return NULL ;
		  }

		// Cas general : erreur
		TRACE0("ici : on crie") ;
		syntax_unexpected_error(cur_lexem, FATAL_ERROR, get_catalog(C_SUBST)) ;
	  }
	}

  if (res->is_a_substitution() == FALSE)
	{
	  // Le parseur LR peut rendre des T_item qui ne sont pas des substitutions
	  // dans le cas les cas suivants :

	  // 1) appel de fonction. Il rend alors un T_op_result si
	  //		l'operation a des parametres d'entree, ou un T_ident si elle n'a
	  //	   pas de parametre. Il faut alors construire un T_op_call correspondant
	  // 		sinon on compromet l'integrite de la liste
	  //
	  // 2) Expression du type xx : (x devient tel que). Il rend alors un
	  //		T_typing_predicate qu'il faut transformer en T_becomes_such_that
	  TRACE1("ATTENTION : res n'est pas une substitution mais un %s",
			 res->get_class_name()) ;

	  T_substitution *new_res = NULL ; // le nouveau noeud
	  switch (res->get_node_type())
		{
		case NODE_OP_RESULT :			// cas 1)
		  {
			TRACE0("on transforme le OP_RESULT en OP_CALL") ;
            bool done = false ;
            T_op_result *op_result = reinterpret_cast<T_op_result *>(res) ;
            while (!done)
              {
                switch (op_result->get_op_name()->get_node_type())
                  {
#ifdef ALLOW_PAREN_AROUND_IDENT
                    case NODE_EXPR_WITH_PARENTHESIS :
                      op_result->set_op_name(static_cast<T_expr_with_parenthesis *>(op_result->get_op_name())->get_expr()) ;
                      break ;
#endif
                    default :
                      new_res = new T_op_call(op_result, father, betree) ;
                      done = true ;
                      break ;
                  }
              }
			break ;
		  }

		case NODE_IDENT :				// cas 1)
		case NODE_RENAMED_IDENT :
		  {
			TRACE0("on transforme le IDENT en OP_CALL") ;
			new_res = new T_op_call((T_ident *)res, father, betree) ;
			break ;
		  }

#ifdef ALLOW_PAREN_AROUND_IDENT
        case NODE_EXPR_WITH_PARENTHESIS :
          {
            bool done = false ;
            T_expr_with_parenthesis *expr_with_parenthesis = reinterpret_cast<T_expr_with_parenthesis *>(res) ;
            while (!done)
              {
                switch (expr_with_parenthesis->get_expr()->get_node_type())
                  {
                    case NODE_IDENT :
                    case NODE_RENAMED_IDENT :
                      {
                        new_res = new T_op_call((T_ident *)expr_with_parenthesis->get_expr(), father, betree) ;
                        done = true ;
                      }
                      break ;
                    case NODE_EXPR_WITH_PARENTHESIS :
                      expr_with_parenthesis = static_cast<T_expr_with_parenthesis *>(expr_with_parenthesis->get_expr()) ;
                      break ;
                    default :
                      syntax_substitution_expected_error(res, FATAL_ERROR) ;
                      break ;
                  }
              }
            break ;
          }
#endif

		case NODE_TYPING_PREDICATE : // cas 2)
		  {
			T_typing_predicate *tpred = (T_typing_predicate *) res ;
            if (tpred->get_typing_predicate_type() != TPRED_BELONGS
                    or tpred->get_type()->get_node_type() != NODE_PREDICATE_WITH_PARENTHESIS)
			  {
				syntax_substitution_expected_error(res, CAN_CONTINUE) ;

				// On utilise un T_skip pour pouvoir continuer.
				new_res = new T_skip(NULL, NULL, father, betree, NULL) ;
			  }
			else
			  {
				TRACE1("on transforme le TYPING_PREDICATE %x en BECOMES_SUCH_THAT",
					   res) ;
				new_res = new T_becomes_such_that((T_typing_predicate *)res,
												  father,
												  betree) ;
				if ( (betree->get_root()->get_machine_type() == MTYPE_IMPLEMENTATION)
					 && (get_B0_allow_becomes_such_that() == FALSE) )
				  {
					// Cette substitution est interdite en implementation
					// CTRL Ref : CSYN 3-11
					syntax_error(new_res->get_ref_lexem(),
								 CAN_CONTINUE,
								 get_error_msg(E_BECOMES_SUCH_THAT_FORBIDDEN_IN_IMP)) ;
				  }
			  }

			break ;
		  }

		default :
		  {
			// Aie !
#ifdef ENABLE_PAREN_IDENT
			TRACE2("ERROR : UNEXPECTED CLASS %s, no_error_msg = %s",
				   res->get_class_name(),
				   (no_error_msg == TRUE) ? "TRUE" : "FALSE") ;
			if (no_error_msg == TRUE)
			  {
				// Mode special pour la "reprise sur erreur" (parseur qui
				// a avale un '(' de trop : on renvoie NULL, sans message
				// d'erreur
				// C'est le cas de l'analyse de "(x) : (x : ENS)" :
				// pour le premier essai, la boucle generale mange la
				// premiere parenthese et on essaie d'analyser
				// "x) : (x : ENS)
				TRACE0("on ne crie pas, on renvoie NULL") ;
				return NULL ;
			  }
#endif
			syntax_substitution_expected_error(res, FATAL_ERROR) ;
		  }
		}

	  TRACE2("on echange l'ancien noeud et le OP_CALL dans la liste adr_first %x adr_last %x", adr_first, adr_last) ;
	  res->exchange_in_list(new_res, adr_first, adr_last) ;

	  TRACE1("on supprime le OP_RESULT %x", res) ;
	  res->unlink() ;
	  res = new_res ;
	}

  *adr_cur_lexem = cur_lexem ;

  // On regarde la suite ...
  // ... seulement si le pere n'est pas une operation
  TRACE3("res %x %s get_father %x", res, res->get_class_name(), res->get_father()) ;
  assert(res->get_father()) ;
  ASSERT(res->is_a_substitution()) ;

  T_node_type father_type = res->get_father()->get_node_type() ;

  if (father_type == NODE_OPERATION)
	{
	  if (res->get_father()->get_father()->get_node_type() == NODE_FLAG)
		{
		  // C'est la clause INITIALISATION (cf Bug 1983) : le NODE_OPERATION
		  // a ete cree ex-nihilo mais pour le parsing cette clause n'est pas
		  // consideree comme une operation
		  father_type = NODE_FLAG ;
		}
	}
  TRACE1("father_type = %d", father_type) ;

  if ( 	(father_type != NODE_OPERATION)
		&& (*adr_cur_lexem != NULL) )
	{
	  TRACE0("le pere est un block .. on analyse la suite") ;
	  T_lex_type lex_type = (*adr_cur_lexem)->get_lex_type() ;

	  switch(lex_type)
		{
		case L_SCOL :
		  {
			// On detecte une sequence
			TRACE0("Sequence ; detectee ... on analyse la substitution suivante") ;
			ENTER_TRACE ;
                        if (mtype == MTYPE_SPECIFICATION && ! isValidationProject && (get_allow_sequential_sub_in_machine() == FALSE))
                        // clause permise pour les projets de type VALIDATION
			  {
				// La substitution ";" est interdite en specification
				// CTRL Ref : CSYN 3-3
				syntax_error(*adr_cur_lexem,
							 CAN_CONTINUE,
							 get_error_msg(E_SEQ_FORBIDDEN_IN_SPEC)) ;
			  }

			res->set_link_type(LINK_SEQUENCE) ;
			*adr_cur_lexem = (*adr_cur_lexem)->get_next_lexem() ;
			syntax_check_eof(*adr_cur_lexem) ;
			syntax_get_substitution(adr_first,
									adr_last,
									father,
									betree,
									adr_cur_lexem) ;
			EXIT_TRACE ;
			TRACE0("Fin analyse sequence") ;
			break ;
		  }

		case L_PARALLEL :
		  {
			// On detecte une sequence
			TRACE0("Sequence // detectee ... on analyse la substitution suivante") ;
			ENTER_TRACE ;


			if ( (mtype == MTYPE_IMPLEMENTATION)
				 && (get_B0_allow_parallel() == FALSE) )

			  {
				// La substitution "||" est interdite en implementation
				// CTRL Ref : CSYN 3-4
				syntax_error(*adr_cur_lexem,
							 CAN_CONTINUE,
							 get_error_msg(E_PARALLEL_FORBIDDEN_IN_IMP)) ;
			  }

			res->set_link_type(LINK_PARALLEL) ;
			*adr_cur_lexem = (*adr_cur_lexem)->get_next_lexem() ;
			syntax_check_eof(*adr_cur_lexem) ;
			syntax_get_substitution(adr_first,
									adr_last,
									father,
									betree,
									adr_cur_lexem) ;
			EXIT_TRACE ;
			TRACE0("Fin analyse sequence") ;
			break ;
		  }

		default :
		  {
			TRACE1("type %s : pas de sequence ...",
				   (*adr_cur_lexem)->get_lex_name()) ;
		  }
		}
	}

  TRACE0("fin syntax_get_substitution(%x)") ;
  return res ;
}



//
//	}{	Fin du fichier
//
