/******************************* CLEARSY **************************************
* Fichier : $Id: d_dskobj.h,v 2.0 1999-04-14 11:41:32 cc Exp $
* (C) 2008 CLEARSY
*
* Description :		Compilateur B
*					Interface de la classe de base T_disk_object
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
#ifndef _C_DISK_OBJECT_H_
#define _C_DISK_OBJECT_H_

class T_disk_object
	{
	// Addresse RAM de l'objet
	void			*address ;

	// Index alloue a l'objet
	size_t	index ;

	// Offset dans le buffer de sauvegarde
	size_t	offset ;

	// Chainage
	T_disk_object	*next ;

	// Resolu ? (utilise en lecture)
	int				solved ;

	// Ordre d'ecriture (utilise en ecriture)
	size_t   	rank ;

public :
#ifndef OPTIM_PERSIST
	  // Ajout d'un objet dans la table
	  T_disk_object(T_object *object) ;
#else // OPTIM_PERSIST
	  // Ajout d'un objet dans la table apres after_this_object
	  // si after_this_object == NULL, insertion en tete
	T_disk_object(T_object *object, T_disk_object *after_this_object) ;
#endif // !OPTIM_PERSIST

	// Ajout d'un couple (index->offset) dans la table
	T_disk_object(size_t new_index,
				 size_t new_offset,
				 size_t new_rank) ;

	// Methodes de lecture
	void *get_address(void) { return address ; } ;
	size_t get_index(void) { return index ; } ;
	size_t get_offset(void) { return offset ; } ;
	T_disk_object *get_next(void) { return next ; } ;
	int get_solved(void) { return solved ; } ;
	size_t get_rank(void) { return rank ; } ;

	// Methodes d'ecriture
	void set_offset(size_t new_offset) { offset = new_offset ; } ;
	void set_address(void *new_address) { address = new_address ; } ;
	void set_solved(int new_solved) { solved = new_solved ; } ;
	void set_rank(void) ;

	// Autres methodes
	void dump_disk(void) ;
	} ;

// Init pour ecriture
extern T_disk_manager *init_disk_object_write(T_betree *new_betree,
													   const char *file_name,
													   T_betree_file_type ftype);

extern void init_disk_object_read(void *new_read_start) ;

extern void exit_disk_object(void) ;
extern size_t dump_disk_object(void) ;
extern size_t load_disk_object(void *start, size_t len) ;
#ifdef PERSIST_TRACE
#define disk_object_find_address(x) _disk_object_find_address(__FILE__, __LINE__, x) ;
extern void *_disk_object_find_address(const char *file,
									  int line,
									  size_t index) ;
#else // !(PERSIST_TRACE)
extern void *disk_object_find_address(size_t index) ;
#endif // PERSIST_TRACE

// Obtention de l'index associe a un objet
// Si l'objet est deja dans la table, l'index associe est retourne
// Sinon, on cree une entree dans la table et on insere l'adresse
// dans la liste des adresses a sauvegarder
extern size_t disk_object_get_object_index(T_object *adr) ;

// Mise a jour de l'adresse pour un index
extern T_disk_object *disk_object_set_address(size_t rank,
													 T_object *new_address) ;

// Gestion de la table d'indirection rank->object
extern void init_rank_to_object_table(size_t max_rank) ;
extern void unlink_rank_to_object_table(void) ;
#endif /* _C_DISK_OBJECT_H_ */
