#ifndef BATTLEPAGE_H
#define BATTLEPAGE_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QStackedLayout>
#include <QKeyEvent>
#include <QTimer>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QGraphicsOpacityEffect>
#include <QSet>
#include <QList>
#include <QPixmap>

#include "GameData.h"
#include "questionwidget.h"
#include "skillpanel.h"
#include "statusbar.h"
#include "managers/QuestionBank.h"
#include "managers/SaveManager.h"
#include "managers/WrongBookManager.h"
#include "managers/AchievementManager.h"
#include "managers/RankingManager.h"

class BattlePage : public QWidget
{
    Q_OBJECT

public:
    explicit BattlePage(QWidget *parent = nullptr);
    ~BattlePage(){}

    void setArenaMode(bool isArena);
    void resetBattle();
    void setChapterId(int chapterId) { m_chapterId = chapterId; }
    void setLoadExisting(bool load) { m_loadExisting = load; }
    void loadFromArchive();
    void saveToArchive();

signals:
    void gameOver(bool victory, int chapterId, int totalDamage, int rounds, int correctCount, bool trophyEarned);
    void quitBattle();
    void arenaQuit();
    void arenaGameFinished(int totalDamage);

protected:
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void onAnswerSubmitted();
    void onExtraRoundTrigger();
    void onSkillSelected(int skillId, Difficulty diff);

    void onPauseMenu();
    void onSaveAndQuit();
    void onQuitWithoutSave();
    void onRestart();

    void onBattleFinished(bool victory);

    void onArenaTimerTick();
    void onPassClicked();
    void startArenaRound();
    void onArenaAnswerSubmitted();
    void onArenaBattleFinished();

private:
    void setupUI();
    void showNextQuestion();
    QuestionData questionToQuestionData(const Question& q);
    void applyDamage(bool playerAttacks, int playerDmg, int enemyDmg);
    void showFloatingText(const QString &text, QPoint pos, QColor color);

    Stats m_playerStats;
    Stats m_enemyStats;
    int m_currentRound;
    int m_roundsPerExtra;
    int m_roundsSinceExtra;
    int m_correctCount;

    int m_chapterId;
    bool m_loadExisting;
    QuestionBank m_questionBank;
    SaveManager m_saveManager;
    WrongBookManager m_wrongBookManager;
    AchievementManager m_achievementManager;
    RankingManager m_rankingManager;
    GameConfig m_gameConfig;
    QSet<int> m_usedQuestionIds;
    QuestionData m_currentQuestionData;   // 当前正在显示的题目，用于判题

    bool m_isExtraRound;
    QString m_currentSkillAttr;
    int m_currentSkillBonus;
    int m_pendingBuffId;

    QLabel *m_backgroundLabel;
    QLabel *m_enemySprite;
    QLabel *m_playerSprite;

    StatusBar *m_statusBar;
    QuestionWidget *m_questionWidget;
    SkillPanel *m_skillPanel;

    QPushButton *m_confirmButton;
    QWidget *m_pauseOverlay;
    QLabel *m_extraRoundLabel;
    QLabel *m_extraRoundTitle;
    QWidget *m_dimOverlay;
    bool m_paused;

    bool m_isArenaMode;

    QProgressBar *m_countdownBar;
    QTimer *m_arenaTimer;
    int m_attackInterval;
    int m_elapsed;
    QPushButton *m_passBtn;
    int m_passCardCount;

    enum ArenaState{Idle, WaitingSkill, WaitingAnswer};
    ArenaState m_arenaState;
    QList<SkillData> m_currentRoundSkills;

    QPushButton *m_leaderboardBtn;

    QList<QPixmap> m_playerIdleFrames;
    QList<QPixmap> m_playerAttackFrames;
    QList<QPixmap> m_playerHurtFrames;
    QList<QPixmap> m_playerDeathFrames;
    QList<QPixmap> m_enemyIdleFrames;
    QList<QPixmap> m_enemyAttackFrames;
    QList<QPixmap> m_enemyHurtFrames;
    QList<QPixmap> m_enemyDeathFrames;

    QTimer* m_animTimer;

    enum AnimState
    {
        Anim_Idle = 0,
        Anim_Attack,
        Anim_Hurt,
        Anim_Death
    };
    AnimState m_playerState;
    AnimState m_enemyState;
    int m_playerFrameIndex;
    int m_enemyFrameIndex;

    void loadFrames(const QString& prefix, int count, QList<QPixmap>& outFrames, bool flipHorizontal = false);
    void playPlayerAnim(AnimState state);
    void playEnemyAnim(AnimState state);
    void onAnimTimerTick();
};

#endif // BATTLEPAGE_H
