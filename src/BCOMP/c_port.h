/******************************* CLEARSY **************************************
* Fichier : $Id: c_port.h,v 2.0 2000-05-02 13:57:13 lv Exp $
* (C) 2008 CLEARSY
*
* Compilations :	-DTRACE 	pour compiler avec des traces
*					-DNO_CHECKS 	pour compiler dans verifications
*					-D__STDC__  	pour compiler en C-ANSI
*					-D__cplusplus 	pour compiler en C++
*					-DIPX 		pour compiler sur une IPX
*
* Description :		Defintions usuelles C (ANSI et standard) et C++
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
#ifndef _C_PORT_H_
#define _C_PORT_H_
#ifdef WIN32
/* prevent including msxml.h, which conflicts with some bcomp definitions */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif /* WIN32 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <assert.h>
#ifdef __GNUC__
#include <dirent.h>
#endif /* __GNUC__ */

#ifdef WIN32
#define PATH_SEPARATOR_CHAR '\\'
#define PATH_SEPARATOR_STRING "\\"
#else /* WIN32 */
#define PATH_SEPARATOR_CHAR '/'
#define PATH_SEPARATOR_STRING "/"
#endif /* WIN32 */

#ifdef WIN32
// On implemente ntohl et ntohs
#define ntohl(x) \
((unsigned long int)((((unsigned long int)(x) & 0x000000ffU) << 24) | \
			     (((unsigned long int)(x) & 0x0000ff00U) <<  8) | \
			     (((unsigned long int)(x) & 0x00ff0000U) >>  8) | \
			     (((unsigned long int)(x) & 0xff000000U) >> 24)))


#define ntohs(x) \
	((unsigned short int)((((unsigned short int)(x) & 0x00ff) << 8) | \
			      (((unsigned short int)(x) & 0xff00) >> 8))) \

#define htonl(x) ntohl(x)
#define htons(x) ntohs(x)

#else // ! WIN32

#include <netinet/in.h>

#ifdef LINUX
#include <sys/types.h>
#endif // LINUX

#endif // WIN32

#ifndef WIN32
#include <ctype.h>
#endif

#if (!defined(_C_PORT_MACROS_))
#define _C_PORT_MACROS_

#define TRUE 1
#define FALSE 0




// Identifiant RCS sans warning pour C et C++
#define RCS_ID(x) \
	static const char *rcs_id = x ; \
	static void _x(const char *) ; \
	static void _y(const char *stub) { _x(stub) ; stub = rcs_id ; } ; \
	static void _x(const char *stub) { _y(stub) ; }

// Prototype "a la C"
#if defined(__cplusplus)
#define EXTERN extern "C"
#else
#define EXTERN extern
#endif

// Identifiant SCCS sans warning
// Sur HP-UX A.09.05 sans gcc, il faut utiliser la version simple !
#if ( (!defined(HPUX)) && (defined(__GNUC__)) )
#define SCCS_ID(x) static char SccsId[]=x ; static char *_sccs_unused=(&_sccs_unused, SccsId)
#else
#define SCCS_ID(x) static char SccsId[] = x
#endif

#undef SCCS_ID
#define SCCS_ID(x)

// Assertions que l'on peut enlever a la compilation
#if (!defined(NO_CHECKS))
#define ASSERT(x) assert(x)
#else
#define ASSERT(x)
#endif

#ifdef IPX
// On implemente streror pour les IPX qui ne l'ont pas
extern char *strerror(int errno) ;
#endif

// Comparaison de chaines sans tenir compte de leur longueur
extern int lowerstrcmp(const char *s1, const char *s2) ;

// Taille max, en caracteres, d'un pointeur
extern const int ADR_SIZE ;


// Acc�s aux fonctions POSIX de manipulation du r�pertoire courant
#ifndef WIN32

#include "unistd.h"

#else // defined(WIN32)

// Sous-syst�me POSIX de Windows
#include <direct.h>

#ifndef PATH_MAX
#define PATH_MAX _MAX_PATH
#endif // PATH_MAX

#define getcwd _getcwd
#define chdir _chdir

typedef int gid_t;

#ifndef __GNUC__
// Visual Studio has _snprintf instead of sprintf
#define snprintf _snprintf
typedef unsigned long ssize_t;
typedef unsigned long mode_t;

#endif // __GNUC__

#endif // defined(WIN32)


#ifdef __BCOMP__

#endif // BCOMP

#endif /* _C_PORT_MACROS_ */

#ifndef WIN32
// Utilisation de strcasecmp qui n'est pas completement POSIX mais est
// presente sous Unix : il faut donner le forward
EXTERN int strcasecmp(const char *s1, const char *s2);

#else // defined(WIN32)

// Pour Windows, il faut utiliser _stricmp().
#define strcasecmp _stricmp

#endif // defined(WIN32)
//
// Verification des options de compilation
//
#ifdef FREE_MEMORY
#error "l'option FREE_MEMORY a disparu depuis la v1.70"
#endif // FREE_MEMORY
#ifdef EVOL_RECORDS
#error "l'option EVOL_RECORDS est de serie depuis la v1.70"
#endif // FREE_MEMORY
#ifdef INCLUDE_DEFINITIONS
#error "l'option INCLUDE_DEFINITIONS est de serie depuis la v1.70"
#endif // FREE_MEMORY
#ifdef MEMORY_CLEANUP
#error "l'option MEMORY_CLEANUP est de serie depuis la v1.70"
#endif // MEMORY_CLEANUP
#ifdef EVOL_INHERIT
#error "l'option EVOL_INHERIT est de serie depuis la v1.70"
#endif // EVOL_INHERIT
#ifdef EVOL_TREES
#error "l'option EVOL_TREES est de serie depuis la v1.70"
#endif // EVOL_TREES
#ifdef RESOURCE
#error "l'option RESOURCE est de serie depuis la v1.70"
#endif // RESOURCE
#ifdef ATB_DIFF
#error "l'option ATB_DIFF est de serie depuis la v1.70"
#endif // ATB_DIFF

/**
 * Crée un fichier temporaire à partir du pattern fourni en paramètre.
 * Si get_temp_path est faux, le répertoire du fichier pattern n'est pas
 * modifié, et le fichier est donc créé soit dans le répertoire courant,
 * soit dans le répertoire donné, si le pattern correspond à un nom de fichier
 * absolu.
 * Si get_temp_path est vrai, pattern ne doit pas contenir un chemin absolu,
 * et le fichier temporaire sera créé dans le répertoire temporaire.
 *
 * Le résultat renvoyé est alloué dynamiquement par malloc, et doit être
 * libéré par un appel à free.
 *
 * @param pattern le pattern à utiliser pour la création du fichier
 * @param get_temp_path indique si le fichier doit être créé dans le répertoire
 *     temporaire système
 * @return le chemin vers le fichier temporaire, ou NULL si aucun nom
 *     de fichier n'a put être créé
 */
EXTERN char *bcomp_mktemp(const char *pattern, int get_temp_path);

//EXTERN const char *bcomp_last_slash(const char *file_name);

EXTERN char *bcomp_last_slash(char *file_name);

#endif /* _C_PORT_H_ */
