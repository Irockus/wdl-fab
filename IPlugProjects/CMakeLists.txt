include (../cmake/IPlugProject.inc)

# Get the list of all directories under this current directory
get_subdir_list(SUBDIRS ${CMAKE_CURRENT_SOURCE_DIR})

# Automatically discover and add new user projects here !

foreach(subdir ${SUBDIRS})
    add_subdirectory(${subdir})
endforeach()
