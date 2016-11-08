# FOKUS WebValidator for TS 103 097

Vanetza unit tests for SecuredMessage serialization use some third-party test data available online: [FOKUS WebValidator](https://werkzeug.dcaiti.tu-berlin.de/etsi/ts103097/)  
Unfortunately, the provided SecuredMessages are for protocol version 1 whereas Vanetza currently follows version 2. The original messages from FOKUS WebValidator are pasted into this document for reference because changes were necessary to adapt these for the recent protocol version.

## SecuredMessage/v1 (1)

``
010181038002010901A8ED6DF65B0E6D6A0100809400000418929DB6A9E452223062C52028E956BF9874E0A40D21D5F9F56564F39C5DD187C922F2E5F0630373879A43393373B9F6205BF01FBD9C1F113165C291C376F535010004EABA91A915D81807E910FD292D99DF8B401EED88CF7F031412D5ED9905F9996469798C412FC8F7237A3AB3469795E2DEF5E1B783EA4F6B6A2359D21772B2EA9D0200210AC040800101C0408101010F01099EB20109B1270003040100960000004B2E6D0D0EE9BC4AD9CD087B601E9AF06031995443D652763455FBB794B33982889260740EF64CFA8C6808A58F98E06CE42A1E9C22A0785D7242647F7895ABFC0000009373931CD7580500021E011C983E690E5F6D755BD4871578A9427E7BC383903DC7DA3B560384013643010000FE8566BEA87B39E6411F80226E792D6E01E77B598F2BB1FCE7F2DD441185C07CEF0573FBFB9876B99FE811486F6F5D499E6114FC0724A67F8D71D2A897A7EB34
``

    struct SecuredMessage {
        uint8 protocol_version: 1
        uint8 security_profile: 1
        HeaderField<259> header_fields {
            struct HeaderField {
                HeaderFieldType type: signer_info (128)
                struct SignerInfo signer {
                    SignerInfoType type: certificate (2)
                    struct Certificate certificate {
                        uint8 version: 1
                        SignerInfo<9> signer_info_v1 {
                            struct SignerInfo {
                                SignerInfoType type: certificate_digest_with_sha256 (1)
                                HashedId8 digest: A8ED6DF65B0E6D6A
                            }
                        }
                        struct SubjectInfo subject_info {
                            SubjectType subject_type: authorization_ticket (1)
                            opaque<0> subject_name: 
                        }
                        SubjectAttribute<148> subject_attributes {
                            struct SubjectAttribute {
                                SubjectAttributeType type: verification_key (0)
                                struct PublicKey key {
                                    PublicKeyAlgorithm algorithm: ecdsa_nistp256_with_sha256 (0)
                                    struct EccPoint public_key {
                                        EccPointType type: uncompressed (4)
                                        opaque[32] x: 18929DB6A9E452223062C52028E956BF9874E0A40D21D5F9F56564F39C5DD187
                                        opaque[32] y: C922F2E5F0630373879A43393373B9F6205BF01FBD9C1F113165C291C376F535
                                    }
                                }
                            }
                            struct SubjectAttribute {
                                SubjectAttributeType type: encryption_key (1)
                                struct PublicKey key {
                                    PublicKeyAlgorithm algorithm: ecdsa_nistp256_with_sha256 (0)
                                    struct EccPoint public_key {
                                        EccPointType type: uncompressed (4)
                                        opaque[32] x: EABA91A915D81807E910FD292D99DF8B401EED88CF7F031412D5ED9905F99964
                                        opaque[32] y: 69798C412FC8F7237A3AB3469795E2DEF5E1B783EA4F6B6A2359D21772B2EA9D
                                    }
                                }
                            }
                            struct SubjectAttribute {
                                SubjectAttributeType type: assurance_level (2)
                                SubjectAssurance assurance_level: assurance level = 0, confidence = 0 (bitmask = 0)
                            }
                            struct SubjectAttribute {
                                SubjectAttributeType type: its_aid_ssp_list (33)
                                ItsAidSsp<10> its_aid_ssp_list {
                                    struct ItsAidSsp {
                                        IntX its_aid: 16512
                                        opaque<1> service_specific_permissions: 01
                                    }
                                    struct ItsAidSsp {
                                        IntX its_aid: 16513
                                        opaque<1> service_specific_permissions: 01
                                    }
                                }
                            }
                        }
                        ValidityRestriction<15> validity_restrictions {
                            struct ValidityRestriction {
                                ValidityRestrictionType type: time_start_and_end (1)
                                Time32 start_validity: 2015-02-12 00:00:00 UTC
                                Time32 end_validity: 2015-02-25 23:59:59 UTC
                            }
                            struct ValidityRestriction {
                                ValidityRestrictionType type: region (3)
                                struct GeographicRegion region {
                                    RegionType region_type: id (4)
                                    struct IdentifiedRegion id_region {
                                        RegionDictionary region_dictionary: un_stats (1)
                                        uint16 region_identifier: 150
                                        IntX local_region: 0
                                    }
                                }
                            }
                        }
                        struct Signature {
                            PublicKeyAlgorithm algorithm: ecdsa_nistp256_with_sha256 (0)
                            struct EcdsaSignature ecdsa_signature {
                                struct EccPoint R {
                                    EccPointType type: x_coordinate_only (0)
                                    opaque[32] x: 4B2E6D0D0EE9BC4AD9CD087B601E9AF06031995443D652763455FBB794B33982
                                }
                                opaque[32] s: 889260740EF64CFA8C6808A58F98E06CE42A1E9C22A0785D7242647F7895ABFC
                            }
                        }
                    }
                }
            }
            struct HeaderField {
                HeaderFieldType type: generation_time (0)
                Time64 generation_time: 2015-02-20 10:36:37.663 UTC
            }
            struct HeaderField {
                HeaderFieldType type: message_type (5)
                uint16 message_type: 2
            }
        }
        Payload<30> payload_fields {
            struct Payload {
                PayloadType type: signed (1)
                opaque<28> data: 983E690E5F6D755BD4871578A9427E7BC383903DC7DA3B5603840136
            }
        }
        TrailerField<67> trailer_fields {
            struct TrailerField {
                TrailerFieldType type: signature (1)
                struct Signature signature {
                    PublicKeyAlgorithm algorithm: ecdsa_nistp256_with_sha256 (0)
                    struct EcdsaSignature ecdsa_signature {
                        struct EccPoint R {
                            EccPointType type: x_coordinate_only (0)
                            opaque[32] x: FE8566BEA87B39E6411F80226E792D6E01E77B598F2BB1FCE7F2DD441185C07C
                        }
                        opaque[32] s: EF0573FBFB9876B99FE811486F6F5D499E6114FC0724A67F8D71D2A897A7EB34
                    }
                }
            }
        }
    }

## SecuredMessage/v1 (2)

``
010181038002010901A8ED6DF65B0E6D6A01008094000004C4EC137145DD4F450145DE530CCA36E73AB3D87FC8275847CDAD8248C1CD20879BD6A8CB54EA9E05D3B41376CE2F24789AEF82836CA818D568ADF4A140E96E48010004D6C268EE68B5B8B387B2312B7E1D21CE0C366D251A32431508B96EB6A3479CCF96A8738F30ED451F00DA8DDE84367C7EB16727D14FF14F5DD8F9791FE0A12A640200210AC040800101C0408101010F01099EB20109B1270003040100960000001EB035FE8E51DCDD8558DE0BE9B87895B36B420583A5C6B2B8B2EAB7F3D3C99163638FA025A0033D4BD80BBA02B8E3DE1B55766459D494677AF24917E51B80AC0000009373CC5F22C8050002220120F29384759027349075829034707ABABABABABAABAB98437985739845783974954301000081E7CDB6D2C741C1700822305C39E8E809622AF9FCA1C0786F762D08E80580C42F1FCC1D5499577210834C390BB4613E102DECB14F575A2820743DC9A66BBD7A
``

    struct SecuredMessage {
        uint8 protocol_version: 1
        uint8 security_profile: 1
        HeaderField<259> header_fields {
            struct HeaderField {
                HeaderFieldType type: signer_info (128)
                struct SignerInfo signer {
                    SignerInfoType type: certificate (2)
                    struct Certificate certificate {
                        uint8 version: 1
                        SignerInfo<9> signer_info_v1 {
                            struct SignerInfo {
                                SignerInfoType type: certificate_digest_with_sha256 (1)
                                HashedId8 digest: A8ED6DF65B0E6D6A
                            }
                        }
                        struct SubjectInfo subject_info {
                            SubjectType subject_type: authorization_ticket (1)
                            opaque<0> subject_name: 
                        }
                        SubjectAttribute<148> subject_attributes {
                            struct SubjectAttribute {
                                SubjectAttributeType type: verification_key (0)
                                struct PublicKey key {
                                    PublicKeyAlgorithm algorithm: ecdsa_nistp256_with_sha256 (0)
                                    struct EccPoint public_key {
                                        EccPointType type: uncompressed (4)
                                        opaque[32] x: C4EC137145DD4F450145DE530CCA36E73AB3D87FC8275847CDAD8248C1CD2087
                                        opaque[32] y: 9BD6A8CB54EA9E05D3B41376CE2F24789AEF82836CA818D568ADF4A140E96E48
                                    }
                                }
                            }
                            struct SubjectAttribute {
                                SubjectAttributeType type: encryption_key (1)
                                struct PublicKey key {
                                    PublicKeyAlgorithm algorithm: ecdsa_nistp256_with_sha256 (0)
                                    struct EccPoint public_key {
                                        EccPointType type: uncompressed (4)
                                        opaque[32] x: D6C268EE68B5B8B387B2312B7E1D21CE0C366D251A32431508B96EB6A3479CCF
                                        opaque[32] y: 96A8738F30ED451F00DA8DDE84367C7EB16727D14FF14F5DD8F9791FE0A12A64
                                    }
                                }
                            }
                            struct SubjectAttribute {
                                SubjectAttributeType type: assurance_level (2)
                                SubjectAssurance assurance_level: assurance level = 0, confidence = 0 (bitmask = 0)
                            }
                            struct SubjectAttribute {
                                SubjectAttributeType type: its_aid_ssp_list (33)
                                ItsAidSsp<10> its_aid_ssp_list {
                                    struct ItsAidSsp {
                                        IntX its_aid: 16512
                                        opaque<1> service_specific_permissions: 01
                                    }
                                    struct ItsAidSsp {
                                        IntX its_aid: 16513
                                        opaque<1> service_specific_permissions: 01
                                    }
                                }
                            }
                        }
                        ValidityRestriction<15> validity_restrictions {
                            struct ValidityRestriction {
                                ValidityRestrictionType type: time_start_and_end (1)
                                Time32 start_validity: 2015-02-12 00:00:00 UTC
                                Time32 end_validity: 2015-02-25 23:59:59 UTC
                            }
                            struct ValidityRestriction {
                                ValidityRestrictionType type: region (3)
                                struct GeographicRegion region {
                                    RegionType region_type: id (4)
                                    struct IdentifiedRegion id_region {
                                        RegionDictionary region_dictionary: un_stats (1)
                                        uint16 region_identifier: 150
                                        IntX local_region: 0
                                    }
                                }
                            }
                        }
                        struct Signature {
                            PublicKeyAlgorithm algorithm: ecdsa_nistp256_with_sha256 (0)
                            struct EcdsaSignature ecdsa_signature {
                                struct EccPoint R {
                                    EccPointType type: x_coordinate_only (0)
                                    opaque[32] x: 1EB035FE8E51DCDD8558DE0BE9B87895B36B420583A5C6B2B8B2EAB7F3D3C991
                                }
                                opaque[32] s: 63638FA025A0033D4BD80BBA02B8E3DE1B55766459D494677AF24917E51B80AC
                            }
                        }
                    }
                }
            }
            struct HeaderField {
                HeaderFieldType type: generation_time (0)
                Time64 generation_time: 2015-02-20 10:52:38.309 UTC
            }
            struct HeaderField {
                HeaderFieldType type: message_type (5)
                uint16 message_type: 2
            }
        }
        Payload<34> payload_fields {
            struct Payload {
                PayloadType type: signed (1)
                opaque<32> data: F29384759027349075829034707ABABABABABAABAB9843798573984578397495
            }
        }
        TrailerField<67> trailer_fields {
            struct TrailerField {
                TrailerFieldType type: signature (1)
                struct Signature signature {
                    PublicKeyAlgorithm algorithm: ecdsa_nistp256_with_sha256 (0)
                    struct EcdsaSignature ecdsa_signature {
                        struct EccPoint R {
                            EccPointType type: x_coordinate_only (0)
                            opaque[32] x: 81E7CDB6D2C741C1700822305C39E8E809622AF9FCA1C0786F762D08E80580C4
                        }
                        opaque[32] s: 2F1FCC1D5499577210834C390BB4613E102DECB14F575A2820743DC9A66BBD7A
                    }
                }
            }
        }
    }

## SecuredMessage/v1 (3)

``
010181038002010901A8ED6DF65B0E6D6A010080940000040209B0434163CCBAFDD34A45333E418FB96C05BBE0E7E1D755D40D0B4BBE8DA508EC2F2723B7ADF0F27C39F3AECFF0783C196F9961F8821E6294375D9294CD6A01000452113CE698DB081491675DF8FFE81C23EA5D0071B2D2BF0E0DA4ADA0CDA58259CA5D999200B6565E194EDAB8BD3DCA863F2DDF39C13E7A0375ECE2566C5EB8C60200210AC040800101C0408101010F01099EB20109B1270003040100960000008DA1F3F9F35E04C3DE77D7438988A8D57EBE44DAA021A4269E297C177C9CFE458E128EC290785D6631961625020943B6D87DAA54919A98F7865709929A7C6E480000009373CF482D400500020A01080123456789ABCDEF43010000371423BBA0902D8AF2FB2226D73A7781D4D6B6772650A8BEE5A1AF198CEDABA2C9BF57540C629E6A1E629B8812AEBDDDBCAF472F6586F16C14B3DEFBE9B6ADB2
``

    struct SecuredMessage {
        uint8 protocol_version: 1
        uint8 security_profile: 1
        HeaderField<259> header_fields {
            struct HeaderField {
                HeaderFieldType type: signer_info (128)
                struct SignerInfo signer {
                    SignerInfoType type: certificate (2)
                    struct Certificate certificate {
                        uint8 version: 1
                        SignerInfo<9> signer_info_v1 {
                            struct SignerInfo {
                                SignerInfoType type: certificate_digest_with_sha256 (1)
                                HashedId8 digest: A8ED6DF65B0E6D6A
                            }
                        }
                        struct SubjectInfo subject_info {
                            SubjectType subject_type: authorization_ticket (1)
                            opaque<0> subject_name: 
                        }
                        SubjectAttribute<148> subject_attributes {
                            struct SubjectAttribute {
                                SubjectAttributeType type: verification_key (0)
                                struct PublicKey key {
                                    PublicKeyAlgorithm algorithm: ecdsa_nistp256_with_sha256 (0)
                                    struct EccPoint public_key {
                                        EccPointType type: uncompressed (4)
                                        opaque[32] x: 0209B0434163CCBAFDD34A45333E418FB96C05BBE0E7E1D755D40D0B4BBE8DA5
                                        opaque[32] y: 08EC2F2723B7ADF0F27C39F3AECFF0783C196F9961F8821E6294375D9294CD6A
                                    }
                                }
                            }
                            struct SubjectAttribute {
                                SubjectAttributeType type: encryption_key (1)
                                struct PublicKey key {
                                    PublicKeyAlgorithm algorithm: ecdsa_nistp256_with_sha256 (0)
                                    struct EccPoint public_key {
                                        EccPointType type: uncompressed (4)
                                        opaque[32] x: 52113CE698DB081491675DF8FFE81C23EA5D0071B2D2BF0E0DA4ADA0CDA58259
                                        opaque[32] y: CA5D999200B6565E194EDAB8BD3DCA863F2DDF39C13E7A0375ECE2566C5EB8C6
                                    }
                                }
                            }
                            struct SubjectAttribute {
                                SubjectAttributeType type: assurance_level (2)
                                SubjectAssurance assurance_level: assurance level = 0, confidence = 0 (bitmask = 0)
                            }
                            struct SubjectAttribute {
                                SubjectAttributeType type: its_aid_ssp_list (33)
                                ItsAidSsp<10> its_aid_ssp_list {
                                    struct ItsAidSsp {
                                        IntX its_aid: 16512
                                        opaque<1> service_specific_permissions: 01
                                    }
                                    struct ItsAidSsp {
                                        IntX its_aid: 16513
                                        opaque<1> service_specific_permissions: 01
                                    }
                                }
                            }
                        }
                        ValidityRestriction<15> validity_restrictions {
                            struct ValidityRestriction {
                                ValidityRestrictionType type: time_start_and_end (1)
                                Time32 start_validity: 2015-02-12 00:00:00 UTC
                                Time32 end_validity: 2015-02-25 23:59:59 UTC
                            }
                            struct ValidityRestriction {
                                ValidityRestrictionType type: region (3)
                                struct GeographicRegion region {
                                    RegionType region_type: id (4)
                                    struct IdentifiedRegion id_region {
                                        RegionDictionary region_dictionary: un_stats (1)
                                        uint16 region_identifier: 150
                                        IntX local_region: 0
                                    }
                                }
                            }
                        }
                        struct Signature {
                            PublicKeyAlgorithm algorithm: ecdsa_nistp256_with_sha256 (0)
                            struct EcdsaSignature ecdsa_signature {
                                struct EccPoint R {
                                    EccPointType type: x_coordinate_only (0)
                                    opaque[32] x: 8DA1F3F9F35E04C3DE77D7438988A8D57EBE44DAA021A4269E297C177C9CFE45
                                }
                                opaque[32] s: 8E128EC290785D6631961625020943B6D87DAA54919A98F7865709929A7C6E48
                            }
                        }
                    }
                }
            }
            struct HeaderField {
                HeaderFieldType type: generation_time (0)
                Time64 generation_time: 2015-02-20 10:53:27.136 UTC
            }
            struct HeaderField {
                HeaderFieldType type: message_type (5)
                uint16 message_type: 2
            }
        }
        Payload<10> payload_fields {
            struct Payload {
                PayloadType type: signed (1)
                opaque<8> data: 0123456789ABCDEF
            }
        }
        TrailerField<67> trailer_fields {
            struct TrailerField {
                TrailerFieldType type: signature (1)
                struct Signature signature {
                    PublicKeyAlgorithm algorithm: ecdsa_nistp256_with_sha256 (0)
                    struct EcdsaSignature ecdsa_signature {
                        struct EccPoint R {
                            EccPointType type: x_coordinate_only (0)
                            opaque[32] x: 371423BBA0902D8AF2FB2226D73A7781D4D6B6772650A8BEE5A1AF198CEDABA2
                        }
                        opaque[32] s: C9BF57540C629E6A1E629B8812AEBDDDBCAF472F6586F16C14B3DEFBE9B6ADB2
                    }
                }
            }
        }
    }
