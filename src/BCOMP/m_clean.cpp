/******************************* CLEARSY **************************************
* Fichier : $Id: m_clean.cpp,v 2.0 1999-10-06 16:39:51 sl Exp $
* (C) 2008 CLEARSY
*
* Description :		Compilateur B / Gestion des projets
*					Lib�ration m�moire
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
RCS_ID("$Id: m_clean.cpp,v 1.8 1999-10-06 16:39:51 sl Exp $") ;

/* Includes systeme */

/* Includes Composant Local */
#include "c_api.h"
#include "m_clean.h"
#include "m_pmana.h"
#include "m_compo.h"
#include "m_state.h"

// Liberation des betrees generes
void ms_memory_cleanup()
{
  TRACE0("ms_memory_cleanup()");

  // On libere les betree
  s_memory_cleanup();

  TRACE0("ms_memory_cleanup()->fin");
}

//
//	}{ Fin du fichier
//
