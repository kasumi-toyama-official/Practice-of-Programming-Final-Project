#include "uimanager.h"
#include <QSize>
#include <mainmenupage.h>
#include <chapterselectpage.h>
#include <battlepage.h>
#include <resultpage.h>
#include <errorbookpage.h>
#include <collectionpage.h>
#include <achievementpage.h>
#include <settingspage.h>

UIManager* UIManager::m_instance = nullptr;

UIManager::UIManager(QObject *parent)
    : QObject{parent}
{}

UIManager* UIManager::instance()
{
    if (!m_instance)
    {
        m_instance = new UIManager;
    }
    return m_instance;
}

void UIManager::setContainer(QWidget *container)
{
    m_container = container;

    m_container->setFixedSize(960, 720);

    registerPage(MainMenu, new MainMenuPage());
    registerPage(ChapterSelect, new ChapterSelectPage());
    registerPage(Battle, new BattlePage());
    BattlePage *battle = qobject_cast<BattlePage*>(m_pages[Battle]);
    connect(battle, &BattlePage::gameOver, this, [this](bool victory, int chapter, int damage) {
        Q_UNUSED(victory);
        Q_UNUSED(chapter);
        Q_UNUSED(damage);
        goTo(Result);
    });
    connect(battle, &BattlePage::quitBattle, this, [this]() {
        // 回到上一个页面（学习模式情况下是 ChapterSelect，竞技模式是 MainMenu）
        // 暂时直接回主菜单
        while (m_history.size() > 1)
            m_history.pop_back();
        goTo(MainMenu);
    });
    connect(battle, &BattlePage::arenaQuit, this, [this]() {
        // 清空页面历史，回到主菜单
        m_history.clear();
        m_history.append(MainMenu);
        goTo(MainMenu);
    });
    registerPage(Result, new ResultPage());
    registerPage(ErrorBook, new ErrorBookPage());
    registerPage(Collection, new CollectionPage());
    registerPage(Achievement, new AchievementPage());
    registerPage(Settings, new SettingsPage());

    QWidget* mainMenu = m_pages[MainMenu];
    mainMenu->show();
    m_currentPage = MainMenu;
    m_history.append(MainMenu);
}

void UIManager::registerPage(Page id, QWidget *page)
{
    page->setParent(m_container);
    page->setGeometry(0, 0, m_container->width(), m_container->height());
    page->hide();
    m_pages.insert(id, page);
}

void UIManager::goTo(Page page)
{
    if (!m_container || !m_pages.contains(page))
        return;
    QWidget* from = m_pages.value(m_currentPage, nullptr);
    QWidget* to = m_pages.value(page);
    if (from == to) return;
    if (page == Battle) {
        BattlePage* bp = qobject_cast<BattlePage*>(to);
        if (bp) {
            bp->setChapterId(m_pendingChapterId);
            if (m_loadExisting) {
                bp->setLoadExisting(true);
                bp->resetBattle();
                bp->loadFromArchive();
            } else {
                bp->setLoadExisting(false);
                bp->resetBattle();
            }
            m_loadExisting = false;
        }
    }
    if (from)
        animateForward(from, to);
    else
        to->show();
    m_history.append(page);
    m_currentPage = page;
}

void UIManager::goBack()
{
    if (m_history.size() <= 1) return;
    m_history.pop_back();
    Page previous = m_history.last();
    QWidget* from = m_pages.value(m_currentPage);
    QWidget* to = m_pages.value(previous);
    animateBack(from, to);
    m_currentPage = previous;
}

void UIManager::animateForward(QWidget *from, QWidget *to)
{
    int w = m_container->width();
    int h = m_container->height();
    to->setGeometry(w, 0, w, h);
    to->show();
    to->raise();
    QPropertyAnimation *animForm = new QPropertyAnimation(from, "pos");
    animForm->setDuration(300);
    animForm->setStartValue(QPoint(0, 0));
    animForm->setEndValue(QPoint(-w, 0));
    animForm->setEasingCurve(QEasingCurve::InOutCubic);
    QPropertyAnimation *animTo = new QPropertyAnimation(to, "pos");
    animTo->setDuration(300);
    animTo->setStartValue(QPoint(w, 0));
    animTo->setEndValue(QPoint(0, 0));
    animTo->setEasingCurve(QEasingCurve::InOutCubic);
    QParallelAnimationGroup *group = new QParallelAnimationGroup;
    group->addAnimation(animForm);
    group->addAnimation(animTo);
    group->start(QAbstractAnimation::DeleteWhenStopped);

    connect(group, &QParallelAnimationGroup::finished, from, &QWidget::hide);
}

void UIManager::animateBack(QWidget *from, QWidget *to)
{
    int w = m_container->width();
    int h = m_container->height();
    to->setGeometry(-w, 0, w, h);
    to->show();
    to->raise();
    QPropertyAnimation *animForm = new QPropertyAnimation(from, "pos");
    animForm->setDuration(300);
    animForm->setStartValue(QPoint(0, 0));
    animForm->setEndValue(QPoint(w, 0));
    animForm->setEasingCurve(QEasingCurve::InOutCubic);
    QPropertyAnimation *animTo = new QPropertyAnimation(to, "pos");
    animTo->setDuration(300);
    animTo->setStartValue(QPoint(-w, 0));
    animTo->setEndValue(QPoint(0, 0));
    animTo->setEasingCurve(QEasingCurve::InOutCubic);
    QParallelAnimationGroup *group = new QParallelAnimationGroup;
    group->addAnimation(animForm);
    group->addAnimation(animTo);
    group->start(QAbstractAnimation::DeleteWhenStopped);

    connect(group, &QParallelAnimationGroup::finished, from, &QWidget::hide);
}