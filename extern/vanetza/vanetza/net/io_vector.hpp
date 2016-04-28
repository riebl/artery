#ifndef IO_VECTOR_HPP_A3ANMI8B
#define IO_VECTOR_HPP_A3ANMI8B

#include <vector>
#include <sys/socket.h>
#include <sys/types.h>

namespace vanetza
{

// forward declaration
class BufferPacket;

/**
 * IoVector eaeses population of struct msghdr.
 * struct msghdr is required for sendmsg() calls.
 */
class IoVector
{
public:
    void append(const void* base, std::size_t length);
    void append(const BufferPacket&);
    void clear();
    std::size_t length() const;
    const iovec* base() const;

private:
    std::vector<iovec> m_vector;
};

} // namespace vanetza

#endif /* IO_VECTOR_HPP_A3ANMI8B */

