add_executable(markovsim
  ${CMAKE_CURRENT_SOURCE_DIR}/markovsim.cpp
)

target_link_libraries(markovsim
  uiiitmarkov
  uiiitsupport
  ${GLOG}
)

add_executable(sfm-latency
  ${CMAKE_CURRENT_SOURCE_DIR}/sfm-latency.cpp
)

target_link_libraries(sfm-latency
  uiiitsupport
  ${GLOG}
)

add_executable(sfm-provisioning
  ${CMAKE_CURRENT_SOURCE_DIR}/sfm-provisioning.cpp
)

target_link_libraries(sfm-provisioning
  uiiitsupport
  ${GLOG}
)

