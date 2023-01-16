#include <Windows.h>
#include <iostream>
#include "funkcje.h"
#include <locale.h>
/*
Założenia projektu:
- punkt zaczepienia znajduje się w prawym górnym narożniku ekranu. check
- wybór znaku kodu ASCII; check
− wczytanie początkowych rozmiarów figury; check
− zmianę wielkości figury klawiszami ′+′ i ′-′; check
− przesuwanie figury za pomocą kursorów ←, ↑, →, ↓;
− ograniczenie przesuwania i rozmiarów figury do obszaru ekranu
*/

int main()
{
    setlocale(LC_ALL, "");
    SetConsoleTitle(L"Rysownik figur");
    //Zaczynamy od stworzenia zmiennej pozwalającej zarządać właściwościami okna konsoli
    //potem jako parametr będą przyjmować je wszystkie funkcje
    HANDLE whnd = GetStdHandle(STD_OUTPUT_HANDLE);
    HANDLE rhnd = GetStdHandle(STD_INPUT_HANDLE);
    changeCursorVisibility(whnd, 0);
    menu(whnd, rhnd);
   

}
