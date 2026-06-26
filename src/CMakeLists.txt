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

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    find_package(X11 REQUIRED)
    find_package(Fontconfig REQUIRED)
    target_link_libraries(carTrainerSystem PRIVATE
        ${X11_LIBRARIES}
        ${X11_Xft_LIB}
        ${X11_Xrender_LIB}
        ${X11_Xfixes_LIB}
        ${X11_Xcursor_LIB}
        ${X11_Xinerama_LIB}
        Fontconfig::Fontconfig
    )
endif()
