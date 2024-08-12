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
- [x] Arcball Camera
- [x] Blinn-Phong Lighting
- [ ] Physically Based Rendering
  - [ ] Kulla-Conty BRDF
- [x] Gamma Correction
- [x] HDR
- [x] Skybox
  - [x] With Cubemap texture
  - [x] With Radiance HDR texture (Need rendered-to-cubemap first)
- [ ] Tonemapping
- [x] Image Based Lighting
  - [x] Irradiance Cubemap
  - [x] Pre-filterd Cubemap
  - [x] Pre-computing environment BRDF LUT
- [ ] Shadow Mapping
  - [ ] Cascaded Shadow Mapping
- [ ] Bloom
- [ ] MSAA
- [ ] FXAA
- [ ] SSAO
- [ ] TAA

## References

- [Learn OpenGL by Joey de Vries](https://learnopengl.com/Introduction)
- [glTF-Sample-Assets](https://github.com/KhronosGroup/glTF-Sample-Assets)
- [Vulkan-glTF-PBR](https://github.com/SaschaWillems/Vulkan-glTF-PBR)
