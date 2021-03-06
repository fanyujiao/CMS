TEMPLATE = app
TARGET = pc-manager
QT += core gui phonon declarative dbus network xml

inst1.files += image/pc-manager.png
inst1.path = /usr/share/pixmaps
inst2.files += ../pc-manager.desktop
inst2.path = /usr/share/applications
target.source += $$TARGET
target.path = /usr/bin
INSTALLS += inst1 \
    inst2 \
    target

include(../qtsingleapplication/src/qtsingleapplication.pri)
LIBS += -lfcitx-qt -lfcitx-config -lfcitx-utils

# Additional import path used to resolve QML modules in Creator's code model
#QML_IMPORT_PATH =

# The .cpp file which was generated for your project. Feel free to hack it.
HEADERS += pc-application.h \
    quibo.h \
    logindialog.h \
    accountcache.h \
    httpauth.h \
    systemdispatcher.h \
    sessiondispatcher.h \
    modaldialog.h \
    warningdialog.h \
    messagedialog.h \
    qmlaudio.h \
    qrangemodel.h \
    qrangemodel_p.h \
    qstyleitem.h \
    qwheelarea.h \
    qtmenu.h \
    qtmenuitem.h \
    qtoplevelwindow.h \
    qcursorarea.h \
    tray.h \
    fcitxcfgwizard.h \
    qtkeytrans.h \
    qtkeytransdata.h \
    fcitxwarndialog.h \
    kthread.h \
    suspensionframe.h \
    alertdialog.h \
    toolkits.h \
#    skinswidget.h \
#    skingrid.h \
    locationdialog.h \
    wizarddialog.h \
    changecitydialog.h \
    util.h \
    processmanager.h \
    yprocess.h \
    devicemanager.h \
    kfontdialog.h \
    aboutdialog.h \
    apttransaction.h \
    common.h \
    singals.h

SOURCES += main.cpp \
    pc-application.cpp \
    logindialog.cpp \
    accountcache.cpp \
    httpauth.cpp \
    quibo.cpp \
    systemdispatcher.cpp \
    sessiondispatcher.cpp \
    modaldialog.cpp \
    warningdialog.cpp \
    messagedialog.cpp \
    qmlaudio.cpp \
    qrangemodel.cpp \
    qstyleitem.cpp \
    qwheelarea.cpp \
    qtmenu.cpp \
    qtmenuitem.cpp \
    qtoplevelwindow.cpp \
    qcursorarea.cpp \
    tray.cpp \
    fcitxcfgwizard.cpp \
    qtkeytrans.cpp \
    fcitxwarndialog.cpp \
    kthread.cpp \
    suspensionframe.cpp \
    alertdialog.cpp \
    toolkits.cpp \
#    skinswidget.cpp \
#    skingrid.cpp \
    locationdialog.cpp \
    wizarddialog.cpp \
    changecitydialog.cpp \
    util.cpp \
    processmanager.cpp \
    yprocess.cpp \
    devicemanager.cpp \
    kfontdialog.cpp \
    aboutdialog.cpp \
    apttransaction.cpp \
    singals.cpp

RESOURCES += pixmap.qrc

FORMS += \
    quibo.ui \
    logindialog.ui \
    modaldialog.ui \
    warningdialog.ui \
    messagedialog.ui \
    tray.ui \
    fcitxwarndialog.ui \
    suspensionframe.ui \
    alertdialog.ui \
    locationdialog.ui \
    wizarddialog.ui \
    changecitydialog.ui \
    kfontdialog.ui \
    aboutdialog.ui

OTHER_FILES += \
    ../dpkgwrapper.sh
