add_library(core_lib
    core/Vehicle.cpp
    core/Student.cpp
    core/Schedule.cpp
    core/TimeUtils.cpp
    core/Manager.cpp
    db/Database.cpp
)
target_include_directories(core_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(core_lib PUBLIC SQLite::SQLite3)

add_executable(carTrainerSystem
    main.cpp
    ui/AppWindow.cpp
)
target_include_directories(carTrainerSystem PRIVATE ${FLTK_INCLUDE_DIR})
target_link_libraries(carTrainerSystem PRIVATE core_lib ${FLTK_LIBRARIES})
