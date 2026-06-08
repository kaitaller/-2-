#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <limits>
#include <cstdlib>

// ============================================================
//  ЦВЕТА (ANSI-коды)
// ============================================================
namespace Color {
    const std::string RESET  = "\033[0m";
    const std::string RED    = "\033[31m";
    const std::string GREEN  = "\033[32m";
    const std::string YELLOW = "\033[33m";
    const std::string CYAN   = "\033[36m";
    const std::string BOLD   = "\033[1m";
    const std::string DIM    = "\033[2m";
}

// ============================================================
//  ТИПЫ ГОЛОВОЛОМОК
// ============================================================
enum class PuzzleType { NONE, RIDDLE, MATH, SEQUENCE };

// ============================================================
//  СТРУКТУРА ГОЛОВОЛОМКИ
// ============================================================
struct Puzzle {
    PuzzleType type   = PuzzleType::NONE;
    std::string question;
    std::string answer;       // правильный ответ (нижний регистр / строка)
    std::string hint;

    bool solve() const {
        if (type == PuzzleType::NONE) return true;

        std::cout << Color::YELLOW << Color::BOLD;
        std::cout << "\n╔══════════════════════════════════════╗\n";
        std::cout << "║         ГОЛОВОЛОМКА!                 ║\n";
        std::cout << "╚══════════════════════════════════════╝\n";
        std::cout << Color::RESET;

        std::cout << Color::CYAN << "\n" << question << "\n" << Color::RESET;
        std::cout << Color::DIM << "(Подсказка: " << hint << ")\n" << Color::RESET;

        std::string input;
        for (int attempt = 1; attempt <= 3; ++attempt) {
            std::cout << "\nПопытка " << attempt << "/3 > ";
            std::getline(std::cin, input);
            // привести к нижнему регистру
            std::string lower = input;
            std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
            if (lower == answer) {
                std::cout << Color::GREEN << "\n✓ Верно! Путь открыт!\n" << Color::RESET;
                return true;
            }
            if (attempt < 3)
                std::cout << Color::RED << "✗ Неверно. Попробуй ещё раз.\n" << Color::RESET;
        }
        std::cout << Color::RED << "\n✗ Попытки исчерпаны. Правильный ответ: " << answer << "\n" << Color::RESET;
        return false;   // пустить игрока даже при неудаче? Нет — вернём false
    }
};

// ============================================================
//  СТРУКТУРА ЛОКАЦИИ
// ============================================================
struct Location {
    int  id;
    std::string name;
    std::vector<std::string> map;   // ASCII-карта
    std::vector<int> connections;   // куда можно попасть (id)
    bool isOpen;
    bool isExit;                    // последняя локация — выход
    Puzzle puzzle;
};

// ============================================================
//  ПЕРСОНАЖ
// ============================================================
struct Character {
    int locationId;
};

// ============================================================
//  ИГРА
// ============================================================
class Game {
public:
    Game() { init(); }
    void run();

private:
    std::map<int, Location> locations;
    Character player;

    void init();
    void clearScreen();
    void printTitle();
    void printLocation(const Location& loc);
    void printConnections();
    void handleMove(int targetId);
    bool isGameOver() const;
};

// ----------------------------------------------------------------
void Game::clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

// ----------------------------------------------------------------
void Game::printTitle() {
    std::cout << Color::BOLD << Color::CYAN;
    std::cout << R"(
  __  __    _    ____  _____    ___  _   _ _____ ____ _____
 |  \/  |  / \  |_  / | ____|  / _ \| | | | ____/ ___|_   _|
 | |\/| | / _ \  / /  |  _|   | | | | | | |  _| \___ \ | |
 | |  | |/ ___ \/ /__ | |___  | |_| | |_| | |___ ___) || |
 |_|  |_/_/   \_\____||_____|  \__\_\\___/|_____|____/ |_|
)" << Color::RESET;
    std::cout << Color::DIM << "         Подземный лабиринт — выберитесь живым!\n\n" << Color::RESET;
}

// ----------------------------------------------------------------
void Game::init() {
    // ---- ЛОКАЦИЯ 1 — Тёмный вход (стартовая, открыта) --------
    Location loc1;
    loc1.id     = 1;
    loc1.name   = "Тёмный вход";
    loc1.isOpen = true;
    loc1.isExit = false;
    loc1.connections = {2};
    loc1.map = {
        "############",
        "#..........#",
        "#....@.....#",
        "#..........#",
        "#.....N....#",
        "#..........#",
        "#.........D#",
        "############"
    };
    loc1.puzzle.type = PuzzleType::NONE;
    locations[1] = loc1;

    // ---- ЛОКАЦИЯ 2 — Зал Загадок (промежуточная, закрыта) ----
    Location loc2;
    loc2.id     = 2;
    loc2.name   = "Зал Загадок";
    loc2.isOpen = false;
    loc2.isExit = false;
    loc2.connections = {3};
    loc2.map = {
        "################",
        "#..............#",
        "#...C......C...#",
        "#..............#",
        "#.....@........#",
        "#..............#",
        "#.....N........#",
        "#...........D..#",
        "################"
    };
    // Тип 1 — ЗАГАДКА
    loc2.puzzle.type     = PuzzleType::RIDDLE;
    loc2.puzzle.question =
        "Я всегда иду вперёд, но никогда не возвращаюсь назад.\n"
        "Меня нельзя купить или продать, но ею дорожат все.\n"
        "Что это?";
    loc2.puzzle.answer   = "время";
    loc2.puzzle.hint     = "одно слово, существительное";
    locations[2] = loc2;

    // ---- ЛОКАЦИЯ 3 — Комната Счисления (промежуточная, закрыта)
    Location loc3;
    loc3.id     = 3;
    loc3.name   = "Комната Счисления";
    loc3.isOpen = false;
    loc3.isExit = false;
    loc3.connections = {4};
    loc3.map = {
        "####################",
        "#..................#",
        "#...C..........C...#",
        "#..................#",
        "#......@...........#",
        "#..................#",
        "#..C...............#",
        "#..................#",
        "#...............D..#",
        "####################"
    };
    // Тип 2 — МАТЕМАТИКА
    loc3.puzzle.type     = PuzzleType::MATH;
    loc3.puzzle.question =
        "Реши уравнение и введи ответ (число):\n\n"
        "   3 * X + 7 = 22\n"
        "   X = ?";
    loc3.puzzle.answer   = "5";
    loc3.puzzle.hint     = "целое число";
    locations[3] = loc3;

    // ---- ЛОКАЦИЯ 4 — Выход (последняя, закрыта) ---------------
    Location loc4;
    loc4.id     = 4;
    loc4.name   = "Врата Свободы";
    loc4.isOpen = false;
    loc4.isExit = true;
    loc4.connections = {};
    loc4.map = {
        "######################",
        "#....................#",
        "#....C..........C....#",
        "#....................#",
        "#........@...........#",
        "#....................#",
        "#....................#",
        "#....C..........C....#",
        "#....................#",
        "######################"
    };
    // Тип 3 — ПОСЛЕДОВАТЕЛЬНОСТЬ
    loc4.puzzle.type     = PuzzleType::SEQUENCE;
    loc4.puzzle.question =
        "Продолжи последовательность (введи следующее число):\n\n"
        "   1, 1, 2, 3, 5, 8, 13, __?";
    loc4.puzzle.answer   = "21";
    loc4.puzzle.hint     = "сумма двух предыдущих чисел";
    locations[4] = loc4;

    // Начальная позиция игрока
    player.locationId = 1;
}

// ----------------------------------------------------------------
void Game::printLocation(const Location& loc) {
    std::cout << Color::BOLD << Color::CYAN
              << "\n[ Локация " << loc.id << " : " << loc.name << " ]\n"
              << Color::RESET;

    std::cout << Color::DIM;
    for (const auto& row : loc.map)
        std::cout << "  " << row << "\n";
    std::cout << Color::RESET;

    std::cout << Color::DIM
              << "\n  @ — Игрок   C — Сундук   N — NPC   D — Дверь\n"
              << "  # — Стена   . — Пол\n" << Color::RESET;
}

// ----------------------------------------------------------------
void Game::printConnections() {
    const Location& cur = locations[player.locationId];
    std::cout << "\n" << Color::BOLD << "Доступные переходы:\n" << Color::RESET;
    for (int id : cur.connections) {
        const Location& dest = locations[id];
        std::string status = dest.isOpen
            ? (Color::GREEN + "[ОТКРЫТА]" + Color::RESET)
            : (Color::RED   + "[ЗАКРЫТА]" + Color::RESET);
        std::cout << "  [" << id << "] " << dest.name << " — " << status << "\n";
    }
}

// ----------------------------------------------------------------
void Game::handleMove(int targetId) {
    const Location& cur = locations[player.locationId];

    // Проверяем, что переход существует
    bool found = false;
    for (int id : cur.connections)
        if (id == targetId) { found = true; break; }

    if (!found) {
        std::cout << Color::RED << "Нет пути к этой локации отсюда.\n" << Color::RESET;
        return;
    }

    Location& dest = locations[targetId];

    if (!dest.isOpen) {
        std::cout << Color::YELLOW
                  << "\nЛокация закрыта! Нужно решить головоломку, чтобы открыть дверь.\n"
                  << Color::RESET;
        bool solved = dest.puzzle.solve();
        if (!solved) {
            std::cout << Color::RED << "\nДверь не открылась. Попробуй позже.\n" << Color::RESET;
            return;
        }
        dest.isOpen = true;
    }

    player.locationId = targetId;
    std::cout << Color::GREEN << "\nТы перешёл в: " << dest.name << "\n" << Color::RESET;
}

// ----------------------------------------------------------------
bool Game::isGameOver() const {
    return locations.at(player.locationId).isExit
        && locations.at(player.locationId).isOpen;
}

// ----------------------------------------------------------------
void Game::run() {
    clearScreen();
    printTitle();

    std::cout << Color::CYAN
              << "Ты очнулся в тёмном подземном лабиринте...\n"
              << "Стены давят. Откуда-то тянет сквозняком — значит, выход есть!\n"
              << Color::RESET;

    std::cout << "\nНажми Enter, чтобы начать...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    while (true) {
        clearScreen();
        printTitle();

        const Location& cur = locations[player.locationId];
        printLocation(cur);

        if (isGameOver()) {
            std::cout << Color::GREEN << Color::BOLD;
            std::cout << "\n╔══════════════════════════════════════╗\n";
            std::cout << "║   🎉  ПОЗДРАВЛЯЕМ! ТЫ ВЫБРАЛСЯ!  🎉  ║\n";
            std::cout << "║   Лабиринт остался позади навсегда.  ║\n";
            std::cout << "╚══════════════════════════════════════╝\n";
            std::cout << Color::RESET;
            break;
        }

        printConnections();

        std::cout << "\n" << Color::BOLD << "Команды: " << Color::RESET
                  << "<номер локации> — перейти  |  q — выйти из игры\n";
        std::cout << "> ";

        std::string input;
        std::getline(std::cin, input);

        if (input == "q" || input == "Q") {
            std::cout << Color::DIM << "\nДо встречи в подземелье...\n" << Color::RESET;
            break;
        }

        // Попытка разобрать число
        try {
            int choice = std::stoi(input);
            handleMove(choice);
            std::cout << "\nНажми Enter для продолжения...";
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        } catch (...) {
            std::cout << Color::RED << "Неверная команда.\n" << Color::RESET;
            std::cout << "\nНажми Enter...";
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }
}

// ============================================================
//  ТОЧКА ВХОДА
// ============================================================
int main() {
    // Поддержка UTF-8 в Windows-консоли
#ifdef _WIN32
    system("chcp 65001 >nul");
#endif

    Game game;
    game.run();
    return 0;
}
