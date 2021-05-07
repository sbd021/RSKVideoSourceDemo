/*
* Copyright (c) 2018 Agora.io
* All rights reserved.
* Proprietary and Confidential -- Agora.io
*/

/*
*  Created by Wang Yongli, 2018
*/

#ifndef AGORA_VIDEO_SOURCE_H
#define AGORA_VIDEO_SOURCE_H

#include "IAgoraStreamingKit.h"
#include <memory>
#include "video_source_param_parser.h"
#include "video_source_ipc.h"
#include "node_process.h"
#include "node_error.h"
#include <thread>
#include <mutex>

class RSKVideoSourceEventHandler;
class RSKVideoSourceRenderFactory;

using namespace agora::streaming;
/**
 * RSKVideoSource is used to wrap RTC engine and provide IPC functionality.
 */
class RSKVideoSource : public AgoraIpcListener
{
    /**
     * RSKVideoSourceEventHandle need to access IPC related functions.
     */
    friend class RSKVideoSourceEventHandler;
public:
    /**
     * To construct RSKVideoSource
     * @param : the parameters to construct RSKVideoSource. It's like 'id:***** pid:****'.Currently id and pid parameters is needed.
     */
    RSKVideoSource(const std::string& param);
    ~RSKVideoSource();

    /**
     * To run video source, including following two steps:
     * 1) To start monitor thread, when the sink process exit, video source stopped.
     * 2) To run IPC to monitor cmds from sink
     */
    void run();

    /**
     * Initialize the video source. Must be called before run.
     */
    bool initialize();

    /**
     * After release, the video source object could not be accessed any more.
     */
    void release();

    /**
     * Each video source has one specific Id.
     */
    std::string getId();

    /**
     * To process IPC messages.
     */
    virtual void onMessage(unsigned int msg, char* payload, unsigned int len) override;

   
protected:
	void onStartStreamingSuccess();
	void onStartStreamingFailure(START_STREAMING_ERROR err, const char* msg);
	void onMediaStreamingError(MEDIA_STREAMING_ERROR err, const char* msg);
	void onStreamingConnectionStateChanged(STREAMING_CONNECTION_STATE state);
private:
    void exit(bool notifySink);
	void deliverFrame(const char* payload, int len);
	void deliverAudioFrame(const char* payload, int len);
private:
    /** Used to process RTC events. */
    std::unique_ptr<RSKVideoSourceEventHandler> m_eventHandler;
  
    /** Used to parse parameters from sink */
    std::unique_ptr<VideoSourceParamParser> m_paramParser;
    /** Used to provide IPC functionality. */
    std::unique_ptr<IAgoraIpc> m_ipc;
	std::unique_ptr<AgoraIpcDataReceiver> m_ipcReceiverVideo;
	std::unique_ptr<AgoraIpcDataReceiver> m_ipcReceiverAudio;
	std::unique_ptr< IStreamingKit> m_rskEngine;
    bool m_initialized;
    std::string m_params;
    std::unique_ptr<INodeProcess> m_process;
	int m_videoProfile;
	agora::media::IVideoFrameObserver::VideoFrame videoFrame;
	agora::media::IAudioFrameObserver::AudioFrame audioFrame;

};

#endif