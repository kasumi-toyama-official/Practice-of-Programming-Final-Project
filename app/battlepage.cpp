#include "battlepage.h"
#include <climits>
#include <QJsonDocument>
#include <QJsonArray>
#include <QMessageBox>
#include <QRandomGenerator>
#include <QFileInfo>
#include <QDir>
#include <QCoreApplication>
#include <QPixmap>
#include <QDateTime>
#include "utils/CodeJudge.h"
#include "chapterselectpage.h"
#include <algorithm>

static const int ARENA_TOLERANCE = 3;

static QString findDataFile(const QString& relativePath)
{
    // 从 exe 所在目录开始，逐层向上查找 data/ 目录
    QDir dir(QCoreApplication::applicationDirPath());
    for (int i = 0; i < 10; ++i) {
        QString candidate = dir.filePath(relativePath);
        if (QFileInfo::exists(candidate))
            return QFileInfo(candidate).absoluteFilePath();
        if (!dir.cdUp())
            break;
    }
    // 最后尝试当前工作目录
    if (QFileInfo::exists(relativePath))
        return QFileInfo(relativePath).absoluteFilePath();
    return QString();
}

BattlePage::BattlePage(QWidget *parent)
    :QWidget(parent),m_currentRound(1), m_roundsPerExtra(3), m_roundsSinceExtra(0), m_correctCount(0), m_chapterId(1), m_loadExisting(false), m_isExtraRound(false), m_paused(false), m_isArenaMode(false),
    m_arenaElapsedMs(0), m_arenaConsecutiveErrors(0), m_arenaLastQuestionId(-1)
{
    setupUI();

    m_gameConfig = GameConfig::getDefault();
    m_wrongBookManager.setFilePath(QCoreApplication::applicationDirPath() + "/data/progress/wrongbook.json");
    m_wrongBookManager.load();
    QString achPath = findDataFile("data/achievements.json");
    m_achievementManager.loadDefinitions(achPath.isEmpty() ? QCoreApplication::applicationDirPath() + "/data/achievements.json" : achPath);
    m_achievementManager.loadProgress(QCoreApplication::applicationDirPath() + "/data/progress/achievements.json");
    m_rankingManager.setFilePath(QCoreApplication::applicationDirPath() + "/data/progress/ranking.json");
    m_rankingManager.load();

    m_playerStats.hp = 100;
    m_playerStats.maxHp = 100;
    m_playerStats.attack = 25;
    m_playerStats.defence = 8;
    m_playerStats.lifesteal = 0.1;
    m_playerStats.passCards = 0;
    m_playerStats.totalDamage = 0;
    m_playerStats.round = m_currentRound;
    m_playerStats.buffs = QStringList();

    m_enemyStats.hp = 150;
    m_enemyStats.maxHp = 150;
    m_enemyStats.attack = 15;
    m_enemyStats.defence = 5;
    m_enemyStats.lifesteal = 0.0;
    m_enemyStats.passCards = 0;
    m_enemyStats.totalDamage = 0;
    m_enemyStats.round = 0;
    m_enemyStats.buffs = QStringList();

    m_statusBar->updatePlayer(m_playerStats);
    m_statusBar->updateEnemy(m_enemyStats);

    m_isArenaMode = false;
    m_attackInterval = 120000;
    m_elapsed = 0;
    m_passCardCount = 3;
    m_arenaState = Idle;
    m_countdownBar = nullptr;
    m_passBtn = nullptr;
    m_leaderboardBtn = nullptr;
    m_arenaTimer = new QTimer(this);
    connect(m_arenaTimer, &QTimer::timeout, this, &BattlePage::onArenaTimerTick);

    loadFrames(":/charactors/enemy/PNG/Golem_03/PNG Sequences/Idle/Golem_03_Idle_", 12, m_enemyIdleFrames, true);
    loadFrames(":/charactors/enemy/PNG/Golem_03/PNG Sequences/Attacking/Golem_03_Attacking_", 12, m_enemyAttackFrames, true);
    loadFrames(":/charactors/enemy/PNG/Golem_03/PNG Sequences/Hurt/Golem_03_Hurt_", 12, m_enemyHurtFrames, true);
    loadFrames(":/charactors/enemy/PNG/Golem_03/PNG Sequences/Dying/Golem_03_Dying_", 15, m_enemyDeathFrames, true);
    if (!m_enemyIdleFrames.isEmpty())
        m_enemySprite->setPixmap(m_enemyIdleFrames.first());

    QPixmap sheet(":/charactors/players/PNG/Swordsman_lvl3/With_shadow/Swordsman_lvl3_Idle_with_shadow.png");
    if (!sheet.isNull()) m_playerIdlePix = sheet.copy(QRect(19, 20, 20, 27));
    sheet = QPixmap(":/charactors/players/PNG/Swordsman_lvl3/With_shadow/Swordsman_lvl3_attack_with_shadow.png");
    if (!sheet.isNull()) m_playerAttackPix = sheet.copy(QRect(19, 20, 20, 27));
    sheet = QPixmap(":/charactors/players/PNG/Swordsman_lvl3/With_shadow/Swordsman_lvl3_Hurt_with_shadow.png");
    if (!sheet.isNull()) m_playerHurtPix = sheet.copy(QRect(19, 20, 20, 27));
    sheet = QPixmap(":/charactors/players/PNG/Swordsman_lvl3/With_shadow/Swordsman_lvl3_Death_with_shadow.png");
    if (!sheet.isNull()) m_playerDeathPix = sheet.copy(QRect(19, 20, 20, 27));
    if (!m_playerIdlePix.isNull())
        m_playerSprite->setPixmap(m_playerIdlePix);

    m_animTimer = new QTimer(this);
    connect(m_animTimer, &QTimer::timeout, this, &BattlePage::onAnimTimerTick);
    m_animTimer->start(100);
    m_playerState = Anim_Idle;
    m_enemyState = Anim_Idle;
    m_playerFrameIndex = 0;
    m_enemyFrameIndex = 0;
}

void BattlePage::setArenaMode(bool isArena)
{
    m_isArenaMode = isArena;
    if (isArena)
    {
        m_chapterId = -1;
        m_statusBar->showRoundLabel(false);
        if (!m_countdownBar)
        {
            m_countdownBar = new QProgressBar(this);
            m_countdownBar->setGeometry(10, 143, 940, 12);
            m_countdownBar->setTextVisible(false);
            m_countdownBar->setStyleSheet("QProgressBar { border: 1px solid #aaa; background: #222;"
                                          " min-height: 12px; margin: 0px; padding: 0px; }"
                                          "QProgressBar::chunk { background: #e55; margin: 0px; }");
        }
        m_countdownBar->setRange(0, m_attackInterval);
        m_countdownBar->setValue(0);
        m_countdownBar->show();
        if (!m_passBtn)
        {
            m_passBtn = new QPushButton(this);
            m_passBtn->setGeometry(415, 97, 130, 28);
            m_passBtn->setStyleSheet(
                "QPushButton { color: white; background-color: #d9534f; border: 2px solid #c9302c;"
                " border-radius: 6px; font-size: 16px; font-weight: bold; }"
                "QPushButton:hover { background-color: #c9302c; }"
                "QPushButton:disabled { background-color: #555; border-color: #444; color: #999; }");
            connect(m_passBtn, &QPushButton::clicked, this, &BattlePage::onPassClicked);
        }
        m_passBtn->raise();
        m_passBtn->setText(QString("Pass (%1)").arg(m_passCardCount));
        m_passBtn->show();
        m_passBtn->setEnabled(m_passCardCount > 0);

        if (!m_leaderboardBtn) {
            m_leaderboardBtn = new QPushButton("退出", this);
            m_leaderboardBtn->setGeometry(555, 97, 80, 28);
            m_leaderboardBtn->setStyleSheet(
                "QPushButton { color: black; background-color: #f0c040; border: 2px solid #c09020;"
                " border-radius: 6px; font-size: 16px; font-weight: bold; }"
                "QPushButton:hover { background-color: #e0b030; }");
            connect(m_leaderboardBtn, &QPushButton::clicked, this, [this]() {
                QMessageBox msgBox;
                msgBox.setWindowTitle("退出确认");
                msgBox.setText("确定退出竞技模式吗？\n退出后将计算总伤害。\n注意：游戏计时不会暂停！");
                msgBox.setStyleSheet("QMessageBox { background-color: #f0f0f0; } QMessageBox QLabel { color: black; }");
                QPushButton *btnYes = msgBox.addButton("确定退出", QMessageBox::YesRole);
                msgBox.addButton("取消", QMessageBox::NoRole);
                msgBox.exec();
                if (msgBox.clickedButton() == btnYes) {
                    onArenaBattleFinished();
                }
            });
        }
        m_leaderboardBtn->raise();
        m_leaderboardBtn->show();
        m_statusBar->setArenaMode(true);
        m_enemyStats.hp = INT_MAX;
        m_enemyStats.maxHp = INT_MAX;
        m_enemyStats.attack = 15;
        m_enemyStats.defence = 5;
        // 玩家竞技模式初始属性
        m_playerStats.hp = 100;
        m_playerStats.maxHp = 100;
        m_playerStats.attack = 25;
        m_playerStats.defence = 8;
        m_playerStats.lifesteal = 0.0;
        m_playerStats.passCards = m_passCardCount;
        m_playerStats.totalDamage = 0;
        m_statusBar->updatePlayer(m_playerStats);
        m_statusBar->updateEnemy(m_enemyStats);
        m_arenaTimer->start(50);
        m_elapsed = 0;
        m_arenaElapsedMs = 0;
        m_arenaConsecutiveErrors = 0;
        m_arenaLastQuestionId = -1;
        m_arenaState = Idle;
        m_pauseOverlay->hide();
        m_completionQuestions.clear();
        for (int ch = 1; ch <= 8; ++ch) {
            QString path = findDataFile(QString("data/questions/chapter%1_completion.json").arg(ch));
            if (!path.isEmpty()) {
                QFile f(path);
                if (f.open(QIODevice::ReadOnly)) {
                    QJsonObject root = QJsonDocument::fromJson(f.readAll()).object();
                    QJsonArray arr = root["questions"].toArray();
                    for (const auto& v : arr)
                        m_completionQuestions[CodeCompletionQuestion::fromJson(v.toObject()).id] = CodeCompletionQuestion::fromJson(v.toObject());
                }
            }
        }
    }
    else
    {
        if (m_countdownBar) m_countdownBar->hide();
        if (m_passBtn) m_passBtn->hide();
        if (m_leaderboardBtn) m_leaderboardBtn->hide();
        m_arenaTimer->stop();
        m_statusBar->setArenaMode(false);
        m_statusBar->showRoundLabel(true);
    }
}

static int getArenaSkillBonus(const QString& attr, Difficulty diff)
{
    if (attr == "攻击力") {
        if (diff == Difficulty::Easy) return 3;
        if (diff == Difficulty::Medium) return 5;
        return 8;
    }
    if (attr == "防御力") {
        if (diff == Difficulty::Easy) return 2;
        if (diff == Difficulty::Medium) return 4;
        return 6;
    }
    if (attr == "生命上限") {
        if (diff == Difficulty::Easy) return 5;
        if (diff == Difficulty::Medium) return 10;
        return 15;
    }
    if (attr == "吸血比例") {
        if (diff == Difficulty::Easy) return 2;
        if (diff == Difficulty::Medium) return 4;
        return 6;
    }
    if (attr == "Pass卡") {
        if (diff == Difficulty::Easy) return 1;
        if (diff == Difficulty::Medium) return 1;
        return 2;
    }
    if (attr == "回复生命") {
        if (diff == Difficulty::Easy) return 10;
        if (diff == Difficulty::Medium) return 20;
        return 30;
    }
    return 0;
}

void BattlePage::startArenaRound()
{
    m_arenaState = WaitingSkill;
    m_arenaConsecutiveErrors = 0;
    m_passBtn->setEnabled(false);
    QList<SkillData> skills;

    QStringList attrs = {"攻击力", "防御力", "生命上限", "吸血比例", "Pass卡", "回复生命"};
    QSet<int> picked;
    while (picked.size() < 3) {
        int idx = QRandomGenerator::global()->bounded(attrs.size());
        if (picked.contains(idx)) continue;
        picked.insert(idx);

        SkillData s;
        s.id = 200 + idx;
        s.attribute = attrs[idx];
        s.name = s.attribute;
        s.iconPath = "";
        s.difficulty = static_cast<Difficulty>(QRandomGenerator::global()->bounded(3));

        s.easyBonus = getArenaSkillBonus(s.attribute, Difficulty::Easy);
        s.mediumBonus = getArenaSkillBonus(s.attribute, Difficulty::Medium);
        s.hardBonus = getArenaSkillBonus(s.attribute, Difficulty::Hard);

        skills.append(s);
    }

    m_currentRoundSkills = skills;
    m_skillPanel->setSkills(skills, false);
    m_skillPanel->showWithAnimation();
    m_questionWidget->hide();
}

void BattlePage::setupUI()
{
    setFixedSize(960, 720);
    setStyleSheet(
        "color: white;"
        "QMessageBox { color: black; background-color: #f0f0f0; }"
        "QMessageBox QLabel { color: black; }"
        "QMessageBox QPushButton { color: black; background-color: #e0e0e0; }");

    m_backgroundLabel = new QLabel(this);
    m_backgroundLabel->setGeometry(0, 0, 960, 720);
    m_backgroundLabel->setStyleSheet("background-color: #1e1e3a;");
    //加背景图

    // 敌人立绘 — 像素化处理
    m_enemySprite = new QLabel(this);
    m_enemySprite->setGeometry(730, 150, 160, 160);
    m_enemySprite->setAlignment(Qt::AlignCenter);
    {
        QPixmap full(":/charactors/enemy/PNG/Golem_03/PNG Sequences/Attacking/Golem_03_Attacking_000.png");
        if (!full.isNull()) {
            QPixmap crop = full.copy(QRect(218, 103, 425, 347));
            // 先缩到像素级小尺寸（约 5×4 像素格），再用临近插值放大回 160px
            QPixmap small = crop.scaled(32, 26, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
            QPixmap pixelated = small.scaled(160, 160, Qt::IgnoreAspectRatio, Qt::FastTransformation);
            m_enemySprite->setPixmap(pixelated);
        }
    }

    // 玩家立绘
    m_playerSprite = new QLabel(this);
    m_playerSprite->setGeometry(70, 150, 160, 160);
    m_playerSprite->setAlignment(Qt::AlignCenter);
    m_playerSprite->setScaledContents(true);
    {
        QPixmap sheet(":/charactors/players/PNG/Swordsman_lvl3/With_shadow/Swordsman_lvl3_Idle_with_shadow.png");
        if (!sheet.isNull()) {
            m_playerSprite->setPixmap(sheet.copy(QRect(19, 20, 20, 27)));
        }
    }

    m_statusBar = new StatusBar(this);
    m_statusBar->setGeometry(10, 5, 940, 135);

    m_questionWidget = new QuestionWidget(this);
    m_questionWidget->setGeometry(30, 320, 900, 380);

    m_extraRoundLabel = new QLabel("额外回合", this);
    m_extraRoundLabel->setGeometry(35, 325, 80, 16);
    m_extraRoundLabel->setStyleSheet("color: #ffcc00; font-weight: bold; font-size: 12px; background: transparent;");
    m_extraRoundLabel->hide();

    m_dimOverlay = new QWidget(this);
    m_dimOverlay->setGeometry(0, 0, 960, 720);
    m_dimOverlay->setStyleSheet("background-color: rgba(0,0,0,200);");
    m_dimOverlay->lower();
    m_dimOverlay->hide();

    m_extraRoundTitle = new QLabel("额外回合选卡", this);
    m_extraRoundTitle->setAlignment(Qt::AlignCenter);
    m_extraRoundTitle->setGeometry(200, 175, 560, 20);
    m_extraRoundTitle->setStyleSheet("color: #ffcc00; font-weight: bold; font-size: 14px; background: transparent;");
    m_extraRoundTitle->hide();

    m_skillPanel = new SkillPanel(this);
    m_skillPanel->setGeometry(200, 200, 560, 250);
    m_skillPanel->hide();

    connect(m_skillPanel, &SkillPanel::skillSelected,
            this, &BattlePage::onSkillSelected);

    connect(m_statusBar, &StatusBar::pauseClicked, this, &BattlePage::onPauseMenu);

    m_pauseOverlay = new QWidget(this);
    m_pauseOverlay->setGeometry(0, 0, 960, 720);
    m_pauseOverlay->setStyleSheet("background-color: rgba(0,0,0,160);");
    m_pauseOverlay->hide();

    QVBoxLayout *pauseLayout = new QVBoxLayout(m_pauseOverlay);
    pauseLayout->setAlignment(Qt::AlignCenter);

    QPushButton *saveBtn = new QPushButton("保存并退出");
    QPushButton *quitBtn = new QPushButton("不保存退出");
    QPushButton *restartBtn = new QPushButton("重新开始");
    QPushButton *resumeBtn = new QPushButton("继续游戏");

    saveBtn->setFixedSize(160, 40);
    quitBtn->setFixedSize(160, 40);
    restartBtn->setFixedSize(160, 40);
    resumeBtn->setFixedSize(160, 40);

    pauseLayout->addWidget(saveBtn);
    pauseLayout->addWidget(quitBtn);
    pauseLayout->addWidget(restartBtn);
    pauseLayout->addWidget(resumeBtn);

    connect(saveBtn, &QPushButton::clicked, this, &BattlePage::onSaveAndQuit);
    connect(quitBtn, &QPushButton::clicked, this, &BattlePage::onQuitWithoutSave);
    connect(restartBtn, &QPushButton::clicked, this, [this]() {
        QMessageBox msgBox(this);
        msgBox.setWindowTitle("重新开始");
        msgBox.setText("请选择：");
        msgBox.setStyleSheet("QMessageBox { background-color: #f0f0f0; } QMessageBox QLabel { color: black; } QPushButton { color: black; background-color: #e0e0e0; border: 1px solid #aaa; padding: 4px 12px; } QPushButton:hover { background-color: #d0d0d0; }");
        QPushButton *btnKeep = msgBox.addButton("使用当前配置", QMessageBox::ActionRole);
        QPushButton *btnChange = msgBox.addButton("修改配置后重开", QMessageBox::ActionRole);
        msgBox.addButton("取消", QMessageBox::RejectRole);
        msgBox.exec();
        QAbstractButton *clicked = msgBox.clickedButton();
        if (clicked == btnKeep) {
            resetBattle();
        } else if (clicked == btnChange) {
            GameConfig newConfig = m_gameConfig;
            if (showConfigDialog(newConfig, this)) {
                m_gameConfig = newConfig;
                resetBattle();
            }
        }
    });
    connect(resumeBtn, &QPushButton::clicked, [this](){
        m_pauseOverlay->hide();
        m_paused = false;
    });

    connect(m_questionWidget, &QuestionWidget::answerSubmitted,
            this, &BattlePage::onAnswerSubmitted);
}

void BattlePage::showNextQuestion()
{
    int targetDifficulty = m_gameConfig.rollDifficulty();
    int targetType = m_gameConfig.rollQuestionType();

    if (targetType == 1 && !m_completionQuestions.isEmpty()) {
        QVector<int> candidates;
        for (int d = 0; d <= 2 && candidates.isEmpty(); ++d) {
            int diff = (targetDifficulty + d) % 3;
            for (auto it = m_completionQuestions.begin(); it != m_completionQuestions.end(); ++it) {
                if (static_cast<int>(it.value().difficulty) == diff
                    && !m_usedQuestionIds[diff].contains(it.key()))
                    candidates.append(it.key());
            }
            if (candidates.isEmpty()) {
                m_usedQuestionIds[diff].clear();
                for (auto it = m_completionQuestions.begin(); it != m_completionQuestions.end(); ++it) {
                    if (static_cast<int>(it.value().difficulty) == diff)
                        candidates.append(it.key());
                }
            }
            if (!candidates.isEmpty()) {
                int qid = candidates[QRandomGenerator::global()->bounded(candidates.size())];
                m_usedQuestionIds[diff].insert(qid);
                CodeCompletionQuestion cq = m_completionQuestions[qid];
                QuestionData d;
                d.type = QuestionType::CodeCompletion;
                d.difficulty = cq.difficulty;
                d.description = cq.title + "\n" + cq.description;
                d.codeTemplate = cq.codeTemplate;
                d.id = cq.id;
                d.explanation = cq.explanation;
                d.tolerance = 3;
                if (!cq.testCases.isEmpty()) {
                    d.testCaseInput = cq.testCases.first().input;
                    d.testCaseOutput = cq.testCases.first().output;
                }
                m_currentQuestionData = d;
                m_currentCompletionData = cq;
                m_codeCompletionRetries = 3;
                m_questionWidget->setQuestion(m_currentQuestionData);
                m_questionWidget->setRemainingTolerance(m_codeCompletionRetries);
                return;
            }
        }
    }

    // 选择题：从 QuestionBank 按难度抽
    auto result = m_questionBank.drawQuestion(m_chapterId, targetDifficulty, m_usedQuestionIds[targetDifficulty]);
    if (!result.has_value()) {
        m_usedQuestionIds[targetDifficulty].clear();
        result = m_questionBank.drawQuestion(m_chapterId, targetDifficulty, m_usedQuestionIds[targetDifficulty]);
    }

    if (!result.has_value()) {
        QMessageBox msgBox(this);
        msgBox.setWindowTitle("提示");
        msgBox.setText("当前章节没有可用题目！");
        msgBox.setStyleSheet("color: black; background-color: #f0f0f0;");
        msgBox.exec();
        return;
    }

    Question backendQ = result.value();
    m_usedQuestionIds[static_cast<int>(backendQ.difficulty)].insert(backendQ.id);
    m_currentQuestionData = questionToQuestionData(backendQ);

    // 随机打乱选择题选项顺序
    if (m_currentQuestionData.type == QuestionType::Choice && m_currentQuestionData.options.size() > 1) {
        // 剥离标号前缀（如"A. "、"B) "），保留纯文本
        QStringList clean;
        for (const QString& opt : m_currentQuestionData.options) {
            QString t = opt.trimmed();
            int cut = 0;
            if (t.length() >= 2 && t[0].isLetter() && (t[1] == QLatin1Char('.') || t[1] == QLatin1Char(')') || t[1] == QLatin1Char('、')))
                cut = (t.length() >= 3 && t[2] == ' ') ? 3 : 2;
            clean.append(t.mid(cut).trimmed());
        }

        QString correctText = clean.value(m_currentQuestionData.correctOptionIndex);
        std::shuffle(clean.begin(), clean.end(), *QRandomGenerator::global());

        // 重新加上 A/B/C/D 标号并更新 correctOptionIndex
        for (int i = 0; i < clean.size(); ++i) {
            if (clean[i] == correctText) m_currentQuestionData.correctOptionIndex = i;
            m_currentQuestionData.options[i] = QString("%1. %2").arg(QChar::fromLatin1(static_cast<char>('A' + i))).arg(clean[i]);
        }
    }

    m_questionWidget->setQuestion(m_currentQuestionData);
    m_questionWidget->setRemainingTolerance(m_currentQuestionData.tolerance);
}

void BattlePage::onAnswerSubmitted()
{
    if (m_isExtraRound) {
        QuestionData q = m_currentQuestionData;
        QString answer = m_questionWidget->getAnswer();

        bool correct = false;
        if (q.type == QuestionType::Choice && q.correctOptionIndex >= 0 && q.correctOptionIndex < q.options.size()) {
            correct = (answer.trimmed() == q.options[q.correctOptionIndex].trimmed());
        } else if (q.type == QuestionType::CodeCompletion) {
            CodeJudgeResult r = CodeJudge::compileAndRun(m_currentCompletionData, answer.trimmed());
            correct = r.passed;
            if (!r.passed) {
                m_codeCompletionRetries--;
                m_questionWidget->setRemainingTolerance(m_codeCompletionRetries);
                QMessageBox msgBox(this);
                msgBox.setWindowTitle("判题结果");
                msgBox.setText(r.errorMessage.isEmpty() ? "答案错误" : r.errorMessage);
                msgBox.setStyleSheet("QMessageBox { background-color: #1e1e3a; } QMessageBox QLabel { color: white; } QPushButton { color: black; background-color: #e0e0e0; border: 1px solid #aaa; padding: 4px 12px; }");
                msgBox.exec();
                if (m_codeCompletionRetries > 0) {
                    m_questionWidget->showFeedback(false);
                    return;
                }
            }
        } else {
            correct = (QRandomGenerator::global()->bounded(2) == 1);
        }

        m_isExtraRound = false;
        m_questionWidget->showFeedback(correct);

        if (!correct) {
            m_wrongBookManager.addWrongQuestion(m_chapterId, q.id);
            m_wrongBookManager.save();
        }

        if (correct && m_pendingBuffId > 0) {
            if (m_currentSkillAttr == "攻击力") {
                m_playerStats.attack += m_currentSkillBonus;
                showFloatingText(QString("攻击力 +%1").arg(m_currentSkillBonus), QPoint(100, 160), QColor(255, 165, 0));
            } else if (m_currentSkillAttr == "防御力") {
                m_playerStats.defence += m_currentSkillBonus;
                showFloatingText(QString("防御力 +%1").arg(m_currentSkillBonus), QPoint(100, 160), QColor(100, 150, 255));
            } else if (m_currentSkillAttr == "回复生命") {
                m_playerStats.hp = qMin(m_playerStats.maxHp, m_playerStats.hp + m_currentSkillBonus);
                showFloatingText(QString("生命 +%1").arg(m_currentSkillBonus), QPoint(100, 160), Qt::green);
            } else if (m_currentSkillAttr == "吸血比例") {
                m_playerStats.lifesteal += m_currentSkillBonus * 0.01;
                showFloatingText(QString("吸血 +%1%").arg(m_currentSkillBonus), QPoint(100, 160), Qt::green);
            }
        }

        m_statusBar->updatePlayer(m_playerStats);
        m_extraRoundLabel->hide();
        m_questionWidget->show();
        showNextQuestion();
        return;
    }

    if (m_isArenaMode) {
        onArenaAnswerSubmitted();
        return;
    }

    QuestionData q = m_currentQuestionData;
    QString answer = m_questionWidget->getAnswer();

    bool correct = false;
    if (q.type == QuestionType::Choice) {
        // 选择题：比较用户选择的文本与正确答案对应的文本
        if (q.correctOptionIndex >= 0 && q.correctOptionIndex < q.options.size()) {
            correct = (answer.trimmed() == q.options[q.correctOptionIndex].trimmed());
        }
    } else if (q.type == QuestionType::CodeCompletion) {
        CodeJudgeResult r = CodeJudge::compileAndRun(m_currentCompletionData, answer.trimmed());
        correct = r.passed;
        if (!r.passed) {
            m_codeCompletionRetries--;
            m_questionWidget->setRemainingTolerance(m_codeCompletionRetries);
            QMessageBox msgBox(this);
            msgBox.setWindowTitle("判题结果");
            msgBox.setText(r.errorMessage.isEmpty() ? "答案错误" : r.errorMessage);
            msgBox.setStyleSheet("QMessageBox { background-color: #1e1e3a; } QMessageBox QLabel { color: white; }");
            msgBox.exec();
            if (m_codeCompletionRetries > 0) {
                m_questionWidget->showFeedback(false);
                return;
            }
        }
    } else {
        correct = (QRandomGenerator::global()->bounded(2) == 1);
    }
    m_questionWidget->showFeedback(correct);

    if (!correct) {
        m_wrongBookManager.addWrongQuestion(m_chapterId, q.id);
        m_wrongBookManager.save();
    } else {
        m_correctCount++;
    }

    int playerDmg = 0, enemyDmg = 0;

    if (correct) {
        playPlayerAnim(Anim_Attack);
        playEnemyAnim(Anim_Hurt);
        playerDmg = qMax(1, m_playerStats.attack - m_enemyStats.defence);
        // 吸血回复
        int heal = static_cast<int>(playerDmg * m_playerStats.lifesteal);
        m_playerStats.hp = qMin(m_playerStats.maxHp, m_playerStats.hp + heal);
        m_playerStats.totalDamage += playerDmg;
        m_enemyStats.hp = qMax(0, m_enemyStats.hp - playerDmg);

        showFloatingText(QString("-%1").arg(playerDmg), QPoint(860, 200), Qt::red);
        if (heal > 0)
            showFloatingText(QString("+%1").arg(heal), QPoint(80, 160), Qt::green);
    }

    playEnemyAnim(Anim_Attack);
    playPlayerAnim(Anim_Hurt);
    enemyDmg = qMax(1, m_enemyStats.attack - m_playerStats.defence);
    m_playerStats.hp = qMax(0, m_playerStats.hp - enemyDmg);
    showFloatingText(QString("-%1").arg(enemyDmg), QPoint(100, 200), Qt::yellow);

    // 刷新状态栏
    m_playerStats.round = m_currentRound;
    m_statusBar->updatePlayer(m_playerStats);
    m_statusBar->updateEnemy(m_enemyStats);

    // 检查战斗结束
    if (m_playerStats.hp <= 0) {
        playPlayerAnim(Anim_Death);
        onBattleFinished(false);
        return;
    }
    if (m_enemyStats.hp <= 0) {
        playEnemyAnim(Anim_Death);
        onBattleFinished(true);
        return;
    }

    // 进入下一回合前检查是否触发额外回合
    m_roundsSinceExtra++;
    if (m_roundsSinceExtra >= m_roundsPerExtra) {
        m_roundsSinceExtra = 0;
        onExtraRoundTrigger();
        return;
    }

    // 进入下一回合
    m_currentRound++;
    showNextQuestion();
}

void BattlePage::onArenaAnswerSubmitted()
{
    QString answer = m_questionWidget->getAnswer();
    bool correct = false;
    if (m_currentQuestionData.type == QuestionType::CodeCompletion) {
        CodeJudgeResult r = CodeJudge::compileAndRun(m_currentCompletionData, answer.trimmed());
        correct = r.passed;
        if (!r.passed) {
            QMessageBox msgBox(this);
            msgBox.setWindowTitle("判题结果");
            msgBox.setText(r.errorMessage.isEmpty() ? "答案错误" : r.errorMessage);
            msgBox.setStyleSheet("QMessageBox { background-color: #1e1e3a; } QMessageBox QLabel { color: white; }");
            msgBox.exec();
        }
    } else {
        correct = !answer.trimmed().isEmpty();
    }
    m_questionWidget->showFeedback(correct);

    if (correct) {
        m_arenaConsecutiveErrors = 0;

        if (m_currentSkillAttr == "攻击力") {
            m_playerStats.attack += m_currentSkillBonus;
        } else if (m_currentSkillAttr == "防御力") {
            m_playerStats.defence += m_currentSkillBonus;
        } else if (m_currentSkillAttr == "生命上限") {
            m_playerStats.maxHp += m_currentSkillBonus;
            m_playerStats.hp = qMin(m_playerStats.hp + m_currentSkillBonus, m_playerStats.maxHp);
        } else if (m_currentSkillAttr == "吸血比例") {
            m_playerStats.lifesteal += m_currentSkillBonus * 0.01;
        } else if (m_currentSkillAttr == "Pass卡") {
            m_passCardCount += m_currentSkillBonus;
            m_playerStats.passCards = m_passCardCount;
            m_passBtn->setText(QString("Pass (%1)").arg(m_passCardCount));
        } else if (m_currentSkillAttr == "回复生命") {
            m_playerStats.hp = qMin(m_playerStats.maxHp, m_playerStats.hp + m_currentSkillBonus);
            showFloatingText(QString("+%1").arg(m_currentSkillBonus), QPoint(80, 160), Qt::green);
        }

        int playerDmg = qMax(1, m_playerStats.attack - m_enemyStats.defence);
        m_enemyStats.hp -= playerDmg;
        m_playerStats.totalDamage += playerDmg;

        int heal = static_cast<int>(playerDmg * m_playerStats.lifesteal);
        m_playerStats.hp = qMin(m_playerStats.maxHp, m_playerStats.hp + heal);

        showFloatingText(QString("-%1").arg(playerDmg), QPoint(860, 200), Qt::red);
        if (heal > 0)
            showFloatingText(QString("+%1").arg(heal), QPoint(80, 160), Qt::green);

        m_statusBar->updatePlayer(m_playerStats);
        m_statusBar->updateEnemy(m_enemyStats);

        if (m_playerStats.hp <= 0) {
            onArenaBattleFinished();
            return;
        }

        m_passBtn->setEnabled(false);
        startArenaRound();
    } else {
        m_arenaConsecutiveErrors++;
        m_questionWidget->setRemainingTolerance(ARENA_TOLERANCE - m_arenaConsecutiveErrors);

        if (m_arenaConsecutiveErrors >= ARENA_TOLERANCE) {
            m_arenaConsecutiveErrors = 0;
            int enemyDmg = qMax(1, m_enemyStats.attack - m_playerStats.defence);
            m_playerStats.hp -= enemyDmg;
            showFloatingText(QString("-%1").arg(enemyDmg), QPoint(100, 200), Qt::yellow);
            m_statusBar->updatePlayer(m_playerStats);
            if (m_playerStats.hp <= 0) {
                onArenaBattleFinished();
                return;
            }
            m_wrongBookManager.addWrongQuestion(m_chapterId, m_currentQuestionData.id);
            m_wrongBookManager.save();
            m_questionWidget->reset();
            m_passBtn->setEnabled(false);
            startArenaRound();
        }
    }
}

void BattlePage::onExtraRoundTrigger()
{
    m_isExtraRound = true;
    m_pendingBuffId = -1;

    QList<SkillData> skills;

    SkillData skip;
    skip.id = 105; skip.name = "跳过额外回合"; skip.iconPath = "";
    skip.difficulty = Difficulty::Easy;
    skills.append(skip);

    QVector<int> pool = {101, 102, 103, 104};
    QSet<int> picked;
    while (picked.size() < 2) {
        int idx = QRandomGenerator::global()->bounded(pool.size());
        picked.insert(pool[idx]);
    }
    for (int id : picked) {
        SkillData s;
        s.id = id; s.iconPath = "";
        if (id == 101) s.attribute = "攻击力";
        else if (id == 102) s.attribute = "防御力";
        else if (id == 103) s.attribute = "回复生命";
        else s.attribute = "吸血比例";
        s.name = s.attribute;
        s.easyBonus = getArenaSkillBonus(s.attribute, Difficulty::Easy);
        s.mediumBonus = getArenaSkillBonus(s.attribute, Difficulty::Medium);
        s.hardBonus = getArenaSkillBonus(s.attribute, Difficulty::Hard);
        skills.append(s);
    }

    m_currentRoundSkills = skills;
    m_skillPanel->setSkills(skills, false);
    m_skillPanel->showWithAnimation();
    m_dimOverlay->show();
    m_extraRoundTitle->show();
    m_questionWidget->hide();
}

void BattlePage::onPassClicked()
{
    if (m_arenaState != WaitingAnswer) return;
    if (m_passCardCount <= 0) return;
    m_passCardCount--;
    m_passBtn->setEnabled(m_passCardCount > 0);
    m_playerStats.passCards = m_passCardCount;
    m_passBtn->setText(QString("Pass (%1)").arg(m_passCardCount));
    m_statusBar->updatePlayer(m_playerStats);
    m_wrongBookManager.addWrongQuestion(m_chapterId, m_currentQuestionData.id);
    m_wrongBookManager.save();
    m_questionWidget->reset();
    m_questionWidget->hide();
    startArenaRound();
}

void BattlePage::onArenaTimerTick()
{
    if (!m_isArenaMode) return;
    m_elapsed += 50;
    m_arenaElapsedMs += 50;
    double seconds = m_arenaElapsedMs / 1000.0;
    m_enemyStats.attack = 15 + static_cast<int>(0.025 * seconds + 0.0001 * seconds * seconds);
    m_enemyStats.defence = 5 + static_cast<int>(0.0125 * seconds + 0.00005 * seconds * seconds);
    m_countdownBar->setValue(m_elapsed);
    m_countdownBar->repaint();
    if (m_elapsed >= m_attackInterval) {
        m_elapsed = 0;
        int enemyDmg = qMax(1, m_enemyStats.attack - m_playerStats.defence);
        m_playerStats.hp -= enemyDmg;
        showFloatingText(QString("-%1").arg(enemyDmg), QPoint(100, 200), Qt::yellow);
        m_statusBar->updatePlayer(m_playerStats);
        if (m_playerStats.hp <= 0) {
            onArenaBattleFinished();
            return;
        }
    }
    m_statusBar->updateEnemy(m_enemyStats);
}

void BattlePage::onSkillSelected(int skillId, Difficulty diff)
{
    m_skillPanel->hideWithAnimation();
    if (m_isArenaMode) {
        SkillData chosenSkill;
        bool found = false;
        for (const SkillData& s : m_currentRoundSkills) {
            if (s.id == skillId) {
                chosenSkill = s;
                found = true;
                break;
            }
        }
        if (!found) return;

        // 记录技能属性和加成（用于答对后应用）
        m_currentSkillAttr = chosenSkill.attribute;
        // 根据难度设定加成数值（使用技能数据中的对应值）
        int bonus = 0;
        if (diff == Difficulty::Easy)      bonus = chosenSkill.easyBonus;
        else if (diff == Difficulty::Medium) bonus = chosenSkill.mediumBonus;
        else                    bonus = chosenSkill.hardBonus;
        m_currentSkillBonus = bonus;

        int diffInt = static_cast<int>(diff);
        QVector<int> candidates;
        for (int d = 0; d <= 2 && candidates.isEmpty(); ++d) {
            int searchDiff = (diffInt + d) % 3;
            for (auto it = m_completionQuestions.begin(); it != m_completionQuestions.end(); ++it) {
                if (static_cast<int>(it.value().difficulty) == searchDiff
                    && !m_usedQuestionIds[searchDiff].contains(it.key()))
                    candidates.append(it.key());
            }
            if (candidates.isEmpty()) {
                m_usedQuestionIds[searchDiff].clear();
                for (auto it = m_completionQuestions.begin(); it != m_completionQuestions.end(); ++it) {
                    if (static_cast<int>(it.value().difficulty) == searchDiff)
                        candidates.append(it.key());
                }
            }
        }
        if (!candidates.isEmpty()) {
            int qid = candidates[QRandomGenerator::global()->bounded(candidates.size())];
            int qDiff = static_cast<int>(m_completionQuestions[qid].difficulty);
            m_usedQuestionIds[qDiff].insert(qid);
            CodeCompletionQuestion cq = m_completionQuestions[qid];
            QuestionData d;
            d.type = QuestionType::CodeCompletion;
            d.difficulty = cq.difficulty;
            d.description = cq.title + "\n" + cq.description;
            d.codeTemplate = cq.codeTemplate;
            d.id = cq.id;
            d.explanation = cq.explanation;
            d.tolerance = 3;
            if (!cq.testCases.isEmpty()) {
                d.testCaseInput = cq.testCases.first().input;
                d.testCaseOutput = cq.testCases.first().output;
            }
            m_currentQuestionData = d;
            m_currentCompletionData = cq;
        } else {
            QMessageBox::warning(this, "错误", "所有章节均无该难度补全题，请联系出题人补充！");
            return;
        }
        m_questionWidget->setQuestion(m_currentQuestionData);
        m_questionWidget->setRemainingTolerance(ARENA_TOLERANCE);
        m_questionWidget->show();

        m_arenaState = WaitingAnswer;
        m_passBtn->setEnabled(m_passCardCount > 0);
        m_skillPanel->hide();
    }
    else
    {
        if (skillId == 105) {
            m_isExtraRound = false;
            m_extraRoundLabel->hide();
            m_dimOverlay->hide();
            m_extraRoundTitle->hide();
            m_questionWidget->reset();
            m_questionWidget->show();
            showNextQuestion();
            return;
        }

        m_pendingBuffId = skillId;
        m_dimOverlay->hide();
        m_extraRoundTitle->hide();

        SkillData chosenSkill;
        for (const SkillData& s : m_currentRoundSkills) {
            if (s.id == skillId) { chosenSkill = s; break; }
        }
        m_currentSkillAttr = chosenSkill.attribute;
        if (diff == Difficulty::Easy)      m_currentSkillBonus = chosenSkill.easyBonus;
        else if (diff == Difficulty::Medium) m_currentSkillBonus = chosenSkill.mediumBonus;
        else                                m_currentSkillBonus = chosenSkill.hardBonus;

        int diffInt = static_cast<int>(diff);
        int qType = m_gameConfig.rollQuestionType();

        bool drawn = false;
        if (qType == 1 && !m_completionQuestions.isEmpty()) {
            QVector<int> candidates;
            for (int d = 0; d <= 2 && candidates.isEmpty(); ++d) {
                int searchDiff = (diffInt + d) % 3;
                for (auto it = m_completionQuestions.begin(); it != m_completionQuestions.end(); ++it) {
                    if (static_cast<int>(it.value().difficulty) == searchDiff
                        && !m_usedQuestionIds[searchDiff].contains(it.key()))
                        candidates.append(it.key());
                }
                if (candidates.isEmpty()) {
                    m_usedQuestionIds[searchDiff].clear();
                    for (auto it = m_completionQuestions.begin(); it != m_completionQuestions.end(); ++it) {
                        if (static_cast<int>(it.value().difficulty) == searchDiff)
                            candidates.append(it.key());
                    }
                }
                if (!candidates.isEmpty()) {
                    int qid = candidates[QRandomGenerator::global()->bounded(candidates.size())];
                    m_usedQuestionIds[searchDiff].insert(qid);
                    CodeCompletionQuestion cq = m_completionQuestions[qid];
                    QuestionData d;
                    d.type = QuestionType::CodeCompletion;
                    d.difficulty = cq.difficulty;
                    d.description = cq.title + "\n" + cq.description;
                    d.codeTemplate = cq.codeTemplate;
                    d.id = cq.id;
                    d.explanation = cq.explanation;
                    d.tolerance = 3;
                    if (!cq.testCases.isEmpty()) {
                        d.testCaseInput = cq.testCases.first().input;
                        d.testCaseOutput = cq.testCases.first().output;
                    }
                    m_currentQuestionData = d;
                    m_currentCompletionData = cq;
                    m_questionWidget->reset();
                    m_questionWidget->setQuestion(m_currentQuestionData);
                    m_questionWidget->show();
                    m_extraRoundLabel->show();
                    drawn = true;
                }
            }
        }

        if (!drawn) {
            auto result = m_questionBank.drawQuestion(m_chapterId, diffInt, m_usedQuestionIds[diffInt]);
            if (!result.has_value()) {
                m_usedQuestionIds[diffInt].clear();
                result = m_questionBank.drawQuestion(m_chapterId, diffInt, m_usedQuestionIds[diffInt]);
            }
            if (result.has_value()) {
                Question q = result.value();
                m_usedQuestionIds[static_cast<int>(q.difficulty)].insert(q.id);
                m_currentQuestionData = questionToQuestionData(q);
                m_questionWidget->reset();
                m_questionWidget->setQuestion(m_currentQuestionData);
                m_questionWidget->show();
                m_extraRoundLabel->show();
                drawn = true;
            }
        }

        if (!drawn) {
            m_isExtraRound = false;
            m_extraRoundLabel->hide();
            m_questionWidget->reset();
            m_questionWidget->show();
            showNextQuestion();
            return;
        }
        m_skillPanel->hide();
    }
}

void BattlePage::showFloatingText(const QString &text, QPoint pos, QColor color)
{
    QLabel *label = new QLabel(this);
    label->setText(text);
    label->setStyleSheet(QString("color: %1; font-weight: bold; font-size: 18px; background: transparent;").arg(color.name()));
    label->adjustSize();
    label->move(pos);
    label->show();

    // 向上移动
    QPropertyAnimation *moveAnim = new QPropertyAnimation(label, "pos");
    moveAnim->setDuration(800);
    moveAnim->setStartValue(pos);
    moveAnim->setEndValue(pos + QPoint(0, -50));
    moveAnim->setEasingCurve(QEasingCurve::OutQuad);

    // 淡出
    QGraphicsOpacityEffect *effect = new QGraphicsOpacityEffect(label);
    label->setGraphicsEffect(effect);
    QPropertyAnimation *fadeAnim = new QPropertyAnimation(effect, "opacity");
    fadeAnim->setDuration(800);
    fadeAnim->setStartValue(1.0);
    fadeAnim->setEndValue(0.0);

    QParallelAnimationGroup *group = new QParallelAnimationGroup(this);
    group->addAnimation(moveAnim);
    group->addAnimation(fadeAnim);
    connect(group, &QParallelAnimationGroup::finished, label, &QObject::deleteLater);
    group->start(QAbstractAnimation::DeleteWhenStopped);
}

void BattlePage::keyPressEvent(QKeyEvent *event)
{
        if (event->key() == Qt::Key_Escape) {
            if (m_isArenaMode) {
                QMessageBox msgBox;
                msgBox.setWindowTitle("退出确认");
                msgBox.setText("确定退出竞技模式吗？\n退出后将计算总伤害。\n注意：游戏计时不会暂停！");
                msgBox.setStyleSheet("QMessageBox { background-color: #f0f0f0; } QMessageBox QLabel { color: black; }");
                QPushButton *btnYes = msgBox.addButton("确定退出", QMessageBox::YesRole);
                msgBox.addButton("取消", QMessageBox::NoRole);
                msgBox.exec();
                if (msgBox.clickedButton() == btnYes) {
                    onArenaBattleFinished();
                }
            } else {
                // 学习模式：打开暂停菜单
                onPauseMenu();
            }
            return;
        }
        QWidget::keyPressEvent(event);
}

void BattlePage::onPauseMenu()
{
    if (!m_paused) {
        m_paused = true;
        m_pauseOverlay->show();
    }
}

void BattlePage::onSaveAndQuit()
{
    saveToArchive();
    emit quitBattle();
}

void BattlePage::onQuitWithoutSave()
{
    m_saveManager.setSaveDirectory(QCoreApplication::applicationDirPath() + "/saves");
    m_saveManager.deleteChapterArchive(m_chapterId);
    emit quitBattle();
}

void BattlePage::onRestart()
{
    // 重置状态
    m_playerStats.hp = m_playerStats.maxHp;
    m_enemyStats.hp = m_enemyStats.maxHp;
    m_currentRound = 1;
    m_roundsSinceExtra = 0;
    m_usedQuestionIds.clear();
    m_playerStats.totalDamage = 0;
    m_playerStats.buffs.clear();
    m_correctCount = 0;

    m_statusBar->updatePlayer(m_playerStats);
    m_statusBar->updateEnemy(m_enemyStats);

    m_questionWidget->show();
    showNextQuestion();
    m_pauseOverlay->hide();
    m_paused = false;
}

void BattlePage::onBattleFinished(bool victory)
{
    m_saveManager.setSaveDirectory(QCoreApplication::applicationDirPath() + "/saves");
    m_saveManager.deleteChapterArchive(m_chapterId);

    bool trophy = victory && m_gameConfig.meetsThreshold();
    if (victory) {
        RankingEntry entry;
        entry.chapterId = m_chapterId;
        entry.totalDamage = m_playerStats.totalDamage;
        entry.rounds = m_currentRound;
        entry.correctCount = m_correctCount;
        entry.timestamp = QDateTime::currentDateTimeUtc().toSecsSinceEpoch();
        entry.trophyEarned = trophy;
        entry.victory = true;
        m_rankingManager.addRecord(entry);
        m_rankingManager.save();

        m_achievementManager.checkChapterClear(m_chapterId, trophy);
        if (trophy) {
            if (m_gameConfig.choiceWeight == 0 && m_gameConfig.codeCompletionWeight > 0)
                m_achievementManager.unlockAchievement("code_only_clear");
            if (m_gameConfig.easyWeight == 0 && m_gameConfig.mediumWeight == 0 && m_gameConfig.hardWeight > 0)
                m_achievementManager.unlockAchievement("hard_only_clear");
        }
        m_achievementManager.saveProgress();
    }

    if (victory) {
        QMessageBox msgBox(this);
        msgBox.setWindowTitle("战斗结束");
        msgBox.setText("你胜利了！");
        msgBox.setStyleSheet("QMessageBox { background-color: #f0f0f0; } QMessageBox QLabel { color: black; }");
        msgBox.exec();
        emit gameOver(victory, m_chapterId, m_playerStats.totalDamage, m_currentRound, m_correctCount, trophy);
    } else {
        QMessageBox msgBox(this);
        msgBox.setWindowTitle("战斗失败");
        msgBox.setText("你失败了...\n要重新开始吗？");
        msgBox.setStyleSheet("QMessageBox { background-color: #f0f0f0; } QMessageBox QLabel { color: black; } QPushButton { color: black; background-color: #e0e0e0; border: 1px solid #aaa; padding: 4px 12px; } QPushButton:hover { background-color: #d0d0d0; }");
        QPushButton *btnKeep = msgBox.addButton("重新开始(当前配置)", QMessageBox::ActionRole);
        QPushButton *btnChange = msgBox.addButton("修改配置后重开", QMessageBox::ActionRole);
        QPushButton *btnQuit = msgBox.addButton("返回主菜单", QMessageBox::RejectRole);
        msgBox.exec();
        QAbstractButton *clicked = msgBox.clickedButton();
        if (clicked == btnKeep) {
            resetBattle();
        } else if (clicked == btnChange) {
            GameConfig newConfig = m_gameConfig;
            if (showConfigDialog(newConfig, this)) {
                m_gameConfig = newConfig;
                resetBattle();
            }
        } else if (clicked == btnQuit) {
            emit quitBattle();
        }
    }
}

void BattlePage::onArenaBattleFinished()
{
    m_arenaTimer->stop();

    int dmg = m_playerStats.totalDamage;
    if (dmg >= 100)  m_achievementManager.unlockAchievement("arena_100");
    if (dmg >= 200)  m_achievementManager.unlockAchievement("arena_200");
    if (dmg >= 500)  m_achievementManager.unlockAchievement("arena_500");
    if (dmg >= 1000) m_achievementManager.unlockAchievement("arena_1000");
    m_achievementManager.saveProgress();

    RankingEntry arenaEntry;
    arenaEntry.chapterId = -1;
    arenaEntry.totalDamage = dmg;
    arenaEntry.timestamp = QDateTime::currentDateTimeUtc().toSecsSinceEpoch();
    arenaEntry.victory = false;
    m_rankingManager.addRecord(arenaEntry);
    m_rankingManager.save();

    QMessageBox msgBox;
    msgBox.setWindowTitle("战斗结束");
    msgBox.setText(QString("你被击败了！\n总伤害: %1").arg(m_playerStats.totalDamage));
    msgBox.setStyleSheet("QMessageBox { background-color: #f0f0f0; } QMessageBox QLabel { color: black; }");
    QPushButton *btnLeaderboard = msgBox.addButton("查看排行榜", QMessageBox::ActionRole);
    QPushButton *btnReturn = msgBox.addButton("返回主菜单", QMessageBox::AcceptRole);
    msgBox.exec();
    if (msgBox.clickedButton() == btnLeaderboard) {
        emit arenaGoToRanking();
    } else {
        emit arenaQuit();
    }
}

void BattlePage::saveToArchive()
{
    ChapterArchive archive;
    archive.chapterId = m_chapterId;
    archive.playerData = m_playerStats.toJson();
    archive.enemyData = m_enemyStats.toJson();
    archive.usedQuestionIds = m_usedQuestionIds;
    archive.normalRoundCount = m_currentRound;
    archive.configData = m_gameConfig;
    archive.hasActiveArchive = true;

    m_saveManager.setSaveDirectory(QCoreApplication::applicationDirPath() + "/saves");
    m_saveManager.saveChapterArchive(archive);
}

void BattlePage::loadFromArchive()
{
    m_saveManager.setSaveDirectory(QCoreApplication::applicationDirPath() + "/saves");
    ChapterArchive archive = m_saveManager.loadChapterArchive(m_chapterId);

    if (!archive.hasActiveArchive) return;

    // 恢复存档中的难度配置
    if (archive.configData.easyWeight + archive.configData.mediumWeight + archive.configData.hardWeight > 0)
        m_gameConfig = archive.configData;

    m_playerStats = Stats::fromJson(archive.playerData);
    m_enemyStats = Stats::fromJson(archive.enemyData);
    m_usedQuestionIds = archive.usedQuestionIds;
    m_currentRound = archive.normalRoundCount;
    m_roundsSinceExtra = 0;

    m_playerStats.totalDamage = 0;
    m_playerStats.round = m_currentRound;
    m_statusBar->updatePlayer(m_playerStats);
    m_statusBar->updateEnemy(m_enemyStats);
    showNextQuestion();
}

void BattlePage::resetBattle()
{
    m_pauseOverlay->hide();
    m_paused = false;

    m_playerStats.hp = 100;
    m_playerStats.maxHp = 100;
    m_playerStats.attack = 25;
    m_playerStats.defence = 8;
    m_playerStats.lifesteal = 0.1;
    m_playerStats.totalDamage = 0;
    m_playerStats.buffs.clear();

    m_enemyStats.hp = 150;
    m_enemyStats.maxHp = 150;
    m_enemyStats.attack = 15;
    m_enemyStats.defence = 5;

    m_currentRound = 1;
    m_roundsSinceExtra = 0;
    m_usedQuestionIds.clear();
    m_isExtraRound = false;

    QString questionPath = findDataFile(QString("data/questions/chapter%1.json").arg(m_chapterId));
    if (!questionPath.isEmpty()) {
        bool loaded = m_questionBank.loadChapter(m_chapterId, questionPath);
        if (!loaded) {
            QMessageBox::critical(this, "错误", QString("无法加载题库 chapter%1.json").arg(m_chapterId));
        } else if (m_questionBank.getChapterQuestionCount(m_chapterId) == 0) {
            QMessageBox::warning(this, "警告", QString("第%1章题库为空").arg(m_chapterId));
        }
    } else {
        QMessageBox::critical(this, "错误", QString("找不到文件 data/questions/chapter%1.json").arg(m_chapterId));
    }

    if (!m_isArenaMode) {
    m_completionQuestions.clear();
    QString completionPath = findDataFile(QString("data/questions/chapter%1_completion.json").arg(m_chapterId));
    if (!completionPath.isEmpty()) {
        QFile f(completionPath);
        if (f.open(QIODevice::ReadOnly)) {
            QJsonObject root = QJsonDocument::fromJson(f.readAll()).object();
            QJsonArray arr = root["questions"].toArray();
            for (const auto& v : arr) {
                CodeCompletionQuestion cq = CodeCompletionQuestion::fromJson(v.toObject());
                m_completionQuestions[cq.id] = cq;
            }
        }
    }
    }

    // 刷新状态栏
    m_statusBar->updatePlayer(m_playerStats);
    m_statusBar->updateEnemy(m_enemyStats);

    // 显示题目区（可能在额外回合时被隐藏了）
    m_questionWidget->show();
    m_skillPanel->hide();
    m_extraRoundLabel->hide();
    m_dimOverlay->hide();
    m_extraRoundTitle->hide();

    // 显示第一道题
    showNextQuestion();
    setArenaMode(m_isArenaMode);
    if (m_isArenaMode)
        QTimer::singleShot(50, this, [this](){ startArenaRound(); });
}

void BattlePage::loadFrames(const QString& prefix, int count, QList<QPixmap>& outFrames, bool flipHorizontal)
{
    outFrames.clear();
    for (int i = 0; i < count; ++i) {
        QString path = QString("%1%2.png").arg(prefix).arg(i, 3, 10, QLatin1Char('0'));
        QPixmap pix(path);
        if (!pix.isNull()) {
            QPixmap scaled = pix.scaled(240, 240, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            if (flipHorizontal)
                scaled = QPixmap::fromImage(scaled.toImage().mirrored(true, false));
            outFrames.append(scaled);
        }
    }
}

void BattlePage::playPlayerAnim(AnimState state)
{
    if (m_playerState == Anim_Death && state != Anim_Death) return;
    if (m_playerState == state) return;
    m_playerState = state;
    switch (state) {
    case Anim_Idle:   if (!m_playerIdlePix.isNull())   m_playerSprite->setPixmap(m_playerIdlePix); break;
    case Anim_Attack: if (!m_playerAttackPix.isNull()) m_playerSprite->setPixmap(m_playerAttackPix); break;
    case Anim_Hurt:   if (!m_playerHurtPix.isNull())   m_playerSprite->setPixmap(m_playerHurtPix); break;
    case Anim_Death:  if (!m_playerDeathPix.isNull())  m_playerSprite->setPixmap(m_playerDeathPix); break;
    }
    if (state == Anim_Attack || state == Anim_Hurt)
        QTimer::singleShot(800, this, [this](){ playPlayerAnim(Anim_Idle); });
}

void BattlePage::playEnemyAnim(AnimState state)
{
    if (m_enemyState == Anim_Death && state != Anim_Death) return;
    if (m_enemyState == state) return;
    m_enemyState = state;
    m_enemyFrameIndex = 0;
}

void BattlePage::onAnimTimerTick()
{
    QList<QPixmap>* frames = nullptr;
    switch (m_enemyState) {
    case Anim_Idle:   frames = &m_enemyIdleFrames; break;
    case Anim_Attack: frames = &m_enemyAttackFrames; break;
    case Anim_Hurt:   frames = &m_enemyHurtFrames; break;
    case Anim_Death:  frames = &m_enemyDeathFrames; break;
    }
    if (frames && !frames->isEmpty()) {
        int idx = m_enemyFrameIndex % frames->size();
        m_enemySprite->setPixmap((*frames)[idx]);
        m_enemyFrameIndex++;
        if ((m_enemyState == Anim_Attack || m_enemyState == Anim_Hurt)
            && m_enemyFrameIndex >= frames->size())
            playEnemyAnim(Anim_Idle);
        if (m_enemyState == Anim_Death && m_enemyFrameIndex >= frames->size())
            m_enemyFrameIndex = frames->size() - 1;
    }
}

QuestionData BattlePage::questionToQuestionData(const Question& q)
{
    QuestionData d;
    d.type = static_cast<QuestionType>(static_cast<int>(q.type));
    d.difficulty = q.difficulty;
    d.description = q.questionText;
    d.correctOptionIndex = q.correctOptionIndex;
    d.id = q.id;
    d.explanation = q.explanation;
    d.tolerance = 0;

    // 转换选项
    for (const QString& opt : q.options) {
        d.options.append(opt);
    }

    return d;
}