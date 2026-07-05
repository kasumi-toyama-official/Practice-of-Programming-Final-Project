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
#include <QMap>

#include "GameData.h"
#include "questionwidget.h"
#include "skillpanel.h"
#include "statusbar.h"
#include "managers/QuestionBank.h"
#include "managers/SaveManager.h"
#include "managers/WrongBookManager.h"
#include "managers/AchievementManager.h"
#include "managers/RankingManager.h"
#include "models/CodeCompletionQuestion.h"

class BattlePage : public QWidget
{
    Q_OBJECT

public:
    explicit BattlePage(QWidget *parent = nullptr);
    ~BattlePage(){}

    void setArenaMode(bool isArena);
    void resetBattle();
    void setChapterId(int chapterId) { m_chapterId = chapterId; }
    void setGameConfig(const GameConfig& config) { m_gameConfig = config; }
    void setLoadExisting(bool load) { m_loadExisting = load; }
    void loadFromArchive();
    void saveToArchive();

signals:
    void gameOver(bool victory, int chapterId, int totalDamage, int rounds, int correctCount, bool trophyEarned);
    void quitBattle();
    void arenaQuit();
    void arenaGameFinished(int totalDamage);
    void arenaGoToRanking();

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
    QMap<int, QSet<int>> m_usedQuestionIds;
    QMap<int, CodeCompletionQuestion> m_completionQuestions;
    QuestionData m_currentQuestionData;
    CodeCompletionQuestion m_currentCompletionData;

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

    qint64 m_arenaElapsedMs;
    int m_arenaConsecutiveErrors;
    int m_arenaLastQuestionId;
    int m_codeCompletionRetries;

    QPushButton *m_leaderboardBtn;

    QList<QPixmap> m_enemyIdleFrames, m_enemyAttackFrames, m_enemyHurtFrames, m_enemyDeathFrames;

    QPixmap m_playerIdlePix, m_playerAttackPix, m_playerHurtPix, m_playerDeathPix;

    QTimer* m_animTimer;
    enum AnimState { Anim_Idle = 0, Anim_Attack, Anim_Hurt, Anim_Death };
    AnimState m_playerState, m_enemyState;
    int m_playerFrameIndex, m_enemyFrameIndex;

    void loadFrames(const QString& prefix, int count, QList<QPixmap>& outFrames, bool flipHorizontal = false);
    void playPlayerAnim(AnimState state);
    void playEnemyAnim(AnimState state);
    void onAnimTimerTick();
};

#endif // BATTLEPAGE_H
