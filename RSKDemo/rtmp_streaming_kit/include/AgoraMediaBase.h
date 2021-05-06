//
//  Agora Engine SDK
//
//  Created by Sting Feng in 2017-11.
//  Copyright (c) 2017 Agora.io. All rights reserved.

#pragma once  // NOLINT(build/header_guard)
#include <stdint.h>
#include <limits>

namespace agora {

namespace rtc {
typedef unsigned int uid_t;
typedef unsigned int track_id_t;
typedef unsigned int conn_id_t;

static const unsigned int DEFAULT_CONNECTION_ID = 0;
static const unsigned int DUMMY_CONNECTION_ID = (std::numeric_limits<unsigned int>::max)();

/**
 音频路由
 */
enum AudioRoute
{
  /**
   -1: 默认的语音路由
   */
  ROUTE_DEFAULT = -1,
  /**
   耳机
   */
  ROUTE_HEADSET,
  /**
   听筒
   */
  ROUTE_EARPIECE,
  /**
   不带麦的耳机
   */
  ROUTE_HEADSETNOMIC,
  /**
   手机的扬声器
   */
  ROUTE_SPEAKERPHONE,
  /**
   外接的扬声器
   */
  ROUTE_LOUDSPEAKER,
  /**
   蓝牙耳机
   */
  ROUTE_HEADSETBLUETOOTH
};

struct AudioParameters {
  int sample_rate;
  size_t channels;
  size_t frames_per_buffer;

  AudioParameters()
      : sample_rate(0),
        channels(0),
        frames_per_buffer(0) {}
};

}  // namespace rtc
namespace media {
namespace base {

typedef void* view_t;

typedef const char* user_id_t;

static const uint8_t kMaxCodecNameLength = 50;

/**
 * RTP 数据 header 中的信息
 */
struct PacketOptions {
  // Rtp timestamp
  uint32_t timestamp;
  // Audio level indication.
  uint8_t audioLevelIndication;
  PacketOptions()
      : timestamp(0),
        audioLevelIndication(127) {}
};

struct AudioPcmFrame {
  enum : size_t {
    kMaxDataSizeSamples = 3840,
    kMaxDataSizeBytes = kMaxDataSizeSamples * sizeof(int16_t),
  };

  uint32_t capture_timestamp = 0;
  size_t samples_per_channel_ = 0;
  int sample_rate_hz_ = 0;
  size_t num_channels_ = 0;
  size_t bytes_per_sample = 0;
  int16_t data_[kMaxDataSizeSamples] = {0};

  AudioPcmFrame() :
    capture_timestamp(0),
    samples_per_channel_(0),
    sample_rate_hz_(0),
    num_channels_(0),
    bytes_per_sample(0) {}
};

class IAudioFrameObserver {
 public:
  virtual void onFrame(const AudioPcmFrame* frame) = 0;
  virtual ~IAudioFrameObserver() {}
};

/**
 视频像素格式
 */
enum VIDEO_PIXEL_FORMAT {
  /**
   `0`: 未知格式
   */
  VIDEO_PIXEL_UNKNOWN = 0,
  /**
   `1`: I420 格式
   */
  VIDEO_PIXEL_I420 = 1,
  /**
   `2`: BGRA 格式
   */
  VIDEO_PIXEL_BGRA = 2,

  VIDEO_PIXEL_NV21 = 3,
  /**
   `4`: RGBA 格式
   */
  VIDEO_PIXEL_RGBA = 4,
  /**
   `8`: NV12 格式
   */
  VIDEO_PIXEL_NV12 = 8,
  VIDEO_PIXEL_I422 = 16,
};

/**
 视频显示的渲染模式
 */
enum RENDER_MODE_TYPE {
  /**
   1: 视频尺寸等比缩放。优先保证视窗被填满。因视频尺寸与显示视窗尺寸不一致而多出的视频将被截掉。
   */
  RENDER_MODE_HIDDEN = 1,
  /**
   2: 视频尺寸等比缩放。优先保证视频内容全部显示。因视频尺寸与显示视窗尺寸不一致造成的视窗未被填满的区域填充黑色。
   */
  RENDER_MODE_FIT = 2,
  /**
   @deprecated
   该模式已被废弃
   */
  RENDER_MODE_ADAPTIVE = 3,
};

/**
 外部视频帧
 */
struct ExternalVideoFrame {
  /**
   视频 Buffer 类型
   */
  enum VIDEO_BUFFER_TYPE {
    /**
     `1`: 视频 Buffer 格式是原始数据
     */
    VIDEO_BUFFER_RAW_DATA = 1,
  };
  /**
   视频 Buffer 类型，详见 #VIDEO_BUFFER_TYPE
   */
  VIDEO_BUFFER_TYPE type;
  /**
   视频像素格式，详见 #VIDEO_PIXEL_FORMAT
   */
  VIDEO_PIXEL_FORMAT format;
  /**
   视频 buffer
   */
  void* buffer;
  /**
   传入视频帧的行间距，单位为像素而不是字节。对于 Texture，该值指的是 Texture 的宽度
   */
  int stride;
  /**
   传入视频帧的高度
   */
  int height;
  /**
   [原始数据相关字段] 指定左边裁剪掉的像素数量，默认为 0
   */
  int cropLeft;
  /**
   [原始数据相关字段] 指定顶边裁剪掉的像素数量，默认为 0
   */
  int cropTop;
  /**
   [原始数据相关字段] 指定右边裁剪掉的像素数量，默认为 0
   */
  int cropRight;
  /**
   [原始数据相关字段] 指定底边裁剪掉的像素数量，默认为 0
   */
  int cropBottom;
  /**
   [原始数据相关字段] 指定是否对传入的视频组做顺时针旋转操作，可选值为 0， 90， 180， 270，默认为 0
   */
  int rotation;
  /**
   传入的视频帧的时间戳，以毫秒为单位。不正确的时间戳会导致丢帧或者音视频不同步
   */
  long long timestamp;
};
/**
 VideoFrame 结构体

 视频数据的格式为 YUV420。缓冲区给出的是指向指针的指针，但接口使用者不能修改缓冲区的指针，只能修改缓冲区的内容。
 */
struct VideoFrame {
  VIDEO_PIXEL_FORMAT type;
  /**
   视频宽 (px)
   */
  int width;  // width of video frame
  /**
   视频高 (px)
   */
  int height;  // height of video frame
  /**
   YUV 数据中的 Y 缓冲区的行跨度
   */
  int yStride;  // stride of Y data buffer
  /**
   YUV 数据中的 U 缓冲区的行跨度
   */
  int uStride;  // stride of U data buffer
  /**
   YUV 数据中的 V 缓冲区的行跨度
   */
  int vStride;  // stride of V data buffer
  /**
   指向 YUV 数据中的 Y 缓冲区指针的指针
   */
  uint8_t* yBuffer;  // Y data buffer
  /**
   指向 YUV 数据中的 U 缓冲区指针的指针
   */
  uint8_t* uBuffer;  // U data buffer
  /**
   指向 YUV 数据中的 V 缓冲区指针的指针
   */
  uint8_t* vBuffer;  // V data buffer
  /**
   在渲染视频前设置该帧的顺时针旋转角度，目前支持 0 度、90 度、180 度，和 270 度
   */
  int rotation;  // rotation of this frame (0, 90, 180, 270)
  /**
   渲染视频的时间戳。

   你可以使用该时间戳还原音频帧顺序；
   在有视频的场景中（包含使用外部视频源的场景），该参数可以帮助实现音视频同步。

   @note 该时间戳只用于渲染视频，不用于采集视频。
   */
  int64_t renderTimeMs;
  /** 预留参数
   */
  int avsync_type;
};

class IVideoFrameObserver {
 public:
  virtual void onFrame(const VideoFrame* frame) = 0;
  virtual ~IVideoFrameObserver() {}
};

enum MEDIA_PLAYER_SOURCE_TYPE {
  MEDIA_PLAYER_SOURCE_DEFAULT,
  MEDIA_PLAYER_SOURCE_FULL_FEATURED,
  MEDIA_PLAYER_SOURCE_SIMPLE,
};

}

/** IAudioFrameObserver 类
 */
class IAudioFrameObserver {
 public:
 /** 音频帧类型
  */
  enum AUDIO_FRAME_TYPE {
    /** 0: PCM 类型 */
    FRAME_TYPE_PCM16 = 0,
  };
  /**
   音频数据。
   */
  struct AudioFrame {
    /** 音频数据的类型，详见 #AUDIO_FRAME_TYPE
     */
    AUDIO_FRAME_TYPE type;
    /**
     每个声道的采样点数。
     */
    int samplesPerChannel;
    /**
     每个采样点的字节数 (byte)。对于 PCM 音频数据，一般是两个字节。
     */
    int bytesPerSample;
    /**
     声道数：
     - 1: 单声道。
     - 2: 双声道。双声道的音频数据是交叉存储的。
     */
    int channels;
    /**
     音频采样率 (Hz)。
     */
    int samplesPerSec;
    /**
     音频 buffer。

     buffer 大小为 `samplesPerChannel` x `channels` x `bytesPerSample`。
     */
    void* buffer;
    /**
     外部音频帧的渲染时间戳（毫秒）。你可以使用该时间戳还原音频帧的顺序。
     在音视频场景下（包含使用外部视频源的场景），该时间戳可以用于实现音频和视频的同步。
     */
    int64_t renderTimeMs;
    /** 预留参数。 */
    int avsync_type;
  };

 public:
  virtual ~IAudioFrameObserver() = default;
  /**
   * 获取本地用户的原始音频数据
   *
   * SDK 会根据 \ref rtc::IRtcEngine::setRecordingAudioFrameParameters "setRecordingAudioFrameParameters"
   * 方法设置的采样间隔定期触发该回调。你可以从该回调中获取本地采集的音频数据。
   *
   * @param audioFrame 音频帧，详见 AudioFrame
   * @return
   * - true: 音频帧是有效的，会发送回 SDK。
   * - false: 音频帧是无效的，不会发送回 SDK。
   */
  virtual bool onRecordAudioFrame(AudioFrame& audioFrame) = 0;
  /**
   * 获取所有远端用户的原始音频数据
   *
   * SDK 会根据 \ref rtc::IRtcEngine::setPlaybackAudioFrameParameters "setPlaybackAudioFrameParameters"
   * 方法设置的采样间隔定期触发该回调。你可以从该回调中获取所有远端用户在本地设备上播放的音频数据。
   *
   * @param audioFrame 音频帧，详见 AudioFrame
   * @return
   * - true: 音频帧是有效的，会发送回 SDK。
   * - false: 音频帧是无效的，不会发送回 SDK。
   */
  virtual bool onPlaybackAudioFrame(AudioFrame& audioFrame) = 0;
  /**
   * 获取本地用户和所有远端用户的原始音频数据
   *
   * SDK 会根据 \ref rtc::IRtcEngine::setMixedAudioFrameParameters "setMixedAudioFrameParameters"
   * 方法设置的采样间隔定期触发该回调。你可以从该回调中获取所有远端用户在本地设备上播放的音频数据。
   *
   * @param audioFrame 音频帧，详见 AudioFrame
   * @return
   * - true: 音频帧是有效的，会发送回 SDK。
   * - false: 音频帧是无效的，不会发送回 SDK。
   */
  virtual bool onMixedAudioFrame(AudioFrame& audioFrame) = 0;
  /**
   * 获取特定远端用户的原始音频数据
   *
   * 你可以从该回调中获取特定远端用户在本地设备上播放的音频数据。
   * @param uid 远端用户 ID
   * @param audioFrame 音频帧，详见 AudioFrame
   * @return
   * - true: 音频帧是有效的，会发送回 SDK。
   * - false: 音频帧是无效的，不会发送回 SDK。
   */
  virtual bool onPlaybackAudioFrameBeforeMixing(unsigned int uid, AudioFrame& audioFrame) = 0;
};
/**
 IVideoFrameObserver 类
 */
class IVideoFrameObserver {
 public:
  using VideoFrame = media::base::VideoFrame;
  enum VIDEO_OBSERVER_POSITION {
    POSITION_POST_CAPTURER = 1 << 0,
    POSITION_PRE_RENDERER = 1 << 1,
    POSITION_PRE_ENCODER = 1 << 2,
    POSITION_POST_FILTERS = 1 << 3,
  };

   public:
  virtual ~IVideoFrameObserver() {}

  /**
   获取本地摄像头采集到每帧视频帧

   成功注册 IVideoFrameObserver 对象后，SDK 会在捕捉到每个视频帧时触发该回调。
   你可以在回调中获取本地摄像头采集到的视频数据，然后根据场景需要，对视频数据进行前处理。

   完成前处理后，你可以在该回调中，传入处理后的视频数据将其发送回 SDK。

   @param videoFrame 视频帧，详见 VideoFrame

   @return 如果视频前处理失败，是否忽略该视频帧：
   - true: 不忽略，将该视频帧发送回 SDK
   - false: 忽略，该视频帧不再发送回 SDK
   */
  virtual bool onCaptureVideoFrame(VideoFrame& videoFrame) = 0;
  /**
   获取远端用户发送的视频数据

   成功注册 IVideoFrameObserver 对象后，SDK 会在捕捉到每个视频帧时触发该回调。
   你可以在回调中获取远端发送的视频数据，然后根据场景需求，对视频数据进行后处理。

   @param uid 发送视频数据的远端用户 ID
   @param connectionId 连接 ID
   @param videoFrame 视频帧，详见 VideoFrame

   @return 如果视频后处理失败，是否忽略该视频帧：
   - true: 不忽略，将该视频帧发送回 SDK
   - false: 忽略，该视频帧不再发送回 SDK
   */
  virtual bool onRenderVideoFrame(rtc::uid_t uid, rtc::conn_id_t connectionId,
                                  VideoFrame& videoFrame) = 0;

  virtual base::VIDEO_PIXEL_FORMAT getVideoPixelFormatPreference() {
      return base::VIDEO_PIXEL_I420; }
  virtual bool getRotationApplied() { return false; }
  virtual bool getMirrorApplied() { return false; }
  virtual bool isExternal() { return true; }


};
} // namespace rtc


}  // namespace agora
