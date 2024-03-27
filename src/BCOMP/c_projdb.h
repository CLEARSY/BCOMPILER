/******************************* CLEARSY **************************************
* Fichier : $Id: c_projdb.h,v 2.0 1998-01-29 11:24:08 sl Exp $
* (C) 2008 CLEARSY
*
* Description :		Interfacage avec la base de donnees projet du
*					Moniteur de Session
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
#ifndef _C_PROJDB_H_
#define _C_PROJDB_H_

// Prise en compte de la base de donnees projet
extern void load_project_database(const char *database_file_name);

// load a library into current project
extern void load_project_library(const char *library_file_name);

#endif /* _C_PROJDB_H_ */


