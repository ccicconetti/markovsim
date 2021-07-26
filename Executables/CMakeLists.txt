add_executable(markovsim
  ${CMAKE_CURRENT_SOURCE_DIR}/markovsim.cpp
)

target_link_libraries(markovsim
  uiiitmarkov
  uiiitsupport
  ${GLOG}
)

add_executable(sfm
  ${CMAKE_CURRENT_SOURCE_DIR}/sfm.cpp
)

target_link_libraries(sfm
  uiiitmarkov
  uiiitsupport
  ${GLOG}
)

