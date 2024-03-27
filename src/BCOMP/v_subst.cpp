/******************************* CLEARSY **************************************
* Fichier : $Id: v_subst.cpp,v 2.0 1999-06-09 12:38:45 sl Exp $
* (C) 2008 CLEARSY
*
* Description :		Compilateur B
*					Substitutions : verification de non affectation en parallele
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
RCS_ID("$Id: v_subst.cpp,v 1.9 1999-06-09 12:38:45 sl Exp $") ;

/* Includes systeme */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

/* Includes Composant Local */
#include "c_api.h"

// Verifier que dans deux subsitiutions en parallele (this et ref_subst),
// les variables modifiees sont differentes
static int first_tag_si = 0 ;
static int second_tag_si = 0 ;
void T_substitution::parallel_checks(T_substitution *ref_subst)
{
  TRACE2("T_substitution(%x)::parallel_checkes(ref_subst=%x)", this, ref_subst) ;

  // Obtention de deux timestamps (1 par passe)
  next_timestamp() ;
  first_tag_si = get_timestamp() ;
  next_timestamp() ;
  second_tag_si = get_timestamp() ;

  // Pour chaque ident modifie par this, on met first_tag_si
  // dans tmp2 sa definition
  TRACE0("Liste des variables modifiees par this ...") ;
  create_list_of_modvar(TRUE) ;

  // Pour chaque ident modifie par ref_subst, on verifie que
  // le tmp2 de sa definition ne vaut pas first_tag_si.
  // Puis on met ce champ a second_tag_si
  TRACE0("Liste des variables modifiees par ref_subst ...") ;
  ref_subst->create_list_of_modvar(FALSE) ;

  //GP 20/01/99
  // CTRL Ref : CSEM 4
  //Il faut verifier que les substitutions n'utilise pas
  //plusieurs operations de la meme instance de machine incluses

  //Liste des instances de machines incluse dont
  //au moins une operation est utilises dans la substitution ||
  T_list_link* first_includes_machine = NULL ;
  T_list_link* last_includes_machine = NULL ;

  TRACE0("Operations d'une meme machine incluse utilises par ref_subst") ;
  TRACE2("this->get_node_tye=%s et ref_subst->get_node_type=%s",
		 get_class_name(),
		 ref_subst->get_class_name()) ;
  int first_pass = TRUE ;
  int second_pass = FALSE ;
  //Premier passage pour la substitution courante
  check_op_call_in_substitution(&first_includes_machine,
								&last_includes_machine,
								first_pass) ;

  //deuxieme passage pour la substitution suivante
  ref_subst->check_op_call_in_substitution(&first_includes_machine,
										   &last_includes_machine,
										   second_pass) ;


  //FIN verification des operations

  // C'est tout !
  TRACE0("fin de parallel_checks()") ;
}

// Creation de la liste des variables modifiees par la substitution
void T_substitution::create_list_of_modvar(int first_phase)
{
  TRACE2("T_substitution(%x:%s)::create_list_of_modvar()",
		 this, get_class_name()) ;

  // Par defaut on ne fait rien
  return ;
  // Pour eviter le warning
  first_phase = 0 ;
}

// Fonction auxilaire qui cree une entree dans la liste
// EFFET DE BORD : modifie les champ tmp et tmp2
// Dans tmp2 on met le timestamp
// Dans tmp on met la localisation de la modification
static void tag_and_check_item(T_item *item, int first_phase)
{
  ASSERT(item->is_an_expr() == TRUE) ;

  T_ident *ident = fetch_ident(item->make_expr()) ;

  if (ident != NULL)
	{
	  T_ident *def = (ident->get_def() == NULL) ? ident : ident->get_def() ;

	  if (first_phase == TRUE)
		{
		  TRACE3("premiere passe : on met %d dans %x:%s",
				 first_tag_si,
				 def,
				 def->get_name()->get_value()) ;
		  def->set_tmp2(first_tag_si) ;
		  def->set_tmp(ident) ;
		}
	  else
		{
		  // Deuxieme passe : il faut verifier !
		  if (def->get_tmp2() == first_tag_si)
			{
			  // Erreur !
			  TRACE3("deuxieme passe : erreur, %x:%s a un tmp2 qui vaut %d",
					 def,
					 def->get_name()->get_value(),
					 def->get_tmp2()) ;
			  T_ident *prev = (T_ident *)def->get_tmp() ;
			  ASSERT(prev->is_an_ident() == TRUE) ;
			  semantic_error(prev,
							 CAN_CONTINUE,
							 get_error_msg(E_VARIABLE_IS_ASSIGNED_IN_PARRALLEL),
							 prev->get_name()->get_value()) ;

			  semantic_error_details(ident,
									 get_error_msg(E_LOCATION_OF_PREVIOUS_ASSIGN),
									 ident->get_name()->get_value()) ;
			}

		  TRACE3("deuxieme passe : on met %d dans %x:%s",
				 second_tag_si,
				 def,
				 def->get_name()->get_value()) ;
		  def->set_tmp2(second_tag_si) ;
		}

	}
  else
	{
	  // Cas non prevu : donc pas une ecriture
	  /*
	  internal_error(item->get_ref_lexem(),
					 __FILE__,
					 __LINE__,
					 "tag_and_fetch_item of class %s is not yet implemented",
					 item->get_class_name()) ;
	  */
	}
}

// Fonction auxiliaire qui ajoute une liste d'entrees dans la liste
static void add_list(T_item *first_item, int first_phase)
{
  TRACE2(">> add_list(%x, %s)",
		 first_item,
		 (first_phase == TRUE) ? "TRUE" : "FALSE") ;

  T_item *cur_item = first_item ;

  while (cur_item != NULL)
	{
	  tag_and_check_item(cur_item, first_phase) ;
	  cur_item = cur_item->get_next() ;
	}
}

//
// }{ Creation de la liste des variables modifiees par les substitutions
//

// Classe T_affect
void T_affect::create_list_of_modvar(int first_phase)
{
  TRACE1(">> T_affect(%x)::create_list_of_modvar()", this) ;
  ENTER_TRACE ;

  add_list(first_name, first_phase) ;

  EXIT_TRACE ;
  TRACE1("<< T_affect(%x)::create_list_of_modvar()", this) ;
}

// Classe T_any
void T_any::create_list_of_modvar(int first_phase)
{
  TRACE1("T_any(%x)::create_list_of_modvar()", this) ;
  add_list(first_ident, first_phase) ;

  T_substitution *cur = first_body ;
  while (cur != NULL)
	{
	  cur->create_list_of_modvar(first_phase) ;
	  cur = (T_substitution *)cur->get_next() ;
	}
}

// Classe T_assert
void T_assert::create_list_of_modvar(int first_phase)
{
  TRACE1("T_assert(%x)::create_list_of_modvar()", this) ;

  T_substitution *cur = first_body ;
  while (cur != NULL)
	{
	  cur->create_list_of_modvar(first_phase) ;
	  cur = (T_substitution *)cur->get_next() ;
	}
}

// Classe T_label
void T_label::create_list_of_modvar(int first_phase)
{
  TRACE1("T_label(%x)::create_list_of_modvar()", this) ;

  T_substitution *cur = first_body ;
  while (cur != NULL)
        {
          cur->create_list_of_modvar(first_phase) ;
          cur = (T_substitution *)cur->get_next() ;
        }
}

// Classe T_jumpif
void T_jumpif::create_list_of_modvar(int first_phase)
{
  TRACE1("T_jumpif(%x)::create_list_of_modvar()", this) ;

}

// Classe T_witness
void T_witness::create_list_of_modvar(int first_phase)
{
  TRACE1("T_witness(%x)::create_list_of_modvar()", this) ;

  T_substitution *cur = first_body ;
  while (cur != NULL)
	{
	  cur->create_list_of_modvar(first_phase) ;
	  cur = (T_substitution *)cur->get_next() ;
	}
}

// Classe T_becomes_member_of
void T_becomes_member_of::create_list_of_modvar(int first_phase)
{
  TRACE1("T_becomes_member_of(%x)::create_list_of_modvar()", this) ;
  add_list(first_ident, first_phase) ;
}

// Classe T_becomes_such_that
void T_becomes_such_that::create_list_of_modvar(int first_phase)
{
  TRACE1("T_becomes_such_that(%x)::create_list_of_modvar()", this) ;
  add_list(first_ident, first_phase) ;
}

// Classe T_begin
void T_begin::create_list_of_modvar(int first_phase)
{
  TRACE1("T_begin(%x)::create_list_of_modvar()", this) ;

  T_substitution *cur = first_body ;
  while (cur != NULL)
	{
	  cur->create_list_of_modvar(first_phase) ;
	  cur = (T_substitution *)cur->get_next() ;
	}
}

// Classe T_case
void T_case::create_list_of_modvar(int first_phase)
{
  TRACE1("T_case(%x)::create_list_of_modvar()", this) ;

  T_case_item *cur = first_case_item ;
  while (cur != NULL)
	{
	  cur->create_list_of_modvar(first_phase) ;
	  cur = (T_case_item *)cur->get_next() ;
	}
}

// Classe T_choice
void T_choice::create_list_of_modvar(int first_phase)
{
  TRACE1("T_choice(%x)::create_list_of_modvar()", this) ;

  T_substitution *cur = first_body ;
  while (cur != NULL)
	{
	  cur->create_list_of_modvar(first_phase) ;
	  cur = (T_substitution *)cur->get_next() ;
	}

  T_or *cur_or = first_or ;
  while (cur_or != NULL)
	{
	  cur_or->create_list_of_modvar(first_phase) ;
	  cur_or = (T_or *)cur_or->get_next() ;
	}
}

// Classe T_if
void T_if::create_list_of_modvar(int first_phase)
{
  TRACE1("T_if(%x)::create_list_of_modvar()", this) ;

  T_substitution *cur = first_then_body ;
  while (cur != NULL)
	{
	  cur->create_list_of_modvar(first_phase) ;
	  cur = (T_substitution *)cur->get_next() ;
	}

  T_else *cur_else = first_else ;
  while (cur_else != NULL)
	{
	  cur_else->create_list_of_modvar(first_phase) ;
	  cur_else = (T_else *)cur_else->get_next() ;
	}
}

// Classe T_let
void T_let::create_list_of_modvar(int first_phase)
{
  TRACE1("T_let(%x)::create_list_of_modvar()", this) ;
  add_list(first_ident, first_phase) ;

  // On ne fait pas la verification dans les valuations
  // car ce sont des predicats
  T_substitution *cur_subst = first_body ;
  while (cur_subst != NULL)
	{
	  cur_subst->create_list_of_modvar(first_phase);
	  cur_subst = (T_substitution *)cur_subst->get_next() ;
	}
}

// Classe T_op_call
void T_op_call::create_list_of_modvar(int first_phase)
{
  TRACE1("T_op_call(%x)::create_list_of_modvar()", this) ;
  add_list(first_out_param, first_phase) ;
}

// Classe T_precond
void T_precond::create_list_of_modvar(int first_phase)
{
  TRACE1("T_precond(%x)::create_list_of_modvar()", this) ;

  T_substitution *cur_subst = first_body ;
  while (cur_subst != NULL)
	{
	  cur_subst->create_list_of_modvar(first_phase);
	  cur_subst = (T_substitution *)cur_subst->get_next() ;
	}
}

// Classe T_select
void T_select::create_list_of_modvar(int first_phase)
{
  TRACE1("T_select(%x)::create_list_of_modvar()", this) ;

  T_substitution *cur = first_then_body ;
  while (cur != NULL)
	{
	  cur->create_list_of_modvar(first_phase) ;
	  cur = (T_substitution *)cur->get_next() ;
	}

  T_when *cur_when = first_when ;
  while (cur_when != NULL)
	{
	  cur_when->create_list_of_modvar(first_phase) ;
	  cur_when = (T_when *)cur_when->get_next() ;
	}
}

// Classe T_var
void T_var::create_list_of_modvar(int first_phase)
{
  TRACE1("T_var(%x)::create_list_of_modvar()", this) ;

  add_list(first_ident, first_phase) ;

  T_substitution *cur_subst = first_body ;
  while (cur_subst != NULL)
	{
	  cur_subst->create_list_of_modvar(first_phase);
	  cur_subst = (T_substitution *)cur_subst->get_next() ;
	}
}

// Classe T_while
void T_while::create_list_of_modvar(int first_phase)
{
  TRACE1("T_while(%x)::create_list_of_modvar()", this) ;

  T_substitution *cur_subst = first_body ;
  while (cur_subst != NULL)
	{
	  cur_subst->create_list_of_modvar(first_phase);
	  cur_subst = (T_substitution *)cur_subst->get_next() ;
	}
}

// Classe T_when
void T_when::create_list_of_modvar(int first_phase)
{
  TRACE1("T_when(%x)::create_list_of_modvar()", this) ;

  T_substitution *cur_subst = first_body ;
  while (cur_subst != NULL)
	{
	  cur_subst->create_list_of_modvar(first_phase);
	  cur_subst = (T_substitution *)cur_subst->get_next() ;
	}
}

// Classe T_else
void T_else::create_list_of_modvar(int first_phase)
{
  TRACE1("T_else(%x)::create_list_of_modvar()", this) ;

  T_substitution *cur_subst = first_body ;
  while (cur_subst != NULL)
	{
	  cur_subst->create_list_of_modvar(first_phase);
	  cur_subst = (T_substitution *)cur_subst->get_next() ;
	}
}

// Classe T_or
void T_or::create_list_of_modvar(int first_phase)
{
  TRACE1("T_or(%x)::create_list_of_modvar()", this) ;

  T_substitution *cur_subst = first_body ;
  while (cur_subst != NULL)
	{
	  cur_subst->create_list_of_modvar(first_phase);
	  cur_subst = (T_substitution *)cur_subst->get_next() ;
	}
}

// Classe T_valuation
void T_valuation::create_list_of_modvar(int first_phase)
{
  TRACE1("T_valuation(%x)::create_list_of_modvar()", this) ;

  add_list(ident, first_phase) ;
}

// Classe T_case_item
void T_case_item::create_list_of_modvar(int first_phase)
{
  TRACE1("T_case_item(%x)::create_list_of_modvar()", this) ;

  T_substitution *cur_subst = first_body ;
  while (cur_subst != NULL)
	{
	  cur_subst->create_list_of_modvar(first_phase);
	  cur_subst = (T_substitution *)cur_subst->get_next() ;
	}
}


//GP 20/01/99
//Cette fonction permet d'appele la fonction
//check_op_call_in_substitution() sur la liste des substitutions
//commancant par cur_subst
static void check_substitution_list(T_substitution* cur_subst,
											 T_list_link** ptr_first_list,
											 T_list_link** ptr_last_list,
											 int pass)
{
  //Principe general
  // Si c'est une substituti
  while(cur_subst != NULL)
	{
	  cur_subst->check_op_call_in_substitution(ptr_first_list,
											   ptr_last_list,
											   pass) ;

	  cur_subst=(T_substitution*)cur_subst->get_next() ;
	}
}

//GP 20/01/99
//Fonction qui permet de verifier que deux operations d'une
//meme instance de machine incluse ne sont pas appelees en parallele
//Dans le cas g�n�ral appel recursivement la fonction dans
//les subtitutions de la substitution.
void T_substitution::check_op_call_in_substitution
(T_list_link** ptr_first_include,
 T_list_link** ptr_last_include,
 int pass)
{
  TRACE0("T_substitution::check_op_call_in_substitutio") ;
  return ;
}

//GP 20/01/99
//Fonction qui permet de verifier que deux operations d'une
//meme instance de machine incluse ne sont pas appelees en parallele
//Dans le cas g�n�ral appel recursivement la fonction dans
//les subtitutions de la substitution.
void T_begin::check_op_call_in_substitution
(T_list_link** ptr_first_include,
 T_list_link** ptr_last_include,
 int pass)
{
  TRACE0("T_begin::check_op_call_in_substitution") ;

  check_substitution_list(first_body,
						  ptr_first_include,
						  ptr_last_include,
						  pass) ;
}

//GP 20/01/99
//Fonction qui permet de verifier que deux operations d'une
//meme instance de machine incluse ne sont pas appelees en parallele
//Dans le cas g�n�ral appel recursivement la fonction dans
//les subtitutions de la substitution.
void T_precond::check_op_call_in_substitution
(T_list_link** ptr_first_include,
 T_list_link** ptr_last_include,
 int pass)
{
  TRACE0("T_precond::check_op_call_in_substitution") ;
  check_substitution_list(first_body,
						  ptr_first_include,
						  ptr_last_include,
						  pass) ;
}

//GP 20/01/99
//Fonction qui permet de verifier que deux operations d'une
//meme instance de machine incluse ne sont pas appelees en parallele
//Dans le cas g�n�ral appel recursivement la fonction dans
//les subtitutions de la substitution.
void T_assert::check_op_call_in_substitution
(T_list_link** ptr_first_include,
 T_list_link** ptr_last_include,
 int pass)
{
   TRACE0("T_assert::check_op_call_in_substitution") ;
  check_substitution_list(first_body,
						  ptr_first_include,
						  ptr_last_include,
						  pass) ;
}

void T_label::check_op_call_in_substitution
(T_list_link** ptr_first_include,
 T_list_link** ptr_last_include,
 int pass)
{
   TRACE0("T_label::check_op_call_in_substitution") ;
  check_substitution_list(first_body,
                                                  ptr_first_include,
                                                  ptr_last_include,
                                                  pass) ;
}
void T_jumpif::check_op_call_in_substitution
(T_list_link** ptr_first_include,
 T_list_link** ptr_last_include,
 int pass)
{
}

//GP 20/01/99
//Fonction qui permet de verifier que deux operations d'une
//meme instance de machine incluse ne sont pas appelees en parallele
//Dans le cas g�n�ral appel recursivement la fonction dans
//les subtitutions de la substitution.
void T_witness::check_op_call_in_substitution
(T_list_link** ptr_first_include,
 T_list_link** ptr_last_include,
 int pass)
{
   TRACE0("T_witness::check_op_call_in_substitution") ;
  check_substitution_list(first_body,
						  ptr_first_include,
						  ptr_last_include,
						  pass) ;
}

//GP 20/01/99
//Fonction qui permet de verifier que deux operations d'une
//meme instance de machine incluse ne sont pas appelees en parallele
//Dans le cas g�n�ral appel recursivement la fonction dans
//les subtitutions de la substitution.
void T_choice::check_op_call_in_substitution
(T_list_link** ptr_first_include,
 T_list_link** ptr_last_include,
 int pass)
{
  TRACE0("T_choice::check_op_call_in_substitution") ;
  check_substitution_list(first_body,
						  ptr_first_include,
						  ptr_last_include,
						  pass) ;

  T_or* cur_or = get_or() ;
  while(cur_or != NULL)
	{
	  check_substitution_list(cur_or->get_body(),
							  ptr_first_include,
							  ptr_last_include,
							  pass) ;

	  cur_or = (T_or*)cur_or->get_next() ;
	}
}

//GP 20/01/99
//Fonction qui permet de verifier que deux operations d'une
//meme instance de machine incluse ne sont pas appelees en parallele
//Dans le cas g�n�ral appel recursivement la fonction dans
//les subtitutions de la substitution.
void T_if::check_op_call_in_substitution
(T_list_link** ptr_first_include,
 T_list_link** ptr_last_include,
 int pass)
{
  TRACE0("T_if::check_op_call_in_substitution") ;
  check_substitution_list(first_then_body,
						  ptr_first_include,
						  ptr_last_include,
						  pass) ;

  if(get_else() != NULL)
	{
	  check_substitution_list(get_else()->get_body(),
							  ptr_first_include,
							  ptr_last_include,
							  pass) ;
	}
}

//GP 20/01/99
//Fonction qui permet de verifier que deux operations d'une
//meme instance de machine incluse ne sont pas appelees en parallele
//Dans le cas g�n�ral appel recursivement la fonction dans
//les subtitutions de la substitution.
void T_select::check_op_call_in_substitution
(T_list_link** ptr_first_include,
 T_list_link** ptr_last_include,
 int pass)
{
  TRACE0("T_select::check_op_call_in_substitution") ;
  check_substitution_list(first_then_body,
						  ptr_first_include,
						  ptr_last_include,
						  pass) ;

  T_when* cur_when = get_when() ;
  while(cur_when != NULL)
	{
	  check_substitution_list(cur_when->get_body(),
							  ptr_first_include,
							  ptr_last_include,
							  pass) ;

	  cur_when = (T_when*)cur_when->get_next() ;
	}
}

//GP 20/01/99
//Fonction qui permet de verifier que deux operations d'une
//meme instance de machine incluse ne sont pas appelees en parallele
//Dans le cas g�n�ral appel recursivement la fonction dans
//les subtitutions de la substitution.
void T_any::check_op_call_in_substitution
(T_list_link** ptr_first_include,
 T_list_link** ptr_last_include,
 int pass)
{
  TRACE0("T_any::check_op_call_in_substitution") ;
  check_substitution_list(first_body,
						  ptr_first_include,
						  ptr_last_include,
						  pass) ;
}

//GP 20/01/99
//Fonction qui permet de verifier que deux operations d'une
//meme instance de machine incluse ne sont pas appelees en parallele
//Dans le cas g�n�ral appel recursivement la fonction dans
//les subtitutions de la substitution.
void T_let::check_op_call_in_substitution
(T_list_link** ptr_first_include,
 T_list_link** ptr_last_include,
 int pass)
{
  TRACE0("T_let::check_op_call_in_substitution") ;
  check_substitution_list(first_body,
						  ptr_first_include,
						  ptr_last_include,
						  pass) ;
}

//GP 20/01/99
//Fonction qui permet de verifier que deux operations d'une
//meme instance de machine incluse ne sont pas appelees en parallele
//Dans le cas g�n�ral appel recursivement la fonction dans
//les subtitutions de la substitution.
void T_var::check_op_call_in_substitution
(T_list_link** ptr_first_include,
 T_list_link** ptr_last_include,
 int pass)
{
  TRACE0("T_var::check_op_call_in_substitution") ;
  check_substitution_list(first_body,
						  ptr_first_include,
						  ptr_last_include,
						  pass) ;
}

//GP 20/01/99
//Fonction qui permet de verifier que deux operations d'une
//meme instance de machine incluse ne sont pas appelees en parallele
//Dans le cas g�n�ral appel recursivement la fonction dans
//les subtitutions de la substitution.
void T_case::check_op_call_in_substitution
(T_list_link** ptr_first_include,
 T_list_link** ptr_last_include,
 int pass)
{
  TRACE0("T_case::check_op_call_in_substitution") ;

  T_case_item* cur_case = get_case_items() ;
  while(cur_case != NULL)
	{
	  check_substitution_list(cur_case->get_body(),
							  ptr_first_include,
							  ptr_last_include,
							  pass) ;

	  cur_case=(T_case_item*)cur_case->get_next() ;
	}
}

//GP 20/01/99
//Verifi si used_machine est un element de la T_list_link
//(1) si oui deux operations appelees en parallele proviennent de
//la meme machine INCLUDES et on leve un message d'erreur
//(2) Sinon on ajoute T_used_machine � la T_list_link
static void check_includes_machine(T_used_machine* included_machine,
											T_list_link** ptr_first_of_list,
											T_list_link** ptr_last_of_list,
											T_op_call* op_call,
											int first_pass)
{
  TRACE0("check_includes_machine") ;
  T_list_link* cur = *ptr_first_of_list ;

  if(first_pass == FALSE)
	{
	  //C'est la seconde passe
	  //On ne doit retrouver aucune instance de machine deja incluse
	  TRACE0("check_includes_machine SECONDE PASS") ;
	  while(cur != NULL)
		{
		  //La comparaison se fait sur la valeur des pointeurs
		  if(cur->get_object() == (T_object*)included_machine)
			{
			  ASSERT(included_machine != NULL) ;
			  semantic_error(op_call,
							 CAN_CONTINUE,
							 get_error_msg(E_ILLEGAL_CALL_OF_INCLUDED_OPERATIONS_IN_PARALLEL_SUBST),
							 included_machine->get_name()->get_name()->get_value()) ;

			}
		  cur = (T_list_link*)cur->get_next() ;
		}
	}
  else
	{
	  //C'est la premiere passe
	  //On cree la liste des instances de machine incluses
	  TRACE0("check_includes_machine PREMIERE PASS") ;

	  (void)new T_list_link(included_machine,
							LINK_OTHER,
							(T_item**)ptr_first_of_list,
							(T_item**)ptr_last_of_list,
							NULL,
							NULL,
							NULL) ;
	}
}

//GP 20/01/99
//Fonction qui permet de verifier que deux operations d'une
//meme instance de machine incluse ne sont pas appelees en parallele
//Dans le cas g�n�ral appel recursivement la fonction dans
//les subtitutions de la substitution.
void T_op_call::check_op_call_in_substitution
(T_list_link** ptr_first_include,
 T_list_link** ptr_last_include,
 int pass)
{
  TRACE1("T_op_call::check_op_call_in_substitution:%x",this) ;
  //(1) On cherche la machine qui definini le ref_op
  if(ref_op == NULL)
	{
	  //reprise sur erreur
	  TRACE0("ref_op est NULL") ;
	  return ;
	}

  T_item* cur_item = ref_op->get_father() ;
  if(cur_item == NULL)
	{
	  //reprise sur erreur
	  TRACE0("cur_item est NULL") ;
	  return ;
	}

  while (cur_item->get_node_type() != NODE_MACHINE)
	{
	  cur_item=cur_item->get_father() ;
	  if(cur_item == NULL)
		{
		  //reprise sur erreur
		  TRACE0("cur_item2 est NULL") ;
		  return ;
		}
	}

  //Cast valide par la while
  //La valeur de cur_object
  T_machine* def_machine = (T_machine*)cur_item ;

  //(2) On regarde si c'est un machine requise
  T_used_machine* def_context = def_machine->get_context() ;
  if(def_context == NULL)
	{
	  //l'operation ne provient pas d'une machine requise
	  return ;
	}
  else
	{
	  //L'operation provient d'une machine requise

	  //(3) Il faut verifier si elle provient d'une
	  //machine includes
	  //RQ: les substitution || sont interdits en implementations
	  //donc EXTENDS = INCLUDES
	  if((def_context->get_use_type()==USE_EXTENDS) ||
		 (def_context->get_use_type()==USE_INCLUDES))
		{
		  //L'operation provient d'une machine incluse
		  check_includes_machine(def_context,
								 ptr_first_include,
								 ptr_last_include,
								 this,
								 pass) ;
		}
	  //Dans tout les autre cas pas de risque
	}
}
