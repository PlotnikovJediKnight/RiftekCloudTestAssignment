QT       += opengl
QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets xml openglwidgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    BallPivotingAlgorithm.cpp \
    main.cpp \
    mainwindow.cpp \
    simpleViewer.cpp

HEADERS += \
    BallPivotingAlgorithm.h \
    DataStructures.h \
    mainwindow.h \
    simpleViewer.h

FORMS += \
    mainwindow.ui


INCLUDEPATH *= E:\QtProjects\libQGLViewer-2.8.0\libQGLViewer-2.8.0
LIBS *= -LE:\QtProjects\libQGLViewer-2.8.0\libQGLViewer-2.8.0\QGLViewer -lQGLViewer2
LIBS *= -lopengl32

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
