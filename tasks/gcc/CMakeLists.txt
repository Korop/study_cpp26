add_executable(reflection_gcc_task reflection_gcc_task.cpp)
target_link_libraries(reflection_gcc_task PRIVATE Threads::Threads)
target_compile_options(reflection_gcc_task PRIVATE -freflection)
set_target_properties(reflection_gcc_task PROPERTIES FOLDER "GccTasks")

add_executable(generator_task generator_task.cpp)
target_link_libraries(generator_task PRIVATE Threads::Threads)
target_compile_options(generator_task PRIVATE -freflection)
set_target_properties(generator_task PROPERTIES FOLDER "GccTasks")

add_executable(yield_task yield_task.cpp)
target_link_libraries(yield_task PRIVATE Threads::Threads)
target_compile_options(yield_task PRIVATE -freflection)
set_target_properties(yield_task PROPERTIES FOLDER "GccTasks")
 
# add_executable(variant2_task alternative_imlementation2_task.cpp)
# target_link_libraries(variant2_task PRIVATE Threads::Threads)
# set_target_properties(variant2_task PROPERTIES FOLDER "GccTasks")