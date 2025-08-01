add_library(TCA9535
    src/TCA9535.cpp
)

set(TCA9535_INC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/inc")
target_include_directories(TCA9535 PUBLIC ${TCA9535_INC_DIR})

target_link_libraries(TCA9535 PUBLIC
    ${HAL_LIBS}
)

