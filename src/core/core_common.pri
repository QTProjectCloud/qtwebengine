include($$QTWEBENGINE_OUT_ROOT/src/buildtools/qtbuildtools-config.pri)
include($$QTWEBENGINE_OUT_ROOT/src/core/qtwebenginecore-config.pri)
QT_FOR_CONFIG += buildtools-private webenginecore webenginecore-private

# NOTE: The TARGET, QT, QT_PRIVATE variables are used in both core_module.pro and core_gyp_generator.pro
# gyp/ninja will take care of the compilation, qmake/make will finish with linking and install.

TARGET = QtWebEngineCore
QT += qml-private quick-private gui-private core-private
qtConfig(webengine-printing-and-pdf): QT += printsupport
QT_PRIVATE += webenginecoreheaders-private

qtConfig(webengine-geolocation): QT += positioning
qtConfig(webengine-webchannel): QT += webchannel

# LTO does not work for Chromium at the moment, so disable it completely for core.
CONFIG -= ltcg

# QtBase requires C++17
CONFIG += c++17

