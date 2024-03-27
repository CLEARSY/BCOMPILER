/******************************* CLEARSY **************************************
* Fichier : $Id: f_file.h,v 2.0 2001-07-04 14:55:47 fc Exp $
* (C) 2008 CLEARSY
*
* Description :		Interface pour ecriture dans un fichier
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
#ifndef _F_FILE_H_
#define _F_FILE_H_

// Ajout de l'entete standard
EXTERN void add_file_header(T_betree *betree,
									 const char *suffix,
									 const char *tool_name) ;

// Mise a jour de l'indentation et du file descriptor
EXTERN void file_reset_indent (FILE *fd) ;

// Renvoie TRUE si file est ouvert sur un descripteur
EXTERN int file_is_open(void) ;

// Etablissement du mode sans separation des lignes
EXTERN void file_set_no_split (void) ;

// Passage en mode  'split avance' prenant en compte les '#' du B-Linker
extern void file_set_advanced_split_mode(int new_advanced_split_mode) ;

// Utilisation d'un caractere de separation des lignes
EXTERN void file_enable_line_separator (char new_sep) ;

// Suprresion d'un caractere de separation des lignes
EXTERN void file_disable_line_separator(void) ;

// new_align_param vaut TRUE si on souhaite interdire le saut de ligne
// avant le premier appel parametre d'un appel d'operation quand
// celui-ci est trop long, sinon il vaut FALSE
EXTERN void align_first_param (int new_align_param) ;

// Utilisation d'espaces au lieu de tabulations
EXTERN void file_use_space();

// Ajout d'un niveau d'indentation
EXTERN void file_push_indent (void) ;
EXTERN void file_old_push_indent (void) ;

#if defined(__cplusplus)
extern void file_push_indent (const char *msg) ;
#endif

// indique a la bibliotheque de ne pas sortir en erreur si les lignes
// sont trop longues
EXTERN void file_use_warning(void) ;

// Retrait d'un niveau d'indentation
EXTERN void file_pop_indent (void) ;
EXTERN void file_old_pop_indent (void) ;
#if defined(__cplusplus)
extern void file_pop_indent (const char *msg) ;
#endif

// Passage a la ligne
EXTERN void file_new_line (void) ;
EXTERN void file_old_new_line (void);

// Sauter une ligne quoi qu'il arrive
EXTERN void file_skip_line(void) ;

// Ecriture dans le fichier
EXTERN void file_fprintf (const char *format_acp, ...) ;

// Ecriture dans le fichier avec retour chariot force
EXTERN void file_fprintf_line (const char *format_acp, ...) ;

// Ecriture dans le fichier sans controle de taille de ligne
EXTERN void file_fprintf_raw (const char *format_acp, ...) ;

// Ecriture d'une chaine en mode raw2 (pas de controle de taille, pas d'argument)
EXTERN void file_fprintf_raw2 (const char *string) ;

// Fonction auxiliaire qui ecrit instance.classe ou classe si instance == NULL
EXTERN void file_dump_instance (const char *instance_name,
										 const char *class_name) ;

// Changement de la configuration
EXTERN void file_set_configuration (int new_tab_size,
											 int new_max_line_size,
											 int use_space) ;

// Faut-il crier si une ligne est insécable, ou continuer ?
// Par defaut, on crie (FALSE)
extern void file_set_accept_unsplittable_lines(int new_do_warn) ;
extern int file_get_accept_unsplittable_lines(void) ;

// On emmet simplement un Warning en cas de depassement
// de la taille max d'une ligne
EXTERN void file_use_warning(void) ;


#endif // _F_FILE_H_

