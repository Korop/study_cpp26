add_executable(complex_task main.cpp)
target_link_libraries(complex_task PRIVATE Threads::Threads)
set_target_properties(complex_task PROPERTIES FOLDER "ComplexTasks")

add_executable(variant2_task alternative_imlementation2_task.cpp)
target_link_libraries(variant2_task PRIVATE Threads::Threads)
set_target_properties(variant2_task PROPERTIES FOLDER "ComplexTasks")