#include "RSK_video_source.h"
#define DATA_IPC_NAME "avsipc"
#define DATA_IPC_AUDIO_NAME "audioipc"
#define PROCESS_RUN_EVENT_NAME "agora_video_source_process_ready_event_name"
namespace agora{
    namespace streaming{
        #define MAX_VIDEO_WIDTH 2560
        #define MAX_VIDEO_HEIGHT 1600

		const unsigned int RSKVideoSourceSink::S_MAX_DATA_LEN = MAX_VIDEO_WIDTH * MAX_VIDEO_HEIGHT;

		const unsigned int RSKVideoSourceSink::S_BUF_LEN = (int)(MAX_VIDEO_WIDTH * MAX_VIDEO_HEIGHT);
		//const unsigned int RSKVideoSourceSink::s_bufLen = (const unsigned int)(2560 * 1600 * 1.5 + 1000);


        /**
         * AgoraVideoSource implementation
         * agoraVideoSource starts two thread, one for message transform, and one for video data transform.
         */
        RSKVideoSourceSink::RSKVideoSourceSink()
            : m_msgThread()
            , m_initialized(false)
            , m_event(false)
        {
           // m_backBuf.resize(RSKVideoSourceSink::s_bufLen);
        }

        RSKVideoSourceSink::~RSKVideoSourceSink()
        {
            clear();
        }

        void RSKVideoSourceSink::clear()
        {
            m_initialized = false;
            m_eventHandler = nullptr;
			
            if (m_ipcMsg.get()) {
                m_ipcMsg->sendMessage(RSK_IPC_DISCONNECT, nullptr, 0);
                m_ipcMsg->disconnect();
            }
            if(m_msgThread.joinable())
                m_msgThread.join();
        }

        node_error RSKVideoSourceSink::release()
        {
            if (m_initialized){
                clear();
            }
            return node_ok;
        }

        bool RSKVideoSourceSink::initialize(IRSKVideoSourceEventHandler *eventHandler, const char* appid)
        {
            if (m_initialized)
                return true;
            if (!appid)
                return false;
            clear();
            m_eventHandler = eventHandler;
#ifdef _WIN32
            UUID uuid = { 0 };
            RPC_CSTR struuid;

            if (UuidCreate(&uuid) != RPC_S_OK)
                return false;
            if (UuidToStringA(&uuid, &struuid) != RPC_S_OK)
                return false;
            m_peerId = (LPSTR)struuid;
            RpcStringFreeA(&struuid);
#else
            uuid_t uuid;
            uuid_generate(uuid);
            uuid_string_t uid = {0};
            uuid_unparse(uuid, uid);
            m_peerId = "/";
            m_peerId += uid;
            m_peerId = m_peerId.substr(0, 20);
#endif
			m_buf.reserve(S_BUF_LEN);
			m_audioBuf.reserve(S_BUF_LEN);
            do {
                m_ipcMsg.reset(createAgoraIpc(this));
                if (!m_ipcMsg.get()) {
                    break;
                }

                if (!m_ipcMsg->initialize(m_peerId))
                    break;

                if (!m_ipcMsg->listen())
                    break;

                m_msgThread = std::thread(&RSKVideoSourceSink::msgThread, this);
                std::string targetPath;
                if (!INodeProcess::getCurrentModuleFileName(targetPath)) {
                    break;
                }

                size_t pos = targetPath.find_last_of("\\/");
                if (pos == targetPath.npos) {
                    break;
                }

                std::stringstream ss;
                ss << INodeProcess::GetCurrentNodeProcessId();
                std::string path = targetPath.substr(0, pos + 1);
                std::string cmdname = "VideoSource";
                std::string idparam = "id:" + m_peerId;
                std::string pidparam = "pid:" + ss.str();
                std::string appidparam = "appid:" + std::string(appid);
                const char* params[] = { cmdname.c_str(), idparam.c_str(), pidparam.c_str(), appidparam.c_str(), nullptr };
                m_sourceNodeProcess.reset(INodeProcess::CreateNodeProcess(path.c_str(), params));
                if (!m_sourceNodeProcess.get()) {
                    break;
                }

                NodeEvent::WaitResult result = m_event.WaitFor(5000);
                if (result != NodeEvent::WAIT_RESULT_SET) {
                    break;
                }

                m_sourceNodeProcess->Monitor([eventHandler](INodeProcess* pProcess) {
                    eventHandler->onVideoSourceExit();
                });
                m_initialized = true;
                return true;
            } while (false);
            clear();
            return false;
        }


        void RSKVideoSourceSink::msgThread()
        {
            m_ipcMsg->run();
        }
#if 0
		node_error RSKVideoSourceSink::join(const char* token, const char* cname, const char* chan_info, uid_t uid)
        {
            if (m_initialized){
                m_peerUid = uid;
                std::unique_ptr<JoinChannelCmd> cmd(new JoinChannelCmd());
				cmd->uid = uid;
                if (token)
                    strncpy_s(cmd->token, token, MAX_TOKEN_LEN);
                if (cname) {
					strncpy_s(cmd->cname, cname, MAX_CNAME_LEN);
                }
                if (chan_info)
					strncpy_s(cmd->chan_info, chan_info, MAX_CHAN_INFO);
              
                return m_ipcMsg->sendMessage(AGORA_IPC_JOIN, (char*)cmd.get(), sizeof(JoinChannelCmd)) ? node_ok : node_generic_error;
            }
            return node_status_error;
        }
#endif

        void RSKVideoSourceSink::onMessage(unsigned int msg, char* payload, unsigned int len)
        {
            LOG_INFO("RSKVideoSourceSink Receive msg : %d\n", msg);
            if (msg == RSK_IPC_SOURCE_READY) {
                m_event.notifyAll();
            }
			if (!m_initialized) {
				LOG_INFO("not initialized\n", msg);
				return;
			}

			if (msg == RSK_IPC_START_STREAMING_SUCCESS) {
				m_eventHandler->onStartStreamingSuccess();
			}
			else if (msg == RSK_IPC_START_STREAMING_FALURE) {
				StreamingFailureCmd* cmd = (StreamingFailureCmd*)payload;
				if (!cmd ) 
					return;
				
				m_eventHandler->onStartStreamingFailure(cmd->err, cmd->msg);
			}
			else if (msg == RSK_IPC_MEDIA_STREAMING_ERROR) {
				MediaStreamingErrorCmd* cmd = (MediaStreamingErrorCmd*)payload;
				if (!cmd)
					return;
				m_eventHandler->onMediaStreamingError(cmd->err, cmd->msg);
			}
			else if (msg == RSK_IPC_STREAMING_CONNECTION_STATE_CHANGED) {
				ConnectionStateCmd* cmd = (ConnectionStateCmd*)payload;
				m_eventHandler->onStreamingConnectionStateChanged((STREAMING_CONNECTION_STATE)cmd->state);
			}
   
        }


		int RSKVideoSourceSink::startStreaming(const char* publishUrl) {
			if (m_initialized) {
				int status = 0;
				std::string id = m_peerId;
				m_ipcSenderVideo.reset(new AgoraIpcDataSender());
				m_ipcSenderAudio.reset(new AgoraIpcDataSender());

				if (!m_ipcSenderVideo->initialize(id + DATA_IPC_NAME)) {
					LOG_ERROR("%s, ipc sender video init fail.", __FUNCTION__);
					return node_generic_error;
				}

				if (!m_ipcSenderAudio->initialize(id + DATA_IPC_AUDIO_NAME)) {
					LOG_ERROR("%s, ipc sender audio init fail.", __FUNCTION__);
					return node_generic_error;
				}
				return m_ipcMsg->sendMessage(RSK_IPC_START_STREAMING, const_cast<char*>(publishUrl), strlen(publishUrl) + 1) ? node_ok : node_generic_error;
			}
			return node_status_error;
		}
		
		int RSKVideoSourceSink::stopStreaming(){
			if (m_initialized) {
				
				return m_ipcMsg->sendMessage(RSK_IPC_STOP_STREAMING, nullptr, 0) ? node_ok : node_generic_error;
			}
			return node_status_error;
		}
		
		int RSKVideoSourceSink::muteAudioStream(bool muted)	{
			if (m_initialized) {
				return m_ipcMsg->sendMessage(RSK_IPC_MUTE_AUDIO_STREAM, (char*)&muted, sizeof(muted)) ? node_ok : node_generic_error;
			}
			return node_status_error;
		}
		
		int RSKVideoSourceSink::muteVideoStream(bool muted) {
			if (m_initialized) {
				return m_ipcMsg->sendMessage(RSK_IPC_MUTE_VIDEO_SYREAM, (char*)&muted, sizeof(muted)) ? node_ok : node_generic_error;
			}
			return node_status_error;
		}

		int RSKVideoSourceSink::setAudioStreamConfiguration(const AudioStreamConfiguration& config){
			if (m_initialized) {
				AudioStreamConfigurationCmd cmd(config);
				return m_ipcMsg->sendMessage(RSK_IPC_SET_AUDIO_CONFIG, (char*)&cmd, sizeof(VideoStreamConfigurationCmd)) ? node_ok : node_generic_error;
			}
			return node_status_error;
		}

		int RSKVideoSourceSink::setVideoStreamConfiguration(const VideoStreamConfiguration& config){
			if (m_initialized) {
				VideoStreamConfigurationCmd cmd(config);
				return m_ipcMsg->sendMessage(RSK_IPC_SET_VIDEO_CONFIG, (char*)&cmd, sizeof(VideoStreamConfigurationCmd)) ? node_ok : node_generic_error;
			}
			return node_status_error;
		}
		
		int RSKVideoSourceSink::switchResolution(int width, int height){
			if (m_initialized) {
				SwitchResolutionCmd cmd;
				cmd.width  = width;
				cmd.height = height;
				return m_ipcMsg->sendMessage(RSK_IPC_SET_VIDEO_CONFIG, (char*)&cmd, sizeof(SwitchResolutionCmd)) ? node_ok : node_generic_error;
			}
			return node_status_error;
		}

		int RSKVideoSourceSink::adjustRecordingSignalVolume(int volume){
			if (m_initialized) {
				return m_ipcMsg->sendMessage(RSK_IPC_ADJUST_RECORDING_SIGNAL_VOLUME, (char*)&volume, sizeof(volume)) ? node_ok : node_generic_error;
			}
			return node_status_error;
		}

		int RSKVideoSourceSink::setMaxExternalAudioFrameParameter(AudioFrameIpcHeader param)
		{
			if (m_initialized) {
				ExternalAudioFrameCmd cmd;
				cmd.audioParameter.audioChannel = param.audioChannel;
				cmd.audioParameter.sampleRate   = param.sampleRate;
				cmd.audioParameter.samplesPerChannel = param.samplesPerChannel;
				cmd.audioParameter.audioSize = param.audioSize;
				return m_ipcMsg->sendMessage(RSK_IPC_SET_VIDEO_CONFIG, (char*)&cmd, sizeof(ExternalAudioFrameCmd)) ? node_ok : node_generic_error;
			}
			return node_status_error;
		}

		int RSKVideoSourceSink::setMaxExternalVideoFrameParameter(VideoFrameIpcHeader param)
		{
			if (m_initialized) {
				ExternalVideoFrameCmd cmd;
				cmd.width  = param.width;
				cmd.height = param.height;
				return m_ipcMsg->sendMessage( RSK_IPC_EXTERNAL_VIDEO_FRAME_PARAMTER, (char*)&cmd, sizeof(ExternalVideoFrameCmd)) ? node_ok : node_generic_error;
			}
			return node_status_error;
		}

		bool RSKVideoSourceSink::sendAudioData(char* payload, int len)
		{
			if (!payload || len == 0)
				return false;

			std::lock_guard<std::mutex> lock(m_ipcSenderAudioMutex);

			if (m_ipcSenderAudio) {
				m_ipcSenderAudio->sendData(payload, len);
				LOG_INFO("Send audio data success\n");
				return true;
			}
			else {
				LOG_WARNING("IPC Audio Sender not initialized before send data.");
				return false;
			}
		}

		bool RSKVideoSourceSink::sendData(char* payload, int len)
		{
			if (!payload || len == 0)
				return false;

			std::lock_guard<std::mutex> lock(m_ipcSenderVideoMutex);

			if (m_ipcSenderVideo) {
				m_ipcSenderVideo->sendData(payload, len);
				LOG_INFO("Send data success\n");
				return true;
			}
			else {
				LOG_WARNING("IPC Sender not initialized before send data.");
				return false;
			}
		}

		void RSKVideoSourceSink::pushExternalVideoFrame(uint8_t* buffer, int width, int height)
		{
			if (!buffer || width == 0 || height == 0)
				return;

			char* pBuf = m_buf.data();
			int size = width * height * 3 / 2;
			VideoFrameIpcHeader* videoHeader = (VideoFrameIpcHeader*)pBuf;
			videoHeader->width  = width;
			videoHeader->height = height;
			videoHeader->len = size;
			char* video_buffer = pBuf + sizeof(VideoFrameIpcHeader);
			memcpy(video_buffer, buffer, size);

			sendData(pBuf, size + sizeof(VideoFrameIpcHeader));
		}

		void RSKVideoSourceSink::pushExternalAudioFrame(uint8_t* buffer, AudioFrameIpcHeader audioHeader)
		{
			char* pBuf = m_audioBuf.data();
			AudioFrameIpcHeader* pAudioHeader = (AudioFrameIpcHeader*)pBuf;
			pAudioHeader->audioChannel = audioHeader.audioChannel;
			pAudioHeader->sampleRate = audioHeader.sampleRate;
			pAudioHeader->samplesPerChannel = audioHeader.samplesPerChannel;
			pAudioHeader->audioSize = audioHeader.audioSize;

			char* audio_buffer = pBuf + sizeof(AudioFrameIpcHeader);
			memcpy(audio_buffer, buffer, audioHeader.audioSize);
			sendAudioData(pBuf, audioHeader.audioSize + sizeof(AudioFrameIpcHeader));
		}
#if 0

        void RSKVideoSourceSink::deliverFrame(const char* payload , int len)
        {
            if (len > (int)m_backBuf.size())
                return;
            char* pBack = m_backBuf.data();
            memcpy(pBack, payload, len);
            //auto *p = getNodeVideoFrameTransporter();
            //p->deliverVideoSourceFrame(pBack, len);
        }
#endif
    }
}
