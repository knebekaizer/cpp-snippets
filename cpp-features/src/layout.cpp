
#include <cstddef>
#include <cinttypes>
#include <arpa/inet.h>


#include <bit>
#include <type_traits>
#include <span>
#include <algorithm>
#include <ranges>
namespace rg = std::ranges;
namespace vs = std::ranges::views;

#include "trace.hpp"

#define HMP_CPU_ENDIAN_little

// TODO Use custom type for network endian integer. Use alias for host endian int
/// Sequential number / SN base - host endian
using SeqNum = uint16_t;
using Nui16 = uint16_t; ///< Network endian
using Nui32 = uint32_t; ///< Network endian
/// RFC 5109           RTP Payload Format for Generic FEC
///   7.3.  FEC Header for FEC Packets
/// @code
///   The FEC header is 10 octets.  The format of the header is shown in
///   Figure 3 and consists of extension flag (E bit), long-mask flag (L
///   bit), P recovery field, X recovery field, CC recovery field, M
///   recovery field, PT recovery field, SN base field, TS recovery field,
///   and length recovery field.
///
///    0                   1                   2                   3
///    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
///   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
///   |E|L|P|X|  CC   |M| PT recovery |            SN base            |
///   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
///   |                          TS recovery                          |
///   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
///   |        length recovery        |
///   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
/// @endcode
struct FECFixedHeader
{
//	if constexpr (std::endian::native == std::endian::big) {
#if defined HMP_CPU_ENDIAN_big
		/// Extension flag.
		uint8_t eBit : 1;
		/// long-mask flag / mask length 16/48 corresponds flag 0/1
		uint8_t longMask : 1;
		/// P recovery field.
		uint8_t pBit : 1;
		/// X recovery field. / Extended header flag.
		uint8_t xBit : 1;
		/// cc recovery field / Number of the Contributed Sources ID.
		uint8_t cc : 4;
		/// Mark recovery bit.
		uint8_t mark : 1;
		/// Payload type recovery.
		uint8_t payloadType : 7;
#elif defined HMP_CPU_ENDIAN_little
//	} else if constexpr (std::endian::native == std::endian::little) {

		/// cc recovery field / Number of the Contributed Sources ID.
		uint8_t cc : 4;
		/// X recovery field. / Extended header flag.
		uint8_t xBit : 1;
		/// P recovery field.
		uint8_t pBit : 1;
		/// long-mask flag / mask length 16/48 corresponds flag 0/1
		uint8_t longMask : 1;
		/// Extension flag.
		uint8_t eBit : 1;
		/// Payload type recovery.
		uint8_t payloadType : 7;
		/// Mark recovery bit.
		uint8_t mark : 1;
#endif // endian
//	} else {
//		static_assert(0, "mixed endian");
//	}
	/// Sequence number.
	Nui16 seqNumBase;
	/// RTP Timestamp.
	Nui32 timestamp;
	/// Synchronization source Id.
	Nui32 SSRC;

	Nui16 lengthRecovery;
};

///   7.4.  FEC Level Header for FEC Packets
/// @code
///   The FEC level header is 4 or 8 octets (depending on the L bit in the
///   FEC header).  The formats of the headers are shown in Figure 4.
///
///   The FEC level headers consist of a protection length field and a mask
///   field.  The protection length field is 16 bits long.  The mask field
///   is 16 bits long (when the L bit is not set) or 48 bits long (when the
///   L bit is set).
///
///   The mask field in the FEC level header indicates which packets are
///   associated with the FEC packet at the current level.  It is either 16
///   or 48 bits depending on the value of the L bit.  If bit i in the mask
///   is set to 1, then the media packet with sequence number N + i is
///   associated with this FEC packet, where N is the SN Base field in the
///   FEC packet header.  The most significant bit of the mask corresponds
///   to i=0, and the least significant to i=15 when the L bit is set to 0,
///   or i=47 when the L bit is set to 1.
///
///    0                   1                   2                   3
///    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
///   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
///   |       Protection Length       |             mask              |
///   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
///   |              mask cont. (present only when L = 1)             |
///   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
/// @endcode

struct FECLevelHeader
{
	const Nui16 protectionLength;
	/// Declare as size = 2, but real size may be 6 if longMask = 1
	const uint8_t mask[2] = {0};
};

/// All accessor methods return host endian integer
class FECHeader
{
public:
	uint16_t lengthRecovery() const
	{
		return ntohs(m_fecHdr.lengthRecovery);
	}
	uint16_t protectionLength() const
	{
		return ntohs(m_fecLevel0.protectionLength);
	}
	uint16_t baseSN() const
	{
		return ntohs(m_fecHdr.seqNumBase);
	};

	size_t lengthMaskBits() const
	{
		return m_fecHdr.longMask ? 48 : 16;
	}

	// TODO Optimize me
	auto maskIndices() const
	{
		return vs::iota(0, m_fecHdr.longMask ? 48 : 16) | vs::filter([&](auto k) { return 1 & (m_fecLevel0.mask[k >> 3] >> (k & 7)); });
	}

	bool hasBit(unsigned bitNo) const
	{
		if (bitNo > lengthMaskBits())
			return false;
		return m_fecLevel0.mask[bitNo >> 3] & (1u << (bitNo & 7));
	}
private:
	FECFixedHeader m_fecHdr;
	FECLevelHeader m_fecLevel0;
};

int main() {
	TraceX(std::is_standard_layout_v<FECHeader>);
	return 0;
}