// tests/logic_tests.cpp

#include <cassert>
#include <iostream>

#include "Board.hpp"   // Должен объявлять Board и Piece/ PieceColor / PieceKind
// #include "GameController.hpp"   // Можно подключить позже, когда появится реализация

// Тест геометрии и валидности клеток Omega-доски
void testBoardGeometry()
{
    Board board;

    // Размер массива 12x12
    assert(Board::ROWS == 12);
    assert(Board::COLS == 12);

    // Внутренние границы массива
    assert(board.isInsideArray(0, 0));
    assert(board.isInsideArray(11, 11));
    assert(!board.isInsideArray(-1, 0));
    assert(!board.isInsideArray(0, 12));

    // Угловые клетки должны быть валидными
    assert(board.isValidCell(0, 0));
    assert(board.isValidCell(0, Board::COLS - 1));
    assert(board.isValidCell(Board::ROWS - 1, 0));
    assert(board.isValidCell(Board::ROWS - 1, Board::COLS - 1));

    // Основная область 10x10: строки/столбцы 1..10
    assert(board.isValidCell(1, 1));
    assert(board.isValidCell(10, 10));
    assert(board.isValidCell(5, 7));

    // Клетка вне основной области и не в углу — невалидна
    assert(!board.isValidCell(0, 5));
    assert(!board.isValidCell(5, 0));
    assert(!board.isValidCell(11, 5));
    assert(!board.isValidCell(5, 11));

    // Вне массива — тоже невалидно
    assert(!board.isValidCell(-1, -1));
    assert(!board.isValidCell(12, 12));

    std::cout << "[OK] testBoardGeometry\n";
}

// Тест базовых операций с клетками: очистка, установка, чтение
void testBoardCells()
{
    Board board;

    // После resetToInitialPosition что-то может стоять, поэтому сначала очистим всё
    board.clear();

    // Проверяем, что валидная клетка становится пустой
    int r = 5;
    int c = 5;
    assert(board.isInsideArray(r, c));
    assert(board.isValidCell(r, c));

    board.clearCell(r, c);
    assert(board.isEmpty(r, c));

    // Установка фигуры
    Piece whitePawn;
    whitePawn.color    = PieceColor::White;
    whitePawn.kind     = PieceKind::Pawn;
    whitePawn.hasMoved = false;

    board.setPieceAt(r, c, whitePawn);

    const Piece &p = board.pieceAt(r, c);
    assert(!p.isEmpty());
    assert(p.color == PieceColor::White);
    assert(p.kind  == PieceKind::Pawn);
    assert(p.hasMoved == false);

    // Снова очистка клетки
    board.clearCell(r, c);
    assert(board.isEmpty(r, c));

    std::cout << "[OK] testBoardCells\n";
}

// Тест начальной позиции (скелет).
// Имеет смысл включить, когда вы реализуете Board::setupInitialPieces().
void testInitialPosition_skeleton()
{
    Board board;
    board.resetToInitialPosition();

    // Пример: ожидаем, что угловые клетки пока пусты
    // (реальные проверки зависят от того, как вы определите начальную расстановку Omega-шахмат)
    //
    // assert(board.isEmpty(0, 0));
    // assert(board.isEmpty(0, Board::COLS - 1));
    // ...

    // Временно просто проверим, что resetToInitialPosition не ломает геометрию
    assert(board.isValidCell(1, 1));
    assert(board.isValidCell(10, 10));

    std::cout << "[OK] testInitialPosition_skeleton (минимальная проверка)\n";
}

int main()
{
    std::cout << "Запуск логических тестов Omega Chess...\n";

    testBoardGeometry();
    testBoardCells();
    testInitialPosition_skeleton();

    std::cout << "Все логические тесты успешно пройдены.\n";
    return 0;
}
