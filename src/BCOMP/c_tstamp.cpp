/***************************** CLEARSY **************************************
* Fichier : $Id: c_tstamp.cpp,v 2.0 1998-01-28 11:35:29 sl Exp $
* (C) 2008 CLEARSY
*
* Description :	Interface du gestionnaire de timestamp
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
****************************************************************************/
#include "c_port.h"
RCS_ID("$Id: c_tstamp.cpp,v 1.3 1998-01-28 11:35:29 sl Exp $") ;

/* Includes systeme */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

/* Includes Composant Local */
#include "c_tstamp.h"
#include "c_trace.h"

// Timestamp courant
static size_t timestamp_si = 1997 ;
static size_t timestamp2_si = 12 ;

// Obtention du timestamp courant
size_t get_timestamp(void)
{
#ifdef FULL_TRACE
TRACE1("get_timestamp()->%d", timestamp_si) ;
#endif
return timestamp_si ;
}

// Forcer le timestamp courant
void set_timestamp(size_t new_timestamp)
{
  timestamp_si = new_timestamp ;
}

// Passage a un nouveau timestamp
void next_timestamp(void)
{
#ifdef FULL_TRACE
TRACE1("next_timestamp()->%d", timestamp_si+1) ;
#endif
timestamp_si ++ ;
}

// Obtention du timestamp2 courant
size_t get_timestamp2(void)
{
#ifdef FULL_TRACE
TRACE1("get_timestamp2()->%d", timestamp2_si) ;
#endif
return timestamp2_si ;
}

// Forcer le timestamp courant
void set_timestamp2(size_t new_timestamp)
{
  timestamp2_si = new_timestamp ;
}

// Passage a un nouveau timestamp2
void next_timestamp2(void)
{
#ifdef FULL_TRACE
TRACE1("next_timestamp2()->%d", timestamp2_si+1) ;
#endif
timestamp2_si ++ ;
}
