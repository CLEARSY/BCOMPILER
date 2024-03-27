/******************************* CLEARSY **************************************
* Fichier : $Id: c_base.cpp,v 2.0 1999-10-06 16:39:38 sl Exp $
* (C) 2008 CLEARSY
*
* Description :		Compilateur B
*					Predicats
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
RCS_ID("$Id: c_base.cpp,v 1.14 1999-10-06 16:39:38 sl Exp $") ;

/* Includes systeme */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

/* Includes Composant Local */
#include "c_api.h"

//
//	}{	Constructeur de la classe T_integer
//
T_integer::T_integer(T_symbol *new_value)
  : T_object(NODE_INTEGER)
{
#ifdef FULL_TRACE
  TRACE1("T_integer::T_integer(%x)", new_value) ;
#endif

  // modif jfm pour correction bug 2218
  const char *cur_char = (new_value->get_value());

  if (*cur_char == '-')
	{
	  cur_char++;
	  is_positive = FALSE ;
	}
  else
	{
	  is_positive = TRUE ;
	}

  //elimination des 0 non significatifs
  while ((*cur_char != '\0') && (*cur_char == '0'))
	{
	  cur_char++;
	}
  //pour eviter un message du style "-0 plus petit que 0"
  //on force 0 a etre positif
  if (*cur_char == '\0')
	{
	  value = lookup_symbol("0");
	  is_positive = TRUE;
	}
  else
	{
	  value = lookup_symbol(cur_char) ;
	}
}

T_integer::T_integer(unsigned int new_value)
  : T_object(NODE_INTEGER)
{
#ifdef FULL_TRACE
  TRACE1("T_integer::T_integer(%x)", new_value) ;
#endif
  char temp[128] ; // A FAIRE
  sprintf(temp, "%u", new_value) ;
  value = lookup_symbol(temp, strlen(temp)) ;
  is_positive = TRUE ;
}

// Pour savoir si l'on est plus petit qu'un autre
// (si ref == NULL, ref = - infini)
int T_integer::is_less_than(T_integer *ref)
{
	/*
  TRACE6("T_integer(%x,%s%s)::is_less_than(%x, %s%s)",
		 this,
		 (is_positive == TRUE) ? "+" : "-",
		 value->get_value(),
		 ref,
		 (ref->is_positive == TRUE) ? "+" : "-",
		 (ref == NULL) ? "- infini" : ref->value->get_value()) ;
	*/

  if (ref == NULL)
	{
	  return FALSE ;
	}
  //plus petit si pas egal et pas plus grand
  return ( (!is_greater_than(ref)) && (!is_equal_to(ref)) );
}

// Pour savoir si l'on est plus grand qu'un autre
// (si ref == NULL, ref = + infini)
int T_integer::is_greater_than(T_integer *ref)
{
	/*
  TRACE6("T_integer(%x,%s%s)::is_greater_than(%x, %s%s)",
		 this,
		 (is_positive == TRUE) ? "+" : "-",
		 value->get_value(),
		 ref,
		 (ref->is_positive == TRUE) ? "+" : "-",
		 (ref == NULL) ? "+ infini" : ref->value->get_value()) ;
	*/

  if (ref == NULL)
	{
	  return FALSE ;
	}

  //si la valeur courante est positive
  if (is_positive == TRUE)
	{
	  //les deux positives
	  if (ref->is_positive == TRUE)
		{
		  if (value->get_len() == ref->get_value()->get_len())
			{
			  //teste par le trop_grand de 2218ter
			  return value->is_greater_than(ref->get_value());
			}
		  else
			{
			  // teste par le beaucoup_trop_grand de 2218ter
			  return (value->get_len() > ref->get_value()->get_len())
				? TRUE : FALSE ;
			}
		}
	  //si la valeur de comparaison est negative
	  else
		{
		  return TRUE;
		}
	}
  //sinon elle est negative
  else
	{
	  //si la valeur de comparaison est positive
	  if (ref->is_positive == TRUE)
		{
		  return FALSE;
		}
	  //les deux negatives
	  else
		{
		  if (value->get_len() == ref->get_value()->get_len())
			{
			  //le plus grand a la plus petite valeur absolue
			  return (value->is_less_than(ref->get_value()));
			}
		  else
			{
			  //teste par le beaucoup_trop_petit de 2218ter
			  return (value->get_len() > ref->get_value()->get_len())
				? FALSE : TRUE ;
			}
		}
	}
}

  // Pour savoir si l'on est plus petit ou egal a un autre
  // (si ref == NULL, ref = - infini)
int T_integer::is_less_than_or_equal(T_integer *ref)
{
	/*
  TRACE6("T_integer(%x,%s%s)::is_less_than_or_equal(%x, %s%s)",
		 this,
		 (is_positive == TRUE) ? "+" : "-",
		 value->get_value(),
		 ref,
		 (ref->is_positive == TRUE) ? "+" : "-",
		 (ref == NULL) ? "- infini" : ref->value->get_value()) ;
	*/

  if (ref == NULL)
	{
	  return FALSE ;
	}
  //si plus petit ou egal TRUE sinon FALSE
  return (is_less_than(ref) || is_equal_to(ref)) ? TRUE : FALSE ;
}

  // Pour savoir si l'on est plus grand ou egal a un autre
  // (si ref == NULL, ref = + infini)
int T_integer::is_greater_than_or_equal(T_integer *ref)
{
	/*
  TRACE6("T_integer(%x,%s%s)::is_greater_than_or_equal(%x, %s%s)",
		 this,
		 (is_positive == TRUE) ? "+" : "-",
		 value->get_value(),
		 ref,
		 (ref->is_positive == TRUE) ? "+" : "-",
		 (ref == NULL) ? "+ infini" : ref->value->get_value()) ;
	*/

  if (ref == NULL)
	{
	  return FALSE ;
	}
  //si on est plus grand ou egal TRUE sinon FALSE
  return (is_greater_than(ref) || is_equal_to(ref)) ? TRUE : FALSE ;
}

// Obtention de la valeur sous type forme d'int
int T_integer::get_int_value(void)
{
  int val = atoi(value->get_value()) ;

  if (is_positive == FALSE)
	{
	  val = - val ;
	}

  return val ;
}
