#pragma once

#include <iostream>
#include <memory>

#define SHARED_PTR(cls) public: \
                                using Ptr = std::shared_ptr<cls>; \
                                template<class... Args> \
                                inline static Ptr New(Args... args) \
                                { \
                                    return std::make_shared<cls>(std::forward<Args>(args)...); \
                                } \

#define SHARED_PTR_STRUCT(st) using Ptr = std::shared_ptr<st>; \
                              inline static Ptr New() \
                              { \
                                    return std::make_shared<st>(); \
                              }
