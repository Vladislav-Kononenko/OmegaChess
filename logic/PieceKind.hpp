#pragma once
#include <string>

/**
 * Класс-обёртка для типа фигуры Omega Chess.
 *
 * Не просто enum, а полноценный тип с удобными методами:
 * - isKing(), isPawn(), isChampion() и т.д.
 * - toString() — человекочитаемое имя
 * - toChar()   — символ ('K', 'Q', 'C', 'W', …)
 */
class PieceKind
{
public:
    enum class Value : unsigned char
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

    PieceKind() : m_value(Value::None) {}
    PieceKind(Value v) : m_value(v) {}

    /// Фабрики
    static PieceKind none()     { return PieceKind(Value::None);     }
    static PieceKind king()     { return PieceKind(Value::King);     }
    static PieceKind queen()    { return PieceKind(Value::Queen);    }
    static PieceKind rook()     { return PieceKind(Value::Rook);     }
    static PieceKind bishop()   { return PieceKind(Value::Bishop);   }
    static PieceKind knight()   { return PieceKind(Value::Knight);   }
    static PieceKind pawn()     { return PieceKind(Value::Pawn);     }
    static PieceKind champion() { return PieceKind(Value::Champion); }
    static PieceKind wizard()   { return PieceKind(Value::Wizard);   }

    /// Проверки типа
    bool isNone()     const { return m_value == Value::None;     }
    bool isKing()     const { return m_value == Value::King;     }
    bool isQueen()    const { return m_value == Value::Queen;    }
    bool isRook()     const { return m_value == Value::Rook;     }
    bool isBishop()   const { return m_value == Value::Bishop;   }
    bool isKnight()   const { return m_value == Value::Knight;   }
    bool isPawn()     const { return m_value == Value::Pawn;     }
    bool isChampion() const { return m_value == Value::Champion; }
    bool isWizard()   const { return m_value == Value::Wizard;   }

    /// Имя типа в виде строки
    std::string toString() const
    {
        switch (m_value)
        {
            case Value::King:     return "King";
            case Value::Queen:    return "Queen";
            case Value::Rook:     return "Rook";
            case Value::Bishop:   return "Bishop";
            case Value::Knight:   return "Knight";
            case Value::Pawn:     return "Pawn";
            case Value::Champion: return "Champion";
            case Value::Wizard:   return "Wizard";
            default:              return "None";
        }
    }

    /// Символ для отображения (используется в BoardView)
    char toChar() const
    {
        switch (m_value)
        {
            case Value::King:     return 'K';
            case Value::Queen:    return 'Q';
            case Value::Rook:     return 'R';
            case Value::Bishop:   return 'B';
            case Value::Knight:   return 'N';
            case Value::Pawn:     return 'P';
            case Value::Champion: return 'C';
            case Value::Wizard:   return 'W';
            default:              return ' ';
        }
    }

    /// Получить внутреннее enum-значение
    Value value() const { return m_value; }

    /// Сравнение
    bool operator==(const PieceKind &other) const noexcept { return m_value == other.m_value; }
    bool operator!=(const PieceKind &other) const noexcept { return m_value != other.m_value; }

private:
    Value m_value;
};
