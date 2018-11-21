#include <iostream>
#include "ChecksumMediaCrypto.h"

static const size_t kChecksumSize = sizeof(uint16_t);
static const uint8_t payloadMask = 0x66;

// Naive implementation of Fletcher16 checksum 
// Taken from wikipedia https://en.wikipedia.org/wiki/Fletcher%27s_checksum
uint16_t Fletcher16(uint8_t *data, size_t count) {
   uint16_t sum1 = 0;
   uint16_t sum2 = 0;
   int index;
   for( index = 0; index < count; ++index ) {
      sum1 = (sum1 + data[index]) % 255;
      sum2 = (sum2 + sum1) % 255;
   }
   return (sum2 << 8) | sum1;
}

bool ChecksumMediaCrypto::Encrypt(cricket::MediaType type,
                                  const webrtc::RtpPacket* packet,
                                  uint32_t ssrc, bool first,
                                  bool last, bool is_intra, uint8_t* payload,
                                  size_t* payload_size) {
    // calculate checksum of original payload
    const uint16_t payload_checksum = Fletcher16(payload, *payload_size);
    for (size_t i = 0; i < *payload_size; i++) {
        payload[i] ^= payloadMask;
    }
    // add the checksum of the original payload as overhead
    // payload has enough room for max overhead over real payload
    payload[*payload_size] = (payload_checksum >> 8) & 0xff;
    payload[*payload_size + 1] = payload_checksum & 0xff;
    // add overhead to the payload_size
    *payload_size += sizeof(payload_checksum);
    return true;
}

size_t ChecksumMediaCrypto::GetMaxEncryptionOverhead() {
    return kChecksumSize;
}

bool ChecksumMediaCrypto::Decrypt(cricket::MediaType type,
                                  const webrtc::WebRtcRTPHeader* rtp_header,
                                  uint32_t ssrc, uint8_t* payload,
                                  size_t* payload_size) {
    // payload_size includes overhead
    size_t real_payload_size = *payload_size - kChecksumSize;
    for (size_t i = 0; i < real_payload_size; i++) {
        payload[i] ^= payloadMask;
    }
    // calculate checksum for the real payload after "decryption"
    const uint16_t payload_checksum = Fletcher16(payload, real_payload_size);
    // recover the original checksum added as overhead when "encrypting"
    uint16_t received_checksum;
    received_checksum = payload[real_payload_size] << 8;
    received_checksum |= payload[real_payload_size + 1]; 
    // do received checksum and actual checksum match?
    if (received_checksum != payload_checksum) {
        std::cerr << "ChecksumMediaCrypto - Decrypt error"
        << " - Received: " <<  received_checksum
        << " - Calculated: " << payload_checksum << std::endl;
        return false;
    }
    // update real payload size
    *payload_size = real_payload_size;
    return true;
}
