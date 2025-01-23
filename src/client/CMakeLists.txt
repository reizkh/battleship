add_executable(GameClient battleship_game.cpp client.cpp grid.cpp guest_client.cpp host_client.cpp)
set_target_properties(GameClient PROPERTIES
                      RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin/"
                      OUTPUT_NAME "game_client")
