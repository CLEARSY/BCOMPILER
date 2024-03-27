/******************************* CLEARSY **************************************
* Fichier : $Id: c_io.cpp,v 2.0 1999-10-06 16:39:43 sl Exp $
*
* (C) 2008 CLEARSY
*
* Description :		Entrees/sorties securisees
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
RCS_ID("$Id: c_io.cpp,v 1.2 1999-10-06 16:39:43 sl Exp $") ;

// Includes systeme
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

// Includes locaux
#include "c_api.h"

//
// BLOC I/O SECURISEES
//

// fprintf securise
EXTERN void s_fprintf(FILE *file_aip, const char *format_acp, ...)
{
  va_list ap ;

  va_start(ap, format_acp) ;

  if (vfprintf(file_aip, format_acp, ap) < 0)
	{
	  toplevel_error(FATAL_ERROR, get_error_msg(E_IO_ERROR), strerror(errno)) ;
	}

  va_end(ap) ;
}

// fprintf raw securise
EXTERN void s_fprintf_raw(FILE *file_aip, const char *string_acp)
{
  const char *p = string_acp ;

  while (*p != '\0')
	{
		//	  TRACE1("s_fprintf_raw <%c>", *p) ;
	  if (fprintf(file_aip, "%c", *p) <= 0)
		{
		  toplevel_error(FATAL_ERROR, get_error_msg(E_IO_ERROR), strerror(errno)) ;
		}
	  p++ ;
	}
}


//
// BLOC INFOS
//

// Informations de compilation (pour compatibilite)
EXTERN void secure_info(void)
{
  file_fprintf_line("- B Secure Library (with B Compiler %s)", get_bcomp_version()) ;
#if defined(TRACE)
  file_fprintf_line("Compiled with -DTRACE - clients MUST be compiled with this option") ;
#endif
#if defined(PATCH_CHECK)
  file_fprintf_line("Run-time memory patch checks (debug version)") ;
#else
#if defined(MEMORY_CHECK)
  file_fprintf_line("Run-time memory integrity checks (verbose version)") ;
#else
  file_fprintf_line("No run-time memory checks (optimised version)") ;
#endif

#endif

}

//
// BLOC FIN DU FICHIER
//
