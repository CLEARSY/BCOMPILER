/******************************* CLEARSY **************************************
* Fichier : $Id: m_compo.cpp,v 2.0 2007-01-08 11:06:15 atelierb Exp $
* (C) 2008 CLEARSY
*
* Description :		Moniteur de session
*					Classe representant un composant
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
RCS_ID("$Id: m_compo.cpp,v 1.26 2007-01-08 11:06:15 atelierb Exp $") ;


/* Includes systeme */

/* Includes bibliotheques */
#include "c_api.h"

/* Includes Composant Local */
#include "m_compo.h"
#include "m_file.h"

#ifdef ACTION_NG

//
// Fonctions du GNF : on n'ecrit pas :
//#include <n_api.h>
// car on ne veut pas de dependances circulaires Betree -> GNF -> Betree
// Donc on met un forward des fonctions utilisees :
// ATTENTION A LA COHERENCE AVEC n_api.h du GNF
//
extern void init_norm(void) ;
extern void generate_normal_form(T_betree *semantic_betree,
										  const char *output_file) ;

///initialisation des
// fonctions virtuelles permettant de parcourir le corps d'une operation
// et de creer la liste des operations appelees (cas de l'Atelier B
// differentiel.
static void init_user_defined(void)
{
	TRACE0(">> init_user_defined") ;
	reset_user_defined_switches();
	set_user_defined_f5(NODE_SKIP,
						(user_defined_5)add_called_op_skip);
	set_user_defined_f5(NODE_BEGIN,
						(user_defined_5)add_called_op_begin);
	set_user_defined_f5(NODE_CHOICE,
						(user_defined_5)add_called_op_choice);
	set_user_defined_f5(NODE_PRECOND,
						(user_defined_5)add_called_op_precond);
	set_user_defined_f5(NODE_ASSERT,
						(user_defined_5)add_called_op_assert);
	set_user_defined_f5(NODE_IF,
						(user_defined_5)add_called_op_if);
	set_user_defined_f5(NODE_SELECT,
						(user_defined_5)add_called_op_select);
	set_user_defined_f5(NODE_CASE,
						(user_defined_5)add_called_op_case);
	set_user_defined_f5(NODE_LET,
						(user_defined_5)add_called_op_let);
	set_user_defined_f5(NODE_VAR,
						(user_defined_5)add_called_op_var);
	set_user_defined_f5(NODE_WHILE,
						(user_defined_5)add_called_op_while);
	set_user_defined_f5(NODE_ANY,
						(user_defined_5)add_called_op_any);
	set_user_defined_f5(NODE_AFFECT,
						(user_defined_5)add_called_op_skip);
	set_user_defined_f5(NODE_BECOMES_MEMBER_OF,
						  (user_defined_5)add_called_op_skip);
	set_user_defined_f5(NODE_BECOMES_SUCH_THAT,
						(user_defined_5)add_called_op_skip);
	set_user_defined_f5(NODE_OP_CALL,
						(user_defined_5)add_called_op_opcall);
	set_user_defined_f5(NODE_SUBSTITUTION,
						(user_defined_5)add_called_op_substitution);
	TRACE0("<< init_user_defined") ;
}
#endif /* ACTION_NG */


// Constructeur
T_component::T_component(T_symbol *name_p,
								  T_machine_type machine_type_p,
								  T_component_type type_p,
								  T_file_component *file_p,
								  T_symbol *checksum_p,
								  T_item **adr_first,
								  T_item **adr_last,
			    				  T_item *father)
  : T_item(NODE_COMPONENT, adr_first, adr_last, father, NULL, NULL)
{
  TRACE7("T_component(%x):T_component(%x:%s,%d,%d,%x,%x)",
		 this, name_p, name_p->get_value(), machine_type_p, type_p,
		 file_p,
		 checksum_p);

  name = name_p;
  machine_type = machine_type_p;
  type = type_p;
  file = file_p;
  if (file != NULL)
	{
	  file->link();
	}
  checksum = checksum_p;
  index = -1;
  machine = NULL;
  compo_checksums = NULL;
}


// Destructeur
T_component::~T_component(void)
{
  TRACE1("T_component(%x):~T_component()", this);
  if (file != NULL)
	{
	  file->unlink();
	}
  if (machine != NULL)
	{
	  machine->unlink();
	}
  if (compo_checksums != NULL)
	{
	  compo_checksums->unlink();
	}
}

// Modification du checksum
T_symbol * T_component::get_checksum(void)
{
  // le checksum existe, on le donne
  if (checksum != NULL)
    return checksum;

  // sinon, il faut aller le lire dans le stc
  // le chk se trouve dans le path du projet:
  T_object *father = get_father();
  while (father != NULL && father->get_node_type() != NODE_PROJECT) {
    father = get_father();
  }
  const char * path = ((T_project *)father)->get_bdp_dir()->get_value();

  const char * name = this->get_name()->get_value();
  const char * checksum_suffix = "chk";
  char * checksum_file = (char *) s_malloc(strlen(path) + strlen(name) + strlen(checksum_suffix) + 3);
  sprintf (checksum_file, "%s/%s.%s", path, name, checksum_suffix);

  TRACE1("get_checksum, checksum_file: %s", checksum_file);

  FILE * f_checksum_file;
  char * buf;
  ssize_t read;

  // lecture du checksum dans le stc
  f_checksum_file = fopen(checksum_file, "r");
  if (f_checksum_file != NULL) {
    buf = (char *)s_malloc (strlen("00000000000000000000000000000000") + 1);
    buf[strlen("00000000000000000000000000000000")] = '\0';
    read = fread(buf, sizeof(char), strlen("00000000000000000000000000000000"), f_checksum_file);

    if (read < strlen("00000000000000000000000000000000"))	// checksum non trouv�
      checksum = NULL;
    else
      checksum = lookup_symbol(buf);
    fclose (f_checksum_file);
  } else {
    checksum = NULL;
  }
  s_free(checksum_file);
  return checksum;
}

T_project* T_component::getProject()
{
	  T_object *father = get_father();
	  while (father != NULL && father->get_node_type() != NODE_PROJECT) {

	    father = get_father();
	  }
	  return (T_project *)father;
}

void T_component::set_checksum(T_symbol *new_checksum)
{
  TRACE2("T_component(%x)::set_checksum(%x)", this, new_checksum);

  // le stc se trouve dans le path du projet courant:
  T_object *father = get_father();
  while (father != NULL && father->get_node_type() != NODE_PROJECT) {

    father = get_father();
  }
  const char * path = ((T_project *)father)->get_bdp_dir()->get_value();

  const char * name = this->get_name()->get_value();
  const char * checksum_suffix = "chk";
  char * checksum_file = (char *) s_malloc(strlen(path) + strlen(name) + strlen(checksum_suffix) + 3);
  sprintf (checksum_file, "%s/%s.%s", path, name, checksum_suffix);
  TRACE1("set_checksum, checksum_file: %s", checksum_file);

  // Ecriture du fichier
  FILE *f_checksum_file;

  f_checksum_file = fopen(checksum_file, "w+");
  if (f_checksum_file != NULL) {
    if (new_checksum == NULL)
      fprintf ( f_checksum_file, "%s", "");
    else
      fprintf ( f_checksum_file, "%s", new_checksum->get_value());
    fclose (f_checksum_file);
  }

  s_free(checksum_file);
  checksum = new_checksum;
}


// Modification de la machine associee
void T_component::set_machine(T_machine *new_mach)
{
  TRACE3("T_component(%x)::set_machine(%x) was %x", this, new_mach, machine);

  if (machine != NULL)
	{
	  machine->unlink();
	}
  machine = new_mach;
  if (new_mach != NULL)
	{
	  machine->link();
	}
}

// Modification du fichier associee
void T_component::set_file(T_file_component *new_file)
{
  TRACE2("T_component(%x)::set_file(%x)", this, new_file);
  if (file != NULL)
	{
	  file->unlink();
	}
  file = new_file;
  if (file != NULL)
	{
	  file->link();
	}
}

// Gestion des dependances
int T_component::check_dependencies(const char* converterName)
{
  TRACE1("T_component(%x)::check_dependencies()", this);
  ASSERT(file != NULL);

  // Calcul des dependances sur le fichier associe
  return file->check_dependencies(converterName);
}

// Expansion des definitions
int T_component::expand(const char* converterName)
{
  TRACE1("T_component(%x)::expand()", this);

#ifndef ACTION_NG
  // Cas ou deja calculee
  if (checksum != NULL)
	{
	  return FALSE;
	}
#endif /* ACTION_NG */

  // Expansion des composants du fichier
  ASSERT(file != NULL);
  return file->expand(converterName);
}


// Decoupage des fichiers multi-composants
int T_component::cut(const char* converterName)
{
  TRACE1("T_component(%x)::cut()", this);

  // Decoupage des composants du fichier
  ASSERT(file != NULL);
  return file->cut(converterName);
}

// Creation de la liste des operations modifiees
static void check_modified_op_list(T_component_checksums *new_state,
											T_component_checksums *old_state)
{
  TRACE2("check_modified_op_list(%x, %x)", new_state, old_state);

  // Parcours de la liste des nouvelles operations
  T_operation_checksums *check_op1 = NULL;
  // Parcours de la liste des anciennes operations
  T_operation_checksums *check_op2 = NULL;
  // Nom de l'operation courante
  T_symbol *cur_name = NULL;
  // Bouleen de recherche de l'ancienne definition d'une operation
  int find = FALSE;

  check_op1 = new_state->get_operations_checksums();
  while (check_op1 != NULL)
	{
	  cur_name = check_op1->get_operation_name();
	  TRACE1("Comparaison de %s", cur_name->get_value());

	  // On recherche la definition de cette operation dans l'ancienne liste
	  check_op2 = old_state->get_operations_checksums();
	  find = FALSE;
	  while ((check_op2 != NULL) && (find == FALSE))
		{
		  if (cur_name->compare(check_op2->get_operation_name()))
			{
			  find = TRUE;
			}
		  else
			{
			  check_op2 = (T_operation_checksums *)check_op2->get_next();
			}
		}
	  if ((find == FALSE) || (check_op1->compare(check_op2) == FALSE))
		{
		  // Il y a une nouvelle operation
		  // On la traite comme une operation modifi�e;
		  TRACE0("Nouvelle operation ou l'operation a ete modifiee");
		  check_op1->set_modified(TRUE);
		}
	  else
		{
		  TRACE1("L'operation n'a pas ete modifiee",
				 check_op1->get_operation_name()->get_value());
		  check_op1->set_modified(FALSE);
		}
	  check_op1 = (T_operation_checksums *)check_op1->get_next();
	}
  TRACE0("check_modified_op_list-> OK");
}

// Creation de l'�tat du composant
int T_component::set_component_checksums(const char* converterName)
{
  TRACE1("T_component(%x)::set_component_checksums()", this);

  T_machine *sem_mach = NULL;

  // On calcule le betree semantique du composant
  ASSERT(file != NULL);
  sem_mach = file->get_semantic_machine(this, converterName);
  if (sem_mach == NULL)
	{
	  TRACE0("set_component_checksums -> KO");
	  return 1;
	}
  compo_checksums = new T_component_checksums(sem_mach, CTYPE_MAIN,
											  NULL, NULL, this);

  // On libere les betree crees par get_semantic_machine
  get_project_manager()->free_betrees();
  TRACE0("set_component_checksums -> 0K");
  return 0;
}

//fait l'analyse syntaxique, semantique, et b0check du composant
//retourne FALSE si le component check echoue
// TRUE sinon
int T_component::do_component_check(const char* converterName)
{
	TRACE0(">> do_component_check ") ;

	T_machine *sem_mach = NULL;

	// On calcule le betree b0 du composant
	ASSERT(file != NULL);
        sem_mach = file->get_b0checked_machine(this, converterName);
	if (sem_mach == NULL)
		{
			TRACE0("do_component_check -> KO");
			return FALSE;
		}

	// On libere les betree crees par get_semantic_machine
	get_project_manager()->free_betrees();
	TRACE0("do_component_check -> 0K");
	return TRUE;

}

#ifdef ACTION_NG
//creation du fichier de forme normale du composant
//retourne fALSE si on a une erreur
//retourne TRUE si tout s'est bien passe
//si GOP_diff est a TRUE, on calcule l'etat du composant
//pour le GOP differentiel
int T_component::comp_generate_normal_form(const char *output_file,
													int GOP_diff)
{
  TRACE1(">> T_component(%x)::generate_normal_form()", this);

  int res = FALSE ; //retour de la fonction
  T_betree *b0checked_betree = NULL;//betree semantique

  //le compilateur B doit etre en mode unique_record_type_mode pour le GNF
  set_unique_record_type_mode(TRUE) ;

  // On calcule le betree B0 checke du composant
  ASSERT(file != NULL);
  b0checked_betree = file->get_B0Checked_betree(this);
  if (b0checked_betree == NULL)
	{
	  TRACE0("generate_normal_form -> KO");
	  return FALSE ;
	}

  //generation de la forme normale du composant
  if ( get_error_count() == 0)
	  {
		  init_norm() ;
		  TRACE0("GNF generate_normal_form") ;
		  generate_normal_form(b0checked_betree, output_file) ;
		  init_user_defined() ;
		  if(GOP_diff == TRUE)
			  {
				  T_machine *b0checked_mach = b0checked_betree->get_root() ;
				  compo_checksums = new T_component_checksums(b0checked_mach,
															  CTYPE_MAIN,
															  NULL,
															  NULL,
															  this);
			  }
		  res = TRUE ;
	  }
  // On libere les betree crees par get_semantic_betree
  get_project_manager()->free_betrees();

  TRACE2("<< T_component::generate_normal_form(%d),FALSE(%d)", res, FALSE);
  return res ;
}
#endif /* ACTION_NG */

// rend 1 si le contexte du composant a ete modifie 0 sinon et -1 si erreur
int T_component::is_context_modified(char *state_file)
{
 TRACE2(">> T_component(%x)::is_context_modified(%s)", this, state_file);
 ASSERT(compo_checksums != NULL);

 int res=-1 ; //resultat
 T_component_checksums *old_state = NULL;
 T_betree_file_type ftype = FILE_COMPONENT_CHECKSUMS ;
 T_io_diagnostic io_diagnostic;

 // On lit l'ancien etat
 old_state = (T_component_checksums *)load_checksums(state_file, ftype,
												  io_diagnostic);
    if ((old_state == NULL) || (io_diagnostic != IO_OK))
	{
	  // On n'arrive pas a le lire, ou il n'existe pas donc on doit creer
	  // la liste de toutes les operations
	  // A la construction, le liste des operation a ete marquee "modifiee",
	  // donc pas de marquage a faire
	  TRACE0("Erreur dans le chargement du fichier");
	  return -1 ;
	}

  if (compo_checksums->has_same_context(old_state, true) == FALSE)
	{
	  // A la construction, le liste des operation a ete marquee "modifiee",
	  // donc pas de marquage a faire
	  TRACE0("Le contexte du composant a ete modifie");
	  res = 1;
	}
  else
	  {
		  res = 0 ;
	  }

  TRACE2("<< T_component(%x)::is_context_modified(%d)", this, res);
  return res ;
}

// Creation de la liste des operation modifiees
// Retourne le nombre d'erreurs
int T_component::set_modified_op_list(char *state_file)
{
  TRACE2("T_component(%x)::set_modified_op_list(%s)", this, state_file);
  ASSERT(compo_checksums != NULL);

  T_component_checksums *old_state = NULL;
  T_betree_file_type ftype = FILE_COMPONENT_CHECKSUMS ;
  T_io_diagnostic io_diagnostic;

  // On lit l'ancien etat
  old_state = (T_component_checksums *)load_checksums(state_file, ftype,
												   io_diagnostic);

  if ((old_state == NULL) || (io_diagnostic != IO_OK))
	{
	  // On n'arrive pas a le lire, ou il n'existe pas donc on doit creer
	  // la liste de toutes les operations
	  // A la construction, le liste des operation a ete marquee "modifiee",
	  // donc pas de marquage a faire
	  TRACE0("Erreur dans le chargement du fichier");
	  return 1;
	}

  if (compo_checksums->has_same_context(old_state, true) == FALSE)
	{
	  // A la construction, le liste des operation a ete marquee "modifiee",
	  // donc pas de marquage a faire
	  TRACE0("Le contexte du composant a ete modifie");
	  return 0;
	}

  // Les deux etats ont le meme contexte, on peut comparer les
  // operations une a une
  TRACE0("Comparaison des operations");
  check_modified_op_list(compo_checksums, old_state);
  TRACE0("set_modified_op_list -> 0K");
  return 0;
}

// Sauvegarde de l'etat du composant dans le fichier state_file
int T_component::save_state(char *state_file)
{
  TRACE1("T_component(%x)::save_state()", this);
  ASSERT(state_file != NULL);
  ASSERT(compo_checksums != NULL);

  compo_checksums->set_father(NULL);

  T_io_diagnostic io_diagnostic = save_checksums(state_file,
													 FILE_COMPONENT_CHECKSUMS);
  // Erreur de sauvegarde
  if (io_diagnostic != IO_OK)
	{
	  return 1;
	}

  TRACE0("T_component::save_state -> OK");
  return 0;
}

  // Nettoyage de l'�tat du composant
void T_component::unlink_state(void)
{
  TRACE1("T_component(%x)::unlink_state", this);
  if (compo_checksums != NULL)
	{
	  compo_checksums->unlink();
	  compo_checksums = NULL;
	}
}

//
// Fin du fichier
//
