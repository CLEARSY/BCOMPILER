/******************************* CLEARSY **************************************
    Fichier : $Id: m_msg.h,v2.0 1999-03-03 10:27:07 cc Exp $
    (C) 2008 CLEARSY

    Description :		Moniteur de session
                    Messages d'erreur et d'avertissement

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
#ifndef _M_MSG_H_
#define _M_MSG_H_

// Messages d'erreur
extern T_user_error_code U_EXISTING_COMPONENT;
extern T_user_error_code U_BAD_COMPONENT_NAME;
extern T_user_error_code U_MODIFIED_FILE_ADD;
extern T_user_error_code U_MODIFIED_FILE_REMOVE;
extern T_user_error_code U_EXISTING_FILE;
extern T_user_error_code U_REMOVE_SOURCE_PATH;
extern T_user_error_code U_ADD_SOURCE_PATH;
extern T_user_error_code U_NON_EXISTING_COMPONENT;
extern T_user_error_code U_ADD_PROOF_MECHANISM;
extern T_user_error_code U_REMOVE_PROOF_MECHANISM;

// Messages d'avertissement
extern T_user_error_code U_ATB_DIFF_NOT_IMPL;


// Fonction d'initialisation
extern void m_init_messages(void);

#endif // _M_MSG_H_
