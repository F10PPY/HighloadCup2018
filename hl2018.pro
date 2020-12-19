TEMPLATE = app
#CONFIG += console c++1z
CONFIG -= app_bundle
CONFIG -= qt

DEFINES += LOCAL
CONFIG(debug, debug|release) {  DEFINES += DEBUG_BUILD }
#else:CONFIG(force_debug_info) { DEFINES += PROFILE_MODE }
#else {                          DEFINES += RELEASE_MODE }
QMAKE_CXXFLAGS += -std=c++2a
QMAKE_CXXFLAGS +=-msse4.2
QMAKE_CXXFLAGS_RELEASE -= -O3 -O2 -O1
QMAKE_CXXFLAGS_RELEASE += -Ofast
QMAKE_CXXFLAGS_DEBUG -= -Ofast -O3 -O2 -O1
#QMAKE_CXXFLAGS_DEBUG += -Ofast

DEPENDPATH *= $${INCLUDEPATH}
INCLUDEPATH+= ./
requires(true) #enable building
LIBS+= -lpthread -lstdc++fs
SOURCES += \
        main.cpp \
    load_data.cpp \
    req_handler.cpp \
    get_filter.cpp \
    resp_templates.cpp \
    util.cpp \
    get_group.cpp \
    get_recommend.cpp \
    get_suggest.cpp \
    tests.cpp \
    index.cpp \
    post_new.cpp \
    post_id.cpp \
    post_likes.cpp \
    worker.cpp \
    tests_bm.cpp

HEADERS += \
ntb_util.h \
    common.h\
    resp_templates.h \
    util.h \
    post.h \
    get_group.h \
    ngvs_map.h \
    index.h \
    chunked_bm_vec.h

