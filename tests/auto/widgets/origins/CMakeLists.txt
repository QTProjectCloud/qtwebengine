include(../../httpserver/httpserver.cmake)
include(../../util/util.cmake)

qt_internal_add_test(tst_origins
    SOURCES
        tst_origins.cpp
    LIBRARIES
        Qt::WebEngineWidgets
        Test::HttpServer
        Test::Util
)

set(tst_origins_resource_files
    "resources/createObjectURL.html"
    "resources/dedicatedWorker.html"
    "resources/dedicatedWorker.js"
    "resources/mixedSchemes.html"
    "resources/mixedSchemesWithCsp.html"
    "resources/mixedSchemes_frame.html"
    "resources/mixedXHR.html"
    "resources/mixedXHR.txt"
    "resources/serviceWorker.html"
    "resources/serviceWorker.js"
    "resources/sharedWorker.html"
    "resources/sharedWorker.js"
    "resources/subdir/frame2.html"
    "resources/subdir/index.html"
    "resources/subdir_frame1.html"
    "resources/viewSource.html"
    "resources/websocket.html"
)

qt_add_resource(tst_origins "tst_origins"
    PREFIX
        "/"
    FILES
        ${tst_origins_resource_files}
)

qt_extend_target(tst_origins CONDITION QT_FEATURE_webengine_webchannel AND TARGET Qt::WebSockets
    DEFINES
        WEBSOCKETS
    LIBRARIES
        Qt::WebSockets
)
