#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cctype>
#include <windows.h> // Для работы с цветом в Windows (только для Windows)

using namespace std;

// Типы шахматных фигур
enum class PieceType { KING, QUEEN, ROOK, BISHOP, KNIGHT, PAWN, NONE };

// Цвета фигур
enum class Color { WHITE, BLACK, NONE };

// Функции для работы с цветом в консоли (Windows-specific)
void setConsoleColor(int color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

void resetConsoleColor() {
    setConsoleColor(7); // Возвращаем стандартный цвет (серый)
}

// Структура, представляющая шахматную фигуру
struct Piece {
    PieceType type = PieceType::NONE; // Тип фигуры
    Color color = Color::NONE;        // Цвет фигуры
    bool hasMoved = false;            // Флаг, указывающий, двигалась ли фигура
};

// Структура для позиции на доске (x - столбец, y - строка)
struct Position {
    int x; // 0-7 (a-h)
    int y; // 0-7 (1-8)
};

// Класс, представляющий шахматную доску и логику игры
class ChessBoard {
private:
    Piece board[8][8];              // Шахматная доска 8x8
    Color currentPlayer = Color::WHITE; // Текущий игрок
    Position whiteKingPos = { 4, 0 };  // Позиция белого короля
    Position blackKingPos = { 4, 7 };  // Позиция черного короля
    bool gameOver = false;           // Флаг окончания игры
    int difficulty = 3;              // Уровень сложности (1-3)

    // Инициализация доски в зависимости от уровня сложности
    void initializeBoard() {
        // Очистка доски
        for (int y = 0; y < 8; ++y) {
            for (int x = 0; x < 8; ++x) {
                board[x][y] = { PieceType::NONE, Color::NONE };
            }
        }

        if (difficulty == 1) {
            // Режим 1: только кони и короли
            board[4][0] = { PieceType::KING, Color::WHITE };   // Белый король
            board[1][0] = { PieceType::KNIGHT, Color::WHITE }; // Белый конь
            board[6][0] = { PieceType::KNIGHT, Color::WHITE }; // Белый конь
            
            board[4][7] = { PieceType::KING, Color::BLACK };   // Черный король
            board[1][7] = { PieceType::KNIGHT, Color::BLACK }; // Черный конь
            board[6][7] = { PieceType::KNIGHT, Color::BLACK }; // Черный конь
        }
        else if (difficulty == 2) {
            // Режим 2: только пешки и короли
            board[4][0] = { PieceType::KING, Color::WHITE }; // Белый король
            for (int x = 0; x < 8; ++x) {
                board[x][1] = { PieceType::PAWN, Color::WHITE }; // Белые пешки
            }
            
            board[4][7] = { PieceType::KING, Color::BLACK }; // Черный король
            for (int x = 0; x < 8; ++x) {
                board[x][6] = { PieceType::PAWN, Color::BLACK }; // Черные пешки
            }
        }
        else {
            // Режим 3: стандартные шахматы
            // Расстановка белых фигур
            board[0][0] = { PieceType::ROOK, Color::WHITE };    // Ладья
            board[1][0] = { PieceType::KNIGHT, Color::WHITE };  // Конь
            board[2][0] = { PieceType::BISHOP, Color::WHITE };  // Слон
            board[3][0] = { PieceType::QUEEN, Color::WHITE };   // Ферзь
            board[4][0] = { PieceType::KING, Color::WHITE };    // Король
            board[5][0] = { PieceType::BISHOP, Color::WHITE };  // Слон
            board[6][0] = { PieceType::KNIGHT, Color::WHITE };  // Конь
            board[7][0] = { PieceType::ROOK, Color::WHITE };    // Ладья
            
            // Белые пешки
            for (int x = 0; x < 8; ++x) {
                board[x][1] = { PieceType::PAWN, Color::WHITE };
            }

            // Расстановка черных фигур
            board[0][7] = { PieceType::ROOK, Color::BLACK };
            board[1][7] = { PieceType::KNIGHT, Color::BLACK };
            board[2][7] = { PieceType::BISHOP, Color::BLACK };
            board[3][7] = { PieceType::QUEEN, Color::BLACK };
            board[4][7] = { PieceType::KING, Color::BLACK };
            board[5][7] = { PieceType::BISHOP, Color::BLACK };
            board[6][7] = { PieceType::KNIGHT, Color::BLACK };
            board[7][7] = { PieceType::ROOK, Color::BLACK };
            
            // Черные пешки
            for (int x = 0; x < 8; ++x) {
                board[x][6] = { PieceType::PAWN, Color::BLACK };
            }
        }
    }

    // Вывод доски в консоль
    void printBoard() const {
        // Цвета: 7 - стандартный, 15 - белый, 9 - синий
        setConsoleColor(7);
        cout << "    a b c d e f g h\n";
        cout << "   -----------------\n";

        // Вывод строк сверху вниз (от 8 до 1)
        for (int y = 7; y >= 0; --y) {
            setConsoleColor(7);
            cout << y + 1 << " | "; // Номер строки

            // Вывод клеток в строке
            for (int x = 0; x < 8; ++x) {
                char c = '.'; // Символ для пустой клетки
                switch (board[x][y].type) {
                case PieceType::KING: c = 'K'; break;
                case PieceType::QUEEN: c = 'Q'; break;
                case PieceType::ROOK: c = 'R'; break;
                case PieceType::BISHOP: c = 'B'; break;
                case PieceType::KNIGHT: c = 'N'; break;
                case PieceType::PAWN: c = 'P'; break;
                default: c = '.'; break;
                }

                // Установка цвета фигуры
                if (board[x][y].color == Color::BLACK) {
                    setConsoleColor(9); // Синий для черных фигур
                    c = tolower(c);     // Черные фигуры выводятся строчными буквами
                }
                else if (board[x][y].color == Color::WHITE) {
                    setConsoleColor(15); // Белый для белых фигур
                }
                else {
                    setConsoleColor(7); // Стандартный цвет для пустых клеток
                }

                cout << c << " "; // Вывод символа фигуры
            }

            setConsoleColor(7);
            cout << "| " << y + 1 << "\n"; // Закрытие строки с номером
        }

        // Нижняя часть доски
        setConsoleColor(7);
        cout << "   -----------------\n";
        cout << "    a b c d e f g h\n\n";
    }

    // Проверка, находится ли позиция в пределах доски
    bool isPositionValid(const Position& pos) const {
        return pos.x >= 0 && pos.x < 8 && pos.y >= 0 && pos.y < 8;
    }

    // Проверка правильности хода
    bool isMoveValid(const Position& from, const Position& to) const {
        // Проверка на выход за пределы доски и ход на ту же клетку
        if (!isPositionValid(from) || !isPositionValid(to) || (from.x == to.x && from.y == to.y)) {
            return false;
        }

        const Piece& piece = board[from.x][from.y];  // Фигура, которую двигают
        const Piece& target = board[to.x][to.y];    // Фигура на целевой клетке

        // Проверка, что фигура принадлежит текущему игроку
        if (piece.color != currentPlayer) {
            return false;
        }

        // Проверка, что не пытаемся съесть свою фигуру
        if (target.color == currentPlayer) {
            return false;
        }

        // Проверка правил движения для каждого типа фигур
        switch (piece.type) {
        case PieceType::KING: {
            // Король ходит на одну клетку в любом направлении
            int dx = abs(to.x - from.x);
            int dy = abs(to.y - from.y);
            return (dx <= 1 && dy <= 1);
        }
        case PieceType::QUEEN: {
            // Ферзь ходит как ладья или слон
            return isStraightMoveValid(from, to) || isDiagonalMoveValid(from, to);
        }
        case PieceType::ROOK: {
            // Ладья ходит по прямым линиям
            return isStraightMoveValid(from, to);
        }
        case PieceType::BISHOP: {
            // Слон ходит по диагоналям
            return isDiagonalMoveValid(from, to);
        }
        case PieceType::KNIGHT: {
            // Конь ходит буквой "Г"
            int dx = abs(to.x - from.x);
            int dy = abs(to.y - from.y);
            return (dx == 1 && dy == 2) || (dx == 2 && dy == 1);
        }
        case PieceType::PAWN: {
            // Пешка имеет особые правила движения
            int direction = (piece.color == Color::WHITE) ? 1 : -1; // Направление движения
            int startRow = (piece.color == Color::WHITE) ? 1 : 6;    // Стартовая позиция
            
            // Ход вперед
            if (from.x == to.x && target.type == PieceType::NONE) {
                // На одну клетку
                if (to.y == from.y + direction) {
                    return true;
                }
                // На две клетки с начальной позиции
                if (from.y == startRow && to.y == from.y + 2 * direction &&
                    board[from.x][from.y + direction].type == PieceType::NONE) {
                    return true;
                }
            }

            // Взятие фигуры по диагонали
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

    // Проверка хода по прямой (для ладьи и ферзя)
    bool isStraightMoveValid(const Position& from, const Position& to) const {
        // Должна быть одна общая координата
        if (from.x != to.x && from.y != to.y) {
            return false;
        }

        // Определяем направление движения
        int stepX = (to.x == from.x) ? 0 : (to.x > from.x ? 1 : -1);
        int stepY = (to.y == from.y) ? 0 : (to.y > from.y ? 1 : -1);

        // Проверяем все клетки между from и to
        int x = from.x + stepX;
        int y = from.y + stepY;
        while (x != to.x || y != to.y) {
            if (board[x][y].type != PieceType::NONE) {
                return false; // На пути есть фигура
            }
            x += stepX;
            y += stepY;
        }

        return true;
    }

    // Проверка хода по диагонали (для слона и ферзя)
    bool isDiagonalMoveValid(const Position& from, const Position& to) const {
        // Разница по x и y должна быть одинаковой
        int dx = abs(to.x - from.x);
        int dy = abs(to.y - from.y);
        if (dx != dy) {
            return false;
        }

        // Определяем направление движения
        int stepX = (to.x > from.x) ? 1 : -1;
        int stepY = (to.y > from.y) ? 1 : -1;

        // Проверяем все клетки между from и to
        int x = from.x + stepX;
        int y = from.y + stepY;
        while (x != to.x || y != to.y) {
            if (board[x][y].type != PieceType::NONE) {
                return false; // На пути есть фигура
            }
            x += stepX;
            y += stepY;
        }

        return true;
    }

    // Проверка, находится ли король под шахом
    bool isInCheck(Color color) const {
        Position kingPos = (color == Color::WHITE) ? whiteKingPos : blackKingPos;

        // Проверяем все фигуры противника
        for (int y = 0; y < 8; ++y) {
            for (int x = 0; x < 8; ++x) {
                if (board[x][y].color != Color::NONE && board[x][y].color != color) {
                    Position from = { x, y };
                    if (isMoveValid(from, kingPos)) {
                        return true; // Найдена фигура, которая может атаковать короля
                    }
                }
            }
        }

        return false;
    }

    // Проверка на мат
    bool isCheckmate(Color color) {
        // Если король не под шахом, то это не мат
        if (!isInCheck(color)) {
            return false;
        }

        // Проверяем все возможные ходы
        for (int fromY = 0; fromY < 8; ++fromY) {
            for (int fromX = 0; fromX < 8; ++fromX) {
                if (board[fromX][fromY].color == color) {
                    Position from = { fromX, fromY };

                    // Проверяем все возможные целевые клетки
                    for (int toY = 0; toY < 8; ++toY) {
                        for (int toX = 0; toX < 8; ++toX) {
                            Position to = { toX, toY };

                            if (isMoveValid(from, to)) {
                                // Делаем временный ход
                                Piece original = board[toX][toY];
                                Piece moved = board[fromX][fromY];
                                board[toX][toY] = moved;
                                board[fromX][fromY] = { PieceType::NONE, Color::NONE };

                                // Обновляем позицию короля, если это он
                                if (moved.type == PieceType::KING) {
                                    if (color == Color::WHITE) {
                                        whiteKingPos = to;
                                    }
                                    else {
                                        blackKingPos = to;
                                    }
                                }

                                // Проверяем, остался ли король под шахом
                                bool stillInCheck = isInCheck(color);

                                // Отменяем временный ход
                                board[fromX][fromY] = moved;
                                board[toX][toY] = original;

                                // Восстанавливаем позицию короля
                                if (color == Color::WHITE) {
                                    whiteKingPos = from;
                                }
                                else {
                                    blackKingPos = from;
                                }

                                // Если нашелся ход, убирающий шах - это не мат
                                if (!stillInCheck) {
                                    return false;
                                }
                            }
                        }
                    }
                }
            }
        }

        // Если ни один ход не убирает шах - это мат
        return true;
    }

    // Выполнение хода
    bool makeMove(const Position& from, const Position& to) {
        if (!isMoveValid(from, to)) {
            return false;
        }

        // Выполняем ход
        Piece movedPiece = board[from.x][from.y];
        movedPiece.hasMoved = true;
        board[to.x][to.y] = movedPiece;
        board[from.x][from.y] = { PieceType::NONE, Color::NONE };

        // Обновляем позицию короля, если это он
        if (movedPiece.type == PieceType::KING) {
            if (movedPiece.color == Color::WHITE) {
                whiteKingPos = to;
            }
            else {
                blackKingPos = to;
            }
        }

        // Проверяем, не поставили ли мы себя под шах
        if (isInCheck(currentPlayer)) {
            // Отменяем ход
            board[from.x][from.y] = movedPiece;
            board[to.x][to.y] = { PieceType::NONE, Color::NONE };

            // Восстанавливаем позицию короля
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

        // Передаем ход другому игроку
        currentPlayer = (currentPlayer == Color::WHITE) ? Color::BLACK : Color::WHITE;

        // Проверяем мат
        if (isCheckmate(currentPlayer)) {
            gameOver = true;
            setConsoleColor(15);
            cout << (currentPlayer == Color::WHITE ? "Белые " : "Чёрные ") << "выигрывают, поставив мат!\n";
            resetConsoleColor();
        }

        return true;
    }

    // Преобразование строки (например, "e2") в позицию на доске
    Position parsePosition(const string& input) const {
        if (input.length() != 2) {
            return { -1, -1 }; // Некорректный ввод
        }

        // Преобразуем букву в столбец (a-h -> 0-7)
        int x = tolower(input[0]) - 'a';
        // Преобразуем цифру в строку (1-8 -> 0-7)
        int y = input[1] - '1';

        return { x, y };
    }

public:
    // Конструктор с установкой уровня сложности
    ChessBoard(int diff) : difficulty(diff) {
        initializeBoard();
    }

    // Основной игровой цикл
    void play() {
        while (!gameOver) {
            printBoard();

            // Приглашение для текущего игрока
            setConsoleColor(currentPlayer == Color::WHITE ? 15 : 9);
            cout << (currentPlayer == Color::WHITE ? "Белые " : "Чёрные ") << "ходят. Введите ход (например, e2 e4): ";
            resetConsoleColor();

            string fromStr, toStr;
            cin >> fromStr >> toStr;
            cout << endl;

            // Преобразуем введенные строки в позиции
            Position from = parsePosition(fromStr);
            Position to = parsePosition(toStr);

            // Проверка корректности позиций
            if (!isPositionValid(from) || !isPositionValid(to)) {
                setConsoleColor(12);
                cout << "Неверная позиция. Попробуйте еще раз.\n";
                resetConsoleColor();
                continue;
            }

            // Попытка сделать ход
            if (!makeMove(from, to)) {
                setConsoleColor(12);
                cout << "Неверный ход. Попробуйте еще раз.\n";
                resetConsoleColor();
            }
        }
    }
};

int main() {
    setlocale(LC_ALL, "ru"); // Для поддержки русского языка
    setConsoleColor(14);
    cout << "Игра «Шахматы»\n";
    resetConsoleColor();

    // Выбор уровня сложности
    cout << "Выберите уровень сложности:\n";
    cout << "1 - Только кони и король\n";
    cout << "2 - Только пешки и король\n";
    cout << "3 - Стандартные шахматы\n";
    cout << "Введите ваш выбор (1-3): ";

    int difficulty;
    cin >> difficulty;
    cout << endl;

    // Проверка корректности ввода
    if (difficulty < 1 || difficulty > 3) {
        setConsoleColor(12);
        cout << "Неверный выбор. Установлен стандартный режим.\n\n";
        resetConsoleColor();
        difficulty = 3;
    }

    // Создание и запуск игры
    ChessBoard game(difficulty);
    game.play();

    return 0;
}
