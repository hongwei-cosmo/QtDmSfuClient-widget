#ifndef DM_PEER_CONNECTION_H
#define DM_PEER_CONNECTION_H

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "api/datachannelinterface.h"
#include "api/mediastreaminterface.h"
#include "api/peerconnectioninterface.h"
#include "dm_video_observer.h"
#include "ChecksumMediaCrypto.h"

class DMPeerConnection : public webrtc::PeerConnectionObserver,
                         public webrtc::CreateSessionDescriptionObserver,
                         public webrtc::DataChannelObserver,
                         public webrtc::AudioTrackSinkInterface {
 public:
  DMPeerConnection() {}
  ~DMPeerConnection() override {}

  bool InitializePeerConnection(const char** turn_urls,
                                const int no_of_urls,
                                const char* username,
                                const char* credential,
                                bool is_receiver);
  void DeletePeerConnection();
  void AddStreams(bool audio_only);
  bool CreateDataChannel();
  bool CreateOffer();
  bool CreateAnswer();
  bool SendDataViaDataChannel(const std::string& data);
  void SetAudioControl(bool is_mute, bool is_record);

  // Register callback functions.
  void RegisterOnLocalI420FrameReady(I420FrameReady_callback callback);
  void RegisterOnRemoteI420FrameReady(I420FrameReady_callback callback);
  void RegisterOnLocalDataChannelReady(LocalDataChannelReady_callback callback);
  void RegisterOnDataFromDataChannelReady(
      DataFromDataChannelReady_callback callback);
  void RegisterOnFailure(Failure_callback callback);
  void RegisterOnAudioBusReady(AudioBusReady_callback callback);
  void RegisterOnLocalSdpReadytoSend(LocalSdpReadyToSend_callback callback);
  void RegisterOnIceCandiateReadytoSend(
      IceCandidateReadyToSend_callback callback);
  bool SetRemoteDescription(const char* type, const char* sdp);
  bool AddIceCandidate(const char* sdp,
                       const int sdp_mlineindex,
                       const char* sdp_mid);

 protected:
  // create a peerconneciton and add the turn servers info to the configuration.
  bool CreatePeerConnection(const char** turn_urls,
                            const int no_of_urls,
                            const char* username,
                            const char* credential);
  void CloseDataChannel();
  std::unique_ptr<cricket::VideoCapturer> OpenVideoCaptureDevice();
  void SetAudioControl();

  // PeerConnectionObserver implementation.
  void OnSignalingChange(
      webrtc::PeerConnectionInterface::SignalingState new_state) override {}
  void OnAddStream(
      rtc::scoped_refptr<webrtc::MediaStreamInterface> stream) override;
  void OnRemoveStream(
      rtc::scoped_refptr<webrtc::MediaStreamInterface> stream) override {}
  void OnDataChannel(
      rtc::scoped_refptr<webrtc::DataChannelInterface> channel) override;
  void OnRenegotiationNeeded() override {}
  void OnIceConnectionChange(
      webrtc::PeerConnectionInterface::IceConnectionState new_state) override {}
  void OnIceGatheringChange(
      webrtc::PeerConnectionInterface::IceGatheringState new_state) override {}
  void OnIceCandidate(const webrtc::IceCandidateInterface* candidate) override;
  void OnIceConnectionReceivingChange(bool receiving) override {}

  // CreateSessionDescriptionObserver implementation.
  void OnSuccess(webrtc::SessionDescriptionInterface* desc) override;
  void OnFailure(const std::string& error) override;

  // DataChannelObserver implementation.
  void OnStateChange() override;
  void OnMessage(const webrtc::DataBuffer& buffer) override;

  // AudioTrackSinkInterface implementation.
  void OnData(const void* audio_data,
              int bits_per_sample,
              int sample_rate,
              size_t number_of_channels,
              size_t number_of_frames) override;

  // Get remote audio tracks ssrcs.
  std::vector<uint32_t> GetRemoteAudioTrackSsrcs();

 private:
  rtc::scoped_refptr<webrtc::PeerConnectionInterface> peer_connection_;
  rtc::scoped_refptr<webrtc::DataChannelInterface> data_channel_;
  std::map<std::string, rtc::scoped_refptr<webrtc::MediaStreamInterface> >
      active_streams_;

  std::unique_ptr<VideoObserver> local_video_observer_;
  std::unique_ptr<VideoObserver> remote_video_observer_;

  webrtc::MediaStreamInterface* remote_stream_ = nullptr;

  LocalDataChannelReady_callback OnLocalDataChannelReady = nullptr;
  DataFromDataChannelReady_callback OnDataFromDataChannelReady = nullptr;
  Failure_callback OnFailureMessage = nullptr;
  AudioBusReady_callback OnAudioReady = nullptr;

  LocalSdpReadyToSend_callback OnLocalSdpReady = nullptr;
  IceCandidateReadyToSend_callback OnIceCandiateReady = nullptr;

  bool is_mute_audio_ = false;
  bool is_record_audio_ = false;
  bool mandatory_receive_ = false;

  // disallow copy-and-assign
  DMPeerConnection(const DMPeerConnection&) = delete;
  DMPeerConnection& operator=(const DMPeerConnection&) = delete;
};

#endif  // DM_PEER_CONNECTION_H
