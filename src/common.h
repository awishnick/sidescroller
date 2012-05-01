#pragma once

#ifdef __clang__
  #define PUSH_ALL_WARNINGS \
    _Pragma("clang diagnostic push") \
    _Pragma("clang diagnostic ignored \"-Wall\"") \
    _Pragma("clang diagnostic ignored \"-Wextra\"")
  #define POP_ALL_WARNINGS \
    _Pragma("clang diagnostic pop")
#else
  #define PUSH_ALL_WARNINGS
  #define POP_ALL_WARNINGS
#endif

