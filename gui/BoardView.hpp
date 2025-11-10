#pragma once

#include <QWidget>
#include "../logic/Board.hpp"
#include "../controller/GameController.hpp"

/**
 * Виджет для отображения доски Омега-шахмат.
 */
class BoardView : public QWidget
{
    Q_OBJECT

public:
    explicit BoardView(GameController *controller, QWidget *parent = nullptr);
    ~BoardView() override = default;

    void setController(GameController *controller) noexcept;
    GameController *controller() const noexcept { return m_controller; }

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

    signals:
        void cellClicked(int row, int col);

public slots:
    void refreshBoard();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    void computeGeometry(int &cellSize, int &offsetX, int &offsetY) const;
    bool mapPointToCell(const QPoint &pt, int &row, int &col) const;
    QRect cellRect(int row, int col) const;

    void drawBoard(QPainter &painter);
    void drawPieces(QPainter &painter);
    QChar pieceChar(PieceKind kind) const;

private:
    GameController *m_controller = nullptr;

    mutable int m_cachedCellSize = 0;
    mutable int m_cachedOffsetX  = 0;
    mutable int m_cachedOffsetY  = 0;
};
