#include <Windows.h>
#include <iostream>
#include "funkcje.h"
#include <string>
/*
Założenia projektu:
- punkt zaczepienia znajduje się w prawym górnym narożniku ekranu.
- wybór znaku kodu ASCII;
− wczytanie początkowych rozmiarów figury;
− zmianę wielkości figury klawiszami ′+′ i ′-′;
− przesuwanie figury za pomocą kursorów ←, ↑, →, ↓;
− ograniczenie przesuwania i rozmiarów figury do obszaru ekranu
*/

char wybierzZnak();
void zmienPozycjeKursora(HANDLE hStdout, int x, int y);
void rysujFigure(HANDLE hStdout, int rozmiary, char znak);
int main()
{
    setlocale(LC_ALL, "");
    //Zaczynamy od stworzenia zmiennej pozwalającej zarządać właściwościami okna konsoli
    HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
    //SMALL_RECT srctWindow{};
    //CONSOLE_SCREEN_BUFFER_INFO info;

    //GetConsoleScreenBufferInfo(hStdout, &info);

    //int top = info.srWindow.Top;
    //int right = info.srWindow.Right;
    //COORD c;
    //c.X = right;
    //c.Y = top;
    //std::cout << c.X << " " << c.Y << std::endl;
    rysujFigure(hStdout, 5, 'x');
   

}
