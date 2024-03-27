/******************************* CLEARSY **************************************
* Date : $Id: c_secure.h,v 2.0 1999-04-22 16:39:51 atelierb Exp $
*
* (C) 2008 CLEARSY
*
* Description :		Gestion de memoire securisee
*					Emulation de l'ancienne API maintenant obsolete
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
#ifdef __BCOMP__
#error "c_secure.h est obsolete et ne doit plus etre utilise dans BCOMP"
#else // !__BCOMP__
#include <c_io.h>
#include <c_memory.h>
#include <c_cleanup.h>
#endif // __BCOMP__


