
sendfile : 只适用于 socket 之间的文件拷贝

零拷贝 避免内核缓冲区和用户缓冲区之间的数据拷贝，

./server 192.168.220.128 10001 client.cpp

telnet  192.168.220.128 10001

sendfile : infd 必须是真实的 文件，不可以是 socket 句柄 管道文件，
	   outfd 只能是 socket 句柄，
	   offset 读入流的位置，
	   count 为 infd 和 outfd 直接传输的字节数
