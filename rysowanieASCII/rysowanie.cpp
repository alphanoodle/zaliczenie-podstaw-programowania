#pragma once
#include <iostream>
#include <string>
#include <locale.h>
#include <Windows.h>
#include <stdio.h>

void wybierzPoczatek(char &znak, int &rozmiar) {
    std::cout << "Wpisz znak jakim chcesz rysowaæ: " << std::endl;
    std::cin >> znak;
    std::cout << "Podaj rozmiar figury: " << std::endl;
    std::cin >> rozmiar;
    system("CLS");
}

void zmienPozycjeKursora(HANDLE hStdout, int pozycjaX, int pozycjaY, int &offsetX, int &offsetY) {
    SMALL_RECT srctWindow{};
    CONSOLE_SCREEN_BUFFER_INFO info;

    GetConsoleScreenBufferInfo(hStdout, &info);

    int gornaKrawedz = info.srWindow.Top;
    int prawaKrawedz = info.srWindow.Right;
    COORD c;
    c.X = prawaKrawedz - pozycjaX + offsetX;
    std::cout << offsetX << " ";
    if (c.X > prawaKrawedz - pozycjaX) { 
        c.X = prawaKrawedz - pozycjaX;
        offsetX -= 1;
    }
    std::cout << offsetX << " ";
    c.Y = gornaKrawedz + pozycjaY - 1 + offsetY;
    //std::cout << c.X << " " << c.Y << std::endl;
    SetConsoleCursorPosition(hStdout, c);
}
//Funkcja która faktycznie rysuje figure opiera sie na rozwiazaniu problemu "Ile pustych znaków musi pojawiæ sie miêdzy znakami ASCII? w poszczególnycg"
//Innym podejœciem, ktore mo¿na zastosowaæ, jest rysowanie znaku tylko w wybranych miejscach.
void rysujFigure(HANDLE hStdout, int rozmiaryFigury, char znak, int offsetX = 0, int offsetY = 0) {
    int srodek;
    int wiersz;

    //sprawdzam parzystosc rozmiaru figury, któr¹ zamierzam narysowac
    //ten test zdeterminuje jakie zmienne oraz ich wartosci mam przyjac podczas jej rysowania
    if (rozmiaryFigury % 2 == 0) {
        srodek = rozmiaryFigury / 2;

        for (int wiersz = 1; wiersz < srodek; ++wiersz) {
            zmienPozycjeKursora(hStdout, rozmiaryFigury, wiersz, offsetX, offsetY);
            std::cout << std::string(wiersz - 1, ' ') << znak << std::string(rozmiaryFigury - (wiersz * 2), ' ') << znak << std::endl;
        }
        for (int i = 0; i < 2; ++i) {
            zmienPozycjeKursora(hStdout, rozmiaryFigury, srodek + i, offsetX, offsetY);
            std::cout << std::string(srodek - 1, ' ') << std::string(2, znak) << std::endl;
        }


        for (int wiersz = 1; wiersz < srodek; ++wiersz) {
            zmienPozycjeKursora(hStdout, rozmiaryFigury, srodek + wiersz + 1, offsetX, offsetY);
            std::cout << std::string(srodek - 1 - wiersz, ' ') << znak << std::string(wiersz * 2, ' ') << znak << std::endl;
        }

    }
    else {
        srodek = (rozmiaryFigury / 2) + 1;

        for (int wiersz = 1; wiersz < srodek; ++wiersz) {
            zmienPozycjeKursora(hStdout, rozmiaryFigury, wiersz, offsetX, offsetY);
            std::cout << std::string(wiersz - 1, ' ') << znak << std::string(rozmiaryFigury - (wiersz * 2), ' ') << znak << std::endl;
        }
        zmienPozycjeKursora(hStdout, rozmiaryFigury, srodek, offsetX, offsetY);
        std::cout << std::string(srodek - 1, ' ') << znak << std::endl;
        for (int wiersz = 1; wiersz < srodek; ++wiersz) {
            zmienPozycjeKursora(hStdout, rozmiaryFigury, srodek + wiersz, offsetX, offsetY);
            std::cout << std::string(srodek - 1 - wiersz, ' ') << znak << std::string(wiersz * 2 - 1, ' ') << znak << std::endl;
        }
    }
};
//bazowa funkcja s³u¿¹ca do stworzenia UI uzytkownika
void menu(HANDLE whnd, HANDLE rhnd) {
    char znak = 'x';
    int rozmiar = 10;

    DWORD events = 0;
    DWORD eventsread = 0;

    int offsetX = 0;
    int offsetY = 0;
    //wybierzPoczatek(znak, rozmiar);
    rysujFigure(whnd, rozmiar, znak);
    while (TRUE) {
        GetNumberOfConsoleInputEvents(rhnd, &events);
        if (events != 0) {
            INPUT_RECORD* eventBuffer = new INPUT_RECORD[events];

            ReadConsoleInput(rhnd, eventBuffer, events, &eventsread);
            for (DWORD i = 0; i < eventsread; ++i) {
                if (eventBuffer[i].EventType == KEY_EVENT && eventBuffer[i].Event.KeyEvent.bKeyDown) {
                    system("CLS");
                    switch (eventBuffer[i].Event.KeyEvent.wVirtualKeyCode)  {
                        case VK_OEM_PLUS:
                            rozmiar++;
                            break;
                        case VK_OEM_MINUS:
                            if (rozmiar != 3) { rozmiar--; }
                            break;
                        case VK_UP:
                            offsetY--;
                            break;
                        case VK_DOWN:
                            offsetY++;
                            break;
                        case VK_RIGHT:
                            offsetX++;
                            break;
                        case VK_LEFT:
                            offsetX--;
                            break;
                        default:
                            std::cout << "not handled yet" << std::endl;
                            break;
                    }
                    rysujFigure(whnd, rozmiar, znak, offsetX, offsetY);
                    std::cout << offsetX << std::endl;

                }

            }
        }
    }
}

void changeCursorVisibility(HANDLE whnd, BOOL visibility) {
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(whnd, &cursorInfo);
    cursorInfo.bVisible = visibility;
    SetConsoleCursorInfo(whnd, &cursorInfo);
}