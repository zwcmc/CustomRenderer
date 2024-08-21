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
  - [x] With Cubemap texture
  - [x] With Radiance HDR texture (Need rendered-to-cubemap first)
- [x] Tonemapping
- [x] Image Based Lighting
  - [x] Irradiance Cubemap
  - [x] Pre-filterd Cubemap
  - [x] Pre-computing environment BRDF LUT
- [x] Main Light Shadow Mapping
  - [x] PCF (Percentage Closer Filter)
  - [ ] PCSS (Percentage Closer Soft Shadow)
  - [ ] Cascaded Shadow Mapping
- [ ] Bloom
- [ ] FXAA
- [ ] MSAA
- [ ] Deferred Rendering
- [ ] SSAO
- [ ] TAA

## References

- [Learn OpenGL by Joey de Vries](https://learnopengl.com/Introduction)
- [glTF-Sample-Assets](https://github.com/KhronosGroup/glTF-Sample-Assets)
- [Vulkan-glTF-PBR](https://github.com/SaschaWillems/Vulkan-glTF-PBR)
