/******************************* CLEARSY **************************************
* Fichier : $Id: p_port.h,v 2.0 1998/11/02 14:16:31 rp Exp $
* (C) 2008 CLEARSY
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
#ifndef _P_PORT_H_
#define _P_PORT_H_

// Prototype "a la C"
#if defined(__cplusplus)
#define EXTERN extern "C"
#else
#define EXTERN extern
#endif

#ifdef __RESOURCE__
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <sys/types.h>
#include <cassert>

// Identifiant RCS sans warning pour C et C++
#define RCS_ID(x) \
	static const char *rcs_id = x ; \
	static void _x(const char *) ; \
	static void _y(const char *stub) { _x(stub) ; stub = rcs_id ; } ; \
	static void _x(const char *stub) { _y(stub) ; }


// Assertions que l'on peut enlever a la compilation
#if (!defined(NO_CHECKS))
#define ASSERT(x) assert(x)
#else
#define ASSERT(x)
#endif

#define TRUE 1
#define FALSE 0
#endif // __RESOURCE__

#endif /* _P_PORT_H_ */
