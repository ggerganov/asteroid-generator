function(cg_add_source_file TARGET_NAME SOURCE_NAME)
    set(${TARGET_NAME}_SOURCE_LIST ${${TARGET_NAME}_SOURCE_LIST}
        ${SOURCE_NAME} PARENT_SCOPE)
endfunction()

function(cg_add_program PROGRAM_NAME)
    set(PROGRAM_TARGET ${PROGRAM_NAME})
    message(STATUS "Creating program '${PROGRAM_NAME}' with sources:")
    foreach(source ${ARGN})
        message(STATUS "   ${source}")
        cg_add_source_file(${PROGRAM_NAME} ${source})
    endforeach()

    add_executable(${PROGRAM_TARGET} ${${PROGRAM_NAME}_SOURCE_LIST})
    target_link_libraries(${PROGRAM_TARGET}
        ${OPENGL_LIBRARIES}
        ${GLUT_LIBRARIES}
        ${CMAKE_THREAD_LIBS_INIT}
        )
    install(TARGETS ${PROGRAM_TARGET} DESTINATION ${PROJECT_SOURCE_DIR}/bin)
endfunction()

include_directories(".")

cg_add_program("asteroid-generator"
    main.cpp
    cg_camera.cpp
    cg_config.cpp
    cg_filters.cpp
    cg_frame_buffer.cpp
    cg_geometry.cpp
    cg_geometry_height_map.cpp
    cg_go_asteroid.cpp
    cg_go_death_star.cpp
    cg_io.cpp
    cg_opengl.cpp
    cg_perlin.cpp
    cg_raytracer.cpp
    cg_sky.cpp
    cg_timer.cpp
    cg_utils.cpp
    cg_vec3.cpp
    )
