#ifndef MUDUO_NOCOPYABLE_H
#define MUDUO_NOCOPYABLE_H

namespace Muduo
{


//继承本类就是不能拷贝的
class nocopyable
{
public:
    nocopyable(const nocopyable&) = delete;
    void operator=(const nocopyable&) = delete;
protected:
    nocopyable() = default;
    ~nocopyable() = default;
};

}



#endif