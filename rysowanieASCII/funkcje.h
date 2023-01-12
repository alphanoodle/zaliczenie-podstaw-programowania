#pragma once
#include <iostream>
#include <string>

char wybierzZnak() {
    char znak;
    std::cout << "Wpisz znak jakim chcesz rysowaæ: " << std::endl;
    /*do {
        std::cin >> znak;
    } while ();*/
    std::cin >> znak;
    if (znak == 'q') exit(0);
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
    std::cout << c.X << " " << c.Y << std::endl;
    SetConsoleCursorPosition(hStdout, c);
}
//Funkcja która faktycznie rysuje figure opiera sie na rozwiazaniu problemu "Ile pustych znaków musi pojawiæ sie miêdzy znakami ASCII? w poszczególnycg"
//Innym podejœciem, ktore mo¿na zastosowaæ, jest rysowanie znaku tylko w wybranych miejscach.
void rysujFigure(HANDLE hStdout, int rozmiaryFigury, char znak) {
    int srodek = 0;
    int wiersz = 1;

    //sprawdzam parzystosc rozmiaru figury, któr¹ zamierzam narysowac
    //ten test zdeterminuje jakie zmienne oraz ich wartosci mam przyjac podczas jej rysowania
    if (rozmiaryFigury % 2 == 0) {
        srodek = rozmiaryFigury / 2;

        for (int wiersz = 1; wiersz < srodek; ++wiersz) {
            zmienPozycjeKursora(hStdout, rozmiaryFigury, wiersz);
            std::cout << std::string(wiersz - 1, ' ') << znak << std::string(rozmiaryFigury - (wiersz * 2), ' ') << znak << std::endl;
        }
        for (int i = 0; i < 2; ++i) {
            zmienPozycjeKursora(hStdout, rozmiaryFigury, srodek+i);
            std::cout << std::string(srodek - 1, ' ') << std::string(2, znak) << std::endl;
        }


        for (int wiersz = 1; wiersz < srodek; ++wiersz) {
            zmienPozycjeKursora(hStdout, rozmiaryFigury, srodek+wiersz+1);
            std::cout << std::string(srodek-1-wiersz, ' ') << znak << std::string(wiersz*2, ' ') << znak << std::endl;
        }

    }
    else {
        srodek = (rozmiaryFigury / 2) + 1;

        for (int wiersz = 1; wiersz < srodek; ++wiersz) {
            zmienPozycjeKursora(hStdout, rozmiaryFigury, wiersz);
            std::cout << std::string(wiersz - 1, ' ') << znak << std::string(rozmiaryFigury - (wiersz * 2), ' ') << znak << std::endl;
        }
        zmienPozycjeKursora(hStdout, rozmiaryFigury, srodek);
        std::cout << std::string(srodek - 1, ' ') << znak << std::endl;
        for (int wiersz = 1; wiersz < srodek; ++wiersz) {
            zmienPozycjeKursora(hStdout, rozmiaryFigury, srodek + wiersz);
            std::cout << std::string(srodek - 1 - wiersz, ' ') << znak << std::string(wiersz * 2 -1 , ' ') << znak << std::endl;
        }
    }
};
void przywitanie() {

}
//funkcja pokazuj¹ca pomocne informacje dla uzytkownika
void help() {

 }
//bazowa funkcja s³u¿¹ca do stworzenia UI uzytkownika
void menu(HANDLE hStdout) {
    char znak{};
    znak = wybierzZnak();
    while(TRUE) {
        rysujFigure(hStdout, 100, znak);
        znak = wybierzZnak();
        system("CLS");
    }
}