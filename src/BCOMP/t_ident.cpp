/******************************* CLEARSY **************************************
* Fichier : $Id: t_ident.cpp,v 2.0 1999-09-28 10:26:24 sl Exp $
*
* (C) 2008 CLEARSY
*
* Description :		Compilateur B
*					Typage par identificateur
*
* Compilations :
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
RCS_ID("$Id: t_ident.cpp,v 1.6 1999-09-28 10:26:24 sl Exp $") ;

/* Includes systeme */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

/* Includes Composant Local */
#include "c_api.h"

//
// API de haut niveau
//
T_ident *T_type::get_typing_ident(void)
{
  // Surtout ne pas appeler make_type_name dans la trace sous peine de bouclage
  // car make_type_name appelle get_typing_ident
  TRACE2("T_type(%x)::get_typing_ident() -> %x",
		 this,
		 typing_ident) ;
  return typing_ident ;
}

void T_type::set_typing_ident(T_ident *new_typing_ident)
{
  // Surtout ne pas appeler make_type_name dans la trace sous peine de bouclage
  // car make_type_name appelle get_typing_ident
  TRACE3("T_type(%x:%x)::set_typing_ident(%x)",
		 this,
		 typing_ident,
		 new_typing_ident) ;

  // On ne met le typing_ident a jour que s'il n'existe pas (car seul
  // le premier typing_ident compte) et si la nouvelle valeur existe
  // (les clients sont offensifs)
  if ( (new_typing_ident != NULL) && (typing_ident == NULL) )
	{
	  typing_ident = new_typing_ident ;
	}
}

T_ident *T_expr::get_typing_ident(void)
{
  return (get_B_type() == NULL) ? NULL : get_B_type()->get_typing_ident() ;
}

void T_expr::set_typing_ident(T_ident *new_typing_ident)
{
  if (get_B_type() != NULL)
	{
	  // reprise sur erreur
	  get_B_type()->set_typing_ident(new_typing_ident) ;
	}
}

T_ident *T_B0_type::get_typing_ident(void)
{
  TRACE2("T_B0_type(%x)::get_typing_ident() -> on demande au type %x",
		 this,
		 get_B_type()) ;
  return (get_B_type() == NULL) ? NULL : get_B_type()->get_typing_ident() ;
}

void T_B0_type::set_typing_ident(T_ident *new_typing_ident)
{
  if (get_B_type() != NULL)
	{
	  // reprise sur erreur
	  get_B_type()->set_typing_ident(new_typing_ident) ;
	}
}

//
// Fin du fichier
//
