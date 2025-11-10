#pragma once
#include <string>

/**
 * Класс-обёртка для цвета фигуры.
 * Не enum — полноценный тип с удобными методами.
 */
class PieceColor
{
public:
    enum class Value : unsigned char
    {
        None  = 0,
        White = 1,
        Black = 2
    };

    PieceColor() : m_value(Value::None) {}
    PieceColor(Value v) : m_value(v) {}

    /// Фабрики
    static PieceColor none()  { return PieceColor(Value::None);  }
    static PieceColor white() { return PieceColor(Value::White); }
    static PieceColor black() { return PieceColor(Value::Black); }

    /// Проверки
    bool isNone()  const { return m_value == Value::None;  }
    bool isWhite() const { return m_value == Value::White; }
    bool isBlack() const { return m_value == Value::Black; }

    /// Получить строковое имя цвета
    std::string toString() const
    {
        switch (m_value)
        {
            case Value::White: return "White";
            case Value::Black: return "Black";
            default:           return "None";
        }
    }

    /// Доступ к enum-значению
    Value value() const { return m_value; }

    /// Сравнение
    bool operator==(const PieceColor &other) const noexcept { return m_value == other.m_value; }
    bool operator!=(const PieceColor &other) const noexcept { return m_value != other.m_value; }

private:
    Value m_value;
};

