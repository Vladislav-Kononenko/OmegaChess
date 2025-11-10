#pragma once

#include <QMainWindow>
#include "../controller/GameController.hpp"   // ВАЖНО: полный заголовок

class QListWidget;
class QToolBar;
class QMenu;
class QAction;
class QLabel;
class BoardView;   // forward-декларация достаточно

/**
 * Главное окно приложения Omega-шахматы.
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(GameController *controller, QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    // Действия меню/кнопок
    void onNewGame();
    void onUndo();
    void onRedo();
    void onExit();
    void onAbout();

    // Реакция на сигналы от GameController
    void onGameStateChanged(GameController::GameState state);
    void onCurrentPlayerChanged(GameController::Player player);
    void onMoveMade(const struct Move &move);

    // Слот для клика по доске
    void onBoardCellClicked(int row, int col);

private:
    void createCentralWidgets();
    void createActions();
    void createMenus();
    void createToolBars();
    void createStatusBar();
    void connectController();

    void updateWindowTitle();
    void updateUndoRedoActions();

private:
    GameController *m_controller = nullptr;

    BoardView   *m_boardView   = nullptr;
    QListWidget *m_moveList    = nullptr;
    QLabel      *m_statusLabel = nullptr;

    QMenu    *m_fileMenu   = nullptr;
    QMenu    *m_gameMenu   = nullptr;
    QMenu    *m_helpMenu   = nullptr;
    QToolBar *m_mainToolBar = nullptr;

    QAction *m_newGameAction = nullptr;
    QAction *m_undoAction    = nullptr;
    QAction *m_redoAction    = nullptr;
    QAction *m_exitAction    = nullptr;
    QAction *m_aboutAction   = nullptr;
};
