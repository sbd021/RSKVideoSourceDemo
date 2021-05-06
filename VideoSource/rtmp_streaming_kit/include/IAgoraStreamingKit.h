//
//  Agora Streaming Kit
//
//  Copyright (c) 2020 Agora.io. All rights reserved.
//

#ifndef AGORA_STREAMING_KIT_H_
#define AGORA_STREAMING_KIT_H_

#include "AgoraBase.h"
#include "IAgoraLog.h"
#include "AgoraMediaBase.h"
#include "AgoraStreamingBase.h"

namespace agora {
namespace streaming {

class IAudioDeviceManager;
class IVideoDeviceManager;
class IVideoPreviewRenderer;
class ISnapshotCallback;

// The maximum device ID length.
// GENERATED_JAVA_ENUM_PACKAGE: io.agora.streaming
enum MAX_DEVICE_ID_LENGTH_TYPE {
  // The maximum device ID length is 512.
  MAX_DEVICE_ID_LENGTH = 512
};

// The error codes for initializing streaming kit.
// GENERATED_JAVA_ENUM_PACKAGE: io.agora.streaming
enum INITIALIZE_ERROR {
  // No error occurs.
  INITIALIZE_KIT_ERR_OK = 0,
  // A general error occurs (no specified reason).
  INITIALIZE_KIT_ERR_FAILED = 1,
  // Invalid argument used.
  INITIALIZE_KIT_ERR_INVALID_ARGUMENT = 2,
  // Streaming kit is already initialized.
  INITIALIZE_KIT_ERR_ALREADY_INITIALIZED = 3,
  // Error occurs while configuring audio
  INITIALIZE_KIT_ERR_AUDIO_CONFIG = 4,
  // Error occurs while configuring video
  INITIALIZE_KIT_ERR_VIDEO_CONFIG = 5,
};

// The error codes for starting media streaming.
// GENERATED_JAVA_ENUM_PACKAGE: io.agora.streaming
enum START_STREAMING_ERROR {
  // No error occurs.
  START_STREAMING_ERR_OK = 0,
  // A general error occurs (no specified reason).
  START_STREAMING_ERR_FAILED = 1,
  // Streaming kit is not initialized.
  START_STREAMING_ERR_NOT_INITIALIZED = 2,
  // Streaming kit is already started.
  START_STREAMING_ERR_ALREADY_STARTED = 3,
  // Failed to open audio device.
  START_STREAMING_ERR_OPEN_AUDIO_DEVICE = 4,
  // Failed to open video device.
  START_STREAMING_ERR_OPEN_VIDEO_DEVICE = 5,
  // Failed to connect RTMP.
  START_STREAMING_ERR_CONNECT_RTMP = 6,
  // Invalid argument used.
  START_STREAMING_ERR_INVALID_ARGUMENT = 7,
};

// The error codes for media streaming
// GENERATED_JAVA_ENUM_PACKAGE: io.agora.streaming
enum MEDIA_STREAMING_ERROR {
  // No error occurs.
  MEDIA_STREAMING_ERR_OK = 0,
  // A general error occurs (no specified reason).
  MEDIA_STREAMING_ERR_FAILED = 1,
  // Audio publication error.
  MEDIA_STREAMING_ERR_AUDIO_PUBLICATION = 2,
  // Video publication error.
  MEDIA_STREAMING_ERR_VIDEO_PUBLICATION = 3,
};

// The connection state of media streaming
// GENERATED_JAVA_ENUM_PACKAGE: io.agora.streaming
enum STREAMING_CONNECTION_STATE {
  // The SDK is disconnected from streaming server.
  STREAMING_CONNECTION_STATE_DISCONNECTED = 0,
  // The SDK is connected to streaming server.
  STREAMING_CONNECTION_STATE_CONNECTED = 1,
  // The SDK is trying to reconnect to streaming server.
  STREAMING_CONNECTION_STATE_RECONNECTING = 2,
  // The SDK fails to connect to streaming server.
  STREAMING_CONNECTION_STATE_FAILED = 3,
};

// The video display mode.
// GENERATED_JAVA_ENUM_PACKAGE: io.agora.streaming
enum RENDER_MODE_TYPE {
  // Uniformly scale the video until it fills the visible boundaries
  // (cropped). One dimension of the video may have clipped contents.
  RENDER_MODE_HIDDEN = 1,
  // Uniformly scale the video until one of its dimension fits the boundary
  // (zoomed to fit). Areas that are not filled due to the disparity in the
  // aspect ratio will be filled with black.
  RENDER_MODE_FIT = 2,
};

// GENERATED_JAVA_ENUM_PACKAGE: io.agora.streaming
enum LOG_FILTER_TYPE {
  LOG_FILTER_OFF = 0,
  LOG_FILTER_DEBUG = 0x080f,
  LOG_FILTER_INFO = 0x000f,
  LOG_FILTER_WARN = 0x000e,
  LOG_FILTER_ERROR = 0x000c,
  LOG_FILTER_CRITICAL = 0x0008,
};

/**
 * The IStreamingEventHandler class.
 *
 * The SDK uses this class to send callback event notifications to the app, and the app inherits
 * the methods in this class to retrieve these event notifications.
 *
 * All methods in this class have their default (empty) implementations, and the app can inherit
 * only some of the required events instead of all. In the callback methods, the app should avoid
 * time-consuming tasks or calling blocking APIs, otherwise the SDK may not work properly.
 */
class IStreamingEventHandler {
 public:
  virtual ~IStreamingEventHandler() {}

  virtual void onStartStreamingSuccess() {}

  virtual void onStartStreamingFailure(START_STREAMING_ERROR err, const char* msg) {
    (void)err;
    (void)msg;
  }

  virtual void onMediaStreamingError(MEDIA_STREAMING_ERROR err, const char* msg) {
    (void)err;
    (void)msg;
  };

  virtual void onStreamingConnectionStateChanged(STREAMING_CONNECTION_STATE state) {
      (void)state;
  };
};

/** The definition of StreamingContext.
 */
struct StreamingContext {
  /**
   * The event handler for IStreamingKit.
   */
  IStreamingEventHandler* eventHandler;

  /**
   * The App ID issued to the developers by Agora. Apply for a new one from Agora
   * if it is missing from your kit.
   */
  const char* appId;

  /**
   * For Android, it is the context of Activity or Application.
   */
  void *context;

  /**
   * Enables or disables audio streaming.
   *
   * Audio streaming is enabled by default.
   *
   * @param enabled Determines whether to enable audio streaming:
   * - true: (Default) Enable audio streaming, that is, to start publish the
   * audio stream when streaming is started .
   * - false: Disable audio streaming.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  bool enableAudioStreaming;

  /**
   * Enables or disables video streaming.
   *
   * Video streaming is enabled by default.
   *
   * @param enabled Determines whether to enable video streaming:
   * - true: (Default) Enable video streaming, that is, to publish the video
   * stream when streaming is started .
   * - false: Disable video streaming.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  bool enableVideoStreaming;

  /**
   * The configuration for the audio stream
   */
  AudioStreamConfiguration audioStreamConfiguration;

  /**
   * The configuration for the video stream
   */
  VideoStreamConfiguration videoStreamConfiguration;

  StreamingContext()
      : eventHandler(NULL), appId(NULL), context(NULL), enableAudioStreaming(true),
      enableVideoStreaming(true) {}
};

/**
 * The IStreamingKit class.
 *
 * This class provides the main methods that can be invoked by your app.
 *
 * Creating an IStreamingKit object and then calling the methods of this object
 * enables you to use Agora SDK's live streaming functionality.
 */
class IStreamingKit {
 public:
  /**
   * Initializes the Agora streaming kit.
   *
   * In this method, you need to enter the issued Agora App ID to start initialization.
   * After creating an IStreamingKit object, call this method to initialize the kit
   * before calling any other methods.
   * @param context The StreamingContext object.
   * @param audioTrack The local audio track. Must not be NULL if audio streaming is enabled.
   * @param videoTrack The local video track. Must not be NULL if video streaming is enabled.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int initialize(const StreamingContext& context) = 0;

  /**
   * Releases the IStreamingKit object.
   *
   * @param sync Determines whether this method is a synchronous call.
   * - true: This method is a synchronous call, which means that the result of this method
   * call returns after the IStreamingKit object resources are released. Do not call this
   * method in any callback generated by the streaming kit, or it may result in a deadlock.
   * - false: This method is an asynchronous call. The result returns immediately even when
   * the IStreamingKit object resources are not released.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual void release(bool sync = false) = 0;

  /**
   * Start media streaming
   *
   * @param publishUrl The CDN streaming URL in the RTMP format. The maximum length of this
   * parameter is 1024 bytes. The URL address must not contain special characters, such as Chinese
   * language characters.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int startStreaming(const char* publishUrl) = 0;

  /**
   * stop media streaming
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int stopStreaming() = 0;

  /**
   * Enables or disables audio recording.
   *
   * @param enabled Determines whether to disable or re-enable audio recording:
   * - true: (Default) Re-enable audio recording.
   * - false: Disable audio recording.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int enableAudioRecording(bool enabled) = 0;

  /**
   * Enables or disables video capturing.
   *
   * @param enabled Determines whether to disable or re-enable video capturing:
   * - true: (Default) Re-enable video capturing.
   * - false: Disable video capturing.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int enableVideoCapturing(bool enabled) = 0;

  /**
   * Stops or resumes publishing the audio stream.
   *
   * @param muted Determines whether to publish or stop publishing the audio stream:
   * - true: Stop publishing the audio stream.
   * - false: (Default) Publish the audio stream.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int muteAudioStream(bool muted) = 0;

  /**
   * Stops or resumes publishing the video stream.
   *
   * @param muted Determines whether to publish or stop publishing the video stream:
   * - true: Stop publishing the video stream.
   * - false: (Default) Publish the video stream.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int muteVideoStream(bool muted) = 0;

  /**
   * Sets the audio stream configuration.
   *
   * Each configuration corresponds to a set of audio parameters, including
   * the audio sample rate, bytes per sample, number of channels, and bitrate.
   *
   * @param config The audio stream configuration: AudioStreamConfiguration.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setAudioStreamConfiguration(const AudioStreamConfiguration& config) = 0;

  /**
   * Sets the video stream configuration.
   *
   * Each configuration corresponds to a set of video parameters, including
   * the resolution, frame rate, and bitrate.
   *
   * @param config The video stream configuration: VideoStreamConfiguration.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setVideoStreamConfiguration(const VideoStreamConfiguration& config) = 0;

 /**
   * switch the video resolution on preview.
   *
   * Minimum resolution support 64*64
   * Maximum resolution support 1920*1080
   *
   * @param width The width of video.
   * @param height The height of video.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int switchResolution(int width, int height) = 0;

 /**
   * snapshot a picture on pushing.
   *
   * @param callback callback a bitmap
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int snapshot(agora::streaming::ISnapshotCallback* callback) = 0;

  /**
   * Adjusts the recording volume.
   *
   * @param volume The recording volume, which ranges from 0 to 400:
   * - 0: Mute the recoridng volume.
   * - 100: The Original volume.
   * - 400: (Maximum) Four times the original volume with signal clipping
   * protection.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int adjustRecordingSignalVolume(int volume) = 0;

	/** Enables the external audio source.
	 *
	 * Call this method before push streaming.
	 * @param enable Determines whether to enable the external audio source.
	 * @return
	 * 0: Success.
	 * < 0: Failure.
	*/
	virtual int setExternalAudioSource(bool enable) = 0;

	/** Enables the external video source.
	 *
	 * Call this method before push streaming.
	 * @param enable Determines whether to enable the external video source.
	 * @return
	 * 0: Success.
	 * < 0: Failure.
	*/
	virtual int setExternalVideoSource(bool enable) = 0;

	/**
	 * Pushes the external video frame to the app.
	 *
	 * @param frame The video buffer data.
	 * @return
	 * - 0: Success.
	 * - < 0: Failure.
	 */
	virtual int pushExternalVideoFrame(const media::IVideoFrameObserver::VideoFrame& Frame) = 0;

	/**
	 * Pushes the external audio data to the app.
	 *
	 * @param frame The audio buffer data.
	 * @return
	 * - 0: Success.
	 * - < 0: Failure.
	 */
	virtual int pushExternalAudioFrame(const media::IAudioFrameObserver::AudioFrame& frame) = 0;

  /**
   * Get the audio device manager
   *
   * @return
   * - Returns an IAudioDeviceManager object that provides the method to manage the
   * audio devices when the call succeeds.
   * - Returns NULL when the call fails (for example, custom audio input is used).
   */
  virtual IAudioDeviceManager* getAudioDeviceManager() = 0;

  /**
   * Get the video device manager
   *
   * @return
   * - Returns an IVideoDeviceManager object that provides the method to manage the
   * video devices when the call succeeds.
   * - Returns NULL when the call fails (for example, custom video input is used).
   */
  virtual IVideoDeviceManager* getVideoDeviceManager() = 0;

  /**
   * Get the video preview renderer
   *
   * @return
   * - Returns an IVideoPreviewRenderer object that provides the method to manger the
   * video preview rendering when the call succeeds.
   * - Returns NULL when the call fails.
   */
  virtual IVideoPreviewRenderer* getVideoPreviewRenderer() = 0;

  /**
   * Registers an audio frame observer object.
   *
   * @param observer A pointer to the audio frame observer object: IAudioFrameObserver.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int registerAudioFrameObserver(media::base::IAudioFrameObserver* observer) = 0;

  /**
   * Unregisters an audio frame observer object.
   *
   * @param observer A pointer to the audio frame observer object: IAudioFrameObserver.
   */
  virtual void unregisterAudioFrameObserver(media::base::IAudioFrameObserver* observer) = 0;

  /**
   * Registers a video frame observer object.
   *
   * @param observer A pointer to the video frame observer: IVideoFrameObserver.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int registerVideoFrameObserver(media::base::IVideoFrameObserver* observer) = 0;

  /**
   * Unregisters a video frame observer object.
   *
   * @param observer A pointer to the video frame observer: IVideoFrameObserver.
   */
  virtual void unregisterVideoFrameObserver(media::base::IVideoFrameObserver* observer) = 0;

  /**
   * Specifies an output log file for the streaming kit.
   *
   * The log file records all log data for the kit's operation. Ensure that the
   * directory for the log file exists and is writable.
   *
   * @note
   * Ensure that you call this method immediately after \ref initialize "initialize",
   * or the output log may not be complete.
   *
   * @param filePath File path of the log file. The string of the log file is in UTF-8.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setLogFile(const char* filePath) = 0;

  /**
   * Sets the output log level of the kit.
   *
   * You can use one or a combination of the filters. The log level follows the
   * sequence of `OFF`, `CRITICAL`, `ERROR`, `WARNING`, `INFO`, and `DEBUG`. Choose a level
   * and you will see logs preceding that level. For example, if you set the log level to
   * `WARNING`, you see the logs within levels `CRITICAL`, `ERROR`, and `WARNING`.
   *
   * @param filter Sets the log filter level:
   * - LOG_FILTER_DEBUG: Output all API logs. Set your log filter as DEBUG
   * if you want to get the most complete log file.
   * - LOG_FILTER_INFO: Output logs of the CRITICAL, ERROR, WARNING, and INFO
   * level. We recommend setting your log filter as this level.
   * - LOG_FILTER_WARNING: Output logs of the CRITICAL, ERROR, and WARNING level.
   * - LOG_FILTER_ERROR: Output logs of the CRITICAL and ERROR level.
   * - LOG_FILTER_CRITICAL: Output logs of the CRITICAL level.
   * - LOG_FILTER_OFF: Do not output any log.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setLogFilter(unsigned int filter) = 0;

  /** Sets the log file size (KB).
   *
   * The SDK has two log files, each with a default size of 512 KB. If you set @p
   * fileSizeInBytes as 1024 KB, the SDK outputs log files with a total maximum size of 2 MB.
   * If the total size of the log files exceed the set value,
   * the new output log files overwrite the old output log files.
   *
   * @param fileSizeInKBytes The SDK log file size (KB).
   * @return
   * - 0: Success.
   * - <0: Failure.
   */
  virtual int setLogFileSize(unsigned int fileSizeInKBytes) = 0;

#if defined(__ANDROID__)
  /**
  * start screen capture
  * @param everything can start system services
  * @param int screen width
  * @param int screen height
  * @return int
  * < 0 FAILED
  * 0 SUCCESS
  */
  virtual int startScreenCapture(void *mediaProjectionPermissionResultData, int width, int height) = 0;

  /**
  * stop screen capture
  * @param none
  * @return none
  */
  virtual void stopScreenCapture() = 0;
#endif
  virtual ~IStreamingKit() {}
 protected:
 
};

/**
 * The IAudioDeviceCollection class.
 */
class IAudioDeviceCollection {
 public:
  /**
   * Gets the total number of recording devices.
   *
   * Call \ref IAudioDeviceManager::enumerateRecordingDevices "enumerateRecordingDevices"()
   * first, and then call this method to return the number of the audio recording devices.
   *
   * @return The number of the audio recording devices.
   */
  virtual int getNumberOfRecordingDevices() = 0;

  /**
   * Gets the specified information about an audio recording device.
   *
   * @param index An input parameter that specifies the device.
   * @param deviceName An output parameter that indicates the device name.
   * @param deviceId An output parameter that indicates the device ID.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int getRecordingDeviceInfo(int index, char deviceName[MAX_DEVICE_ID_LENGTH],
                                     char deviceId[MAX_DEVICE_ID_LENGTH]) = 0;

  /**
   * Releases all IAudioDeviceCollection resources.
   */
  virtual void release() = 0;

 protected:
  virtual ~IAudioDeviceCollection() {};
};

/**
 * The IAudioDeviceManager class.
 */
class IAudioDeviceManager {
 public:
  /**
   * Enumerates the audio recording devices.
   *
   * This method returns an IAudioDeviceCollection object that includes all the
   * audio recording devices in the system. With the IAudioDeviceCollection object,
   * the application can enumerate the audio recording devices. The application
   * needs to call the IAudioDeviceCollection::release() method to release the
   * eturned object after using it.
   *
   * @return
   * - Returns an IAudioDeviceCollection object that includes all the audio
   * recording devices in the system when the call succeeds.
   * - Returns NULL when the call fails.
   */
  virtual IAudioDeviceCollection* enumerateRecordingDevices() = 0;

  /**
   * Specifies an audio recording device with the device ID.
   *
   * @param deviceId ID of the audio recording device. It can be retrieved by
   * the enumerateRecordingDevices() method. Plugging or unplugging the audio device
   * does not change the device ID.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setRecordingDevice(const char deviceId[MAX_DEVICE_ID_LENGTH]) = 0;

  /**
   * Gets the audio recording device by the device ID and device name.
   *
   * @param deviceId ID of the audio recording device.
   * @param deviceName The name of the audio recording device.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int getRecordingDeviceInUse(char deviceId[MAX_DEVICE_ID_LENGTH],
                                      char deviceName[MAX_DEVICE_ID_LENGTH]) = 0;

  /**
   * Sets the volume of the microphone.
   *
   * @param volume The volume of the microphone, ranging from 0 to 255.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setRecordingDeviceVolume(unsigned int volume) = 0;

  /**
   * Gets the volume of the microphone.
   *
   * @param volume The volume of the microphone, ranging from 0 to 255.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int getRecordingDeviceVolume(unsigned int* volume) = 0;

  /**
   * Releases all IAudioDeviceManager resources.
   */
  virtual void release() = 0;

 protected:
  virtual ~IAudioDeviceManager() {}
};

/**
 * The IVideoDeviceCollection class.
 */
class IVideoDeviceCollection {
 public:

  /**
   * Gets the total number of the indexed video capture devices in the system.
   *
   * @return The total number of the indexed video capture devices.
   */
  virtual int getCount() = 0;

  /**
   * Gets the specified information of an indexed video capture device.
   *
   * @param index An input parameter that specifies the device. It is a specified
   * index and must be smaller than the return value of getCount().
   * @param deviceName An output parameter that indicates the device name.
   * @param deviceId An output parameter that indicates the device ID.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int getDeviceInfo(int index, char deviceName[MAX_DEVICE_ID_LENGTH],
                            char deviceId[MAX_DEVICE_ID_LENGTH]) = 0;

  /**
   * Releases all IVideoDeviceCollection resources.
   */
  virtual void release() = 0;

 protected:
  virtual ~IVideoDeviceCollection() {};
};

/**
 * The IVideoDeviceManager class.
 */
class IVideoDeviceManager {
 public:
  /**
   * Enumerates the video capture devices.
   *
   * This method returns an IVideoDeviceCollection object that includes all the
   * video capture devices in the system. With the IVideoDeviceCollection object,
   * the application can enumerate the video capture devices. The application must
   * call the IVideoDeviceCollection::release() method to release the returned
   * object after using it.
   *
   * @return
   * - Returns an IVideoDeviceCollection object that includes all the
   * video capture devices in the system when the method call succeeds.
   * - Returns NULL when the method call fails.
   */
  virtual IVideoDeviceCollection* enumerateVideoDevices() = 0;

  /**
   * Specifies a device with the device ID.
   *
   * @param deviceId ID of the video capture device. You can call
   * enumerateVideoDevices() to retrieve it. Plugging or unplugging the device does
   * not change the device ID.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setDevice(const char deviceId[MAX_DEVICE_ID_LENGTH]) = 0;

  /**
   * Gets the video capture device that is in use.
   *
   * @param deviceId An Output parameter that specifies the ID of the video
   * capture device.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int getDeviceInUse(char deviceId[MAX_DEVICE_ID_LENGTH],
                             char deviceName[MAX_DEVICE_ID_LENGTH]) = 0;

  /** Releases all IVideoDeviceManager resources.
   */
  virtual void release() = 0;

protected:
  virtual ~IVideoDeviceManager() {}
};

/**
 * The IVideoPreviewRenderer class.
 */
class IVideoPreviewRenderer {
 public:
  /**
   * Set the local video view.
   *
   * This method sets the video display window for the local video stream.
   * To unbind the local video from the view, set `view` as `null`.
   *
   * @param view The video display window.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setView(view_t view) = 0;

  /**
   * Updates the display mode of the local video view.
   *
   * After setting the local video view, you can call this method to update its
   * rendering mode. If affects only the video view that the local user sees, not
   * the published local video stream.
   *
   * @param renderMode Sets the local display mode. See #RENDER_MODE_TYPE.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setRenderMode(RENDER_MODE_TYPE renderMode) = 0;

  /**
   * Sets the video mirror mode.
   *
   * @param mirrorMode Sets the video mirror mode:
   * - `VIDEO_MIRROR_MODE_AUTO(0)`: (Default) The mirror mode determined by the Kit.
   * If you use the front camera, the SDK enables the mirror mode;
   * if you use the rear camera, the SDK disables the mirror mode.
   * - `VIDEO_MIRROR_MODE_ENABLED(1)`: Enable the mirror mode.
   * - `VIDEO_MIRROR_MODE_DISABLED(2)`: Disable the mirror mode.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setMirrorMode(VIDEO_MIRROR_MODE_TYPE mirrorMode) = 0;

  /**
   * Releases all IVideoPreviewRenderer resources.
   */
  virtual void release() = 0;

 protected:
  virtual ~IVideoPreviewRenderer() {};
};

class ISnapshotCallback {
 public:
  virtual void onSnapshot(const uint8_t* imageBuffer, int bufferLength, int imageWidth, int imageHeight) = 0;
  virtual ~ISnapshotCallback() {}
};

}  // namespace streaming
}  // namespace agora

/**
 * Creates an Agora streaming kit object and returns the pointer.
 * @return
 * - The pointer to \ref agora::streaming::IStreamingKit "IStreamingKit", if the method call succeeds.
 * - The empty pointer NULL, if the method call fails.
 */
AGORA_API agora::streaming::IStreamingKit* AGORA_CALL createAgoraStreamingKit();

/** Gets the SDK version number.
 *
 * @param build Build number of the Agora SDK.
 * @return The version of the current SDK in the string format.
 */
AGORA_API const char* AGORA_CALL getAgoraSdkVersion(int* build);

#endif  // AGORA_STREAMING_KIT_H_
