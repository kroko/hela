
## Naming

### Template

`<type>-<os>-<layer>-<driver>-<soc>`

### Implemented

`embedded-linux-fbdev-mali-allwinner`

`desktop-genericos(-generic-generic-generic)`

`desktop` actually means `x64`. `embedded` also is dubious as there is x64 embedded. ignore.

## MotorStrap

These extensions were found while using `r8p1` drivers (see repos root *README*).

We really need `GL_OES_vertex_array_object` and `GL_OES_depth24`. Shameful forward definitions for these (and not only) in `MotorStrap.hpp` so we can align user code.

```
[D] --------------------------------
[D] EGL INFO
[D] EGL version: 1.4
[D] --------------------------------
[D] GL INFO
[D] GL Version: OpenGL ES 2.0 "mali450-r5p1-01rel0-lollipop-233-g52c929d"
[D] GL GLSL version: OpenGL ES GLSL ES 1.00
[D] GL Renderer: Mali-400 MP
[D] GL Vendor: ARM
[D] GL Extensions:

GL_OES_texture_npot
GL_OES_vertex_array_object
GL_OES_compressed_ETC1_RGB8_texture
GL_EXT_compressed_ETC1_RGB8_sub_texture
GL_OES_standard_derivatives
GL_OES_EGL_image
GL_OES_depth24
GL_ARM_rgba8
GL_ARM_mali_shader_binary
GL_OES_depth_texture
GL_OES_packed_depth_stencil
GL_EXT_texture_format_BGRA8888
GL_OES_vertex_half_float
GL_EXT_blend_minmax
GL_OES_EGL_image_external
GL_OES_EGL_sync
GL_OES_rgb8_rgba8
GL_EXT_multisampled_render_to_texture
GL_EXT_discard_framebuffer
GL_OES_get_program_binary
GL_ARM_mali_program_binary
GL_EXT_shader_texture_lod
GL_EXT_robustness
GL_OES_depth_texture_cube_map
GL_KHR_debug
GL_ARM_shader_framebuffer_fetch
GL_ARM_shader_framebuffer_fetch_depth_stencil
GL_OES_mapbuffer
GL_KHR_no_error

[D] Display: EGL_CLIENT_APIS: OpenGL_ES
[D] Display: EGL_VENDOR: ARM
[D] Display: EGL_VERSION: 1.4 Linux-r8p1-00rel0
[D] Display: EGL_EXTENSIONS:

EGL_KHR_image
EGL_KHR_image_base
EGL_KHR_image_pixmap
EGL_KHR_gl_texture_2D_image
EGL_KHR_gl_texture_cubemap_image
EGL_KHR_gl_renderbuffer_image
EGL_KHR_reusable_sync
EGL_KHR_fence_sync
EGL_KHR_lock_surface
EGL_KHR_lock_surface2
EGL_EXT_create_context_robustness
EGL_ANDROID_blob_cache
EGL_KHR_create_context
EGL_KHR_partial_update
EGL_KHR_create_context_no_error

[D] GL_MAX_VERTEX_ATTRIBS Max no. of vertex attributes supported: 16
[D] GL_MAX_TEXTURE_IMAGE_UNITS Max no. of sim tex units supported: 8
```

