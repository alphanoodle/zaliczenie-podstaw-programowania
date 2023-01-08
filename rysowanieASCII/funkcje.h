#pragma once
#include <iostream>


char wybierzZnak() {
    char znak;
    std::cout << "Wpisz znak jakim chcesz rysowac" << std::endl;
    /*do {
        std::cin >> znak;
    } while ();*/
    std::cin >> znak;
    return znak;
}

void zmienPozycjeKursora(HANDLE hStdout, int x=1, int y=1) {
    SMALL_RECT srctWindow{};
    CONSOLE_SCREEN_BUFFER_INFO info;

    GetConsoleScreenBufferInfo(hStdout, &info);

    int top = info.srWindow.Top;
    int right = info.srWindow.Right;
    COORD c;
    c.X = right-x;
    c.Y = top + y - 1;
    //std::cout << c.X << " " << c.Y << std::endl;
    SetConsoleCursorPosition(hStdout, c);
}
//Funkcja która faktycznie rysuje figure opiera sie na rozwiazaniu problemu "Ile pustych znaków musi pojawić sie między znakami ASCII? w poszczególnycg"
//Innym podejściem, ktore można zastosować, jest rysowanie znaku tylko w wybranych miejscach.
void rysujFigure(HANDLE hStdout, int rozmiary, char znak) {
    int srodek = 0;
    int wiersz = 1;

    //sprawdzam parzystosc rozmiaryi, czy wielkosci, figury ktora zamierzam narysowac
    //ten test zdeterminuje jakie zmienne oraz wartosci mam przyjac podczas jej rysowania
    if (rozmiary % 2 == 0) {
        srodek = rozmiary / 2;

        for (int wiersz = 1; wiersz < srodek; ++wiersz) {
            zmienPozycjeKursora(hStdout, rozmiary, wiersz);
            std::cout << std::string(wiersz - 1, ' ') << znak << std::string(rozmiary - (wiersz * 2), ' ') << znak << std::endl;
        }
        for (int i = 0; i < 2; ++i) {
            zmienPozycjeKursora(hStdout, rozmiary, srodek+i);
            std::cout << std::string(srodek - 1, ' ') << std::string(2, znak) << std::endl;
        }


        for (int wiersz = 1; wiersz < srodek; ++wiersz) {
            zmienPozycjeKursora(hStdout, rozmiary, srodek+wiersz);
            std::cout << std::string(srodek-1-wiersz, ' ') << znak << std::string(wiersz*2, ' ') << znak << std::endl;
        }

    }
    else {
        srodek = (rozmiary / 2) + 1;

        for (int wiersz = 1; wiersz < srodek; ++wiersz) {
            zmienPozycjeKursora(hStdout, rozmiary, wiersz);
            std::cout << std::string(wiersz - 1, ' ') << znak << std::string(rozmiary - (wiersz * 2), ' ') << znak << std::endl;
        }
        zmienPozycjeKursora(hStdout, rozmiary, srodek);
        std::cout << std::string(srodek - 1, ' ') << znak << std::endl;
        for (int wiersz = 1; wiersz < srodek; ++wiersz) {
            zmienPozycjeKursora(hStdout, rozmiary, srodek + wiersz);
            std::cout << std::string(srodek - 1 - wiersz, ' ') << znak << std::string(wiersz * 2 -1 , ' ') << znak << std::endl;
        }
    }



};