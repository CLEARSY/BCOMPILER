/******************************* CLEARSY **************************************
* Fichier : $Id: p_tool.cpp,v 2.0 2000/07/11 16:10:38 fl Exp $
* (C) 2008 CLEARSY
*
* Description :	Corps du module p_tool
*				Modelisation d'un outil.
*
This file is part of RESSOURCE
    Copyright (C) 2008 ClearSy (contact@clearsy.com)

    RESSOURCE is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; GNU General Public License version 3
    of the License

    RESSOURCE is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with RESSOURCE; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
******************************************************************************/

// Includes port
#include "p_port.h"
RCS_ID("$Id: p_tool.cpp,v 1.10 2000/07/11 16:10:38 fl Exp $") ;

// Includes systeme
#include <cstdlib>
#include <cstdio>

// includes head
#include "p_head.h"

// Definition de Fonctions
// ________________________

// Retourne le nom du fichier utilisateur par defaut
const char *get_default_user_file()
{
  // Fichier utilisateur par defaut
  static const char *file_scp = "/.AtelierB" ;

  // Chemin du fichier par defaut
  char *home_lcp = getenv("HOME") ;

  // Concatenation des deux
  char *user_file_lcp = new char [strlen(file_scp)+strlen(home_lcp)+1] ;
  sprintf(user_file_lcp, "%s%s", home_lcp, file_scp) ;

#ifdef DEBUG_RESOURCES
  TRACE1("Path par defaut %s", user_file_lcp) ;
#endif

  return user_file_lcp ;
}

// Cree la chaine contant la valeur d'une ligne rattachee a une resource
static T_resource_string *new_value(T_resource_ident* resource_aop)
{
  // Pointe l'outil pere de la resource
  T_resource_tool *tool_lop =
	(T_resource_tool*)(resource_aop->get_father()) ;

  // <MARK_RESOURCE><*><TOOL><*><RESS><:>< ><VALUE><\0>
  char *line_value_lcp = new char [strlen(get_RESOURCE_MARK_RESOURCE()) +
								  1 +
								  tool_lop->get_name()->get_length() +
								  1 +
								  resource_aop->get_name()->get_length() +
								  1 +
								  1 +
								  resource_aop->get_value()->get_length() +
								  1] ;

  // Cree la valeur de la ligne
  sprintf(line_value_lcp,
		  "%s%c%s%c%s%c%c%s",
		  get_RESOURCE_MARK_RESOURCE(),
		  get_RESOURCE_MARK_STAR(),
		  tool_lop->get_name()->get_string(),
		  get_RESOURCE_MARK_STAR(),
		  resource_aop->get_name()->get_string(),
		  get_RESOURCE_MARK_COLON(),
		  get_RESOURCE_MARK_SPACE(),
		  resource_aop->get_value()->get_string()) ;


  T_resource_string *new_value_lop = new T_resource_string(line_value_lcp) ;

  delete [] line_value_lcp;

  // Retourne la nouvelle valeur
  return new_value_lop ;
}

// Cree une nouvelle ligne rattachee a une resource
static void new_line_value(T_resource_ident* resource_aop)
{
#ifdef DEBUG_RESOURCES
  ENTER_TRACE ;
  TRACE2("-> New RESOURCE LINE ? of %x name \"%s\"",
		 resource_aop,
		 (resource_aop->get_name() != NULL)?
		 resource_aop->get_name()->get_string():"VIDE") ;
#endif

  // Appelle le manager
  T_resource_manager *manager_lop = get_resource_manager() ;
  ASSERT(manager_lop != NULL) ;

  // Trouve le fichier ou ajouter la ligne rattachee a la resource.
  T_resource_file *file_lop = manager_lop->get_file(RESOURCE_EXPLICIT) ;

  // Si le fichier explicite n'existe pas,
  // Le fichier par defaut est celui de l'utilisateur
  if(file_lop == NULL)
	{
	  file_lop = manager_lop->get_file(RESOURCE_USER) ;

	  if(file_lop == NULL)
		{
		  // Si besion, cree un nouveau fichier utilisateur
		  file_lop = manager_lop->set_file(get_default_user_file(),
										   RESOURCE_USER) ;
		}
	}

  // Calcule du numero de ligne
  int next_number_li = 1 ;

  T_resource_item **first_line_lopp = file_lop->get_adr_first_line() ;
  T_resource_item **last_line_lopp  = file_lop->get_adr_last_line() ;

  if ((*last_line_lopp) != NULL)
	{
	  next_number_li = ((T_resource_line*)(*last_line_lopp))->get_number()+1 ;
	}

  // Calcule la valeur de la ligne
  T_resource_string *line_value_lop = new_value(resource_aop) ;

  // Cree la nouvelle ligne
  T_resource_line *line_lop = new T_resource_line(next_number_li,
												  line_value_lop,
												  first_line_lopp,
												  last_line_lopp,
												  file_lop) ;

  // Libere la chaine
  delete line_value_lop ;
  line_value_lop = NULL ;

  // Attribute la resource a la ligne
  line_lop->set_item(resource_aop) ;

  // Rattache la ligne a la resource
  resource_aop->set_line(line_lop) ;

#ifdef DEBUG_RESOURCES
  TRACE3("<- New RESOURCE LINE %x of %x name \"%s\"",
		 line_lop,
		 resource_aop,
		 (resource_aop->get_name() != NULL)?
		 resource_aop->get_name()->get_string():"VIDE") ;
  EXIT_TRACE ;
#endif
}

// Mise a jour de la ligne rattachee a une resource
static void set_line_value(T_resource_ident *resource_aop)
{
#ifdef DEBUG_RESOURCES
  ENTER_TRACE ;
  TRACE3("-> Set RESOURCE LINE %x \"%s\" value \"%s\"",
		 resource_aop,
		 (resource_aop != NULL)?
		 resource_aop->get_name()->get_string():"VIDE",
		 (resource_aop != NULL)?
		 resource_aop->get_value()->get_string():"VIDE") ;
#endif

  // Pointeur sur la ligne courante car elle contient tjrs la valeur de la ress
  T_resource_line *line_lop = resource_aop->get_line() ;

  if(line_lop != NULL)
	{
	  // Calcule la valeur de la ligne
	  T_resource_string *line_value_lop = new_value(resource_aop) ;

	  // Mise a jour de la ligne
	  line_lop->set_value(line_value_lop) ;

	  // Libere ress_name_lcp
	  delete line_value_lop ;
	  line_value_lop = NULL ;
	}

#ifdef DEBUG_RESOURCES
  TRACE3("<- Set RESOURCE LINE %x \"%s\" value \"%s\"",
		 resource_aop,
		 (resource_aop != NULL)?
		 resource_aop->get_name()->get_string():"VIDE",
		 (resource_aop != NULL)?
		 resource_aop->get_value()->get_string():"VIDE") ;
  EXIT_TRACE ;
#endif
}


// Definition de Methodes
// _______________________

// Constructeur
T_resource_tool::T_resource_tool(const char *new_name_acp,
										  T_resource_item **adr_first_aopp,
										  T_resource_item **adr_last_aopp,
										  T_resource_item *new_father_aop)
  : T_resource_item(adr_first_aopp, adr_last_aopp, new_father_aop)
{
#ifdef DEBUG_RESOURCES
  ENTER_TRACE ;
  TRACE2("-> New TOOL %x name \"%s\"", this, new_name_acp) ;
#endif

  // Creation du nom
  name_op = new T_resource_string(new_name_acp) ;

  // Positionne la liste de resource
  first_resource_op = NULL ;
  last_resource_op  = NULL ;

#ifdef DEBUG_RESOURCES
  TRACE2("<- New TOOL %x name \"%s\"", this, new_name_acp) ;
  EXIT_TRACE ;
#endif
}

// Destructeur
T_resource_tool::~T_resource_tool(void)
{
#ifdef DEBUG_RESOURCES
  ENTER_TRACE ;
  TRACE2("Del TOOL %x name \"%s\"", this, name_op) ;
#endif

  // Libere la liste des resources
  delete_list(first_resource_op, last_resource_op) ;

  // Libere la cha�ne nom
  if(get_name() != NULL)
	{
	  delete name_op ;
	}
#ifdef DEBUG_RESOURCES
  EXIT_TRACE ;
#endif
}

// Recherche une resource de la liste
T_resource_ident *
T_resource_tool::get_resource(const char *resource_name_acp)
{
#ifdef DEBUG_RESOURCES
  ENTER_TRACE ;
  TRACE2("-> Get TOOL RESOURCE %x name \"%s\" value ?",
		 this,
		 resource_name_acp) ;
#endif

  // Pointeur courant sur la liste
  T_resource_ident *resource_lop = (T_resource_ident*)first_resource_op ;

  // Cree un objet string de resource_name_acp
  T_resource_string *string_lop = new T_resource_string(resource_name_acp) ;

  // Parcours de la liste
  while( (resource_lop != NULL) &&
		 (string_lop->compare(resource_lop->get_name()) != TRUE) )
	{
	  resource_lop = (T_resource_ident*)resource_lop->get_next() ;
	}
  // Libere string_lop
  delete string_lop ;
  string_lop = NULL ;

#ifdef DEBUG_RESOURCES
  TRACE3("<- Get TOOL RESOURCE %x name \"%s\" value \"%s\"",
		 resource_lop,
		 (resource_lop != NULL)?
		 resource_lop->get_name()->get_string():"VIDE",
		 (resource_lop != NULL)?
		 resource_lop->get_value()->get_string():"VIDE") ;
  EXIT_TRACE ;
#endif

  // Retourne la resource ou NULL
  return resource_lop ;
}

// Affecte une resource de la liste
T_resource_ident *
T_resource_tool::set_resource(const char *resource_name_acp,
								const char *resource_value_acp,
								T_resource_line *line_aop)
{
#ifdef DEBUG_RESOURCES
  ENTER_TRACE ;
  TRACE3("-> Set TOOL RESOURCE %x name \"%s\" value \"%s\"",
		 this,
		 resource_name_acp,
		 resource_value_acp) ;
#endif

  // Recherche la resource dans la liste
  T_resource_ident *ress_lop = get_resource(resource_name_acp) ;

  // Si la resource existe
  if(ress_lop != NULL)
	{
	  // Affecte la valeur de la resource
	  ress_lop->set_value(resource_value_acp) ;

	  // Si ligne en parametre
	  if(line_aop != NULL)
		{
		  T_resource_line *old_line_lop = ress_lop->get_line() ;

		  if(old_line_lop == NULL)
			{
			  fprintf(stderr,
					  "Resource \"%s\" sans ligne rattachee",
					  resource_name_acp) ;

			  return NULL ; // Echec
			}
		  else
			{
			  // Evite de detruire la ligne en parametre
			  if(old_line_lop != line_aop)
				{
				  T_resource_file *file_lop =
					((T_resource_file*)old_line_lop->get_father()) ;

				  T_resource_item **first_line_lopp =
					file_lop->get_adr_first_line() ;
				  T_resource_item **last_line_lopp =
					file_lop->get_adr_last_line() ;

				  // Detache la ligne
				  old_line_lop->set_item(NULL) ;

				  // Detruit la ligne
				  delete_item(old_line_lop,
							  *first_line_lopp,
							  *last_line_lopp) ;
				}
			}
		  // Attribut la ligne a la resource
		  ress_lop->set_line(line_aop) ;

		  // Attache la resource a la ligne
		  line_aop->set_item(ress_lop) ;
		}
	  // Pas de ligne en parametre
	  else
		{
		  T_resource_line *old_line_lop = ress_lop->get_line() ;

		  if(old_line_lop == NULL)
			{
			  fprintf(stderr,
					  "Resource \"%s\" sans ligne rattachee",
					  resource_name_acp) ;

			  return NULL ; // Echec
			}
		  else
			{
			  T_resource_file *file_lop =
				((T_resource_file*)old_line_lop->get_father()) ;

			  T_resource_item **first_line_lopp = file_lop->get_adr_first_line() ;
			  T_resource_item **last_line_lopp  = file_lop->get_adr_last_line() ;

			  if( (file_lop->get_source() == RESOURCE_EXPLICIT) ||
				  ( (file_lop->get_source() == RESOURCE_USER) &&
					(((T_resource_manager*)
					  get_father())->get_file(RESOURCE_EXPLICIT) == NULL) ) )
				{
				  // Met a jour la ligne
				  set_line_value(ress_lop) ;
				}
			  else
				{
				  // Detache la ligne
				  old_line_lop->set_item(NULL) ;

				  // Detruit la ligne
				  delete_item(old_line_lop,
							  *first_line_lopp,
							  *last_line_lopp) ;

				  // Cree une nouvelle ligne
				  new_line_value(ress_lop) ;
				}
			}
		}
	}
  // La resource n'existe pas
  else
	{
	  // Cree une nouvelle resource rattachee a un fichier USER ou EXPLICIT
	  T_resource_string *ress_name_lop =
		new T_resource_string(resource_name_acp) ;

	  T_resource_string *ress_value_lop =
		new T_resource_string(resource_value_acp) ;

	  // Cree la resource rattachee a la ligne en argument
	  ress_lop = new T_resource_ident(ress_name_lop,
									   ress_value_lop,
									   line_aop,
									   &first_resource_op,
									   &last_resource_op,
									   this) ;

	  if(line_aop == NULL)
		{
		  // Cree une nouvelle ligne rattachee a la resource
		  new_line_value(ress_lop) ;
		}
	  else
		{
		  // Attache la ligne existante a la resource
		  line_aop->set_item(ress_lop) ;
		}

      delete ress_name_lop ;
      delete ress_value_lop ;
	}

#ifdef DEBUG_RESOURCES
  TRACE3("<- Set TOOL RESOURCE %x name \"%s\" value \"%s\"",
		 ress_lop,
		 (ress_lop != NULL)? ress_lop->get_name()->get_string():"VIDE",
		 (ress_lop != NULL)? ress_lop->get_value()->get_string():"VIDE") ;
  EXIT_TRACE ;
#endif

  // Retourne la resource modifiee ou creee
  return ress_lop ;
}

// Libere une resource de la liste
void
T_resource_tool::delete_resource(const char *resource_name_acp)
{
#ifdef DEBUG_RESOURCES
  ENTER_TRACE ;
  TRACE2("-> Del TOOL RESOURCE %x name \"%s\"",
		 this,
		 resource_name_acp) ;
#endif

  // Recherche la resource dans la liste
  T_resource_ident *resource_lop = get_resource(resource_name_acp) ;

  if(resource_lop != NULL)
	{
	  // Libere la resource
	  delete_item(resource_lop, first_resource_op, last_resource_op) ;
	}

#ifdef DEBUG_RESOURCES
  TRACE1("<- Del TOOL RESOURCE %x",
		 resource_lop) ;
  EXIT_TRACE ;
#endif
}


//Imprime les ressources dans un fichier
void T_resource_tool::print_resources(FILE *fd)
{
#ifdef DEBUG_RESOURCES
  TRACE1("-> T_resource_tool::print_resources(%x)", fd) ;
#endif

  T_resource_ident *resource_lop = (T_resource_ident*)first_resource_op ;
  T_resource_string *string_lop = NULL;

	// Parcours de la liste
  while(resource_lop != NULL)
	{
	  //r�cup�ration de la chaine contenant
	  //la valeur d'une ligne rattachee a la resource
	  string_lop = new_value(resource_lop) ;
	  //impression de cette chaine
	  string_lop->print(fd) ;
	  delete string_lop;
	  //impression d'un retour a la ligne
	  fputc('\n' ,fd) ;
	  resource_lop = (T_resource_ident*)resource_lop->get_next() ;
	}

#ifdef DEBUG_RESOURCES
  TRACE1("<- T_resource_tool::print_resources(%x)", fd) ;
#endif
}
// Fin de fichier
// _______________
