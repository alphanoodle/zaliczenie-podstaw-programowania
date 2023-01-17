#pragma once
#include <iostream>
#include <string>
#include <locale.h>
#include <Windows.h>
#include <stdio.h>

struct FIGURA{
    int rozmiar = 3;
    COORD translacja = { 0,0 };


    void zmienRozmiar(int wielkoscZmiany, SHORT maksymalnaWysokosc){
        if (wielkoscZmiany > 0 || rozmiar != 3) {
            std::cout << rozmiar << " " << maksymalnaWysokosc;
            rozmiar += wielkoscZmiany;
            if (rozmiar >= maksymalnaWysokosc) {
                rozmiar = maksymalnaWysokosc;
            }
        }
    }
};

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


void zmienPozycjeKursora(HANDLE whnd, int pozycjaX, int pozycjaY, COORD &translacja) {
    CONSOLE_SCREEN_BUFFER_INFO infoOBufferze;
    GetConsoleScreenBufferInfo(whnd, &infoOBufferze);

    SHORT gornaKrawedz = infoOBufferze.srWindow.Top;
    SHORT prawaKrawedz = infoOBufferze.srWindow.Right;

    SHORT dolnaKrawedz = infoOBufferze.srWindow.Bottom;
    SHORT lewaKrawedz = infoOBufferze.srWindow.Left;

    static COORD punktOdniesienia = { 
        prawaKrawedz,
        dolnaKrawedz
    };
    //if (sprawdzZmianeBufforu(prawaKrawedz, dolnaKrawedz)) {
    //    granica.X = prawaKrawedz;
    //    granica.Y = dolnaKrawedz;
    //}
    COORD c;
    
    if (translacja.X > prawaKrawedz - punktOdniesienia.X) {
        translacja.X = prawaKrawedz - punktOdniesienia.X;
    }
    if (abs(translacja.X) > punktOdniesienia.X - pozycjaX) {
        translacja.X = -(punktOdniesienia.X - pozycjaX);
    }
    if (translacja.Y < 0) {
        translacja.Y = 0;
    }
    if (translacja.Y > dolnaKrawedz - pozycjaX) {
        translacja.Y = dolnaKrawedz - pozycjaX;
    }
    //std::cout << translacja.X << " " << translacja.Y;

    c.X = punktOdniesienia.X - pozycjaX + translacja.X;
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
    CONSOLE_SCREEN_BUFFER_INFO infoOBufferze;
    FIGURA a;
    char znak = 'x';
    a.rozmiar = 10;

    DWORD events = 0;
    DWORD eventsread = 0;

    //wybierzPoczatek(znak, rozmiar);
    rysujFigure(whnd,a.rozmiar, znak, a.translacja);
    while (TRUE) {
        GetNumberOfConsoleInputEvents(rhnd, &events);
        if (events != 0) {
            INPUT_RECORD* eventBuffer = new INPUT_RECORD[events];

            ReadConsoleInput(rhnd, eventBuffer, events, &eventsread);
            for (DWORD i = 0; i < eventsread; ++i) {
                if (eventBuffer[i].EventType == KEY_EVENT && eventBuffer[i].Event.KeyEvent.bKeyDown) {
                    system("CLS");
                    GetConsoleScreenBufferInfo(whnd, &infoOBufferze);
                    switch (eventBuffer[i].Event.KeyEvent.wVirtualKeyCode)  {
                        case VK_OEM_PLUS:
                            a.zmienRozmiar(1, infoOBufferze.srWindow.Bottom);
                            break;
                        case VK_OEM_MINUS:
                            a.zmienRozmiar(-1, infoOBufferze.srWindow.Bottom);
                            break;
                        case VK_UP:
                            a.translacja.Y--;
                            break;
                        case VK_DOWN:
                            a.translacja.Y++;
                            break;
                        case VK_RIGHT:
                            a.translacja.X++;
                            break;
                        case VK_LEFT:
                            a.translacja.X--;
                            break;
                        default:
                            std::cout << "not handled yet" << std::endl;
                            break;
                    }
                    rysujFigure(whnd, a.rozmiar, znak, a.translacja);
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