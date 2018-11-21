#ifndef CHECKSUMMEDIACRYPTO_H
#define CHECKSUMMEDIACRYPTO_H

#include "api/mediatypes.h"
#include "api/mediacrypto.h"

class ChecksumMediaCrypto: public webrtc::MediaCrypto {
public:
    ChecksumMediaCrypto() = default;
    virtual ~ChecksumMediaCrypto() = default;
    virtual bool Encrypt(cricket::MediaType type,
                         const webrtc::RtpPacket* packet,
                         uint32_t ssrc, bool first,
                         bool last, bool is_intra, uint8_t* payload,
                         size_t* payload_size) override;
    virtual size_t GetMaxEncryptionOverhead() override;
    virtual bool Decrypt(cricket::MediaType type,
                         const webrtc::WebRtcRTPHeader* rtp_header,
                         uint32_t ssrc, uint8_t* payload,
                         size_t* payload_size) override;
};


#endif /* CHECKSUMMEDIACRYPTO_H */
