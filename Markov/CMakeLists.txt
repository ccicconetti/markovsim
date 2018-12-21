add_library(uiiitmarkov SHARED
  ${CMAKE_CURRENT_SOURCE_DIR}/mmc.cpp
)

target_link_libraries(uiiitmarkov
  uiiitsupport
  ${GLOG}
)
