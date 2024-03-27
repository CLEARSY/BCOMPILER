/******************************* CLEARSY **************************************
* Fichier : $Id: c_ident.cpp,v 2.0 2007-03-15 14:05:40 jburlando Exp $
* (C) 2008 CLEARSY
*
* Description :		Compilateur B
*					Identificateurs
*
* Compilations :	-DDEBUG_NAME pour tracer get_ident_type_name()
*					-DDEBUG_CLONE pour tracer le clonage des identificateurs
*					-DDEBUG_IDENT pour tracer les allocations d'identificateurs
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
RCS_ID("$Id: c_ident.cpp,v 1.127 2007-03-15 14:05:40 jburlando Exp $") ;

/* Includes systeme */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

/* Includes Composant Local */
#include "c_api.h"

// Pour dire si l'on est dans la clause VALUES
static int inside_values_si = FALSE ;
void set_inside_values(int new_inside_values)
{
  TRACE1("set_inside_values(%s)", (new_inside_values == TRUE) ? "TRUE" : "FALSE") ;
  inside_values_si = new_inside_values ;
}

//GP 05/01/99
//Retourne TRUE si l'ident se trouve dans la clause passe en parametre
//sinon retourn FALSE
static int is_the_right_clause_location(T_ident* ident,
												T_lex_type clause)
{
  T_lexem *loc = ident->get_ref_lexem() ;
  TRACE1("Lexem: %x", loc) ;
  // Quand la fonction est appele pour les types de bases
  if(loc == NULL)
	{
	  return FALSE ;
	}

  while(is_a_clause(loc) == FALSE)
	{
	  loc = loc->get_prev_lexem() ;
	  if(loc == NULL)
		{
		  return FALSE ;
		}
	}

  return (loc->get_lex_type() == clause) ? TRUE : FALSE ;
}

//
//	}{	Constructeur avec type
//
T_ident::T_ident(const char *new_name,
						  T_ident_type new_ident_type,
						  T_item **adr_first,
						  T_item **adr_last,
						  T_item *new_father,
						  T_betree *new_betree,
						  T_lexem *new_ref_lexem)
  : T_expr(NODE_IDENT, adr_first, adr_last, new_father, new_betree, new_ref_lexem)
{
#ifdef DEBUG_IDENT
  TRACE7("T_ident(%x)::T_ident(%s, %x, %x, %x, %x, %x)",
		 this,
		 new_name, adr_first, adr_last, new_father, new_betree, new_ref_lexem) ;
#endif

  size_t len = strlen(new_name) ;
  name = lookup_symbol(new_name, len) ;

  typing_pred_or_subst = NULL ;
  ref_machine = NULL ;
  ref_op = NULL ;
  ref_glued = NULL ;
  ref_inherited = NULL ;
  ref_glued_ring = this ;
  implemented_by = NULL ;
  homonym_in_abstraction = NULL ;
  homonym_in_required_mach = NULL ;
  explicitly_declared = new_ref_lexem ? TRUE : FALSE ;

  first_value = last_value = NULL ;
  inherited = FALSE ;

#ifdef DEBUG_IDENT
  TRACE4("%x:%s VALUES %x, %x", this, name->get_value(), first_value, last_value) ;
#endif // DEBUG_IDENT

  ident_type = new_ident_type ;
  access_entity_location = LOC_NON_VISIBLE ;

  if (get_dep_mode() == FALSE)
	{
	  // Si le nom finit par $0, il faut mettre a jour
	  // prefix
	  const char *p = (const char *)((size_t)new_name + len - 2) ;

      if ( (len >= 2) && (*p == '$') && (*(p + 1) == '0') )
		{
		  // On construit le prefixe dans une chaine temporaire
		  TRACE0("CONSTRUCTION DU PREFIXE ...") ;
		  char *tmp = new char [len + 1] ;
		  strcpy(tmp, new_name) ;
		  *(char *)((size_t)tmp + len - 2) = '\0' ;
		  TRACE1("RECHERCHE DE PREFIXE = \"%s\"", tmp) ;
		  prefix = lookup_symbol(tmp, len -2) ;
		  delete [] tmp ;
		  suffix_number = 0 ;
		  TRACE1("FIN DE RECHERCHE DU PREFIXE = \"%s\"", tmp) ;

		  if (get_dep_mode() == FALSE)
			{
			  // On s'insere dans la liste chainee des identificateurs a verifier
			  // a posteriori (i.e. verifier qu'il appartient a un WHILE ou a un
			  // devient tel que)
			  get_betree()->get_root()->add_dollar_zero_ident(this) ;
			}
		}
	  else
		{
		  prefix = NULL ;
		}


	  // Detection des eventuels problemes lies a la collaboration avec le Kernel
	  check_kernel_compatibility() ;
	}

  if (ident_type == ITYPE_UNKNOWN)
	{
	  get_object_manager()->add_unsolved_ident(this) ;
	}

  // Chainage dans l'object manager
  get_object_manager()->add_ident(this) ;

}

//
//	}{	Constructeur pour identificateur builtin
//
T_ident::T_ident(T_keyword *builtin_name)
  : T_expr(NODE_IDENT, NULL, NULL, NULL, NULL, NULL)
{
#ifdef DEBUG_IDENT
  TRACE3("T_ident(%x)::T_ident(%x:%s)", this,
		 builtin_name, builtin_name->get_value()) ;
#endif

  name = builtin_name ;
  ident_type = ITYPE_BUILTIN ;
  access_entity_location = LOC_NON_VISIBLE ;

  typing_pred_or_subst = NULL ;
  ref_machine = NULL ;
  ref_op = NULL ;
  ref_glued = NULL ;
  ref_inherited = NULL ;
  ref_glued_ring = this;
  implemented_by = NULL ;
  homonym_in_abstraction = NULL ;
  homonym_in_required_mach = NULL ;
  explicitly_declared = FALSE ;

  first_value = last_value = NULL ;
  inherited = FALSE ;
}

// Constructeur de clonage
T_ident::T_ident(T_ident *ref_ident,
						  T_item **adr_first,
						  T_item **adr_last,
						  T_item *new_father)
  : T_expr(NODE_IDENT, adr_first, adr_last, new_father,
		   ref_ident->get_betree(),
		   ref_ident->get_ref_lexem())
{
#if (defined(DEBUG_CLONE) || defined(DEBUG_IDENT) )
  TRACE6("T_ident(%x)::T_ident(%x:%s, %x, %x, %x) CLONAGE",
		 this,
		 ref_ident,
		 ref_ident->name->get_value(),
		 adr_first,
		 adr_last,
		 new_father) ;
#endif

  typing_pred_or_subst = NULL ;

  // On clone tous les champs specifiques. Merci la metaclasse !
  // A FAIRE :: etudier la factorisation de cet algorithme
  T_metaclass *meta = get_metaclass(NODE_IDENT) ;

  T_field *cur_field = meta->get_fields() ;
  size_t first_offset = cur_field->get_field_offset() ;
  void *cur_adr = (void *)((size_t)this + first_offset) ;
  void *ref_adr = (void *)((size_t)ref_ident + first_offset) ;

  while (cur_field != NULL)
	{
#ifdef DEBUG_CLONE
	  TRACE2("ici cur_field %x  type %d", cur_field, cur_field->get_field_type()) ;
#endif // DEBUG_CLONE
	  switch(cur_field->get_field_type())
		{
		case FIELD_INTEGER :
		  {
#ifdef DEBUG_CLONE
			TRACE1("copie du champ %x ", cur_field) ;
			TRACE1("copie du champ %s ", cur_field->get_field_name()) ;
			TRACE1("copie du champ &valeur %x", ref_adr) ;
			TRACE2("copie du champ %s valeur %d",
				   cur_field->get_field_name(),
				   *(int *)ref_adr) ;
#endif
			*(int *)cur_adr = *(int *)ref_adr ;
			break ;
		  }
		case FIELD_OBJECT :
		case FIELD_STRING :
		case FIELD_POINTER :
		case FIELD_FILLING :
		  {
#ifdef DEBUG_CLONE
			TRACE1("copie du champ %x ", cur_field) ;
			TRACE1("copie du champ %s ", cur_field->get_field_name()) ;
			TRACE1("copie du champ &valeur %x", ref_adr) ;
			TRACE2("copie du champ %s valeur %x",
				   cur_field->get_field_name(),
				   *(void **)ref_adr) ;
#endif
			*(void **)cur_adr = *(void **)ref_adr ;
			break ;
		  }

		case FIELD_RESET :
		  {
			*(void **)cur_adr = (void *)NULL ;
			break ;
		  }

		case FIELD_USER :
		case FIELD_UPGRADE :
		  {
			// impossible
			assert(FALSE) ;  // A FAIRE
		  }
		// Pas de default pour etre sur qu'on traite tous les cas
		}
	  cur_adr = (void *)((size_t)cur_adr + cur_field->get_field_size()) ;
	  ref_adr = (void *)((size_t)ref_adr + cur_field->get_field_size()) ;
	  cur_field = cur_field->get_next() ;
	}

  // Il manque le champ expr de T_expr
  // A FAIRE : algo general de clonage ...

  // Recuperation du type (on en fait une copie privee)
  T_type *src_type = ref_ident->T_expr::get_B_type() ;
  if (src_type != NULL)
	{
	  T_type *ref_type = src_type->clone_type(this, get_betree(), NULL) ;
	  T_lexem *loc = (def == NULL)? typing_location : def->typing_location ;
	  T_expr::set_B_type(ref_type, loc) ;
	  object_unlink(ref_type) ;
	}
  TRACE2("type B recupere de %x : %x", ref_ident, get_B_type()) ;

  if (ref_ident->ident_type == ITYPE_ENUMERATED_SET)
	{
	  // On re-clone les values car l'algo precedent n'a fait qu'une
	  // copie de pointeur
	  // On il en faut des privees pour la mise a jour du def et du ref_machine
	  // (tables de visibilite)
	  // Cast ci-dessous et dans le while justifie par le IF ci-dessus
	  T_literal_enumerated_value *cur_evalue ;
	  cur_evalue = (T_literal_enumerated_value *)ref_ident->first_value ;
	  first_value = last_value = NULL ;

	  while (cur_evalue != NULL)
		{
		  ASSERT(cur_evalue->get_node_type() == NODE_LITERAL_ENUMERATED_VALUE) ;
		  (void)new T_literal_enumerated_value(cur_evalue,
										  (T_item **)&first_value,
										  (T_item **)&last_value,
										  this);
		  cur_evalue = (T_literal_enumerated_value *)cur_evalue->get_next() ;
		}
	}

  // Mise-a-jour de l'anneau d'homonymie
  // On le reinitialise car il est "casse" par le clonage
  ref_glued_ring = this;

  // Mise a jour des champs d'homonymie
  // Car le clonage n'a pas reinitialise les champs
  homonym_in_required_mach = NULL ;
  homonym_in_abstraction = NULL ;
  explicitly_declared = FALSE ;

  // Chainage dans l'object manager
  get_object_manager()->add_ident(this) ;
}

T_machine *T_ident::get_ref_machine(void)
{
#ifdef DEBUG_IDENT
  TRACE3("T_ident(%x:%s)::get_ref_machine(%x)",
		 this,
		 get_name()->get_value(),
		 ref_machine) ;
#endif // DEBUG_IDENT

  if (ref_machine == NULL)
	{
	  find_machine_and_op_def(&ref_machine, NULL) ;
	}

  return ref_machine ;
}

// Verification que l'identificateur pourra etre utilise sans probleme
// par le couple Kernel/Prouveur en aval du compilateur
// Detecte en particulier une partie des "conflits caches"
void T_ident::check_kernel_compatibility(void)
{
#ifdef DEBUG_IDENT
  TRACE2("T_ident(%x:%s)::check_kernel_compatibility",
		 this,
		 name->get_value()) ;
#endif // DEBUG_IDENT

#ifndef BALBULETTE
  if (    (ident_type != ITYPE_BUILTIN)
       && (ident_type != ITYPE_DEFINITION_PARAM)
       && (ident_type != ITYPE_PRAGMA_PARAMETER)
       && (name->get_len() < 2) )
    {
      // Le Kernel n'aime pas les identificateurs de moins de 2 caracteres
      // On ne fait pas le test pour les builtins car, entre autres, on
      // cree "0" et "1" au debut car c'est plus pratique
      // Idem pour les parametres formels de definitions, qui
      // n'apparaissent jamais dans le source B expanse

      if (get_kernel_joker_is_an_error() == TRUE)
        {
          syntax_error(get_ref_lexem(),
                       CAN_CONTINUE,
                       get_error_msg(E_IDENT_INVALID_FOR_KERNEL),
                       name->get_value()) ;
        }
      else
        {
          if (get_kernel_joker_is_a_warning() == TRUE) {
              syntax_warning(get_ref_lexem(),
                             BASE_WARNING,
                             get_warning_msg(W_IDENT_INVALID_FOR_KERNEL),
                             name->get_value()) ;
          }
        }
    }

#endif /* BALBULETTE */

  if (get_proof_clash_detection() == TRUE)
	{
#ifdef DEBUG_IDENT
	  TRACE1(" on regarde si \"%s\" est un mot cle du kernel", name->get_value()) ;
#endif // DEBUG_IDENT
	  // On verifie que l'on n'utilise pas un builtin du Kernel
	  // Variables statiques : on ne les initialise qu'une fois

	  static T_symbol *kernel_builtins[] =
	  {
		/*
		  Specifique PR
		*/
		lookup_symbol("FORWARD"),
		lookup_symbol("FORWARDTHEORIES"),
		lookup_symbol("NORMAL"),
		lookup_symbol("NORMALTHEORIES"),
		lookup_symbol("PROOFLEVEL"),
		lookup_symbol("PROOFMETHOD"),
		lookup_symbol("SET"),
		lookup_symbol("SETSPE"),
		lookup_symbol("THEORIES"),

		/*
		  GOP/PR/Animateur
		*/
		lookup_symbol("finite"),
		lookup_symbol("infinite"),
		lookup_symbol("choice") ,
		lookup_symbol("plus") ,
		lookup_symbol("minus") ,
		lookup_symbol("multiply") ,
		lookup_symbol("divide"),

		/*
		  Specifique GOP
		*/
		lookup_symbol("SET"),

		/*
		  Specifique Kernel
		*/
		lookup_symbol("ARI"),
		lookup_symbol("CATL"),
		lookup_symbol("DED"),
		lookup_symbol("DEF"),
		lookup_symbol("END"),
		lookup_symbol("FLAT"),
		lookup_symbol("GEN"),
		lookup_symbol("HYP"),
		lookup_symbol("IS"),
		lookup_symbol("LMAP"),
		lookup_symbol("MAP"),
		lookup_symbol("MODR"),
		lookup_symbol("NEWV"),
		lookup_symbol("RES"),
		lookup_symbol("REV"),
		lookup_symbol("RULE"),
		lookup_symbol("SHELL"),
		lookup_symbol("SUB"),
		lookup_symbol("THEORY"),
		lookup_symbol("WRITE"),
		lookup_symbol("bUpident"),
		lookup_symbol("band"),
		lookup_symbol("bappend"),
		lookup_symbol("bcall"),
		lookup_symbol("bcall1"),
		lookup_symbol("bcall2"),
		lookup_symbol("bcatl"),
		lookup_symbol("bclean"),
		lookup_symbol("bclose"),
		lookup_symbol("bcompile"),
		lookup_symbol("bconnect"),
		lookup_symbol("bcrel"),
		lookup_symbol("bcrelr"),
		lookup_symbol("bcrer"),
		lookup_symbol("bctrule"),
		lookup_symbol("bdef1"),
		lookup_symbol("bdef2"),
		lookup_symbol("bdump"),
		lookup_symbol("berv"),
		lookup_symbol("bfalse"),
		lookup_symbol("bfwd"),
		lookup_symbol("bget"),
		lookup_symbol("bgethyp"),
		lookup_symbol("bgetresult"),
		lookup_symbol("bgoal"),
		lookup_symbol("bguard"),
		lookup_symbol("bhalt"),
		lookup_symbol("bident"),
		lookup_symbol("binhyp"),
		lookup_symbol("blemma"),
		lookup_symbol("blen"),
		lookup_symbol("blenf"),
		lookup_symbol("blent"),
		lookup_symbol("blident"),
		lookup_symbol("bload"),
		lookup_symbol("blvar"),
		lookup_symbol("bmark"),
		lookup_symbol("bmatch"),
		lookup_symbol("bmodr"),
		lookup_symbol("bnewv"),
		lookup_symbol("bnlmap"),
		lookup_symbol("bnmap"),
		lookup_symbol("bnot"),
		lookup_symbol("bnum"),
		lookup_symbol("bpattern"),
		lookup_symbol("bpop"),
		lookup_symbol("bprintf"),
		lookup_symbol("bproved"),
		lookup_symbol("breade"),
		lookup_symbol("breadf"),
		lookup_symbol("brecompact"),
		lookup_symbol("bresetcomp"),
		lookup_symbol("bresult"),
		lookup_symbol("brev"),
		lookup_symbol("brule"),
		lookup_symbol("bsearch"),
		lookup_symbol("bsetmode"),
		lookup_symbol("bshell"),
		lookup_symbol("bslmap"),
		lookup_symbol("bsmap"),
		lookup_symbol("bsparemem"),
		lookup_symbol("bsrv"),
		lookup_symbol("bstatistics"),
		lookup_symbol("bstring"),
		lookup_symbol("bsubfrm"),
		lookup_symbol("btest"),
		lookup_symbol("bunproved"),
		lookup_symbol("bvrb"),
		lookup_symbol("bwritef"),
		lookup_symbol("bwritem"),
		lookup_symbol("trace"),
	  } ;

	  static int nb_keywords = sizeof(kernel_builtins) / sizeof(T_symbol *) ;
	  // TRACE1("il y a %d builtins du kernel", nb_keywords) ;

	  int i = 0 ;

	  for (i = 0 ; i < nb_keywords ; i++)
		{
		  if (kernel_builtins[i] == name)
			{
			  syntax_error(get_ref_lexem(),
						   CAN_CONTINUE,
						   get_error_msg(E_IDENT_CLASHES_WITH_KERNEL_BUILTIN),
						   name->get_value()) ;
			}
		}
	}
}


//
//	Destructeur
//
T_ident::~T_ident(void)
{
  TRACE2("T_ident::~T_ident(%x, %s)", this, name->get_value()) ;
  ENTER_TRACE ;

  // Surtout ne pas oublier de supprimer l'identificateur
  // de la liste de l'object manager
  get_object_manager()->delete_ident(this) ;

  // Idem liste "unsolved ident" (methode intelligente
  // qui verifie si on est dans la liste)
  get_object_manager()->delete_unsolved_ident(this) ;

  EXIT_TRACE ;
  TRACE0(" fin ~T_ident") ;

}

void T_ident::set_ref_glued(T_ident *new_ref_glued)
{
#ifdef DEBUG_GLUED
  TRACE2("T_ident::set_ref_glued(%x, %x)", this, new_ref_glued);
#endif

  ref_glued = new_ref_glued ;

  // L'heritage est en fait un collage
  ref_inherited = NULL ;

  // Maj du champ inherited
  inherited = TRUE ;

  // Maj de l'anneau des identificateurs colles
  insert_in_glued_ring(new_ref_glued);
}

void T_ident::set_ref_inherited(T_ident *new_ref_inherited)
{
#ifdef DEBUG_GLUED
  TRACE2("T_ident::set_ref_inherited(%x, %x)", this, new_ref_inherited);
#endif

  ref_inherited = new_ref_inherited ;

  // Maj de l'anneau des identificateurs colles
  insert_in_glued_ring(new_ref_inherited);
}

// Insere un nouvel identificateur, et l'anneau associe, dans l'anneau des
// homonymes
void T_ident::insert_in_glued_ring(T_ident *new_ref_glued)
{
#ifdef DEBUG_GLUED
  TRACE2("T_ident::insert_in_glued_ring(%x, %x)", this, new_ref_glued);
#endif

  if ((ident_type == ITYPE_UNKNOWN) ||
	  (new_ref_glued->get_ident_type() == ITYPE_UNKNOWN))
	{
	  // On ne met que les definitions dans les anneaux
#ifdef DEBUG_GLUED
	  TRACE0("L'un des deux n'est pas une definition");
#endif
	  return;
	}

  if (this == new_ref_glued)
	{
#ifdef DEBUG_GLUED
	  TRACE0("C'est le meme identificateur !");
#endif
	  return;
	}

  T_ident *cur = ref_glued_ring;
  while (cur != this)
	{
	  if (cur == new_ref_glued)
		{
          // Le nouvel identificateur est deja dans l'anneau
#ifdef DEBUG_GLUED
		  TRACE0("C'est le meme anneau !");
#endif
		  return;
		}
	  cur = cur->get_ref_glued_ring();
	}

  // on insere le nouvel identificateur et son anneau
  T_ident *tmp_ident = ref_glued_ring;
  ref_glued_ring = new_ref_glued->get_ref_glued_ring();
  new_ref_glued->set_ref_glued_ring(tmp_ident);

  // Pour un ensemble enumere, il faut aussi coller les elements entre
  // eux.
  if (this->get_ident_type() == ITYPE_ENUMERATED_SET &&
	  new_ref_glued->get_ident_type() == ITYPE_ENUMERATED_SET)
	{
#ifdef DEBUG_GLUED
      TRACE0("On colle aussi les literaux enumeres");
#endif
	  T_expr *cur1 = this->get_values();
	  T_expr *cur2 = new_ref_glued->get_values();
	  while (cur1 != NULL && cur2 != NULL)
		{
		  ASSERT(cur1->is_an_ident());
		  ASSERT(cur2->is_an_ident());

#ifdef DEBUG_GLUED
		  ENTER_TRACE;
#endif
		  cur1->make_ident()->insert_in_glued_ring(cur2->make_ident());
#ifdef DEBUG_GLUED
		  EXIT_TRACE;
#endif

		  cur1 = (T_expr *) cur1->get_next();
		  cur2 = (T_expr *) cur2->get_next();
		}

#ifdef DEBUG_GLUED
      TRACE0("Fin collage des literaux enumeres");
#endif
	}



#ifdef DEBUG_GLUED
  TRACE2("%x->ref_glued_ring = %x", this, ref_glued_ring);
  TRACE2("%x->ref_glued_ring = %x",
		 new_ref_glued, new_ref_glued->get_ref_glued_ring());
#endif
}

// Retourne TRUE si this et ident2 sont dans le meme anneau d'homonymie,
// FALSE sinon
int T_ident::is_in_glued_ring(T_ident *ident2)
{
  TRACE2("T_ident::is_in_glued_ring(%x, %x)", this, ident2);

  // On parcourt l'anneau d'homonymie de this
  T_ident *cur = ref_glued_ring;
  while (cur != ident2 && cur != this)
	{
	  cur = cur->get_ref_glued_ring();
	}

  return (cur == ident2);
}


//
//	}{	Classe T_literal_enumerated_value
//
T_literal_enumerated_value::
T_literal_enumerated_value(const char *new_name,
							T_item **adr_first,
							T_item **adr_last,
							T_item *new_father,
							T_betree *new_betree,
							T_lexem *new_ref_lexem)
  : T_ident(new_name,
			ITYPE_ENUMERATED_VALUE,
			adr_first,
			adr_last,
			new_father,
			new_betree,
			new_ref_lexem)
{
  //#ifdef DUMP_TYPES
  TRACE6("T_literal_enumerated_value::T_literal_eval(%s, %x, %x, %x, %x, %x)",
		 new_name, adr_first, adr_last, new_father, new_betree, new_ref_lexem) ;
  //#endif


  set_node_type(NODE_LITERAL_ENUMERATED_VALUE) ;

  T_literal_enumerated_value *previous = (T_literal_enumerated_value *)get_prev() ;

  if (previous == NULL)
	{
	  value = 0 ;
	}
  else
	{
	  value = previous->value + 1 ;
	}
}

//
//	Destructeur
//
T_literal_enumerated_value::~T_literal_enumerated_value(void)
{
  TRACE1("T_literal_enumerated_value::~T_literal_enumerated_value(%x)", this) ;
}

//
//	}{	Classe T_renamed_ident : Constructeur sans type
//
T_renamed_ident::T_renamed_ident(const char *new_name,
										  T_ident_type new_ident_type,
										  T_item **adr_first,
										  T_item **adr_last,
										  T_item *new_father,
										  T_betree *new_betree,
										  T_lexem *new_ref_lexem)
  : T_ident(new_name,
			new_ident_type,
			adr_first,
			adr_last,
			new_father,
			new_betree,
			new_ref_lexem)
{
#ifdef DUMP_TYPES
  TRACE6("T_renamed_ident::T_renamed_ident(%s, %x, %x, %x, %x, %x)",
		 new_name, adr_first, adr_last, new_father, new_betree, new_ref_lexem) ;
#endif


  // On corrige le node_type
  set_node_type(NODE_RENAMED_IDENT) ;

  // On cherche le '.' - il y est par construction
  const char *full_name = get_name()->get_value() ;
  char *tmp_name ;
  clone(&tmp_name, full_name) ;
  const char *p = strchr(full_name, '.') ;
  ASSERT(p != NULL) ;
  size_t instance_len = (size_t)p - (size_t)full_name ;
  *(char *)((size_t)tmp_name + instance_len) = '\0' ;
  TRACE0("..instance name") ;
  instance_name = lookup_symbol(tmp_name, instance_len) ;
  TRACE0("..component name") ;
  component_name = lookup_symbol(p + 1,
								 get_name()->get_len() - (instance_len + 1)) ;

  s_free(tmp_name) ;
#ifdef DUMP_TYPES
  TRACE3("renamed_ident(%x): instance_name %x component_name %x",
		 this,
		 instance_name,
		 component_name) ;
  TRACE5("renamed_ident(%x): instance_name %x:%s component_name %x:%s",
		 this,
		 instance_name,
		 instance_name->get_value(),
		 component_name,
		 component_name->get_value()) ;
#endif

}

// Constructeur de clonage
T_renamed_ident::T_renamed_ident(T_ident *ref_ident,
										  T_symbol *prefix,
										  T_item **adr_first,
										  T_item **adr_last,
										  T_item *new_father)
  : T_ident(ref_ident, adr_first, adr_last, new_father)
{
#ifdef DEBUG_CLONE
  TRACE6("T_renamed_ident(%x)::T_renamed_ident(%x:%s, %x, %x, %x) CLONAGE",
		 this,
		 ref_ident,
		 ref_ident->get_name()->get_value(),
		 adr_first,
		 adr_last,
		 new_father) ;
#endif
  set_node_type(NODE_RENAMED_IDENT) ;

  instance_name = prefix ;
  component_name = ref_ident->get_name() ;

  char *name = new char[prefix->get_len() + get_name()->get_len() + 2] ;
#ifdef STATS_ALLOC
  add_alloc(name,
			prefix->get_len() + get_name()->get_len() + 2,
			__FILE__,
			__LINE__) ;
#endif
  sprintf(name, "%s.%s",
		  instance_name->get_value(),
		  get_name()->get_value()) ;

  set_name(lookup_symbol(name, prefix->get_len() + get_name()->get_len() + 1)) ;

#ifdef DUMP_TYPES
  TRACE3("renamed_ident(%x): instance_name %x component_name %x",
		 this,
		 instance_name,
		 component_name) ;
  TRACE5("renamed_ident(%x): instance_name %x:%s component_name %x:%s",
		 this,
		 instance_name,
		 instance_name->get_value(),
		 component_name,
		 component_name->get_value()) ;
#endif

}

//
//	Destructeur
//
T_renamed_ident::~T_renamed_ident(void)
{
  TRACE1("--> T_renamed_ident::~T_renamed_ident(%x)", this) ;
}

// Table anglaise
static const char *const english_ident_type_name_sct[] =
{
  /* 00 */
  "UNKNOWN",
  /* 01 */
  "concrete constant",
  /* 02 */
  "concrete variable",
  /* 03 */
  "builtin",
  /* 04 */
  "machine name",
  /* 05 */
  "abstraction name",
  /* 06 */
  "machine scalar formal parameter",
  /* 07 */
  "set",
  /* 08 */
  "operation name",
  /* 09 */
  "local operation name",
  /* 10 */
  "machine set formal parameter",
  /* 11 */
  "used machine name",
  /* 12 */
  "enumerated value",
  /* 13 */
  "definition parameter",
  /* 14 */
  "operation input parameter",
  /* 15 */
  "operation output parameter",
  /* 16 */
  "operation local variable",
  /* 17 */
  "local quantifier",
  /* 18 */
  "abstract variable",
  /* 19 */
  "abstract constant",
  /* 20 */
  "enumerated set",
  /* 21 */
  "used operation name",
  /* 22 */
  "record label",
  /* 23 */
  "local quantifier",
  /* 24 */
  "pragma parameter",
} ;

// Table francaise
static const char *const french_ident_type_name_sct[] =
{
  /* 00 */
  "UNKNOWN",
  /* 01 */
  "constante concrete",
  /* 02 */
  "variable concrete",
  /* 03 */
  "predefini",
  /* 04 */
  "nom de machine",
  /* 05 */
  "nom d'abstraction",
  /* 06 */
  "parametre formel scalaire de machine",
  /* 07 */
  "ensemble",
  /* 08 */
  "nom d'operation",
  /* 09 */
  "nom d'operation locale",
  /* 10 */
  "parametre formel ensembliste de machine",
  /* 11 */
  "nom de machine utilisee",
  /* 12 */
  "nom de valeur enumeree",
  /* 13 */
  "parametre de definition",
  /* 14 */
  "parametre d'entree d'operation",
  /* 15 */
  "parametre de sortie d'operation",
  /* 16 */
  "variable locale d'operation",
  /* 17 */
  "quantificateur local",
  /* 18 */
  "variable abstraite",
  /* 19 */
  "constante abstraite",
  /* 20 */
  "ensemble enumere",
  /* 21 */
  "nom d'operation utilisee",
  /* 22 */
  "label de record",
  /* 23 */
  "quantificateur local",
  /* 24 */
  "parametre de pragma",
} ;

static int use_english = TRUE ;
void french_ident_type_name(void)
{
  ASSERT(sizeof(french_ident_type_name_sct)
		 == sizeof(english_ident_type_name_sct)) ;
   use_english = FALSE ;
}

void english_ident_type_name(void)
{
  ASSERT(sizeof(french_ident_type_name_sct)
		 == sizeof(english_ident_type_name_sct)) ;
   use_english = TRUE ;
}

const char *T_ident::get_ident_type_name(void)
{
#ifdef DEBUG_NAME
  TRACE1("T_ident(%x)::get_ident_type_name()", this) ;
#endif
  // On  remonte a la definition seulement si le type est ITYPE_UNKNOWN
  if (ident_type == ITYPE_UNKNOWN && def != NULL)
	{
#ifdef DEBUG_NAME
	  TRACE1("on demande a def = %x", def) ;
#endif
#ifndef NO_CHECKS
	  if (def == this)
		{
		  TRACE2("PANIC %x:%s :: def = this", this, name->get_value()) ;
		  assert(FALSE) ;
		  return "<PANIC IL Y A BOUCLAGE>" ;
		}
#endif
	  return def->get_ident_type_name() ;
	}

#ifndef NO_CHECKS
  if ((size_t)ident_type >=
	  (size_t) (sizeof(english_ident_type_name_sct) / sizeof(char *)))
	{
	  // Developpement !!
	  fprintf(stderr,
			  "!!! ident type %d is out of bounds 0..%d\n",
			  ident_type,
                          (int)(sizeof(english_ident_type_name_sct) / sizeof(char *) - 1)) ;
	  return "???" ;
	}
#endif /* NO_CHECKS */

  // A FAIRE :: etudier la faisabililte d'une indirection par catalogue
  return (use_english == TRUE)
	? english_ident_type_name_sct[ident_type]
	: french_ident_type_name_sct[ident_type] ;
}

// Les 2 methodes qui suivent permettent de donner le type B de
// l'ident et le type d'identificateur. Elles remontent toutes les 2
// tant que def != NULL. Elles doivent s'arreter au meme endroit donc
// TOUJOURS MODIFIER LES 2 METHODES EN MEME TEMPS !!!
T_type *T_ident::get_B_type(void)
{
#ifdef DUMP_TYPES
  TRACE2("T_ident(%x)::get_B_type() :: demande au def = %x", this, def) ;
	//  TRACE3("T_ident(%x:%s)::get_B_type->%x",
	// this, name->get_value(),
		 //		 (def == NULL) ? T_expr::get_B_type() : def->get_B_type()) ;
#endif
  return (def == NULL) ? T_expr::get_B_type() : def->get_B_type() ;
}

const char *T_ident::get_def_type_name(void)
{
  return (def == NULL) ? get_ident_type_name() : def->get_def_type_name() ;
}


T_type *T_ident::get_original_B_type(void)
{
  return (def == NULL)
	? T_expr::get_original_B_type() : def->get_original_B_type() ;
}

// Pour tous les abstract_type comme atype, propagation de la valuation
// Idem pour tous les identificateurs values avec ces types
void T_ident::fix_atype_list(T_setof_type *setof,
									  T_abstract_type *atype,
									  T_abstract_type *list_atype)
{
#ifdef FULL_DUMP_TYPES
  T_machine *ref_machine = NULL ;
  find_machine_and_op_def(&ref_machine, NULL) ;
  TRACE3("fix_atype_list(%x:%s, contexte %s)",
		 this,
		 name->get_value(),
		 ref_machine->get_context()->get_name()->get_name()->get_value()) ;
  //  get_object_manager()->dump_atype_list() ;
#endif


  //  T_ident *ref_name = atype->get_set() ;
  T_abstract_type *cur_atype = list_atype ;
  T_ident *val_ident = atype->get_valuation_ident() ;

  T_type *new_type = setof->get_base_type() ;

  while (cur_atype != NULL)
	{
#ifdef FULL_DUMP_TYPES
	  TRACE2("cur_atype %x:%s", cur_atype, cur_atype->get_class_name()) ;
#endif

	  ASSERT(cur_atype->get_node_type() == NODE_ABSTRACT_TYPE) ;

	  //	  if (cur_atype->get_set()->get_name()->compare(ref_name) == TRUE)
	  // DEBUT SL 11/03/1999
	  // was :
	  // if (cur_atype->get_set() == ref_name)
	  // new :
	  if (cur_atype->get_set()->get_definition() == get_definition())
		// FIN SL 11/03/1999
		{
		  // Maj champ after_valuation_type
#ifdef FULL_DUMP_TYPES
		  TRACE4("maj type %x:%s avec %x:%s",
				 cur_atype,
				 cur_atype->make_type_name()->get_value(),
				 setof->get_base_type(),
				 setof->get_base_type()->make_type_name()->get_value()) ;
#endif

		  if (cur_atype != new_type)
			{
			  cur_atype->set_after_valuation_type(new_type) ;

			  // TMP_STATE
			  // set_block_state(new_type, MINFO_ALWAYS_PERSISTENT) ;
			  // TMP_STATE

			  cur_atype->set_valuation_ident(val_ident) ;
			}

		}

	  cur_atype = cur_atype->get_next_abstract_type() ;
	}
}


void T_ident::set_B_type(T_type *new_B_type,
								  T_lexem *location)
{
#ifdef DUMP_TYPES
  TRACE6("T_ident(%x:%s)::set_B_type(%x:%s) location=%x inside_value: %s",
		 this,
		 name->get_value(),
		 new_B_type,
		 (new_B_type == NULL) ? "null" : new_B_type->make_type_name()->get_value(),
		 location,
		 inside_values_si == TRUE ? "true" : "false") ;
#endif
  T_item *father = get_father();
#ifdef TRACE
  if (father == NULL)
	{
	  TRACE2(" T_ident::set_B_type : father de %s (%x) == NULL !!!!!!!!", name->get_value(), this) ;
	}
  else
	{
	  if (father->is_a_predicate() == FALSE)
		TRACE1("father->is_a_predicate() == FALSE pour %s !!!!!!",name->get_value()) ;
	  if (father->is_a_substitution() == FALSE)
		TRACE1("father->is_a_substitution() == FALSE pour %s !!!!!!",name->get_value()) ;
	  if (father->is_a_typing_formula() == TRUE)
		TRACE1("father->is_a_typing_formula() == TRUE pour %s !!!!!!",name->get_value()) ;
	}
#endif

  if (father != NULL &&
	  (father->is_a_predicate() == TRUE ||
	   father->is_a_substitution() == TRUE))
	{
	  this->set_typing_pred_or_subst(father) ;
	}

  if (new_B_type == NULL)
	{
	  // Reprise sur erreur
	  return ;
	}

  if (    (get_B_type() == NULL) // ident pas encore type
	   && (new_B_type->has_wildcards() == TRUE) ) // typage eventuel avec jokers
	{
	  // Une expression avec wildcards n'est pas typante
#ifdef DUMP_TYPES
	  TRACE0("type avec wildcards non typant ") ;
#endif // DUMP_TYPES

	  semantic_error(new_B_type,
					 CAN_CONTINUE,
					 get_error_msg(E_CAN_NOT_TYPE_WITH_A_GENERIC_TYPE),
					 get_name()->get_value(),
					 make_type_name(new_B_type)->get_value()) ;
	  return ;
	}

  // Si l'expression etait deja typee, alors on verifie
  // la compatibilite du nouveau type
  //
  // Sauf : si l'identificateur est un SET ou une CONSTANTE
  // abstraite, et que l'on est en train de la valuer
  // Dans ce cas on est en train de "plonger" le type
  // i.e. sauf si on est dans la clause VALUES
  // i.e. sauf si on est dans la clause VALUES
  if (inside_values_si == TRUE)
	{
	  // Clause values ...
	  // On type !
#ifdef DUMP_TYPES
	  TRACE2("Clause Values : type precedent = %x %s",
			 get_B_type(),
			 (get_B_type() == NULL)
			 ? "" : get_B_type()->make_type_name()->get_value()) ;
#endif


	  T_type *prev_type = get_B_type() ;
	  T_abstract_type *atype = NULL ;

	  // Est-ce une valuation ?
	  if (    (prev_type != NULL)
		   && (prev_type->can_be_valuated_by(new_B_type, this, &atype) == TRUE) )
		{
		  // Cas justifie car type test dans can_be_valuated_by
		  T_setof_type *setof = (T_setof_type *)new_B_type->clone_type(this,
																	   get_betree(),
																	   location) ;

		  // Mise a jour de tous les abstract_type
		  fix_atype_list(setof,
						 atype,
						 get_object_manager()->get_abstract_types()) ;

		  // Le plongement est effectue avec succes : on s'arrete la
	 	  return ;
		}

	  // Ici : ce n'est pas un cas de plongement. On retombe dans
	  // l'algorithme generique d'affectation de type
	}

  // Si on n'est pas dans une formule de typage, on s'arrete tout de suite
  // apres avoir verifie la compatibilite de type
  int it_is_not_a_typing_formula = FALSE ;
  if ( (get_ref_lexem() != NULL) // pas un builtin
	   && (get_father() != NULL) // reprise sur erreur
	   && (get_father()->is_a_typing_formula() == FALSE)
	   && (BOM_type == NULL) )// pas typage
	{
	  TRACE4("%s:%d:%d N'EST PAS UN CONTEXTE DE TYPAGE DE %s",
			 get_ref_lexem()->get_file_name()->get_value(),
			 get_ref_lexem()->get_file_line(),
			 get_ref_lexem()->get_file_column(),
			 get_name()->get_value()) ;
	  it_is_not_a_typing_formula = TRUE ;
	}

  // Algorithme generique d'affectation de type
  // Si l'identificateur est deja type, on verifie la compatibilite
  if (get_B_type() != NULL)
	{
	  TRACE0("B_type = NULL") ;
	  if (get_B_type()->is_compatible_with(new_B_type) == FALSE)
		{


#ifdef DUMP_TYPES
		  TRACE0("types incompatibles -> on ne type pas") ;
#endif

		  const char *old_type_name = make_type_name(this)->get_value() ;
		  semantic_error(location,
						 CAN_CONTINUE,
						 get_error_msg(E_INCOMPATIBLE_TYPES),
						 new_B_type->make_type_name()->get_value(),
						 old_type_name) ;

		  semantic_error_details(get_typing_location(),
								 get_error_msg(E_LOCATION_OF_IDENT_TYPE),
								 old_type_name,
								 get_def_type_name(),
								 name->get_value()) ;
		  // On ne peut pas continuer !
		  return ;


		}

	  if (it_is_not_a_typing_formula == TRUE)
		{
		  return ;

		}
#ifdef DUMP_TYPES
	  TRACE0("ici : on sait que les types sont compatibles") ;
#endif

	  // Identificateur deja type avec un type compatible :
	  // on peut sortir sans rien faire !
	  // Sauf : si le type precedent a des bornes "T_bound",
	  // auquel cas on les met a jour
	  // On utilise "do_return" pour savoir s'il fait faire
	  // ce travail ou pas
	  int do_return = TRUE ;
	  if ( (is_a_set() == TRUE) && (new_B_type->has_wildcards(TRUE) == FALSE) )
		{
		  T_type *base = ((T_setof_type *)get_B_type())->get_base_type() ;
		  if (base->is_a_base_type() == TRUE)
			{
			  TRACE0("base->is_a_base_type() = TRUE") ;
			  T_base_type *btype = (T_base_type *)base ;
			  T_expr *upper_bound = btype->get_upper_bound() ;

			  if ( (upper_bound != NULL)
				   && (upper_bound->get_node_type() == NODE_BOUND) )
				{
				  // Par construction
				  ASSERT(btype->get_lower_bound()->get_node_type()==NODE_BOUND);

				  // On recupere les bornes !
				  TRACE0("on recupere les bornes ...") ;
				  do_return = FALSE ;

				  // On garde le nom avant de patcher le type
				  TRACE1("new_B_type %x", new_B_type) ;
				  TRACE1("new_B_type %s",
						 new_B_type->make_type_name()->get_value()) ;
				  TRACE1("new_B_type %s", new_B_type->get_class_name()) ;
				  ASSERT(new_B_type->is_a_set() == TRUE) ;

				  // Cast justifie par le if ci-dessus
				  T_base_type *obase = (T_base_type *)base ;

				  // Cast justifies par equivalence de type
				  T_type *tmp = ((T_setof_type *)new_B_type)->get_base_type();
				  ASSERT(tmp->is_a_base_type() == TRUE) ;
				  T_base_type *nbase = (T_base_type *)tmp ;

				  // Debut ajout 17/08/97 : recuperation eventuelle
				  // du nom de type
				  if (obase->get_name() != NULL)
					{
					  TRACE3("on recupere le nom %x:%s -> maj de %x",
							 obase->get_name(),
							 obase->get_name()->get_name()->get_value(),
							 nbase) ;
					  nbase->set_name(obase->get_name()) ;
					}
				  // Fin ajout 17/08/97

				  TRACE2("on garde le nom de l'intervalle %s->on le met ds %x",
						 obase->get_name()->get_name()->get_value(), nbase) ;
				  ((T_base_type *)nbase)->set_name(obase->get_name()) ;

				  if ( (obase->get_node_type() == NODE_PREDEFINED_TYPE)
					   &&(((T_predefined_type *)obase)->get_ref_interval()
						  !=NULL)
					   && (nbase->get_node_type() == NODE_PREDEFINED_TYPE) )
					{
					  TRACE3("!!! maj du ref interval de nbase %x avec celui de obase %x ie %x",
							 nbase,
							 obase,
							 ((T_predefined_type *)obase)->get_ref_interval()) ;
					  ((T_predefined_type *)nbase)->set_ref_interval(
																	 ((T_predefined_type *)obase)->get_ref_interval()) ;
					}
				}

			}
		  // Modif 17/08/97 : test do_return apres le bloc,
		  // et pas dans car si is_a_set est FALSE, on
		  // veut sortir
		}

#ifdef DUMP_TYPES
	  TRACE1("ici do_return = %s", (do_return == TRUE) ? "TRUE" : "FALSE") ;
#endif
	  if (do_return == TRUE)
		{
		  // Ici, new_B_type ne sert a rien, donc pourrait etre libere
		  return ;
		}
	  // } Modif 17/08/97, cf commentaire ci-dessus, meme date
	}
  else
	{
	  if (it_is_not_a_typing_formula == TRUE)
		{
		  return ;
		}

	  if (new_B_type->get_node_type() == NODE_RECORD_TYPE)
		// et implicitement : get_B_type() == NULL
		{
		  // Dans le cas d'un type record, il faut verifier qu'il
		  // ne contient pas de jokers
		  if ( ((T_record_type *)new_B_type)->has_jokers() == TRUE)
			{
			  semantic_error(this,
							 CAN_CONTINUE,
							 get_error_msg(
							 	E_CAN_NOT_TYPE_IDENT_WITH_JOKER_RECORD_TYPE),
							 make_type_name(new_B_type)->get_value()) ;
			  return ;
			}
		}
	}

  // On type !
#ifdef DUMP_TYPES
  if (get_ref_lexem() != NULL)
	{
	  TRACE4("on type %s %s:%d:%d",
			 get_name()->get_value(),
			 get_ref_lexem()->get_file_name()->get_value(),
			 get_ref_lexem()->get_file_line(),
			 get_ref_lexem()->get_file_column()) ;
	}
#endif

  internal_set_B_type(new_B_type, location) ;
}

// Affectation effective une fois les controles realises
void T_ident::internal_set_B_type(T_type *new_B_type,
										   T_lexem *location)
{
#ifdef DUMP_TYPES
  TRACE4("T_ident(%x:%s)::internal_set_B_type(%x) location=%x",
		 this,
		 name->get_value(),
		 new_B_type,
		 location) ;
#endif

  // Recherche de la definition
  T_ident *cur = this ;
  while (cur->def != NULL)
	{
	  cur = cur->def ;
	}


#ifdef DUMP_TYPES
  TRACE1("cur->typing_location = %x", cur->typing_location) ;
#endif

  //GP 6/01/99 was:
#if 0
  //Supprime car ici le plongement est utilise a tord
  if (cur->typing_location != NULL)
	{
	  TRACE1("cur->get_name %s", cur->get_name()->get_value()) ;
	  cur->original_typing_location = cur->typing_location ;
	  cur->typing_location = location ;
	}
#endif

  //On plonge seulement dans la clause VALUES
  if(cur->typing_location != NULL)
	{
	if(is_the_right_clause_location(this, L_VALUES) == TRUE)
	  {
		cur->original_typing_location = cur->typing_location ;
		cur->typing_location = location ;
	  }
	}
  else
	{
	   cur->typing_location = location ;
	}


  cur->T_expr::set_B_type(new_B_type, location) ;
}

T_lexem *T_ident::get_typing_location(void)
{
#ifdef DUMP_TYPES
  TRACE4("T_ident(%x:%s)::get_typing_location(def = %x)->%x",
		 this, name->get_value(),
		 def,
		 typing_location)  ;
#endif

  if (get_Use_B0_Declaration() == TRUE
      &&
      BOM_lexem_type != NULL)
    {
      return BOM_lexem_type;
    }
  else
    {
      return (def == NULL) ? typing_location : def->get_typing_location() ;
    }
}

T_lexem *T_ident::get_original_typing_location(void)
{
#ifdef DUMP_TYPES
  TRACE4("T_ident(%x:%s)::get_original_typing_location(def = %p)->%p",
		 this, name->get_value(),
		 def,
		 original_typing_location)  ;
#endif
  if (get_Use_B0_Declaration() == TRUE
      &&
      BOM_lexem_type != NULL)
    {
      return BOM_lexem_type;
    }
  else
    {
      return (def == NULL)
	? original_typing_location : def->get_original_typing_location() ;
    }
}

T_item *T_ident::get_typing_pred_or_subst(void)
{
  T_ident *cur_ident = this->get_definition() ;


  TRACE4("--> get_typing_pred_or_subst %s (%p, this : %p) return %p",
		 cur_ident->get_name()->get_value(),
		 cur_ident,
		 this,
		 cur_ident->typing_pred_or_subst) ;

  return cur_ident->typing_pred_or_subst ;
}

void T_ident::set_typing_pred_or_subst(T_item *new_pred)
{
  ASSERT(new_pred->is_a_predicate() == TRUE ||
		 new_pred->is_a_substitution() == TRUE) ;

  T_ident *cur_ident = this->get_definition() ;

  if (cur_ident->typing_pred_or_subst != NULL)
        {
          TRACE4("--> set_typing_pred_or_subst ident : %s (%p, this : %p) pred : %p deje fait",
                 cur_ident->get_name()->get_value(),
                 cur_ident,
                 this,
                 new_pred) ;
        }
  else
        {
          cur_ident->typing_pred_or_subst = new_pred ;
          TRACE4("--> set_typing_pred_or_subst ident : %s (%p, this : %p) pred : %p !!",
				 cur_ident->get_name()->get_value(),
				 cur_ident,
				 this,
				 new_pred) ;
        }
}


// Reset du type B
void T_ident::reset_B_type(void)
{
  TRACE1("T_ident(%x)::reset_B_type()", this) ;
  if (def == NULL)
	{
	  T_expr::reset_B_type() ;
	  typing_location = NULL ;
	}
  else
	{
	  def->reset_B_type() ;
	}
}

static T_access_entity_type ident_type_to_entity_type_sit[] =
{
  /* 00 ITYPE_UNKNOWN */
  OTHER_ACCESS,
  /* 01 ITYPE_CONCRETE_CONSTANT */
  ACCESS_CONCRETE_CONSTANT,
 /* 02 ITYPE_CONCRETE_VARIABLE */
  ACCESS_CONCRETE_VARIABLE,
 /* 03 ITYPE_BUILTIN */
  OTHER_ACCESS,
  /* 04 ITYPE_MACHINE_NAME */
  OTHER_ACCESS,
  /* 05 ITYPE_ABSTRACTION_NAME */
  OTHER_ACCESS,
  /* 06 ITYPE_MACHINE_SCALAR_FORMAL_PARAM */
  ACCESS_FORMAL_PARAMETER,
  /* 07 ITYPE_ABSTRACT_SET */
  ACCESS_ABSTRACT_SET,
  /* 08 ITYPE_OP_NAME */
  ACCESS_OPERATION,
  /* 09 ITYPE_LOCAL_OP_NAME */
  ACCESS_LOCAL_OPERATION,
  /* 10 ITYPE_MACHINE_SET_FORMAL_PARAM */
  ACCESS_FORMAL_PARAMETER,
  /* 11 ITYPE_USED_MACHINE_NAME */
  OTHER_ACCESS,
  /* 12 ITYPE_ENUMERATED_VALUE */
  ACCESS_ENUMERATED_VALUE,
  /* 13 ITYPE_DEFINITION_PARAM */
  OTHER_ACCESS,
  /* 14 ITYPE_OP_IN_PARAM */
  ACCESS_IN_PARAMETER,
  /* 15 ITYPE_OP_OUT_PARAM */
  OTHER_ACCESS,
  /* 16 ITYPE_LOCAL_VARIABLE */
  OTHER_ACCESS,
  /* 17 ITYPE_LOCAL_QUANTIFIER */
  OTHER_ACCESS,
  /* 18 ITYPE_ABSTRACT_VARIABLE */
  ACCESS_ABSTRACT_VARIABLE,
  /* 19 ITYPE_ABSTRACT_CONSTANT */
  ACCESS_ABSTRACT_CONSTANT,
    /* 20 ITYPE_ENUMERATED_SET */
    ACCESS_ENUMERATED_SET,
    /* 21 ITYPE_USED_OP_NAME */
    OTHER_ACCESS,
    /* 22 ITYPE_RECORD_LABEL */
    OTHER_ACCESS,
    /* 23 ITYPE_ANY_QUANTIFIER */
    ACCESS_ANY_QUANTIFIER,
} ;

// Calcul du T_access_entity_type correspodant a l'identificateur
//
//  Cette methode est appliquee a la definition de l'identificateur
//  dont l'occurence qu'on veut verifier est cur_ident.
T_access_entity_type T_ident::make_entity_type(T_ident *cur_ident)
{
   T_machine *cur_machine = cur_ident->get_ref_machine() ;

#ifdef DUMP_TYPES
  TRACE4("T_ident(%x:%s, cur_machine %s, get_ref_machine %s):make_entity_type",
		 this, name->get_value(),
		 cur_machine->get_machine_name()->get_name()->get_value(),
		 get_ref_machine()->get_machine_name()->get_name()->get_value()) ;
#endif

  T_access_entity_type etype = ident_type_to_entity_type_sit[(size_t)ident_type] ;

  T_ident *def = this ;
  while (def->get_def() != NULL)
	{
	  def = def->get_def() ;
	}

#ifdef DUMP_TYPES
  TRACE2("def %x->get_ref_machine() %s",
		 def,
		 def->get_ref_machine()->get_machine_name()->get_name()->get_value()) ;
#endif


  // Enventuellement ici on a besoin de savoir dans le cas d'une
  // constante/variable abstraite dans une rafinement, si elle est
  // raffinee ou pas
  // Pour cela, il faut que l'identificateur soit defini dans une spec
  // ou un raffinement, d'ou le test de def...
  // IL faut aussi que cette abstraction soit une abstraction de la
  // machine en cours d'analyse, i.e. cur_machine soit un raffinement
  // de cette machine
  T_symbol *occ_name = cur_ident->get_name() ;

  if (etype == ACCESS_ABSTRACT_VARIABLE)
	{
	  if (   (cur_machine->find_variable(occ_name) == NULL)
		   && (cur_machine->get_ref_abstraction() != NULL)
		   && (cur_machine->get_ref_abstraction()->find_variable(occ_name) != NULL))
		{
		  etype = ACCESS_UNREFD_ABSTRACT_VARIABLE_FROM_ABS ;
		  TRACE2("La variable abstraite %s n'est pas raffinee ds %s",
				 this->name->get_value(),
				 get_ref_machine()->get_machine_name()
				 ->get_name()->get_value()) ;
		  TRACE0("-> type ACCESS_UNREFD_ABSTRACT_VARIABLE_FROM_ABS") ;
		}
	}
  else if (etype == ACCESS_ABSTRACT_CONSTANT)
	{
	  if (   (cur_machine->find_constant(occ_name) == NULL)
		   && (cur_machine->get_ref_abstraction() != NULL)
		   && (cur_machine->get_ref_abstraction()->find_constant(occ_name) != NULL))
		{
		  etype = ACCESS_UNREFD_ABSTRACT_CONSTANT_FROM_ABS ;
		  TRACE2("La constant abstraite %s n'est pas raffinee ds %s",
				 this->name->get_value(),
				 get_ref_machine()->get_machine_name()
				 ->get_name()->get_value()) ;
		  TRACE0("-> type ACCESS_UNREFD_ABSTRACT_CONSTANT_FROM_ABS") ;
		}
	}

  return etype ;
}

// Recherche de l'operation et de la machine de definition d'un item
void T_ident::find_machine_and_op_def(T_machine **adr_ref_machine,
											   T_op **adr_ref_op)
{
#ifdef DEBUG_IDENT
  TRACE4("T_ident(%x:%s)::find_machine_and_op_def(%x, %x)",
		 this,
		 name->get_value(),
		 adr_ref_machine,
		 adr_ref_op) ;
#endif // DEBUG_IDENT

  // On regarde si on a memorise les valeurs
  if (ref_machine != NULL)
	{
	  if (adr_ref_machine != NULL)
		{
		  *adr_ref_machine = ref_machine ;
		}
	  if (adr_ref_op != NULL)
		{
		  *adr_ref_op = ref_op ;
		}
#ifdef DEBUG_IDENT
  TRACE4("DEJA MEMORISE : T_ident(%x:%s)::find_machine_and_op_def() -> ref_machine %x, ref_op %x)",
		 this,
		 name->get_value(),
		 ref_machine,
		 ref_op) ;
#endif // DEBUG_IDENT
	  return ;
	}

  // Methode generale
  T_item::find_machine_and_op_def(&ref_machine, &ref_op) ;

  if (adr_ref_machine != NULL)
	{
	  *adr_ref_machine = ref_machine ;
	}

  if (adr_ref_op != NULL)
	{
	  *adr_ref_op = ref_op ;
	}

#ifdef DEBUG_IDENT
  TRACE4("T_ident(%x:%s)::find_machine_and_op_def() -> ref_machine %x, ref_op %x)",
		 this,
		 name->get_value(),
		 ref_machine,
		 ref_op) ;
#endif // DEBUG_IDENT
}

// Provoquer une nouvelle recherche de l'operation et de la machine de
// definition d'un item
void T_ident::reset_machine_and_op_def(void)
{
  TRACE1("T_ident(%x)::reset_machine_and_op_def", this) ;
  ref_machine = NULL ;
  ref_op = NULL ;
  find_machine_and_op_def(NULL, NULL) ;
}

// Calcul du T_access_from_context correspodant a l'identificateur
T_access_from_context
T_ident::make_context(T_access_entity_type etype)
{
#ifdef DUMP_TYPES
    TRACE3("T_ident(%s:%s:%s)::make_context()",
           name->get_value(),
           get_ident_type_name(),
           get_ref_machine()->get_machine_name()->get_name()->get_value()) ;
#endif
    T_access_from_context res = FROM_CONSTRAINTS ;
    T_item *cur = get_father() ;
    int pas_trouve = TRUE ;
    // then_found == TRUE si on a trouve un THEN avant
    // le ASSERT, i.e. si on est dans la substitution
    // et pas le predicat d'assert
    int then_found = FALSE ;

    // typing_found permet de déterminer si dans la clause WITNESS, on est à droite ou à gauche de l'égalité
    bool typing_found = false;

    while ((pas_trouve == TRUE) && (cur != NULL))
    {
#ifdef DUMP_TYPES
        TRACE2("cur %x %s", cur, cur->get_class_name()) ;
#endif

        switch (cur->get_node_type())
        {
        case NODE_FLAG :
        {
            // On a trouve un drapeau, donc on va pouvoir connaitre le contexte
            T_lexem *lex = ((T_flag *)cur)->get_ref_lexem() ;
#ifdef DUMP_TYPES
            TRACE1("cur est un drapeau de lexeme associe %s",
                   lex->get_lex_ascii()) ;
#endif

            pas_trouve = FALSE ;
            switch(lex->get_lex_type())
            {
            case L_CONSTRAINTS :
            {
#ifdef DUMP_TYPES
                TRACE0("FROM_CONSTRAINTS") ;
#endif
                res = FROM_CONSTRAINTS ;
                break ;
            }

            case L_THEN :
            {
                // On a trouve un THEN. Interessant pour distinguer
                // la substitution du predicat dans un ASSERT
                // Pour cela on utilise la variable then_found
                // exploitee dans case NODE_ASSERT
                then_found = TRUE ;
                pas_trouve = TRUE ; // pour continuer
                break ;
            }

            case L_INCLUDES :
            case L_EXTENDS :
            case L_IMPORTS :
            {
#ifdef DUMP_TYPES
                TRACE0("FROM_INCL_OR_EXT_OR_IMP") ;
#endif
                res = FROM_INCL_OR_EXT_OR_IMP ;
                break ;
            }

            case L_PROPERTIES :
            {
#ifdef DUMP_TYPES
                TRACE0("FROM_PROPERTIES") ;
#endif
                res = FROM_PROPERTIES ;
                break ;
            }

            case L_ASSERTIONS :
            {
#ifdef DUMP_TYPES
                TRACE0("FROM_INVARIANT_OR_ASSERTIONS") ;
#endif
                res = FROM_INVARIANT_OR_ASSERTIONS ;
                break ;
            }

            case L_VARIANT :
            {
#ifdef DUMP_TYPES
                TRACE0("FROM_VARIANT") ;
#endif
                res = FROM_VARIANT_OR_INVARIANT ;
                break ;
            }

            case L_INVARIANT :
            {
                // Il faut regarder si c'est l'invariant de la machine
                // ou d'une boucle. Pour cela, il suffit de regarder le pere
                if (cur->get_father()->get_node_type() == NODE_MACHINE)
                {
#ifdef DUMP_TYPES
                    TRACE0("FROM_INVARIANT_OR_ASSERTIONS") ;
#endif
                    res = FROM_INVARIANT_OR_ASSERTIONS ;
                }
                else
                {
#ifndef NO_CHECKS
                    assert(cur->get_father()->get_node_type() == NODE_WHILE) ;
#endif
#ifdef DUMP_TYPES
                    TRACE0("FROM_VARIANT_OR_INVARIANT") ;
#endif
                    res = FROM_VARIANT_OR_INVARIANT ;
                }
                break ;
            }

            case L_VALUES :
            {
#ifdef DUMP_TYPES
                TRACE0("FROM_VALUES") ;
#endif
                res = FROM_VALUES ;
                break ;
            }

            case L_INITIALISATION :
            {
                // Substitution, cas general car on n'a pas trouve
                // d'ASSERT ni de INVARIANT ni de VARIANT
#ifdef DUMP_TYPES
                TRACE0("FROM_SUBSTITUTION") ;
#endif
                res = FROM_SUBSTITUTION ;
                break ;
            }

            case L_CONCRETE_CONSTANTS :
            {
                // pour BOM uniquement
                if (get_Use_B0_Declaration() == TRUE)
                {
                    res = FROM_CONCRETE_CONSTANTS;
                }
                else
                {
                    // On ne fait rien !
                    pas_trouve = TRUE ;
                }
                break;

            }

            case L_CONCRETE_VARIABLES :
            {
                // pour BOM uniquement
                if (get_Use_B0_Declaration() == TRUE)
                {
                    res = FROM_CONCRETE_VARIABLES;
                }
                else
                {
                    // On ne fait rien !
                    pas_trouve = TRUE ;
                }
                break;

            }
            default :
            {
                // On ne fait rien !
                pas_trouve = TRUE ;
#ifdef DUMP_TYPES
                TRACE0("rien d'interessant trouve, bizarre ...") ;
#endif
            }
            }

            break ;
        }

        case NODE_OPERATION :
        {
            // Cast justifie par le case
            T_op *op = (T_op *)cur ;

            // Il faut regarder si on est ou pas dans une
            // (specification d') operation locale
            res = (op->get_is_a_local_op() == TRUE)
                    ? FROM_LOCAL_OPERATION
                    : FROM_SUBSTITUTION ;
#ifdef DUMP_TYPES
            TRACE1("%s", (op->get_is_a_local_op() == TRUE)
                   ? "FROM_LOCAL_OPERATION"
                   : "FROM_SUBSTITUTION") ;
#endif
            pas_trouve = FALSE ;
            break ;
        }


        case NODE_ASSERT :
        {
            if (then_found == FALSE)
            {
#ifdef DUMP_TYPES
                TRACE0("FROM_ASSERT_PREDICATE") ;
#endif
                // then_found == TRUE si on a trouve un THEN avant
                // le ASSERT, i.e. si on est dans la substitution
                // et pas le predicat d'assert
                res = FROM_ASSERT_PREDICATE ;
                pas_trouve = FALSE ;
                break ;
            }

            // Sinon, fall into
            break;
        }
        case NODE_LABEL :
        {
            if (then_found == FALSE)
            {
#ifdef DUMP_TYPES
                TRACE0("FROM_LABEL_PREDICATE") ;
#endif
                // then_found == TRUE si on a trouve un THEN avant
                // le ASSERT, i.e. si on est dans la substitution
                // et pas le predicat d'assert
                res = FROM_LABEL_PREDICATE ;
                pas_trouve = FALSE ;
                break ;
            }

            // Sinon, fall into
            break;
        }
        case NODE_WITNESS :
        {
            if (then_found == FALSE && typing_found == true)
            {
#ifdef DUMP_TYPES
                TRACE0("FROM_WITNESS_PREDICATE") ;
#endif
                // then_found == TRUE si on a trouve un THEN avant
                // le WITNESS, i.e. si on est dans la substitution
                // et pas le predicat d'assert
                res = FROM_WITNESS_PREDICATE ;
                pas_trouve = FALSE ;
                break ;
            }

            // Sinon, fall into
            break;
        }
        case NODE_TYPING_PREDICATE :
        {
            if (((T_typing_predicate*) cur)->get_identifiers() == this) {
                typing_found = true;
            }
        }
        default :
        {
            // On ne fait rien par defaut
        }
        }


	  if (pas_trouve == TRUE)
		{
		  // Encore une fois !
#ifdef DUMP_TYPES
		  TRACE0("on passe au pere") ;
#endif
		  cur = cur->get_father() ;
		}
	}

  if (pas_trouve == TRUE)
	{
	  internal_error(this->get_ref_lexem(),
					 __FILE__,
					 __LINE__,
					 get_error_msg(E_UNABLE_TO_FIND_CONTEXT_FOR_IDENT),
					 name->get_value(),
					 get_ident_type_name()) ;
	}

  //  Pour une constante concrete ou un ensemble abstrait, il faut
  //  regarder si on est avant ou apres sa valuation.
  if (res == FROM_VALUES && (etype == ACCESS_CONCRETE_CONSTANT ||
							 etype == ACCESS_ABSTRACT_SET))
	{
	  TRACE0("Cas particulier des valuations");

	  cur = get_father();
	  while (cur != NULL && cur->get_node_type() != NODE_VALUATION)
		{
		  cur = cur->get_father();
		}

      // On a de arriver a une valuation, sinon l'arbre est errone.
	  assert(cur != NULL);
	  T_valuation *cur_valu = (T_valuation *) cur;
	  T_ident *cur_ident = cur_valu->get_ident();

	  if (cur_ident != NULL && cur_ident->get_name() == name)
		{
          TRACE0("On est en train de valuer la donnee.");
		}
	  else
		{
          // On recherche si la donnee a deja ete valuee avant en
          // remontant le chaenage de liste des valuations.
		  cur_valu = (T_valuation *) cur_valu->get_prev();
		  while (cur_valu != NULL)
			{
			  cur_ident = cur_valu->get_ident();
			  if (cur_ident != NULL && cur_ident->get_name() == name)
				{
                  TRACE0("La donnee a ete valuee au prealable.");
				  res = FROM_VALUES_AFTER_VALUATION;
				  break;
				}
			  cur_valu = (T_valuation *) cur_valu->get_prev();
			}
		}
	}

  TRACE0("avant trace");
  TRACE4("T_ident(%x:%s):make_context(%s) -> %s",
		 this,
		 name->get_value(),
		 get_access_entity_type_name(etype),
		 get_access_from_context_name(res)) ;
  TRACE0("apres");
  return res ;
}

// Verifie que les droits obtenus sont suffisants
int T_ident::check_rights(T_ident *def_ident,
								   T_access_authorisation auth)
{
#ifdef DUMP_TYPES
  TRACE2("T_ident::check_rights(%s, d)",
		 def_ident->name->get_value(),
		 auth) ;
#endif


  if ( (def_ident->ident_type == ITYPE_OP_NAME) ||
	   (def_ident->ident_type == ITYPE_LOCAL_OP_NAME) )
	{
	  if (auth == AUTH_ALL_OP)
		{
		  TRACE0("OPERATION AVEC TOUS LES DROITS");
		  return TRUE ;
		}
	  else if (auth == AUTH_READ_OP)
		{
		  TRACE1("OPERATION EN LECTURE SEULE : %s ",
				 def_ident->get_name()->get_value());
		  return def_ident->get_ref_op()->get_body()->is_read_only();
		}
	}

  // Il faut demander au pere ce qu'il veut faire
  T_access_authorisation needed_auth = get_father()->get_auth_request(this);

#ifdef DUMP_TYPES
  TRACE2("CHECK_RIGHTS necessaire %s, obtenue %s",
		 get_access_authorisation_msg(needed_auth),
		 get_access_authorisation_msg(auth)) ;
#endif

  switch (needed_auth)
	{
	case AUTH_DENIED :
	  {
		return FALSE ;
	  }

	case AUTH_READ :
	  {
		return ( (auth == AUTH_READ) || (auth == AUTH_READ_WRITE) ) ? TRUE : FALSE ;
	  }

	case AUTH_WRITE :
	  {
		return ((auth == AUTH_WRITE) || (auth == AUTH_READ_WRITE)) ? TRUE : FALSE ;
	  }

	case AUTH_READ_WRITE :
	  {
		return (auth == AUTH_READ_WRITE) ? TRUE : FALSE ;
	  }

	default :
	  {
		// Dans les autres cas, il faut la meme autorisation
		return (needed_auth == auth) ? TRUE : FALSE ;
	  }
	}

}

// Verification de typage
// Utilise pour verifier qu'un identificateur est type avant d'etre
// accede. Ne fait rien dans le cas general
void T_ident::check_type()
{
#ifdef DUMP_TYPES
  TRACE2("T_ident(%x:%s)::check_type", this, name->get_value()) ;
#endif

  if(get_error_count() != 0)
	{
	  //reprise sur erreur
	  return ;
	}

  if (get_B_type() == NULL && get_warn_ident() == TRUE)
	{
	  semantic_error(this,
					 CAN_CONTINUE,
					 get_error_msg(E_ACCESS_UNTYPED_IDENT),
					 name->get_value()) ;
	}
}


// Donne l'identificateur a resoudre suivant
// Attention, si le suivant a un ident_type != ITYPE_UNKNOWN,
// alors il faut le sauter
// C'est le cas par exemple pour les variables des predicats
// universels, construits tout d'abord en ITYPE_UNKNOW par
// le parseur LR, puis patches en ITYPE_... par le Betree
T_ident *T_ident::get_next_unsolved_ident(void)
{
#ifdef DUMP_TYPES
  TRACE1("T_ident(%x)::get_next_unsolved_ident()", this) ;
#endif
#ifdef DUMP_UNSOLVED
  get_object_manager()->dump("get_next_unsolved_ident") ;
#endif /* DUMP_UNSOLVED */

  if ( 	(next_unsolved_ident == NULL)
		|| (next_unsolved_ident->ident_type == ITYPE_UNKNOWN) )
	{
#ifdef DUMP_TYPES
	  TRACE2("T_ident(%x)::get_next_unsolved_ident() -> %x",
			 this,
			 next_unsolved_ident) ;
#endif
	  return next_unsolved_ident ;
	}
  // Il faut le sauter !
#ifdef DUMP_TYPES
  TRACE1("T_ident(%x)::get_next_unsolved_ident() -> demande au suivant", this) ;
#endif
  return next_unsolved_ident->get_next_unsolved_ident() ;
}

int T_ident::is_an_ident(void)
{
  return TRUE ;
} ;

int T_ident::is_a_set(void)
{
#ifdef DUMP_TYPES
  TRACE2("T_ident(%x:%s)::is_a_set", this, name->get_value()) ;
#endif
  return (def == NULL) ? T_expr::is_a_set() : def->T_expr::is_a_set() ;
}

int T_ident::is_a_relation(void)
{
#ifdef DUMP_TYPES
  TRACE2("T_ident(%x:%s)::is_a_relation", this, name->get_value()) ;
#endif
  return (def == NULL) ? T_expr::is_a_relation() : def->T_expr::is_a_relation() ;
}

int T_ident::is_a_seq(void)
{
#ifdef DUMP_TYPES
  TRACE2("T_ident(%x:%s)::is_a_seq", this, name->get_value()) ;
#endif
  return (def == NULL) ? T_expr::is_a_seq() : def->T_expr::is_a_seq() ;
}

int T_ident::is_a_non_empty_seq(void)
{
#ifdef DUMP_TYPES
  TRACE2("T_ident(%x:%s)::is_a_non_empty_seq", this, name->get_value()) ;
#endif
  return (def == NULL)
	? T_expr::is_a_non_empty_seq() : def->T_expr::is_a_non_empty_seq() ;
}

int T_ident::is_an_integer_set(void)
{
#ifdef DUMP_TYPES
  TRACE2("T_ident(%x:%s)::is_an_integer_set", this, name->get_value()) ;
#endif
  return (def == NULL)
	? T_expr::is_an_integer_set() : def->T_expr::is_an_integer_set() ;
}

int T_ident::is_a_real_set(void)
{
#ifdef DUMP_TYPES
  TRACE2("T_ident(%x:%s)::is_a_real_set", this, name->get_value()) ;
#endif
  return (def == NULL)
        ? T_expr::is_a_real_set() : def->T_expr::is_a_real_set() ;
}

// Verifie qu'une liste d'identificateurs ne comporte pas de doublons.
// Emet un message d'erreur le cas echeant
void check_no_double_definition(T_ident *list_ident)
{
#ifdef DUMP_TYPES
  TRACE1("check_no_double_definition(%x)", list_ident) ;
#endif

  // Verification bete et incrementale pour l'instant
  // Devrait suffir en terme de performances car les listes
  // sont courtes et les comparaisons de symboles optimisees
  T_ident *cur = list_ident ;

  for (;;)
	{
	  // On avance d'un cran
	  cur = (T_ident *)cur->get_next() ;

	  if (cur == NULL)
		{
		  // Fin de la verification
		  return ;
		}

	  T_symbol *ref = cur->get_name() ;
	  T_ident *check = list_ident ;

	  while (check != cur)
		{
		  if (check->get_name()->compare(ref) == TRUE)
			{
			  // Redefinition
			  semantic_error(cur,
							 CAN_CONTINUE,
							 get_error_msg(E_IDENT_CLASH),
							 ref->get_value(),
							 cur->get_ident_type_name(),
							 check->get_ident_type_name()) ;
			  semantic_error_details(check,
									 get_error_msg(E_IDENT_CLASH_OTHER_LOCATION)) ;

              // Ajout dans l'anneau de collage pour ne pas begayer
			  cur->insert_in_glued_ring(check) ;
			}

		  check = (T_ident *)check->get_next() ;
		}
	}

}

// Verifie qu'une liste d'items ne comporte pas deux fois le meme
// identficateur.
// Emet un message d'erreur le cas echeant
void check_no_double_ident(T_item *list_item)
{
  TRACE1("--> check_no_double_ident(%p)", list_item);

  T_item *check_item = list_item ;
  while (check_item != NULL && check_item->get_next() != NULL)
	{
      // Tant qu'on est pas sur le dernier element de la liste.
	  if (check_item->is_an_ident())
		{
		  T_symbol *check_name = check_item->make_ident()->get_name();

		  T_item *cur_item = check_item->get_next();
		  while (cur_item != NULL)
			{
			  if (cur_item->is_an_ident())
				{
				  if (check_name->compare(cur_item->make_ident()->get_name()))
					{
                      /* les identificateurs sont les memes */
					  semantic_error(cur_item,
									 CAN_CONTINUE,
									 get_error_msg(E_DOUBLE_REF),
									 check_name->get_value());
					}
				}
			  cur_item = cur_item->get_next();
			}
		}
      check_item = check_item->get_next();
	}
  TRACE1("<-- check_no_double_ident(%p)", list_item);
}


// Est-ce une constante/variable ?
// Par defaut, NON
// OUI ssi classe T_ident avec le bon ident_type
int T_ident::is_a_constant(int ask_def)
{
  if ( (def != NULL) && (ask_def == TRUE) )
	{
	  // On demande au def
	  return def->is_a_constant() ;
	}

  if ( 	  (ident_type == ITYPE_CONCRETE_CONSTANT)
	   || (ident_type == ITYPE_ABSTRACT_CONSTANT) )
	{
	  return TRUE ;
	}

  // Par defaut : non
  return FALSE ;
}
int T_ident::is_a_variable(int ask_def)
{
  if ( (def != NULL) && (ask_def == TRUE) )
	{
	  // On demande au def
	  return def->is_a_variable() ;
	}

  if ( 	  (ident_type == ITYPE_CONCRETE_VARIABLE)
	   || (ident_type == ITYPE_ABSTRACT_VARIABLE) )
	{
	  return TRUE ;
	}

  // Par defaut : non
  return FALSE ;
}

// Constructeur de clonage
T_literal_enumerated_value::T_literal_enumerated_value
(T_literal_enumerated_value *ref_evalue,
 T_item **adr_first,
 T_item **adr_last,
 T_item *new_father)
  : T_ident(ref_evalue,
			adr_first,
			adr_last,
			new_father)
{
#ifdef DUMP_TYPES
  TRACE2("T_literal_enumerated_value(%x)::T_literal_enumerated_value(ref=%x)",
		 this,
		 ref_evalue) ;
#endif


  // Reste a cloner le champ specifique ...
  value = ref_evalue->value ;

  // .. et a corriger le node type
  set_node_type(NODE_LITERAL_ENUMERATED_VALUE) ;
}

T_B0_type *T_ident::get_B0_type(void)
{
  TRACE2("T_ident(%x:%s)::get_B0_type", this,
		 get_name() ? get_name()->get_value() : "(null)") ;
  if (def != NULL)
	{
	  return def->get_B0_type() ;
	}

  return B0_type ;
}

void T_ident::set_B0_type(T_B0_type *new_B0_type)
{
  TRACE2("T_ident(%x)::set_B0_type(%x)", this, new_B0_type) ;
  if (new_B0_type == NULL)
	{
	  return ;
	}

  if (def != NULL)
	{
	  TRACE1("appel recursif sur def=%x", def) ;
	  def->set_B0_type(new_B0_type) ;
	}
  else
	{
	  B0_type = new_B0_type ;
	  B0_type->link() ;
	  TRACE2("%x a pose un lien sur %x", this, B0_type) ;
	}
}


// Renvoie l'identificateur "contenu" dans this
// i.e. this pour la classe T_ident et sous-classes
//      this->object pour T_list_link qui contient un ident, ...
// prerequis : is_an_ident == TRUE
T_ident *T_ident::make_ident(void)
{
  return this ;
}

// Parcours de tous les get_def jusqu'a avoir la definition
T_ident *T_ident::get_definition(void)
{
  T_ident *cur = this ;

  for (;;)
	{
	  T_ident *glued = cur->get_ref_glued() ;

	  if (glued != NULL)
		{
		  cur = glued  ;
		}
	  else
		{
		  T_ident *defi = cur->get_def() ;

		  if (defi != NULL)
			{
			  cur = defi ;
			}
		  else
			{
			  return cur ;
			}
		}
	}
}

//
// Test si l'ident est un type
//
int T_ident::is_a_BOM_type(void)
{
  TRACE1("T_ident(%s)::is_a_BOM_type",name->get_value());
  int result = FALSE;
  if (ident_type == ITYPE_UNKNOWN)
    {
      result = def->is_a_BOM_type();
    }
  else
	{
	  switch (ident_type)
		{
		case ITYPE_CONCRETE_CONSTANT:
		  ASSERT(BOM_lexem_type != NULL);

		  if (BOM_lexem_type->get_lex_type() == L_EQUAL)
			{
			  result = TRUE;
			}
		  break;
		case ITYPE_ABSTRACT_SET:
		case ITYPE_ENUMERATED_SET:
		  result = TRUE;
		  break;
		case ITYPE_BUILTIN:
		  if (get_builtin_BOOL()->get_name() == name)
			{
			  result = TRUE;
			}
		  break;
		default:
		  result = FALSE;
		}
	}
  TRACE2("<--(%s)is_a_BOM_type == %s",
		 name->get_value(),
		 result == TRUE?"true":"false");
  return result;
}
//
// Test si l'ident est un tableau
// c'est un tableau si et seulement si sont type est un tableau
//
int T_ident::is_an_array(void)
{
  int result = FALSE;
  if (ident_type == ITYPE_UNKNOWN)
    {
      result = def->is_an_array();
    }
  else
	{
	  ASSERT(BOM_type != NULL);
	  if (BOM_type->is_an_ident())
		{
		  result =  BOM_type->make_ident()->is_an_array_type();
		}
	}
  return result;
}

//
// Test si l'identificateur est un operateur. Un operateur n'est pas
// un type et la valeur du BOM type est une relation
//

int T_ident::is_an_operator(void)
{
  TRACE1("T_ident(%s)::is_an_operator()", name->get_value());
  int result = FALSE;
  if (ident_type == ITYPE_UNKNOWN)
    {
      result = def->is_an_operator();
    }
  else if (is_a_BOM_type() == FALSE
		   &&
		   BOM_type != NULL
		   &&
		   BOM_type->get_node_type() == NODE_RELATION)
    {
      result = TRUE;
    }
  TRACE1("%s<--T_ident()::is_an_operator()",
	 result == TRUE ? "true" : "false");
  return result;
}

//
// Un ident est un type tableau ssi c'est un type et la valeur est une relation
//
int T_ident::is_an_array_type(void)
{
  TRACE1("T_iden(%s)::is_an_array_type", name->get_value());
  int result = FALSE;
  if (ident_type == ITYPE_UNKNOWN)
	{
	  result = def->is_an_array_type();
	}
  else if (is_a_BOM_type() == TRUE
		  &&
		  BOM_type != NULL
		  &&
		  BOM_type->get_node_type() == NODE_RELATION)
	{
	  result = TRUE;
	}
  return result;
}

//
// Fonction qui renvoie le type de destination
// N'a de sens que pour un type tableau ou une operation
//

T_ident *T_ident::get_dst_type(void)
{
  TRACE1("T_ident(%s)::get_dst_type", name->get_value());
  T_ident *result = NULL;
  if (ident_type == ITYPE_UNKNOWN)
	{
	  result = def->get_dst_type();
	}
  else
	{
	  ASSERT(BOM_type != NULL
			 &&
			 BOM_type->get_node_type() == NODE_RELATION);

	  T_expr *expr_result = ((T_relation *)BOM_type)->get_dst_set();
	  ASSERT(expr_result->is_an_ident());
	  result = expr_result->make_ident();
	}
  TRACE1("%s<--get_dst_type()",
		 result == NULL ? "null" :result->get_name()->get_value()) ;
  return result;
}

//
// Fonction d'acces au premier ensemble d'un operateur unaire ou
// binaire
//
T_ident *T_ident::get_src1_type(void)
{
  TRACE1("T_ident(%s)::get_src1_type", name->get_value());
  T_ident *result = NULL;
  if (ident_type == ITYPE_UNKNOWN)
	{
	  result = def->get_src1_type();
	}
  else
	{
	  ASSERT(BOM_type != NULL
			 &&
			 BOM_type->get_node_type() == NODE_RELATION);

	  T_expr *expr_result = ((T_relation *)BOM_type)->get_src_set();

	  if (expr_result->is_an_ident() == TRUE)
		{
		  result = expr_result->make_ident();
		}
	  else if (expr_result->get_node_type() == NODE_BINARY_OP)
		{
		  T_binary_op *bin_op = (T_binary_op *)expr_result;
		  ASSERT(bin_op->get_op1()->is_an_ident() == TRUE);
		  result = bin_op->get_op1()->make_ident();
		}
	}
  TRACE1("%s<--get_src1_type()",
		 result == NULL ? "null" :result->get_name()->get_value()) ;
  return result;
}

//
// Fonction d'acces au deuxieme ensemble source d'un
// operateur binaire ou unaire.
// Dans le cas d'un operateur unaire la methode renvoie NULL
//
T_ident *T_ident::get_src2_type(void)
{
  TRACE1("T_ident(%s)::get_dst_type", name->get_value());
  T_ident *result = NULL;
  if (ident_type == ITYPE_UNKNOWN)
	{
	  result = def->get_src2_type();
	}
  else
	{
	  ASSERT(BOM_type != NULL
			 &&
			 BOM_type->get_node_type() == NODE_RELATION);

	  T_expr *expr_result = ((T_relation *)BOM_type)->get_src_set();

	  if (expr_result->get_node_type() == NODE_BINARY_OP)
		{
		  T_binary_op *bin_op = (T_binary_op *)expr_result;
		  ASSERT(bin_op->get_op2()->is_an_ident() == TRUE);
		  result = bin_op->get_op2()->make_ident();
		}
	}
  TRACE1("%s<--get_src2_type()",
		 result == NULL ? "null" :result->get_name()->get_value()) ;
  return result;
}
//GP 22/01/99
//Fonction qui verifie que le type string est correctement utilise
//is_a_belong_pre doit etre appele avec TRUE si l'indent est type
//avec le predicat ':'
void T_ident::check_string_uses(int is_a_belong_pre)
{
    // Initialisation du gestionnaire de projets, si ce n'est deja fait
    init_project_manager() ;

    // Chargement du projet
    T_project *project = get_project_manager()->get_current() ;

    int isValidationProject = 0;

    if (project != NULL) {
        isValidationProject = project->isValidation() ;
        if (isValidationProject)
            return;
    }
    else
    {
        if(T_project::get_default_project_type() == PROJECT_VALIDATION)
        {
            return;
        }
    }

    T_type* ident_type = get_B_type() ;

  if(ident_type == NULL)
	{
	  //reprise sur erreur
	  TRACE1("T_ident(%x)::check_string_uses : reprise sur erreur", this) ;
	  return ;
	}

  if(ident_type->is_based_on_string() == FALSE)
	{
	  //Pas de type STRING en vu, rien a verifier ici
	  return ;
	}

  TRACE0("IL Y A DU STRING") ;

  if(ident_type->get_node_type() != NODE_PREDEFINED_TYPE)
	{
	  //C'est du string complique: INTERDIT
	  semantic_error(this,
					 CAN_CONTINUE,
					 get_error_msg(E_STRING_TYPE_ONLY_FOR_OP_INPUT_PARAM)) ;

	  return ;
	}

  TRACE0("STRING UTILISE EN TYPE DE BASE") ;

  // ICI on est sur d'avoir affaire a du STRING

  if(is_a_belong_pre == FALSE)
	{
	  //STRING uniquement autorise avec ':',
	  semantic_error(this,
					 CAN_CONTINUE,
					 get_error_msg(E_STRING_TYPE_ONLY_FOR_OP_INPUT_PARAM)) ;

	  return ;
	}

  //On cherche la definition de l'ident
  T_ident* this_def = this ;

  while(this_def->get_def() != NULL)
	{
	  this_def = this_def->get_def() ;
	}

  if(this_def->get_ident_type() == ITYPE_UNKNOWN)
	{
	  TRACE0("PANIC ITYPE_UNKNOWN") ;
	}

  if(this_def->get_ident_type() != ITYPE_OP_IN_PARAM)
	{
	  //DESOLE, STRING SEULEMENT AUTORISE POUR LES ITYPE_OP_IN_PARAM
	  semantic_error(this,
					 CAN_CONTINUE,
					 get_error_msg(E_STRING_TYPE_ONLY_FOR_OP_INPUT_PARAM)) ;

	}
  else
	{
	  TRACE0("Pas de pb c'est bien du ITYPE_OP_IN_PARAM") ;
    }
}


// Retourne un symbole constitue de PREFIX suivi de '$' et de
// SUFFIX_NUMBER sous forme decimale.
extern T_symbol *
add_suffix_to_name(T_symbol *prefix,
				   size_t suffix_number)
{
  // Verifie que le numero est valide
  ASSERT(suffix_number < 7777) ;

  // Construction du nouveau symbole.
  size_t new_len = prefix->get_len() + 1 + 4 + 1 ;

  //  NEW_NAME contient a minima PREFIX suivi de '$' SUFFIX_NUMBER et '\0'
  // et SUFFIX_NUMBER ne fait pas plus de 4 caracteres.
  char *new_name = new char[new_len] ;
  (void) sprintf(new_name, "%s$%d", prefix->get_value(), (int) suffix_number) ;
  T_symbol *new_symb = lookup_symbol(new_name) ;
  delete [] new_name ;
  return new_symb ;
}


void T_ident::set_suffix_number(size_t new_suffix_number)
{
  if (prefix == NULL)
	{
      // Indique que la variable est maintenant numerotee
	  prefix = name ;
	}

  // Construction du nouveau symbole.
  T_symbol *new_symb = add_suffix_to_name(prefix, new_suffix_number) ;

  TRACE2("T_ident::set_suffix_number avant '%s', new_suffix_number = '%d'",
		 name->get_value(),
		 new_suffix_number) ;
  TRACE1("T_ident::set_suffix_number apres '%s'", new_symb->get_value()) ;
  name = new_symb ;
  suffix_number = new_suffix_number ;
}


void T_ident::unset_suffix_number(void)
{
  if (prefix != NULL)
	{
	  name = prefix ;
	  prefix = NULL ;
	}
}


// Retourne la partie situee apres le dernier '.' dans le symbole SYMB
// sous la forme d'un nouveau symbole.
static T_symbol *
get_component_name_from_name(T_symbol *symb)
{
  const char *name = symb->get_value() ;
  const char *dot_place = strrchr(name, '.') ;

  // Le nom de l'identificateur comporte un '.' de renommage
  ASSERT(dot_place != NULL) ;

  return lookup_symbol(dot_place + 1) ;
}


void T_renamed_ident::set_suffix_number(size_t new_suffix_number)
{
  // Appel de la methode de la superclasse.
  T_ident::set_suffix_number(new_suffix_number) ;

  component_name = get_component_name_from_name(get_name()) ;
}


void T_renamed_ident::unset_suffix_number(void)
{
  // Appel de la methode de la superclasse.
  T_ident::unset_suffix_number() ;

  component_name = get_component_name_from_name(get_name()) ;
}


// Retourne un symbole forme de la concatenation de deux symboles
// donnes, eventuellement separes par un caractere '.' si DELETE_DOT
// vaut FALSE..
static T_symbol *concat_symbols(T_symbol *left,
										 T_symbol *right,
										 int delete_dot)
{
  TRACE3("-> concat_symbols('%s', '%s', %s)",
		 left ? left->get_value() : "(null)",
		 right ? right->get_value() : "(null)",
		 delete_dot == TRUE ? "sans '.'" : "avec '.'");

  T_symbol *result;

  if (left == NULL)
	{
	  result = right;
	}
  else if (right == NULL)
	{
	  result = left;
	}
  else
	{
	  size_t len = left->get_len() + (delete_dot == FALSE) + right->get_len();
	  char *buf = new char[len + 1];
	  const char *format = delete_dot == FALSE ? "%s.%s" : "%s%s";
	  sprintf(buf, format, left->get_value(), right->get_value());
	  result = lookup_symbol(buf, len);
	  delete[] buf;
	}

  TRACE1("<- concat_symbols '%s'", result ? result->get_value() : "(null)");
  return result;
}

// Supprime tous les caracteres '.' dans la chaene passee en parametre
static void suppress_dots(char *name)
{
  TRACE1("-> suppress_dots (%s)", name);
  char *dst = name;
  for (char *src = name; *src; ++src)
	{
	  if (*src != '.')
		{
		  *dst++ = *src;
		}
	}
  *dst = '\0';
  TRACE1("<- suppress_dots (%s)", name);
} // suppress_dots

// Retourne le chemin de renommage d'un identificateur en omettant
// eventuellement les caracteres '.' de renommage.
T_symbol *T_ident::get_prefix(int delete_dot,
									   T_used_machine *ref_context)
{

  TRACE3("T_ident::get_prefix(%x, %d, %x)", this, delete_dot, ref_context);

  T_machine *machine = get_ref_machine();
  T_symbol *result = NULL;

  if (machine != NULL)
	{
	  T_used_machine *context = machine->get_context();
	  if (context != NULL && context != ref_context)
		{
		  if (context->get_use_type() == USE_SEES)
			{
              // Cas particulier pour une machine vue: Le prefixe est
              // entierement donne par la clause de visibilite.  Par
			  // exemple, si context contient "r1.r2.mach" alors le
              // prefixe est "r1.r2".
			  T_symbol *full_name = context->get_name()->get_name();

			  // Suppression du dernier composant du nom (i.e., le nom
			  // de la machine).  On ne peut pas utiliser
			  // "get_instance_name()" car celui-ci ne rendrait que
			  // "r1" dans l'exemple ci-dessus.
			  char *copy = new char[full_name->get_len() + 1];
			  strcpy(copy, full_name->get_value());
			  char *last_dot = strrchr(copy, '.');
			  char *prefix;
			  if (last_dot != NULL)
				{
				  prefix = copy;
				  *last_dot = '\0';

                  // Suppression eventuelle des autres '.' de renommage
				  if (delete_dot == TRUE) suppress_dots(prefix);

                  // Ici, on ne connait pas precisement la longueur de la
                  // chaene.
				  result = lookup_symbol(prefix);
				}
			  else
				{
				  result = NULL;
				}
			  delete [] copy;
			}
		  else
			{
              // Cas general: machine incluse, etendue ou utilisee.
			  T_symbol *left = context->get_name()->get_prefix(delete_dot,
															   ref_context);
			  T_symbol *right = context->get_instance_name();
			  result = concat_symbols(left, right, delete_dot);
			}
		}
	}
  TRACE3("T_ident::get_prefix (%s, %s) --> %s",
		 name->get_value(),
		 delete_dot == TRUE ? "sans '.'" : "avec '.'",
		 result == NULL ? "NULL" : result->get_value());
  return result;
}


// Retourne le symbole correspondant au nom complet d'un
// identificateur (y compris les prefixes de renommage) avec ou sans
// les caracteres '.' de renommage.
T_symbol *T_ident::get_full_name(int delete_dot,
										  T_used_machine *ref_context)
{
  TRACE3("T_ident::get_full_name(%x, %d, %x)", this, delete_dot, ref_context);

  ////////////////////////////////////////////////////////////////
  //
  //  ATTENTION:
  //
  //  ANO 2724 : LV, le 25/01/2001:
  //
  //  J'ai ajoute le renommage des parametres formels de machine, bien
  //  que ce ne soit pas fait par le TC actuel (celui livre dans
  //  l'Atelier 3.6).  En effet, ce renommage etait necessaire pour
  //  que le BOILER soit accepte par le bcomp en mode ``conflit
  //  cache''.
  //
  //  On est alors dans une situation bancale dans la mesure oe le
  //  BCOMP fait des tests d'identificateur avec des noms differents
  //  de ceux utilises par le TC.  Cette situation desagreable est
  //  temporaire et sera corrigee des l'introduction du GNF dans
  //  l'Atelier B.
  //
  ////////////////////////////////////////////////////////////////

  // Puis, ajout eventuel du prefiwe
  T_ident *def = get_definition();
  T_symbol *result;
  if ((def->get_ident_type() == ITYPE_CONCRETE_VARIABLE ||
	   def->get_ident_type() == ITYPE_ABSTRACT_VARIABLE ||
	   def->get_ident_type() == ITYPE_MACHINE_SET_FORMAL_PARAM ||
	   def->get_ident_type() == ITYPE_MACHINE_SCALAR_FORMAL_PARAM ||
	   def->get_ident_type() == ITYPE_OP_NAME))
	{
	  result = concat_symbols(def->get_prefix(delete_dot, ref_context),
							  def->get_name(),
							  delete_dot);
	}
  else
	{
	  result = def->get_name();
	}

  TRACE4("T_ident::get_full_name (%s, %s, %s) --> %s",
		 name->get_value(),
		 delete_dot ? "sans '.'" : "avec '.'",
		 (ref_context != NULL ?
		  ref_context->get_pathname()->get_value()
		  : "(null)"),
		 result->get_value());
  return result;
} // T_ident::get_full_name


//
//	}{	Fin du fichier
//
