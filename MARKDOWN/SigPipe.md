
### SIGPIPE产生的原因
    > SIGPIPE产生的原因是这样的：如果一个 socket 在接收到了 RST packet 之后，程序仍然向这个 socket 写入数据，那么就会产生SIGPIPE信号。
　　> 这种现象是很常见的，譬如说，当 client 连接到 server 之后，这时候 server 准备向 client 发送多条消息，但在发送消息之前，client 进程意外奔溃了，那么接下来 server 在发送多条消息的过程中，就会出现SIGPIPE信号。

### 解决
    > 一般来说我们可以选择进行忽略
