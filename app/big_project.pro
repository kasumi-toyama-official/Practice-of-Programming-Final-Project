QT += widgets

CONFIG += c++17

INCLUDEPATH += ../src

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    achievementpage.cpp \
    battlepage.cpp \
    chapterselectpage.cpp \
    collectionpage.cpp \
    errorbookpage.cpp \
    main.cpp \
    mainmenupage.cpp \
    mainwindow.cpp \
    questionwidget.cpp \
    resultpage.cpp \
    settingspage.cpp \
    skillpanel.cpp \
    statusbar.cpp \
    uimanager.cpp \
    ../src/managers/QuestionBank.cpp \
    ../src/managers/SaveManager.cpp \
    ../src/managers/WrongBookManager.cpp \
    ../src/managers/AchievementManager.cpp \
    ../src/managers/RankingManager.cpp \
    ../src/models/ChapterArchive.cpp \
    ../src/models/Question.cpp \
    ../src/models/CodeCompletionQuestion.cpp \
    ../src/models/WrongBookEntry.cpp \
    ../src/models/Achievement.cpp \
    ../src/models/RankingEntry.cpp \
    ../src/models/GameConfig.cpp \
    ../src/utils/JsonUtils.cpp \
    ../src/utils/CodeJudge.cpp \
    ../src/utils/RandomUtils.cpp

HEADERS += \
    GameData.h \
    achievementpage.h \
    battlepage.h \
    chapterselectpage.h \
    collectionpage.h \
    errorbookpage.h \
    mainmenupage.h \
    mainwindow.h \
    questionwidget.h \
    resultpage.h \
    settingspage.h \
    skillpanel.h \
    statusbar.h \
    uimanager.h

FORMS += \
    achievementpage.ui \
    battlepage.ui \
    chapterselectpage.ui \
    collectionpage.ui \
    errorbookpage.ui \
    mainmenupage.ui \
    mainwindow.ui \
    resultpage.ui \
    settingspage.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc
