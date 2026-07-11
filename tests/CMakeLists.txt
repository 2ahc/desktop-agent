set(CMAKE_AUTOMOC ON)

function(add_unit_test test_name test_source)
    add_executable(${test_name} ${test_source})
    target_include_directories(${test_name} PRIVATE
        ${CMAKE_SOURCE_DIR}/src
    )
    target_link_libraries(${test_name} PRIVATE
        DesktopAgentLib
        Qt6::Core
        Qt6::Test
    )
    add_test(NAME ${test_name} COMMAND $<TARGET_FILE:${test_name}>)
endfunction()

add_unit_test(TestTokenEstimator TestTokenEstimator.cpp)
add_unit_test(TestConversationBuffer TestConversationBuffer.cpp)
add_unit_test(TestStreamParser TestStreamParser.cpp)
add_unit_test(TestSessionStore TestSessionStore.cpp)
