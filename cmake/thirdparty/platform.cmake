include(FetchContent)

set(FETCHCONTENT_UPDATES_DISCONNECTED TRUE)

FetchContent_Declare(
  platform
  GIT_REPOSITORY git@github.com:dzhiblavi/platform.git
  GIT_TAG main
  GIT_PROGRESS TRUE
  GIT_SHALLOW TRUE
  EXCLUDE_FROM_ALL)

FetchContent_MakeAvailable(platform)
