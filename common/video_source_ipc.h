/*
* Copyright (c) 2018 Agora.io
* All rights reserved.
* Proprietary and Confidential -- Agora.io
*/

/*
*  Created by Wang Yongli, 2018
*/

/**
 * the file define classes used to deliver command and event between node ADDON and video source process.
 */

#ifndef AGORA_VIDEO_SOURCE_IPC_H
#define AGORA_VIDEO_SOURCE_IPC_H
#include <string>
#include <vector>
#include "ipc_shm.h"
#include <functional>
#include <thread>
#include <memory>
//#include "IAgoraStreamingKit.h"

#define MAX_PARAMETER_LEN 512
#define MAX_MSG_LEN 512
//using namespace agora::streaming;
/**
 * RSKIpcMsg define the message type transferred between node ADDON and vidoe source process
 */
enum RSKIpcMsg
{
    /**  obsolete*/
    RSK_IPC_CONNECT = 1,
    /** obsolete  */
    RSK_IPC_CONNECT_CONFIRM,
    /** obsolete  */
    RSK_IPC_DISCONNECT,
	/** external audio frame parameter*/
	RSK_IPC_EXTERNAL_AUDIO_FRAME_PARAMTER,
	/** external video frame parameter*/
	RSK_IPC_EXTERNAL_VIDEO_FRAME_PARAMTER,

    /** To notify RSK video source to start streaming*/
    RSK_IPC_START_STREAMING,
	/** To notify RSK video source to stop streaming*/
	RSK_IPC_STOP_STREAMING,
    /** To notify Video Sink that Video Source is ready*/
    RSK_IPC_SOURCE_READY, //6
    /** RSK Video Source ==> start streaming success event */
    RSK_IPC_START_STREAMING_SUCCESS,
	/** RSK Video Source ==> start streaming failure event */
	RSK_IPC_START_STREAMING_FALURE, // 8
	
	/** RSK Video Source ==> media streaming Error event */
	RSK_IPC_MEDIA_STREAMING_ERROR,
	/** RSK Video Source ==> streaming connection state changed event */
	RSK_IPC_STREAMING_CONNECTION_STATE_CHANGED, // 10

	/** To notify RSK video source to enable Audio Recording*/
	RSK_IPC_ENABLE_AUDIO_RECORDING,
	/** To notify RSK video source to enable video capturing*/
	RSK_IPC_ENABLE_VIDEO_CAPTURING,
	/** To notify RSK video source to mute audio stream*/
	RSK_IPC_MUTE_AUDIO_STREAM,
	/** To notify RSK RSK video source to mute video stream*/
	RSK_IPC_MUTE_VIDEO_SYREAM,
	
	/** To notify RSK video source to set audio stream configuration*/
	RSK_IPC_SET_AUDIO_CONFIG,
	/** To notify RSK video source to set video stream configuration*/
	RSK_IPC_SET_VIDEO_CONFIG,
	/** To notify RSK video source to switch resolution*/
	RSK_IPC_SWITCH_RESOLUTION,
	/** To notify RSK video source to adjust recording signal volume*/
	RSK_IPC_ADJUST_RECORDING_SIGNAL_VOLUME,
	
    /** video source ==> Node ADDON, local video preview complete.*/
    RSK_IPC_START_VS_PREVIEW_COMPLETE,
	AGORA_UPC_SETUP_LOCAL_RENDER_MODE
};

enum VIDEO_SOURCE_START_STREAMING_ERROR {
	// No error occurs.
	VIDEO_SOURCE_START_STREAMING_ERR_OK = 0,
	// A general error occurs (no specified reason).
	VIDEO_SOURCE_START_STREAMING_ERR_FAILED = 1,
	// Streaming kit is not initialized.
	VIDEO_SOURCE_START_STREAMING_ERR_NOT_INITIALIZED = 2,
	// Streaming kit is already started.
	VIDEO_SOURCE_START_STREAMING_ERR_ALREADY_STARTED = 3,
	// Failed to open audio device.
	VIDEO_SOURCE_START_STREAMING_ERR_OPEN_AUDIO_DEVICE = 4,
	// Failed to open video device.
	VIDEO_SOURCE_START_STREAMING_ERR_OPEN_VIDEO_DEVICE = 5,
	// Failed to connect RTMP.
	VIDEO_SOURCE_START_STREAMING_ERR_CONNECT_RTMP = 6,
	// Invalid argument used.
	VIDEO_SOURCE_START_STREAMING_ERR_INVALID_ARGUMENT = 7,
};

// The error codes for media streaming
// GENERATED_JAVA_ENUM_PACKAGE: io.agora.streaming
enum VIDEO_SOURCE_MEDIA_STREAMING_ERROR {
	// No error occurs.
	VIDEO_SOURCE_MEDIA_STREAMING_ERR_OK = 0,
	// A general error occurs (no specified reason).
	VIDEO_SOURCE_MEDIA_STREAMING_ERR_FAILED = 1,
	// Audio publication error.
	VIDEO_SOURCE_MEDIA_STREAMING_ERR_AUDIO_PUBLICATION = 2,
	// Video publication error.
	VIDEO_SOURCE_MEDIA_STREAMING_ERR_VIDEO_PUBLICATION = 3,
};

// The connection state of media streaming
// GENERATED_JAVA_ENUM_PACKAGE: io.agora.streaming
enum VIDEO_SOURCE_STREAMING_CONNECTION_STATE {
	// The SDK is disconnected from streaming server.
	VIDEO_SOURCE_STREAMING_CONNECTION_STATE_DISCONNECTED = 0,
	// The SDK is connected to streaming server.
	VIDEO_SOURCE_STREAMING_CONNECTION_STATE_CONNECTED = 1,
	// The SDK is trying to reconnect to streaming server.
	VIDEO_SOURCE_STREAMING_CONNECTION_STATE_RECONNECTING = 2,
	// The SDK fails to connect to streaming server.
	VIDEO_SOURCE_STREAMING_CONNECTION_STATE_FAILED = 3,
};


// only I420
struct VideoFrameIpcHeader
{
	int width = 1920;
	int height = 1080;
	uint32_t len;
	int64_t renderTimeMs;
};

struct AudioFrameIpcHeader
{
	int audioChannel;
	int sampleRate;
	int samplesPerChannel;
	uint32_t audioSize;
	int64_t renderTimeMs;
};

struct ExternalAudioFrameCmd
{
	/*int audioChannel;
	int sampleRate;
	int samplesPerChannel;
	uint32_t audioSize;*/
	AudioFrameIpcHeader audioParameter;
public:
	ExternalAudioFrameCmd()
		: audioParameter{ 2, 48000, 480, 2 * 2 * 480 }
		/*: audioChannel(2)
		, sampleRate(48000)
		, samplesPerChannel(480)
		, audioSize(2 * 2 * 480)*/
	{

	}
};

struct ExternalVideoFrameCmd {
	int width;
	int height;
public:
	ExternalVideoFrameCmd()
		: width(1920)
		, height(1080) {

	}
};

struct MuteAudioCmd {
	bool mute;
public:
	MuteAudioCmd()
		: mute(false) {

	}
};

struct MuteVideoCmd {
	bool mute;
public:
	MuteVideoCmd()
		: mute(false) {

	}
};

struct VideoConfig {
	int width;
	int height;
	int fps;
	int bitrate;
public:
	VideoConfig()
		: width(640)
		, height(360)
		, fps(15)
		, bitrate(500) 
	{

	}

	VideoConfig(int w, int h, int f, int b)
		: width(w)
		, height(h)
		, fps(f)
		, bitrate(b)
	{}
};

struct AudioStreamConfigurationCmd {
	int sampleRateHz;
	int bytesPerSample;
	int numberOfChannels;
	int bitrate;
public:
	AudioStreamConfigurationCmd() : sampleRateHz(48000), bytesPerSample(2),
		numberOfChannels(1), bitrate(48) {}
	/*AudioStreamConfigurationCmd(agora::streaming::AudioStreamConfiguration config)
		: sampleRateHz(config.sampleRateHz), bytesPerSample(config.bytesPerSample)
		, numberOfChannels(config.numberOfChannels), bitrate(config.bitrate) 
	{}*/
};

struct VideoStreamConfigurationCmd {
	int width;
	int height;
	int framerate;
	int bitrate;
	int maxBitrate;
	int minBitrate;
	int orientationMode;
	int mirrorMode;
	int videoEncodingMode;
public:
	VideoStreamConfigurationCmd() : width(640), height(360), framerate(15),
		bitrate(800), maxBitrate(960), minBitrate(600),
		orientationMode(1),//ORIENTATION_MODE_FIXED_LANDSCAPE
		mirrorMode(2), //VIDEO_MIRROR_MODE_DISABLED
		videoEncodingMode(1)//VIDEO_ENCODING_MODE_HARDWARE
	{}

	/*VideoStreamConfigurationCmd(agora::streaming::VideoStreamConfiguration config)
		: width(config.width)
		, height(config.height)
		, framerate(config.framerate)
		, bitrate(config.bitrate)
		, minBitrate(config.minBitrate)
		, maxBitrate(config.maxBitrate)
		, orientationMode(config.orientationMode)
		, mirrorMode(config.mirrorMode)
		, videoEncodingMode(config.videoEncodingMode)
		
	{}*/
};


struct SwitchResolutionCmd {
	int width;
	int height;
public:
	SwitchResolutionCmd()
		: width(640) 
		, height(360)
	{

	}
};

struct RecordingSignalVolumeCmd {
	int volume;
public:
	RecordingSignalVolumeCmd()
		: volume(100) 
	{

	}
};

struct SetParameterCmd
{
    char parameters[MAX_PARAMETER_LEN];
public:
    SetParameterCmd()
    {}
};

struct StreamingFailureCmd {
	char msg[MAX_MSG_LEN];
	VIDEO_SOURCE_START_STREAMING_ERROR err;
public:
	StreamingFailureCmd()
		: err(VIDEO_SOURCE_START_STREAMING_ERR_OK)
	{
		memset(msg, 0, MAX_MSG_LEN);
	}
};

struct MediaStreamingErrorCmd {
	char msg[MAX_MSG_LEN];
	VIDEO_SOURCE_MEDIA_STREAMING_ERROR err;
public:
	MediaStreamingErrorCmd()
		: err(VIDEO_SOURCE_MEDIA_STREAMING_ERR_OK)
	{
		memset(msg, 0, MAX_MSG_LEN);
	}
};

struct ConnectionStateCmd {
	VIDEO_SOURCE_STREAMING_CONNECTION_STATE state;
public:
	ConnectionStateCmd()
		: state(VIDEO_SOURCE_STREAMING_CONNECTION_STATE_DISCONNECTED)
	{
		
	}
};

/**
 * AgoraIpcListener is used to monitor IPC message
 */
class AgoraIpcListener
{
public:
    virtual ~AgoraIpcListener(){}
    virtual void onMessage(unsigned int msg, char* payload, unsigned int len)
    {
        (void)payload;
        (void)len;
    }
};

/**
 * IAgoraIpc is used to facilitate communications between processes. This is one virtual class, may have different implementation on different platforms.
 */
class IAgoraIpc
{
public:
    IAgoraIpc(AgoraIpcListener* listener)
        : m_listener(listener)
    {}
    virtual ~IAgoraIpc(){}
    virtual const std::string& getId(){ return m_id; }
    /**
     * To initialize IPC.
     * @param id : the id used to identify the IPC endpoint.
     */
    virtual bool initialize(const std::string& id) = 0;
    /**
     * To put IPC endpoint in listen state, then other endpoint can connect the endpoint.
     */
    virtual bool listen() = 0;
    /**
     * To connect to other IPC endpoint.
     */
    virtual bool connect() = 0;
    /**
     * To disconnect the IPC
     */
    virtual bool disconnect() = 0;
    /**
     * To start IPC.
     */
    virtual void run() = 0;
    /**
     * To send message.
     * @param msg : the message id.
     * @param payload : the pointer to the transferred data.
     * @param len : the length of the data payload.
     */
    virtual bool sendMessage(RSKIpcMsg msg, char* payload, unsigned int len) = 0;
protected:
    std::string m_id;
    AgoraIpcListener *m_listener;
};

/**
 * The class is used for IPC with large throughput. 
 * AgoraIpcDataSender provide send-only facilities.
 */
#define DATA_DELIVER_BLOCK_SIZE 6145000
//3110450
class AgoraIpcDataSender
{
public:
    AgoraIpcDataSender();
    ~AgoraIpcDataSender();

    /**
     * To initialize the AgoraIpcDataSender.
     * @param id : sender identifier.
     */
    bool initialize(const std::string& id);
    /**
     * To send data.
     * @param payload : the pointer to data to be sent.
     * @param len : the length of the data.
     */
    void sendData(char* payload, unsigned int len);
    /**
     * To send multiple data in one time.
     * @payloads : vector of data to be sent.
     */
    void sendMultiData(const std::vector<std::pair<char*, int32_t>>& payloads);

    /** Disconnect the sender and IPC */
    void Disconnect();
private:
    shm_ipc<1, DATA_DELIVER_BLOCK_SIZE> m_ipcData;
    bool m_initialized;
    std::string m_id;
};

/**
 * The class is used to IPC with large throughput.
 * The AgoraIpcDataReceiver provide receive-only facilities.
 */
class AgoraIpcDataReceiver
{
public:
    AgoraIpcDataReceiver();
    ~AgoraIpcDataReceiver();

    /**
     * To initialize AgoraIpcDataReciever
     * @param id : IPC identifier
     * @param handler : the receiver event handler.
     */
    bool initialize(const std::string& id, const std::function<void(const char*, int)>& handler);
    /**
     * To start the IPC.
     */
    void run(bool async = false);

    /**
    * To stop the IPC
    */
    void stop();
private:
    std::function<void(const char*, int)> m_handler;
    std::string m_id;
    bool m_initialized;
    shm_ipc<1, DATA_DELIVER_BLOCK_SIZE> m_ipcData;
    std::unique_ptr<std::thread> m_thread;
};

/**
 * To create IAgroaIpc instance.
 */
IAgoraIpc* createAgoraIpc(AgoraIpcListener *listner);

#endif
