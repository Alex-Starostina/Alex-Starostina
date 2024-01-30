#ifndef HANDLEMESSERROR_H
#define HANDLEMESSERROR_H
#include <QString>

class handleMessError
{
public:
    handleMessError(QString amessage, QString acontext);
    QString getMessage();
    QString getContext();

private:
    QString message;
    QString context;
};

#endif // HANDLEMESSERROR_H
