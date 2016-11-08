#ifndef BYTE_BUFFER_CONVERTIBLE_HPP_CFOQNR35
#define BYTE_BUFFER_CONVERTIBLE_HPP_CFOQNR35

#include <vanetza/common/byte_buffer.hpp>
#include <cstddef>
#include <memory>
#include <string>
#include <type_traits>

namespace vanetza
{
namespace convertible
{

struct byte_buffer
{
    virtual void convert(ByteBuffer&) const = 0;
    virtual std::size_t size() const = 0;
    virtual std::unique_ptr<byte_buffer> duplicate() const;
    virtual ~byte_buffer() {}
};

template<class T>
struct byte_buffer_impl;

template<>
struct byte_buffer_impl<ByteBuffer> : public byte_buffer
{
    byte_buffer_impl(ByteBuffer&& buffer) : m_buffer(std::move(buffer)) {}
    void convert(ByteBuffer& buf) const override { buf = m_buffer; }
    std::size_t size() const override { return m_buffer.size(); }

    ByteBuffer m_buffer;
};

template<>
struct byte_buffer_impl<std::unique_ptr<ByteBuffer>> : public byte_buffer
{
    byte_buffer_impl(std::unique_ptr<ByteBuffer> buf) :
        m_buffer(std::move(buf)) {}
    void convert(ByteBuffer& buf) const override { buf = *m_buffer; }
    std::size_t size() const override { return m_buffer->size(); }

    std::unique_ptr<ByteBuffer> m_buffer;
};

template<>
struct byte_buffer_impl<std::string> : public byte_buffer
{
    byte_buffer_impl(const std::string& str);
    byte_buffer_impl(std::string&& str);
    void convert(ByteBuffer& buffer) const override;
    std::size_t size() const override;

    std::string m_buffer;
};

template<>
struct byte_buffer_impl<std::nullptr_t> : public byte_buffer
{
    void convert(ByteBuffer& buffer) const override { buffer.clear(); }
    std::size_t size() const override { return 0; }
    std::unique_ptr<byte_buffer> duplicate() const override;
};

} // namespace convertible

/**
 * ByteBufferConvertible is an extensible mechanism for providing
 * ByteBuffer representations of various data structures and objects.
 *
 * ByteBufferConvertible utilizes type erasure and accesses byte buffer data
 * through the convertible::byte_buffer interface.
 * Extending ByteBufferConvertible is possible by providing specializations
 * of convertible::byte_buffer_impl<T> implementing convertible::byte_buffer.
 */
class ByteBufferConvertible
{
public:
    ByteBufferConvertible() :
        m_wrapper(new convertible::byte_buffer_impl<std::nullptr_t>()) {}

    ByteBufferConvertible(std::unique_ptr<convertible::byte_buffer> ptr) :
        m_wrapper(std::move(ptr)) {}

    template<class T>
    ByteBufferConvertible(T&& t) :
        m_wrapper(new convertible::byte_buffer_impl<typename std::decay<T>::type>(std::forward<T>(t))) {}

    ByteBufferConvertible(const ByteBufferConvertible&);
    ByteBufferConvertible& operator=(const ByteBufferConvertible&);

    ByteBufferConvertible(ByteBufferConvertible&& other) = default;
    ByteBufferConvertible& operator=(ByteBufferConvertible&& other) = default;

    void convert(ByteBuffer& destination) const { m_wrapper->convert(destination); }
    std::size_t size() const { return m_wrapper->size(); }

    const convertible::byte_buffer* ptr() const { return m_wrapper.get(); }
    convertible::byte_buffer* ptr() { return m_wrapper.get(); }

private:
    std::unique_ptr<convertible::byte_buffer> m_wrapper;
};

} // namespace vanetza

#endif /* BYTE_BUFFER_CONVERTIBLE_HPP_CFOQNR35 */

