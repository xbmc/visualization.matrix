/*
 *  Copyright (C) 2024 Team Kodi <https://kodi.tv>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#include <gtest/gtest.h>
#include <cmath>
#include <vector>

// Include the functions to test from main.cpp
// We need to extract the pure functions that don't depend on OpenGL/Kodi

// --- Copied from main.cpp for testing ---

#define AUDIO_BUFFER (1024)
#define NUM_BANDS (AUDIO_BUFFER / 2)

struct kiss_fft_cpx
{
  float r;
  float i;
};

// BlackmanWindow function (copied from main.cpp)
float BlackmanWindow(float in, size_t i, size_t length)
{
  double alpha = 0.16;
  double a0 = 0.5 * (1.0 - alpha);
  double a1 = 0.5;
  double a2 = 0.5 * alpha;

  float x = (float)i / (float)length;
  return in * (a0 - a1 * cos(2.0 * M_PI * x) + a2 * cos(4.0 * M_PI * x));
}

// LinearToDecibels function (copied from main.cpp)
float LinearToDecibels(float linear)
{
  if (!linear)
    return -1000;
  return 20 * log10f(linear);
}

// SmoothingOverTime function (copied from main.cpp)
void SmoothingOverTime(float* outputBuffer, float* lastOutputBuffer, kiss_fft_cpx* inputBuffer, size_t length, float smoothingTimeConstant, unsigned int fftSize)
{
  for (size_t i = 0; i < length; i++)
  {
    kiss_fft_cpx c = inputBuffer[i];
    float magnitude = sqrt(c.r * c.r + c.i * c.i) / (float)fftSize;
    outputBuffer[i] = smoothingTimeConstant * lastOutputBuffer[i] + (1.0 - smoothingTimeConstant) * magnitude;
  }
}

// --- Tests ---

class VisualizationMatrixTest : public ::testing::Test
{
protected:
  void SetUp() override {}
  void TearDown() override {}
};

// Test BlackmanWindow function
TEST_F(VisualizationMatrixTest, BlackmanWindow_ZeroInput)
{
  float result = BlackmanWindow(0.0f, 0, AUDIO_BUFFER);
  EXPECT_FLOAT_EQ(result, 0.0f);
}

TEST_F(VisualizationMatrixTest, BlackmanWindow_NonZeroInput)
{
  float result = BlackmanWindow(1.0f, 0, AUDIO_BUFFER);
  // At position 0, the window should be: a0 - a1*cos(0) + a2*cos(0) = a0 - a1 + a2
  double alpha = 0.16;
  double a0 = 0.5 * (1.0 - alpha);
  double a1 = 0.5;
  double a2 = 0.5 * alpha;
  float expected = a0 - a1 + a2;
  EXPECT_NEAR(result, expected, 0.0001f);
}

TEST_F(VisualizationMatrixTest, BlackmanWindow_MiddlePosition)
{
  float result = BlackmanWindow(1.0f, AUDIO_BUFFER / 2, AUDIO_BUFFER);
  // At middle position, x = 0.5
  float x = 0.5f;
  double alpha = 0.16;
  double a0 = 0.5 * (1.0 - alpha);
  double a1 = 0.5;
  double a2 = 0.5 * alpha;
  float expected = a0 - a1 * cos(2.0 * M_PI * x) + a2 * cos(4.0 * M_PI * x);
  EXPECT_NEAR(result, expected, 0.0001f);
}

TEST_F(VisualizationMatrixTest, BlackmanWindow_EndPosition)
{
  float result = BlackmanWindow(1.0f, AUDIO_BUFFER - 1, AUDIO_BUFFER);
  // At end position, x ~ 1.0
  float x = (float)(AUDIO_BUFFER - 1) / (float)AUDIO_BUFFER;
  double alpha = 0.16;
  double a0 = 0.5 * (1.0 - alpha);
  double a1 = 0.5;
  double a2 = 0.5 * alpha;
  float expected = a0 - a1 * cos(2.0 * M_PI * x) + a2 * cos(4.0 * M_PI * x);
  EXPECT_NEAR(result, expected, 0.0001f);
}

// Test LinearToDecibels function
TEST_F(VisualizationMatrixTest, LinearToDecibels_Zero)
{
  float result = LinearToDecibels(0.0f);
  EXPECT_FLOAT_EQ(result, -1000.0f);
}

TEST_F(VisualizationMatrixTest, LinearToDecibels_One)
{
  float result = LinearToDecibels(1.0f);
  EXPECT_FLOAT_EQ(result, 0.0f);  // 20 * log10(1) = 0
}

TEST_F(VisualizationMatrixTest, LinearToDecibels_Ten)
{
  float result = LinearToDecibels(10.0f);
  EXPECT_NEAR(result, 20.0f, 0.0001f);  // 20 * log10(10) = 20
}

TEST_F(VisualizationMatrixTest, LinearToDecibels_Half)
{
  float result = LinearToDecibels(0.5f);
  EXPECT_NEAR(result, -6.0206f, 0.0001f);  // 20 * log10(0.5) ≈ -6.0206
}

TEST_F(VisualizationMatrixTest, LinearToDecibels_SmallValue)
{
  float result = LinearToDecibels(0.001f);
  EXPECT_NEAR(result, -60.0f, 0.0001f);  // 20 * log10(0.001) = -60
}

// Test SmoothingOverTime function
TEST_F(VisualizationMatrixTest, SmoothingOverTime_ZeroInput)
{
  std::vector<float> outputBuffer(NUM_BANDS, 0.0f);
  std::vector<float> lastOutputBuffer(NUM_BANDS, 1.0f);
  std::vector<kiss_fft_cpx> inputBuffer(NUM_BANDS);
  
  for (size_t i = 0; i < NUM_BANDS; i++)
  {
    inputBuffer[i].r = 0.0f;
    inputBuffer[i].i = 0.0f;
  }
  
  SmoothingOverTime(outputBuffer.data(), lastOutputBuffer.data(), inputBuffer.data(), NUM_BANDS, 0.5f, AUDIO_BUFFER);
  
  for (size_t i = 0; i < NUM_BANDS; i++)
  {
    // With zero input, output should be: 0.5 * lastOutputBuffer[i] + 0.5 * 0 = 0.5 * lastOutputBuffer[i]
    EXPECT_NEAR(outputBuffer[i], 0.5f, 0.0001f);
  }
}

TEST_F(VisualizationMatrixTest, SmoothingOverTime_NonZeroInput)
{
  std::vector<float> outputBuffer(NUM_BANDS, 0.0f);
  std::vector<float> lastOutputBuffer(NUM_BANDS, 0.0f);
  std::vector<kiss_fft_cpx> inputBuffer(NUM_BANDS);
  
  // Set up input buffer with magnitude 1.0 for all bands
  for (size_t i = 0; i < NUM_BANDS; i++)
  {
    inputBuffer[i].r = AUDIO_BUFFER;  // magnitude * fftSize = 1.0 * 1024
    inputBuffer[i].i = 0.0f;
  }
  
  SmoothingOverTime(outputBuffer.data(), lastOutputBuffer.data(), inputBuffer.data(), NUM_BANDS, 0.0f, AUDIO_BUFFER);
  
  for (size_t i = 0; i < NUM_BANDS; i++)
  {
    // With smoothingTimeConstant = 0, output should be: 0 * last + 1 * magnitude = magnitude
    // magnitude = sqrt(r^2 + i^2) / fftSize = 1024 / 1024 = 1.0
    EXPECT_NEAR(outputBuffer[i], 1.0f, 0.0001f);
  }
}

TEST_F(VisualizationMatrixTest, SmoothingOverTime_FullSmoothing)
{
  std::vector<float> outputBuffer(NUM_BANDS, 0.0f);
  std::vector<float> lastOutputBuffer(NUM_BANDS, 1.0f);
  std::vector<kiss_fft_cpx> inputBuffer(NUM_BANDS);
  
  // Set up input buffer with magnitude 0.0 for all bands
  for (size_t i = 0; i < NUM_BANDS; i++)
  {
    inputBuffer[i].r = 0.0f;
    inputBuffer[i].i = 0.0f;
  }
  
  // With smoothingTimeConstant = 1.0, output should be: 1.0 * lastOutputBuffer[i] + 0 * magnitude = lastOutputBuffer[i]
  SmoothingOverTime(outputBuffer.data(), lastOutputBuffer.data(), inputBuffer.data(), NUM_BANDS, 1.0f, AUDIO_BUFFER);
  
  for (size_t i = 0; i < NUM_BANDS; i++)
  {
    EXPECT_NEAR(outputBuffer[i], 1.0f, 0.0001f);
  }
}

// Test edge cases
TEST_F(VisualizationMatrixTest, BlackmanWindow_NegativeInput)
{
  float result = BlackmanWindow(-1.0f, 0, AUDIO_BUFFER);
  EXPECT_FLOAT_EQ(result, -BlackmanWindow(1.0f, 0, AUDIO_BUFFER));
}

TEST_F(VisualizationMatrixTest, LinearToDecibels_NegativeInput)
{
  // Negative linear values should still work (though physically meaningless)
  float result = LinearToDecibels(-1.0f);
  // log10 of negative number is NaN, but we handle it gracefully
  // In practice, this should not happen with magnitude values
  EXPECT_TRUE(std::isnan(result) || result == -1000.0f);
}
