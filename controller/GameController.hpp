#pragma once

#include <QObject>
#include <vector>
#include <cstddef>

class Board;

/// Простейшая координата на доске
struct Position
{
    int row = 0;
    int col = 0;

    Position() = default;
    Position(int r, int c) : row(r), col(c) {}
};

/// Описание хода: из клетки в клетку
struct Move
{
    Position from;
    Position to;
};

class GameController : public QObject
{
    Q_OBJECT

public:
    enum class Player {
        White,
        Black
    };
    Q_ENUM(Player)

    enum class GameState {
        Running,
        Check,
        Checkmate,
        Stalemate
    };
    Q_ENUM(GameState)

    explicit GameController(QObject *parent = nullptr);
    ~GameController();

    void startNewGame();
    void resetToInitialPosition();

    bool makeMove(const Position &from, const Position &to);
    bool makeMove(const Move &move);

    Player     currentPlayer() const noexcept;
    GameState  gameState()     const noexcept;
    const Board& board()       const;

    bool canUndo() const noexcept;
    bool canRedo() const noexcept;

public slots:
    void undo();
    void redo();

signals:
    void boardChanged();
    void currentPlayerChanged(GameController::Player player);
    void gameStateChanged(GameController::GameState state);
    void moveMade(const Move &move);
    void undoAvailabilityChanged(bool canUndo);
    void redoAvailabilityChanged(bool canRedo);

private:
    // Служебные методы
    void switchPlayer();
    void updateGameState();
    void notifyHistoryChanged();

    // Применить ход к доске (без истории, без смены игрока)
    bool applyMoveOnBoard(const Move &move);

    // --- НОВОЕ: логика шаха ---
    bool isKingInCheck(Player side) const;                   // король side под ударом?
    bool isSquareAttacked(int row, int col, Player bySide) const; // клетка под атакой стороны bySide?

private:
    Board *m_board = nullptr;

    Player    m_currentPlayer = Player::White;
    GameState m_gameState     = GameState::Running;

    std::vector<Move> m_history;
    std::size_t       m_historyIndex = 0;
};
