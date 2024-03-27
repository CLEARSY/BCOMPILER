/******************************* CLEARSY **************************************
* Date : $Id: c_io.h,v 2.0 1999-04-20 08:14:06 sl Exp $
*
* (C) 2008 CLEARSY
*
* Description :		Entrees/sorties securisees
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
#ifndef _C_IO_H
#define _C_IO_H

// Les fonctions ont des prototypes C pour l'interfacage Betree -> B0tree

// fprintf securise
EXTERN void s_fprintf(FILE *file_aip, const char *format_acp, ...) ;

// fprintf raw securise (pas d'analyse de format)
EXTERN void s_fprintf_raw(FILE *file_aip, const char *string_acp) ;


#endif /* _C_IO_H */


