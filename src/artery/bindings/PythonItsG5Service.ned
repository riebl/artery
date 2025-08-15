package artery.bindings;

import artery.application.ItsG5Service;

simple PythonItsG5Service like ItsG5Service {
    parameters:
        string pythonModule;
        string pythonServiceClass;
}