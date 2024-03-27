/******************************* CLEARSY **************************************
* Fichier : $Id: m_synsta.h,v 2.0 1999-09-24 08:30:43 clb Exp $
*
* (C) 2008 CLEARSY
*
* Description : analyseur syntaxique du fichier d'etat d'un composant
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
#ifdef  ACTION_NG

#ifndef _M_SYNSTA_H_
#define _M_SYNSTA_H_

#include "m_comsta.h"

extern T_component_state *
state_syntax_analysis(const char *state_file_name) ;

#endif /* _M_SYNSTA_H_ */

#endif /* ACTION_NG */
