/******************************* CLEARSY **************************************
* Fichier : $Id: c_lex.h,v 2.0 2007-07-25 14:07:45 arequet Exp $
* (C) 2008 CLEARSY
*
* Description :		Compilateur B
*					Interface de l'analyseur lexical
*					Definition des lexemes
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
#ifndef _C_LEX_H_
#define _C_LEX_H_

#include "c_lextyp.h"
#include "c_object.h"
#include "c_item.h"
#include "c_mach.h"
#include "c_betree.h"
#include "c_defini.h"
#ifdef __BCOMP__
#include "c_md5.h"
#include "c_lexstk.h"
#endif

#include "c_objman.h"

// Fonction qui saute les commentaires a partir de cur_lexem et
// renvoie le premier lexeme non commentaire
// Si cur_lexem == NULL, renvoie NULL
class T_lexem ;
class T_item ;
class T_betree ;

// Obtention du nom de lexeme a partir d'un lex_type
// exemple : get_lex_type_name(L_IDENT) -> "L_IDENT"
const char *get_lex_type_name(T_lex_type lex_type) ;

// Obtention de l'origine ASCII d'un lexeme a partir d'un lex_type
// exemple : get_lex_type_ascii(L_OPERATIONS) -> "OPERATIONS"
const char *get_lex_type_ascii(T_lex_type lex_type) ;

// Obtention de la longueur de l'origine ASCII d'un lexeme a partir d'un lex_type
// exemple : get_lex_type_name(L_OPERATIONS) -> "OPERATIONS" -> 10
// VALABLE UNIQUEMENT POUR LES MOTS CLES
size_t get_lex_type_ascii_len(T_lex_type lex_type) ;

// Definition d'un lexeme
#ifdef TRACE_LEX
#define get_next_lexem() _get_next_lexem(__FILE__, __LINE__)
#endif

class T_lexem : public T_object
{
  friend void watch_last_lexem(void) ;
  friend T_betree *internal_syntax_analysis(int dep_mode,
													 const char *input_file,
                                                                                                         const char *converterName,
                                                     const char *s_i_file,
                                                     const char *t_i_file,
                                                     T_lexem *load_lexem,
                                                     int complain_if_unrdble) ;
#ifdef __BCOMP__
  friend int lex_analysis(const char *file_name_acp,
                                   const char *converterName,
                                   const char *second_file_name_acp,
                                   const char *third_file_name_acp,
                                   T_lexem *load_lexem,
                                   MD5Context *adr_ctx,
                                   int complain_if_unreadable) ;
  friend int load_file(const char *file_name_acp,
								const char *second_file_name_acp,
								const char *third_file_name_acp,
								T_lexem *load_lexem,
								MD5Context *adr_ctx,
                                int complain_if_unreadable) ;
  friend FILE *open_source_file(const char *file_name,
										 size_t &path_len,
										 size_t &file_size) ;
  friend void analyse_word(void) ;
  friend T_betree *compiler_dep_analysis(const char *input_file,
                                                  const char *converterName,
                                                  int complain_if_unreadable) ;
  friend void lex_unload(void) ;
#ifdef TRACE
  friend T_lexem *_lex_get_last_lexem(const char *file, int line) ;
#else
  friend T_lexem *lex_get_last_lexem(void) ;
#endif
  friend void compute_lines(T_lexem *first_lexem) ;
  friend void *lex_push_context(void) ;
  friend void lex_pop_context(void *ctx) ;
  friend void lex_unlink_current_context(void) ;
  friend void parse_DEFINITIONS(T_lexem *first_lexem,
                                         const char* converterName,
										 T_definition **adr_first_def,
										 T_definition **adr_last_def,
										 T_flag **adr_def_clause,
										 T_file_definition **,
										 T_file_definition **,
										 T_lexem *) ;
  friend class T_definition;
  //friend T_definition *analyse_single_DEFINITION(T_lexem **adr_cur_lexem,
		//												  T_definition **,
		//												  T_definition **,
		//												  T_file_definition **,
		//												  T_file_definition **,
		//												  T_lexem *) ;
  friend void move_DEFINITIONS_lexems(T_lexem *cur, T_lexem *def_clause) ;
  friend void include_file_definitions(const char *file_name,
												T_lexem **adr_cur_lexem,
												int local_search_only,
												T_file_definition **,
												T_file_definition **) ;
  friend void parse_DEFINITIONS_inside_clause(T_lexem **adr_cur_lexem,
													   T_definition **,
													   T_definition **,
													   T_flag **,
													   T_file_definition **,
													   T_file_definition **,
													   T_lexem *) ;
  friend void expand_DEFINITIONS_in_definitions(T_definition *first_def,
														 T_definition *last_def) ;
  friend void expand_DEFINITIONS_in_source(T_lexem **adr_first_lexem,
													T_lexem **adr_last_lexem,
													T_definition *first_def,
													T_definition *last_def) ;
  friend void order_DEFINITIONS(T_definition **adr_first_definition,
										 T_definition **adr_last_definition) ;
  friend int expand_ident(T_lexem *,T_lexem **,T_lexem **,T_definition *) ;
  friend void shrink_for_dep(T_lexem *cur_lexem) ;
  friend void fix_label_col(T_lexem *first) ;
  friend void rebuild_chain(T_lexem *,
									 T_lexem *,
									 T_lexem *,
									 T_lexem *,
									 T_lexem **,
									 T_lexem **) ;
  friend T_lexem *syntax_analysis(T_lexem *cur_lexem) ;
  friend T_betree *syntax_analysis(T_lexem *, const char *) ;
  friend T_betree *read_betree(const char *, T_betree_file_type) ;
  friend T_betree::~T_betree(void) ;
  //friend void T_definition::syntax_analysis(T_lexem **adr_ref_lexem,
		//											 T_lexem *last_code_lexem) ;
  friend void reset_lexem_context(void) ;
  friend T_betree *create_betree_refinement(T_betree *abstraction,
													 const char *new_name,
													 int create_imp) ;
#endif // __ BCOMP__
  friend T_lexem *lex_backwards_skip_comments(T_lexem *cur_lexem) ;
  friend T_lexem *lex_skip_comments(T_lexem *cur_lexem) ;

  // Type du lexeme
  T_lex_type				lex_type ;

  // Valeur du lexeme sous forme de chaine de caracteres
  // Contenu valide ssi lex_type vaut L_IDENT, L_COMMENT
  // L_STRING, L_REAL ou L_NUMBER
  const char 	   		*value ;
  size_t  	 			value_len ;

  // Nom de fichier
  T_symbol				*file_name ;

  // Numero de ligne et de colonne du lexeme
  int						file_line ;
  int						file_column ;

  // Lexemes suivants et precedents dans le fichier
  T_lexem					*next_lexem ;
  T_lexem					*prev_lexem ;

  // Lexemes suivants et precedents dans le fichier, une fois les
  // definitions expansees. NULL si aucune definition n'est expansee
  // pour le lexeme suivant ou precedent
  T_lexem					*next_expanded_lexem ;
  T_lexem					*prev_expanded_lexem ;

  // Pour un lexeme expanse, reference :
  // 1) sur la definition (i.e. le nom de macro avant expansion)
  T_lexem					*original_lexem ;
  // 2) sur le bout de regle de reecriture
  T_lexem					*rewrite_rule_lexem ;

  // Lignes suivantes et precedentes
  T_lexem					*prev_line ;
  T_lexem					*next_line ;

  // Debut de la ligne courante
  T_lexem					*start_of_line ;

  // Deja expanse ???
  int						expanded ;

  // Numero d'ordre dans la regle de reecriture en cas d'expansion
  int						position ;

  // Chainage dans l'object manager
  T_lexem 				*next_list_lexem ;

  // Commentaire suivant/precedent non resolu
  // Valable seulement pour les lexemes de type L_COMMENT
  // ou L_KEPT_COMMENT
  T_lexem					*next_unsolved_comment ;
  T_lexem					*prev_unsolved_comment ;

public :
  // Constructeur
  T_lexem() : T_object(NODE_LEXEM) {} ;
  T_lexem(T_lex_type new_lex_type,
		  const char *new_value,
		  size_t new_value_len,
		  int new_file_line,
		  int new_file_column,
		  T_symbol *new_file_name,
		  int auto_chain = TRUE) ; // si TRUE, chainage automatique en fin de flux

  // Constructeur de clonage
  T_lexem(T_lexem *ref_lexem,
		  T_lexem *new_original_lexem,
		  T_lexem **adr_first,
		  T_lexem **adr_last) ;

  // Insertion d'un lexeme AVANT le lexeme courant
  T_lexem(T_lexem *before_lexem, T_lex_type new_lex_type) ;

  // Destructeur
  virtual ~T_lexem() ;

  // Methodes de lecture
  virtual const char *get_class_name(void) { return "T_lexem" ; } ;
  T_lex_type get_lex_type(void) 		{ return lex_type ; } ;
  const char *get_value(void) 	   	{
#ifdef DEBUG_LEX
	TRACE2("T_lexem(%x)::get_value(%s)", this, value) ;
#endif
	return value ;
  } ;
  size_t	get_value_len(void)		{ return value_len ; } ;
  T_symbol *get_file_name(void)    { return file_name ; } ;
  int get_file_line(void)	   		{ return file_line ; } ;
  int	get_file_column(void)	   		{ return file_column ; } ;
  T_lexem *get_original_lexem(void)	{ return original_lexem ; } ;
  T_lexem	*get_prev_line(void) 		{ return prev_line ; }  ;
  T_lexem	*get_next_line(void)	   	{ return next_line ; } ;
  T_lexem	*get_start_of_line(void)	{ return start_of_line ; } ;
  int get_expanded(void) 			{ return expanded ; } ;
  int get_position(void) { return position ; } ;
  T_lexem *get_next_unsolved_comment(void)
	{
	  ASSERT( (lex_type == L_COMMENT) || (lex_type == L_KEPT_COMMENT)
		  || (lex_type == L_CPP_COMMENT) || (lex_type == L_CPP_KEPT_COMMENT)) ;
	  return next_unsolved_comment ;
	} ;
  T_lexem *get_prev_unsolved_comment(void)
	{
	  ASSERT( (lex_type == L_COMMENT) || (lex_type == L_KEPT_COMMENT)
		  || (lex_type == L_CPP_COMMENT) || (lex_type == L_CPP_KEPT_COMMENT)) ;
	  return prev_unsolved_comment ;
	} ;
  T_lexem *get_next_list_lexem(void)	{ return next_list_lexem ; } ;
  // Lexeme suivant/precent, en sautant les commentaires
#ifdef TRACE_LEX
  T_lexem	*_get_next_lexem(const char *file, int line)
#else
	T_lexem	*get_next_lexem(void)
#endif
	{
	  T_lexem *next = (next_expanded_lexem == NULL)
		? next_lexem : next_expanded_lexem ;
	  T_lexem *res = lex_skip_comments(next) ;
#ifdef TRACE_LEX
	  if (res == NULL)
		{
		  TRACE2("get_next_lexem depuis %s:%d -> NULL", file, line) ;
		}
	  else
		{
		  TRACE5("get_next_lexem depuis %s:%d -> %x type %d value %s",
				 file,
				 line,
				 res,
				 res->get_lex_type(),
				 res->get_value()) ;
		}
#endif
	  return res ;
	} ;
  T_lexem	*get_prev_lexem(void)
	{
	  T_lexem *prev = (prev_expanded_lexem == NULL)
		? prev_lexem : prev_expanded_lexem ;
	  return lex_backwards_skip_comments(prev) ;
	} ;

  // Lexeme suivant/precent, sans expanser les definitions
  T_lexem	*get_next_raw_lexem(void)
	{ return lex_skip_comments(next_lexem) ; } ;
  T_lexem	*get_prev_raw_lexem(void)
	{ return lex_skip_comments(prev_lexem) ; } ;

  // Lexeme suivant/precedent, sans rien sauter
  T_lexem *get_real_next_lexem(void) ;
  T_lexem *get_real_prev_lexem(void) ;

  // Deplacer les coordonnees pour etre au meme endroit que ref_lexem
  void move_to_position(T_lexem *ref_lexem)
	{
	  file_column = ref_lexem->file_column ;
	  file_line = ref_lexem->file_line ;
	}
  // Methodes d'ecriture
  void set_lex_type(T_lex_type new_lex_type)
	{ lex_type = new_lex_type ; } ;
  void set_prev_line(T_lexem *new_prev_line)
	{ prev_line = new_prev_line ; } ;
  void set_next_line(T_lexem *new_next_line)
	{ next_line = new_next_line ; } ;
  void set_prev_lexem(T_lexem *new_prev_lexem)
	{ prev_lexem = new_prev_lexem ; } ;
  void set_next_lexem(T_lexem *new_next_lexem)
	{ next_lexem = new_next_lexem  ; } ;
  void set_prev_expanded_lexem(T_lexem *new_prev_expanded_lexem)
	{ prev_expanded_lexem = new_prev_expanded_lexem ; } ;
  void set_next_expanded_lexem(T_lexem *new_next_expanded_lexem)
	{ next_expanded_lexem = new_next_expanded_lexem ; } ;
  void set_start_of_line(T_lexem *new_start_of_line)
	{ start_of_line = new_start_of_line ; } ;
  void set_expanded(int new_expanded) { expanded = new_expanded ; } ;
  void set_position(int new_position) { position = new_position ; } ;
  void set_next_list_lexem(T_lexem *new_next_list_lexem)
	{ next_list_lexem = new_next_list_lexem ; } ;
  void set_next_unsolved_comment(T_lexem *next_unsolved) ;
  void set_prev_unsolved_comment(T_lexem *prev_unsolved) ;
  void set_original_lexem(T_lexem *new_original_lexem)
	{ original_lexem = new_original_lexem ; } ;
  void set_rewrite_rule_lexem(T_lexem *new_rewrite_rule_lexem)
{ rewrite_rule_lexem = new_rewrite_rule_lexem ; } ;

#ifdef COMPAT
  // Methode qui permet de dire qu'un lexeme doit etre ignore par le
  // decompilateur. Si le lexeme provient de l'expansion d'une
  // definition, on propage la correction a la definition elle-meme
  void mark_as_void(void) ;
#endif

  //Ajout CT
  void set_value( const char *new_value ) { value = new_value ; } ;
  void set_value_len( size_t new_value_len )
	{ value_len = new_value_len ; } ;
  void set_file_name( T_symbol *new_file_name )
	{ file_name = new_file_name ; } ;
  void set_file_line( int new_file_line )
	{ file_line = new_file_line ; } ;
  void set_file_column( int new_file_column )
	{ file_column = new_file_column ; } ;

  //
  //	AUTRES METHODES
  //

  // renvoie le nom ou le type
  const char *get_lex_name(void) ;

  // Est-ce un commentaire
  int is_a_comment(void)
	{ return ( (lex_type == L_KEPT_COMMENT) || (lex_type == L_COMMENT) 
	|| (lex_type == L_CPP_COMMENT) || (lex_type == L_CPP_KEPT_COMMENT))
		? TRUE : FALSE ; } ;

	int is_a_kept_comment(void)
	{
		ASSERT(is_a_comment());
		return ((lex_type == L_CPP_KEPT_COMMENT) || (lex_type == L_KEPT_COMMENT))
                        ? TRUE : FALSE;

	}

  // Renvoie TRUE si le lexeme courant est avant ref_lexem dans le fichier
  // Renvoie FALSE sinon
  int is_before(T_lexem *ref_lexem) ;

  // Obtention de l'origine ASCII d'un lexeme a partir d'un lex_type
  // exemple : get_lex_type_name(L_OPERATIONS) -> "OPERATIONS"
  const char *get_lex_ascii()
	{ return (value == NULL) ? get_lex_type_ascii(lex_type) : value ; };

  // Obtention de la longueur de l'origine ASCII d'un lexeme
  // VALIDE SEULEMENT POUR LES MOTS CLES
  size_t get_lex_ascii_len()
	{ return (value == NULL) ? get_lex_type_ascii_len(lex_type) : value_len ; } ;

  // Dump des lexemes
  void dump_lexems(void) ;

  // Methode de dump HTML
  FILE *dump_html(void) ;

  //T_symbol * get_checksum(void);

  // Fonctions utilisateur
  virtual int user_defined_f1(void) ;
  virtual T_object *user_defined_f2(void) ;
  virtual int user_defined_f3(int arg) ;
  virtual T_object *user_defined_f4(T_object *arg) ;
  virtual int user_defined_f5(T_object *arg) ;
} ;

#ifdef __BCOMP__
// Interface de l'analyseur lexical : chargement d'un fichier
// load_lexem = lexeme qui a provoque le chargement
extern int lex_analysis(const char *file_name_acp,
                                 const char *converterName,
                                 const char *second_file_name_acp = NULL,
                                 const char *third_file_name_acp = NULL,
                                 T_lexem *load_lexem = NULL,
                                 MD5Context *adr_ctx = NULL,
                                 int complain_if_unreadable = TRUE)  ;

// Obtention du dernier lexeme non-commentaire
extern T_lexem *lex_get_last_lexem(void) ;
#endif // __BCOMP__

// Pour savoir si un lexeme est un nom de clause
extern int is_a_clause(T_lexem *lexem) ;

// Pour savoir si un lexeme est un nom de clause de debut de composant
int is_a_begin_component_clause(T_lex_type lex_type) ;

// Pour savoir si un lexeme est un nom de clause de dependance
extern int is_a_dep_clause(T_lexem *lexem) ;

//Pour savoir si un lexem est un operateur unaire builtin max,min,conc...
extern int is_a_builtin_lexem(T_lexem *lexem ) ;

#ifdef FULL_TRACE
#define clone(x,y) _clone_(__FILE__,__LINE__,x,y)
#endif

#ifdef FULL_TRACE
extern int _clone_(
#else
				   extern int clone(
#endif
#ifdef FULL_TRACE
									const char *file,
									int line,
#endif
									char **dest,
									const char *src) ;

				   extern const char *get_open_string(void) ;
				   extern const char *get_close_string(void) ;


// Nom du fichier parse
extern void set_file_name(const char *new_file_name) ;
extern const char *get_file_name(void) ;

#ifdef MATRA
//Ajout CT le 23.07.97 : resolution du probleme ancienne chaine/nouvelle chaine
//anomalie matra 001 .
//les expressions max{a,b} sont remplacees par max({a,b}).
//Il en est de mem pour tous les operateurs predefinis max, min, size, conc....
extern void find_builtin_and_add_L_PAREN( T_lexem *from_first_lexem );
#endif // MATRA

// Mode d'analyse lexicale de fichier de definitions
// Lecture et mise a jour de la bascule
extern int get_file_definitions_mode(void) ;
extern void set_file_definitions_mode(int new_file_definitions_mode) ;

// Algo de recherche des fichiers
typedef enum
{
  FFMODE_DEFAULT=0, 	// recherche par defaut : local puis bibliotheques
  FFMODE_LOCAL=1,		// recherche local seulement
  FFMODE_LIBRARY=2,		// recherche en bibliotheque seulement
  FFMODE_MS=3,			// recherche en .db seulement (APRES EXPANSION MULTI-COMPO)
  FFMODE_ORIGINAL_MS=4,	// recherche en .db seulement (AVANT EXPANSION MULTI-COMPO)
  FFMODE_USER=5         // recherche à l'aide d'une classe fournie par l'utilisateur
} T_file_fetch_mode ;

// Mise/Lecteur a jour de l'algo de recherche des fichiers.
// Pour utiliser le mode FFMODE_USER, il faut passer par la fonction
// set user_file_fetch_mode
void set_file_fetch_mode(T_file_fetch_mode new_file_fetch_mode) ;
T_file_fetch_mode get_file_fetch_mode(void) ;

/**
 * Classe permettant de définir un algorithme de recherche personnalisé pour les
 * composants.
 * La fonction à implémenter est la fonction open_component.
 */
class T_file_fetcher
{
public:
    /**
     * Donne le chemin correspondant au composant passé en paramètre.
     * le résultat doit être alloué par new[] (il sera libéré par delete[])
     *
     * @param component_name le nom ou le fichier du composant.
     * @return le nom du fichier à ouvrir
     */
    virtual char *get_file_name(const char *component_name) = 0;
};

/**
 * Définit le mode utilisateur pour file_fetch_mode
 */
void set_user_file_fetch_mode(T_file_fetcher *f);

// Forcer un reset complet des variables de classe de T_lexem au
// prochain appel de reset_lexem_context
extern void force_full_lexems_reset(void) ;

// Pour savoir si on a commence a parser le premier composant du fichier
extern int get_first_component_started(void) ;
// Methode d'ecriture associee
extern void set_first_component_started(int new_first_component_started) ;

//
// Fonctions de manipulation de chaines
//
// copie privee en minuscules/majuscules
EXTERN char *lower(const char *srs_acp) ;
EXTERN char *upper(const char *srs_acp) ;

#endif /* _C_LEX_H_ */
