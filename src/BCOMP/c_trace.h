/******************************* CLEARSY **************************************
* Fichier : $Id: c_trace.h,v 2.0 1997-07-08 07:44:54 sl Exp $
* (C) 2008 CLEARSY
*
* Description :	Interface du Gestionnaire de Traces
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
#ifndef _C_TRACE_H
#define _C_TRACE_H

#if (defined TRACE)

#define TRACE0(x)					trace(__FILE__,__LINE__,x)
#define TRACE1(x,x1)				trace(__FILE__,__LINE__,x,x1)
#define TRACE2(x,x1,x2)				trace(__FILE__,__LINE__,x,x1,x2)
#define TRACE3(x,x1,x2,x3)			trace(__FILE__,__LINE__,x,x1,x2,x3)
#define TRACE4(x,x1,x2,x3,x4)		trace(__FILE__,__LINE__,x,x1,x2,x3,x4)
#define TRACE5(x,x1,x2,x3,x4,x5)	trace(__FILE__,__LINE__,x,x1,x2,x3,x4,x5)
#define TRACE6(x,x1,x2,x3,x4,x5,x6)	trace(__FILE__,__LINE__,x,x1,x2,x3,x4,x5,x6)
#define TRACE7(x,x1,x2,x3,x4,x5,x6,x7)	trace(__FILE__,__LINE__,x,x1,x2,x3,x4,x5,x6,x7)
#define TRACE8(x,x1,x2,x3,x4,x5,x6,x7,x8)	trace(__FILE__,__LINE__,x,x1,x2,x3,x4,x5,x6,x7,x8)
#define TRACE9(x,x1,x2,x3,x4,x5,x6,x7,x8,x9)	trace(__FILE__,__LINE__,x,x1,x2,x3,x4,x5,x6,x7,x8,x9)
#define TRACE10(x,x1,x2,x3,x4,x5,x6,x7,x8,x9,x10)	trace(__FILE__,__LINE__,x,x1,x2,x3,x4,x5,x6,x7,x8,x9,x10)
#ifdef FULL_TRACE
#define ENTER_TRACE					TRACE0(" ENTER_TRACE") ; enter_trace()
#define EXIT_TRACE					TRACE0("EXIT_TRACE") ; exit_trace()
#else
#define ENTER_TRACE					enter_trace()
#define EXIT_TRACE					exit_trace()
#endif

// Prototypes C pour l'interfacage Betree -> B0tree
EXTERN void trace(const char *file_acp, long line_ai, const char *format_asp, ...) ;

EXTERN void enter_trace(void) ;
EXTERN void exit_trace(void) ;

#else

#define TRACE0(x)
#define TRACE1(x,x1)
#define TRACE2(x,x1,x2)
#define TRACE3(x,x1,x2,x3)
#define TRACE4(x,x1,x2,x3,x4)
#define TRACE5(x,x1,x2,x3,x4,x5)
#define TRACE6(x,x1,x2,x3,x4,x5,x6)
#define TRACE7(x,x1,x2,x3,x4,x5,x6,x7)
#define TRACE8(x,x1,x2,x3,x4,x5,x6,x7,x8)
#define TRACE9(x,x1,x2,x3,x4,x5,x6,x7,x8,x9)
#define TRACE10(x,x1,x2,x3,x4,x5,x6,x7,x8,x9,x10)
#define ENTER_TRACE
#define EXIT_TRACE

#endif // not TRACE

#endif
