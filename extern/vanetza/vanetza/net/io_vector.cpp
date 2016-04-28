#include "io_vector.hpp"
#include "buffer_packet.hpp"

namespace vanetza
{

void IoVector::clear()
{
    m_vector.clear();
}

void IoVector::append(const void* base, std::size_t length)
{
    iovec node;
    node.iov_base = const_cast<void*>(base);
    node.iov_len = length;
    m_vector.push_back(node);
}

void IoVector::append(const BufferPacket& packet)
{
    for (auto& kv : packet) {
        const ByteBuffer& buffer = kv.second;
        append(buffer.data(), buffer.size());
    }
}

std::size_t IoVector::length() const
{
    return m_vector.size();
}

const iovec* IoVector::base() const
{
    return m_vector.empty() ? nullptr : &m_vector[0];
}

} // namespace vanetza

