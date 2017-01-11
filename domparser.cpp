
#include "domparser.h"
#include <QRegExp>

DomParser::DomParser() {}
DomParser::~DomParser() {}

DomModel *DomParser::fromDom(QDomDocument d)
{
    // get root element
    QDomElement root = d.documentElement();

    // check cosistency

    if (root.tagName() != "plist")
    {
        qWarning("The file is not a plist!");
        return NULL;
    }

    // get dict element
    QDomElement dict = root.firstChildElement();

    // create model
    DomModel *m = new DomModel();

    // create root item in model
    DomItem *i = m->getRoot()->addChild();
    i->setName("plist");
    i->setType("dict");

    // parse doc recursively
    parseElement(dict, i);

    return m;
}
 QDomDocument DomParser::toDom(DomModel *m)
{
    // create doc with doctype
    QDomDocument doc("plist PUBLIC \"-//Apple//DTD PLIST 1.0//EN\" \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\"");

    // create and add processing instruction
    QDomProcessingInstruction instr = doc.createProcessingInstruction(
                "xml", "version=\"1.0\" encoding=\"UTF-8\"");
    doc.appendChild(instr);

    // create root item in doc
    QDomElement doc_root = doc.createElement("plist");

    QDomAttr version = doc.createAttribute("version");
    version.setValue("1.0");

    doc_root.setAttributeNode(version);

    QDomElement doc_dict = doc.createElement("dict");

    doc_root.appendChild(doc_dict);

    // add root item to doc
    doc.appendChild(doc_root);

    // get root item from model
    DomItem *model_root = m->getRoot()->child(0);

    // parse model recursively
    parseItem(model_root, doc_dict, doc);

    return doc;

}

void DomParser::parseElement(QDomElement &n, DomItem *item)
{
    if (n.hasChildNodes())
    {
        QDomElement element = n.firstChildElement();

        while (!element.isNull())
        {
            QString tagName = element.tagName();

            if (tagName == "key" || tagName == "array" || tagName == "dict")
            {

                // create child
                DomItem *domItem = item->addChild();

                // counter for items without keys
                int nC = 1;

                if (tagName == "key")
                {
                    // get key value
                    QString eName = element.firstChild().nodeValue();

                    // get val element
                    QDomElement val = element.nextSiblingElement();

                    // get type and val
                    QString eType = val.tagName();
                    QString eValue = val.firstChild().nodeValue();

                    // set data
                    domItem->setData(eName, eType, eValue);

                    parseElement(val, domItem);

                    element = val.nextSiblingElement();
                }
                else
                {
                    domItem->setType(tagName);
                    domItem->setName(QString("Item %1").arg(nC++));
                    parseElement(element, domItem);
                    element = element.nextSiblingElement();
                }
            }
            else
            {
                qWarning("No value matching the key");
                break;
            }

        }
    }
}

void DomParser::parseItem(DomItem *item, QDomElement &n, QDomDocument &doc)
{

    QDomElement element;
    QString name = item->getName();
    QString type = item->getType();
    QString value = item->getValue();

    if (name != "plist")
    {
        QRegExp rp("Item\\s\\d+");

        // item without key
        if (rp.exactMatch(name))
        {
            QDomElement container = doc.createElement(type);
            n.appendChild(container);
            element = container;
        }
        else
        {

            QDomElement key = doc.createElement("key");
            QDomText keyText = doc.createTextNode(name);

            key.appendChild(keyText);
            n.appendChild(key);

            QDomElement val = doc.createElement(type);
            n.appendChild(val);

            if (type != "array" && type != "dict")
            {
                QDomText valText = doc.createTextNode(value);
                val.appendChild(valText);
            }

            element = val;
        }

    }
    else element = n;


    // get children count
    int c = item->childCount();

    // iterate through children
    for (int i = 0; i < c; ++i)
    {
        DomItem *child = item->child(i);
        parseItem(child, element, doc);
    }
}
