#include "MainWindow.hpp"

#include <QListWidget>
#include <QToolBar>
#include <QMenuBar>
#include <QStatusBar>
#include <QAction>
#include <QLabel>
#include <QMessageBox>
#include <QWidget>
#include <QHBoxLayout>
#include <QSplitter>

#include "BoardView.hpp"
#include "GameController.hpp"

// Конструктор/деструктор

MainWindow::MainWindow(GameController *controller, QWidget *parent)
    : QMainWindow(parent)
    , m_controller(controller)
{
    createCentralWidgets();
    createActions();
    createMenus();
    createToolBars();
    createStatusBar();
    connectController();

    resize(900, 700);

    if (m_controller)
    {
        m_controller->startNewGame();
    }

    updateUndoRedoActions();
    updateWindowTitle();
}

MainWindow::~MainWindow() = default;

// Создание центральной области: доска + список ходов

void MainWindow::createCentralWidgets()
{
    // Центральный контейнер
    auto *central = new QWidget(this);
    auto *layout  = new QHBoxLayout(central);
    layout->setContentsMargins(2, 2, 2, 2);
    layout->setSpacing(4);

    // Виджет доски
    m_boardView = new BoardView(m_controller, central);

    // Список ходов
    m_moveList = new QListWidget(central);
    m_moveList->setMinimumWidth(200);
    m_moveList->setUniformItemSizes(true);

    // Можно использовать QSplitter для удобного ресайза
    auto *splitter = new QSplitter(Qt::Horizontal, central);
    splitter->addWidget(m_boardView);
    splitter->addWidget(m_moveList);
    splitter->setStretchFactor(0, 3);
    splitter->setStretchFactor(1, 1);

    layout->addWidget(splitter);

    setCentralWidget(central);
}

// Создание QAction’ов

void MainWindow::createActions()
{
    m_newGameAction = new QAction(tr("Новая партия"), this);
    m_newGameAction->setShortcut(QKeySequence::New);
    connect(m_newGameAction, &QAction::triggered,
            this, &MainWindow::onNewGame);

    m_undoAction = new QAction(tr("Отменить ход"), this);
    m_undoAction->setShortcut(QKeySequence::Undo);
    connect(m_undoAction, &QAction::triggered,
            this, &MainWindow::onUndo);

    m_redoAction = new QAction(tr("Повторить ход"), this);
    m_redoAction->setShortcut(QKeySequence::Redo);
    connect(m_redoAction, &QAction::triggered,
            this, &MainWindow::onRedo);

    m_exitAction = new QAction(tr("Выход"), this);
    m_exitAction->setShortcut(QKeySequence::Quit);
    connect(m_exitAction, &QAction::triggered,
            this, &MainWindow::onExit);

    m_aboutAction = new QAction(tr("О программе"), this);
    connect(m_aboutAction, &QAction::triggered,
            this, &MainWindow::onAbout);
}

// Меню

void MainWindow::createMenus()
{
    m_fileMenu = menuBar()->addMenu(tr("Файл"));
    m_fileMenu->addAction(m_newGameAction);
    m_fileMenu->addSeparator();
    m_fileMenu->addAction(m_exitAction);

    m_gameMenu = menuBar()->addMenu(tr("Игра"));
    m_gameMenu->addAction(m_undoAction);
    m_gameMenu->addAction(m_redoAction);

    m_helpMenu = menuBar()->addMenu(tr("Справка"));
    m_helpMenu->addAction(m_aboutAction);
}

// Тулбар

void MainWindow::createToolBars()
{
    m_mainToolBar = addToolBar(tr("Основная панель"));
    m_mainToolBar->setMovable(true);

    m_mainToolBar->addAction(m_newGameAction);
    m_mainToolBar->addSeparator();
    m_mainToolBar->addAction(m_undoAction);
    m_mainToolBar->addAction(m_redoAction);
}

// Статус-бар

void MainWindow::createStatusBar()
{
    m_statusLabel = new QLabel(this);
    statusBar()->addPermanentWidget(m_statusLabel);
    m_statusLabel->setText(tr("Готово"));
}

// Подключение сигналов/слотов контроллера и доски

void MainWindow::connectController()
{
    if (!m_controller)
        return;

    // Сигналы контроллера
    connect(m_controller, &GameController::gameStateChanged,
            this, &MainWindow::onGameStateChanged);

    connect(m_controller, &GameController::currentPlayerChanged,
            this, &MainWindow::onCurrentPlayerChanged);

    connect(m_controller, &GameController::moveMade,
            this, &MainWindow::onMoveMade);

    connect(m_controller, &GameController::undoAvailabilityChanged,
            this, &MainWindow::updateUndoRedoActions);
    connect(m_controller, &GameController::redoAvailabilityChanged,
            this, &MainWindow::updateUndoRedoActions);

    // Сигнал от BoardView при клике по клетке
    if (m_boardView)
    {
        connect(m_boardView, &BoardView::cellClicked,
                this, &MainWindow::onBoardCellClicked);

        // Обновление доски при изменении board
        connect(m_controller, &GameController::boardChanged,
                m_boardView,    &BoardView::refreshBoard);
    }
}

// Обновление заголовка окна

void MainWindow::updateWindowTitle()
{
    QString title = tr("Omega-шахматы");

    if (m_controller)
    {
        QString playerStr;
        switch (m_controller->currentPlayer())
        {
        case GameController::Player::White:
            playerStr = tr("Ход белых");
            break;
        case GameController::Player::Black:
            playerStr = tr("Ход чёрных");
            break;
        }

        title += QString(" — %1").arg(playerStr);
    }

    setWindowTitle(title);
}

// Обновление доступности действий Undo/Redo

void MainWindow::updateUndoRedoActions()
{
    if (!m_controller)
    {
        if (m_undoAction) m_undoAction->setEnabled(false);
        if (m_redoAction) m_redoAction->setEnabled(false);
        return;
    }

    if (m_undoAction)
        m_undoAction->setEnabled(m_controller->canUndo());
    if (m_redoAction)
        m_redoAction->setEnabled(m_controller->canRedo());
}

// СЛОТЫ МЕНЮ/КНОПОК

void MainWindow::onNewGame()
{
    if (!m_controller)
        return;

    m_controller->startNewGame();
    if (m_moveList)
        m_moveList->clear();

    m_statusLabel->setText(tr("Новая партия начата"));
    updateUndoRedoActions();
    updateWindowTitle();
}

void MainWindow::onUndo()
{
    if (!m_controller)
        return;

    m_controller->undo();
    m_statusLabel->setText(tr("Ход отменён"));
    updateUndoRedoActions();
    updateWindowTitle();
}

void MainWindow::onRedo()
{
    if (!m_controller)
        return;

    m_controller->redo();
    m_statusLabel->setText(tr("Ход повторён"));
    updateUndoRedoActions();
    updateWindowTitle();
}

void MainWindow::onExit()
{
    close();
}

void MainWindow::onAbout()
{
    QMessageBox::about(
        this,
        tr("О программе"),
        tr("Omega-шахматы\n\nПрототип приложения на Qt для варианта шахмат Omega Chess.")
    );
}

// СЛОТЫ ДЛЯ СИГНАЛОВ CONTROLLER’а

void MainWindow::onGameStateChanged(GameController::GameState state)
{
    QString text;

    switch (state)
    {
    case GameController::GameState::Running:
        text = tr("Игра продолжается");
        break;
    case GameController::GameState::Check:
        text = tr("Шах");
        break;
    case GameController::GameState::Checkmate:
        text = tr("Мат");
        break;
    case GameController::GameState::Stalemate:
        text = tr("Ничья");
        break;
    }

    m_statusLabel->setText(text);
    updateWindowTitle();
}

void MainWindow::onCurrentPlayerChanged(GameController::Player /*player*/)
{
    // Здесь достаточно обновить заголовок/статус
    updateWindowTitle();
}

// Добавление записи в список ходов

void MainWindow::onMoveMade(const Move &move)
{
    if (!m_moveList)
        return;

    QString itemText =
        QString("(%1,%2) → (%3,%4)")
            .arg(move.from.row)
            .arg(move.from.col)
            .arg(move.to.row)
            .arg(move.to.col);

    m_moveList->addItem(itemText);
    m_moveList->scrollToBottom();
}

// Обработка кликов по доске
//
// Здесь реализован простой «двухкликовый» ввод:
//  1-й клик — выбор клетки-источника
//  2-й клик — выбор клетки-назначения и попытка сделать ход.

void MainWindow::onBoardCellClicked(int row, int col)
{
    if (!m_controller)
        return;

    // Статические переменные для хранения "выбранной" клетки
    static bool hasFrom = false;
    static int  fromRow = -1;
    static int  fromCol = -1;

    if (!hasFrom)
    {
        // Первый клик: запоминаем клетку-источник
        hasFrom = true;
        fromRow = row;
        fromCol = col;

        m_statusLabel->setText(
            tr("Выбрана начальная клетка: (%1,%2)")
                .arg(fromRow)
                .arg(fromCol)
        );
    }
    else
    {
        // Второй клик: делаем попытку хода
        const Position from(fromRow, fromCol);
        const Position to(row, col);

        bool ok = m_controller->makeMove(from, to);
        if (!ok)
        {
            QMessageBox::warning(
                this,
                tr("Неверный ход"),
                tr("Этот ход не разрешён правилами.")
            );
            m_statusLabel->setText(tr("Неверный ход"));
        }
        else
        {
            m_statusLabel->setText(
                tr("Ход: (%1,%2) → (%3,%4)")
                    .arg(fromRow)
                    .arg(fromCol)
                    .arg(row)
                    .arg(col)
            );
            updateUndoRedoActions();
        }

        // Сбрасываем выбор
        hasFrom = false;
        fromRow = -1;
        fromCol = -1;
    }
}
