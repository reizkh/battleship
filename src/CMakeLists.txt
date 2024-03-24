add_subdirectory(connection)
add_subdirectory(user_io)
add_subdirectory(client)

target_link_libraries(GameClient PUBLIC Connection UserIO)
