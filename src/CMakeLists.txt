add_subdirectory(connection)
add_subdirectory(user_io)
add_subdirectory(client)

find_package(Threads REQUIRED)
target_link_libraries(GameClient PUBLIC Connection UserIO Threads::Threads)

