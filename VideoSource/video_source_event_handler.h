/*
* Copyright (c) 2018 Agora.io
* All rights reserved.
* Proprietry and Confidential -- Agora.io
*/

/*
*  Created by Wang Yongli, 2018
*/

#ifndef AGORA_VIDEO_SOURCE_EVNET_HANDLER_H
#define AGORA_VIDEO_SOURCE_EVNET_HANDLER_H

#include "IAgoraStreamingKit.h"
#include "video_source.h"

using namespace agora::streaming;

/**
 * RSKVideoSourceEventHandler provide implementation of IStreamingEventHandler.
 * RSKVideoSourceEventHandler rely on video source to transfer event to sink.
 */
class RSKVideoSourceEventHandler : public IStreamingEventHandler
{
public:
    RSKVideoSourceEventHandler(RSKVideoSource& videoSource);
    ~RSKVideoSourceEventHandler();
	virtual void onStartStreamingSuccess() override;

	virtual void onStartStreamingFailure(START_STREAMING_ERROR err, const char* msg)override;

	virtual void onMediaStreamingError(MEDIA_STREAMING_ERROR err, const char* msg) override;

	virtual void onStreamingConnectionStateChanged(STREAMING_CONNECTION_STATE state)override;

private:
	RSKVideoSource& m_videoSource;
};

#endif