add_executable(complex_task main.cpp)
target_link_libraries(complex_task PRIVATE Threads::Threads)
set_target_properties(complex_task PROPERTIES FOLDER "ComplexTasks")
