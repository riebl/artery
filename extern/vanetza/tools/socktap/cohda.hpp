#ifndef COHDA_HPP_GBENHCVN
#define COHDA_HPP_GBENHCVN

#include <vanetza/common/byte_buffer.hpp>
#include <vanetza/common/clock.hpp>

namespace vanetza
{

// forward declaration
namespace dcc { class DataRequest; }

ByteBuffer create_cohda_tx_header(const dcc::DataRequest&);

} // namespace vanetza

#endif /* COHDA_HPP_GBENHCVN */

