#ifndef InputEventCodes_hpp
#define InputEventCodes_hpp

// Hela does not invent, but uses same key namings as Linux kernel for events passed to user code

#if defined(HELA_LINUX)
  #ifdef __cplusplus
    extern "C" {
  #endif
    #include <linux/input-event-codes.h>
  #ifdef __cplusplus
    }
  #endif
#else
  // https://github.com/torvalds/linux/blob/master/include/uapi/linux/input-event-codes.h
  #include "InputEventCodesLinux.hpp"
#endif

#define HELA_RELEASE  0
#define HELA_PRESS    1
#define HELA_REPEAT   2

#endif // InputEventCodes_hpp
