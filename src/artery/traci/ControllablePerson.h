#ifndef CONTROLLABLEPERSON_H_DJ96H4LS
#define CONTROLLABLEPERSON_H_DJ96H4LS

#include "artery/traci/PersonController.h"

class ControllablePerson
{
public:
    virtual traci::PersonController* getPersonController() = 0;
    virtual ~ControllablePerson() = default;
};

#endif /* CONTROLLABLEPERSON_H_DJ96H4LS */

