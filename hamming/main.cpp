// http://archive.dimacs.rutgers.edu/drei/1997/classroom/lessons/hamming.html

// Computación tolerante a fallas, D02, Equipo 02
#include <bitset>
#include <cmath>
#include <iostream>
#include <string>
using namespace std;

/*
  Se programará el código de Hamming para corregir más de 1 bit de error en él envió de datos.
  - La entrada de dato será una trama de bits con uno o más errores que ya se encuentra codificada en Hamming.
  - Mostrará en la salida de pantalla la trama recibida y remarcara donde se ha hecho la corrección de dato.
  - El programa deberá estar validado para entradas erróneas distintas a binario.
*/

struct Hamming {
  bool isValid(const string& str, const string alphabet = "01") {
    for (auto c : str) {
      bool exists = false;
      for (int i = 0; i < alphabet.size() && !exists; i += 2)
        if (alphabet[i] <= c && c <= alphabet[i + 1])
          exists = true;
      if (!exists)
        return false;
    }
    return true;
  }

  int outputSize(const string& str) {
    int n = str.size();
    for (int i = 0;; ++i)
      if (pow(2, i) >= n + i + 1)
        return i + n;
    return 0;
  }

  int inputSize(const string& hammingCode) {
    int k = 0;
    while (pow(2, k) < hammingCode.size())
      k++;
    return hammingCode.size() - k;
  }

  string calculateHamming(const string& input) {
    if (isValid(input)) {
      // cout << "Valido\n";

      string output(outputSize(input), '-');
      auto it = input.begin();
      int k = output.size() - input.size();

      // Find the output string with input data entered without parity bits
      for (int position = 0, index = 0; it != input.end(); ++position) {
        if (pow(2, index) == position + 1) {
          ++index;
          continue;
        }
        output[position] = *it++;
      }

      // cout << "Output(Without Parity Bits) : " << output << '\n';

      // Find the output string now with parity bits
      bitset<10> bits;
      for (int index = 0; index < k; ++index) {
        int positionOfParity = 0;
        int countOfOnes = 0;
        for (int c = 1; c <= output.size(); ++c) {
          bits = c;
          // index from 0 to 3
          if (bits.test(index)) {
            if (output[c - 1] == '-')
              positionOfParity = c - 1;
            else if (output[c - 1] == '1')
              ++countOfOnes;
          }
        }
        output[positionOfParity] = char('0' + countOfOnes % 2);
      }

      return output;
    } else {
      // cout << "Invalido\n";
      return "";
    }
  }

  int findErrorsHamming(string hammingCode) {
    if (isValid(hammingCode) && hammingCode.size() > 4) {
      cout << "Es una string válida\n";

      string correctHammingCode = findCorrectHammingCode(hammingCode);
      if (correctHammingCode == hammingCode) {
        cout << "No hay errores\n";
      } else {
        cout << "Hamming correcto : " << correctHammingCode << '\n';
        if (correctHammingCode.size() == hammingCode.size()) {
          cout << "Errores en las posiciones : ";
          for (int index = 0; index != hammingCode.size(); ++index) {
            if (correctHammingCode[index] != hammingCode[index])
              cout << " " << index + 1;
          }
        } else {
          cout << "Hamming inválido\n";
        }
      }
    } else {
      cout << "Entrada inválida\n";
      return -1;
    }

    return 0;
  }

  string findCorrectHammingCode(const string& hammingCode) {
    bitset<10> output(0);
    bitset<10> bits(0);
    size_t k = hammingCode.size() - inputSize(hammingCode);
    bool error = false;
    for (size_t c = 0; c < k; ++c) {
      int countOfOnes = 0;
      for (size_t index = 1; index <= hammingCode.size(); ++index) {
        bits = index;
        if (bits.test(c)) {
          if (hammingCode[index - 1] == '1')
            ++countOfOnes;
        }
      }
      if (countOfOnes % 2 != 0) {
        output.set(c, 1);
        error = true;
      }
    }
    if (error) {
      unsigned int finalIndex = (unsigned int)output.to_ulong();
      string outputstring = hammingCode;
      outputstring[finalIndex - 1] = outputstring[finalIndex - 1] ^ 1;
      return outputstring;
    } else {
      return hammingCode;
    }
  }

  void toLowerCase(string& s) {
    for (char& c : s)
      c = tolower(c);
  }

  string toBinary(string word) {
    string binary;
    // reverse(word.begin(), word.end());
    for (char c : word) {
      bitset<5> bs(c - 'a');
      // cout << c << " " << bs.to_string() << '\n';
      binary += bs.to_string();
    }
    return binary;
  }

  string toAlpha(const string& binary) {
    cout << "Binario: " << binary << '\n';
    string word;
    for (int i = 0; i < binary.size(); i += 5) {
      bitset<5> bs(binary.substr(i, 5));
      cout << binary.substr(i, 5) << " " << bs.to_ulong() << '\n';
      word += char('a' + bs.to_ulong());
    }
    return word;
  }

  string solve(string& word) {
    toLowerCase(word);
    if (isValid(word, "az")) {
      string binary = toBinary(word);
      string hamming = findCorrectHammingCode(binary);

      cout << toAlpha(binary) << '\n';
      cout << toAlpha(hamming) << '\n';

      return hamming;
    } else {
      return "La cadena contiene caracteres que no corresponden al alfabeto.";
    }
  }
};

int main() {
  Hamming hamming;

  while (1) {
    cout << "Ingresa una palabra del español o el caracter '?' para salir \n";
    string word;
    cin >> word;

    if (word == "?") {
      cout << "Fin\n";
      break;
    }

    cout << hamming.solve(word) << '\n';
  }

  return 0;
}
