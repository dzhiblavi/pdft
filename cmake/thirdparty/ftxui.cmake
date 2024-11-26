include(FetchContent)

set(FETCHCONTENT_UPDATES_DISCONNECTED TRUE)

FetchContent_Declare(
  ftxui
  GIT_REPOSITORY https://github.com/ArthurSonzogni/ftxui
  GIT_TAG v5.0.0
  GIT_PROGRESS TRUE
  GIT_SHALLOW TRUE
  EXCLUDE_FROM_ALL)

FetchContent_MakeAvailable(ftxui)
