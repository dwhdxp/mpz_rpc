aux_source_directory(. SRC_LIST)
# 制作动态库mpzrpc
add_library(mpzrpc SHARED ${SRC_LIST})