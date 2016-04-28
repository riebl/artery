#ifndef EXCEPTION_HPP_IY8LEMBQ
#define EXCEPTION_HPP_IY8LEMBQ

#include <stdexcept>

namespace vanetza
{
namespace security
{

/// thrown when a serialization error occurred
class serialization_error : public std::runtime_error
{
public:
    using std::runtime_error::runtime_error;
};

/// thrown when a deserialization error occurred
class deserialization_error : public std::runtime_error
{
public:
    using std::runtime_error::runtime_error;
};

} // namespace security
} // namespace vanetza

#endif /* EXCEPTION_HPP_IY8LEMBQ */
