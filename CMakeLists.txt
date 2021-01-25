cmake_minimum_required(VERSION 3.1.0)
project(server VERSION 0.1.0)


add_compile_options(-Wall -D_POSIX_C_SOURCE=200809L -lm)


add_executable(naive_server bin/naive_server.c 
    src/utils.c src/utils.h
    src/server.c src/server.h
)
target_include_directories(naive_server PRIVATE src)
set_property(TARGET naive_server PROPERTY C_STANDARD 11)
target_link_libraries(naive_server m)

# add_executable(poll_server bin/poll_server.c 
#     src/reqh.c src/reqh.h 
#     src/utils.c src/utils.h
#     src/server.c src/server.h
#     
# )
# target_include_directories(poll_server PRIVATE src)
# set_property(TARGET poll_server PROPERTY C_STANDARD 11)
# target_link_libraries(poll_server m)

# add_executable(select_server bin/select_server.c 
#     src/reqh.c src/reqh.h 
#     src/utils.c src/utils.h
#     src/server.c src/server.h
#     
# )
# target_include_directories(select_server PRIVATE src)
# set_property(TARGET select_server PROPERTY C_STANDARD 11)
# target_link_libraries(select_server m)

add_executable(epoll_server bin/epoll_server.c 
    src/reqh.c src/reqh.h 
    src/utils.c src/utils.h
    src/server.c src/server.h
)
target_include_directories(epoll_server PRIVATE src)
set_property(TARGET epoll_server PROPERTY C_STANDARD 11)
target_link_libraries(epoll_server m)


add_executable(client bin/client.c 
    src/utils.c src/utils.h
)
target_include_directories(client PRIVATE src)
set_property(TARGET client PROPERTY C_STANDARD 11)
target_link_libraries(client m)


add_executable(talker bin/talker.c 
    src/utils.c src/utils.h
)
target_include_directories(talker PRIVATE src)
set_property(TARGET talker PROPERTY C_STANDARD 11)

add_executable(listener bin/listener.c 
    src/utils.c src/utils.h
    src/server.c src/server.h
)
target_include_directories(listener PRIVATE src)
set_property(TARGET listener PROPERTY C_STANDARD 11)


set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
