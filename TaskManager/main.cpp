// Computación tolerante a fallas, D01, Equipo 02
// - Correa Patiño Rogelio Andres
// - Guzmán Mendoza Jesús Uriel
// - Murillo Sandoval Abraham Baltazar
// - Valencia Curiel Armando Saul
// - Villalpando Armenta Marco Antonio

#include <algorithm>
#include <array>
#include <bitset>
#include <cmath>
#include <conio.h>
#include <cstdio>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <queue>
#include <set>
#include <string>
#include <thread>
#include <utility>
#include <vector>
#include <windows.h>

using namespace std;

const int BAR_HEIGHT = 5;
const int BLACK = 0;
const int WHITE = 15;
int columns, rows;

struct Task {
  int id;
  int start;
  int end;
  int x, y;
};

void paint(int x, int y, int color, char ch = ' ') {
  // Hacer que pinte (x, y) con cierto color
  COORD coord = {x, y};
  SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
  cout << ch;
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

    for (int row = 0; row < 5; row++) {
      int k = 0;
      for (int i = 0; i < digits.size(); i++)
        for (char ch : number[digits[i]][row])
          paint(x + i + k++, y + row, color, ch);
    }
  }
} numbers;

void clearScreen() {
#if defined(WIN32) || defined(WIN32) || defined(WIN32) || defined(NT_)
  system("cls");
#else
  system("clear");
#endif
}

int main() {
  numbers.read();
  CONSOLE_SCREEN_BUFFER_INFO csbi;

  GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
  columns = csbi.srWindow.Right - csbi.srWindow.Left + 1;
  rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;

  // printf("columns: %d\n", columns);
  // printf("rows: %d\n", rows);

  int n;
  cout << "Escriba la cantidad de procesos:\n";
  cin >> n;

  int maxEnd = 0;
  vector<Task> tasks;
  int lastY = BAR_HEIGHT + 3;
  for (int id = 0; id < n; id++) {
    Task task;
    int duration;
    cout << "Escriba cuando inicia y cu�nto dura el proceso:\n";
    cin >> task.start >> duration;
    task.end = task.start + duration - 1;
    task.id = id;

    task.x = 0;
    task.y = lastY;

    tasks.emplace_back(task);

    maxEnd = max(maxEnd, task.end);
    lastY += BAR_HEIGHT;
  }

  // Escalar las barras
  // Tengo que hacer maxEnd pedazos en columns
  int barWidth = max(1, columns / (maxEnd + 1));
  // cout << barWidth << endl;
  for (Task& task : tasks) {
    task.x = (task.start * barWidth);
    // cout << task.id << " " << task.x << " " << task.y << endl;
  }

  clearScreen();

  paint(0, lastY + 2, WHITE);
  cout << "Datos de los procesos\n";
  cout << "Proceso  | Inicio   | Duracion\n";
  for (Task& task : tasks) {
    cout << setw(7) << "P" + to_string(task.id) << setw(10) << task.start << setw(10) << task.end - task.start + 1 << "\n";
  }

  sort(tasks.begin(), tasks.end(), [&](const Task& a, const Task& b) {
    if (a.start != b.start)
      return a.start < b.start;
    return a.end < b.end;
  });

  vector<int> byEndTime(n);
  for (int i = 0; i < n; i++)
    byEndTime[i] = i;
  sort(byEndTime.begin(), byEndTime.end(), [&](int i, int j) {
    if (tasks[i].end != tasks[j].end)
      return tasks[i].end < tasks[j].end;
    return tasks[i].start < tasks[j].start;
  });

  for (Task& task : tasks) {
    paint(task.x - 4, task.y + 2, WHITE);
    cout << task.id;
  }

  int clockX = 8;
  int clockY = 1;
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
    numbers.print(clockX, clockY, BLACK, globalTime - 1);

    // Pintarlo de nuevo pero en blanco
    numbers.print(clockX, clockY, WHITE, globalTime);

    vector<int> colors = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
    for (auto it : inExecution) {
      Task& task = tasks[it.second];
      // Pintar con gotoxy()
      // Se supone que cada task tiene un (x, y), pintar el cuadrado siguiente
      for (int i = 0; i < barWidth; i++)
        for (int j = 0; j < BAR_HEIGHT; j++) {
          paint(task.x + i, task.y + j, colors[task.id % 15] * 16);
        }
      task.x += barWidth;
    }

    paint(0, lastY + 2 + tasks.size() + 4, WHITE);
    cout << "Orden de ejecucion\n";
    cout << "Proceso  |  Ejecucion\n";
    for (int i : byEndTime) {
      Task& task = tasks[i];
      if (globalTime >= task.start) {
        cout << setw(7) << "P" + to_string(task.id) << setw(10) << min(globalTime, task.end) << "\n";
      }
    }
    Sleep(1000);
  }

  return 0;
}
