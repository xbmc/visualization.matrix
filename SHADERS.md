# Shader Documentation for visualization.matrix

This document describes all **shaders**, **uniforms**, **textures**, and **constants** used in the **Matrix Visualization** addon for Kodi.

---

## 📁 **Shader Files Overview**

| **Shader File** | **Type** | **Description** | **Preset Name** | **Channel Usage** |
|-----------------|----------|-----------------|-----------------|-------------------|
| [`main_matrix_GL.vert.glsl`](visualization.matrix/resources/shaders/main_matrix_GL.vert.glsl) | Vertex (OpenGL) | Basic vertex shader for OpenGL | All | - |
| [`main_matrix_GLES.vert.glsl`](visualization.matrix/resources/shaders/main_matrix_GLES.vert.glsl) | Vertex (GLES) | Basic vertex shader for OpenGL ES | All | - |
| [`logo.frag.glsl`](visualization.matrix/resources/shaders/logo.frag.glsl) | Fragment | Kodi logo with rain effect | "Kodi" | `iChannel0` (FFT), `iChannel1` (Logo) |
| [`album.frag.glsl`](visualization.matrix/resources/shaders/album.frag.glsl) | Fragment | Album art with rain effect | "Album" | `iChannel0` (FFT), `iChannel3` (Album) |
| [`nologo.frag.glsl`](visualization.matrix/resources/shaders/nologo.frag.glsl) | Fragment | Rain effect only | "Rain only" | `iChannel0` (FFT), `iChannel2` (Noise) |
| [`nologowf.frag.glsl`](visualization.matrix/resources/shaders/nologowf.frag.glsl) | Fragment | Rain with waveform | "Rain with waveform" | `iChannel0` (FFT+Waveform), `iChannel2` (Noise) |
| [`nologowfenv.frag.glsl`](visualization.matrix/resources/shaders/nologowfenv.frag.glsl) | Fragment | Rain with waveform envelope | "Rain with waveform envelope" | `iChannel0` (FFT+Waveform), `iChannel2` (Noise) |
| [`clean.frag.glsl`](visualization.matrix/resources/shaders/clean.frag.glsl) | Fragment | Clean rain effect | "Clean" | `iChannel0` (FFT) |
| [`cleanwf.frag.glsl`](visualization.matrix/resources/shaders/cleanwf.frag.glsl) | Fragment | Clean with waveform | "Clean with waveform" | `iChannel0` (FFT+Waveform) |
| [`cleanwfenv.frag.glsl`](visualization.matrix/resources/shaders/cleanwfenv.frag.glsl) | Fragment | Clean with waveform envelope | "Clean with waveform envelope" | `iChannel0` (FFT+Waveform) |
| [`main_test.frag.glsl`](visualization.matrix/resources/shaders/main_test.frag.glsl) | Fragment | Test shader for bit precision detection | Internal | `iChannel0` (FFT) |

---

## 🎨 **Presets and Channel Mappings**

The presets are defined in [`src/main.cpp`](src/main.cpp) as follows:

```cpp
const std::vector<Preset> g_presets =
{
   {"Kodi",                         30100, "logo.frag.glsl",        99,  0,  1, -1},
   {"Album",                        30101, "album.frag.glsl",       99, -1,  1,  2},
   {"Rain only",                    30102, "nologo.frag.glsl",      99, -1,  1, -1},
   {"Rain with waveform",           30103, "nologowf.frag.glsl",    99, -1,  1, -1},
   {"Rain with waveform envelope",  30104, "nologowfenv.frag.glsl", 99, -1,  1, -1},
   {"Clean",                        30105, "clean.frag.glsl",       99, -1, -1, -1},
   {"Clean with waveform",          30106, "cleanwf.frag.glsl",     99, -1, -1, -1},
   {"Clean with waveform envelope", 30107, "cleanwfenv.frag.glsl",  99, -1, -1, -1},
};
```

### **Channel Mapping Legend**
| **Channel Index** | **Value** | **Meaning** | **Texture/Usage** |
|-------------------|-----------|-------------|-------------------|
| `channel[0]` | `99` | FFT/Audio Data | `iChannel0` (Frequency spectrum + waveform) |
| `channel[1]` | `0` | Logo | `iChannel1` (Kodi logo texture) |
| `channel[1]` | `-1` | Unused | - |
| `channel[2]` | `1` | Noise | `iChannel2` (Noise texture) |
| `channel[2]` | `-1` | Unused | - |
| `channel[3]` | `2` | Album Art | `iChannel3` (Album cover texture) |
| `channel[3]` | `-1` | Unused | - |

---

## 🔧 **Uniform Variables**

### **Global Uniforms (All Shaders)**

| **Uniform** | **Type** | **Description** | **Set By** | **Default Value** |
|-------------|----------|-----------------|------------|-------------------|
| `iTime` | `float` | Global time in seconds (scaled by `m_fallSpeed`) | `CVisualizationMatrix::RenderTo()` | `0.0` |
| `iResolution` | `vec2` | Render resolution (width, height) | `GatherDefines()` | `vec2(Width(), Height())` |

### **Texture Samplers**

| **Uniform** | **Type** | **Description** | **Used By** | **Texture Source** |
|-------------|----------|-----------------|------------|-------------------|
| `iChannel0` | `sampler2D` | FFT magnitude spectrum + waveform data | All shaders | Dynamically generated from audio |
| `iChannel1` | `sampler2D` | Logo texture | `logo.frag.glsl` | `resources/textures/logo.png` |
| `iChannel2` | `sampler2D` | Noise texture | `nologo.frag.glsl`, `nologowf.frag.glsl`, `nologowfenv.frag.glsl` | `resources/textures/noise.png` |
| `iChannel3` | `sampler2D` | Album cover texture | `album.frag.glsl` | Loaded from `UpdateAlbumart()` |

### **Album-Specific Uniforms**

| **Uniform** | **Type** | **Description** | **Used By** | **Set By** |
|-------------|----------|-----------------|------------|------------|
| `iAlbumPosition` | `vec3` | Album position offset (`xy`) and scale (`z`) | `album.frag.glsl` | `CVisualizationMatrix::RenderTo()` |
| `iAlbumRGB` | `vec3` | Album color multiplier (RGB) | `album.frag.glsl` | `CVisualizationMatrix::RenderTo()` |

---

## 🎛 **Constants (Shader Defines)**

The following constants are injected into the shaders via `GatherDefines()` in [`src/main.cpp`](src/main.cpp):

### **Visual Style Constants**

| **Constant** | **Type** | **Description** | **Default Value** | **Configurable** |
|--------------|----------|-----------------|-------------------|------------------|
| `cRNDSEED1` | `float` | Random seed 1 for noise generation | `170.12` | ❌ No |
| `cRNDSEED2` | `float` | Random seed 2 for noise generation | `7572.1` | ❌ No |
| `cINTENSITY` | `float` | Overall brightness intensity | `1.0` | ❌ No |
| `cMININTENSITY` | `float` | Minimum intensity for rain lines | `0.075` | ❌ No |
| `cDISTORTFACTORX` | `float` | Horizontal distortion factor | `0.6` | ❌ No |
| `cDISTORTFACTORY` | `float` | Vertical distortion factor | `0.4` | ❌ No |
| `cVIGNETTEINTENSITY` | `float` | Vignette effect strength | `0.05` | ❌ No |

### **User-Configurable Constants**

| **Constant** | **Type** | **Description** | **Source** | **Range** |
|--------------|----------|-----------------|------------|-----------|
| `cDotSize` | `float` | Size of the "dots" (pixels) | `m_dotSize` (Setting: `dotsize`) | `1.0` – `10.0` |
| `cColumns` | `float` | Number of rain columns | Calculated from `Width() / (m_dotSize * 2.0)` | Depends on resolution |
| `cNoiseFluctuation` | `float` | Noise texture fluctuation speed | `m_noiseFluctuation` (Setting: `noisefluctuation`) | `0.0` – `0.1` |
| `cDistortThreshold` | `float` | Threshold for distortion effects | `m_distortThreshold` (Setting: `distortthreshold`) | `0.0` – `0.1` |
| `cRainHighlights` | `float` | Intensity of rain highlights | `m_rainHighlights` (Setting: `rainhighlights`) | `0.0` – `1.0` |
| `cColor` | `vec3` | Color of the rain dots (RGB) | `m_dotColor` (Settings: `red`, `green`, `blue`) | `0.0` – `1.0` per channel |

---

## 📜 **Common Functions (Injected into Shaders)**

The following functions are injected into all fragment shaders via `fsCommonFunctionsLowPower` or `fsCommonFunctionsNormal` in [`src/main.cpp`](src/main.cpp):

### **Low-Power Mode Functions**

```glsl
// Simplified hash function for low-power devices
float h11(float p)
{
  return fract(.13 * p + 217943.37373737 / (p + 0.31));
}

// Waveform visualization
float waveform(vec2 uv)
{
  float wave = texture(iChannel0, vec2(uv.x * 0.15 + 0.5, 0.75)).x - 0.5;
  return min(abs(uv.y * 20.0 + wave * 10.0), 0.5);
}

// Noise texture sampling (if dNoise is defined)
#ifdef dNoise
float noise(vec2 gv)
{
  return texture(iChannel2, vec2(gl_FragCoord.xy / (256.0 * cDotSize))).x;
}
#endif

// Convert brightness to color
vec3 bw2col(float bw, vec2 uv)
{
  float d = length(fract(uv * cColumns) - 0.5);
  float peakcolor = 0.6 - d;
  float basecolor = 0.8 - d;
  return (basecolor * cColor + peakcolor) * bw;
}

// Get normalized UV coordinates
vec2 getUV()
{
  vec2 uv = (gl_FragCoord.xy - 0.5 * cResolution.xy) / cResolution.y;
  return uv;
}
```

### **Normal Mode Functions**

```glsl
// Enhanced hash function for normal mode
float h11(float p)
{
  return fract(20.12345 + sin(p * cRNDSEED1) * cRNDSEED2);
}

// Waveform visualization (enhanced)
float waveform(vec2 uv)
{
  float wave = texture(iChannel0, vec2(uv.x * 0.15 + 0.5, 0.75)).x * 0.5 + uv.y;
  return abs(smoothstep(0.225, 0.275, wave) - 0.5);
}

// Noise texture sampling (if dNoise is defined)
#ifdef dNoise
float noise(vec2 gv)
{
  return texture(iChannel2, (gv * 0.035431) + iTime * cNoiseFluctuation).x;
}
#endif

// Convert brightness to color (smooth)
vec3 bw2col(float bw, vec2 uv)
{
  float d = length(fract(uv * cColumns) - 0.5);
  float peakcolor = smoothstep(0.35, 0.0, d) * bw;
  float basecolor = smoothstep(0.85, 0.0, d) * bw;
  return basecolor * cColor + peakcolor;
}

// Get normalized UV coordinates (with optional CRT curve)
#ifdef dCrtCurve
vec2 getUV()
{
  vec2 uv = (gl_FragCoord.xy - 0.5 * cResolution.xy) / cResolution.y;
  uv = uv / (1.0 - length(uv * 0.1));
  return uv;
}
#else
vec2 getUV()
{
  vec2 uv = (gl_FragCoord.xy - 0.5 * cResolution.xy) / cResolution.y;
  return uv;
}
#endif
```

---

## 🔍 **Shader-Specific Details**

### **1. `album.frag.glsl`**

**Description:** Renders the album cover with rain effects and distortions.

**Key Features:**
- Uses `iChannel3` for album cover texture.
- Applies VHS-like distortions based on FFT data (`iChannel0`).
- Uses `iAlbumPosition` and `iAlbumRGB` for positioning and coloring.
- Implements shadow effects and vignette.

**Uniforms Used:**
- `iTime`, `iResolution`
- `iChannel0` (FFT data)
- `iChannel3` (Album texture)
- `iAlbumPosition` (Position offset and scale)
- `iAlbumRGB` (Color multiplier)

**Constants Used:**
- `cColumns`, `cDistortThreshold`, `cDISTORTFACTORX`, `cDISTORTFACTORY`, `cRainHighlights`, `cVIGNETTEINTENSITY`, `cINTENSITY`, `cMININTENSITY`

---

### **2. `logo.frag.glsl`**

**Description:** Renders the Kodi logo with rain effects and distortions.

**Key Features:**
- Uses `iChannel1` for the Kodi logo texture.
- Applies VHS-like distortions based on FFT data (`iChannel0`).
- Implements interlaced logo distortion effect.

**Uniforms Used:**
- `iTime`, `iResolution`
- `iChannel0` (FFT data)
- `iChannel1` (Logo texture)

**Constants Used:**
- `cColumns`, `cDistortThreshold`, `cDISTORTFACTORX`, `cDISTORTFACTORY`, `cRainHighlights`, `cVIGNETTEINTENSITY`

---

### **3. `nologo.frag.glsl`**

**Description:** Renders rain effect only (no logo or album).

**Key Features:**
- Pure rain effect with noise texture.
- Uses FFT data for distortion effects.
- Implements vignette effect.

**Uniforms Used:**
- `iTime`, `iResolution`
- `iChannel0` (FFT data)
- `iChannel2` (Noise texture)

**Constants Used:**
- `cColumns`, `cNoiseFluctuation`, `cRainHighlights`, `cVIGNETTEINTENSITY`, `cINTENSITY`

---

### **4. `nologowf.frag.glsl` & `nologowfenv.frag.glsl`**

**Description:** Rain effect with waveform visualization.

**Key Features:**
- Combines rain effect with waveform data from `iChannel0`.
- `nologowfenv.frag.glsl` uses waveform envelope for smoother transitions.

**Uniforms Used:**
- `iTime`, `iResolution`
- `iChannel0` (FFT + waveform data)
- `iChannel2` (Noise texture)

**Constants Used:**
- `cColumns`, `cNoiseFluctuation`, `cRainHighlights`, `cVIGNETTEINTENSITY`, `cINTENSITY`

---

### **5. `clean.frag.glsl`, `cleanwf.frag.glsl`, `cleanwfenv.frag.glsl`**

**Description:** Clean rain effect without logo or album.

**Key Features:**
- Minimalistic rain effect.
- `cleanwf.frag.glsl` adds waveform visualization.
- `cleanwfenv.frag.glsl` uses waveform envelope.

**Uniforms Used:**
- `iTime`, `iResolution`
- `iChannel0` (FFT + waveform data)

**Constants Used:**
- `cColumns`, `cRainHighlights`

---

## 🛠 **Texture Files**

| **Texture File** | **Path** | **Usage** | **Format** | **Dimensions** |
|------------------|----------|-----------|------------|----------------|
| `logo.png` | `resources/textures/logo.png` | Kodi logo | RGBA | 512×512 |
| `noise.png` | `resources/textures/noise.png` | Noise texture | RGBA | 256×256 |

---

## 📊 **Performance Considerations**

1. **Texture Uploads:**
   - Audio data textures (`iChannel0`) are updated every frame via `glTexImage2D`.
   - Album textures (`iChannel3`) are only updated when the album art changes.

2. **Shader Complexity:**
   - Low-power mode uses simplified functions (`fsCommonFunctionsLowPower`).
   - Normal mode uses more complex functions (`fsCommonFunctionsNormal`).

3. **Preset Switching:**
   - Switching presets reloads the shader and reconfigures the uniforms.
   - Textures are reused where possible.

4. **Framebuffer:**
   - A framebuffer is used for rendering to a texture before displaying.
   - Resolution is configurable via `m_state.fbwidth` and `m_state.fbheight`.

---

## 🔗 **See Also**

- [Main Source Code (`src/main.cpp`)](src/main.cpp)
- [Header File (`src/main.h`)](src/main.h)
- [Kodi Visualization API](https://kodi.tv)
- [GLSL Reference](https://www.khronos.org/opengl/)
