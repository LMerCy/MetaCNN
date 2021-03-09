project(MetaCNN)
set(CMAKE_CXX_STANDARD 17)

include_directories(
        "${CMAKE_CURRENT_SOURCE_DIR}"
)
add_library(MetaCNN
        MetaCNN/facilities/var_type_dict.h)

add_executable(test 
        test/facilities/test_var_type.cpp)

target_link_libraries(test MetaCNN)