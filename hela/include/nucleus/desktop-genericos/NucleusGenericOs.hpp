#ifndef NucleusGenericOs_hpp
#define NucleusGenericOs_hpp

#include "nucleus/NucleusAbstract.hpp"

namespace hela { namespace nucleus {

  class RunnerGenericOs;

  class NucleusGenericOs : public NucleusAbstract {
  public:

    NucleusGenericOs();
    virtual ~NucleusGenericOs();

    void quit() override;

    // as on GLFW we always assume presence of keyboard and mouse we can actually return the input value
    bool eventHasKeyboard(bool fakeReturnValue = true) const override { return fakeReturnValue; }
    bool eventHasMouse(bool fakeReturnValue = true) const override { return fakeReturnValue; }

    void setCaptureCursor(bool capture = true) override;
    bool isCaptureCursor() const override;
    void setFullScreen(bool fullScreen = true) override;
    bool isFullScreen() const override;

    template<typename NucleusT>
    static void main(
                     int argc,
                     char const * const argv[],
                     const NucleusSettingsFn &nucleusSettingsFn = NucleusSettingsFn(),
                     const hela::motor::Motor::MotorSettingsFn &motorMotorSettingsFn = hela::motor::Motor::MotorSettingsFn()
                   );

  protected:

    void launch() override;

  private:

     std::shared_ptr<RunnerGenericOs> m_runner = nullptr;

  }; // NucleusGenericOs

  template<typename NucleusT>
  void NucleusGenericOs::main(
                              int argc,
                              char const * const argv[],
                              const NucleusSettingsFn &nucleusSettingsFn,
                              const hela::motor::Motor::MotorSettingsFn &motorMotorSettingsFn
                            ) {

    try {
      NucleusGenericOs::prepareLaunch();
      {
        NucleusSettings nucleusSettings;
        if (nucleusSettingsFn) {
          nucleusSettingsFn(&nucleusSettings);
        }
        hela::motor::Motor::MotorSettings motorSettings;
        if (motorMotorSettingsFn) {
          motorMotorSettingsFn(&motorSettings);
        }
        hela::motor::Motor_S motor = std::make_shared<hela::motor::Motor>(motorSettings);
        NucleusGenericOs::initialize(argc, argv, &nucleusSettings, motor);
        {
          std::shared_ptr<NucleusGenericOs> nucleus = std::make_shared<NucleusT>();
          nucleus->executeLaunch();
        }
      }
      NucleusGenericOs::cleanupLaunch();
    }
    catch (std::exception &e){
      spdlog::error("Main got excepton: {}", e.what());
    }
    catch (...) {
      spdlog::error("Main got excepton");
    }

  } // main

} }

#endif // NucleusGenericOs_hpp
