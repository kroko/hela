#ifndef Motor_hpp
#define Motor_hpp

#include "Hela.hpp"
#include "nucleus/MotorStrap.hpp" // include only OpenGL

namespace hela { namespace motor {

  bool isExtensionSupported(std::string extensionName);
  bool isExtensionSupported(const char* extensionName);

  typedef std::shared_ptr<class Motor> Motor_S;

  // ============================================================================
  class Motor {

  public:
    // Forward Motor inners
    class MotorSettings;

  public:

    Motor() {
      spdlog::trace("Motor constructor empty");
    }

    Motor(const MotorSettings& settings = MotorSettings()) :
    m_motorSettings(settings) {
      spdlog::trace("Motor constructor with settings");
    }

    virtual ~Motor();

    const MotorSettings& getMotorSettings() const {
      return m_motorSettings;
    }

    // ----------------------------------------------------------------------------
    // MotorSettings
  public:

    typedef std::function<void (MotorSettings *settings)>  MotorSettingsFn;

    class MotorSettings {

      //: friends
      friend Motor;

    public:

      // --------------------------------
      MotorSettings() :
      m_coreProfile(true),
      m_version(std::pair<uint8_t,uint8_t>(3, 3)),
      m_msaaSamples(0),
      m_vsync(true),
      m_bufferColorBitsPerChannel(8),
      m_bufferDepthBits(24),
      m_bufferStencilBits(8),
      m_bufferDepthEnabled(true),
      m_bufferStencilEnabled(false)
      {
        // the defaults
        spdlog::trace("Motor::MotorSettings construct");
      }

      // --------------------------------
      // Irrelevant for Allwinner
      // for allwinner it is always GLES2
      // in order to signal that to coder, simply do not implement these

      void setVersion(uint8_t major, uint8_t minor) { m_version = std::make_pair(major, minor); }
      void setVersion(std::pair<uint8_t,uint8_t> version) {
        if (version.first < 3) {
          spdlog::warn("No lower than OpenGL 3.3 is allowed, setting 3.3. Remember, no immediate mode!");
          m_version = std::make_pair(3, 3);
        }
        m_version = version;
      }
      std::pair<uint8_t,uint8_t>  getVersion() const { return m_version; }

      bool isCoreProfile() const { return m_coreProfile; }
      void setCoreProfile(bool enable) { m_coreProfile = enable; }

      void setMsaa(uint8_t samples) {
        if (samples == 0) { m_msaaSamples = samples; }
        else if (((samples & (samples - 1)) == 0) && (samples > 1) && (samples <= 16)) { m_msaaSamples = samples; }
        else {
          if (samples < 2) { m_msaaSamples = 0; }
          else if (samples > 16) { m_msaaSamples = 16; }
          else { m_msaaSamples = [](uint8_t s)->uint8_t{ s = s | (s >> 1); s = s | (s >> 2); s = s | (s >> 4); return s - (s >> 1); }(samples); }
          spdlog::warn("Motor::MotorSettings MSAA value can be zero or 2^n where n=[2;4], you gave {}, will set to nearest smaller valid {}", samples, m_msaaSamples);
        }
      }
      uint8_t getMsaa() const { return m_msaaSamples; }

      void setVsync(bool vsync = true) { m_vsync = vsync; }
      bool isVsync() const { return m_vsync; }

      void setColorBufferChannelBits(uint8_t colorBitsPerChannel) { m_bufferColorBitsPerChannel = colorBitsPerChannel; }
      uint8_t getColorBufferChannelBits() const { return m_bufferColorBitsPerChannel; }

      void setDepthBufferEnabled(bool createDepth = true) { m_bufferDepthEnabled = createDepth; }
      bool isDepthBufferEnabled() const { return m_bufferDepthEnabled; }

      void setDepthBufferBits(uint8_t depthBufferBits) { m_bufferDepthBits = depthBufferBits; }
      uint8_t getDepthBufferBits() const { return m_bufferDepthBits; }

      void setStencilBufferEnabled(bool createStencil = true) { m_bufferStencilEnabled = createStencil; }
      bool isStencilBufferEnabled() const { return m_bufferStencilEnabled; }

      void setStencilBufferBits(uint8_t stencilBufferBits) { m_bufferStencilBits = stencilBufferBits; }
      uint8_t getStencilBufferBits() const { return m_bufferStencilBits; }

    protected:

      bool                        m_coreProfile;
      std::pair<uint8_t,uint8_t>  m_version;
      uint8_t                     m_msaaSamples;
      bool                        m_vsync;
      uint8_t                     m_bufferColorBitsPerChannel;
      uint8_t                     m_bufferDepthBits;
      uint8_t                     m_bufferStencilBits;
      bool                        m_bufferDepthEnabled;
      bool                        m_bufferStencilEnabled;

    }; // Motor::MotorSettings

  protected:

    MotorSettings m_motorSettings;

  }; // Motor

} }

#endif // Motor_hpp
