/******************************* CLEARSY **************************************
* Fichier : $Id: m_file.cpp,v 2.0 2007-05-18 13:44:38 jburlando Exp $
* (C) 2008-2025 CLEARSY
*
* Description :		Moniteur de session
*					Classe representant un fichier contenant un composant
*
This file is part of B_COMPILER
    Copyright (C) 2008-2025 ClearSy (contact@clearsy.com)

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
RCS_ID("$Id: m_file.cpp,v 1.48 2007-05-18 13:44:38 jburlando Exp $") ;


/* Includes systeme */
#include <sys/stat.h>
#include <limits.h>

/* Includes bibliotheques */
#include "c_api.h"

/* Includes Composant Local */
#include "m_compo.h"
#include "m_file.h"
#include "m_projec.h"
#include "m_pmana.h"
#include "m_api.h"
#include "m_msg.h"

// Constructeur
T_file_component::T_file_component(const char *filename,
											const char *filesuffix,
											const char *filedir,
											const char *username,
											T_item **adr_first,
											T_item **adr_last,
											T_item *father)
    : T_item(NODE_FILE_COMPONENT, adr_first, adr_last, father, NULL, NULL)
{
  TRACE5("T_file_component(%x):T_file_component(%s,%s,%s,%s)",
		 this, filename, filesuffix, filedir, username);
  path = lookup_symbol(filedir);
  name = lookup_symbol(filename);
  suffix = lookup_symbol(filesuffix);
  user = lookup_symbol(username);
  first_component = NULL;
  last_component = NULL;
  first_file_definition = NULL;
  last_file_definition = NULL;
  generated_from_component = NULL;
  // on initialise la date du fichier
  TRACE0("On initialise la date du fichier");
  raz_date();
}

// Destructeur
T_file_component::~T_file_component(void)
{
  TRACE1("T_file_component(%x):~T_file_component()", this);

  // destruction de la liste des composants
  ASSERT(first_component != NULL);
  list_unlink((T_item *)first_component);
}


// Ajout d'un composant dans la liste des composants
void T_file_component::add_component(T_component *new_comp)
{
  TRACE3("T_file_component(%x):add_component(%x:%s)",
		 this, new_comp, new_comp->get_name()->get_value());

  T_list_link *new_link;

  // chainage en queue
  new_link = new T_list_link(new_comp,
							 LINK_OTHER,
							 NULL,
							 (T_item **)&first_component,
							 (T_item **)&last_component,
							 this,
							 NULL,
							 NULL);

  // on positionne le flag pour qu'il libere son objet
  new_link->set_do_free();

}

// teste si un fichier est multi-composants
int T_file_component::is_a_multi_components_file()
{
  TRACE1("T_file_component(%x):is_a_multi_components_file()", this );
  if (strcmp("mod", suffix->get_value()) == 0)
	{
	  return TRUE;
	}
  else
	{
	  return FALSE;
	}
}

// Recherche d'un composant dans la liste
T_component *T_file_component::search_component(T_symbol *name)
{
  ASSERT(name != NULL);
  TRACE2("T_file_component(%x)::search_component(%x)", this, name);
  T_list_link *elem = first_component;
  int found = 0;
  T_component *comp = NULL;
  ASSERT(elem != NULL);
  while ( (!found) && (elem != NULL) )
	{
	  comp = (T_component *)elem->get_object();
	  ASSERT(comp != NULL);
	  if (name->compare(comp->get_name()) == TRUE)
		{
		  found = 1;
		}
	  else
		{
		  elem = (T_list_link *)elem->get_next();
		}
	}
  if (found)
	{
	  TRACE1("search_component --> %x", comp);
	  return comp;
	}
  TRACE0("search_component --> NULL");
  return NULL;
}

// Recherche des dependances
int T_file_component::check_dependencies(const char* converterName)
{
  TRACE1("T_file_component(%x):check_dependencies()", this );
  T_machine *machine;
  T_component *comp;
  int error_flag = 0;
  T_symbol *namecomp = NULL;
  T_project *proj = NULL;
  int save_flag = 0;
  char *fullname = NULL;       // chemin complet du fichier

  // chemin complet du fichier
  fullname = (char *)s_persistent_malloc(path->get_len() +
										 name->get_len() +
										 suffix->get_len() +
										 3); // "/." + '\0'
  sprintf(fullname,
		  "%s/%s.%s",
		  path->get_value(),
		  name->get_value(),
		  suffix->get_value());

  // si la date maximum du fichier lu et de ses fichiers de definition est la
  // meme que celui deja en m�moire, il est inutile de refaire les betree,
  // on s'arrete
  if (is_up_to_date(fullname) == TRUE)
	{
	  TRACE1("Le fichier %s est up-to-date", fullname);
	  s_free(fullname);
	  return error_flag;
	}

  TRACE0("Le fichier est nouveau, il faut cr�er le betree");

  // Reinitialisation de la liste des fichiers de definitions
  // qui sera reconstruite avec les nouveaux fichiers
  set_file_definition(NULL);

  // on recupere le projet pere (courant ou librairie)
  proj = (T_project *)this->get_father();
  ASSERT(proj != NULL);

  // on recupere le type des composants associes au fichier
  T_component_type type;
  ASSERT(first_component != NULL);
  comp = (T_component *)first_component->get_object();
  type = comp->get_type();

  T_betree *betree = load_betrees(BC_CALL_DEPENDENCE, converterName);

  if (betree == NULL) return TRUE;

  // changement de l'heure pour marquer les composants du fichier
  next_timestamp();

  // parcours de la liste des betree
  while (betree != NULL)
	{
	  machine = betree->get_root();

	  TRACE2("Betree : %s checksum %s",
			 machine->get_machine_name()->get_name()->get_value(),
			 betree->get_expanded_checksum()->get_value());

	  // on verifie que le composant est dans la liste locale
	  comp = search_component(machine->get_machine_name()->get_name());

	  // Cas ou le composant est deja dans la liste locale
	  if (comp != NULL)
		{
		  // on verifie qu'il n'a pas change de type
		  if (machine->get_machine_type() != comp->get_machine_type())
			{
			  // on modifie son type
			  comp->set_machine_type(machine->get_machine_type());

			  // on le signale au manager pour que le Moniteur de session
			  // recupere l'information
			  get_project_manager()->set_modified_flag(1);

			  // il faut resauver le projet
			  save_flag = 1;
			}

		  // on le marque
		  comp->set_stamp();
		  TRACE2("Stamp(%x) :%d", comp, comp->get_stamp());

		  // on met a jour sa machine
		  comp->set_machine(machine);

		  // on regarde si son checksum a change
		  // si il a change on le vide
		  if (comp->get_checksum() != NULL)
			{
			  TRACE2("composant %s checksum non NULL (%s)",
					 comp->get_name()->get_value(),
					 comp->get_checksum()->get_value());
			  if (comp->get_checksum()->
				  compare(betree->get_expanded_checksum()) != TRUE)
				{
				  TRACE1("Checksum different (%s)",
						 betree->get_expanded_checksum()->get_value());
				  // on met le checksum a NULL pour refaire les expansions.
				  //seulement si on n'est pas
				  //dans la nouvelle gestion des actions
#ifndef ACTION_NG
				  TRACE0("Mise a NULL du checksum") ;
				  comp->set_checksum(NULL);
#else /* ACTION_NG */
				  comp->set_checksum(betree->get_expanded_checksum()) ;
#endif /* ACTION_NG */
				}
			  else
				{
				  TRACE0("Checksum identiques");
				}
			}
		  else
			{
				//dans la nouvelle gestion des action, il faut mettre a jour
				//le checksum
#ifndef ACTION_NG
			  TRACE1("composant %s ancien checksum NULL",
					 comp->get_name()->get_value());
			  // on laisse le checksum a NULL pour refaire les expansions
#else /* ACTION_NG */
			  comp->set_checksum(betree->get_expanded_checksum()) ;
#endif /* ACTION_NG */

			}
		}

	  // Cas ou le composant vient d'etre ajoute au fichier multi-composants
	  else
		{
		  // on regarde si le composant est deja defini dans le projet courant
		  namecomp = machine->get_machine_name()->get_name();
		  comp = proj->search_component(namecomp);

		  TRACE1("Nouveau composant ajoute %s", namecomp->get_value());

		  // si le composant est deja defini - sortie en erreur
		  if (comp != NULL)
			{
			  user_error(CAN_CONTINUE,
						 get_user_error_msg(U_EXISTING_COMPONENT),
						 namecomp->get_value(),
						 fullname);
			  error_flag = 1;
			}
		  else
			{
			  // le composant n'est pas defini
			  // creation d'un nouveau composant
			  // le checksum est NULL car l'expansion n'a pas ete faite.
			  T_component *new_comp = new T_component(
													  machine->get_machine_name()->get_name(),
													  machine->get_machine_type(),
													  type,
													  this,
													  NULL,
													  NULL,
													  NULL,
													  this->get_father());

			  // on marque le composant
			  new_comp->set_stamp();

			  // on enregistre les dependances
			  new_comp->set_machine(machine);

			  // on le rajoute au fichier
			  this->add_component(new_comp);

			  // ajout au projet
			  proj->add_component_to_list(new_comp);

			  // ajout a la liste des composants ajoutes
			  get_project_manager()->add_added_component(new_comp);

			  // si c'est le projet courant, il faudra le sauver sur disque
			  if (proj == get_project_manager()->get_current())
				{
				  save_flag = 1;
				}
			}
		}

	  // mis-�-jour de la liste des fichirs de definition
	  insert_file_definition(betree->get_file_definitions());

	  // betree suivant
	  betree = betree->get_next_betree();
	} // fin de la boucle sur les betree

  // Cas ou il n'y a pas d'erreur
  if (!error_flag)
	{
	  // On cherche si il n'y a pas des composants supprimes du fichier
	  // multi-composants
	  T_list_link *elem = first_component;
	  T_list_link *next_elem = NULL;
	  while ( (!error_flag) && (elem != NULL) )
		{
		  // Suivant
		  next_elem = (T_list_link *)elem->get_next();

		  // si le composant n'est pas marque, il n'existe plus
		  comp = (T_component *)elem->get_object();
		  TRACE3("Composant :%x (stamp = %d, timestamp = %d)",
				 comp, comp->get_stamp(), get_timestamp());
		  if ( comp->get_stamp() != get_timestamp() )
			{
			  // on le met dans la liste des composants enleves
			  get_project_manager()->add_removed_component(comp);

			  // on l'enleve de la liste des composants du projet
			  proj->remove_component(comp);
			  comp->set_machine(NULL);

			  // on l'enleve de la liste des composants du fichier
			  elem->remove_from_list((T_item **)&first_component,
									 (T_item **)&last_component);

			  // si c'est le projet courant, il faudra le sauver sur disque
			  if (proj == get_project_manager()->get_current())
				{
				  save_flag = 1;
				}
			}
		  // Suivant
		  elem = next_elem;
		}

	  // Sauvegarde de la nouvelle date
	  TRACE0("Recuperation de la date");
	  construct_new_date(fullname);
	}

  // liberation des betree
  s_free(fullname);
  get_project_manager()->free_betrees();

  // si le projet courant est modifie
  // il faut le sauver sur disque, on en laisse la
  // responsabilite au Moniteur de session
  if ( (!error_flag) && (save_flag) )
	{
	  get_project_manager()->set_save_flag();
	}

  TRACE1("sortie de check_dependencies->%d", error_flag);
  return error_flag;
}

// Expansion des definitions
int T_file_component::expand(const char* converterName)
{
  TRACE1("T_file_component(%x)::expand()", this);
  T_machine *machine = NULL;
  T_component *comp = NULL;
  T_betree *res_decomp = NULL;

  // appel au compilateur - passe syntaxique
  TRACE0("appel au compilateur");

  // recuperation du ou des Betree
  T_betree *betree = load_betrees(BC_CALL_SYNTAX, converterName);
  if (betree == NULL)  return TRUE;

  // changement de l'heure pour marquer les composants du fichier
  next_timestamp();

  TRACE0("Parcours des betree");
  // parcours de la liste des betree
  while (betree != NULL)
	{
	  machine = betree->get_root();

	  // on verifie que le composant est dans la liste locale
	  comp = search_component(machine->get_machine_name()->get_name());
	  if (comp == NULL)
		{
		  char *fullname = new char[path->get_len() +
								   name->get_len() +
								   suffix->get_len() +
								   3]; // "/." + '\0'
		  sprintf(fullname, "%s/%s.%s", path->get_value(),
				  name->get_value(), suffix->get_value());
		  user_error(CAN_CONTINUE,
					 get_user_error_msg(U_MODIFIED_FILE_ADD),
					 fullname,
					 machine->get_machine_name()->get_name()->get_value());
		  delete [] fullname;
		  get_project_manager()-> free_betrees();
		  return TRUE;
		}
	  else
		{
		  comp->set_stamp();
		  betree = betree->get_next_betree();
		}
	}

  TRACE0("Composant supprime ?");
  // On verifie qu'il n'y a pas de composant supprime depuis le graphe
  T_list_link *elem = first_component;
  while (elem != NULL)
	{
	  comp = (T_component *)elem->get_object();
	  ASSERT(comp != NULL);
	  if (comp->get_stamp() != get_timestamp())
		{
		  char *fullname = new char[path->get_len() +
								   name->get_len() +
								   suffix->get_len() +
								   3]; // "/." + '\0'
		  sprintf(fullname, "%s/%s.%s", path->get_value(),
				  name->get_value(), suffix->get_value());
		  user_error(CAN_CONTINUE,
					 get_user_error_msg(U_MODIFIED_FILE_REMOVE),
					 fullname,
					 comp->get_name()->get_value());
		  delete [] fullname;
		  get_project_manager()->free_betrees();
		  return TRUE;
		}
	  elem = (T_list_link *)elem->get_next();
	}

  // on recupere le projet pour avoir les repertoires de decompilation
  T_project *proj = (T_project *)this->get_father();
  ASSERT(proj != NULL);
  ASSERT(proj->get_decomp_dir() != NULL);
  ASSERT(proj->get_old_tools_decomp_dir() != NULL);
  TRACE1("proj->decomp_dir = %s", proj->get_decomp_dir()->get_value());
  TRACE1("proj->old_tools_decomp_dir = %s",
		 proj->get_old_tools_decomp_dir()->get_value());

  // on reparcourt la liste des betree pour generer les fichiers
  betree = get_betree_manager()->get_betrees();
  while (betree != NULL)
	{
	  machine = betree->get_root();

	  // on verifie que le composant est dans la liste locale
	  comp = search_component(machine->get_machine_name()->get_name());

#ifndef ACTION_NG
	  // on ne decompile que les composants pour lesquels c'est necessaire
	  if (comp->get_checksum() == NULL)
		{
#endif /* ACTION_NG */

		  TRACE1("Decompilation necessaire pour comp %x", betree);

		  // decompilation normale
		  set_decompiler_output_path(proj->get_decomp_dir()->get_value());
		  res_decomp = decompile_betree(betree, FALSE, FALSE);

		  // Cas d'erreur de decompilation normale
		  if (res_decomp == NULL)
			{
			  TRACE1("Echec Decompilation normale de %s",
					 comp->get_name()->get_value());
			  get_project_manager()->free_betrees();
			  return TRUE;
			}
		  TRACE1("decompilation normale de %s : reussie",
				 comp->get_name()->get_value());

		  // decompilation avec expansion
		  set_decompiler_output_path(proj->get_old_tools_decomp_dir()->
									 get_value());
		  res_decomp = decompile_betree(betree, TRUE, TRUE);

		  // Cas d'erreur de decompilation avec expansion
		  if (res_decomp == NULL)
			{
			  TRACE1("Echec Decompilation avec expansion  de %s",
					 comp->get_name()->get_value());
			  get_project_manager()->free_betrees();
			  return TRUE;
			}
		  TRACE1("decompilation avec expansion de %s : reussie",
				 comp->get_name()->get_value());

		  // on enregistre le checksum
		  comp->set_checksum(betree->get_expanded_checksum());
		  TRACE2("Modification checksum de %s (%s)",
				 comp->get_name()->get_value(),
				 betree->get_expanded_checksum()->get_value());

		  // Bug_018 (Project Engine)
		  // Jerome Burlando
		  // Sauvegarde du fichier projet inutile car les
		  // checksums ne sont plus enregistr�s dans la nouvelle
		  // version du fichier projet (.db)
		  //
		  // Avant modification :
		  //
		  // // on signale au manager qu'il faut enregistrer le .db
		  // // car les checksum ont change.
		  // get_project_manager()->set_save_flag();

#ifndef ACTION_NG
		} // fin si checksum != NULL
#endif // ACTION_NG

	  // suivant
	  betree = betree->get_next_betree();
	}

  // liberations memoire
  get_project_manager()->free_betrees();

  return FALSE;
}

// Decoupage des fichiers multi-composants
int T_file_component::cut(const char* converterName)
{
  TRACE1("T_file_component(%x)::cut()", this);
  T_betree *res_decomp = NULL;

  // recuperation du ou des Betree
  TRACE0("appel au compilateur, passe syntaxique");
  T_betree *betree = load_betrees(BC_CALL_SYNTAX, converterName);
  if (betree == NULL) return TRUE;

  // on recupere le projet pour avoir les repertoires de decompilation
  T_project *proj = (T_project *)this->get_father();
  ASSERT(proj != NULL);
  ASSERT(proj->get_decomp_dir() != NULL);
  TRACE1("proj->decomp_dir = %s", proj->get_decomp_dir()->get_value());

  // on reparcourt la liste des betree pour generer les fichiers
  while (betree != NULL)
	{
	  TRACE1("Decompilation de comp %x", betree);
	  set_decompiler_output_path(proj->get_decomp_dir()->get_value());
	  res_decomp = decompile_betree(betree, FALSE, FALSE);

	  // Cas d'erreur
	  if (res_decomp == NULL)
		{
		  TRACE0("Echec Decompilation");
		  get_project_manager()->free_betrees();
		  return TRUE;
		}

	  TRACE0("Decompilation reussie");

	  // suivant
	  betree = betree->get_next_betree();
	}

  // liberation memoire
  get_project_manager()->free_betrees();

  return FALSE;
}


// Chargement des betree d'un fichier
T_betree *T_file_component::load_betrees(T_bcomp_call_type bc_type,
                                                  const char *converterName,
                                                                                                  T_component *compo)
{
  TRACE3("T_file_component(%x):load_betrees(%d, %s)",
		 this, bc_type,
		 (compo == NULL ? "NULL" : compo->get_name()->get_value()));

#ifndef ACTION_NG
  T_symbol *compo_name = NULL; // Nom du composant eventuel
#endif /* ACTION_NG */
  char *fullname = NULL;   // Chemin absolu du fichier
  T_betree *betree = NULL; // Resultat

  TRACE0("initialisations");

  // initialisation du manager
  if (bc_type == BC_CALL_DEPENDENCE)
	{
	  get_project_manager()->set_modified_flag(0);
	}

  // initialisation du compteur d'erreur
  reset_error_count();

  switch(bc_type)
	{
	case BC_CALL_DEPENDENCE :
	  {
		// appel au compilateur - passe dependances

		// on change de repertoire momentanement pour aller dans le repertoire
		// du source B ainsi la recherche des sources en mode FFMODE_LOCAL peut
		// s'effectuer dans le repertoire du source B, meme si on n y etait pas
		// precedemment

		char *filename= NULL ; //nom du fichier

		char original_cwd[PATH_MAX];
		(void) getcwd(original_cwd, sizeof original_cwd);
		chdir(path->get_value());

		// "." + '\0'
		filename = new char[name->get_len() + suffix->get_len() +2];
		sprintf(filename, "%s.%s", name->get_value(), suffix->get_value());

		TRACE1("appel au compilateur dependance: filename = %s", filename);
		set_file_fetch_mode(FFMODE_LOCAL);
                compiler_dep_analysis(filename, converterName);
		chdir(original_cwd);
        delete[] filename;
		break;
	  }
	case BC_CALL_SYNTAX :
#ifdef ACTION_NG
	case BC_CALL_SEMANTIC :
	case BC_CALL_B0CHECKER :
#endif /* ACTION_NG */
	  {
		// appel au compilateur - passe syntaxique
		// on change de repertoire momentanement pour aller dans le repertoire
		// du source B ainsi la recherche des sources en mode FFMODE_LOCAL peut
		// s'effectuer dans le repertoire du source B, meme si on n y etait pas
		// precedemment

		char *filename= NULL ; //nom du fichier

		char original_cwd[PATH_MAX];
		(void) getcwd(original_cwd, sizeof original_cwd);
		chdir(path->get_value());

		// "." + '\0'
		filename = new char[name->get_len() + suffix->get_len() +2];
		sprintf(filename, "%s.%s", name->get_value(), suffix->get_value());

		TRACE1("appel au compilateur syntaxique: filename = %s", filename);
		set_file_fetch_mode(FFMODE_LOCAL);
                compiler_syntax_analysis(NULL, filename, converterName);
		chdir(original_cwd);
        delete[] filename;
		break;
	  }
#ifndef ACTION_NG
	case BC_CALL_SEMANTIC :
	case BC_CALL_B0CHECKER :
	  {
		ASSERT(compo != NULL);
		compo_name = compo->get_name();
		// appel au compilateur - passe semantique
		fullname = new char[compo_name->get_len() + 5];
		  //5 = .mch, .ref ou .imp + '\0'

		switch(compo->get_machine_type())
		  {
		  case MTYPE_SPECIFICATION :
			{
			                     /* 12345 */
			  sprintf(fullname, "%s.mch", compo_name->get_value());
			  break;
			}
		  case MTYPE_SYSTEM :
			{
			                     /* 12345 */
			  sprintf(fullname, "%s.sys", compo_name->get_value());
			  break;
			}
		  case MTYPE_REFINEMENT :
			{
				                /* 12345 */
			  sprintf(fullname, "%s.ref", compo_name->get_value());
			  break;
			}
		  case MTYPE_IMPLEMENTATION :
			{
				                /* 12345 */
			  sprintf(fullname, "%s.imp", compo_name->get_value());
			  break;
			}
		  }
		TRACE1("appel au compilateur syntaxique: fullname = %s", fullname);
		set_file_fetch_mode(FFMODE_MS);
                compiler_syntax_analysis(NULL, fullname, converterName);
		break;
	  }
#endif /* ACTION_NG */

	}
  delete [] fullname;

  // R�cup�ration des betree
  betree = get_betree_manager()->get_betrees();

  // Cas de l'analyse semantique d'un composant
  if ( ((bc_type == BC_CALL_SEMANTIC) || (bc_type == BC_CALL_B0CHECKER)) &&
	   (get_error_count() == 0) )
	{
	  // on recupere le betree semantique
	  TRACE1("Appel du compilateur semantique sur %x", betree);
#ifndef ACTION_NG
	  set_file_fetch_mode(FFMODE_MS);
#else /* ACTION_NG */
   	  set_file_fetch_mode(FFMODE_ORIGINAL_MS);
#endif /* ACTION_NG */
	  reset_error_count();
          betree = compiler_semantic_analysis(betree, converterName);
	  //compiler_dump_HTML_betree(betree);
	}

  if ((betree == NULL) || (get_error_count() != 00))
	{
	  TRACE3("erreur analyse %d : Betree %x : errors : %d",
			 bc_type,
			 betree,
			 get_error_count());
	  // Erreur !! analyse des dependances impossible
	  if (betree != NULL)
		{
		  TRACE0("unlink du betree manager");
		  get_project_manager()->free_betrees();
		  TRACE0("apres unlink du betree manager");
		}
	  return NULL;
	}

  if( (get_error_count() == 0 ) && (bc_type == BC_CALL_B0CHECKER) )
	  {
		  // on recupere le betree b0
		  TRACE1("Appel du B0 Checker sur %x", betree);
		  set_file_fetch_mode(FFMODE_ORIGINAL_MS);
		  reset_error_count();
		  betree = compiler_B0_check(betree) ;
	  }

  if ((betree == NULL) || (get_error_count() != 0))
	  {
		  TRACE3("erreur analyse %d : Betree %x : errors : %d",
				 bc_type,
				 betree,
				 get_error_count());
		  // Erreur !! analyse des dependances impossible
		  if (betree != NULL)
			  {
				  TRACE0("unlink du betree manager");
				  get_project_manager()->free_betrees();
				  TRACE0("apres unlink du betree manager");
			  }
		  return NULL;
	  }

  TRACE1("sortie de T_file_component::load_betrees ->%x", betree);
  return betree;
}

// Recuperation de la T_machine semantique d'un composant
// ATTENTION :cette fonction charge les betree, il faudra penser a faire
// du menage quand le traitement sera termine ...
T_machine *T_file_component::get_semantic_machine(T_component *compo, const char* converterName)
{
  TRACE3("T_file_component(%x)::get_semantic_machine(%x:%s)",
		 this, compo, compo->get_name()->get_value());

  T_betree *betree = NULL;

  betree = load_betrees(BC_CALL_SEMANTIC, converterName, compo);

  if (betree != NULL)
	{
	  TRACE1("T_file_component::get_semantic_machine-->%x",
			 betree->get_root());
	  return betree->get_root();
	}
  TRACE0("T_file_component::get_semantic_machine--> NULL");
  return NULL;
}

#ifdef ACTION_NG
// Recuperation du T_betree semantique d'un composant
// ATTENTION :cette fonction charge les betree, il faudra penser a faire
// du menage quand le traitement sera termine ...
T_betree *T_file_component::get_B0Checked_betree(T_component *compo)
{
  TRACE3("T_file_component(%x)::get_B0Checked_betree(%x:%s)",
		 this, compo, compo->get_name()->get_value());

  T_betree *betree = NULL;

  betree = load_betrees(BC_CALL_B0CHECKER, converterName, compo);

  /* si le composant appartient a un fichier multicomposant
	 on cherche le bon betree dans l'ensemble des betree charges */
  if ( m_belongs_to_a_multi_components_file(m_component_name(compo)) )
	{
	  while(
			(betree->get_betree_name() !=
			lookup_symbol(m_component_name(compo))) &&
			(betree != NULL)
			)
		{
		  betree = betree->get_next_betree() ;
		}
	}

  if (betree != NULL)
	{
	  TRACE1("T_file_component::get_B0Checked_betree-->%x", betree) ;
	  return betree ;
	}
  TRACE0("T_file_component::get_B0Checked_betree --> NULL");

  return NULL;
}

#endif /* ACTION_NG */
// Recuperation de la T_machine b0 d'un composant
// ATTENTION :cette fonction charge les betree, il faudra penser a faire
// du menage quand le traitement sera termine ...
T_machine *T_file_component::get_b0checked_machine(T_component *compo, const char* converterName)
{
  TRACE3("T_file_component(%x)::get_b0checked_machine(%x:%s)",
		 this, compo, compo->get_name()->get_value());

  T_betree *betree = NULL;

  betree = load_betrees(BC_CALL_B0CHECKER, converterName, compo);

  if (betree != NULL)
	{
	  TRACE1("T_file_component::get_b0checked_machine-->%x",
			 betree->get_root());
	  return betree->get_root();
	}
  TRACE0("T_file_component::get_b0checked_machine--> NULL");
  return NULL;
}

// Reinitialisation de la liste des fichiers de definition
void T_file_component::set_file_definition(T_file_definition *file_def)
{
#ifdef DEBUG_MFILE
  TRACE2("T_file_component(%x)::set_file_definition(%x)", this, file_def);
#endif // DEBUG_MFILE

  T_file_definition *cur_file_def = first_file_definition;
  T_file_definition *tmp = NULL;

  while (cur_file_def != NULL)
	{
	  tmp = (T_file_definition *)cur_file_def->get_next();
	  cur_file_def->unlink();
	  cur_file_def = tmp;
	}
  first_file_definition = NULL;
  last_file_definition = NULL;

  if (file_def != NULL)
	{
	  first_file_definition = file_def;
	  first_file_definition->link();
	  last_file_definition = first_file_definition;
	  while (last_file_definition->get_next() != NULL)
		{
		  last_file_definition = (T_file_definition *)
			last_file_definition->get_next();
		  last_file_definition->link();
		}
	}
}

// Mise a jour de la liste des fichiers de definition
void T_file_component::insert_file_definition(
												    T_file_definition *file_def)
{
  TRACE2("T_file_component(%x)::insert_file_definition(%x)", this, file_def);

  if (file_def == NULL)
	{
	  return;
	}

  if (first_file_definition == NULL)
	{
	  first_file_definition = file_def;
	  last_file_definition = file_def;
	  first_file_definition->link();
	  TRACE1("link file_def %x", first_file_definition);
	}
  else
	{
	  last_file_definition->set_next(file_def);
	}

  while (last_file_definition->get_next() != NULL)
	{
	  last_file_definition = (T_file_definition *)
		last_file_definition->get_next();
	  last_file_definition->link();
	}
}


// Retourne TRUE si les T_machine du fichier sont a jour
int T_file_component::is_up_to_date(char *file_name)
{
  TRACE2("T_file_component(%x)::is_up_to_date(%s)", this, file_name);

  struct stat status;          // descripteur fichier
  time_t new_date = date;
  char *def_name = NULL;       // chemin complet du fichier de definition
  struct stat def_status;      // descripteur du fichier de definition
  unsigned int def_len = 0;    // taille du chemin complet
  unsigned int old_def_len = 0;// ancienne place allouee
  T_file_definition *file_def = NULL; // fichiers de def

  if (stat(file_name, &status) == 0)
	{
	  new_date = status.st_mtime;
	  TRACE1("La date du nouveau fichier est : %d", new_date);
	}
  else
	{
	  TRACE0("Le fichier n'existe plus");
	  return FALSE ;
	}

  // On compare la date de l'ancien fichier lu avec celle du nouveau fichier,
  // puis avec celle de tous les fichiers de definition. Des que l'une des
  // nouvelles dates est superieure a la date de l'ancien fichier, on peut en
  // deduire qu'il faut re-calculer les dependances
  file_def = first_file_definition;
  while (new_date <= date && file_def != NULL)
	{
	  TRACE1("file_def = %x", file_def);

	  if (file_def->get_path() == NULL)
		{
		  // Cas d'un fichier de definition "en local"
		  def_len = path->get_len() + file_def->get_name()->get_len() + 2;
		}
	  else
		{
		  def_len = file_def->get_path()->get_len() +
			file_def->get_name()->get_len() + 2;
		}
	  if (def_name == NULL)
		{
		  TRACE0("def_name est NULL");
		  def_name = (char *)s_volatile_malloc(def_len);
		  old_def_len = def_len;
		}
	  else if (old_def_len < def_len)
		{
		  // def_name a deja ete utilise, et il est necessaire de reallouer.
		  def_name = (char *)s_volatile_realloc(def_name, def_len);
		  old_def_len = def_len;
		}

	  if (file_def->get_path() == NULL)
		{
		  // Cas d'un fichier de definition "en local"
	                        /* 1  2*/
		  sprintf(def_name, "%s/%s",
				  path->get_value(), file_def->get_name()->get_value());
		}
	  else
		{
	                        /* 1  2*/
		  sprintf(def_name, "%s/%s",
				  file_def->get_path()->get_value(),
				  file_def->get_name()->get_value());
		}

	  TRACE1("def_name = %s", def_name);
	  if ((stat(def_name, &def_status) == 0) &&
		  (def_status.st_mtime > new_date))
		{
		  new_date = def_status.st_mtime;
		}
	  file_def = (T_file_definition *)file_def->get_next();
	}

  if (def_name != NULL) s_free(def_name);
  return (new_date == date);
}

// Calcule la nouvelle date du fichier
void T_file_component::construct_new_date(char *file_name)
{
  TRACE2(" T_file_component(%x)::construct_new_date(%s)", this, file_name);

  struct stat status;          // descripteur fichier
  char *def_name = NULL;       // chemin complet du fichier de definition
  struct stat def_status;      // descripteur du fichier de definition
  unsigned int def_len = 0;    // taille du chemin complet
  unsigned int old_def_len = 0;// ancienne place allouee
  T_file_definition *file_def = NULL; // fichiers de def

  if (stat(file_name, &status) == 0)
	{
	  date = status.st_mtime;
	  TRACE1("La date du nouveau fichier est : %d", date);
	}

  // On recupere les dates de tous les fichiers de definition
  file_def = first_file_definition;
  while (file_def != NULL)
	{
	  TRACE1("file_def = %x", file_def);
	  if (file_def->get_path() == NULL)
		{
		  // Cas d'un fichier de definition "en local"
		  def_len = path->get_len() + file_def->get_name()->get_len() + 2;
		}
	  else
		{
		  def_len = file_def->get_path()->get_len() +
			file_def->get_name()->get_len() + 2;
		}
	  if (def_name == NULL)
		{
		  TRACE0("def_name est NULL");
		  def_name = (char *)s_volatile_malloc(def_len);
		  old_def_len = def_len;
		}
	  else if (old_def_len < def_len)
		{
		  // def_nam a deja ete utilise, et il est necessaire de reallouer.
		  def_name = (char *)s_volatile_realloc(def_name, def_len);
		  old_def_len = def_len;
		}
	  if (file_def->get_path() == NULL)
		{
		  // Cas d'un fichier de definition "en local"
		                    /* 1  2*/
		  sprintf(def_name, "%s/%s",
				  path->get_value(), file_def->get_name()->get_value());
		}
	  else
		{
		                    /* 1  2*/
		  sprintf(def_name, "%s/%s",
				  file_def->get_path()->get_value(),
				  file_def->get_name()->get_value());
		}

	  TRACE1("def_name = %s", def_name);

	  if ((stat(def_name, &def_status) == 0) &&
		  (def_status.st_mtime > date))
		{
		  date = def_status.st_mtime;
		}
	  file_def = (T_file_definition *)file_def ->get_next();
	}
  if (def_name != NULL) s_free(def_name);
  TRACE1("date du fichier = %d", date);
}
//
// Fin du fichier
//
