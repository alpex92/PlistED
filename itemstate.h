#ifndef ITEMSTATE_H
#define ITEMSTATE_H

#include "dommodel.h"

class ItemState
{
public:
    ~ItemState();

private:
    friend class DomModel;

    ItemState(DomItem *other);
    void setState(DomItem *other);
    DomItem *getState();


private:
    DomItem *item;

};

#endif // ITEMSTATE_H
