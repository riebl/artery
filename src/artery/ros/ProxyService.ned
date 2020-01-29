package artery.ros;

import artery.application.ItsG5Service;

simple ProxyService like ItsG5Service
{
    parameters:
        @class(RosProxyService);
}
