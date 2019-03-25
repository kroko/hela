#ifndef Nucleus_hpp
#define Nucleus_hpp

#include "Hela.hpp"

// Just forward typedefs for user code side normalization across target and devbenches

#if defined(HELA_LINUX) && defined(HELA_LINUX_ARM) && defined(HELA_LINUX_ARM_FBDEV_MALI_ALLWINNER)
  // FBDev on Allwinner
  #include "nucleus/embedded-linux-fbdev-mali-allwinner-mali-utgard/NucleusLinuxFbdevMaliAllwinner.hpp"
  namespace hela { namespace nucleus {
    typedef NucleusLinuxFbdevMaliAllwinner Nucleus;
  } }

#else
  // Anything else currently is directed to generic desktop (x64) os
  #include "nucleus/desktop-genericos/NucleusGenericOs.hpp"
  namespace hela { namespace nucleus {
    typedef NucleusGenericOs Nucleus;
  } }
#endif

#endif // Nucleus_hpp
