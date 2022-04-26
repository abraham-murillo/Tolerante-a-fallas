// Computación tolerante a fallas, D01, Equipo 02
// - Correa Patiño Rogelio Andres
// - Guzmán Mendoza Jesús Uriel
// - Murillo Sandoval Abraham Baltazar
// - Valencia Curiel Armando Saul

#include <algorithm>
#include <array>
#include <bitset>
#include <cmath>
#include <fstream>
#include <iostream>
#include <map>
#include <queue>
#include <set>
#include <string>
#include <thread>
#include <utility>
#include <vector>
using namespace std::this_thread; // sleep_for, sleep_until
using namespace std::chrono_literals; // ns, us, ms, s, h, etc.
using namespace std;

// Tiempo que espera en pantalla
const auto WAIT_ON_SCREEN = 1s;
const int BAR_HEIGHT = 5;
const int BAR_WIDTH = 3;

struct Task {
  int id;
  int start;
  int end;
  int x, y;
};

void paint(int x, int y, int color) {
  // Hacer que pinte (x, y) con cierto color
}

struct Numbers {
  string number[10][5];

  void read() {
    ifstream data("numbers.txt");
    if (!data) {
      cerr << "Error no se puede abrir el archivo\n";
      exit(1);
    } else {
      for (int num = 0; num <= 9; num++) {
        for (int row = 0; row < 5; row++) {
          getline(data, number[num][row]);
          while (number[num][row].size() < 6)
            number[num][row] += " ";
        }
      }
    }
    data.close();
  }

  void print(int x, int y, int color, int num) {
    if (num < 0)
      return;

    vector<int> digits;
    if (num == 0) {
      digits.push_back(0);
    } else {
      while (num > 0) {
        digits.push_back(num % 10);
        num /= 10;
      }
      reverse(digits.begin(), digits.end());
    }

    for (int row = 0; row < 5; row++)
      for (int i = 0; i < digits.size(); i++)
        if (number[digits[i]][row] == '#')
          paint(x + i, y + row, color);
  }
} numbers;

void clearScreen() {
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
  system("cls");
#else
  system("clear");
#endif
}

int main() {
  numbers.read();

  int n;
  cout << "Escriba la cantidad de procesos:\n";
  cin >> n;

  int maxEnd = 0;
  vector<Task> tasks;
  for (int id = 0, lastY = 20; id < n; id++, lastY += BAR_HEIGHT) {
    Task task;
    int duration;
    cout << "Escriba cuando inicia y cuánto dura el proceso:\n";
    cin >> task.start >> duration;
    task.end = task.start + duration - 1;
    task.id = id;

    task.x = 0;
    task.y = lastY;

    tasks.emplace_back(task);

    maxEnd = max(maxEnd, task.end);
  }

  clearScreen();

  sort(tasks.begin(), tasks.end(), [&](const Task& a, const Task& b) {
    if (a.start != b.start)
      return a.start < b.start;
    return a.end < b.end;
  });

  set<pair<int, int>> inExecution; // {end, task position}
  int lastPos = 0;
  for (int globalTime = 0; globalTime <= maxEnd + 1; globalTime++) {
    // Agregar los tasks que apenas empiezan
    while (lastPos < n && tasks[lastPos].start == globalTime) {
      inExecution.emplace(tasks[lastPos].end, lastPos);
      lastPos++;
    }

    // Eliminar los tasks terminados
    while (inExecution.size() && inExecution.begin()->first < globalTime) {
      inExecution.erase(inExecution.begin());
    }

    // Pintarlo de negro para "borrarlo"
    numbers.print(0, 0, BLACK, globalTime - 1);
    // Pintarlo de nuevo pero en blanco
    numbers.print(0, 0, WHITE, globalTime);

    for (auto&& [end, pos] : inExecution) {
      const Task& task = tasks[pos];
      // Pintar con gotoxy()
      // Se supone que cada task tiene un (x, y), pintar el cuadrado siguiente
      for (int i = 0; i < BAR_WIDTH; i++)
        for (int j = 0; j < BAR_HEIGHT; j++) {
          paint(task.x + i, task.y + j, GREEN);
        }
      task.x += BAR_WIDTH;
    }

    sleep_for(WAIT_ON_SCREEN);
  }

  return 0;
}