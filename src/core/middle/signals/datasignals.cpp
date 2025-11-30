#include "datasignals.h"

Q_GLOBAL_STATIC(DataSignals,ins)

DataSignals::DataSignals(QObject *parent)
    : QObject{parent}
{}

DataSignals *DataSignals::instance()
{
    return ins();
}
