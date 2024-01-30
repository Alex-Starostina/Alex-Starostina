#ifndef REPORTGENERATOR_H
#define REPORTGENERATOR_H

#include <QObject>
#include <QAxObject>

class ReportGenerator : public QObject
{
    Q_OBJECT
public:

    explicit ReportGenerator(QObject *parent = nullptr );

    struct estate_Rec {
        QString adr;
        QString kadNum;
        QString info;
    };

    void reportTableWithPhoto(QMap<QString, estate_Rec>);
    void doReportVneIzhS(QString scrShot, estate_Rec realEstateRec);
    void doReportCadastrTerr(QString scrShot, estate_Rec realEstateRec);


//signals:

private:

    void replaceStrInReport(const QString& oldStr, const QString& newStr, QAxObject *wordApp);
    void doReport(QString fileName,QString scrShot,estate_Rec realEstateRec);


private slots:
    void printError(int errCod, QString src, QString desc, QString helpTxt);
};

#endif // REPORTGENERATOR_H
