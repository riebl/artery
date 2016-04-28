#include <vanetza/asn1/gen/asn_application.h>
#include <vanetza/asn1/gen/constraints.h>
#include "asn1c_wrapper.hpp"
#include <vanetza/common/byte_buffer.hpp>
#include <boost/format.hpp>
#include <algorithm>
#include <cassert>
#include <iterator>
#include <memory>
#include <stdexcept>
#include <string>

namespace vanetza
{
namespace asn1
{

static int write_buffer(const void* in, std::size_t size, void* out_void)
{
    assert(out_void != nullptr);
    auto out = static_cast<ByteBuffer*>(out_void);
    std::copy_n(static_cast<const uint8_t*>(in), size, std::back_inserter(*out));
    return 0;
}

static int write_null(const void*, std::size_t, void*)
{
    return 0;
}

void* allocate(std::size_t length)
{
    void* ptr = calloc(1, length);
    if (nullptr == ptr) {
        throw std::runtime_error("Bad ASN.1 memory allocation");
    }
    return ptr;
}

void free(asn_TYPE_descriptor_t& td, void* t)
{
    if (t != nullptr) {
        ASN_STRUCT_FREE(td, t);
    }
}

void* copy(asn_TYPE_descriptor_t& td, const void* original)
{
    void* copy = nullptr;
    ByteBuffer buffer;

    asn_enc_rval_t ec;
    ec = der_encode(&td, const_cast<void*>(original), write_buffer, &buffer);
    if (ec.encoded == -1) {
        throw std::runtime_error("DER encoding failed");
    }

    asn_dec_rval_t dc;
    dc = ber_decode(0, &td, &copy, &buffer[0], buffer.size());
    if (dc.code != RC_OK) {
        free(td, copy);
        throw std::runtime_error("BER decoding failed");
    }

    return copy;
}

bool validate(asn_TYPE_descriptor_t& td, const void* t)
{
    return asn_check_constraints(&td, t, nullptr, nullptr) == 0;
}

bool validate(asn_TYPE_descriptor_t& td, const void* t, std::string& error)
{
    char errbuf[1024];
    std::size_t errlen = sizeof(errbuf);
    bool ok = asn_check_constraints(&td, t, errbuf, &errlen) == 0;
    if (!ok) {
        error = errbuf;
    }
    return ok;
}

std::size_t size(asn_TYPE_descriptor_t& td, const void* t)
{
    asn_enc_rval_t ec;
    ec = uper_encode(&td, const_cast<void*>(t), write_null, 0);
    if (ec.encoded < 0) {
        const char* failed_type = ec.failed_type ? ec.failed_type->name : "unknown";
        const auto error_msg = boost::format(
                "Can't determine size for unaligned PER encoding of type %1% because of %2% sub-type")
                % td.name % failed_type;
        throw std::runtime_error(error_msg.str());
    }

    // Caution! ec.encoded are bits not bytes!
    return (ec.encoded + 7) / 8;
}

ByteBuffer encode(asn_TYPE_descriptor_t& td, const void* t)
{
    ByteBuffer buffer;
    asn_enc_rval_t ec = uper_encode(&td, const_cast<void*>(t), write_buffer, &buffer);
    if (ec.encoded == -1) {
        const char* failed_type = ec.failed_type ? ec.failed_type->name : "unknown";
        const auto error_msg = boost::format(
                "Unaligned PER encoding of type %1% failed because of %2% sub-type")
                % td.name % failed_type;
        throw std::runtime_error(error_msg.str());
    }
    return buffer;
}

bool decode(asn_TYPE_descriptor_t& td, void** t, const ByteBuffer& buffer)
{
    asn_codec_ctx_t ctx {};
    asn_dec_rval_t ec = uper_decode_complete(&ctx, &td, t, buffer.data(), buffer.size());
    return ec.code == RC_OK;
}

} // namespace asn1
} // namespace vanetza

