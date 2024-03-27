/******************************* CLEARSY **************************************
* Fichier : $Id: c_port.cpp,v 2.0 2000-09-01 12:55:58 fl Exp $
* (C) 2008 CLEARSY
*
* Description :		Compilateur B
*					Module de portabilite
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
#include "c_memory.h"
#ifdef WIN32
#include <io.h>
#endif // WIN32

RCS_ID("$Id: c_port.cpp,v 1.6 2000-09-01 12:55:58 fl Exp $") ;

char *bcomp_last_slash(char *file_name)
{
    char *current = file_name;
    char *result = 0;

    for( ;*current != '\0'; ++current)
    {
        switch(*current)
        {
        case '/':
        case '\\':
            result = current;
            break;
        }
    }

    return result;
}


//const char *bcomp_last_slash(const char *file_name)
//{
//    const char *current = file_name;
//    const char *result = 0;

//    for( ;*current != '\0'; ++current)
//    {
//        switch(*current)
//        {
//        case '/':
//        case '\\':
//            result = current;
//            break;
//        }
//    }

//    return result;
//}


char *bcomp_mktemp(const char *pattern, int get_temp_path)
{
    char *result;
#ifdef linux
    int fid;
#endif /* linux */

    if(!get_temp_path)
    {
        result = (char*)s_malloc(strlen(pattern)+1);

        if(!result)
        {
            return NULL;
        }
        strcpy(result, pattern);
    }
    else
    {
        result = (char*)s_malloc(256 * sizeof(char));
        if(result)
        {
#ifdef WIN32
            if(GetTempPath(256 - strlen(pattern) - 1, result) == 0)
            {
                free(result);
                return 0;
            }
#else // WIN32
            strcpy(result, "/tmp/");
#endif // WIN32
            strcat(result, pattern);
        }
        else
        {
            return NULL;
        }
    }
#ifdef linux
    fid = mkstemp(result);
    if(fid == -1)
    {
        s_free(result);
        return NULL;
    }
    close(fid);
#else // linux
#ifdef _MSC_VER
	_mktemp_s(result, strlen(result));
#else
    result = mktemp(result);
#endif // WIN32
#endif // linux

    return result;
}

// Taille max, en caracteres, d'un pointeur
const int ADR_SIZE = 10 ;

//
//	}{	Impl�mentation strerror pour les IPX
//
#ifdef IPX
static char errno_buf[1024] ;
char *strerror(int errno)
{
sprintf(errno_buf, "(errno = %d)", errno) ;
return errno_buf ;
}
#endif

//
//	}{	Comparaison de chaines sans tenir compte de leur casse
//
int lowerstrcmp(const char *s1, const char *s2)
{
  return strcasecmp(s1, s2) ;
}

//
//	}{	DEBUT Implementation domaine public de getopt
//
/*
Date: Tue, 25 Dec 84 19:20:50 EST
From: Keith Bostic <harvard!seismo!keith>
To: genrad!sources
Subject: public domain getopt(3)

There have recently been several requests for a public
domain version of getopt(3), recently.  Thought this
might be worth reposting.

		Keith Bostic
			ARPA: keith@seismo
			UUCP: seismo!keith

======================================================================
In April of this year, Henry Spencer (utzoo!henry) released a public
domain version of getopt (USG, getopt(3)).  Well, I've been trying to
port some USG dependent software and it didn't seem to work.  The problem
ended up being that the USG version of getopt has some external variables
that aren't mentioned in the documentation.  Anyway, to fix these problems,
I rewrote the public version of getopt.  It has the following advantages:

	-- it includes those "unknown" variables
	-- it's smaller/faster 'cause it doesn't use the formatted
		output conversion routines in section 3 of the UNIX manual.
	-- the error messages are the same as S5's.
	-- it has the same side-effects that S5's has.
	-- the posted bug on how the error messages are flushed has been
		implemented.  (posting by Tony Hansen; pegasus!hansen)

I won't post the man pages since Henry already did; a special note,
it's not documented in the S5 manual that the options ':' and '?' are
illegal.  It should be obvious, but I thought I'd mention it...
This software was derived from binaries of S5 and the S5 man page, and is
(I think?) totally (I'm pretty sure?) compatible with S5 and backward
compatible to Henry's version.

		Keith Bostic
			ARPA: keith@seismo
			UUCP: seismo!keith

*UNIX is a trademark of Bell Laboratories

.. cut along the dotted line .........................................
*/

int ansi_c_needs_something_here_too;

#define NEED_GETOPT
#ifdef NEED_GETOPT
#include <stdio.h>
#include <string.h>

/*
 * get option letter from argument vector
 */
int	optind = 1,		/* index into parent argv vector */
	optopt;			/* character checked for validity */
char	*optarg;		/* argument associated with option */

#define BADCH	(int)'?'
static char  EMSG[]	="";
#define tell(s)	fputs(*nargv,stderr);fputs(s,stderr); \
		fputc(optopt,stderr);fputc('\n',stderr);return(BADCH);

int getopt(int nargc,char * const nargv[], const char *ostr)
{
	static char	*place = EMSG;	/* option letter processing */
	static const char	*lastostr = (char *) 0;
	register const char	*oli = NULL ;		/* option letter list index */

	/* LANCE PATCH: dynamic reinitialization */
	if (ostr != lastostr) {
		lastostr = ostr;
		place = EMSG;
	}
	if(!*place) {			/* update scanning pointer */
		if((optind >= nargc) || (*(place = nargv[optind]) != '-')
				|| ! *++place) {
			place = EMSG;
			return(EOF);
		}
		if (*place == '-') {	/* found "--" */
			++optind;
			return(EOF);
		}
	}				/* option letter okay? */
	if ((optopt = (int)*place++) == (int)':' || !(oli = strchr(ostr,optopt))) {
		if(!*place) ++optind;
		tell(": illegal option -- ");
	}
	if (*++oli != ':') {		/* don't need argument */
		optarg = NULL;
		if (!*place) ++optind;
	}
	else {				/* need an argument */
		if (*place) optarg = place;	/* no white space */
		else if (nargc <= ++optind) {	/* no arg */
			place = EMSG;
			tell(": option requires an argument -- ");
		}
	 	else optarg = nargv[optind];	/* white space */
		place = EMSG;
		++optind;
	}
	return(optopt);			/* dump back option letter */
}

#endif



//
//	}{	FIN Implementation domaine public de getopt
//

