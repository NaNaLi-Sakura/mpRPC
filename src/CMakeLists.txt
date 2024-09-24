# 当前目录的所有文件 => 源文件列表
aux_source_directory(. SRC_LIST)

# 生成静态库
add_library(mprpc STATIC ${SRC_LIST})
# 库文件生成所需链接的库
target_link_libraries(mprpc muduo_net muduo_base pthread)
target_link_libraries(mprpc /usr/local/lib/libzookeeper_mt.so)
