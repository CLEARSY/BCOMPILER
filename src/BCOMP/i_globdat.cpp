/******************************* CLEARSY **************************************
* Fichier : $Id: i_globdat.cpp,v 2.0 1999-01-14 16:19:11 ab Exp $
* (C) 2008 CLEARSY
*
* Description :		Interface des donnees globales necessaires a la
*                   verification de B0
*
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
#ifdef B0C

#include "i_globdat.h"

static T_B0_check check = B0CHK_DEFAULT;

static T_list_ident *current_list_ident ;

T_B0_check get_check(void)
{
  return check;
}

void set_check(T_B0_check value)
{
  check = value;
}

T_list_ident * get_current_list_ident(void)
{
  return current_list_ident;
}

void set_current_list_ident(T_list_ident * value)
{
  current_list_ident = value;
}

#endif // B0C
