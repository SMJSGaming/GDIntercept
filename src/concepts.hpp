#include <cocos2d.h>

template<typename T>
concept Node = std::is_base_of_v<cocos2d::CCNode, T>;