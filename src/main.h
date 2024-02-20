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

#include "kissfft/kiss_fft.h"

class ATTR_DLL_LOCAL CVisualizationMatrix
  : public kodi::addon::CAddonBase
  , public kodi::addon::CInstanceVisualization
{
public:
  CVisualizationMatrix();
  ~CVisualizationMatrix() override;

  bool Start(int channels, int samplesPerSec, int bitsPerSample, const std::string& songName) override;
  void Stop() override;
  void AudioData(const float* audioData, size_t audioDataLength) override;
  void Render() override;
  bool GetPresets(std::vector<std::string>& presets) override;
  int GetActivePreset() override;
  bool PrevPreset() override;
  bool NextPreset() override;
  bool LoadPreset(int select) override;
  bool RandomPreset() override;
  bool UpdateAlbumart(const std::string& albumart) override;

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
  void SmoothingOverTime(float* outputBuffer, float* lastOutputBuffer, kiss_fft_cpx* inputBuffer, size_t length, float smoothingTimeConstant, unsigned int fftSize);
  float LinearToDecibels(float linear);
  int DetermineBitsPrecision();
  bool UpdateAlbumart();
  void GatherDefines();
  //double MeasurePerformance(const std::string& shaderPath, int size);

  kiss_fft_cfg m_kissCfg;
  std::vector<GLubyte> m_audioData;
  std::vector<float> m_magnitudeBuffer;
  float* m_pcm;

  bool m_initialized = false;
  int64_t m_initialTime = 0; // in ms
  double m_lastAlbumChange = 0;
  bool m_AlbumNeedsUpload = true;
  bool m_lowpower = false;
  float m_albumX = 0.0;
  float m_albumY = 0.0;
  int m_bitsPrecision = 0;
  int m_currentPreset = 0;
  float m_dotMode = false;
  float m_dotSize = 0.0;
  float m_fallSpeed = 0.25;
  float m_distortThreshold = 0.0;
  float m_noiseFluctuation = 0.0;
  float m_rainHighlights = 0.0;
  bool m_crtCurve = false;

  int m_samplesPerSec = 0; // Given by Start(...)
  bool m_needsUpload = true; // Set by AudioData(...) to mark presence of data

  std::string m_albumArt = "";
  std::string m_defines = "";

  //GLint m_attrResolutionLoc = 0;
  GLint m_attrGlobalTimeLoc = 0;
  GLint m_attrAlbumPositionLoc = 0;
  GLint m_attrAlbumRGBLoc = 0;
  //GLint m_attrChannelTimeLoc = 0;
  //GLint m_attrMouseLoc = 0;
  //GLint m_attrDateLoc = 0;
  //GLint m_attrSampleRateLoc = 0;
  //GLint m_attrChannelResolutionLoc = 0;
  GLint m_attrChannelLoc[4] = {0};
  GLuint m_channelTextures[4] = {0};
  //GLint m_attrDotSizeLoc = 0;

  kodi::gui::gl::CShaderProgram m_matrixShader;
  //kodi::gui::gl::CShaderProgram m_displayShader;

  struct
  {
    float red;
    float green;
    float blue;
  } m_dotColor;

  struct
  {
    GLuint vertex_buffer;
    GLuint attr_vertex_e;
    GLuint attr_vertex_r, uTexture;
    GLuint effect_fb;
    GLuint framebuffer_texture;
    GLuint uScale;
    int fbwidth, fbheight;
  } m_state;

  std::string m_usedShaderFile;
  struct ShaderPath
  {
    bool audio = false;
    std::string texture;
  } m_shaderTextures[4];
};
