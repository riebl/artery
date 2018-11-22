package artery.networking;

simple SecurityEntity like ISecurityEntity
{
    parameters:
        @class(SecurityEntity);
        string runtimeModule;
        string positionModule;

        string CryptoBackend = default("Null");
        string CertificateProvider = default("Null");
        string CertificateValidator = default("NullOk");
        string SignService = default("dummy");
        string VerifyService = default("dummy");
}
