#include "BoardView.hpp"

#include <QPainter>
#include <QMouseEvent>
#include <QFontMetrics>

BoardView::BoardView(GameController *controller, QWidget *parent)
    : QWidget(parent)
    , m_controller(controller)
{
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);
}

void BoardView::setController(GameController *controller) noexcept
{
    if (m_controller == controller)
        return;

    m_controller = controller;
    update();
}

QSize BoardView::sizeHint() const
{
    const int cell = 40;
    return QSize(Board::COLS * cell, Board::ROWS * cell);
}

QSize BoardView::minimumSizeHint() const
{
    const int cell = 24;
    return QSize(Board::COLS * cell, Board::ROWS * cell);
}

void BoardView::refreshBoard()
{
    update();
}

void BoardView::computeGeometry(int &cellSize, int &offsetX, int &offsetY) const
{
    const int w = width();
    const int h = height();

    if (w <= 0 || h <= 0)
    {
        cellSize = 0;
        offsetX  = 0;
        offsetY  = 0;
        return;
    }

    const int cellW = w / Board::COLS;
    const int cellH = h / Board::ROWS;
    cellSize = qMin(cellW, cellH);

    const int boardWidth  = cellSize * Board::COLS;
    const int boardHeight = cellSize * Board::ROWS;

    offsetX = (w - boardWidth) / 2;
    offsetY = (h - boardHeight) / 2;

    m_cachedCellSize = cellSize;
    m_cachedOffsetX  = offsetX;
    m_cachedOffsetY  = offsetY;
}

bool BoardView::mapPointToCell(const QPoint &pt, int &row, int &col) const
{
    if (m_cachedCellSize <= 0)
        return false;

    const int cellSize = m_cachedCellSize;
    const int offsetX  = m_cachedOffsetX;
    const int offsetY  = m_cachedOffsetY;

    const int x = pt.x() - offsetX;
    const int y = pt.y() - offsetY;

    if (x < 0 || y < 0)
        return false;

    col = x / cellSize;
    row = y / cellSize;

    if (row < 0 || row >= Board::ROWS ||
        col < 0 || col >= Board::COLS)
    {
        return false;
    }

    if (!m_controller)
        return false;

    const Board &board = m_controller->board();
    if (!board.isValidCell(row, col))
        return false;

    return true;
}

QRect BoardView::cellRect(int row, int col) const
{
    const int cellSize = m_cachedCellSize;
    const int offsetX  = m_cachedOffsetX;
    const int offsetY  = m_cachedOffsetY;

    return QRect(
        offsetX + col * cellSize,
        offsetY + row * cellSize,
        cellSize,
        cellSize
    );
}

void BoardView::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    int cellSize = 0;
    int offsetX  = 0;
    int offsetY  = 0;
    computeGeometry(cellSize, offsetX, offsetY);

    painter.fillRect(rect(), palette().window());

    if (!m_controller || cellSize <= 0)
        return;

    drawBoard(painter);
    drawPieces(painter);
}

void BoardView::mousePressEvent(QMouseEvent *event)
{
    if (!m_controller)
        return;

    if (event->button() != Qt::LeftButton)
        return;

    int row = -1;
    int col = -1;
    if (!mapPointToCell(event->pos(), row, col))
        return;

    emit cellClicked(row, col);
}

void BoardView::drawBoard(QPainter &painter)
{
    const Board &board = m_controller->board();

    const QColor lightColor(240, 217, 181);
    const QColor darkColor(181, 136, 99);
    const QColor invalidColor(80, 80, 80);

    QPen pen(Qt::black);
    pen.setWidth(1);
    painter.setPen(pen);

    const int cellSize = m_cachedCellSize;

    for (int r = 0; r < Board::ROWS; ++r)
    {
        for (int c = 0; c < Board::COLS; ++c)
        {
            QRect cell = cellRect(r, c);

            if (!board.isValidCell(r, c))
            {
                painter.fillRect(cell, invalidColor);
                painter.drawRect(cell);
                continue;
            }

            const bool dark = ((r + c) % 2 != 0);
            painter.fillRect(cell, dark ? darkColor : lightColor);
            painter.drawRect(cell);
        }
    }
}

void BoardView::drawPieces(QPainter &painter)
{
    const Board &board = m_controller->board();

    QFont font = painter.font();
    font.setBold(true);
    painter.setFont(font);

    const QFontMetrics fm(font);
    const int cellSize = m_cachedCellSize;

    for (int r = 0; r < Board::ROWS; ++r)
    {
        for (int c = 0; c < Board::COLS; ++c)
        {
            if (!board.isValidCell(r, c))
                continue;

            const Piece &p = board.pieceAt(r, c);
            if (p.isEmpty())
                continue;

            const QRect cell = cellRect(r, c);
            const QChar ch = pieceChar(p.kind);

            painter.setPen(p.color == PieceColor::White ? Qt::white : Qt::black);

            const int textWidth  = fm.horizontalAdvance(ch);
            const int textHeight = fm.ascent();

            const int x = cell.x() + (cellSize - textWidth) / 2;
            const int y = cell.y() + (cellSize + textHeight) / 2;

            painter.drawText(x, y, QString(ch));
        }
    }
}

QChar BoardView::pieceChar(PieceKind kind) const
{
    switch (kind)
    {
    case PieceKind::King:     return QChar('K');
    case PieceKind::Queen:    return QChar('Q');
    case PieceKind::Rook:     return QChar('R');
    case PieceKind::Bishop:   return QChar('B');
    case PieceKind::Knight:   return QChar('N');
    case PieceKind::Pawn:     return QChar('P');
    case PieceKind::Champion: return QChar('C');
    case PieceKind::Wizard:   return QChar('W');
    default:
        return QChar(' ');
    }
}
