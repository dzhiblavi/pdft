include(FetchContent)
find_package(asio REQUIRED)

set(FETCHCONTENT_UPDATES_DISCONNECTED TRUE)

set(BOOST_SAM_STANDALONE ON)

FetchContent_Declare(
  sam
  GIT_REPOSITORY https://github.com/dzhiblavi/sam.git
  GIT_TAG master
  GIT_PROGRESS TRUE
  GIT_SHALLOW TRUE
  EXCLUDE_FROM_ALL)

FetchContent_MakeAvailable(sam)
target_link_libraries(boost_sam PUBLIC asio::asio)
add_compile_definitions(BOOST_SAM_STANDALONE)
