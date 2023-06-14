QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17 qwt

linux {
    # for Intel VTune Profiler
    QMAKE_CXXFLAGS += -g3 -gdwarf-2
    QMAKE_CXXFLAGS_RELEASE += -g
    QMAKE_CFLAGS_RELEASE += -g

#    CONFIG += sanitizer sanitize_address
#    LIBS += -lasan
}

QMAKE_CXXFLAGS += -Wall -Wextra -Wpedantic

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    Configs.cpp \
    components/FrequencyScaleDraw.cpp \
    listener.cpp \
    main.cpp \
    models/CommutateTableModel.cpp \
    models/ScannerBaseTableModel.cpp \
    sendertoinst.cpp \
    views/uiAccessControl.cpp \
    views/uiAdCommutateConfig.cpp \
    views/uiAdFreqConfig.cpp \
    views/uiBasesViewSearch.cpp \
    views/uiClockDetuneView.cpp \
    views/uiCorrelationView.cpp \
    views/uiDucConfig.cpp \
    views/uiDumpWritter.cpp \
    views/uiFlashFirmware.cpp \
    views/uiInformation.cpp \
    views/uiMainView.cpp \
    views/uiMobileConfig.cpp \
    views/uiSMSViewer.cpp \
    views/uiSpectrumView.cpp \
    views/uiStatusConnect.cpp \
    views/uiStatusView.cpp

HEADERS += \
    Configs.h \
    components/FrequencyScaleDraw.h \
    listener.h \
    models/CommutateTableModel.h \
    models/ScannerBaseTableModel.h \
    sendertoinst.h \
    views/uiAccessControl.h \
    views/uiAdCommutateConfig.h \
    views/uiAdFreqConfig.h \
    views/uiBasesViewSearch.h \
    views/uiClockDetuneView.h \
    views/uiCorrelationView.h \
    views/uiDucConfig.h \
    views/uiDumpWritter.h \
    views/uiFlashFirmware.h \
    views/uiInformation.h \
    views/uiMainView.h \
    views/uiMobileConfig.h \
    views/uiSMSViewer.h \
    views/uiSpectrumView.h \
    views/uiStatusConnect.h \
    views/uiStatusView.h

FORMS += \
    views/uiAccessControl.ui \
    views/uiAdCommutateConfig.ui \
    views/uiAdFreqConfig.ui \
    views/uiBasesViewSearch.ui \
    views/uiClockDetuneView.ui \
    views/uiCorrelationView.ui \
    views/uiDucConfig.ui \
    views/uiDumpWritter.ui \
    views/uiFlashFirmware.ui \
    views/uiInformation.ui \
    views/uiMainView.ui \
    views/uiMobileConfig.ui \
    views/uiSMSViewer.ui \
    views/uiSpectrumView.ui \
    views/uiStatusConnect.ui \
    views/uiStatusView.ui

#QWT
win32:INCLUDEPATH += $$(QWT_ROOT)/src

win32:CONFIG(release, debug|release): LIBS += -L$$(QWT_ROOT)/lib/ -lqwt
win32:CONFIG(debug,   debug|release): LIBS += -L$$(QWT_ROOT)/lib/ -lqwtd

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

#LIBVMS
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../IVMSlib/release/ -lIVMSlib
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../IVMSlib/debug/ -lIVMSlib
else:unix: LIBS += -L$$OUT_PWD/../IVMSlib/ -lIVMSlib

INCLUDEPATH += $$PWD/../IVMSlib
DEPENDPATH += $$PWD/../IVMSlib

#USBLIB
win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../IVMSlib/3rdparty/libusb-win32/MinGW64/dll/ -llibusb-1.0.dll
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../IVMSlib/3rdparty/libusb-win32/MinGW64/dll/ -llibusb-1.0.dll
else:unix: LIBS += -lusb-1.0

win32:INCLUDEPATH += $$PWD/../IVMSlib/3rdparty/libusb-win32/include/libusb-1.0
win32:DEPENDPATH += $$PWD/../IVMSlib/3rdparty/libusb-win32/include/libusb-1.0
