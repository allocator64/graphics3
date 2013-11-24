QT       += core gui widgets

CXX_FLAGS += -std=c++11

TARGET = cube
TEMPLATE = app

SOURCES += main.cpp \
    engine.cpp \
#    cubeengine.cpp \
#    sphereengine.cpp \


qtHaveModule(opengl) {
    QT += opengl

    SOURCES += mainwidget.cpp

    HEADERS += \
        mainwidget.h

    RESOURCES += \
        shaders.qrc \
        textures.qrc
}

# install
target.path = $$[QT_INSTALL_EXAMPLES]/opengl/cube
INSTALLS += target

simulator: warning(This example might not fully work on Simulator platform)

HEADERS += \
    engine.h \
#    cubeengine.h \
#    sphereengine.h \
