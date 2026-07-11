set(SOURCES
    app/Application.cpp

    core/Message.cpp
    core/TokenEstimator.cpp
    core/ConversationBuffer.cpp
    core/Session.cpp
    core/SessionStore.cpp
    core/ConversationManager.cpp

    network/StreamParser.cpp
    network/NetworkWorker.cpp
    network/OllamaClient.cpp

    ui/MainWindow.cpp
    ui/SessionListWidget.cpp
    ui/ChatWidget.cpp
    ui/MessageListWidget.cpp
    ui/MessageBubble.cpp
    ui/StreamingTextBrowser.cpp
    ui/InputWidget.cpp
    ui/SettingsDialog.cpp
    ui/CodeHighlighter.cpp

    utils/JsonHelper.cpp
    utils/AppSettings.cpp
)

set(HEADERS
    app/Application.h

    core/Message.h
    core/TokenEstimator.h
    core/ConversationBuffer.h
    core/Session.h
    core/SessionStore.h
    core/ConversationManager.h

    network/StreamParser.h
    network/NetworkWorker.h
    network/OllamaClient.h

    ui/MainWindow.h
    ui/SessionListWidget.h
    ui/ChatWidget.h
    ui/MessageListWidget.h
    ui/MessageBubble.h
    ui/StreamingTextBrowser.h
    ui/InputWidget.h
    ui/SettingsDialog.h
    ui/CodeHighlighter.h

    utils/JsonHelper.h
    utils/AppSettings.h
)

set(RESOURCES
    ${CMAKE_SOURCE_DIR}/resources/resources.qrc
)

# Static library for sharing between main exe and tests
add_library(DesktopAgentLib STATIC
    ${SOURCES}
    ${HEADERS}
)

target_include_directories(DesktopAgentLib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(DesktopAgentLib PUBLIC
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Network
)

# Main executable
add_executable(DesktopAgent
    main.cpp
    ${RESOURCES}
)

target_link_libraries(DesktopAgent PRIVATE
    DesktopAgentLib
)

if(WIN32)
    set_target_properties(DesktopAgent PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
endif()
