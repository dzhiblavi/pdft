include(FetchContent)

set(FETCHCONTENT_UPDATES_DISCONNECTED TRUE)

set(SAF_BUILD_EXAMPLES OFF)
set(SAF_BUILD_TESTING OFF)

FetchContent_Declare(
  saf
  GIT_REPOSITORY https://github.com/dzhiblavi/saf.git
  GIT_TAG main
  GIT_PROGRESS TRUE
  GIT_SHALLOW TRUE
  EXCLUDE_FROM_ALL)

add_compile_definitions(SAF_ASIO_STANDALONE)

FetchContent_MakeAvailable(saf)
