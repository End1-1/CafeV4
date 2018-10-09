#ifndef ORDERPRINTING_H
#define ORDERPRINTING_H

#include <QXmlDefaultHandler>
#include "printing.h"
#include "order.h"

class OrderPrinting : public QXmlDefaultHandler
{
private:
    friend class Order;
public:
    OrderPrinting(const QByteArray &printTemplate);
};

#endif // ORDERPRINTING_H
