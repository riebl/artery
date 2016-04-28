#include <vanetza/security/exception.hpp>
#include <vanetza/security/validity_restriction.hpp>

namespace vanetza
{
namespace security
{

Duration::Duration() : m_raw(0)
{
}

Duration::Duration(BitNumber<uint16_t, 13> value, Units unit) :
    m_raw(static_cast<decltype(m_raw)>(unit))
{
    m_raw <<= 13;
    m_raw |= value.raw();
}

Duration::Duration(uint16_t raw) : m_raw(raw)
{
}

ValidityRestrictionType get_type(const ValidityRestriction& restriction)
{
    struct validity_restriction_visitor : public boost::static_visitor<ValidityRestrictionType>
    {
        ValidityRestrictionType operator()(const EndValidity& validity)
        {
            return ValidityRestrictionType::Time_End;
        }
        ValidityRestrictionType operator()(const StartAndEndValidity& validity)
        {
            return ValidityRestrictionType::Time_Start_And_End;
        }
        ValidityRestrictionType operator()(const StartAndDurationValidity& validity)
        {
            return ValidityRestrictionType::Time_Start_And_Duration;
        }
        ValidityRestrictionType operator()(const GeographicRegion& region)
        {
            return ValidityRestrictionType::Region;
        }
    };

    validity_restriction_visitor visit;
    return boost::apply_visitor(visit, restriction);
}

size_t get_size(const Time32& time)
{
    return sizeof(Time32);
}

size_t get_size(const Time64& time)
{
    return sizeof(Time64);
}

size_t get_size(const Duration& duration)
{
    return sizeof(uint16_t);
}

size_t get_size(const StartAndEndValidity& validity)
{
    size_t size = sizeof(validity.end_validity);
    size += sizeof(validity.start_validity);
    return size;
}

size_t get_size(const StartAndDurationValidity& validity)
{
    size_t size = sizeof(validity.start_validity);
    size += get_size(validity.duration);
    return size;
}

size_t get_size(const ValidityRestriction& restriction)
{
    size_t size = sizeof(ValidityRestrictionType);
    struct validity_restriction_visitor : public boost::static_visitor<size_t>
    {
        size_t operator()(const EndValidity& validity)
        {
            return sizeof(validity);
        }
        size_t operator()(const StartAndEndValidity& validity)
        {
            return get_size(validity);
        }
        size_t operator()(const StartAndDurationValidity& validity)
        {
            return get_size(validity);
        }
        size_t operator()(const GeographicRegion& region)
        {
            return get_size(region);
        }
    };

    validity_restriction_visitor visit;
    size += boost::apply_visitor(visit, restriction);
    return size;
}

size_t deserialize(InputArchive& ar, ValidityRestriction& restriction)
{
    size_t size = 0;
    ValidityRestrictionType type;
    deserialize(ar, type);
    size += sizeof(ValidityRestrictionType);
    switch (type) {
        case ValidityRestrictionType::Time_End: {
            EndValidity end;
            deserialize(ar, end);
            size += sizeof(end);
            restriction = end;
            break;
        }
        case ValidityRestrictionType::Time_Start_And_Duration: {
            StartAndDurationValidity validity;
            deserialize(ar, validity.start_validity);
            uint16_t duration;
            deserialize(ar, duration);
            validity.duration = Duration(duration);
            size += get_size(validity);
            restriction = validity;
            break;
        }
        case ValidityRestrictionType::Time_Start_And_End: {
            StartAndEndValidity validity;
            deserialize(ar, validity.start_validity);
            deserialize(ar, validity.end_validity);
            restriction = validity;
            size += get_size(validity);
            break;
        }
        case ValidityRestrictionType::Region: {
            GeographicRegion region;
            size += deserialize(ar, region);
            restriction = region;
            break;
        }
        default:
            throw deserialization_error("Unknown ValidityRestrictionType");
    }
    return size;
}

void serialize(OutputArchive& ar, const ValidityRestriction& restriction)
{
    struct validity_restriction_visitor : public boost::static_visitor<>
    {
        validity_restriction_visitor(OutputArchive& ar) :
            m_archive(ar)
        {
        }
        void operator()(const EndValidity& validity)
        {
            serialize(m_archive, host_cast(validity));
        }
        void operator()(const StartAndEndValidity& validity)
        {
            serialize(m_archive, host_cast(validity.start_validity));
            serialize(m_archive, host_cast(validity.end_validity));
        }
        void operator()(const StartAndDurationValidity& validity)
        {
            serialize(m_archive, host_cast(validity.start_validity));
            serialize(m_archive, host_cast(validity.duration.raw()));
        }
        void operator()(const GeographicRegion& region)
        {
            serialize(m_archive, region);
        }
        OutputArchive& m_archive;
    };

    ValidityRestrictionType type = get_type(restriction);
    serialize(ar, type);
    validity_restriction_visitor visit(ar);
    boost::apply_visitor(visit, restriction);
}

} // namespace security
} // namespace vanetza
