#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cctype>
#include <windows.h> // Для работы с цветом в Windows

using namespace std;

enum class PieceType { KING, QUEEN, ROOK, BISHOP, KNIGHT, PAWN, NONE };
enum class Color { WHITE, BLACK, NONE };

// Функции для работы с цветом в консоли
void setConsoleColor(int color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

void resetConsoleColor() {
    setConsoleColor(7); // Возвращаем стандартный цвет
}

struct Piece {
    PieceType type = PieceType::NONE;
    Color color = Color::NONE;
    bool hasMoved = false;
};

struct Position {
    int x; // 0-7 (a-h)
    int y; // 0-7 (1-8)
};

class ChessBoard {
private:
    Piece board[8][8];
    Color currentPlayer = Color::WHITE;
    Position whiteKingPos = { 4, 0 };
    Position blackKingPos = { 4, 7 };
    bool gameOver = false;
    int difficulty = 3;

    void initializeBoard() {
        // Очистка доски
        for (int y = 0; y < 8; ++y) {
            for (int x = 0; x < 8; ++x) {
                board[x][y] = { PieceType::NONE, Color::NONE };
            }
        }

        if (difficulty == 1) {
            // Только кони и короли
            board[4][0] = { PieceType::KING, Color::WHITE };
            board[1][0] = { PieceType::KNIGHT, Color::WHITE };
            board[6][0] = { PieceType::KNIGHT, Color::WHITE };

            board[4][7] = { PieceType::KING, Color::BLACK };
            board[1][7] = { PieceType::KNIGHT, Color::BLACK };
            board[6][7] = { PieceType::KNIGHT, Color::BLACK };
        }
        else if (difficulty == 2) {
            // Только пешки и короли
            board[4][0] = { PieceType::KING, Color::WHITE };
            for (int x = 0; x < 8; ++x) {
                board[x][1] = { PieceType::PAWN, Color::WHITE };
            }

            board[4][7] = { PieceType::KING, Color::BLACK };
            for (int x = 0; x < 8; ++x) {
                board[x][6] = { PieceType::PAWN, Color::BLACK };
            }
        }
        else {
            // Стандартная расстановка
            // Белые фигуры
            board[0][0] = { PieceType::ROOK, Color::WHITE };
            board[1][0] = { PieceType::KNIGHT, Color::WHITE };
            board[2][0] = { PieceType::BISHOP, Color::WHITE };
            board[3][0] = { PieceType::QUEEN, Color::WHITE };
            board[4][0] = { PieceType::KING, Color::WHITE };
            board[5][0] = { PieceType::BISHOP, Color::WHITE };
            board[6][0] = { PieceType::KNIGHT, Color::WHITE };
            board[7][0] = { PieceType::ROOK, Color::WHITE };
            for (int x = 0; x < 8; ++x) {
                board[x][1] = { PieceType::PAWN, Color::WHITE };
            }

            // Черные фигуры
            board[0][7] = { PieceType::ROOK, Color::BLACK };
            board[1][7] = { PieceType::KNIGHT, Color::BLACK };
            board[2][7] = { PieceType::BISHOP, Color::BLACK };
            board[3][7] = { PieceType::QUEEN, Color::BLACK };
            board[4][7] = { PieceType::KING, Color::BLACK };
            board[5][7] = { PieceType::BISHOP, Color::BLACK };
            board[6][7] = { PieceType::KNIGHT, Color::BLACK };
            board[7][7] = { PieceType::ROOK, Color::BLACK };
            for (int x = 0; x < 8; ++x) {
                board[x][6] = { PieceType::PAWN, Color::BLACK };
            }
        }
    }

    void printBoard() const {
        // Цвета: 7 - стандартный, 15 - белый, 9 - синий
        setConsoleColor(7);
        cout << "    a b c d e f g h\n";
        cout << "   -----------------\n";

        for (int y = 7; y >= 0; --y) {
            setConsoleColor(7);
            cout << y + 1 << " | ";


            for (int x = 0; x < 8; ++x) {
                char c = '.';
                switch (board[x][y].type) {
                case PieceType::KING: c = 'K'; break;
                case PieceType::QUEEN: c = 'Q'; break;
                case PieceType::ROOK: c = 'R'; break;
                case PieceType::BISHOP: c = 'B'; break;
                case PieceType::KNIGHT: c = 'N'; break;
                case PieceType::PAWN: c = 'P'; break;
                default: c = '.'; break;
                }

                if (board[x][y].color == Color::BLACK) {
                    setConsoleColor(9); // Синий для черных фигур
                    c = tolower(c);
                }
                else if (board[x][y].color == Color::WHITE) {
                    setConsoleColor(15); // Белый для белых фигур
                }
                else {
                    setConsoleColor(7); // Стандартный для пустых клеток
                }

                cout << c << " ";
            }

            setConsoleColor(7);
            cout << "| " << y + 1 << "\n";
        }

        setConsoleColor(7);
        cout << "   -----------------\n";
        cout << "    a b c d e f g h\n\n";
    }

    bool isPositionValid(const Position& pos) const {
        return pos.x >= 0 && pos.x < 8 && pos.y >= 0 && pos.y < 8;
    }

    bool isMoveValid(const Position& from, const Position& to) const {
        if (!isPositionValid(from) || !isPositionValid(to) || (from.x == to.x && from.y == to.y)) {
            return false;
        }

        const Piece& piece = board[from.x][from.y];
        const Piece& target = board[to.x][to.y];

        if (piece.color != currentPlayer) {
            return false;
        }

        if (target.color == currentPlayer) {
            return false;
        }

        switch (piece.type) {
        case PieceType::KING: {
            int dx = abs(to.x - from.x);
            int dy = abs(to.y - from.y);
            return (dx <= 1 && dy <= 1);
        }
        case PieceType::QUEEN: {
            return isStraightMoveValid(from, to) || isDiagonalMoveValid(from, to);
        }
        case PieceType::ROOK: {
            return isStraightMoveValid(from, to);
        }
        case PieceType::BISHOP: {
            return isDiagonalMoveValid(from, to);
        }
        case PieceType::KNIGHT: {
            int dx = abs(to.x - from.x);
            int dy = abs(to.y - from.y);
            return (dx == 1 && dy == 2) || (dx == 2 && dy == 1);
        }
        case PieceType::PAWN: {
            int direction = (piece.color == Color::WHITE) ? 1 : -1;
            int startRow = (piece.color == Color::WHITE) ? 1 : 6;

            if (from.x == to.x && target.type == PieceType::NONE) {
                if (to.y == from.y + direction) {
                    return true;
                }
                if (from.y == startRow && to.y == from.y + 2 * direction &&
                    board[from.x][from.y + direction].type == PieceType::NONE) {
                    return true;
                }
            }

            if (abs(to.x - from.x) == 1 && to.y == from.y + direction &&
                target.type != PieceType::NONE && target.color != currentPlayer) {
                return true;
            }

            return false;
        }
        default:
            return false;
        }
    }

    bool isStraightMoveValid(const Position& from, const Position& to) const {
        if (from.x != to.x && from.y != to.y) {
            return false;
        }

        int stepX = (to.x == from.x) ? 0 : (to.x > from.x ? 1 : -1);
        int stepY = (to.y == from.y) ? 0 : (to.y > from.y ? 1 : -1);

        int x = from.x + stepX;
        int y = from.y + stepY;

        while (x != to.x || y != to.y) {
            if (board[x][y].type != PieceType::NONE) {
                return false;
            }
            x += stepX;
            y += stepY;
        }

        return true;
    }


    bool isDiagonalMoveValid(const Position& from, const Position& to) const {
        int dx = abs(to.x - from.x);
        int dy = abs(to.y - from.y);
        if (dx != dy) {
            return false;
        }

        int stepX = (to.x > from.x) ? 1 : -1;
        int stepY = (to.y > from.y) ? 1 : -1;

        int x = from.x + stepX;
        int y = from.y + stepY;

        while (x != to.x || y != to.y) {
            if (board[x][y].type != PieceType::NONE) {
                return false;
            }
            x += stepX;
            y += stepY;
        }

        return true;
    }

    bool isInCheck(Color color) const {
        Position kingPos = (color == Color::WHITE) ? whiteKingPos : blackKingPos;

        for (int y = 0; y < 8; ++y) {
            for (int x = 0; x < 8; ++x) {
                if (board[x][y].color != Color::NONE && board[x][y].color != color) {
                    Position from = { x, y };
                    if (isMoveValid(from, kingPos)) {
                        return true;
                    }
                }
            }
        }

        return false;
    }

    bool isCheckmate(Color color) {
        if (!isInCheck(color)) {
            return false;
        }

        for (int fromY = 0; fromY < 8; ++fromY) {
            for (int fromX = 0; fromX < 8; ++fromX) {
                if (board[fromX][fromY].color == color) {
                    Position from = { fromX, fromY };

                    for (int toY = 0; toY < 8; ++toY) {
                        for (int toX = 0; toX < 8; ++toX) {
                            Position to = { toX, toY };

                            if (isMoveValid(from, to)) {
                                Piece original = board[toX][toY];
                                Piece moved = board[fromX][fromY];
                                board[toX][toY] = moved;
                                board[fromX][fromY] = { PieceType::NONE, Color::NONE };

                                if (moved.type == PieceType::KING) {
                                    if (color == Color::WHITE) {
                                        whiteKingPos = to;
                                    }
                                    else {
                                        blackKingPos = to;
                                    }
                                }

                                bool stillInCheck = isInCheck(color);

                                board[fromX][fromY] = moved;
                                board[toX][toY] = original;

                                if (color == Color::WHITE) {
                                    whiteKingPos = from;
                                }
                                else {
                                    blackKingPos = from;
                                }

                                if (!stillInCheck) {
                                    return false;
                                }
                            }
                        }
                    }
                }
            }
        }

        return true;
    }

    bool makeMove(const Position& from, const Position& to) {
        if (!isMoveValid(from, to)) {
            return false;
        }

        Piece movedPiece = board[from.x][from.y];
        movedPiece.hasMoved = true;
        board[to.x][to.y] = movedPiece;
        board[from.x][from.y] = { PieceType::NONE, Color::NONE };

        if (movedPiece.type == PieceType::KING) {
            if (movedPiece.color == Color::WHITE) {
                whiteKingPos = to;
            }
            else {
                blackKingPos = to;
            }
        }

        if (isInCheck(currentPlayer)) {
            board[from.x][from.y] = movedPiece;
            board[to.x][to.y] = { PieceType::NONE, Color::NONE };


            if (movedPiece.type == PieceType::KING) {
                if (movedPiece.color == Color::WHITE) {
                    whiteKingPos = from;
                }
                else {
                    blackKingPos = from;
                }
            }

            return false;
        }

        currentPlayer = (currentPlayer == Color::WHITE) ? Color::BLACK : Color::WHITE;

        if (isCheckmate(currentPlayer)) {
            gameOver = true;
            setConsoleColor(15);
            cout << (currentPlayer == Color::WHITE ? "Белые " : "Чёрные ") << " выигрывают, поставив мат!\n";
            resetConsoleColor();
        }

        return true;
    }

    Position parsePosition(const string& input) const {
        if (input.length() != 2) {
            return { -1, -1 };
        }

        int x = tolower(input[0]) - 'a';
        int y = input[1] - '1';

        return { x, y };
    }

public:
    ChessBoard(int diff) : difficulty(diff) {
        initializeBoard();
    }

    void play() {
        while (!gameOver) {
            printBoard();

            setConsoleColor(currentPlayer == Color::WHITE ? 15 : 9);
            cout << (currentPlayer == Color::WHITE ? "Белые " : "Чёрные ") << "ходят. Введите ход (например, e2 e4): ";
            resetConsoleColor();

            string fromStr, toStr;
            cin >> fromStr >> toStr;
            cout << endl;

            Position from = parsePosition(fromStr);
            Position to = parsePosition(toStr);

            if (!isPositionValid(from) || !isPositionValid(to)) {
                setConsoleColor(12);
                cout << "Неверная позиция. Попробуйте еще раз.\n";
                resetConsoleColor();
                continue;
            }

            if (!makeMove(from, to)) {
                setConsoleColor(12);
                cout << "Неверный ход. Попробуйте еще раз.\n";
                resetConsoleColor();
            }
        }
    }
};

int main() {
    setlocale(LC_ALL, "ru");
    setConsoleColor(14);
    cout << "Игра «Шахматы»\n";
    resetConsoleColor();

    cout << "Выберите уровень сложности:\n";
    cout << "1 - Только кони и король\n";
    cout << "2 - Только пешки и король\n";
    cout << "3 - Стандартные шахматы\n";
    cout << "Введите ваш выбор (1-3): ";

    int difficulty;
    cin >> difficulty;
    cout << endl;

    if (difficulty < 1 || difficulty > 3) {
        setConsoleColor(12);
        cout << "Неверный выбор.\n\n";
        resetConsoleColor();
        difficulty = 3;
    }

    ChessBoard game(difficulty);
    game.play();

    return 0;
}
