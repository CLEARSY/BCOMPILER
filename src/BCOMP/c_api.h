/******************************* CLEARSY **************************************
* Fichier : $Id: c_api.h,v 2.0 2007/01/08 11:06:15 atelierb Exp $
* (C) 2008-2025 CLEARSY
*
* Compilations :	-D__BCOMP__ lors de la generation du compilateur
*					(les clients NE DOIVENT PAS positionner ce drapeau)
*
* Description :		Compilateur B
*					Interface de la bibliotheque du compilateur B
*
This file is part of B_COMPILER
    Copyright (C) 2008-2025 CLEARSY (contact@clearsy.com)

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
#ifndef _C_API_H_
#define _C_API_H_
#include <c_port.h>

/* Informations sur les outils : prototypes mixte C/C++ */
EXTERN void betree_info(void);
EXTERN void decompiler_info(void);

#if defined(__cplusplus)

//
// Interface C++ du Betree
//


#ifndef __BCOMP__


// Flag B0C automatique pour synchroniser les clients avec le compilo
#ifndef B0C
#define B0C
#endif // !B0C

// Inclusion automatique des interfaces des classes exportees pour les clients

//
//	}{	Classes
//
#include <c_trace.h>
#include <c_io.h>
#include <c_memory.h>
#include <c_cleanup.h>
#include <c_nodtyp.h>
#include <c_llktyp.h>
#include <v_vistbl.h>
#include <c_object.h>
#include <c_item.h>
#include <c_lextyp.h>
#include <c_mach.h>
#include <c_defini.h>
#include <c_betree.h>
#include <c_symb.h>
#include <c_lextyp.h>
#include <c_lex.h>
#include <c_pralex.h>
#include <c_exptyp.h>
#include <c_expr.h>
#include <c_pretyp.h>
#include <c_pred.h>
#include <c_msg.h>
#include <c_out.h>
#include <c_commen.h>
#include <c_subtyp.h>
#include <c_subst.h>
#include <c_relati.h>
#include <c_bltin.h>
#include <c_ident.h>
#include <c_lstlnk.h>
#include <c_genop.h>
#include <c_op.h>
#include <c_flag.h>
#include <c_umach.h>
#include <c_base.h>
#include <c_valu.h>
#include <c_usedef.h>
#include <c_usecre.h>
#include <c_char.h>
#include <c_chain.h>
#include <c_cata.h>
#include <c_meta.h>
#include <c_uop.h>
#include <c_bmana.h>
#include <c_blist.h>
#include <c_objman.h>
#include <c_type.h>
#include <c_bound.h>
#include <v_toplvl.h>
#include <c_pragma.h>
#include <c_projdb.h>
#include <c_b0type.h>
#include <c_env.h>
#include <c_tstamp.h>
#include <c_record.h>
#include <c_path.h>
#include <f_file.h>
#include <m_api.h>
#include <m_compo.h>
#include <m_file.h>
#include <m_msg.h>
#include <m_pmana.h>
#include <m_projec.h>
#include <m_state.h>

#else	// __BCOMP__

#include "c_trace.h"
#include "c_io.h"
#include "c_memory.h"
#include "c_cleanup.h"
#include "c_nodtyp.h"
#include "c_llktyp.h"
#include "v_vistbl.h"
#include "c_object.h"
#include "c_item.h"
#include "c_lextyp.h"
#include "c_mach.h"
#include "c_defini.h"
#include "c_betree.h"
#include "c_symb.h"
#include "c_lextyp.h"
#include "c_lex.h"
#include "c_pralex.h"
#include "c_exptyp.h"
#include "c_expr.h"
#include "c_pretyp.h"
#include "c_pred.h"
#include "c_msg.h"
#include "c_out.h"
#include "c_commen.h"
#include "c_subtyp.h"
#include "c_subst.h"
#include "c_relati.h"
#include "c_bltin.h"
#include "v_vistbl.h"
#include "c_ident.h"
#include "c_lstlnk.h"
#include "c_genop.h"
#include "c_op.h"
#include "c_flag.h"
#include "c_umach.h"
#include "c_base.h"
#include "c_valu.h"
#include "c_usedef.h"
#include "c_usecre.h"
#include "c_char.h"
#include "c_chain.h"
#include "c_cata.h"
#include "c_meta.h"
#include "c_uop.h"
#include "c_bmana.h"
#include "c_blist.h"
#include "c_objman.h"
#include "c_type.h"
#include "v_toplvl.h"
#include "c_bound.h"
#include "c_path.h"
#include "c_pragma.h"
#include "c_projdb.h"
#include "c_b0type.h"
#include "c_env.h"
#include "c_tstamp.h"
#include "c_record.h"
#include "f_file.h"
#include "m_api.h"
#include "m_compo.h"
#include "m_file.h"
#include "m_lexsta.h"
#include "m_msg.h"
#include "m_pmana.h"
#include "m_projec.h"
#include "m_state.h"
#include "t_ident.h"
#include "t_locop.h"

#endif /* ! __BCOMP__ */


//
//	}{	Fonctions
//

// Initialisation du compilateur
extern void init_compiler(void) ;

extern void set_localedir(char *l);

// Forcer un reset du compilateur
extern void reset_compiler(void) ;

// Liberation du compilateur
extern void unlink_compiler(void) ;

// si les fichiers source et definitions associ��s sont inchang��s alors le fichier expand est inchang��
extern int is_expand_dep_needed (const char *input_file);

// Fonction qui cree le Betree minimal permettant l'analyse des
// dependances
extern T_betree *compiler_dep_analysis(const char *input_file,
                                                const char *converterName,
                                                int complain_if_unrdble = TRUE) ;

// Fonction qui lit un fichier source, fait l'analyse syntaxique et
// rend un Betree
// second_input_file = 2e essai pour trouver le nom du composant
// (ex: REFINES MM, input_file = MM.mch, second_input_file = MM.ref)
// load_lexem = lexeme qui a demande le chargement le cas echeant
// (ex: bb  dans SEES bb)
extern T_betree *
    compiler_syntax_analysis(const char *component_name,
                         const char *input_file,
                                 const char* converterName,
							 const char *second_input_file = NULL,
							 const char *third_input_file = NULL,
							 T_lexem *load_lexem = NULL) ;

// Fonction qui fait l'analyse semantique d'un Betree syntaxique
// Le Betree enrichi est retourne en cas de succes. En cas d'erreur,
// la fonction renvoie NULL
extern T_betree *compiler_semantic_analysis(T_betree *syntaxic_betree,
                                                     const char* converterName) ;

// Fonction pour la compatibilit� ascendante. Elle appelle la
// fonction compiler_B0_check suivante avec comme valeur pour le
// param�tre force: FALSE
extern T_betree *compiler_B0_check(T_betree *semantic_betree) ;

// Fonction qui effectue les verifications B0 d'un Betree et qui
// calcule les types B0. Le param�tres must_be_concrete permet de
// pr�ciser que le module dont le betree est la racine est concret
extern T_betree *compiler_B0_check(T_betree *semantic_betree,
											int must_be_concrete) ;

//// Fonction qui sauvegarde un betree
//extern void compiler_save_betree(T_betree *betree,
//										  const char *output_file) ;

//// Fonction qui charge un betree a partir d'un nom de fichier
//extern T_betree *compiler_load_betree(T_betree_file_type file_type,
//											   const char *input_file) ;

//// Fonction qui charge un betree a partir d'un T_used_machine
//extern T_betree *compiler_load_betree(T_betree_file_type file_type,
//											   T_used_machine *reference) ;

//// Fonction qui charge un betree a partir d'un T_symbol
//extern T_betree *compiler_load_betree(T_betree_file_type file_type,
//											   T_symbol *reference) ;

// Fonction qui dumpe un betree sous format HTML
extern void compiler_dump_HTML_betree(T_betree *betree) ;

// Dump des tables au format LaTeX 2e
extern void latex2e_dump_LR_tables(const char *file_name,
											const char *output_path) ;

// Dump des tables au format HTML
extern void html_dump_LR_tables(const char *file_name,
										 const char *output_path) ;

// Version de la bibliotheque
extern const char *get_bcomp_version(void) ;

// Supprimer les messages d'avertissement
extern void compiler_disable_warnings(void) ;

// Afficher tous les messages d'avertissement
extern void compiler_enable_extended_warnings(void) ;

#ifdef __BCOMP__
//
//	Methodes internes au compilateur
//

// Le compilateur est-il initialise ?
extern int get_init_compiler_done(void) ;
#endif // __BCOMP__

// Informations
EXTERN void betree_info(void) ;

// gestionnaire de sortie
#include <setjmp.h>
extern void set_exit_handler(jmp_buf *adr_env) ;
extern jmp_buf *get_exit_handler(void) ;

// Mise en place ou sortie du mode "pas d'exit"
extern void set_no_exit_mode(int new_no_exit_mode) ;
extern int get_no_exit_mode(void) ;

// Mode ou les lexemes correspondant aux donnees incluses sont
// "corriges", i.e. si A include B alors les donnees heritees de B
// auront des lexemes correspondant a la clause INCLUDES ou EXTENDS de
// A qui a provoque l'heritage
// Valeur par defaut : FALSE
extern int get_fix_inherited_lexems_mode(void) ;
extern void set_fix_inherited_lexems_mode(int new_mode) ;


// Fonction qui met a jour l'identificateur new_ident, herite
// depuis src_ident, et "a cause" de la clause au lexeme
// inherit_lexem, pour que le Betree soit exploitable par des outils
// comme le Xref : corrige les champs ref_lexem et ref_inherited
extern void fix_inherited(T_ident *new_ident,
								   T_ident *src_ident,
								   T_lexem *inherit_lexem) ;

// Compilation autonome du compilateur i.e. sans decompilateur
// On fait un stub pour les fonctions du decompilateur
// Initialisation du decompilateur
extern void init_decompiler(void) ;
extern void reset_decompiler(void) ;

// Informations (prototype C)
EXTERN void decompiler_info(void) ;

  // Chemins de production
extern void set_decompiler_output_path(const char *new_output_path) ;

// Decompilation d'une branche de Betree
extern char *decompile_branch(T_item *root);

// Decompilation d'un Betree avec ou sans expand
extern T_betree *decompile_betree(T_betree *betree,
										   int expand,
										   int paren_ident) ;

// Gestion du mode "compatibilite anciens outils" : lecture/ecriture
// Le mode comprend : parenthesage des identificateurs renommes +
// remplacer la sequence vide "[]" en "<>"
extern void set_compatibility_mode(int new_compatibility_mode) ;


// Creation d'un raffinement d'un Betree (qui doit etre une spec ou
// une machine ...). Si create_imp vaut TRUE, on cree une
// implementation, sinon c'est un raffinement que l'on cree
// new_name contient le nom du betree a creer
// Si do_semantic vaut TRUE, l'analyse semantique du betree
// est effectuee si necessaire, et des informations semantiques
// sont ajoutees (operations promues)
// svi clauses to be copied : SEES, VARIABLES, INITIALISATION\n");
// using 0 disregards the clause.
extern T_betree *create_betree_refinement(T_betree *abstraction,
                                                   const char* converterName,
												   const char *new_name,
												   int create_imp,
												   int do_semantic/* = FALSE*/,
												   char *svi,
												   int nb_operations,
												   char **operations) ;

#endif // __cplusplus

#endif /* _C_API_H_ */
