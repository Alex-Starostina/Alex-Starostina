#include "handlemesserror.h"

handleMessError::handleMessError(QString amessage, QString acontext)
{
    message=amessage;
    context=acontext;
}


QString handleMessError::getMessage()
{
    return message;
}

QString handleMessError::getContext()
{
    return context;
}

