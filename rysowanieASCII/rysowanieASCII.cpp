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
− przesuwanie figury za pomocą kursorów ←, ↑, →, ↓; check 
− ograniczenie przesuwania i rozmiarów figury do obszaru ekranu check
*/

int main()
{    

    /*
        Ustawiam jezyk na systemowy, aby móc w programie używać polskie znaki diakrytyczne oraz
        wartość argumentu "seed" jako punkt startowy generatora pseudo losowych liczb calkowitych.
        Reszta programu znajduje się w "funkcje.h" oraz "rysowanie.cpp".
    */
    setlocale(LC_ALL, "");
    srand(time(NULL));
    menu();
   

}
