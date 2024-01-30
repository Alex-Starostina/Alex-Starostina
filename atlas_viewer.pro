QT       += core gui sql
QT       += xml

QT += printsupport
QT += axcontainer

QT += uitools

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11


include(common.pri)

DEFINES += QT_DEPRECATED_WARNINGS


SOURCES += \
    atlaspainter.cpp \
    borderobj.cpp \
    controlpolygon.cpp \
    dbworker.cpp \
    domparser.cpp \
    formmark.cpp \
    formobjectviolation.cpp \
    formviolation.cpp \
    handlemesserror.cpp \
    listpolygons.cpp \
    main.cpp \
    mainwindow.cpp \
    mapimagescene.cpp \
    maskitem.cpp \
    parserfiletab.cpp \
    pixelpolygon.cpp \
    readerparcelsjson.cpp \
    reportgenerator.cpp \
    settings.cpp \
    splashscreen.cpp \
    tusermask.cpp \
    violationsqlmodel.cpp

HEADERS += \
    atlaspainter.h \
    borderobj.h \
    controlpolygon.h \
    dbworker.h \
    domparser.h \
    formmark.h \
    formobjectviolation.h \
    formviolation.h \
    handlemesserror.h \
    listpolygons.h \
    mainwindow.h \
    mapimagescene.h \
    maskitem.h \
    parserfiletab.h \
    pixelpolygon.h \
    readerparcelsjson.h \
    reportgenerator.h \
    settings.h \
    splashscreen.h \
    tusermask.h \
    violationsqlmodel.h

FORMS += \
    formmark.ui \
    formobjectviolation.ui \
    formviolation.ui \
    mainwindow.ui \
    splashscreen.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DESTDIR= bin

win32:CONFIG(release, debug|release): LIBS += -L'C:/Program Files (x86)/GnuWin32/lib/'

else:win32:CONFIG(debug, debug|release): LIBS += -L'C:/Program Files (x86)/GnuWin32/lib/'


INCLUDEPATH += 'C:/Program Files (x86)/GnuWin32/include'
DEPENDPATH += 'C:/Program Files (x86)/GnuWin32/include'


RESOURCES += \
    controlland.qrc
#RESOURCES += '254-127-Б/p.jpg'

RC_FILE = icontrolland.rc

