#-------------------------------------------------
#
# Project created by QtCreator 2017-12-21T12:58:45
#
#-------------------------------------------------

QT       += core gui network sql widgets multimedia
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = OptPlugServer
TEMPLATE = app
RC_ICONS = opt.ico
RC_FILE = version.rc

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
    logic/Logic.cpp \
        main.cpp \
        mainwindow.cpp \
    tcp_server.cpp \
    tcp_server_private.cpp \
    ClientForm.cpp \
    SettingsDialog.cpp \
    com/clsSettings.cpp \
    com/Functions.cpp \
    GraphicsView.cpp \
    ImgForm.cpp \
    SortingDialog.cpp \
    ActionDialog.cpp \
    model/AiThread.cpp \
    model/SortThread.cpp \
    ItemForm.cpp \
    TabSpinBox.cpp \
    SpinForm.cpp \
    DoubleSpinForm.cpp \
    ComboBox.cpp \
    PointForm.cpp \
    model/CsvStream.cpp \
    model/ExcelSaver.cpp \
    model/Saver.cpp \
    view/ImageViewer.cpp \
    view/StandardDlg.cpp \
    view/AiPortDialog.cpp \
    view/ImageSetDialog.cpp \
    view/DefButton.cpp \
    view/PosButton.cpp \
    view/NgDialog.cpp \
    view/PwdDialog.cpp \
    model/LineEdit.cpp \
    view/Dialog.cpp \
    view/EditDialog.cpp \
    model/HeartTimer.cpp

HEADERS += \
    logic/Logic.h \
        mainwindow.h \
    tcp_server.h \
    tcp_server_private.h \
    ClientForm.h \
    SettingsDialog.h \
    com/clsSettings.h \
    com/Functions.h \
    GraphicsView.h \
    ImgForm.h \
    SortingDialog.h \
    ActionDialog.h \
    model/AiThread.h \
    model/SortThread.h \
    ItemForm.h \
    TabSpinBox.h \
    SpinForm.h \
    DoubleSpinForm.h \
    ComboBox.h \
    PointForm.h \
    model/CsvStream.h \
    model/ExcelSaver.h \
    model/Saver.h \
    view/ImageViewer.h \
    view/StandardDlg.h \
    view/AiPortDialog.h \
    view/ImageSetDialog.h \
    view/DefButton.h \
    view/PosButton.h \
    view/NgDialog.h \
    view/PwdDialog.h \
    model/LineEdit.h \
    view/Dialog.h \
    view/EditDialog.h \
    model/HeartTimer.h

FORMS += \
        mainwindow.ui \
    ClientForm.ui \
    SettingsDialog.ui \
    ImgForm.ui \
    SortingDialog.ui \
    ActionDialog.ui \
    ItemForm.ui \
    TabSpinBox.ui \
    SpinForm.ui \
    DoubleSpinForm.ui \
    ComboBox.ui \
    PointForm.ui \
    view/ImageViewer.ui \
    view/StandardDlg.ui \
    view/AiPortDialog.ui \
    view/ImageSetDialog.ui \
    view/DefButton.ui \
    view/PosButton.ui \
    view/NgDialog.ui \
    view/PwdDialog.ui \
    view/Dialog.ui \
    view/EditDialog.ui

RESOURCES += \
    Res.qrc
