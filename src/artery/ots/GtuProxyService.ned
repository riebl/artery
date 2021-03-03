package artery.ots;

import artery.application.ItsG5Service;

simple GtuProxyService like ItsG5Service
{
    parameters:
        string otsCore;
        int aid = default(16480);
}