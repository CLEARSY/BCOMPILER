/******************************* CLEARSY **************************************
* Fichier : $Id: c_out.h,v 2.0 2002-07-12 13:05:45 cm Exp $
* (C) 2008 CLEARSY
*
* Description :		Compilateur B
*					Interface du gestionnaire de messages
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
#ifndef _C_OUT_H_
#define _C_OUT_H_

// Mode de fonctionnement du gestionnaire : destination des messages
typedef enum
{
  MSG_DEST_STANDARD_STREAMS, // affichage dans stdout/stderr
  MSG_DEST_BUFFERS, // production des messages dans des buffers
  MSG_DEST_STANDARD_STREAMS_AND_BUFFERS // stdout/stderr + buffers
} T_msg_destination ;

// Type de flux
typedef enum
{
  MSG_ERROR_STREAM,		// flux "erreur" i.e. stderr
  MSG_NOMINAL_STREAM	// flux "nominal" i.e. stdout
} T_msg_stream ;

enum class T_msg_level {
  ERROR,
  WARNING,
	INFO,
};

// Modelisation d'une ligne du flux de messages
class T_msg_line : public T_item
{
	// warning or error
  T_msg_level msg_level;

  // Position
  T_symbol			*file_name ;
  int				file_line ;
  int 				file_column ;

  // Contenu
  T_symbol			*contents ;

public :
  T_msg_line(void) : T_item(NODE_MSG_LINE) {} ;
  T_msg_line(T_msg_level level,
			 T_item **adr_first,
			 T_item **adr_last,
			 T_item *father) ;
  ~T_msg_line() ;

  // Methode de dump HTML
  virtual FILE *dump_html(void) ;

  // Methodes de lecture
  virtual const char *get_class_name(void) { return "T_msg_line" ; }  ;
  T_msg_level get_level(void) { return msg_level ; } ;
  T_symbol *get_file_name(void) { return file_name ; } ;
  int get_file_line(void) { return file_line ; } ;
  int get_file_column(void) { return file_column ; } ;
  T_symbol *get_contents(void) { return contents ; } ;

  // Methodes d'ecriture
  void set_position(T_symbol *new_file_name,
							 int new_file_line,
							 int new_file_column)
	{
	  file_name = new_file_name ;
	  file_line = new_file_line ;
	  file_column = new_file_column ;
	} ;

  void set_contents(T_symbol *new_contents) { contents = new_contents ; } ;
  // Fonctions utilisateur
  virtual int user_defined_f1(void) ;
  virtual T_object *user_defined_f2(void) ;
  virtual int user_defined_f3(int arg) ;
  virtual T_object *user_defined_f4(T_object *arg) ;
  virtual int user_defined_f5(T_object *arg) ;
} ;

// Gestionnaire de lignes
class T_msg_line_manager : public T_item
{
  // Liste de messages
  T_msg_line		*first_msg ;
  T_msg_line		*last_msg ;

public :
  T_msg_line_manager(void) ;
  ~T_msg_line_manager(void) ;

  // Methode de dump HTML
  virtual FILE *dump_html(void) ;

  // Methodes de lecture
  virtual const char *get_class_name(void)
	{ return "T_msg_line_manager" ; }  ;
  T_msg_line *get_messages(void) { return first_msg ; } ;

  // Creation d'une ligne
  T_msg_line *create_line(T_msg_level level) ;

  // Liberation de toutes les lignes
  void unlink_lines(void) ;

  // Fonctions utilisateur
  virtual int user_defined_f1(void) ;
  virtual T_object *user_defined_f2(void) ;
  virtual int user_defined_f3(int arg) ;
  virtual T_object *user_defined_f4(T_object *arg) ;
  virtual int user_defined_f5(T_object *arg) ;
} ;

// Acces au gestionnaire de lignes (avec creation le cas echeant)
extern T_msg_line_manager *get_msg_line_manager(void) ;

// Reset du gestionnaire de lignes
extern void reset_msg_line_manager(void) ;

// Choix de la destination des messages
// Par defaut c'est MSG_DEST_STANDARD_STREAMS
extern void set_msg_destination(T_msg_destination new_msg_destination) ;

// Buffer du gestionnaire
// Taille : 64Ko
extern char *get_msg_buffer(void) ;

// Affichage d'un positionnement
// version fichier/ligne/colonne
extern void locate_msg(T_msg_level level,
								const char *file_name,
								int file_line,
								int file_column) ;

// version at_toplevel
extern void locate_msg(T_msg_level level) ;

// Prise en compte du message dans le buffer (affichage immediat et/ou
// ajout dans les buffers)
extern void deliver_msg(T_msg_level level) ;


//
// }{ Interface de production des messages
//
// Erreur de syntaxe
extern void syntax_error(T_lexem *lexem,
								  T_error_level error_level,
								  const char *format,
								  ...) ;

// Arret immediat
extern void stop(void) ;

// Erreur au niveau du compilateur
extern void toplevel_error(T_error_level error_level,
									const char *format,
									...) ;

// Warning au niveau du compilateur
extern void toplevel_warning(T_warning_level error_level,
									  const char *format,
									  ...) ;

// Erreur de parsing (caractere illegal)
extern void parse_error(const char *file,
								 int line,
								 int column,
								 T_error_level error_level,
								 const char *format,
								 ...) ;

class T_pragma_lexem ;
extern void syntax_error(T_pragma_lexem *lexem,
								  T_error_level error_level,
								  const char *format,
								  ...) ;
// Erreur de semantique
extern void semantic_error(T_item *item,
									T_error_level error_level,
									const char *format,
									...) ;

extern void semantic_error(T_lexem *lexem,
									T_error_level error_level,
									const char *format,
									...) ;

// Erreur de semantique : details
extern void semantic_error_details(T_item *item,
											const char *format,
											...) ;

extern void semantic_error_details(T_lexem *lexem,
											const char *format,
											...) ;

// Avertissement de syntaxe
extern void syntax_warning(T_lexem *lexem,
									T_warning_level warning_level,
									const char *format,
									...) ;

// Avertissement semantique
extern void semantic_warning(T_item *item,
									  T_warning_level warning_level,
									  const char *format,
									  ...) ;


// Erreur du type : lexeme non attendu
extern void syntax_unexpected_error(T_lexem *cur_lexem,
											 T_error_level error_level,
											 const char *expected_value) ;

// Erreur du type : item non attendu
// Si force_invalid_name != NULL on force le nom de l'item invalide
extern void syntax_unexpected_error(T_item *cur_item,
											 T_error_level error_level,
											 const char *expected_value) ;

// Erreur de type : ident attendu, got ...
extern void syntax_ident_expected_error(T_lexem *cur_lexem,
										T_error_level error_level) ;

// Erreur de type : ident attendu, got ...
extern void syntax_ident_expected_error(T_item *cur_item,
										T_error_level error_level) ;

// Erreur de type : expression attendue, got ...
extern void syntax_expr_expected_error(T_item *cur_item,
									   T_error_level error_level) ;

// Erreur de type : predicat attendu, got ...
extern void syntax_predicate_expected_error(T_item *cur_item,
											T_error_level error_level) ;

// Erreur de type : substitution attendue, got ...
extern void syntax_substitution_expected_error(T_item *cur_item,
											   T_error_level error_level) ;

// Erreur du type : lexeme redefinition
extern void syntax_already_error(T_lexem *cur_lexem,
										  T_error_level error_level,
										  const char *redefined_item) ;


// Erreur du type : lexeme redefinition -- info de positionnement
extern void already_location(T_lexem *cur_lexem) ;

// Indication de la localisation d'une clause en cas d'erreur
extern void clause_location_error(T_item *cur_item,
										   const char *clause_name) ;

// Erreur interne du compilateur
extern void internal_error(T_lexem *cur_lexem,
						   const char *file,
						   int line,
						   const char *format,
						   ...) ;


// Warning au niveau lexical
extern void lex_warning(const char *file,
								 int line,
								 int column,
								 T_warning_level warning_level,
								 const char *format,
								 ...) ;

// Warning de type END non aligne
extern void end_warning(const char *name,
								 T_lexem *start,
								 T_lexem *end) ;

// Production d'un message de sortie
extern void print(const char *format, ...) ;

// Obtention du nombre d'erreurs deja rencontrees
extern int get_error_count(void) ;
extern int get_warning_count(void) ;

// Reset du compteur d'erreurs
extern void reset_error_count(void) ;
extern void reset_warning_count(void) ;

// Obtention du nom d'une classe
extern const char *make_class_name(T_object *object) ;
extern const char *make_class_name(T_node_type node_type) ;

// Erreur de type "clause 1 implique clause 2"
extern void cl1_implies_cl2_error(T_flag *cl1, T_lex_type cl2) ;

// Erreur de type : double definition d'identificateur
extern void double_identifier_error(T_ident *new_def,
											 T_ident *old_def) ;

// Passage en mode verbeux
extern void set_verbose_mode(void) ;

// Description verbeuse
// (imprimee seulement en mode verbeux)
extern void verbose_describe(const char *format, ...) ;

// Fonction qui regarde s'il y a eu des erreurs
// Si c'est le cas, elle renvoie FALSE. Sinon elle renvoie TRUE
int check_errors(void) ;

// Production d'une erreur utilisateur
extern void user_error(T_item *item,
								T_error_level error_level,
								const char *format,
								...) ;
extern void user_error(T_error_level error_level,
								const char *format,
								...) ;

// Production d'un avertissement utilisateur
extern void user_warning(T_warning_level warning_level,
								  const char *format,
								  ...) ;
extern void user_warning(T_item *item,
								  T_warning_level warning_level,
								  const char *format,
								  ...) ;

extern void B0_syntax_error(T_item *item,
									 T_error_level error_level,
									 const char *format,
									 ...) ;

extern void B0_semantic_error(T_item *item,
									   T_error_level error_level,
									   const char *format,
									   ...) ;

extern void B0_semantic_error(T_lexem *lexem,
									   T_error_level error_level,
									   const char *format,
									   ...) ;

extern void B0_semantic_warning(T_item *item,
										 T_warning_level warning_level,
										 const char *format,
										 ...) ;
// Gestion du nom d'outil
// Par defaut : bcomp
extern void set_tool_name(const char *tool_name) ;
extern T_symbol *get_tool_name(void) ;

#endif // _C_OUT_H_
