# Custom Renderer

A customized renderer built with C++ and OpenGL 4.1 Core.

![CustomRenderer_pic](/CustomRenderer_pic.jpeg)

## Feature

- [x] Loading glTF 2.0 models
- [x] Loading Wavefront Object (.obj) models
- [x] Arcball Camera
- [x] Blinn-Phong Lighting
- [x] Physically Based Renderingk
- [x] Gamma Correction
- [x] HDR
- [x] Skybox
  - [x] Radiance HDR texture (Need rendered-to-cubemap first)
- [x] Tonemapping
- [x] Image Based Lighting
  - [x] Irradiance Cubemap
  - [x] Pre-filterd Cubemap
  - [x] Pre-computing environment BRDF LUT
- [x] Main Light Shadow Maps
  - [x] Cascaded Shadow Maps
  - [x] PCF (Percentage Closer Filter)
- [x] Bloom
- [x] FXAA
- [x] Deferred Rendering

## To-do List

- [ ] Optimization: Use only one material for rendering scene node
- [ ] First-person Camera
- [ ] Kulla-Conty BRDF
- [ ] SSR
- [ ] TAA
- [ ] VSM (Variance Shadow Maps)
- [ ] PCSS (Percentage Closer Soft Shadow)
- [ ] SSAO

## Reference

- [filament](https://github.com/google/filament)
- [DirectX Technical Articles](https://learn.microsoft.com/en-us/windows/win32/dxtecharts/dx9-technical-articles)
- [Learn OpenGL by Joey de Vries](https://learnopengl.com/Introduction)
- [glTF-Sample-Assets](https://github.com/KhronosGroup/glTF-Sample-Assets)
- [Vulkan-glTF-PBR](https://github.com/SaschaWillems/Vulkan-glTF-PBR)
