# Hela

[Hela](https://en.wikipedia.org/wiki/HeLa) is abstraction that was created inhouse in order to be able to write PoC code for [OpenGL ES](https://en.wikipedia.org/wiki/OpenGL_ES) on top of [EGL](https://en.wikipedia.org/wiki/EGL_(API)) interface on SoC's that have [Utgard - Mali-400 series](https://en.wikipedia.org/wiki/Mali_(GPU)) GPUs and [Mali](https://en.wikipedia.org/wiki/Mali_(GPU)) driver whilst running on [Linux framebuffer](https://en.wikipedia.org/wiki/Linux_framebuffer) natively (no pbuffer).

No *wayland*, no *Xlib*, no *DirectFB*, no *DRM*. Just *EGL* & *GLES* on *FBDev* within bare terminal.

This PoC was demanded for [Allwiner's Utgard - Mali-400 series](http://linux-sunxi.org/Mali#Utgard_.28Mali-400_and_Mali-450.29) thus it is written towards it. Hela is not a framework. It does not contain any OpenGL helper classes, wrappers, state machine RAIIs.

The SBC this was developed on / tested against was [Orange Pi Lite](http://www.orangepi.org/orangepilite/) that employs *Allwinner H3*. It was running `r8p1` [Mali kernel driver](https://github.com/mripard/sunxi-mali) with `r8p1` [Mali userspace library](https://github.com/bootlin/mali-blobs) installed and set up in the system. By judging from [discussions](https://github.com/mripard/sunxi-mali/issues) in kernel module repo it seems that these driver support all *Utgard* series GPUs that are used by *Allwinner*. For more on drivers refer to *Example usage* notes below.

## Goal

The goal was to establish abstraction where it is possible to write tests on some x64 devbench, run them on devbench and then just push them to SoC and build and run there. Hela takes care of detecting platform and branching off. A necessary NIH syndrome.

The approach for *user side code* is overriding run loop logic via `setup, update, draw, cleanup`. It is the good old approach of [p5](https://processing.org) continued by [oF](https://openframeworks.cc), [cinder](https://libcinder.org).

The structure is inspired by *cinder* although the resemblance is more in use of factories. Lela is stripped naked and violated.

The abstraction ended at the level where it started to run. The intention *is* to use *ELG* and *OpenGL ES* on *FBdev*, intention *is* to directly call *OpenGL* functions in user code. Any abstraction, RAIIs if wanted is done in user code, Hela does not care as it is PoC, remember - this is meant to be just for tests. Creating possibility in Hela to choose rendering backend on SoC is irrelevant. Same applies to desktop test benches where [GLFW](https://github.com/glfw/glfw) and [glad](https://github.com/Dav1dde/glad) is used exclusively, these run everywhere, Hela does not give possibility to choose straight native window and I/O bindings, not to mention other rendering backends such as DirectX and ANGLE.

## Hela / Nucleus

`Nucleus` typedefs to `Nucleus<os><layer><driver><soc>(<gpu>)`. `Nucleus<...>` has entrance factory, it's interface is managed via `NucleusAbstract` using (pure) virtuals and itself having basic factories (btw `<...>` is not notion of templates).

Currently implemented `Nucleus` are `NucleusLinuxFbdevMaliAllwinner(MaliUtgard)` for Allwinner with Mali Utgard GPU and Mali drivers and `NucleusGenericOs` for generic x64 (currently tested on MSW with MSVC and macOS with Clang).

This level of abstraction enables Hela to be potentially used also on different SoCs which for example utilises same *Utgard* architecture (probably has different kernel driver and userspace libraries capabilities, say has float texture support), or utilises *Midgard* (which amongst different kernel driver and userspace libraries means GLES3). Either `define` branches in current or typedef to new *Nucleus* branch and off we go with trying to get say *NucleusLinuxFbdevMaliExynosMaliMidgard* working.

## Hela / Flagella

If asked Hela autodetects keyboard and/or mouse, if asked Hela uses supplied keyboard and/or mouse endpoints, if asked Hela does not try to use keyboard and/or mouse. See *Example usage* notes below.

## Hela / Food

Only external dependency for Hela to live is [spdlog](https://github.com/gabime/spdlog). It is submodule, thus this directory needs to be cloned recursively.

## Example usage

Usage is supplied via some test projects that reside in [separate repo](https://github.com/WARP-LAB/hela-tests). By following notes there test projects will download Hela and set up food in place. It also briefly discusses Linux kernel setup and driver details.

## Outro

Biology is cool, mkay.

`cat /dev/urandom > /dev/fb0`