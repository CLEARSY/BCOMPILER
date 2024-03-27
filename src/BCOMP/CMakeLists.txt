# CMake project file for compiling the B compiler
project(BCOMP)

set(BCOMP_LIB_ACTION_NG_SOURCES
  m_lexsta.cpp m_synsta.cpp m_gcheck.cpp m_action.cpp m_sign.cpp m_actsta.cpp
  m_comsta.cpp
  )

set(BCOMP_LIB_SOURCES
  c_trace.cpp c_port.cpp c_object.cpp c_lex.cpp c_pralex.cpp
  c_memory.cpp c_cleanup.cpp c_io.cpp
  c_lexana.cpp c_lexstk.cpp
  c_item.cpp c_chain.cpp c_msg.cpp c_out.cpp c_api.cpp c_betree.cpp
  c_subst.cpp c_subblk.cpp c_subato.cpp c_expr.cpp c_gethostname.cpp c_ident.cpp
  c_parexp.cpp c_litexp.cpp c_varexp.cpp c_binexp.cpp c_unexp.cpp
  c_mach.cpp c_umach.cpp c_op.cpp c_valu.cpp c_relati.cpp c_char.cpp
  c_bltin.cpp c_base.cpp c_defini.cpp c_flag.cpp c_commen.cpp
  c_lstlnk.cpp c_usedef.cpp c_usecre.cpp c_cata.cpp c_symb.cpp c_pred.cpp
  c_chain2.cpp c_meta.cpp c_uop.cpp c_bmana.cpp c_blist.cpp
  c_tstamp.cpp c_objman.cpp c_type.cpp c_clntyp.cpp c_namtyp.cpp
  c_equtyp.cpp c_cmptyp.cpp c_genop.cpp c_umsg.cpp c_dollar.cpp
  c_bound.cpp c_path.cpp c_pragma.cpp c_projdb.cpp c_b0type.cpp
  c_env.cpp c_clone.cpp c_record.cpp c_md5.cpp c_skel.cpp

  s_lex.cpp s_pralex.cpp s_synta.cpp s_lr.cpp s_pstack.cpp
  s_lrtab.cpp s_defini.cpp s_expr.cpp s_pred.cpp s_subst.cpp s_mach.cpp
  s_op.cpp s_misc.cpp s_subblk.cpp
  v_toplvl.cpp v_colli.cpp v_hcolli.cpp v_compo.cpp v_ldreq.cpp
  v_aux.cpp v_proj.cpp v_op.cpp v_find.cpp
  v_import.cpp v_visi.cpp v_glued.cpp v_subst.cpp v_modif.cpp

  i_toplvl.cpp i_type.cpp i_aux.cpp i_check.cpp i_listid.cpp
  i_globdat.cpp i_typchk.cpp i_valchk.cpp i_lstind.cpp i_subst.cpp
  i_decl.cpp i_expr.cpp i_op.cpp i_pred.cpp

  t_misc.cpp t_expr.cpp t_opres.cpp t_pred.cpp t_subst.cpp
  t_binop.cpp t_op.cpp t_locop.cpp t_record.cpp t_ident.cpp

  d_misc.cpp
  d_dskman.cpp d_dskobj.cpp d_dskstr.cpp

  h_misc.cpp h_expr.cpp h_pred.cpp h_subst.cpp h_bmana.cpp
  h_type.cpp  h_b0type.cpp
  f_file.cpp
  m_api.cpp m_compo.cpp m_file.cpp m_msg.cpp
  m_pmana.cpp m_projec.cpp m_state.cpp m_clean.cpp

  tinyxml.cpp tinyxmlparser.cpp tinyxmlerror.cpp tinystr.cpp
  )

set(BCOMP_LIB_ACTION_NG_HEADERS
  m_lexsta.h m_synsta.h m_action.h
  m_gcheck.h m_sign.h m_actsta.h m_comsta.h
  )

set(BCOMP_LIB_HEADERS
    c_io.h c_memory.h c_cleanup.h c_object.h c_chain.h
    c_betree.h c_mach.h c_item.h c_symb.h c_secure.h
    c_op.h c_subst.h c_trace.h c_pred.h
    c_expr.h c_lstlnk.h c_commen.h
    c_flag.h c_defini.h c_umach.h c_gethostname.h c_ident.h c_port.h
    c_base.h c_bltin.h c_char.h c_lex.h c_pralex.h c_lexstk.h
    c_valu.h c_relati.h c_usedef.h c_usecre.h c_msg.h c_out.h c_api.h
    c_nodtyp.h c_lextyp.h c_exptyp.h c_llktyp.h
    c_pretyp.h c_subtyp.h c_cata.h
    c_chain2.h c_meta.h c_uop.h c_bmana.h c_blist.h
    c_tstamp.h c_objman.h c_type.h c_genop.h
    c_bound.h c_path.h c_pragma.h c_projdb.h c_b0type.h c_env.h
    c_record.h
    c_md5.h
    v_vistbl.h v_toplvl.h v_visi.h
    i_aux.h i_listid.h i_globdat.h i_ctx.h i_lstind.h
    i_aux.h i_listid.h i_globdat.h i_ctx.h
    t_ident.h
    f_file.h
    m_api.h m_compo.h m_file.h m_msg.h
    m_pmana.h m_projec.h m_state.h m_clean.h
    m_lextyp.h
    t_locop.h
    tinyxml.h tinystr.h
    version.h
)

add_custom_target(BCOMP_VERSION
  ${CMAKE_COMMAND}
    -D SRC=${CMAKE_CURRENT_SOURCE_DIR}/version.h.in
    -D DST=${CMAKE_CURRENT_SOURCE_DIR}/version.h
    -D GIT_EXECUTABLE=${GIT_EXECUTABLE}
    -D PROJECT_NAME=${PROJECT_NAME}
    -P ${BCOMPILER_SOURCE_DIR}/cmake/macros/GenerateVersionHeader.cmake
  DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/version.h.in
  BYPRODUCTS
    ${CMAKE_CURRENT_SOURCE_DIR}/version.h
    )

include_directories(${BCOMP_SOURCE_DIR}
	${RESOURCES_SOURCE_DIR}
	${STUB_DECOMPILER_SOURCE_DIR})

set(BCOMP_SOURCES bcomp.cpp c_auto_test.cpp)

set(BCOMP_NLS_PACKAGE bcomp)
set(BCOMP_NLS_LOCALEDIR ../../translations/locale)

option(ENABLE_NLS "Native Language Support" ON)

if(ENABLE_NLS)
   set (BCOMP_NLS_ENABLED TRUE)

   if (BCOMP_NLS_ENABLED)
      find_program (MSGFMT_EXECUTABLE msgfmt)
      if (NOT MSGFMT_EXECUTABLE)
         message (STATUS "msgfmt not found! Internationalization aborted.")
         set (BCOMP_NLS_ENABLED FALSE)
      endif (NOT MSGFMT_EXECUTABLE)
   endif (BCOMP_NLS_ENABLED)

else (ENABLE_NLS)
   set (BCOMP_NLS_ENABLED FALSE)
endif (ENABLE_NLS)

if (BCOMP_NLS_ENABLED)
   add_subdirectory (po)
   message (STATUS "Native language support enabled.")
else (BCOMP_NLS_ENABLED)
   message (STATUS "Native language support disabled.")
endif (BCOMP_NLS_ENABLED)

add_definitions(-D__BCOMP__ -DB0C)
if (BCOMP_NLS_ENABLED)
	add_definitions(-DBCOMP_NLS_ENABLED)
	add_definitions(-DBCOMP_NLS_PACKAGE=\"${BCOMP_NLS_PACKAGE}\")
	add_definitions(-DBCOMP_NLS_LOCALEDIR=\"${BCOMP_NLS_LOCALEDIR}\")
endif (BCOMP_NLS_ENABLED)

include_directories(${ICU_INCLUDE})

add_library(BCOMP_LIB
  STATIC ${BCOMP_LIB_SOURCES} ${BCOMP_LIB_HEADERS})
set_target_properties(BCOMP_LIB PROPERTIES PREFIX "lib")
set_target_properties(BCOMP_LIB PROPERTIES OUTPUT_NAME "Bcomp")

add_executable(BCOMP_EXE ${BCOMP_SOURCES})
set_target_properties(BCOMP_EXE	PROPERTIES OUTPUT_NAME "bcomp")
add_dependencies(BCOMP_EXE BCOMP_VERSION)

target_link_libraries(BCOMP_EXE 
    -Bstatic BCOMP_LIB RESOURCES_LIB STUB_DECOMP_LIB
    -Bdynamic ${ICU_LIBRARIES} ${CMAKE_DL_LIBS} ${WINMM})
