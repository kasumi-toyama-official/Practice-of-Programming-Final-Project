#ifndef UIMANAGER_H
#define UIMANAGER_H

#include <QObject>
#include <QWidget>
#include <QMap>
#include <QList>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>

class UIManager : public QObject
{
    Q_OBJECT
public:
    // 页面枚举，方便使用名字而不是数字
    enum Page {
        MainMenu,
        ChapterSelect,
        Battle,
        Result,
        ErrorBook,
        Collection,
        Achievement,
        Settings,
        Ranking
    };

    static UIManager* instance();  // 单例获取

    void setContainer(QWidget* container); // 设置主容器
    void goTo(Page page);                  // 跳转到某页
    void goBack();                         // 返回上一页
    void clearHistory() { m_history.clear(); }  // 清空历史
    void setPendingChapterId(int id) { m_pendingChapterId = id; }
    void setLoadExisting(bool load) { m_loadExisting = load; }
    QWidget* getPage(Page page) { return m_pages.value(page, nullptr); }

private:
    explicit UIManager(QObject *parent = nullptr);
    static UIManager* m_instance;

    QWidget* m_container = nullptr;        // 主容器
    QMap<Page, QWidget*> m_pages;          // 所有页面
    Page m_currentPage = MainMenu;         // 当前显示的页
    QList<Page> m_history;                 // 页面历史，用于返回
    int m_pendingChapterId = 1;
    bool m_loadExisting = false;            // 选中的章节ID
    static const int PAGE_WIDTH = 960;
    static const int PAGE_HEIGHT = 640;

    void registerPage(Page id, QWidget* page); // 注册页面
    void animateForward(QWidget* from, QWidget* to); // 前进动画
    void animateBack(QWidget* from, QWidget* to);    // 后退动画
};

#endif