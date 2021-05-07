/*
* Copyright (c) 2018 Agora.io
* All rights reserved.
* Proprietary and Confidential -- Agora.io
*/

/*
*  Created by Wang Yongli, 2018
*/

#include "video_source.h"
#include "video_source_event_handler.h"
//#include "video_source_render.h"
#include "video_source_param_parser.h"
#include "video_source_ipc.h"
#include "node_log.h"

#define PROCESS_RUN_EVENT_NAME "agora_video_source_process_ready_event_name"
#define DATA_IPC_NAME "avsipc"
#define DATA_IPC_AUDIO_NAME "audioipc"
//using agora::rtc::RtcEngineContext;
//using agora::rtc::uid_t;

#ifdef TEST
int audio_file_size = 0;
FILE* fpPcm = NULL;
int read_audio_size = 0;
#endif
RSKVideoSource::RSKVideoSource(const std::string& param)
	: m_initialized(false)
	, m_params(param)
	, m_videoProfile(0)
{
    LOG_ENTER;
	videoFrame.yBuffer = new uint8_t[1920 * 1080];
	videoFrame.uBuffer = new uint8_t[1920 * 1080/4];
	videoFrame.vBuffer = new uint8_t[1920 * 1080/4];


	ExternalAudioFrameCmd cmd;
	audioFrame.buffer = new uint8_t[cmd.audioParameter.audioSize];
	audioFrame.type = agora::media::IAudioFrameObserver::FRAME_TYPE_PCM16;
	audioFrame.avsync_type = 0;
	audioFrame.bytesPerSample = cmd.audioParameter.audioChannel * sizeof(short);

#ifdef TEST
	fopen_s(&fpPcm, "D:\\test2.pcm", "rb");
	fseek(fpPcm, 0, SEEK_END);
	audio_file_size = ftell(fpPcm);
	fseek(fpPcm, 0, SEEK_SET);
#endif
    LOG_LEAVE;
}

RSKVideoSource::~RSKVideoSource()
{
    LOG_ENTER;
   // m_rtcEngine.reset();
    m_eventHandler.reset();
   // m_renderFactory.reset();
    m_ipc.reset();
    m_paramParser.reset();
    LOG_LEAVE;
}

std::string RSKVideoSource::getId()
{
    return m_paramParser->getParameter("id");
}

bool RSKVideoSource::initialize()
{
    LOG_ENTER;
    m_paramParser.reset(new VideoSourceParamParser());
    m_paramParser->initialize(m_params);

    std::string appid = m_paramParser->getParameter("appid");
	std::string width = m_paramParser->getParameter("width");
	std::string height = m_paramParser->getParameter("height");

	std::string fps = m_paramParser->getParameter("fps");
	std::string bitrate = m_paramParser->getParameter("bitrate");
	//std::string sampleRate = m_paramParser->getParameter("sampleRate");
	//std::string channels = m_paramParser->getParameter("channels");
	//std::string samples = m_paramParser->getParameter("samples");

    if (appid.empty()) {
        LOG_ERROR("%s, appid is null\n", __FUNCTION__);
        LOG_LEAVE;
        return false;
    }

    std::string id = m_paramParser->getParameter("id");
    if (id.empty()) {
        LOG_ERROR("%s, id is null\n", __FUNCTION__);
        LOG_LEAVE;
        return false;
    }

    m_ipc.reset(createAgoraIpc(this));
    if (!m_ipc->initialize(id)){
        LOG_ERROR("%s, ipc init fail\n", __FUNCTION__);
        LOG_LEAVE;
        return false;
    }
    if (!m_ipc->connect()){
        LOG_ERROR("%s, ipc connect fail.\n", __FUNCTION__);
        LOG_LEAVE;
        return false;
    }

	m_rskEngine.reset(createAgoraStreamingKit());
	if (!m_rskEngine.get()) {
		LOG_ERROR("%s, rsk engine create fail.\n", __FUNCTION__);
		LOG_LEAVE;
		return false;
	}

	m_eventHandler.reset(new RSKVideoSourceEventHandler(*this));
	StreamingContext context;
	context.eventHandler = m_eventHandler.get();
	context.appId = appid.c_str();

	context.audioStreamConfiguration.sampleRateHz = 48000;
	context.audioStreamConfiguration.numberOfChannels = 2;
	context.videoStreamConfiguration.width = strtoul(width.c_str(), nullptr, 10);
	context.videoStreamConfiguration.height = strtoul(height.c_str(), nullptr, 10);;
	context.videoStreamConfiguration.framerate = strtoul(fps.c_str(), nullptr, 10);;
	context.videoStreamConfiguration.bitrate = strtoul(bitrate.c_str(), nullptr, 10);;
	context.videoStreamConfiguration.mirrorMode = VIDEO_MIRROR_MODE_DISABLED;
	context.videoStreamConfiguration.orientationMode = ORIENTATION_MODE_FIXED_LANDSCAPE;
	if (m_rskEngine->initialize(context) != 0) {
		LOG_ERROR("%s, RSKVideoSource initialize failed.\n", __FUNCTION__);
		LOG_LEAVE;
		return false;
	}

	m_rskEngine->setExternalVideoSource(true);
	m_rskEngine->setExternalAudioSource(true);
	LOG_INFO("%s, set external audio and video source\n", __FUNCTION__);
	
	m_ipcReceiverVideo.reset();
	m_ipcReceiverAudio.reset();
	m_ipc->sendMessage(RSK_IPC_SOURCE_READY, nullptr, 0);
	m_initialized = true;
    LOG_LEAVE;
    return true;
}

void RSKVideoSource::onStartStreamingSuccess()
{
	if (!m_initialized)
		return;
	m_ipcReceiverVideo.reset(new AgoraIpcDataReceiver());
	m_ipcReceiverAudio.reset(new AgoraIpcDataReceiver());

	std::string id = m_paramParser->getParameter("id");
	
	if (!m_ipcReceiverVideo->initialize(id + DATA_IPC_NAME, std::bind(&RSKVideoSource::deliverFrame, this, std::placeholders::_1, std::placeholders::_2))) {
		m_ipcReceiverVideo.reset();
		return;
	}

	if (!m_ipcReceiverAudio->initialize(id + DATA_IPC_AUDIO_NAME, std::bind(&RSKVideoSource::deliverAudioFrame, this, std::placeholders::_1, std::placeholders::_2))) {
		m_ipcReceiverAudio.reset();
		return;
	}
	
	m_ipc->sendMessage(RSK_IPC_START_STREAMING_SUCCESS, nullptr, 0);
	m_ipcReceiverVideo->run(true);
	m_ipcReceiverAudio->run(true);
}

void RSKVideoSource::onStartStreamingFailure(START_STREAMING_ERROR err, const char* msg)
{
	std::unique_ptr<StreamingFailureCmd> cmd(new StreamingFailureCmd);
	cmd->err = (VIDEO_SOURCE_START_STREAMING_ERROR)err;
	strcpy_s(cmd->msg, MAX_MSG_LEN, msg);
	m_ipc->sendMessage(RSK_IPC_START_STREAMING_FALURE, (char*)&cmd, sizeof(StreamingFailureCmd));
}

void RSKVideoSource::onMediaStreamingError(MEDIA_STREAMING_ERROR err, const char* msg)
{
	std::unique_ptr<MediaStreamingErrorCmd> cmd(new MediaStreamingErrorCmd);
	cmd->err  = (VIDEO_SOURCE_MEDIA_STREAMING_ERROR)err;
	strcpy_s(cmd->msg, MAX_MSG_LEN, msg);
	m_ipc->sendMessage(RSK_IPC_MEDIA_STREAMING_ERROR, (char*)&cmd, sizeof(MediaStreamingErrorCmd));
}

void RSKVideoSource::onStreamingConnectionStateChanged(STREAMING_CONNECTION_STATE state)
{
	std::unique_ptr<ConnectionStateCmd> cmd(new ConnectionStateCmd);
	cmd->state = (VIDEO_SOURCE_STREAMING_CONNECTION_STATE)state;
	m_ipc->sendMessage(RSK_IPC_STREAMING_CONNECTION_STATE_CHANGED, (char*)cmd.get(), sizeof(int));
}

void RSKVideoSource::release()
{
    delete this;
}

void RSKVideoSource::onMessage(unsigned int msg, char* payload, unsigned int len)
{
	LOG_ENTER;
	if (!m_initialized) {
		LOG_ERROR("%s, no init.\n", __FUNCTION__);
		LOG_LEAVE;
		return;
	}

	LOG_INFO("%s, msg : %d\n", __FUNCTION__, msg);

	if (msg == RSK_IPC_START_STREAMING) {
		char* url = payload;
		LOG_INFO("%s, startStreaming, url:%s\n", __FUNCTION__, url);
		m_rskEngine->startStreaming(url);
	}
	else if (msg == RSK_IPC_STOP_STREAMING) {
		m_ipcReceiverVideo.reset();
		m_ipcReceiverAudio.reset();

		LOG_INFO("%s, stopStreaming\n", __FUNCTION__);
		m_rskEngine->stopStreaming();
	}
	else if (msg == RSK_IPC_ENABLE_AUDIO_RECORDING) {

	}
	else if (msg == RSK_IPC_ENABLE_VIDEO_CAPTURING) {

	}
	else if (msg == RSK_IPC_MUTE_AUDIO_STREAM) {
		MuteAudioCmd* cmd = (MuteAudioCmd*)payload;
		LOG_INFO("%s, muteAudioStream, mute:%d\n", __FUNCTION__, cmd->mute);
		m_rskEngine->muteAudioStream(cmd->mute);
	}
	else if (msg == RSK_IPC_MUTE_VIDEO_SYREAM) {
		MuteVideoCmd* cmd = (MuteVideoCmd*)payload;
		LOG_INFO("%s, muteVideoStream, mute:%d\n", __FUNCTION__, cmd->mute);
		m_rskEngine->muteVideoStream(cmd->mute);
	}
	else if (msg == RSK_IPC_SET_AUDIO_CONFIG) {
		AudioStreamConfigurationCmd* cmd = (AudioStreamConfigurationCmd*)payload;
		AudioStreamConfiguration asc;
		asc.sampleRateHz = cmd->sampleRateHz;
		asc.bytesPerSample = cmd->bytesPerSample;
		asc.numberOfChannels = cmd->numberOfChannels;
		asc.bitrate = cmd->bitrate;
		LOG_INFO("setAudioStreamConfiguration, sampleRateHz : %d, bytesPerSample : %d, numberOfChannels :%d, bitrate :%d\n", asc.sampleRateHz, asc.bytesPerSample, asc.numberOfChannels, asc.bitrate);
		m_rskEngine->setAudioStreamConfiguration(asc);
	}
	else if (msg == RSK_IPC_SET_VIDEO_CONFIG) {
		VideoStreamConfigurationCmd* cmd = (VideoStreamConfigurationCmd*)payload;
		agora::streaming::VideoStreamConfiguration vsc;
		vsc.width = cmd->width;
		vsc.height = cmd->height;
		vsc.framerate = cmd->framerate;
		vsc.bitrate = cmd->bitrate;
		vsc.maxBitrate = cmd->maxBitrate;
		vsc.minBitrate = cmd->minBitrate;
		vsc.orientationMode = ORIENTATION_MODE_FIXED_LANDSCAPE;//(ORIENTATION_MODE)cmd->orientationMode;
		vsc.mirrorMode = (VIDEO_MIRROR_MODE_TYPE)cmd->mirrorMode;
		vsc.videoEncodingMode = (VIDEO_ENCODING_MODE_TYPE)cmd->videoEncodingMode;
		LOG_INFO("setVideoStreamConfiguration, width : %d, height : %d, framerate :%d, bitrate :%d, maxBitrate:%d,minBitrate:%d,orientationMode:%d, mirrorMode:%d, videoEncodingMode:%d \n",
			vsc.width, vsc.height, vsc.framerate, vsc.bitrate, vsc.maxBitrate, vsc.minBitrate, vsc.orientationMode, vsc.mirrorMode, vsc.videoEncodingMode);
		m_rskEngine->setVideoStreamConfiguration(vsc);
	}
	else if (msg == RSK_IPC_SWITCH_RESOLUTION) {
		SwitchResolutionCmd* cmd = (SwitchResolutionCmd*)payload;
		LOG_INFO("%s, switchResolution, width:%d, height:%d\n", __FUNCTION__, cmd->width, cmd->height);
		m_rskEngine->switchResolution(cmd->width, cmd->height);
	}
	else if (msg == RSK_IPC_ADJUST_RECORDING_SIGNAL_VOLUME) {
		RecordingSignalVolumeCmd* cmd = (RecordingSignalVolumeCmd*)payload;
		LOG_INFO("%s, adjustRecordingSignalVolume, volume:%d\n", __FUNCTION__, cmd->volume);
		m_rskEngine->adjustRecordingSignalVolume(cmd->volume);
	}
	else if (msg == RSK_IPC_EXTERNAL_AUDIO_FRAME_PARAMTER) {
		ExternalAudioFrameCmd* cmd = (ExternalAudioFrameCmd*)payload;
		audioFrame.channels = cmd->audioParameter.audioChannel;
		audioFrame.samplesPerSec = cmd->audioParameter.sampleRate;
		audioFrame.bytesPerSample = cmd->audioParameter.audioChannel * sizeof(short);
		if (audioFrame.buffer) {
			delete[] audioFrame.buffer;
			audioFrame.buffer = new uint8_t[cmd->audioParameter.audioSize];
		}
		LOG_INFO("%s, set external audio frame parameter, audioChannel:%d, sampleRate:%d, samplesPerChannel:%d, audioSize:%d\n",
			__FUNCTION__, audioFrame.channels, audioFrame.samplesPerSec, audioFrame.bytesPerSample, cmd->audioParameter.audioSize);
	}
	else if (msg == RSK_IPC_EXTERNAL_VIDEO_FRAME_PARAMTER) {
		ExternalVideoFrameCmd* cmd = (ExternalVideoFrameCmd*)payload;
		if (videoFrame.yBuffer) {
			delete[] videoFrame.yBuffer;
			videoFrame.yBuffer = new uint8_t[cmd->width * cmd->height];
		}

		if (videoFrame.uBuffer) {
			delete[] videoFrame.uBuffer;
			videoFrame.uBuffer = new uint8_t[1920 * 1080 / 4];
		}

		if (videoFrame.vBuffer) {
			delete[] videoFrame.vBuffer;
			videoFrame.vBuffer = new uint8_t[1920 * 1080 / 4];
		}
		
		LOG_INFO("%s, set external video frame parameter, width:%d, height:%d\n", __FUNCTION__, cmd->width, cmd->height);
	}
    LOG_LEAVE;
}

void RSKVideoSource::deliverFrame(const char* payload, int len)
{
	VideoFrameIpcHeader* videoHeader = (VideoFrameIpcHeader*)payload;
	char* ybuffer = const_cast<char*>(payload) + sizeof(VideoFrameIpcHeader);
	char* ubuffer = ybuffer + videoHeader->width*videoHeader->height;
	char* vbuffer = ubuffer + videoHeader->width*videoHeader->height / 4;

	memcpy(videoFrame.yBuffer, ybuffer, videoHeader->width*videoHeader->height);
	memcpy(videoFrame.uBuffer, ubuffer, videoHeader->width*videoHeader->height / 4);
	memcpy(videoFrame.vBuffer, vbuffer, videoHeader->width*videoHeader->height / 4);
	videoFrame.width = videoHeader->width;
	videoFrame.height = videoHeader->height;
	videoFrame.type = agora::media::base::VIDEO_PIXEL_I420;
	videoFrame.renderTimeMs = videoHeader->renderTimeMs;

	videoFrame.yStride = videoHeader->width;
	videoFrame.uStride = videoHeader->width / 2;
	videoFrame.vStride = videoHeader->width / 2;
	m_rskEngine->pushExternalVideoFrame(videoFrame);
}

void RSKVideoSource::deliverAudioFrame(const char* payload, int len)
{
	AudioFrameIpcHeader* audioHeader = (AudioFrameIpcHeader*)payload;
	audioFrame.channels = audioHeader->audioChannel;
	audioFrame.samplesPerSec = audioHeader->sampleRate;
	audioFrame.samplesPerChannel = audioHeader->samplesPerChannel;
	char* buffer = const_cast<char*>(payload) + sizeof(AudioFrameIpcHeader);
    
#ifdef TEST

	int audio_bytes = fread(audioFrame.buffer, 1, 480 * 2 * 2, fpPcm);
	read_audio_size += audio_bytes;
	if (read_audio_size == audio_file_size) {
		fseek(fpPcm, 0, SEEK_SET);
		read_audio_size = 0;
	}
#endif
	memcpy(audioFrame.buffer, buffer, audioHeader->audioSize);
	audioFrame.renderTimeMs = audioHeader->renderTimeMs;
	m_rskEngine->pushExternalAudioFrame(audioFrame);
}

void RSKVideoSource::exit(bool notifySink)
{
    {
		m_ipcReceiverVideo.reset();
		m_ipcReceiverAudio.reset();
       // std::lock_guard<std::mutex> lock(m_ipcSenderVideoMutex);
       // m_ipcSenderVideo.reset();
    }
    m_ipc->disconnect();
}

void RSKVideoSource::run()
{
    LOG_ENTER;
#ifdef _WIN32
    std::string idstr = m_paramParser->getParameter("pid");
#else
    std::string idstr = m_paramParser->getParameter("fd");
#endif
    if (idstr.empty()){
        LOG_ERROR("%s, pid is null\n", __FUNCTION__);
        LOG_LEAVE;
        return;
    }
    m_process.reset(INodeProcess::OpenNodeProcess(std::atoi(idstr.c_str())));
    if (!m_process.get()){
        LOG_ERROR("Process open fail.\n");
        LOG_LEAVE;
        return;
    }
    m_process->Monitor([this](INodeProcess*) {
        LOG_WARNING("%s, remote process ext.\n", __FUNCTION__);
        this->exit(false);
    });
    m_ipc->run();
    LOG_LEAVE;
}

void initLogService()
{
    std::string currentPath;
    INodeProcess::getCurrentModuleFileName(currentPath);
    std::string logFile = currentPath + ".txt";
    startLogService(logFile.c_str());
}

int main(int argc, char* argv[])
{
	SetProcessDPIAware();
    initLogService();
    if (argc < 3){ 
        LOG_ERROR("Need at least 3 parameter. Current parameter num : %d\n", argc);
        return 0;
    }
    std::string param;
    for (int i = 1; i < argc; i++) {
        param.append(argv[i]);
        param.append(" ");
    }
	
    LOG_INFO("Args : %s\n", param.c_str());
    auto videoSource = new RSKVideoSource(param);
    videoSource->initialize();
    videoSource->run();
    videoSource->release();
    stopLogService();
}
