./server  192.168.220.128 10001

telnet 192.168.220.128 10001


使用 writev 发送多个内存块，

struct iovec  用于保存内存

iovec.iov_base
iovec.iov_len
