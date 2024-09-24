# set(SRC_LIST ./userservice.cc ../user.pb.cc)
set(SRC_LIST ./friendservice.cc ../friend.pb.cc)

# 生成可执行程序
add_executable(provider ${SRC_LIST})

# 链接库
target_link_libraries(provider mprpc protobuf)