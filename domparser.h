#ifndef DOMPARSER_H
#define DOMPARSER_H

#include <QDomDocument>
#include "dommodel.h"

class DomParser
{
public:

    DomParser();
    ~DomParser();

    static DomModel *fromDom(QDomDocument d);
    static QDomDocument toDom(DomModel *m);

private:
    static void parseElement(QDomElement &n, DomItem *item);
    static void parseItem(DomItem *item, QDomElement &n, QDomDocument &doc);
};

#endif // DOMPARSER_H
