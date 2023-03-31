find_package(fmt REQUIRED)

set(JSON_BuildTests OFF CACHE INTERNAL "")
add_subdirectory(third-party/json)

add_executable(ingredientParser main.cpp parser.hpp)

target_link_libraries(ingredientParser PRIVATE nlohmann_json::nlohmann_json fmt::fmt)

set(JSON_INPUT_FILE ${CMAKE_CURRENT_SOURCE_DIR}/ingredients.json)
set(JSON_OUTPUT_FILE ${CMAKE_CURRENT_BINARY_DIR}/ingredients.hpp)

add_custom_command(
        OUTPUT ${JSON_OUTPUT_FILE}
        COMMAND ingredientParser "${JSON_INPUT_FILE}" "${JSON_OUTPUT_FILE}"
        DEPENDS ingredientParser
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/ingredient.hpp DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/ingredients.json DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

add_custom_target(
        generate_header ALL DEPENDS ${JSON_OUTPUT_FILE}
)