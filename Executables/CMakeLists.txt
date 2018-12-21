add_executable(markovsim
  ${CMAKE_CURRENT_SOURCE_DIR}/markovsim.cpp
)

target_link_libraries(markovsim
  uiiitmarkov
  uiiitsupport
  ${GLOG}
)

