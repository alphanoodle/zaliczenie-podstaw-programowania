#pragma once
#include <iostream>
#include <string>
#include <locale.h>
#include <Windows.h>
#include <stdio.h>


void changeCursorVisibility(HANDLE whnd, BOOL visibility) {
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(whnd, &cursorInfo);
    cursorInfo.bVisible = visibility;
    SetConsoleCursorInfo(whnd, &cursorInfo);
}

void wybierzPoczatek(char &znak, int &rozmiar) {
    std::cout << "Wpisz znak jakim chcesz rysowaæ: " << std::endl;
    std::cin >> znak;
    std::cout << "Podaj rozmiar figury: " << std::endl;
    std::cin >> rozmiar;
    system("CLS");
}

bool sprawdzZmianeBufforu(SHORT prawaKrawedz, SHORT dolnaKrawedz) {
    bool windowResized = true;
    static SHORT oldRight = 0;
    static SHORT oldBottom = 0;

    if (!oldRight && !oldBottom) {
        oldBottom = dolnaKrawedz;
        oldRight = prawaKrawedz;
    }
    if (prawaKrawedz != oldRight || dolnaKrawedz != oldBottom) {
        oldBottom = dolnaKrawedz;
        oldRight = prawaKrawedz;
        windowResized = false;
    }
    return windowResized;
}

void zmienPozycjeKursora(HANDLE whnd, int pozycjaX, int pozycjaY, COORD &translacja) {
    CONSOLE_SCREEN_BUFFER_INFO infoOBufferze;
    GetConsoleScreenBufferInfo(whnd, &infoOBufferze);

    SHORT gornaKrawedz = infoOBufferze.srWindow.Top;
    SHORT prawaKrawedz = infoOBufferze.srWindow.Right;

    SHORT dolnaKrawedz = infoOBufferze.srWindow.Bottom;
    SHORT lewaKrawedz = infoOBufferze.srWindow.Left;

    static COORD granica = { 
        prawaKrawedz,
        dolnaKrawedz
    };
    std::cout << granica.X << " " << granica.Y << " : ";
    std::cout << sprawdzZmianeBufforu(prawaKrawedz, dolnaKrawedz);
    if (sprawdzZmianeBufforu(prawaKrawedz, dolnaKrawedz)) {
        granica.X = prawaKrawedz;
        granica.Y = dolnaKrawedz;
    }
    std::cout << " " << granica.X << " " << granica.Y;
    COORD c;
    if (translacja.X + prawaKrawedz > granica.X) {
        translacja.X = granica.X - prawaKrawedz;
    }
    if (abs(translacja.X) > granica.X - pozycjaX) {
        translacja.X = -(granica.X - pozycjaX);
    }
    if (translacja.Y < 0) {
        translacja.Y = 0;
    }
    if (translacja.Y > granica.Y - pozycjaX) {
        translacja.Y = granica.Y - pozycjaX;
    }
    //std::cout << translacja.X << " " << translacja.Y;

    c.X = granica.X - pozycjaX + translacja.X;
    c.Y = gornaKrawedz + pozycjaY - 1 + translacja.Y;
    SetConsoleCursorPosition(whnd, c);
}
//Funkcja która faktycznie rysuje figure opiera sie na rozwiazaniu problemu "Ile pustych znaków musi pojawiæ sie miêdzy znakami ASCII? w poszczególnycg"
//Innym podejœciem, ktore mo¿na zastosowaæ, jest rysowanie znaku tylko w wybranych miejscach.
void rysujFigure(HANDLE whnd, int rozmiaryFigury, char znak, COORD& tanslacja) {
    changeCursorVisibility(whnd, 0);
    int srodek;
    int wiersz;

    //sprawdzam parzystosc rozmiaru figury, któr¹ zamierzam narysowac
    //ten test zdeterminuje jakie zmienne oraz ich wartosci mam przyjac podczas jej rysowania
    if (rozmiaryFigury % 2 == 0) {
        srodek = rozmiaryFigury / 2;

        for (int wiersz = 1; wiersz < srodek; ++wiersz) {
            zmienPozycjeKursora(whnd, rozmiaryFigury, wiersz, tanslacja);
            std::cout << std::string(wiersz - 1, ' ') << znak << std::string(rozmiaryFigury - (wiersz * 2), ' ') << znak << std::endl;
        }
        for (int i = 0; i < 2; ++i) {
            zmienPozycjeKursora(whnd, rozmiaryFigury, srodek + i, tanslacja);
            std::cout << std::string(srodek - 1, ' ') << std::string(2, znak) << std::endl;
        }


        for (int wiersz = 1; wiersz < srodek; ++wiersz) {
            zmienPozycjeKursora(whnd, rozmiaryFigury, srodek + wiersz + 1, tanslacja);
            std::cout << std::string(srodek - 1 - wiersz, ' ') << znak << std::string(wiersz * 2, ' ') << znak << std::endl;
        }

    }
    else {
        srodek = (rozmiaryFigury / 2) + 1;

        for (int wiersz = 1; wiersz < srodek; ++wiersz) {
            zmienPozycjeKursora(whnd, rozmiaryFigury, wiersz, tanslacja);
            std::cout << std::string(wiersz - 1, ' ') << znak << std::string(rozmiaryFigury - (wiersz * 2), ' ') << znak << std::endl;
        }
        zmienPozycjeKursora(whnd, rozmiaryFigury, srodek, tanslacja);
        std::cout << std::string(srodek - 1, ' ') << znak << std::endl;
        for (int wiersz = 1; wiersz < srodek; ++wiersz) {
            zmienPozycjeKursora(whnd, rozmiaryFigury, srodek + wiersz, tanslacja);
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

    COORD translacjaOWektor{ 0,0 };
    //wybierzPoczatek(znak, rozmiar);
    rysujFigure(whnd,rozmiar, znak, translacjaOWektor);
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
                            translacjaOWektor.Y--;
                            break;
                        case VK_DOWN:
                            translacjaOWektor.Y++;
                            break;
                        case VK_RIGHT:
                            translacjaOWektor.X++;
                            break;
                        case VK_LEFT:
                            translacjaOWektor.X--;
                            break;
                        default:
                            std::cout << "not handled yet" << std::endl;
                            break;
                    }
                    rysujFigure(whnd, rozmiar, znak, translacjaOWektor);
                }
                //if (eventBuffer[i].EventType == WINDOW_BUFFER_SIZE_EVENT) {
                //    system("CLS");
                //    std::cout << "jajco" << std::endl;
                //    GetConsoleScreenBufferInfo(whnd, &infoOBufferze);
                //    std::cout << eventBuffer[i].Event.WindowBufferSizeEvent.dwSize.X << " " << eventBuffer[i].Event.WindowBufferSizeEvent.dwSize.Y << std::endl;

                //    //std::cout << eventBuffer[i].Event.WindowBufferSizeEvent.dwSize.X << " " << eventBuffer[i].Event.WindowBufferSizeEvent.dwSize.Y << std::endl;
                //    //std::cout << infoOBufferze.srWindow.Left << " " << infoOBufferze.srWindow.Top << " " << infoOBufferze.srWindow.Right << " " << infoOBufferze.srWindow.Bottom << std::endl;
                //    rysujFigure(whnd, infoOBufferze, rozmiar, znak, translacjaOWektor);
                //}
            }
        }
    }
}