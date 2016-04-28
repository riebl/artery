#include <vanetza/security/exception.hpp>
#include <vanetza/security/subject_attribute.hpp>

namespace vanetza
{
namespace security
{

SubjectAttributeType get_type(const SubjectAttribute& sub)
{
    struct subject_attribute_visitor : public boost::static_visitor<SubjectAttributeType>
    {
        SubjectAttributeType operator()(VerificationKey key)
        {
            return SubjectAttributeType::Verification_Key;
        }
        SubjectAttributeType operator()(EncryptionKey key)
        {
            return SubjectAttributeType::Encryption_Key;
        }
        SubjectAttributeType operator()(SubjectAssurance assurance)
        {
            return SubjectAttributeType::Assurance_Level;
        }
        SubjectAttributeType operator()(std::list<IntX> list)
        {
            return SubjectAttributeType::Its_Aid_List;
        }
        SubjectAttributeType operator()(EccPoint ecc)
        {
            return SubjectAttributeType::Reconstruction_Value;
        }
        SubjectAttributeType operator()(std::list<ItsAidSsp> list)
        {
            return SubjectAttributeType::Its_Aid_Ssp_List;
        }
    };

    subject_attribute_visitor visit;
    return boost::apply_visitor(visit, sub);
}

void serialize(OutputArchive& ar, const ItsAidSsp& its_aid_ssp)
{
    serialize(ar, its_aid_ssp.its_aid);
    size_t size = its_aid_ssp.service_specific_permissions.size();
    serialize_length(ar, size);
    for (auto& byte : its_aid_ssp.service_specific_permissions) {
        ar << byte;
    }
}

size_t deserialize(InputArchive& ar, ItsAidSsp& its_aid_ssp)
{
    size_t size = 0;
    size += deserialize(ar, its_aid_ssp.its_aid);
    const size_t buf_size = deserialize_length(ar);
    its_aid_ssp.service_specific_permissions.resize(buf_size);
    size += buf_size + length_coding_size(buf_size);
    for (size_t i = 0; i < buf_size; ++i) {
        ar >> its_aid_ssp.service_specific_permissions[i];
    }
    return size;
}

size_t get_size(const SubjectAssurance& assurance)
{
    return sizeof(assurance.raw);
}

size_t get_size(const ItsAidSsp& its_aid_ssp)
{
    size_t size = get_size(its_aid_ssp.its_aid);
    size += its_aid_ssp.service_specific_permissions.size();
    size += length_coding_size(its_aid_ssp.service_specific_permissions.size());
    return size;
}

size_t get_size(const SubjectAttribute& sub)
{
    size_t size = sizeof(SubjectAttributeType);
    struct subject_attribute_visitor : public boost::static_visitor<size_t>
    {
        size_t operator()(const VerificationKey& key)
        {
            return get_size(key.key);
        }
        size_t operator()(const EncryptionKey& key)
        {
            return get_size(key.key);
        }
        size_t operator()(const SubjectAssurance& assurance)
        {
            return get_size(assurance);
        }
        size_t operator()(const std::list<IntX>& list)
        {
            size_t size = get_size(list);
            size += length_coding_size(size);
            return size;
        }
        size_t operator()(const EccPoint& ecc)
        {
            return get_size(ecc);
        }
        size_t operator()(const std::list<ItsAidSsp>& list)
        {
            size_t size = get_size(list);
            size += length_coding_size(size);
            return size;
        }
    };

    subject_attribute_visitor visit;
    size += boost::apply_visitor(visit, sub);
    return size;
}

void serialize(OutputArchive& ar, const SubjectAttribute& subjectAttribute)
{
    struct subject_attribute_visitor : public boost::static_visitor<>
    {
        subject_attribute_visitor(OutputArchive& ar) :
            m_archive(ar)
        {
        }
        void operator()(const VerificationKey& key)
        {
            serialize(m_archive, key.key);
        }
        void operator()(const EncryptionKey& key)
        {
            serialize(m_archive, key.key);
        }
        void operator()(const SubjectAssurance& assurance)
        {
            m_archive << assurance.raw;
        }
        void operator()(const std::list<IntX>& list)
        {
            serialize(m_archive, list);
        }
        void operator()(const EccPoint& ecc)
        {
            // TODO: specification of corresponding public key algorithm is missing
            throw serialization_error("unsupported serialization of SubjectAttribute with EccPoint");
        }
        void operator()(const std::list<ItsAidSsp>& list)
        {
            serialize(m_archive, list);
        }
        OutputArchive& m_archive;
    };

    SubjectAttributeType type = get_type(subjectAttribute);
    serialize(ar, type);
    subject_attribute_visitor visit(ar);
    boost::apply_visitor(visit, subjectAttribute);
}

size_t deserialize(InputArchive& ar, SubjectAttribute& sub)
{
    SubjectAttributeType type;
    size_t size = 0;
    deserialize(ar, type);
    size += sizeof(type);
    switch (type) {
        case SubjectAttributeType::Assurance_Level: {
            SubjectAssurance assurance;
            ar >> assurance.raw;
            size += get_size(assurance);
            sub = assurance;
            break;
        }
        case SubjectAttributeType::Verification_Key: {
            VerificationKey key;
            size += deserialize(ar, key.key);
            sub = key;
            break;
        }
        case SubjectAttributeType::Encryption_Key: {
            EncryptionKey key;
            size += deserialize(ar, key.key);
            sub = key;
            break;
        }
        case SubjectAttributeType::Its_Aid_List: {
            std::list<IntX> intx_list;
            size_t tmp_size = deserialize(ar, intx_list);
            size += tmp_size;
            size += length_coding_size(tmp_size);
            sub = intx_list;
            break;
        }
        case SubjectAttributeType::Its_Aid_Ssp_List: {
            std::list<ItsAidSsp> itsAidSsp_list;
            size_t tmp_size = deserialize(ar, itsAidSsp_list);
            size += tmp_size;
            size += length_coding_size(tmp_size);
            sub = itsAidSsp_list;
            break;
        }
        case SubjectAttributeType::Reconstruction_Value:
            throw deserialization_error("unsupported deserialization of SubjectAttribute with EccPoint");
            break;
        default:
            throw deserialization_error("Unknown SubjectAttributeType");
    }

    return size;
}

} //namespace security
} //namespace vanetza
