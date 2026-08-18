/*
 *  Copyright (C) 2005-2021 Team Kodi <https://kodi.tv>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#pragma once

#include <kodi/addon-instance/Visualization.h>
#include <kodi/gui/gl/Shader.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <kissfft/kiss_fft.h>

#include <vector>
#include <string>
#include <memory>

class ATTRIBUTE_HIDDEN CVisualizationMatrix
  : public kodi::addon::CAddonBase
  , public kodi::addon::CInstanceVisualization
{
public:
  CVisualizationMatrix();
  ~CVisualizationMatrix() override;

  // Disable copy and move
  CVisualizationMatrix(const CVisualizationMatrix&) = delete;
  CVisualizationMatrix& operator=(const CVisualizationMatrix&) = delete;
  CVisualizationMatrix(CVisualizationMatrix&&) = delete;
  CVisualizationMatrix& operator=(CVisualizationMatrix&&) = delete;

  bool Start(int channels, int samplesPerSec, int bitsPerSample, std::string songName) override;
  void Stop() override;
  void AudioData(const float* audioData, int audioDataLength, float* freqData, int freqDataLength) override;
  void Render() override;
  bool GetPresets(std::vector<std::string>& presets) override;
  int GetActivePreset() override;
  bool PrevPreset() override;
  bool NextPreset() override;
  bool LoadPreset(int select) override;
  bool RandomPreset() override;
  bool UpdateAlbumart(std::string albumart) override;

private:
  void RenderTo(GLuint shader, GLuint effect_fb);
  void Mix(float* destination, const float* source, size_t frames, size_t channels);
  void WriteToBuffer(const float* input, size_t length, size_t channels);
  void Launch(int preset);
  void LoadPreset(const std::string& shaderPath);
  void UnloadPreset();
  void UnloadTextures();
  GLuint CreateTexture(GLint format, unsigned int w, unsigned int h, const GLvoid* data);
  GLuint CreateTexture(const GLvoid* data, GLint format, unsigned int w, unsigned int h, GLint internalFormat, GLint scaling, GLint repeat);
  GLuint CreateTexture(const std::string& file, GLint internalFormat, GLint scaling, GLint repeat);
  float BlackmanWindow(float in, size_t i, size_t length);
  void SmoothingOverTime(std::vector<float>& outputBuffer, const std::vector<float>& lastOutputBuffer, 
                         kiss_fft_cpx* inputBuffer, size_t length, float smoothingTimeConstant, unsigned int fftSize);
  float LinearToDecibels(float linear);
  int DetermineBitsPrecision();
  bool UpdateAlbumart();
  void GatherDefines();

  // FFT configuration
  static constexpr size_t AUDIO_BUFFER = 1024;
  static constexpr size_t NUM_BANDS = AUDIO_BUFFER / 2;

  // Audio processing
  kiss_fft_cfg m_kissCfg;
  std::vector<GLubyte> m_audioData;
  std::vector<float> m_magnitudeBuffer;
  std::vector<float> m_pcm;

  // State
  bool m_initialized = false;
  int64_t m_initialTime = 0; // in ms
  double m_lastAlbumChange = 0;
  bool m_AlbumNeedsUpload = true;
  bool m_lowpower = false;
  float m_albumX = 0.0f;
  float m_albumY = 0.0f;
  int m_bitsPrecision = 0;
  int m_currentPreset = 0;
  float m_dotMode = false;
  float m_dotSize = 0.0f;
  float m_fallSpeed = 0.25f;
  float m_distortThreshold = 0.0f;
  float m_noiseFluctuation = 0.0f;
  float m_rainHighlights = 0.0f;
  bool m_crtCurve = false;

  int m_samplesPerSec = 0; // Given by Start(...)
  bool m_needsUpload = true; // Set by AudioData(...) to mark presence of data

  std::string m_albumArt;
  std::string m_defines;
  std::string m_usedShaderFile;

  // OpenGL locations
  GLint m_attrGlobalTimeLoc = 0;
  GLint m_attrAlbumPositionLoc = 0;
  GLint m_attrAlbumRGBLoc = 0;
  GLint m_attrChannelLoc[4] = {0};
  GLuint m_channelTextures[4] = {0};

  kodi::gui::gl::CShaderProgram m_matrixShader;

  struct DotColor
  {
    float red = 0.0f;
    float green = 0.0f;
    float blue = 0.0f;
  } m_dotColor;

  struct State
  {
    GLuint vertex_buffer = 0;
    GLuint attr_vertex_e = 0;
    GLuint attr_vertex_r = 0;
    GLuint uTexture = 0;
    GLuint effect_fb = 0;
    GLuint framebuffer_texture = 0;
    GLuint uScale = 0;
    int fbwidth = 0;
    int fbheight = 0;
  } m_state;

  struct ShaderPath
  {
    bool audio = false;
    std::string texture;
  } m_shaderTextures[4];
};
