# Produces files ${DST} from file ${SRC} with
# @GIT_REPO_SHA@ replaced with `git rev-parse HEAD`
# @GIT_DESCRIBE_TAG@ replaced with `git describe --tags`
# assuming ${SRC} is under version control in a git repository
#

if(GIT_EXECUTABLE)
  get_filename_component(SRC_DIR ${SRC} DIRECTORY)
  # Generate a git-rev-parse SHA from Git repository
  execute_process(
    COMMAND ${GIT_EXECUTABLE} rev-parse HEAD
    WORKING_DIRECTORY "${SRC_DIR}"
    OUTPUT_VARIABLE GIT_REPO_SHA
    RESULT_VARIABLE GIT_SHA_ERROR_CODE
    OUTPUT_STRIP_TRAILING_WHITESPACE
    )
  if(GIT_SHA_ERROR_CODE)
    set(GIT_REPO_SHA "00000000")
  endif()
  # Generate a git-describe tag string from Git repository
  execute_process(
    COMMAND ${GIT_EXECUTABLE} describe --tags
    WORKING_DIRECTORY "${SRC_DIR}"
    OUTPUT_VARIABLE GIT_DESCRIBE_TAG
    RESULT_VARIABLE GIT_DESCRIBE_ERROR_CODE
    OUTPUT_STRIP_TRAILING_WHITESPACE
    )
  if(GIT_DESCRIBE_ERROR_CODE)
    set(GIT_DESCRIBE_TAG "unset")
  endif()
endif()

# Final fallback: Just use a bogus sha string and warn to the developer.
if(NOT DEFINED GIT_REPO_SHA)
  message(WARNING "Failed to determine GIT_REPO_SHA from Git. Using default version \"${GIT_REPO_SHA}\".")
endif()
if(NOT DEFINED GIT_DESCRIBE_TAG)
  message(WARNING "Failed to determine GIT_DESCRIBE_TAG from Git. Using default version \"${GIT_DESCRIBE_TAG}\".")
endif()

configure_file(${SRC} ${DST} @ONLY)
