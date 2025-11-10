#include "GameController.hpp"

#include "../logic/Board.hpp"
#include "../logic/Piece.hpp"

#include <algorithm>

// Вспомогательный внутренний метод: применить ход к доске БЕЗ
// изменения истории, смены игрока и т.п.
// Здесь пока нет полной проверки легальности по правилам Omega-шахмат –
// только базовые проверки: валидность клеток, наличие фигуры,
// правильный цвет, запрет рубить свои фигуры.
bool GameController::applyMoveOnBoard(const Move &move)
{
    if (!m_board)
        return false;

    Board &board = *m_board;

    const int fromRow = move.from.row;
    const int fromCol = move.from.col;
    const int toRow   = move.to.row;
    const int toCol   = move.to.col;

    // Координаты в пределах массива
    if (!board.isInsideArray(fromRow, fromCol) ||
        !board.isInsideArray(toRow, toCol))
    {
        return false;
    }

    // Клетки должны быть валидными для геометрии Omega-доски
    if (!board.isValidCell(fromRow, fromCol) ||
        !board.isValidCell(toRow, toCol))
    {
        return false;
    }

    Piece fromPiece = board.pieceAt(fromRow, fromCol);
    if (fromPiece.isEmpty())
    {
        // С пустой клетки ходить нельзя
        return false;
    }

    // Ходить можно только своей фигурой
    if ((m_currentPlayer == Player::White && fromPiece.color != PieceColor::White) ||
        (m_currentPlayer == Player::Black && fromPiece.color != PieceColor::Black))
    {
        return false;
    }

    Piece toPiece = board.pieceAt(toRow, toCol);

    // Нельзя рубить свою фигуру
    if (!toPiece.isEmpty() && toPiece.color == fromPiece.color)
    {
        return false;
    }

    // >>> НОВОЕ ПРАВИЛО: короля бить нельзя
    if (!toPiece.isEmpty() && toPiece.kind == PieceKind::King)
    {
        // В Omega/шахматах король не снимается с доски, партия
        // должна заканчиваться матом, а не взятием.
        return false;
    }

    // TODO: сюда позже добавим полную проверку легальности хода
    // (по типу фигуры, шах/само-шах, рокировка и т.п.).

    // Перемещаем фигуру
    fromPiece.hasMoved = true;
    board.setPieceAt(toRow, toCol, fromPiece);
    board.clearCell(fromRow, fromCol);

    return true;
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
    // Начальное состояние
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

    // Сбрасываем фигуры и состояние
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
    // По сути то же самое, что startNewGame(), но без изменения истории
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

    // Сначала пробуем применить ход к текущему состоянию доски (без учёта истории)
    if (!applyMoveOnBoard(move))
        return false;

    // Ход оказался допустим (на уровне текущих проверок),
    // теперь обновляем историю и состояние контроллера.

    // Если до этого были сделаны undo, нужно обрезать "хвост" истории
    if (m_historyIndex < m_history.size())
    {
        m_history.erase(m_history.begin() + static_cast<std::ptrdiff_t>(m_historyIndex),
                        m_history.end());
    }

    m_history.push_back(move);
    ++m_historyIndex;

    // Смена игрока
    switchPlayer();

    // Пересчёт состояния (пока stub)
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
    static Board dummy; // на случай, если m_board == nullptr (не должен случаться)
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
// Слоты undo / redo
// ---------------------------------------------------------------------

void GameController::undo()
{
    if (!m_board || !canUndo())
        return;

    // Один шаг назад в истории
    --m_historyIndex;

    // Восстанавливаем доску: берём начальную позицию и
    // последовательно накатываем первые m_historyIndex ходов.

    m_board->resetToInitialPosition();
    m_currentPlayer = Player::White;   // в начале всегда ход белых

    for (std::size_t i = 0; i < m_historyIndex; ++i)
    {
        const Move &mv = m_history[i];
        // При "переигрывании" ходов мы не хотим снова менять историю,
        // поэтому вызываем только низкоуровневый метод.
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

    // Применяем следующий ход в истории к доске
    const Move &mv = m_history[m_historyIndex];

    if (!applyMoveOnBoard(mv))
    {
        // Если вдруг не удалось (не должно происходить при корректной истории),
        // просто выходим.
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
// Внутренние методы
// ---------------------------------------------------------------------

void GameController::switchPlayer()
{
    m_currentPlayer = (m_currentPlayer == Player::White)
                        ? Player::Black
                        : Player::White;
}

void GameController::updateGameState()
{
    // TODO: Реализовать анализ позиции:
    //  - находит ли король текущего игрока под шахом;
    //  - есть ли допустимые ходы;
    //  - различать состояния: Running / Check / Checkmate / Stalemate.
    //
    // Пока просто считаем, что игра идёт без шахов и матов.
    m_gameState = GameState::Running;

    emit gameStateChanged(m_gameState);
}

void GameController::notifyHistoryChanged()
{
    emit undoAvailabilityChanged(canUndo());
    emit redoAvailabilityChanged(canRedo());
}
