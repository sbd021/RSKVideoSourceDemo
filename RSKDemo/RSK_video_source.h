/*
* Copyright (c) 2018 Agora.io
* All rights reserved.
* Proprietary and Confidential -- Agora.io
*/

/*
*  Created by Wang Yongli, 2018
*/

#ifndef AGORA_VIDEO_SOURCE_SINK_H
#define AGORA_VIDEO_SOURCE_SINK_H

#include <string>
#include <thread>
#include <functional>
#include <string>
#include <sstream>
#include <fstream>
#include <memory>
#include <vector>
#ifdef _WIN32
#include <Rpc.h>
#include <RpcDce.h>
#else
#include <uuid/uuid.h>
#endif

#include "video_source_ipc.h"
//#include "IAgoraStreamingKit.h"
#include "node_error.h"
#include "node_log.h"
#include "node_process.h"
#include "node_event.h"

namespace agora {
	namespace streaming {
		class IRSKVideoSourceEventHandler {
		public:
			virtual ~IRSKVideoSourceEventHandler(){}

			virtual void onStartStreamingSuccess() = 0;
			virtual void onStartStreamingFailure(VIDEO_SOURCE_START_STREAMING_ERROR err, const char* msg) = 0;
			virtual void onMediaStreamingError(VIDEO_SOURCE_MEDIA_STREAMING_ERROR err, const char* msg) = 0;
			virtual void onStreamingConnectionStateChanged(VIDEO_SOURCE_STREAMING_CONNECTION_STATE state) = 0;
			virtual void onVideoSourceExit() = 0;
		};

		class RSKVideoSourceSink : public AgoraIpcListener
		{
		public:
			RSKVideoSourceSink();
			~RSKVideoSourceSink();
			virtual bool initialize(IRSKVideoSourceEventHandler *eventHandler, const char* appid, VideoConfig config);
			virtual node_error release();
			virtual void onMessage(unsigned int msg, char* payload, unsigned int len)override;

			int startStreaming(const char* publishUrl);
			 int stopStreaming();
			int muteAudioStream(bool muted);
			int muteVideoStream(bool muted);
			int setAudioStreamConfiguration(const AudioStreamConfigurationCmd& config);
			int setVideoStreamConfiguration(const VideoStreamConfigurationCmd& config);
			int switchResolution(int width, int height);
			int adjustRecordingSignalVolume(int volume);

			int setMaxExternalAudioFrameParameter(AudioFrameIpcHeader);
			int setMaxExternalVideoFrameParameter(VideoFrameIpcHeader);

			void pushExternalVideoFrame(uint8_t* buffer,VideoFrameIpcHeader videoHeader);
			void pushExternalAudioFrame(uint8_t* buffer , AudioFrameIpcHeader audioHeader);
		private:
			void msgThread();
			//void deliverFrame(const char* payload, int len);
			void clear();
			/**
			 * To send data via IPC
			 */
			bool sendData(char* payload, int len);
			bool sendAudioData(char* payload, int len);
			//	void onStartPreviewComplete();

		private:
			std::thread m_msgThread;
			std::mutex m_lock;
			std::unique_ptr<IAgoraIpc> m_ipcMsg;
			/** Used to transfer video data */
			std::unique_ptr<AgoraIpcDataSender> m_ipcSenderVideo;
			std::unique_ptr<AgoraIpcDataSender> m_ipcSenderAudio;
			std::mutex m_ipcSenderVideoMutex;
			std::mutex m_ipcSenderAudioMutex;

			std::unique_ptr<INodeProcess> m_sourceNodeProcess;
			IRSKVideoSourceEventHandler *m_eventHandler;
			std::string m_peerId;
			bool m_peerJoined;
			bool m_initialized;
			unsigned int m_peerUid;
			NodeEvent m_event;
			//buffer_list m_buffers;
			std::vector<char> m_backBuf;
		private:
			//static const unsigned int s_bufLen;

			std::vector<char> m_buf;
			std::vector<char> m_audioBuf;
			static const unsigned int S_MAX_DATA_LEN;
			static const unsigned int S_BUF_LEN;
		};
	
	}
}

#endif
