/******************************* CLEARSY **************************************
* Fichier : $Id: m_msg.cpp,v 2.0 2006-02-28 16:58:09 va Exp $
* (C) 2008 CLEARSY
*
* Description :		Moniteur de session
*                   Messages d'erreur et d'avertissement
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
RCS_ID("$Id: m_msg.cpp,v 1.4 2006-02-28 16:58:09 va Exp $") ;


/* Includes systeme */

/* Includes bibliotheques */
#include "c_api.h"

/* Includes Composant Local */
#include "m_msg.h"


// Messages d'erreur
T_user_error_code U_EXISTING_COMPONENT;
T_user_error_code U_BAD_COMPONENT_NAME;
T_user_error_code U_MODIFIED_FILE_ADD;
T_user_error_code U_MODIFIED_FILE_REMOVE;
T_user_error_code U_REMOVE_SOURCE_PATH;
T_user_error_code U_ADD_SOURCE_PATH;
T_user_error_code U_ADD_PROOF_MECHANISM;
T_user_error_code U_REMOVE_PROOF_MECHANISM;

// Messages d'avertissement
T_user_warning_code U_ATB_DIFF_NOT_IMPL;
T_user_warning_code U_EXISTING_FILE;
T_user_error_code U_NON_EXISTING_COMPONENT;

// initialisation des messages d'erreur
static void m_init_error_messages(void)
{
  U_EXISTING_COMPONENT = add_user_error_msg(
	"Component %s in file %s can't be added, there is already a component with the same name in the project");
  U_BAD_COMPONENT_NAME = add_user_error_msg(
	"Component %s not present in the project");
  U_MODIFIED_FILE_ADD = add_user_error_msg(
	"File %s is modified, component %s added during action");
  U_MODIFIED_FILE_REMOVE = add_user_error_msg(
	"File %s is modified, component %s removed during action");
  U_REMOVE_SOURCE_PATH =  add_user_error_msg(
	"There is no source path %s in the current project");
  U_ADD_SOURCE_PATH = add_user_error_msg(
	"Source path %s already present in the current project");
  U_ADD_PROOF_MECHANISM = add_user_error_msg(
    "Proof mechanism %s is already authorized in the current project");
  U_REMOVE_PROOF_MECHANISM = add_user_error_msg(
    "Proof mechanism %s is not authorized in the current project");
}

// initialisation des messages d'avertissement
static void m_init_warning_messages(void)
{
  U_ATB_DIFF_NOT_IMPL= add_user_warning_msg(
	"Differential Atelier B is not implemented for substitution %d");
  U_EXISTING_FILE = add_user_warning_msg(
	"File %s can't be added, there is already a file with the same name in the project\n(%s/%s.%s)");
  U_NON_EXISTING_COMPONENT = add_user_warning_msg(
	"Component %s does not exists, ignoring generated from information");
}

// initialisation de tous les messages
void m_init_messages(void)
{
  m_init_error_messages();
  m_init_warning_messages();
}
