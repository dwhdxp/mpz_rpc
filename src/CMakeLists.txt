# aux_source_directory(. SRC_LIST)

set (SRC_LIST mpzrpcapplication.cc mpzrpcconfig.cc rpcheader.pb.cc rpcprovider.cc mpzrpcchannel.cc)
# 制作静态库mpzrpc
add_library(mpzrpc ${SRC_LIST})
# 需要依赖的库文件
target_link_libraries(mpzrpc muduo_net muduo_base pthread)