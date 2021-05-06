/*
* Copyright (c) 2018 Agora.io
* All rights reserved.
* Proprietry and Confidential -- Agora.io
*/

/*
*  Created by Wang Yongli, 2018
*/

#include "video_source_event_handler.h"
#include "node_log.h"

RSKVideoSourceEventHandler::RSKVideoSourceEventHandler(RSKVideoSource& videoSource)
    : m_videoSource(videoSource)
{

}

RSKVideoSourceEventHandler::~RSKVideoSourceEventHandler()
{}

void RSKVideoSourceEventHandler::onStartStreamingSuccess()
{
	LOG_INFO("onStartStreamingSuccess");
	m_videoSource.onStartStreamingSuccess();
}

void RSKVideoSourceEventHandler::onStartStreamingFailure(START_STREAMING_ERROR err, const char* msg)
{
	LOG_INFO("onStartStreamingFailure, err=%d, msg:%s", err, msg);
	m_videoSource.onStartStreamingFailure(err, msg);
}

void  RSKVideoSourceEventHandler::onMediaStreamingError(MEDIA_STREAMING_ERROR err, const char* msg)
{
	LOG_INFO("onMediaStreamingError, err=%d, msg:%s", err, msg);
	m_videoSource.onMediaStreamingError(err, msg);
}

void RSKVideoSourceEventHandler::onStreamingConnectionStateChanged(STREAMING_CONNECTION_STATE state)
{
	LOG_INFO("onStreamingConnectionStateChanged, state=%d", state);
	m_videoSource.onStreamingConnectionStateChanged(state);
}