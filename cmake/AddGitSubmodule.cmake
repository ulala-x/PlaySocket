function(add_git_submodule relative_dir)
    find_package(Git REQUIRED)

    set(FULL_DIR ${CMAKE_SOURCE_DIR}/${relative_dir})

    if (NOT EXISTS ${FULL_DIR}/CMakeLists.txt)
        execute_process(COMMAND ${GIT_EXECUTABLE}
            submodule update --init --recursive -- ${relative_dir}
            WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
    endif()

    if (EXISTS ${FULL_DIR}/CMakeLists.txt)
        message("Submodule is CMake Project: ${FULL_DIR}/CMakeLists.txt")
        add_subdirectory(${FULL_DIR})
    else()
        message("Submodule is NO CMake Project: ${FULL_DIR}")
    endif()
endfunction(add_git_submodule)


function(add_git_submodule_with)
    set(options )
    set(oneValueArgs GIT_TAG RELATIVE_DIR CMD PARAM)
    cmake_parse_arguments(ADD_GIT_SUBMODULE "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    # 매개변수를 변수로 할당
    set(git_tag ${ADD_GIT_SUBMODULE_GIT_TAG})
    set(relative_dir ${ADD_GIT_SUBMODULE_RELATIVE_DIR})
    set(cmd ${ADD_GIT_SUBMODULE_CMD})
    set(param ${ADD_GIT_SUBMODULE_PARAM})


    find_package(Git REQUIRED)

    set(FULL_DIR ${CMAKE_SOURCE_DIR}/${relative_dir})

    if (NOT EXISTS ${FULL_DIR}/CMakeLists.txt)
        execute_process(COMMAND ${GIT_EXECUTABLE}
            submodule update --init --recursive -- ${relative_dir}
            WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
    endif()

    if(git_tag)
        execute_process(COMMAND ${GIT_EXECUTABLE}
            checkout ${git_tag}
            WORKING_DIRECTORY ${FULL_DIR})
    endif()

        # USER_COMMAND 호출
    if (cmd AND param)
        message(STATUS "user command ${cmd} ${param}")
        message(STATUS "dir: ${FULL_DIR}")

        execute_process(COMMAND ${cmd}
            ${param}
            WORKING_DIRECTORY ${FULL_DIR}
            RESULT_VARIABLE result
        )

        message("Command executed successfully.")

        if(result EQUAL 0)
            message("Command executed successfully.")
        else()
            message("Command execution failed with error code: ${result}")
        endif()

    endif()

    if (EXISTS ${FULL_DIR}/CMakeLists.txt)
        message("Submodule is CMake Project: ${FULL_DIR}/CMakeLists.txt")
        add_subdirectory(${FULL_DIR})
    else()
        message("Submodule is NO CMake Project: ${FULL_DIR}")
    endif()


endfunction(add_git_submodule_with)

function(post_cpm)
    set(options )
    set(oneValueArgs PACKAGE CMD PARAM)
    cmake_parse_arguments(POST_CPM "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    # 매개변수를 변수로 할당
    set(relative_dir ${POST_CPM_PACKAGE})
    set(cmd ${POST_CPM_CMD})
    set(param ${POST_CPM_PARAM})

    find_package(Git REQUIRED)

    set(FULL_DIR "${${relative_dir}_SOURCE_DIR}")

    # if (NOT EXISTS ${FULL_DIR}/CMakeLists.txt)
    #     execute_process(COMMAND ${GIT_EXECUTABLE}
    #         submodule update --init --recursive -- ${relative_dir}
    #         WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
    # endif()

        # USER_COMMAND 호출
    if (cmd AND param)
        message(STATUS "user command ${cmd} ${param}")
        message(STATUS "dir: ${FULL_DIR}")

        execute_process(COMMAND ${cmd}
            ${param}
            WORKING_DIRECTORY ${FULL_DIR}
            RESULT_VARIABLE result
        )

        message("Command executed successfully.")

        if(result EQUAL 0)
            message("Command executed successfully.")
        else()
            message("Command execution failed with error code: ${result}")
        endif()

    endif()

    if (EXISTS ${FULL_DIR}/CMakeLists.txt)
        message("Submodule is CMake Project: ${FULL_DIR}/CMakeLists.txt")
        add_subdirectory(${FULL_DIR})
    else()
        message("Submodule is NO CMake Project: ${FULL_DIR}")
    endif()


endfunction(post_cpm)
