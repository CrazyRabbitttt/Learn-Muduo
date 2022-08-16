#ifndef BING_NET_BUFFER_H_
#define BING_NET_BUFFER_H_



#include <vector>
#include <string>
#include <algorithm>


/*
            缓冲区的模型：
 +-------------------+------------------+------------------+
 | prependable bytes |  readable bytes  |  writable bytes  |
 |                   |     (CONTENT)    |                  |
 +-------------------+------------------+------------------+
 |                   |                  |                  |
 0      <=      readerIndex   <=   writerIndex    <=     size

*/

/*
    1.预留了8字节的空间，序列化的时候方便在首部操作字节
*/

namespace bing {
 
 class Buffer {
  public:
    static const size_t kcheapPrepend = 8;          // 前置的一直保留的8字节
    static const size_t kInitialSize = 1024;        // 数据部分的长度，初始化为1024

    explicit Buffer(size_t initialSize = kInitialSize)
        : buffer_(kcheapPrepend + initialSize),
          readIdx_(kcheapPrepend),
          writeIdx_(kcheapPrepend)
          {}
    
        // 可读的部分的长度
        size_t readableBytes() const {
            return writeIdx_ - readIdx_;
        }
        // 可写部分的长度
        size_t writeableBytes() const {
            return buffer_.size() - writeIdx_;
        }

        // read指针之前的部分
        size_t prependanleBytes() const {
            return readIdx_;
        }

        // 返回可以进行读的位置的指针
        const char* peek() const {
            return begin() + readIdx_;
        }

        // 将可读部分全部取出来，ReturnAsString
        std::string retrieveAllAsString() {
            return retieveAsString(readableBytes());
        }

        // 实际操作：读数据、更新指针
        std::string retieveAsString(size_t len) {
            std::string res(peek(), len);           // 将数据读出来
            //下面需要更换指针的位置
            retrieve(len);
            return res;
        } 

        //读了len之后对指针进行移动
        void retrieve(size_t len) {  
            if (len < readableBytes()) {            // 没读完，移动readIdx_
                readIdx_ += len;        
            } else {                                // 全部都读完了
                retrieveAll();                      // 将指针复位到原来的位置
            }
        } 

        // 将指针复位到原来的位置可读的区域是0
        void retrieveAll() {
            readIdx_ = writeIdx_ = kcheapPrepend;
        }

        // 确保缓冲区能够写内容
        void ensureWriteableBytes(size_t len) {
            if (writeableBytes() < len) {
                makeSpace(len);
            }
        }

        // 添加数据（写）
        void append(const char* data, size_t len) {
            ensureWriteableBytes(len);      // 保证一下数据有空间可以进行写
            std::copy(data, data + len, beginwritePtr());   // 将数据写到可写的部分去
            writeIdx_ += len;
        }


        // 返回可写部分的指针
        char* beginwritePtr() {
            return begin() + writeIdx_;
        }

        const char* beginwritePtr() const {
            return begin() + writeIdx_;
        }

        // 从fd上面读取数据
        ssize_t readFd(int fd, int* saveErrno);

        // 向fd上写数据
        ssize_t writeFd(int fd, int* saveErrno);



  private:
    //获得vector的指针
    char* begin() {
        return & (*buffer_.begin());
    }

    const char* begin() const {
        return &*buffer_.begin();
    }

    // 给出空间：扩容，缩减一下（移动下位置）
    void makeSpace(size_t len) {
        // 如果所有的空闲位置都是放不下了，直接进行扩容
        if (writeableBytes() + prependanleBytes() < len + kcheapPrepend) {
            buffer_.resize(writeIdx_ + len);
        } else {        // 通过调整指针的位置， 缩减前面的
            size_t readable = readableBytes();
            // 将可读区域向前移动， 空出来给可写的区域
            std::copy(begin() + readIdx_, begin() + writeIdx_, 
                            begin() + kcheapPrepend);
            readIdx_  = kcheapPrepend;
            writeIdx_ = readIdx_ + readable; 
        }

    }

    std::vector<char> buffer_;      // 缓冲区，使用vector进行扩容操作
    size_t readIdx_;                // 可写区域的起始指针
    size_t writeIdx_;               // 可读区域的起始指针

 };

}       //namespace bing 
#endif