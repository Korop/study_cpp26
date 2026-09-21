# Automatically discover and add all single-file tasks
file(GLOB SINGLE_FILE_TASKS "*.cpp")
list(FILTER SINGLE_FILE_TASKS EXCLUDE REGEX "/utils\\.cpp$")
foreach(task_file ${SINGLE_FILE_TASKS})
    get_filename_component(target_name ${task_file} NAME_WE)
    add_executable(${target_name} ${task_file})
    target_link_libraries(${target_name} PRIVATE Threads::Threads)
    
    # Organize them in an IDE folder if generated
    set_target_properties(${target_name} PROPERTIES FOLDER "SimpleTasks")
endforeach()

target_sources(cppcore_task PRIVATE utils.cpp)

# Automatically discover and add all complex tasks (subdirectories with CMakeLists.txt)
file(GLOB children RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/*)
foreach(child ${children})
    if(child STREQUAL "gcc" AND NOT CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        continue()
    endif()

    if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${child})
        if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${child}/CMakeLists.txt)
            add_subdirectory(${child})
        endif()
    endif()
endforeach()
