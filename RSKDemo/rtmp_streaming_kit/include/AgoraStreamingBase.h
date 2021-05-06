//
//  Agora Streaming Base
//
//  Created by Haonong Yu in 2020-02.
//  Copyright (c) 2020 Agora.io. All rights reserved.
//

#pragma once  // NOLINT(build/header_guard)

#include "AgoraBase.h"
#include "AgoraMediaBase.h"
#include "AgoraRefPtr.h"

namespace agora {
namespace streaming {

// The video orientation mode of the video.
// GENERATED_JAVA_ENUM_PACKAGE: io.agora.streaming
enum ORIENTATION_MODE {
  // Landscape mode.
  // The output video is always in landscape mode. If the captured video is in portrait mode, the
  // video encoder crops it to fit the output.
  ORIENTATION_MODE_FIXED_LANDSCAPE = 1,

  // Portrait mode.
  // The output video is always in portrait mode. If the captured video is in landscape mode, the
  // video encoder crops it to fit the output.
  ORIENTATION_MODE_FIXED_PORTRAIT = 2,
};

// The video mirror mode.
// GENERATED_JAVA_ENUM_PACKAGE: io.agora.streaming
enum VIDEO_MIRROR_MODE_TYPE {
	// (Default) 0: The mode mode determined by the SDK.
	VIDEO_MIRROR_MODE_AUTO = 0,
	// 1: Enable the mirror mode.
	VIDEO_MIRROR_MODE_ENABLED = 1,
	// 2: Disable the mirror mode.
	VIDEO_MIRROR_MODE_DISABLED = 2,
};

// The video encoding mode.
// GENERATED_JAVA_ENUM_PACKAGE: io.agora.streaming
enum VIDEO_ENCODING_MODE_TYPE {
	// (Default) 0: The mode mode determined by the SDK.
	// VIDEO_ENCODING_MODE_AUTO = 0,
	// 1: Enable hardware encoder.
	VIDEO_ENCODING_MODE_HARDWARE = 1,
	// 2: Enable software encoder.
	VIDEO_ENCODING_MODE_SOFTWARE = 2,
};

/**
 * Configurations for the RTMP audio stream.
 */
struct AudioStreamConfiguration {
  /**
   * Sampling rate (Hz). The default value is 44100 (i.e. 44.1kHz).
   */
  int sampleRateHz;

  /**
   * Number of bytes per sample. The default value is 2 (i.e. 16-bit PCM).
   */
  int bytesPerSample;

  /**
   * Number of channels. The default value is 1 (i.e. mono).
   */
  int numberOfChannels;

  /**
   * The target bitrate (Kbps) of the output audio stream to be published.
   * The default value is 48 Kbps.
  */
  int bitrate;

  AudioStreamConfiguration(): sampleRateHz(44100), bytesPerSample(2),
      numberOfChannels(1), bitrate(48) {}
};

/**
 * Configurations for the RTMP video stream.
 */
struct VideoStreamConfiguration {
  /**
   * The width (in pixels) of the video. The default value is 640.
   *
   * @note
   * - The value of the dimension (with the |height| below) does not indicate the orientation mode
   * of the output ratio. For how to set the video orientation,
   * see {@link OrientationMode OrientationMode}.
   */
  int width;

  /**
   * The height (in pixels) of the video. The default value is 360.
   *
   * @note
   * - The value of the dimension (with the |width| above) does not indicate the orientation mode
   * of the output ratio. For how to set the video orientation,
   * see {@link OrientationMode OrientationMode}.
   */
  int height;

  /**
   * Frame rate (fps) of the output video stream to be published. The default
   * value is 15 fps.
   */
  int framerate;

  /**
   * The target bitrate (Kbps) of the output video stream to be published.
   * The default value is 800 Kbps.
   */
  int bitrate;

  /**
   *  (For future use) The maximum bitrate (Kbps) for video.
   *  The default value is 960 Kbps.
   */
  int maxBitrate;

  /**
   *  (For future use) The minimum bitrate (Kbps) for video.
   *  The default value is 600 Kbps.
   */
  int minBitrate;

  /**
   * The orientation mode.
   * See {@link ORIENTATION_MODE ORIENTATION_MODE}.
   */
  ORIENTATION_MODE orientationMode;

	/**
	 * If mirror_type is set to VIDEO_MIRROR_MODE_ENABLED, then the video frame would be mirrored before encoding. Default is VIDEO_MIRROR_MODE_DISABLED.
	 */
	VIDEO_MIRROR_MODE_TYPE mirrorMode;

  /**
   * The video encoding mode.
   * See {@link VIDEO_ENCODING_MODE_TYPE VIDEO_ENCODING_MODE_TYPE}.
   */
  VIDEO_ENCODING_MODE_TYPE videoEncodingMode;

  VideoStreamConfiguration(): width(360), height(640), framerate(15),
      bitrate(800), maxBitrate(960), minBitrate(600),
      orientationMode(ORIENTATION_MODE_FIXED_PORTRAIT),
			mirrorMode(VIDEO_MIRROR_MODE_DISABLED),
      videoEncodingMode(VIDEO_ENCODING_MODE_HARDWARE) {}
};


}  // namespace streaming
}  // namespace agora
