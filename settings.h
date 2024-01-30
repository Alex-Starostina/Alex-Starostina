#ifndef SETTINGS_H
#define SETTINGS_H
#include <QtGui>

const QString suffix_atl="atl";

class Settings
{
public:

    static Settings &Instance()
    {
        static Settings settings;
        return settings;
    }

    qreal getCalcError();
    qreal getWidthLine();
    qreal getMinSq();
    QString getAppName();
    int getPart();

    QString getSquareLineColor();
    QString getErrorLineColor();
    uint getWidthArea();
    uint getHeightArea();
    QString getTmplReportVneIZHS();
    QString getTmplReportDopZahvat();

    QString getPathNeuralNetHouse();
    void setDir(QString fileName);
    QString getFilePhoto();
    QString getDir();
    QString  getFileBuildings();
    QString  getFileBorders();
    QString getFileCMap();
    QString getDirXml();
    bool dirValid();
    int getIdPhoto();
    QString getPathDB();


private:
    Settings();    
    QString appName;
    QString WidthBorder;
    QString calcError;
    QString minSq;
    QString part;
    QString colorSq;
    QString colorError;
    QString tmplReportVneIZHSPath;
    QString tmpltDopZahvatPath;
    QString pathDB;

    QString createTmpIniFile();
    QString pathNeuralNetHouse;
    QString dir;
    bool isDirValid=false;
    int idPhoto;
    uint widthArea;
    uint heightArea;
    bool createConnection();

};
#endif // SETTINGS_H
