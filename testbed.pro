QT += core \
    gui \
    opengl
TARGET = final
TEMPLATE = app
INCLUDEPATH += main \
    lib \
    math \
    geom \
    support
DEPENDPATH += main \
    lib \
    math \
    geom \
    support
HEADERS += main/glwidget.h \
    support/mainwindow.h \
    main/glwidget.h \
    math/CS123Matrix.h \
    math/CS123Algebra.h \
    math/CS123Vector.h \
    lib/utils.h \
    lib/CS123Common.h \
    support/OrbitingCamera.h \
    support/camera.h \
    lib/aabb.h \
    support/mouseinput.h \
    geom/polygon.h \
    geom/edge.h \
    geom/vertex.h \
    geom/mesh.h \
    geom/material.h \
    math/ray.h \
    geom/triangle.h \
    lib/aabb2D.h \
    tools/manipulator.h
SOURCES += main/glwidget.cpp \
    support/mainwindow.cpp \
    support/main.cpp \
    math/CS123Matrix.cpp \
    math/CS123Matrix.inl \
    math/CS123Vector.inl \
    lib/utils.cpp \
    support/OrbitingCamera.cpp \
    support/mouseinput.cpp \
    geom/polygon.cpp \
    geom/edge.cpp \
    geom/vertex.cpp \
    geom/mesh.cpp \
    geom/material.cpp \
    math/ray.cpp \
    geom/triangle.cpp \
    main/geomInteraction.cpp \
    tools/manipulator.cpp
FORMS += mainwindow.ui \
    support/mainwindow.ui

RESOURCES +=

