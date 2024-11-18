include(FetchContent)

FetchContent_Declare(
  fswatch_ext
  GIT_REPOSITORY https://github.com/emcrisostomo/fswatch.git
  GIT_TAG 1.17.1
  GIT_PROGRESS TRUE
  INSTALL_COMMAND "")

FetchContent_GetProperties(fswatch_ext)
FetchContent_MakeAvailable(fswatch_ext)
