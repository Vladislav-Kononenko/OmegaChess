#include "GameController.hpp"

#include "../logic/Board.hpp"
#include "../logic/Piece.hpp"

#include <algorithm>

// Вспомогательная функция: цвет по игроку
static PieceColor colorOf(GameController::Player p)
{
    return (p == GameController::Player::White)
           ? PieceColor::White
           : PieceColor::Black;
}

// Вспомогательная функция: противоположный игрок
static GameController::Player opposite(GameController::Player p)
{
    return (p == GameController::Player::White)
           ? GameController::Player::Black
           : GameController::Player::White;
}

// Проверка: фигура p с клетки (pr,pc) атакует ли клетку (tr,tc)?
// Используем реальные шаблоны ходов Omega Chess (король, ферзь, ладья, слон, конь, пешка,
// чемпион, волшебник). Для пешек учитываем только шаблон взятия.
static bool pieceAttacksSquare(const Board &board,
                               const Piece &p,
                               int pr, int pc,
                               int tr, int tc)
{
    if (p.isEmpty())
        return false;

    const int dr = tr - pr;
    const int dc = tc - pc;

    auto sameSign = [](int x, int y) {
        return (x == 0 || y == 0) ? false : ( (x > 0) == (y > 0) );
    };

    // ---------------- ПЕШКА ----------------
    if (p.kind == PieceKind::Pawn)
    {
        if (p.color == PieceColor::White)
        {
            // Белые бьют вверх (в сторону меньших row)
            return (dr == -1 && (dc == -1 || dc == 1));
        }
        else if (p.color == PieceColor::Black)
        {
            // Чёрные бьют вниз (в сторону больших row)
            return (dr == 1 && (dc == -1 || dc == 1));
        }
        return false;
    }

    // ---------------- КОНЬ ----------------
    if (p.kind == PieceKind::Knight)
    {
        const int adr = std::abs(dr);
        const int adc = std::abs(dc);
        return (adr == 1 && adc == 2) || (adr == 2 && adc == 1);
    }

    // ---------------- КОРОЛЬ ----------------
    if (p.kind == PieceKind::King)
    {
        return std::max(std::abs(dr), std::abs(dc)) == 1;
    }

    // ---------------- ЛАДЬЯ / ФЕРЗЬ (по прямым) ----------------
    auto rookLikeAttacks = [&](void) -> bool
    {
        if (dr != 0 && dc != 0)
            return false;

        int stepR = (dr == 0) ? 0 : (dr > 0 ? 1 : -1);
        int stepC = (dc == 0) ? 0 : (dc > 0 ? 1 : -1);

        int r = pr + stepR;
        int c = pc + stepC;
        while (board.isInsideArray(r, c) && board.isValidCell(r, c))
        {
            if (r == tr && c == tc)
                return true;

            if (!board.isEmpty(r, c))
                break;

            r += stepR;
            c += stepC;
        }
        return false;
    };

    // ---------------- СЛОН / ФЕРЗЬ (по диагоналям) ----------------
    auto bishopLikeAttacks = [&](void) -> bool
    {
        if (std::abs(dr) != std::abs(dc) || dr == 0)
            return false;

        int stepR = (dr > 0 ? 1 : -1);
        int stepC = (dc > 0 ? 1 : -1);

        int r = pr + stepR;
        int c = pc + stepC;
        while (board.isInsideArray(r, c) && board.isValidCell(r, c))
        {
            if (r == tr && c == tc)
                return true;

            if (!board.isEmpty(r, c))
                break;

            r += stepR;
            c += stepC;
        }
        return false;
    };

    // ---------------- CHAMPION ----------------
    // Champion: может прыгать на 2 клетки по прямым или диагоналям, либо
    // шагнуть на 1 клетку по прямой (WAD в Betza нотации).
    if (p.kind == PieceKind::Champion)
    {
        const int adr = std::abs(dr);
        const int adc = std::abs(dc);

        // Одноклеточный ход по вертикали/горизонтали
        if ((adr == 1 && adc == 0) || (adr == 0 && adc == 1))
            return true;

        // Двухклеточный прыжок по вертикали/горизонтали
        if ((adr == 2 && adc == 0) || (adr == 0 && adc == 2))
            return true;

        // Двухклеточный прыжок по диагонали
        if (adr == 2 && adc == 2)
            return true;

        return false;
    }

    // ---------------- WIZARD ----------------
    // Wizard: может ходить на 1 по диагонали, либо «растянутый конь»:
    // (1,3) или (3,1) в любом направлении. Все ходы — прыжки.
    if (p.kind == PieceKind::Wizard)
    {
        const int adr = std::abs(dr);
        const int adc = std::abs(dc);

        // Один шаг по диагонали
        if (adr == 1 && adc == 1)
            return true;

        // «верблюдовый» скачок 1x3
        if ((adr == 1 && adc == 3) || (adr == 3 && adc == 1))
            return true;

        return false;
    }

    // ---------------- ЛАДЬЯ ----------------
    if (p.kind == PieceKind::Rook)
    {
        return rookLikeAttacks();
    }

    // ---------------- СЛОН ----------------
    if (p.kind == PieceKind::Bishop)
    {
        return bishopLikeAttacks();
    }

    // ---------------- ФЕРЗЬ ----------------
    if (p.kind == PieceKind::Queen)
    {
        return rookLikeAttacks() || bishopLikeAttacks();
    }

    return false;
}

// ---------------------------------------------------------------------
// Конструктор / деструктор
// ---------------------------------------------------------------------

GameController::GameController(QObject *parent)
    : QObject(parent)
    , m_board(new Board)
    , m_currentPlayer(Player::White)
    , m_gameState(GameState::Running)
    , m_history()
    , m_historyIndex(0)
{
    startNewGame();
}

GameController::~GameController()
{
    delete m_board;
    m_board = nullptr;
}

// ---------------------------------------------------------------------
// Публичный интерфейс
// ---------------------------------------------------------------------

void GameController::startNewGame()
{
    if (!m_board)
        m_board = new Board;

    m_board->resetToInitialPosition();
    m_currentPlayer = Player::White;
    m_gameState     = GameState::Running;

    m_history.clear();
    m_historyIndex = 0;

    emit boardChanged();
    emit currentPlayerChanged(m_currentPlayer);
    emit gameStateChanged(m_gameState);
    notifyHistoryChanged();
}

void GameController::resetToInitialPosition()
{
    if (!m_board)
        return;

    m_board->resetToInitialPosition();
    m_currentPlayer = Player::White;
    m_gameState     = GameState::Running;

    emit boardChanged();
    emit currentPlayerChanged(m_currentPlayer);
    emit gameStateChanged(m_gameState);
}

bool GameController::makeMove(const Position &from, const Position &to)
{
    Move move;
    move.from = from;
    move.to   = to;
    return makeMove(move);
}

bool GameController::makeMove(const Move &move)
{
    if (!m_board)
        return false;

    // Сохраняем состояние доски перед попыткой хода
    Board  oldBoard       = *m_board;
    Player movingSide     = m_currentPlayer;

    // Пробуем применить ход к доске (проверка границ, своих/чужих фигур и т.п.)
    if (!applyMoveOnBoard(move))
        return false;

    // НОВОЕ: ход не должен оставлять собственного короля под ударом
    if (isKingInCheck(movingSide))
    {
        // Откат доски
        *m_board = oldBoard;
        return false;
    }

    // Если до этого были сделаны undo, обрежем «хвост» истории
    if (m_historyIndex < m_history.size())
    {
        m_history.erase(m_history.begin() + static_cast<std::ptrdiff_t>(m_historyIndex),
                        m_history.end());
    }

    m_history.push_back(move);
    ++m_historyIndex;

    // Переход хода к сопернику
    switchPlayer();

    // Обновляем состояние игры (проверяем, не находится ли теперь соперник под шахом)
    updateGameState();

    emit moveMade(move);
    emit boardChanged();
    notifyHistoryChanged();
    emit currentPlayerChanged(m_currentPlayer);

    return true;
}

GameController::Player GameController::currentPlayer() const noexcept
{
    return m_currentPlayer;
}

GameController::GameState GameController::gameState() const noexcept
{
    return m_gameState;
}

const Board &GameController::board() const
{
    static Board dummy;
    return m_board ? *m_board : dummy;
}

bool GameController::canUndo() const noexcept
{
    return m_historyIndex > 0;
}

bool GameController::canRedo() const noexcept
{
    return m_historyIndex < m_history.size();
}

// ---------------------------------------------------------------------
// Undo / Redo
// ---------------------------------------------------------------------

void GameController::undo()
{
    if (!m_board || !canUndo())
        return;

    // Один шаг назад
    --m_historyIndex;

    // Восстанавливаем позицию: с нуля применяем первые m_historyIndex ходов
    m_board->resetToInitialPosition();
    m_currentPlayer = Player::White;

    for (std::size_t i = 0; i < m_historyIndex; ++i)
    {
        const Move &mv = m_history[i];
        applyMoveOnBoard(mv);
        switchPlayer();
    }

    updateGameState();

    emit boardChanged();
    notifyHistoryChanged();
    emit currentPlayerChanged(m_currentPlayer);
}

void GameController::redo()
{
    if (!m_board || !canRedo())
        return;

    const Move &mv = m_history[m_historyIndex];

    if (!applyMoveOnBoard(mv))
    {
        return;
    }

    ++m_historyIndex;
    switchPlayer();
    updateGameState();

    emit moveMade(mv);
    emit boardChanged();
    notifyHistoryChanged();
    emit currentPlayerChanged(m_currentPlayer);
}

// ---------------------------------------------------------------------
// Низкоуровневое применение хода
// ---------------------------------------------------------------------

bool GameController::applyMoveOnBoard(const Move &move)
{
    if (!m_board)
        return false;

    Board &board = *m_board;

    const int fromRow = move.from.row;
    const int fromCol = move.from.col;
    const int toRow   = move.to.row;
    const int toCol   = move.to.col;

    if (!board.isInsideArray(fromRow, fromCol) ||
        !board.isInsideArray(toRow, toCol))
    {
        return false;
    }

    if (!board.isValidCell(fromRow, fromCol) ||
        !board.isValidCell(toRow, toCol))
    {
        return false;
    }

    Piece fromPiece = board.pieceAt(fromRow, fromCol);
    if (fromPiece.isEmpty())
    {
        return false;
    }

    // Ходим только своей фигурой
    if ((m_currentPlayer == Player::White && fromPiece.color != PieceColor::White) ||
        (m_currentPlayer == Player::Black && fromPiece.color != PieceColor::Black))
    {
        return false;
    }

    Piece toPiece = board.pieceAt(toRow, toCol);

    // Нельзя бить свою фигуру
    if (!toPiece.isEmpty() && toPiece.color == fromPiece.color)
    {
        return false;
    }

    // Нельзя «снимать» короля противника
    if (!toPiece.isEmpty() && toPiece.kind == PieceKind::King)
    {
        return false;
    }

    // TODO: здесь можно добавить полную проверку допустимости хода
    // по типу фигуры (в данный момент допускается любой "псевдолегальный" ход).

    fromPiece.hasMoved = true;
    board.setPieceAt(toRow, toCol, fromPiece);
    board.clearCell(fromRow, fromCol);

    return true;
}

// ---------------------------------------------------------------------
// Логика шаха
// ---------------------------------------------------------------------

bool GameController::isKingInCheck(Player side) const
{
    if (!m_board)
        return false;

    const Board &board = *m_board;
    const PieceColor myColor   = colorOf(side);
    const Player     enemySide = opposite(side);

    int kingRow = -1;
    int kingCol = -1;

    // Ищем короля данной стороны
    for (int r = 0; r < Board::ROWS; ++r)
    {
        for (int c = 0; c < Board::COLS; ++c)
        {
            if (!board.isValidCell(r, c))
                continue;

            const Piece &p = board.pieceAt(r, c);
            if (p.isEmpty())
                continue;

            if (p.kind == PieceKind::King && p.color == myColor)
            {
                kingRow = r;
                kingCol = c;
                break;
            }
        }
        if (kingRow != -1)
            break;
    }

    if (kingRow == -1)
    {
        // Теоретически этого быть не должно (мы запрещаем взятие короля),
        // но на всякий случай считаем, что король "под бесконечным шахом".
        return true;
    }

    return isSquareAttacked(kingRow, kingCol, enemySide);
}

bool GameController::isSquareAttacked(int row, int col, Player bySide) const
{
    if (!m_board)
        return false;

    const Board &board = *m_board;
    const PieceColor attackColor = colorOf(bySide);

    for (int r = 0; r < Board::ROWS; ++r)
    {
        for (int c = 0; c < Board::COLS; ++c)
        {
            if (!board.isValidCell(r, c))
                continue;

            const Piece &p = board.pieceAt(r, c);
            if (p.isEmpty() || p.color != attackColor)
                continue;

            if (pieceAttacksSquare(board, p, r, c, row, col))
                return true;
        }
    }

    return false;
}

// ---------------------------------------------------------------------
// Внутренние служебные методы
// ---------------------------------------------------------------------

void GameController::switchPlayer()
{
    m_currentPlayer = opposite(m_currentPlayer);
}

void GameController::updateGameState()
{
    if (!m_board)
    {
        m_gameState = GameState::Running;
        emit gameStateChanged(m_gameState);
        return;
    }

    // Здесь трактуем gameState так:
    //  - Running: король стороны, которой сейчас ходить, НЕ под шахом
    //  - Check:   король стороны, которой сейчас ходить, ПОД шахом
    //
    // Мат/пат пока не определяем (это потребует полного генератора ходов).

    if (isKingInCheck(m_currentPlayer))
    {
        m_gameState = GameState::Check;
    }
    else
    {
        m_gameState = GameState::Running;
    }

    emit gameStateChanged(m_gameState);
}

void GameController::notifyHistoryChanged()
{
    emit undoAvailabilityChanged(canUndo());
    emit redoAvailabilityChanged(canRedo());
}
