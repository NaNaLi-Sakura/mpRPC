# set(SRC_LIST ./calluserservice.cc ../user.pb.cc)
set(SRC_LIST ./callfriendservice.cc ../friend.pb.cc)

# 生成可执行程序
add_executable(consumer ${SRC_LIST})

# 链接库
target_link_libraries(consumer mprpc protobuf)