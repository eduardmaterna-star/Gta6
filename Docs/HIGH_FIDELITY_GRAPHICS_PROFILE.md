# High-fidelity graphics profile — Unreal Engine 5.8

## Goal

Visual quality has priority over keeping the installed game below 5 GB. Five gigabytes is an acceptable early target, not a hard cap. Do not remove necessary environment art, textures, audio or gameplay content just to meet that number. Measure the real size only after a packaged Windows build exists.

The current target PC is Ryzen 5 5600, GeForce RTX 5050 8 GB VRAM and 16 GB system RAM. The first art/renderer target is a detailed 1080p desktop experience. 1080p / 120 FPS remains a goal to measure, not a promise. If the quality target and frame budget conflict, profile the scene and expose quality presets rather than silently reducing visual quality.

## Renderer baseline in DefaultEngine.ini

- **DirectX 12** is the Windows default RHI.
- **Lumen GI and reflections** are enabled for dynamic indirect lighting and reflections. Software ray tracing is the baseline; hardware ray tracing is deliberately not forced for every scene.
- **Virtual Shadow Maps** are enabled as the high-detail desktop shadow path.
- **Nanite project support** is enabled. This only makes the project ready for Nanite; each imported static mesh still needs an appropriate Nanite setting and validation.
- **Temporal Super Resolution (TSR)** is selected as the default anti-aliasing/upscaling method.
- **Mesh Distance Fields** remain enabled for Lumen software tracing.

These settings require a restart and trigger shader/DDC work in the Unreal Editor. The editor may take longer to open after the first change while shaders are compiled.

## Asset and environment quality rules

1. Replace the current cube-based greybox city with original modular building kits, believable roofs, windows, balconies, street details and authored landmarks.
2. Use PBR material sets with correctly authored base color, normal and packed roughness/metallic/ambient-occlusion data. Prefer material instances over many duplicated materials.
3. Use 2K textures as a normal starting point for visible environment assets; reserve 4K for close-up landmarks and hero assets where profiling justifies the memory cost. Use lower mips/resolutions for small or distant props.
4. Enable Nanite per suitable static mesh. Keep conventional LODs or suitable fallbacks where needed, especially for asset types or materials that do not use the Nanite path.
5. Use texture streaming, mipmaps, HLODs and World Partition streaming for the large city. Do not keep every district's full-resolution assets resident at once.
6. Build lighting around Lumen, a controlled exposure setup and a carefully authored sky/atmosphere. Do not add large numbers of shadow-casting dynamic lights without measuring their cost.
7. Use a global Post Process Volume for a consistent baseline, then tune exposure, color grading, bloom, depth of field and motion blur intentionally. Avoid strong effects that reduce readability during gameplay.
8. Create detailed interiors only for important buildings and locations; stream them rather than loading the entire city's interiors at once.

## Quality and performance policy

- The first target preset is **High visual quality**, with an optional Epic/Ultra preset once a real scene is available for measurement.
- Do not force Cinematic scalability as the default. It is intended for offline/cinematic rendering and may be unnecessarily expensive for gameplay.
- Lumen quality, reflection quality, shadow resolution, foliage density, view distance and TSR screen percentage must be measured together in a representative city block.
- Record GPU frame time, CPU frame time, 1% lows, system RAM and VRAM at 1080p. Test a quiet street, a dense street, an interior and a district transition.
- The RTX 5050 has 8 GB VRAM, so texture residency and streaming budgets are mandatory. The PC currently has 16 GB system RAM; Epic's UE hardware guidance recommends 32 GB for development, so editor/build memory pressure is a known risk.

## Acceptance criteria

- UE 5.8 opens the project using DX12 and compiles the renderer shaders without errors.
- A representative map visibly uses Lumen GI/reflections, Virtual Shadow Maps and TSR.
- At least one suitable detailed static mesh is validated with Nanite enabled.
- A packaged Windows build is checked for missing/low-resolution mip levels, shader errors, visual artefacts and streaming hitches.
- Performance report records exact GPU/driver, resolution, settings, frame-time, 1% lows, system RAM and VRAM.
- No 120 FPS claim is made until measured on the target PC.

## Known limitation

The current native UE city generator still creates basic cube geometry. Enabling Lumen, Nanite support, VSM and TSR improves the rendering pipeline but does not create detailed Prague architecture or finished materials. The actual visual upgrade depends on adding and testing original art assets and a real editor-authored level.

## References

- [UE 5.8 hardware and software specifications](https://dev.epicgames.com/documentation/en-us/unreal-engine/hardware-and-software-specifications-for-unreal-engine)
- [UE 5.8 Lumen GI and reflections](https://dev.epicgames.com/documentation/en-us/unreal-engine/lumen-global-illumination-and-reflections-in-unreal-engine)
- [UE 5.8 Temporal Super Resolution](https://dev.epicgames.com/documentation/en-us/unreal-engine/temporal-super-resolution-in-unreal-engine)
- [UE 5.8 scalability reference](https://dev.epicgames.com/documentation/en-us/unreal-engine/scalability-reference-for-unreal-engine)
