#include "Board.hpp"
#include <stdexcept>

Board::Board()
{
    clear();
}

/**
 * Очистить всю доску (все клетки — пустые).
 */
void Board::clear()
{
    for (int r = 0; r < ROWS; ++r)
    {
        for (int c = 0; c < COLS; ++c)
        {
            m_cells[r][c] = Piece::empty();
        }
    }
}

/**
 * Проверка выхода за границы массива 12×12.
 */
bool Board::isInsideArray(int row, int col) const noexcept
{
    return row >= 0 && row < ROWS &&
           col >= 0 && col < COLS;
}

/**
 * Валидные клетки Omega-доски:
 *  - основное поле 10×10: row = 1..10, col = 1..10
 *  - 4 угла: (0,0), (0,11), (11,0), (11,11)
 */
bool Board::isValidCell(int row, int col) const noexcept
{
    if (!isInsideArray(row, col))
        return false;

    // основное поле 10×10
    if (row >= 1 && row <= 10 &&
        col >= 1 && col <= 10)
    {
        return true;
    }

    // четыре угловые клетки
    if ((row == 0 || row == ROWS - 1) &&
        (col == 0 || col == COLS - 1))
    {
        return true;
    }

    return false;
}

const Piece& Board::pieceAt(int row, int col) const
{
    if (!isInsideArray(row, col))
        throw std::out_of_range("Board::pieceAt: index out of range");
    return m_cells[row][col];
}

Piece& Board::pieceAt(int row, int col)
{
    if (!isInsideArray(row, col))
        throw std::out_of_range("Board::pieceAt: index out of range");
    return m_cells[row][col];
}

void Board::setPieceAt(int row, int col, const Piece& piece)
{
    if (!isInsideArray(row, col))
        throw std::out_of_range("Board::setPieceAt: index out of range");
    m_cells[row][col] = piece;
}

void Board::clearCell(int row, int col)
{
    if (!isInsideArray(row, col))
        return;
    m_cells[row][col] = Piece::empty();
}

bool Board::isEmpty(int row, int col) const
{
    if (!isInsideArray(row, col))
        return true;
    return m_cells[row][col].isEmpty();
}

/**
 * Сброс к начальной позиции Omega Chess.
 */
void Board::resetToInitialPosition()
{
    clear();
    setupInitialPieces();
}

/**
 * Начальная расстановка Omega Chess.
 *
 * Нумерация:
 *  - строки 0..11 сверху вниз;
 *  - столбцы 0..11 слева направо.
 *
 * Основное поле 10×10:
 *  - строки 1..10
 *  - столбцы 1..10
 *
 * Белые внизу:
 *  - главная линия белых: row = 10
 *  - пешки белых:         row = 9
 *
 * Чёрные сверху:
 *  - главная линия чёрных: row = 1
 *  - пешки чёрных:          row = 2
 *
 * Чемпионы стоят в углах 10×10.
 * Волшебники (Wizards) — на диагональных углах за чемпионами.
 */
void Board::setupInitialPieces()
{
    // ---------- ЧЁРНЫЕ ФИГУРЫ (верх) ----------

    const int blackBackRank = 1; // a0..j0 в терминах Omega — у нас row=1
    const int blackPawnsRow = 2;

    // Порядок фигур на основной линии:
    // Champion, Rook, Knight, Bishop, Queen, King, Bishop, Knight, Rook, Champion
    Piece blackChampion = Piece{PieceColor::Black, PieceKind::Champion, false};
    Piece blackRook     = Piece{PieceColor::Black, PieceKind::Rook,     false};
    Piece blackKnight   = Piece{PieceColor::Black, PieceKind::Knight,   false};
    Piece blackBishop   = Piece{PieceColor::Black, PieceKind::Bishop,   false};
    Piece blackQueen    = Piece{PieceColor::Black, PieceKind::Queen,    false};
    Piece blackKing     = Piece{PieceColor::Black, PieceKind::King,     false};
    Piece blackPawn     = Piece{PieceColor::Black, PieceKind::Pawn,     false};
    Piece blackWizard   = Piece{PieceColor::Black, PieceKind::Wizard,   false};

    // Столбцы 1..10
    setPieceAt(blackBackRank,  1, blackChampion);
    setPieceAt(blackBackRank,  2, blackRook);
    setPieceAt(blackBackRank,  3, blackKnight);
    setPieceAt(blackBackRank,  4, blackBishop);
    setPieceAt(blackBackRank,  5, blackQueen);
    setPieceAt(blackBackRank,  6, blackKing);
    setPieceAt(blackBackRank,  7, blackBishop);
    setPieceAt(blackBackRank,  8, blackKnight);
    setPieceAt(blackBackRank,  9, blackRook);
    setPieceAt(blackBackRank, 10, blackChampion);

    // Пешки чёрных (10 штук)
    for (int col = 1; col <= 10; ++col)
    {
        setPieceAt(blackPawnsRow, col, blackPawn);
    }

    // Волшебники чёрных — на угловых клетках за чемпионами:
    // диагональные углы относительно (1,1) и (1,10)
    setPieceAt(0, 0, blackWizard);          // за левым верхним чемпионом
    setPieceAt(0, COLS - 1, blackWizard);   // за правым верхним чемпионом



    // ---------- БЕЛЫЕ ФИГУРЫ (низ) ----------

    const int whiteBackRank = 10;
    const int whitePawnsRow = 9;

    Piece whiteChampion = Piece{PieceColor::White, PieceKind::Champion, false};
    Piece whiteRook     = Piece{PieceColor::White, PieceKind::Rook,     false};
    Piece whiteKnight   = Piece{PieceColor::White, PieceKind::Knight,   false};
    Piece whiteBishop   = Piece{PieceColor::White, PieceKind::Bishop,   false};
    Piece whiteQueen    = Piece{PieceColor::White, PieceKind::Queen,    false};
    Piece whiteKing     = Piece{PieceColor::White, PieceKind::King,     false};
    Piece whitePawn     = Piece{PieceColor::White, PieceKind::Pawn,     false};
    Piece whiteWizard   = Piece{PieceColor::White, PieceKind::Wizard,   false};

    setPieceAt(whiteBackRank,  1, whiteChampion);
    setPieceAt(whiteBackRank,  2, whiteRook);
    setPieceAt(whiteBackRank,  3, whiteKnight);
    setPieceAt(whiteBackRank,  4, whiteBishop);
    setPieceAt(whiteBackRank,  5, whiteQueen);
    setPieceAt(whiteBackRank,  6, whiteKing);
    setPieceAt(whiteBackRank,  7, whiteBishop);
    setPieceAt(whiteBackRank,  8, whiteKnight);
    setPieceAt(whiteBackRank,  9, whiteRook);
    setPieceAt(whiteBackRank, 10, whiteChampion);

    for (int col = 1; col <= 10; ++col)
    {
        setPieceAt(whitePawnsRow, col, whitePawn);
    }

    // Волшебники белых — углы снизу
    setPieceAt(ROWS - 1, 0,         whiteWizard); // (11,0)
    setPieceAt(ROWS - 1, COLS - 1,  whiteWizard); // (11,11)
}
