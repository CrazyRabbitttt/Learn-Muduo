#ifndef BING_BASE_BOCOPYABLE_H
#define BING_BASE_BOCOPYABLE_H

namespace bing {
class nocopyable {
 public:
    nocopyable(const nocopyable&) = delete;
    void operator=(const nocopyable&) = delete;

 protected:
    nocopyable() = default;
    ~nocopyable() = default;
};

}     //namespace bing 
#endif