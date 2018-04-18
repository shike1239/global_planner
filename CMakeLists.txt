cmake_minimum_required(VERSION 2.8.3)
project(global_planner)

## C++11
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall")
elseif(COMPILER_SUPPORTS_CXX0X)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else()
    message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()

find_package(catkin REQUIRED
  COMPONENTS
    angles
    costmap_2d
    dynamic_reconfigure
    geometry_msgs
    nav_core
    navfn
    nav_msgs
    pluginlib
    roscpp
    tf
    base_local_planner
)

generate_dynamic_reconfigure_options(
  cfg/GlobalPlanner.cfg
)

catkin_package(
  INCLUDE_DIRS include
  LIBRARIES ${PROJECT_NAME}
  CATKIN_DEPENDS
    costmap_2d
    dynamic_reconfigure
    geometry_msgs
    nav_core
    navfn
    nav_msgs
    pluginlib
    roscpp
    tf
    pcl
)


## OPEN MOTION PLANNING LIBRARY
find_package(OMPL REQUIRED)

if(NOT OMPL_FOUND)
    message(AUTHOR_WARNING,"Open Motion Planning Library not found")
endif(NOT OMPL_FOUND)


include_directories(
  include
  ${catkin_INCLUDE_DIRS}
)
include_directories(
  include
  ${OMPL_INCLUDE_DIRS}
)
include_directories(
  include
  include
)
add_library(${PROJECT_NAME}
  src/quadratic_calculator.cpp
  src/dijkstra.cpp
  src/astar.cpp
  src/grid_path.cpp
  src/gradient_path.cpp
  src/orientation_filter.cpp
  src/planner_core.cpp
  src/hybrid_astar/algorithm.cpp
  src/hybrid_astar/bucketedqueue.cpp
  src/hybrid_astar/collisiondetection.cpp
  src/hybrid_astar/dubins.cpp
  src/hybrid_astar/dynamicvoronoi.cpp
  src/hybrid_astar/main.cpp
  src/hybrid_astar/node2d.cpp
  src/hybrid_astar/node3d.cpp
  src/hybrid_astar/path.cpp
  src/hybrid_astar/planner.cpp
  src/hybrid_astar/smoother.cpp
  src/hybrid_astar/tf_broadcaster.cpp
  src/hybrid_astar/visualize.cpp
  include/global_planner/astar.h
  include/global_planner/dijkstra.h
  include/global_planner/expander.h
  include/global_planner/gradient_path.h
  include/global_planner/grid_path.h
  include/global_planner/orientation_filter.h
  include/global_planner/planner_core.h
  include/global_planner/potential_calculator.h
  include/global_planner/quadratic_calculator.h
  include/global_planner/traceback.h
  include/global_planner/hybrid_astar/algorithm.h
  include/global_planner/hybrid_astar/bucketedqueue.h
  include/global_planner/hybrid_astar/collisiondetection.h
  include/global_planner/hybrid_astar/constants.h
  include/global_planner/hybrid_astar/dubins.h
  include/global_planner/hybrid_astar/dynamicvoronoi.h
  include/global_planner/hybrid_astar/gradient.h
  include/global_planner/hybrid_astar/helper.h
  include/global_planner/hybrid_astar/lookup.h
  include/global_planner/hybrid_astar/node2d.h
  include/global_planner/hybrid_astar/node3d.h
  include/global_planner/hybrid_astar/path.h
  include/global_planner/hybrid_astar/planner.h
  include/global_planner/hybrid_astar/point.h
  include/global_planner/hybrid_astar/smoother.h
  include/global_planner/hybrid_astar/vector2d.h
  include/global_planner/hybrid_astar/visualize.h
)
add_dependencies(${PROJECT_NAME} ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})
target_link_libraries(${PROJECT_NAME} ${catkin_LIBRARIES})
target_link_libraries(${PROJECT_NAME} ${OMPL_LIBRARIES})


add_executable(planner
  src/plan_node.cpp 
)
add_dependencies(planner ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})
target_link_libraries(planner
  ${PROJECT_NAME}
  ${catkin_LIBRARIES}
)

install(TARGETS ${PROJECT_NAME} planner
  ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
  LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
  RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION})

install(DIRECTORY include/${PROJECT_NAME}/
  DESTINATION ${CATKIN_PACKAGE_INCLUDE_DESTINATION}
  PATTERN ".svn" EXCLUDE)

install(FILES bgp_plugin.xml
  DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION})