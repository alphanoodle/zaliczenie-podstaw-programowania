#include <Windows.h>
#include <iostream>
#include "funkcje.h"
#include <locale.h>
/*
Założenia projektu:
- punkt zaczepienia znajduje się w prawym górnym narożniku ekranu. check
- wybór znaku kodu ASCII; check
− wczytanie początkowych rozmiarów figury; check
− zmianę wielkości figury klawiszami ′+′ i ′-′;
− przesuwanie figury za pomocą kursorów ←, ↑, →, ↓;
− ograniczenie przesuwania i rozmiarów figury do obszaru ekranu
*/

char wybierzZnak();
void zmienPozycjeKursora(HANDLE hStdout, int x, int y);
void rysujFigure(HANDLE hStdout, int rozmiary, char znak);
void menu(HANDLE hStdout);
void przywitanie(), help();
int main()
{
    setlocale(LC_ALL, "");
    SetConsoleTitle(L"Rysownik figur");
    //Zaczynamy od stworzenia zmiennej pozwalającej zarządać właściwościami okna konsoli
    //potem jako parametr będą przyjmować je wszystkie funkcje
    HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
    menu(hStdout);
   

}
