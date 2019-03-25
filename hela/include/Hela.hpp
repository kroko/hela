#ifndef Hela_hpp
#define Hela_hpp

#if defined(__clang__)
#elif defined(__GNUC__) || defined(__GNUG__)
#elif defined(_MSC_VER)
#endif

namespace hela {

  #define HELA_HELA

  // GNU/Linux
  #if (defined(linux) || defined(__linux) || defined(__linux__)) && ! defined(__ANDROID__)
    #define HELA_LINUX
    #if defined(__arm__)
      #define HELA_LINUX_ARM
      #if !defined(HELA_LINUX_ARM_FBDEV_MALI_ALLWINNER)
        #pragma message("It seems you are on GNU/Linux and on ARM. Assuming you are on Allwinner(?) You forgot to build with HELA_LINUX_ARM_FBDEV_MALI_ALLWINNER flag, I will asume GNU/Linux x64.")
        // What kind of person cross compiles on another ARM for Allwinner? ^_^
        #pragma message("If you are not on Allwinner then I am not tested even once on other SoCs running GNU/Linux. But if you are croscompiling on another ARM for Allwinner set HELA_LINUX_ARM_FBDEV_MALI_ALLWINNER")
      #endif
    #else
      #if defined(CROSS_COMPILE) || (defined(ARCH) && (ARCH == arm))
        #if !defined(HELA_LINUX_ARM_FBDEV_MALI_ALLWINNER)
          #pragma message("If you try to crosscompile on x64 for Allwinner then set HELA_LINUX_ARM_FBDEV_MALI_ALLWINNER nevertheless.")
        #endif
      #else
        #pragma message("I am not tested even once on GNU/Linux x64, only GNU/Linux ARM on Allwinner (apparently, as it is the sole purpose of me). Are you trying to crosscompile aigainst Allwinner's ARM libMali.so? If so then please fix above logic to detect if this is cross compilation.")
      #endif
    #endif
  // macOS
  #elif defined(macintosh) || defined(__APPLE__) || defined(__APPLE_CC__)
    #define HELA_MACOS
  // MSW
  #elif defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
    #if defined(WINAPI_PARTITION_DESKTOP)
      #if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP)
        #define HELA_MSW
      #else
        #error "Microsoft Universal Windows Platform, are you kidding me?"
      #endif
    #else
      #define HELA_MSW
      // #include <sdkddkver.h>
    #endif
  // Android
  #elif defined(__ANDROID__) && (defined(linux) || defined(__linux) || defined(__linux__))
    #error "Android testbench?"
  #else
    #error "Platform unknown / not supported"
  #endif

  #if defined(HELA_LINUX) ||  defined(HELA_MACOS)
    #define HELA_POSIX
  #endif

}

// fixed width integer types in namespace are ones from <cstdint>
// https://en.cppreference.com/w/cpp/types/integer

#include <cstdint>

#if defined(HELA_MACOS) && !defined(_LIBCPP_VERSION)
  #error "libstdc++ meh"
#endif

#if defined (__clang__)
  #if ! __has_include(<cstdint>)
  #error "<cstdint> meh"
  #endif
#endif

namespace hela {
  using std::int8_t;
  using std::uint8_t;
  using std::int16_t;
  using std::uint16_t;
  using std::int32_t;
  using std::uint32_t;
  using std::int64_t;
  using std::uint64_t;
}


#include "Log.hpp"
#include "Vector.hpp"
#include "InputEventCodes.hpp"

#endif // Hela_hpp
