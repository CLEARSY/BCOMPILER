/******************************* STERIA **************************************
* Fichier : $Id: stub.cpp,v 1.8 2000/05/02 13:24:43 lv Exp $
* (C) 1998 STERIA
*
* Description :		Stub decompilateur B
*					Utilise pour compiler les outils en standalon
*
* Auteur(s) :		SL
*
* Historique : $Log: stub.cpp,v $
* Historique : Revision 1.8  2000/05/02 13:24:43  lv
* Historique : Ajout valeur de retour "NULL" pour decompile_betree().
* Historique :
* Historique : Revision 1.7  1999/09/24 09:12:57  sl
* Historique : Stubs pour gnf
* Historique :
* Historique : Revision 1.6  1999/06/09 12:46:58  sl
* Historique : Menage options de compilation
* Historique :
* Historique : Revision 1.5  1999/01/20 17:53:28  sl
* Historique : MEMORY_CLEANUP pour decompilateur
* Historique :
* Historique : Revision 1.4  1998/10/09 07:09:34  sl
* Historique : Ajout decompiler_info avec prototype C
* Historique :
* Historique : Revision 1.3  1998/10/08 17:12:50  atelierb
* Historique : Ajout de la fonction decompiler_info
* Historique :
* Historique : Revision 1.2  1998/02/19 09:59:26  sl
* Historique : Ajout "set_compatibility_mode"
* Historique :
* Historique : Revision 1.1.1.1  1998/01/29 12:42:16  sl
* Historique : Version initiale
* Historique :
*
******************************************************************************/
#include <c_port.h>
RCS_ID("$Id: stub.cpp,v 1.8 2000/05/02 13:24:43 lv Exp $") ;

/* Includes systeme */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

/* Includes Composant Local */
#include <c_api.h>

// Compilation autonome du compilateur i.e. sans decompilateur
// On fait un stub pour les fonctions du decompilateur
// Version du decompilateur
EXTERN void decompiler_info(void)
{
  TRACE0("STUB -- decompiler_info") ;
}

// Initialisation du decompilateur
void init_decompiler(void)
{
  TRACE0("STUB -- init_decompiler") ;
}

void reset_decompiler(void)
{
  TRACE0("STUB -- reset_decompiler") ;
}

// Chemins de production
void set_decompiler_output_path(const char *new_output_path)
{
  TRACE1("STUB -- set_decompiler_output_path(%s)", new_output_path) ;
}

// Decompilation d'un Betree avec ou sans expand
extern T_betree *decompile_betree(T_betree *betree,
										   int expand,
										   int paren_ident)
{
  TRACE3("STUB -- decompile_betree(%x, %d, %d)", betree, expand, paren_ident) ;
  assert(FALSE) ;
  return NULL ;
}


// Decompilation d'une branche de Betree
extern char *decompile_branch(T_item *root)
{
  TRACE1("STUB -- decompile_branch(%x)", root) ;
  assert(FALSE) ;
  return NULL ;
}

// Gestion du mode "compatibilite anciens outils" : lecture/ecriture
// Le mode comprend : parenthesage des identificateurs renommes +
// remplacer la sequence vide "[]" en "<>"
void set_compatibility_mode(int new_compatibility_mode)
{
	TRACE0("STUB -- set compatibilty mode") ;
	assert(FALSE) ;
}

// Stubs pour GNF
void init_norm(void) 
{
  assert(FALSE) ;
}
void generate_normal_form(T_betree *semantic_betree,
								   const char *output_file) 
{
  assert(FALSE) ;
}
void set_norm_strict_mode(int new_norm_strict_mode)
{
  assert(FALSE) ;
}

//
// Fin du stub
// 
