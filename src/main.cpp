/*
 *  Copyright (C) 2005-2021 Team Kodi <https://kodi.tv>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#include "main.h"

#include <regex>

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_JPEG
#define STBI_ONLY_PNG
#define STBI_ONLY_BMP
#include "stb_image.h"
#include "kodi/Filesystem.h"
#include "kodi/General.h"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <memory>

#define SMOOTHING_TIME_CONSTANT (0.5f) // default 0.8
#define MIN_DECIBELS (-100.0f)
#define MAX_DECIBELS (-30.0f)

// Override GL_RED if not present with GL_LUMINANCE, e.g. on Android GLES
#ifndef GL_RED
#define GL_RED GL_LUMINANCE
#endif

struct Preset
{
  std::string name;
  uint32_t labelId;
  std::string file;
  int channel[4];
};

// NOTE: With "#if defined(HAS_GL)" the use of some shaders is avoided
//       as they can cause problems on weaker systems.
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
   {"Neon",                         30108, "neon.frag.glsl",       99, -1, -1, -1},
   {"Odysseus",                     30109, "odysseus.frag.glsl",    99, -1, -1, -1},
};

const std::vector<std::string> g_fileTextures =
{
  "logo.png",
  "noise.png",
};

std::string fsCommonFunctionsLowPower = 
R"functions(float h11(float p)
{
  return fract(.13 * p + 217943.37373737 / (p + 0.31));
}

float waveform(vec2 uv)
{
  float wave = texture(iChannel0,vec2(uv.x*.15+.5,0.75)).x - .5;
  return min(abs(uv.y*20.+wave*10.),0.5);
}

#ifdef dNoise
float noise(vec2 gv)
{
	return texture(iChannel2, vec2(gl_FragCoord.xy/(256.*cDotSize))).x;
}
#endif

vec3 bw2col(float bw, vec2 uv)
{
  float d = length(fract(uv*cColumns)-.5);
  float peakcolor = .6-d;
  float basecolor = .8-d;
  return (basecolor*cColor+peakcolor)*bw;
}

vec2 getUV()
{
  vec2 uv = (gl_FragCoord.xy-0.5*cResolution.xy)/cResolution.y;
  return uv;
}

)functions";

std::string fsCommonFunctionsNormal = 
R"functions(float h11(float p)
{
  return fract(20.12345+sin(p*cRNDSEED1)*cRNDSEED2);
}

float waveform(vec2 uv)
{
  float wave = texture(iChannel0,vec2(uv.x*.15+.5,0.75)).x*.5 + uv.y;
  return abs(smoothstep(.225,.275,wave) -.5);
}

#ifdef dNoise
float noise(vec2 gv)
{
  return texture(iChannel2, (gv*.035431) + iTime*cNoiseFluctuation).x;
}
#endif

vec3 bw2col(float bw, vec2 uv)
{
  float d = length(fract(uv*cColumns)-.5);
  float peakcolor = smoothstep(.35,.0,d)*bw;
  float basecolor = smoothstep(.85,.0,d)*bw;
  return basecolor*cColor+peakcolor;
}

#ifdef dCrtCurve
vec2 getUV()
{
  vec2 uv = (gl_FragCoord.xy-0.5*cResolution.xy)/cResolution.y;
  uv = uv / (1.00 - length(uv*.1));
  return uv;
}
#else
vec2 getUV()
{
  vec2 uv = (gl_FragCoord.xy-0.5*cResolution.xy)/cResolution.y;
  return uv;
}
#endif

)functions";

CVisualizationMatrix::CVisualizationMatrix()
  : m_kissCfg(kiss_fft_alloc(static_cast<int>(AUDIO_BUFFER), 0, nullptr, nullptr)),
    m_audioData(AUDIO_BUFFER),
    m_magnitudeBuffer(NUM_BANDS),
    m_pcm(AUDIO_BUFFER)
{
  m_currentPreset = kodi::GetSettingInt("lastpresetidx");
  m_dotMode = static_cast<float>(kodi::GetSettingBoolean("dotmode"));
  if (m_dotMode)
  {
    m_dotSize = static_cast<float>(kodi::GetSettingInt("dotsize"));
  }
  else
  {
    if (Height() <= 900) m_dotSize = 3.0f;
    else if (Height() <= 1500) m_dotSize = 4.0f;
    else  m_dotSize = 5.0f;
  }
  m_fallSpeed = static_cast<float>(kodi::GetSettingInt("fallspeed")) * 0.01f;
  m_distortThreshold = static_cast<float>(kodi::GetSettingInt("distortthreshold")) * 0.005f;
  m_rainHighlights = static_cast<float>(kodi::GetSettingInt("rainhighlights")) * 0.016f;
  m_dotColor.red = static_cast<float>(kodi::GetSettingInt("red")) / 255.0f;
  m_dotColor.green = static_cast<float>(kodi::GetSettingInt("green")) / 255.0f;
  m_dotColor.blue = static_cast<float>(kodi::GetSettingInt("blue")) / 255.0f;
  m_lowpower = kodi::GetSettingBoolean("lowpower");
  m_noiseFluctuation = m_lowpower ? (static_cast<float>(kodi::GetSettingInt("noisefluctuation")) * 0.0002f)/m_fallSpeed * 0.25f 
                                  : (static_cast<float>(kodi::GetSettingInt("noisefluctuation")) * 0.0004f)/m_fallSpeed * 0.25f;
  m_crtCurve = kodi::GetSettingBoolean("crtcurve");
  m_lastAlbumChange = 0.0;
}

CVisualizationMatrix::~CVisualizationMatrix()
{
  // Vectors are automatically cleaned up
  free(m_kissCfg);
}

//-- Render -------------------------------------------------------------------
// Called once per frame. Do all rendering here.
//-----------------------------------------------------------------------------
void CVisualizationMatrix::Render()
{
  if (m_initialized)
  {
    RenderTo(m_matrixShader.ProgramHandle(), 0);
  }
}

bool CVisualizationMatrix::Start(int iChannels, int iSamplesPerSec, int iBitsPerSample, std::string szSongName)
{
  kodi::Log(ADDON_LOG_DEBUG, "Start %i %i %i %s\n", iChannels, iSamplesPerSec, iBitsPerSample, szSongName.c_str());

  //background vertex
  static const GLfloat vertex_data[] =
  {
    -1.0f, 1.0f, 1.0f, 1.0f,
     1.0f, 1.0f, 1.0f, 1.0f,
     1.0f,-1.0f, 1.0f, 1.0f,
    -1.0f,-1.0f, 1.0f, 1.0f,
  };

  // Upload vertex data to a buffer
  glGenBuffers(1, &m_state.vertex_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, m_state.vertex_buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_data), vertex_data, GL_STATIC_DRAW);

  m_samplesPerSec = iSamplesPerSec;
  Launch(m_currentPreset);
  m_initialized = true;

  return true;
}

void CVisualizationMatrix::Stop()
{
  m_initialized = false;
  kodi::Log(ADDON_LOG_DEBUG, "Stop");

  UnloadPreset();
  UnloadTextures();

  glDeleteBuffers(1, &m_state.vertex_buffer);
}

void CVisualizationMatrix::AudioData(const float* pAudioData, int iAudioDataLength, float* pFreqData, int iFreqDataLength)
{
  WriteToBuffer(pAudioData, static_cast<size_t>(iAudioDataLength), 2);

  std::vector<kiss_fft_cpx> in(AUDIO_BUFFER);
  std::vector<kiss_fft_cpx> out(AUDIO_BUFFER);
  
  for (size_t i = 0; i < AUDIO_BUFFER; i++)
  {
    in[i].r = BlackmanWindow(m_pcm[i], i, AUDIO_BUFFER);
    in[i].i = 0;
  }

  kiss_fft(m_kissCfg, in.data(), out.data());

  out[0].i = 0;

  // Create temporary copy for smoothing
  std::vector<float> tempBuffer = m_magnitudeBuffer;
  SmoothingOverTime(m_magnitudeBuffer, tempBuffer, out.data(), NUM_BANDS, SMOOTHING_TIME_CONSTANT, static_cast<unsigned int>(AUDIO_BUFFER));

  const double rangeScaleFactor = MAX_DECIBELS == MIN_DECIBELS ? 1 : (1.0 / (MAX_DECIBELS - MIN_DECIBELS));
  for (size_t i = 0; i < NUM_BANDS; i++)
  {
    float linearValue = m_magnitudeBuffer[i];
    double dbMag = !linearValue ? MIN_DECIBELS : LinearToDecibels(linearValue);
    double scaledValue = UCHAR_MAX * (dbMag - MIN_DECIBELS) * rangeScaleFactor;

    m_audioData[i] = static_cast<GLubyte>(std::max(std::min(static_cast<int>(scaledValue), UCHAR_MAX), 0));
  }

  for (size_t i = 0; i < NUM_BANDS; i++)
  {
    float v = (m_pcm[i] + 1.0f) * 128.0f;
    m_audioData[i + NUM_BANDS] = static_cast<GLubyte>(std::max(std::min(static_cast<int>(v), UCHAR_MAX), 0));
  }

  m_needsUpload = true;
}

//-- OnAction -----------------------------------------------------------------
// Handle Kodi actions such as next preset, lock preset, album art changed etc
//-----------------------------------------------------------------------------
bool CVisualizationMatrix::NextPreset()
{
  m_currentPreset = (m_currentPreset + 1) % static_cast<int>(g_presets.size());
  Launch(m_currentPreset);
  UpdateAlbumart();
  kodi::SetSettingInt("lastpresetidx", m_currentPreset);
  return true;
}

bool CVisualizationMatrix::PrevPreset()
{
  m_currentPreset = (m_currentPreset - 1 + static_cast<int>(g_presets.size())) % static_cast<int>(g_presets.size());
  Launch(m_currentPreset);
  UpdateAlbumart();
  kodi::SetSettingInt("lastpresetidx", m_currentPreset);
  return true;
}

bool CVisualizationMatrix::LoadPreset(int select)
{
  kodi::Log(ADDON_LOG_DEBUG, "Loading preset %i\n", select);
  m_currentPreset = select % static_cast<int>(g_presets.size());
  Launch(m_currentPreset);
  UpdateAlbumart();
  kodi::SetSettingInt("lastpresetidx", m_currentPreset);
  return true;
}

bool CVisualizationMatrix::RandomPreset()
{
  m_currentPreset = static_cast<int>((std::rand() / static_cast<float>(RAND_MAX)) * g_presets.size());
  Launch(m_currentPreset);
  UpdateAlbumart();
  kodi::SetSettingInt("lastpresetidx", m_currentPreset);
  return true;
}

//-- GetPresets ---------------------------------------------------------------
// Return a list of presets to Kodi for display
//-----------------------------------------------------------------------------
bool CVisualizationMatrix::GetPresets(std::vector<std::string>& presets)
{
  std::string name;
  for (const auto& preset : g_presets)
  {
    name = kodi::GetLocalizedString(preset.labelId, preset.name);
    presets.push_back(name);
  }
  return true;
}

//-- GetPreset ----------------------------------------------------------------
// Return the index of the current playing preset
//-----------------------------------------------------------------------------
int CVisualizationMatrix::GetActivePreset()
{
  return m_currentPreset;
}

bool CVisualizationMatrix::UpdateAlbumart()
{
  return UpdateAlbumart(m_albumArt);
}

bool CVisualizationMatrix::UpdateAlbumart(std::string albumart)
{
  m_albumArt = std::move(albumart);

  kodi::Log(ADDON_LOG_DEBUG, "Updating album art %s\n", m_albumArt.c_str());
  if (g_presets[m_currentPreset].channel[3] != 2)
  {
    return false;
  }

  std::string thumb = kodi::vfs::GetCacheThumbName(m_albumArt.c_str());
  thumb = thumb.substr(0, 8);
  std::string special = std::string("special://thumbnails/") + thumb[0] + std::string("/") + thumb;

  if (kodi::vfs::FileExists(special + std::string(".png")))
  {
    m_channelTextures[3] = CreateTexture(kodi::vfs::TranslateSpecialProtocol(special + std::string(".png")), GL_RGBA, GL_LINEAR, GL_CLAMP_TO_EDGE);
    return true;
  }
  else if (kodi::vfs::FileExists(special + std::string(".jpg")))
  {
    m_channelTextures[3] = CreateTexture(kodi::vfs::TranslateSpecialProtocol(special + std::string(".jpg")), GL_RGBA, GL_LINEAR, GL_CLAMP_TO_EDGE);
    return true;
  }

  m_channelTextures[3] = CreateTexture(kodi::GetAddonPath("resources/textures/logo.png"), GL_RGBA, GL_LINEAR, GL_CLAMP_TO_EDGE);
  
  return false;
}

void CVisualizationMatrix::RenderTo(GLuint shader, GLuint effect_fb)
{
  glUseProgram(shader);

  if (shader == m_matrixShader.ProgramHandle())
  {
    GLuint w = Width();
    GLuint h = Height();
    if (m_state.fbwidth && m_state.fbheight)
      w = static_cast<GLuint>(m_state.fbwidth), h = static_cast<GLuint>(m_state.fbheight);
    
    const auto now = std::chrono::high_resolution_clock::now();
    const auto duration = std::chrono::duration<double>(now.time_since_epoch());
    int64_t intt = static_cast<int64_t>(duration.count() * 1000.0 * m_fallSpeed) - m_initialTime;
    if (m_bitsPrecision)
      intt &= (1 << m_bitsPrecision) - 1;

    if (m_needsUpload)
    {
      for (int i = 0; i < 4; i++)
      {
        if (m_shaderTextures[i].audio)
        {
          glActiveTexture(GL_TEXTURE0 + i);
          glBindTexture(GL_TEXTURE_2D, m_channelTextures[i]);
          glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, static_cast<GLsizei>(NUM_BANDS), 2, 0, GL_RED, GL_UNSIGNED_BYTE, m_audioData.data());
        }
      }
      m_needsUpload = false;

      if (g_presets[m_currentPreset].channel[3] == 2)
      {
        const auto logotimer = std::chrono::duration<double>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
        const float delta = static_cast<float>(logotimer - m_lastAlbumChange) * 0.6f;
        const GLfloat r = std::max(std::sin(delta), 0.0f) * 0.7f;
        const GLfloat g = std::max(std::sin(delta - 1.0f), 0.0f) * 0.7f;
        const GLfloat b = std::max(std::sin(delta - 2.0f), 0.0f) * 0.7f;
        glUniform3f(m_attrAlbumRGBLoc, r, g, b);
        if (m_lastAlbumChange == 0.0)
        {
          glUniform3f(m_attrAlbumPositionLoc, 0.0f, 0.0f, 2.0f);
        }
        if (logotimer - m_lastAlbumChange >= 10.0)
        {
          m_albumX = static_cast<GLfloat>(std::fmod(logotimer * 1234.0, 1.0) * (static_cast<double>(Width())/static_cast<double>(Height()) + 1.0) - 1.0);
          m_albumY = static_cast<GLfloat>(std::fmod(logotimer * 7654.0, 1.0));
          m_lastAlbumChange = logotimer;
          m_AlbumNeedsUpload = true;
        }
        if (m_AlbumNeedsUpload)
        {
          // Clamp album position to ensure it stays within screen bounds [-1.0, 1.0]
          m_albumX = std::max(-1.0f, std::min(1.0f, m_albumX));
          m_albumY = std::max(-1.0f, std::min(1.0f, m_albumY));
          glUniform3f(m_attrAlbumPositionLoc, m_albumX, m_albumY, 2.0f);
          m_AlbumNeedsUpload = false; // Fixed: was always true
        }
      }
    }

    const float t = static_cast<float>(intt) / 1000.0f;

    glUniform1f(m_attrGlobalTimeLoc, t);

    for (int i = 0; i < 4; i++)
    {
      glActiveTexture(GL_TEXTURE0 + i);
      glUniform1i(m_attrChannelLoc[i], i);
      glBindTexture(GL_TEXTURE_2D, m_channelTextures[i]);
    }
  }
  else
  {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_state.framebuffer_texture);
    glUniform1i(m_state.uTexture, 0); // first currently bound texture "GL_TEXTURE0"
  }

  // Draw the effect to a texture or direct to framebuffer
  glBindFramebuffer(GL_FRAMEBUFFER, effect_fb);

  glBindBuffer(GL_ARRAY_BUFFER, m_state.vertex_buffer);
  glVertexAttribPointer(m_state.attr_vertex_e, 4, GL_FLOAT, GL_FALSE, 16, nullptr);
  glEnableVertexAttribArray(m_state.attr_vertex_e);
  glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
  glDisableVertexAttribArray(m_state.attr_vertex_e);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  for (int i = 0; i < 4; i++)
  {
    glActiveTexture(GL_TEXTURE0 + i);
    glBindTexture(GL_TEXTURE_2D, 0);
  }

  glUseProgram(0);
}

void CVisualizationMatrix::Mix(float* destination, const float* source, size_t frames, size_t channels)
{
  const size_t length = frames * channels;
  for (size_t i = 0; i < length; i += channels)
  {
    float v = 0.0f;
    for (size_t j = 0; j < channels; j++)
    {
      v += source[i + j];
    }

    destination[(i / 2)] = v / static_cast<float>(channels);
  }
}

void CVisualizationMatrix::WriteToBuffer(const float* input, size_t length, size_t channels)
{
  const size_t frames = length / channels;

  if (frames >= AUDIO_BUFFER)
  {
    const size_t offset = frames - AUDIO_BUFFER;

    Mix(m_pcm.data(), input + offset, AUDIO_BUFFER, channels);
  }
  else
  {
    const size_t keep = AUDIO_BUFFER - frames;
    std::copy(m_pcm.begin() + frames, m_pcm.end(), m_pcm.begin());

    Mix(m_pcm.data() + keep, input, frames, channels);
  }
}

void CVisualizationMatrix::Launch(int preset)
{
  m_bitsPrecision = DetermineBitsPrecision();
  // mali-400 has only 10 bits which means millisecond timer wraps after ~1 second.
  // we'll fudge that up a bit as having a larger range is more important than ms accuracy
  m_bitsPrecision = std::max(m_bitsPrecision, 13);
  kodi::Log(ADDON_LOG_DEBUG, "bits of precision: %d", m_bitsPrecision);

  UnloadTextures();

  // Reset album upload flag - only set to true for album shader
  m_AlbumNeedsUpload = (g_presets[preset].channel[3] == 2);

  m_usedShaderFile = kodi::GetAddonPath("resources/shaders/" + g_presets[preset].file);
  for (int i = 0; i < 4; i++)
  {
    if (g_presets[preset].channel[i] >= 0 && g_presets[preset].channel[i] < static_cast<int>(g_fileTextures.size()))
    {
      m_shaderTextures[i].texture = kodi::GetAddonPath("resources/textures/" + g_fileTextures[g_presets[preset].channel[i]]);
    }
    else if (g_presets[preset].channel[i] == 99) // framebuffer
    {
      m_shaderTextures[i].audio = true;
    }
    else
    {
      m_shaderTextures[i].texture.clear();
      m_shaderTextures[i].audio = false;
    }
  }
  // Audio
  m_channelTextures[0] = CreateTexture(GL_RED, static_cast<unsigned int>(NUM_BANDS), 2, m_audioData.data());
  // Logo
  if (!m_shaderTextures[1].texture.empty())
  {
    m_channelTextures[1] = CreateTexture(m_shaderTextures[1].texture, GL_RGBA, GL_LINEAR, GL_CLAMP_TO_EDGE);
  }
  // Noise
  if (!m_shaderTextures[2].texture.empty())
  {
    m_channelTextures[2] = CreateTexture(m_shaderTextures[2].texture, GL_RGBA, GL_LINEAR, GL_REPEAT);
  }
  // Album
  if (!m_shaderTextures[3].texture.empty())
  {
    m_channelTextures[3] = CreateTexture(m_shaderTextures[3].texture, GL_RGBA, GL_LINEAR, GL_CLAMP_TO_EDGE);
  }

  m_state.fbwidth = Width();
  m_state.fbheight = Height();
  LoadPreset(m_usedShaderFile);
}

void CVisualizationMatrix::UnloadTextures()
{
  for (int i = 0; i < 4; i++)
  {
    if (m_channelTextures[i])
    {
      glDeleteTextures(1, &m_channelTextures[i]);
      m_channelTextures[i] = 0;
    }
  }
}

void CVisualizationMatrix::LoadPreset(const std::string& shaderPath)
{
  UnloadPreset();
  GatherDefines();
  const std::string vertMatrixShader = kodi::GetAddonPath("resources/shaders/main_matrix_" GL_TYPE_STRING ".vert.glsl");
  if (!m_matrixShader.LoadShaderFiles(vertMatrixShader, shaderPath) ||
      !m_matrixShader.CompileAndLink("", "", m_defines, ""))
  {
    kodi::Log(ADDON_LOG_ERROR, "Failed to compile matrix shaders (current file '%s')", shaderPath.c_str());
    return;
  }

  const GLuint matrixShader = m_matrixShader.ProgramHandle();

  m_attrGlobalTimeLoc = glGetUniformLocation(matrixShader, "iTime");
  m_attrAlbumPositionLoc = glGetUniformLocation(matrixShader, "iAlbumPosition");
  m_attrAlbumRGBLoc = glGetUniformLocation(matrixShader, "iAlbumRGB");
  m_attrChannelLoc[0] = glGetUniformLocation(matrixShader, "iChannel0");
  m_attrChannelLoc[1] = glGetUniformLocation(matrixShader, "iChannel1");
  m_attrChannelLoc[2] = glGetUniformLocation(matrixShader, "iChannel2");
  m_attrChannelLoc[3] = glGetUniformLocation(matrixShader, "iChannel3");

  m_state.attr_vertex_e = glGetAttribLocation(matrixShader,  "vertex");

  // Prepare a texture to render to
  glActiveTexture(GL_TEXTURE0);
  glGenTextures(1, &m_state.framebuffer_texture);
  glBindTexture(GL_TEXTURE_2D, m_state.framebuffer_texture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_state.fbwidth, m_state.fbheight, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  // Prepare a framebuffer for rendering
  glGenFramebuffers(1, &m_state.effect_fb);
  glBindFramebuffer(GL_FRAMEBUFFER, m_state.effect_fb);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_state.framebuffer_texture, 0);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  const auto now = std::chrono::high_resolution_clock::now();
  const auto duration = std::chrono::duration<double>(now.time_since_epoch());
  m_initialTime = static_cast<int64_t>(duration.count() * 1000.0);
  m_initialTime += (m_initialTime % 100000);
}

void CVisualizationMatrix::UnloadPreset()
{
  if (m_state.framebuffer_texture)
  {
    glDeleteTextures(1, &m_state.framebuffer_texture);
    m_state.framebuffer_texture = 0;
  }
  if (m_state.effect_fb)
  {
    glDeleteFramebuffers(1, &m_state.effect_fb);
    m_state.effect_fb = 0;
  }
}

GLuint CVisualizationMatrix::CreateTexture(GLint format, unsigned int w, unsigned int h, const GLvoid* data)
{
  GLuint texture = 0;
  glActiveTexture(GL_TEXTURE0);
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);

  glTexParameteri(GL_TEXTURE_2D,  GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D,  GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, format, GL_UNSIGNED_BYTE, data);
  return texture;
}

GLuint CVisualizationMatrix::CreateTexture(const GLvoid* data, GLint format, unsigned int w, unsigned int h, GLint internalFormat, GLint scaling, GLint repeat)
{
  GLuint texture = 0;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, scaling);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, scaling);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, repeat);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, repeat);

  glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, w, h, 0, format, GL_UNSIGNED_BYTE, data);
  glBindTexture(GL_TEXTURE_2D, 0);

  return texture;
}

GLuint CVisualizationMatrix::CreateTexture(const std::string& file, GLint internalFormat, GLint scaling, GLint repeat)
{
  kodi::Log(ADDON_LOG_DEBUG, "creating texture %s\n", file.c_str());

  int width = 0, height = 0, n = 0;
  unsigned char* image = nullptr;
  stbi_set_flip_vertically_on_load(true);
  
  image = stbi_load(file.c_str(), &width, &height, &n, STBI_rgb_alpha);
  if (image == nullptr)
  {
    kodi::Log(ADDON_LOG_ERROR, "couldn't load image");
    return 0;
  }

  const GLuint texture = CreateTexture(image, GL_RGBA, static_cast<unsigned int>(width), 
                                       static_cast<unsigned int>(height), internalFormat, scaling, repeat);
  stbi_image_free(image);
  image = nullptr;

  return texture;
}

float CVisualizationMatrix::BlackmanWindow(float in, size_t i, size_t length)
{
  constexpr double alpha = 0.16;
  constexpr double a0 = 0.5 * (1.0 - alpha);
  constexpr double a1 = 0.5;
  constexpr double a2 = 0.5 * alpha;

  const float x = static_cast<float>(i) / static_cast<float>(length);
  return in * static_cast<float>(a0 - a1 * std::cos(2.0 * M_PI * x) + a2 * std::cos(4.0 * M_PI * x));
}

void CVisualizationMatrix::SmoothingOverTime(std::vector<float>& outputBuffer, const std::vector<float>& lastOutputBuffer, 
                                           kiss_fft_cpx* inputBuffer, size_t length, float smoothingTimeConstant, unsigned int fftSize)
{
  for (size_t i = 0; i < length; i++)
  {
    const kiss_fft_cpx c = inputBuffer[i];
    const float magnitude = std::sqrt(c.r * c.r + c.i * c.i) / static_cast<float>(fftSize);
    outputBuffer[i] = smoothingTimeConstant * lastOutputBuffer[i] + (1.0f - smoothingTimeConstant) * magnitude;
  }
}

float CVisualizationMatrix::LinearToDecibels(float linear)
{
  if (!linear)
    return -1000.0f;
  return 20.0f * std::log10(linear);
}

int CVisualizationMatrix::DetermineBitsPrecision()
{
  m_state.fbwidth = 32;
  m_state.fbheight = 26 * 10;
  LoadPreset(kodi::GetAddonPath("resources/shaders/main_test.frag.glsl"));
  RenderTo(m_matrixShader.ProgramHandle(), m_state.effect_fb);
  glFinish();

  std::unique_ptr<unsigned char[]> buffer(new unsigned char[m_state.fbwidth * m_state.fbheight * 4]);
  if (buffer)
    glReadPixels(0, 0, m_state.fbwidth, m_state.fbheight, GL_RGBA, GL_UNSIGNED_BYTE, buffer.get());

  int bits = 0;
  unsigned char b = 0;
  for (int j = 0; j < m_state.fbheight; j++)
  {
    const unsigned char c = buffer[4 * (j * m_state.fbwidth + (m_state.fbwidth >> 1))];
    if (c && !b)
      bits++;
    b = c;
  }
  UnloadPreset();
  return bits;
}

void CVisualizationMatrix::GatherDefines()
{
  m_defines.clear();
#if defined(HAS_GL)
  m_defines += "#version 150\n";
  m_defines += "#extension GL_OES_standard_derivatives : enable\n";
  m_defines += "out vec4 FragColor;\n";
  m_defines += "#ifndef texture2D\n#define texture2D texture\n#endif\n\n";
#else
  m_defines += "#version 100\n\n";
  m_defines += "#extension GL_OES_standard_derivatives : enable\n\n";
  m_defines += "precision mediump float;\n";
  m_defines += "precision mediump int;\n\n";
  m_defines += "#define FragColor gl_FragColor\n";
  m_defines += "#ifndef texture\n#define texture texture2D\n#endif\n\n";
#endif

  m_defines += "const float cRNDSEED1 = 170.12;\n";
  m_defines += "const float cRNDSEED2 = 7572.1;\n";
  m_defines += "const float cINTENSITY = 1.0;\n";
  m_defines += "const float cMININTENSITY = 0.075;\n";
  m_defines += "const float cDISTORTFACTORX = 0.6;\n";
  m_defines += "const float cDISTORTFACTORY = 0.4;\n";
  m_defines += "const float cVIGNETTEINTENSITY = 0.05;\n";

  m_defines += "const float cDotSize = " + std::to_string(m_dotSize) + ";\n";
  m_defines += "const float cColumns = " + std::to_string(static_cast<float>(Width()) / (m_dotSize * 2.0f)) + ";\n";
  m_defines += "const float cNoiseFluctuation = " + std::to_string(m_noiseFluctuation) + ";\n";
  m_defines += "const float cDistortThreshold = " + std::to_string(m_distortThreshold) + ";\n";
  m_defines += "const float cRainHighlights = " + std::to_string(m_rainHighlights) + ";\n";
  m_defines += "const vec3 cColor = vec3(" + std::to_string(m_dotColor.red) + "," + 
               std::to_string(m_dotColor.green) + "," + std::to_string(m_dotColor.blue) + ");\n";

  if (m_state.fbwidth && m_state.fbheight)
  {
    m_defines += "const vec2 cResolution = vec2(" + std::to_string(m_state.fbwidth) + "," + std::to_string(m_state.fbheight) + ");\n";
  }
  else
  {
    m_defines += "const vec2 cResolution = vec2(" + std::to_string(Width()) + ".," + std::to_string(Height()) + ".);\n";
  }

  m_defines += "uniform sampler2D iChannel0;\n";

  if (g_presets[m_currentPreset].channel[1] != -1)
  {
    m_defines += "uniform sampler2D iChannel1;\n";
  }

  if (g_presets[m_currentPreset].channel[2] != -1)
  {
    m_defines += "uniform sampler2D iChannel2;\n";
    m_defines += "#define dNoise\n";
  }
  
  if (g_presets[m_currentPreset].channel[3] != -1)
  {
    m_defines += "uniform sampler2D iChannel3;\n";
  }

  if (g_presets[m_currentPreset].channel[3] == 2)
  {
    m_defines += "uniform vec3 iAlbumPosition;\n";
    m_defines += "uniform vec3 iAlbumRGB;\n";
  }

  if (m_crtCurve)
  {
    m_defines += "#define dCrtCurve\n";
  }

  m_defines += "uniform float iTime;\n";

  if (m_lowpower)
  {
    m_defines += fsCommonFunctionsLowPower;
  }
  else
  {
    m_defines += fsCommonFunctionsNormal;
  }

  kodi::Log(ADDON_LOG_DEBUG, "Fragment shader header\n%s", m_defines.c_str());
}

ADDONCREATOR(CVisualizationMatrix) // Don't touch this!
