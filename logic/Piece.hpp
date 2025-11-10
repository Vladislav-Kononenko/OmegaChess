#pragma once

#include <cstdint>

enum class PieceColor : std::uint8_t
{
    None  = 0,
    White = 1,
    Black = 2
};

enum class PieceKind : std::uint8_t
{
    None      = 0,
    King      = 1,
    Queen     = 2,
    Rook      = 3,
    Bishop    = 4,
    Knight    = 5,
    Pawn      = 6,
    Champion  = 7,
    Wizard    = 8
};

struct Piece
{
    PieceColor color    = PieceColor::None;
    PieceKind  kind     = PieceKind::None;
    bool       hasMoved = false;

    bool isEmpty() const noexcept
    {
        return color == PieceColor::None || kind == PieceKind::None;
    }

    static Piece empty() noexcept
    {
        return Piece{};
    }
};
