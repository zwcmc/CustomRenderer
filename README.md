# Custom Renderer

A customized OpenGL renderer made with C++ and OpenGL 4.1

## Feature

- [x] Loading glTF 2.0 models
  - [x] With POSITION, TEXCOORD_0 and NORMAL attributes
  - [x] Textures and Factors
    - [x] baseColorTexture and baseColorFactor
    - [x] normalTexture
    - [x] emissiveTexture and emissiveFactor
    - [x] metallicRoughnessTexture, metallicFactor and roughnessFactor
    - [x] occlusionTexture
  - [x] Alpha Blend
  - [x] Alpha Cutoff
  - [x] Double Sided Rendering (Face Culling)
- [x] Implement Assimp for loading OBJ files
- [x] Arcball Camera
- [x] Blinn-Phong Lighting
- [ ] Physically Based Rendering
  - [x] Disney BRDF
  - [ ] Kulla-Conty BRDF
- [x] Gamma Correction
- [x] HDR
- [x] Skybox
  - [ ] With KTX Cubemap texture
  - [x] With Radiance HDR texture (Need rendered-to-cubemap first)
- [x] Tonemapping
- [x] Image Based Lighting
  - [x] Irradiance Cubemap
  - [x] Pre-filterd Cubemap
  - [x] Pre-computing environment BRDF LUT
- [x] Main Light Shadow Maps
  - [x] Calculate a projection for the light's frustum to tightly cover the camera's frustum
  - [x] Remove the shimmering edge effect along the edges of shadows due to the light changing to fit the camera
  - [x] Calculate the near and far planes by computing intersections with the light frustum and the scene
  - [x] Cascaded Shadow Maps
    - [x] Interval-Based Cascade Selection
    - [x] Map-Based Cascade Selection
  - [x] PCF (Percentage Closer Filter)
  - [ ] VSM (Variance Shadow Maps)
  - [ ] PCSS (Percentage Closer Soft Shadow)
- [x] Bloom
- [x] FXAA
- [x] Deferred Rendering
- [x] SSAO
- [ ] TAA
- [ ] SSR

## References

- [filament](https://github.com/google/filament)
- [DirectX Technical Articles](https://learn.microsoft.com/en-us/windows/win32/dxtecharts/dx9-technical-articles)
- [Learn OpenGL by Joey de Vries](https://learnopengl.com/Introduction)
- [glTF-Sample-Assets](https://github.com/KhronosGroup/glTF-Sample-Assets)
- [Vulkan-glTF-PBR](https://github.com/SaschaWillems/Vulkan-glTF-PBR)
