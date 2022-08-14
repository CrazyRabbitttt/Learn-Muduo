#include "Learn-Muduo/Net/Buffer.h"

#include <errno.h>
#include <sys/uio.h>
#include <unistd.h>

using namespace bing;

/*
从fd进行数据的读取， 本epoll采用的是LT模式， 没读完就一直通知
buffer有大小，从tcp读取数据不知道多少？
*/


ssize_t Buffer::readFd(int fd, int* saveErrno) {
    char extrabuf[65535] = {0};     //栈空间，用来进行帮助读取数据
    // iovec, readv进行分散读， 相当好的一种思路
    struct iovec vec[2];
    size_t writeable = writeableBytes();
    // 1. 缓冲区一、首先第一部分是要写到buf中
    vec[0].iov_base = begin() + writeIdx_;
    vec[0].iov_len  = writeable;

    // 2. 缓冲区二、如果说buffer中的位置是不够的，暂时使用栈空间的，最多128K
    vec[1].iov_base = extrabuf;
    vec[1].iov_len  = sizeof extrabuf;

    /*
        是否使用栈空间作为第二个缓冲区？ 
        如果buffer空间>=64K, 就不需要使用栈空间啦
    */ 
    const int iocnt = (writeable < sizeof extrabuf) ? 2 : 1;

   // 使用readv进行分散读，将数据读到缓冲区中
    const size_t n = ::readv(fd, vec, iocnt);
    if (n < 0) {
        *saveErrno = errno;
    } else if (n <= writeable) {            // 压根就没有用到extra, buffer空间就足够了
        writeIdx_ += n;     
    } else {                                // 使用了extra的空间，
        // extra中有数据，将数据写到buffer中
        writeIdx_ = buffer_.size();         // 填满了
        append(extrabuf, n - writeable);    // 不够的话会进行扩容的
    }
}


ssize_t Buffer::writeFd(int fd, int* saveErrno) {
    ssize_t n = ::write(fd, peek(), readableBytes());
    if (n < 0) {
        *saveErrno = errno;
    }
    return n;
}


