#pragma once

#include "Piece.hpp"

/**
 * Модель доски Омега-шахмат.
 *
 * Внутреннее представление: массив 12x12.
 * Валидные клетки:
 *  - 10x10: row = 1..10, col = 1..10
 *  - углы: (0,0), (0,11), (11,0), (11,11)
 */
class Board
{
public:
    static constexpr int ROWS = 12;
    static constexpr int COLS = 12;

    Board();
    ~Board() = default;

    void resetToInitialPosition();

    bool isInsideArray(int row, int col) const noexcept;
    bool isValidCell(int row, int col) const noexcept;

    const Piece& pieceAt(int row, int col) const;
    Piece&       pieceAt(int row, int col);

    void setPieceAt(int row, int col, const Piece& piece);
    void clearCell(int row, int col);
    bool isEmpty(int row, int col) const;
    void clear();

private:
    Piece m_cells[ROWS][COLS];

    void setupInitialPieces();
};
