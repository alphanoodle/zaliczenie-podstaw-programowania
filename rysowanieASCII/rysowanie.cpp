#pragma once
#include <iostream>
#include <string>
#include <locale.h>
#include <Windows.h>
#include <stdio.h>
#include "funkcje.h"
#include <stdlib.h>

//funkcja zmieniaj�ca atrybut kursora okre�laj�cy jego widoczno��, w ten spos�b nie wida�
//ka�dego przej�cia kursora
void changeCursorVisibility(HANDLE whnd, BOOL visibility) {
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(whnd, &cursorInfo);        //pozyskanie informacji o kursorze
    cursorInfo.bVisible = visibility;       //zmiana warto�ci atrybutu odpowiadaj�cego za widoczno�� kursora
    SetConsoleCursorInfo(whnd, &cursorInfo);
}

//ta funkcja powsta�a z potrzeby zmiany pozycji kursora po ka�dym znaku nowej linii
//bez wykorzystywania jej w ka�dej kolejnej linii figury ca�a jej zawarto�� by�aby umiejscowiona po prawej stronie 
//ekranu, a chcemy aby program by� responsywny wobec dzia�ania strza�ek.
void zmienPozycjeKursora(HANDLE whnd, int pozycjaX, int pozycjaY, COORD &translacja) {
    CONSOLE_SCREEN_BUFFER_INFO infoOBuforze;
    GetConsoleScreenBufferInfo(whnd, &infoOBuforze);

    //wyznaczam granice ekranu na podstawie informacji o buforze
    SHORT gornaKrawedz = infoOBuforze.srWindow.Top;
    SHORT prawaKrawedz = infoOBuforze.srWindow.Right;

    SHORT dolnaKrawedz = infoOBuforze.srWindow.Bottom;
    SHORT lewaKrawedz = infoOBuforze.srWindow.Left;

    static COORD punktOdniesienia = {       //punkt do kt�rego b�d� si� odnosi� w wypadku zmiany rozmiar�w okna, w ten spos�b uniezale�niaj�c rozmiar figury od rozmiar�w sameg okna
        prawaKrawedz,
        dolnaKrawedz
    };
    COORD c;
    
    //cz�� odpowiadaj�ca za trzymanie figury w granicach ekranu
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
    //finalne ustawienie wsp�rz�dnych, kt�re zmieni� pozycje kursora na osiach X i Y
    c.X = punktOdniesienia.X - pozycjaX + translacja.X;     //prawa kraw�d� - rozmiar figury (poniewa� tym jest przekazywany argument pozycjaX) + dodatkowe przesuni�cie strza�kami
    c.Y = gornaKrawedz + pozycjaY - 1 + translacja.Y;       //prawy g�rny r�g okna + szczeg�lny wierz figury + dodatkowe przesuni�cie strza�kami
    SetConsoleCursorPosition(whnd, c);
}

/*
    Po stworzeniu po raz pierwszy figury ta funkcja ma za zadanie ci�g�e dbanie o responsywno�� programu.
    P�tla "while(true)" dzia�a a� do jej przerwania przez u�ytkownika, wykorzystuj�c bufor konsoli do przyjmowania
    informacji z klawiatury i reakcji na niego w czasie rzeczywistym.
*/
void obsluzInput(HANDLE whnd, HANDLE rhnd, FIGURA &figura) {
    DWORD events = 0;
    DWORD eventsread = 0;
    while (true) {
        GetNumberOfConsoleInputEvents(rhnd, &events);       //odczytaj aktualn� liczbe wydarze� oczekuj�cych na odpowied� w buforze
        if (events != 0) {
            INPUT_RECORD* eventBuffer = new INPUT_RECORD[events];       //stworz tablice tych wydarze�, kt�ra je przechowa nie dopuszczaj�c do ich stracenia

            ReadConsoleInput(rhnd, eventBuffer, events, &eventsread);       //wczytaj te wydarzenia w buforze do tablicy
            if (obsluzEvent(whnd, eventBuffer, figura, eventsread)) {       //je�eli jest to wydarzenie (event), na kt�re program reaguje, to:
                figura.zmienRozmiar(whnd, 0);       //upewnij si�, �e po reakcji na wydarzenie figura jest nadal w ramach okna konsoli
                rysujFigure(whnd, figura);      //narysuj now� figur�
            }
        }
    }
}
/*
    Funkcja odpowiadaj� na pytanie "Czy sta�o si� na klawiaturze co�, co warto obs�u�y�?"
    oraz posiadaj�ca konkretne reakcje na input.
*/
bool obsluzEvent(HANDLE whnd, INPUT_RECORD eventBuffer[], FIGURA& figura, DWORD liczbaCzytanychEventow) {
    CONSOLE_SCREEN_BUFFER_INFO infoOBuforze;
    GetConsoleScreenBufferInfo(whnd, &infoOBuforze);


    static int kolorCzcionki = infoOBuforze.wAttributes;        //zmienn� inicjalizuje atrybutem tekstu sczytnym z bufora w momencie pierwszego narysowania figury, aby konsola by�a w stanie w jakim u�ytkownik j� uruchomi�
    static int kolorT�a = 0;
    bool czyRysowacFlag = 0;        //zwracana zmienna

    for (DWORD i = 0; i < liczbaCzytanychEventow; ++i) {
        czyRysowacFlag = 0;
        if (eventBuffer[i].EventType == KEY_EVENT && eventBuffer[i].Event.KeyEvent.bKeyDown) {      //sprawdzam czy wydarzenie pochodzi z klawiatury oraz czy jest to wci�ni�cie przycisku
            switch (eventBuffer[i].Event.KeyEvent.wVirtualKeyCode) {
                case VK_OEM_PLUS:       //powi�kszanie figury
                    system("CLS");
                    czyRysowacFlag = 1;
                    figura.zmienRozmiar(whnd, 1);
                    break;
                case VK_OEM_MINUS:      //zmniejszanie figury
                    system("CLS");
                    czyRysowacFlag = 1;
                    figura.zmienRozmiar(whnd, -1);
                    break;
                case VK_UP:     //przesuwanie w g�re
                    system("CLS");
                    czyRysowacFlag = 1;
                    figura.translacja.Y--;
                    break;
                case VK_DOWN:       //przesuwanie w d�
                    system("CLS");
                    czyRysowacFlag = 1;
                    figura.translacja.Y++;
                    break;
                case VK_RIGHT:      //przesuwanie w prawo
                    system("CLS");
                    czyRysowacFlag = 1;
                    figura.translacja.X++;
                    break;
                case VK_LEFT:       //przesuwanie w lewo
                    system("CLS");
                    czyRysowacFlag = 1;
                    figura.translacja.X--;
                    break;
                case VK_SPACE:      //zmiana koloru czcionki
                    czyRysowacFlag = 1;
                    SetConsoleTextAttribute(whnd, generatorKoloruCzcionki(kolorCzcionki, kolorT�a));
                    system("CLS");
                    break;
                case VK_TAB:        //zmiana koloru t�a
                    czyRysowacFlag = 1;
                    SetConsoleTextAttribute(whnd, generatorKoloruT�a(kolorCzcionki, kolorT�a));
                    system("CLS");
                    break;
                case 0x48:      //wy�wietlenie pomocy dla u�ytkownika
                    czyRysowacFlag = 1;
                    SetConsoleTextAttribute(whnd, 7);       //zmiana koloru tekstu na co� czytelnego
                    help();
                    SetConsoleTextAttribute(whnd, kolorCzcionki + kolorT�a);        //zmiana do ustawie� pozostawionych przed wywo�aniem help()
                    system("CLS");
                    break;
                case VK_BACK:       //zako�czenie pracy programu przycieksiem Backspace
                    exit(0);
                default:        //domy�lnie, je�eli klawisz nie jest obs�ugiwany, wy�wietlam komunikat o przycisku wy�wietlaj�cym pomoc
                    system("CLS");
                    czyRysowacFlag = 1;
                    std::cout << "By uzyska� pomoc wci�nij 'h'." << std::endl;
                    break;
            }
        }
        return czyRysowacFlag;
    }
    return czyRysowacFlag;
}

//Funkcja kt�ra faktycznie rysuje figure opiera sie na rozwiazaniu problemu "Ile pustych znak�w musi pojawi� sie mi�dzy znakami ASCII? w poszczeg�lnycg"
//Innym podej�ciem, ktore mo�na zastosowa�, jest rysowanie znaku tylko w wybranych miejscach.
void rysujFigure(HANDLE whnd, FIGURA &figura) {
    int srodek;
    int wiersz;

    /*
        Sprawdzam parzystosc rozmiaru figury, kt�r� zamierzam narysowa�.
        Ten test zdeterminuje jak� warto�� ma �rodkowy wiersz figury podczas jej rysowania.
        Metoda rysowania jak� przyj��em sk�ada si� z trzech krok�w:
            1. narysuj cz�� figury do jej �rodka
            2. narysuj �rodek
            3. narysuj od �rodka do ko�ca
        Kroki 1 i 3 sk�adaj� si� z p�tel, kt�re: przechodz� do kolejnej linii na odpowiedni� pozycje dzi�ki
        funkcji zmiany pozycji kursora, rysuj� znaki podany przez u�ytkownia oraz znaki spacji mi�dzy nimi.
    */

    // rozmiar figury jest parzysty
    if (figura.rozmiar % 2 == 0) {
        srodek = figura.rozmiar / 2;

        for (wiersz = 1; wiersz < srodek; ++wiersz) {
            zmienPozycjeKursora(whnd, figura.rozmiar, wiersz, figura.translacja);
            std::cout << std::string((wiersz - 1), ' ') << figura.znak << std::string((figura.rozmiar - (wiersz * 2)), ' ') << figura.znak << std::endl;
        }

        for (int i = 0; i < 2; ++i) {
            zmienPozycjeKursora(whnd, figura.rozmiar, srodek + i, figura.translacja);
            std::cout << std::string((srodek - 1), ' ') << std::string(2, figura.znak) << std::endl;
        }

        for (wiersz = 1; wiersz < srodek; ++wiersz) {
            zmienPozycjeKursora(whnd, figura.rozmiar, srodek + wiersz + 1, figura.translacja);
            std::cout << std::string((srodek - 1 - wiersz), ' ') << figura.znak << std::string((wiersz * 2), ' ') << figura.znak << std::endl;
        }

    }
    //rozmiar figury jest nieparzysty
    else {
        srodek = (figura.rozmiar / 2) + 1;

        for (wiersz = 1; wiersz < srodek; ++wiersz) {
            zmienPozycjeKursora(whnd, figura.rozmiar, wiersz, figura.translacja);
            std::cout << std::string((wiersz - 1), ' ') << figura.znak << std::string((figura.rozmiar - (wiersz * 2)), ' ') << figura.znak << std::endl;
        }

        zmienPozycjeKursora(whnd, figura.rozmiar, srodek, figura.translacja);
        std::cout << std::string((srodek - 1), ' ') << figura.znak << std::endl;

        for (wiersz = 1; wiersz < srodek; ++wiersz) {
            zmienPozycjeKursora(whnd, figura.rozmiar, srodek + wiersz, figura.translacja);
            std::cout << std::string((srodek - 1) - wiersz, ' ') << figura.znak << std::string((wiersz * 2 - 1), ' ') << figura.znak << std::endl;
        }
    }
};


/*
    Kolejne dwie funkcje s�u�� do zmiany kolor�w czcionki i t�a konsoli.
    Parametry przekazywane przez referencje maj� za zadanie zmieni� warto�� na now�,
    za� te przekazywane przez wartosci maj� pozosta� niezmienne.
    Ma to na celu rozdzielenie t�a od czcionki, aby u�ytkownik zmienia� tylko jedn� warto�� na raz.
*/
int generatorKoloruCzcionki(int& obecnyKolorCzcionki, int obecnyKolorT�a) {
    obecnyKolorCzcionki = rand() % 15;
    return obecnyKolorT�a + obecnyKolorCzcionki;
}

/*
    Tablica kolory t�a przechowuje warto�ci r�nych mo�liwych kolor�w t�a.
    Pro�ciej by�o mi wypisa� dok�adne warto�ci zamiast stworzy� osobn� p�tle do ich wygenerowania z racji na natur� funkcji "SetConsoleTextAttribute".
    Jej atrybuty "wAttributes" s� stale zdefiniowane, jednak posiadaj� r�wnie� swoj� reprezentacje w systemie szesnastowym.
    Podstawowymi warto�ciami kolor�w t�a s� BLUE, GREEN, RED i INTENSITY, kt�rych odpowiedniki to 0x0010, 0x0020, 0x0040, 0x0080.
    Dodaj�c ich warto�ci powstaje zbi�r kombinacji umieszczony w tabeli koloryT�a, z kt�rej nast�pnie losowana jest jedna warto��.
*/
int generatorKoloruT�a(int obecnyKolorCzcionki, int& obecnyKolorT�a) {
    int koloryTla[] = { 16, 32, 48, 64, 80, 96, 112, 128, 144, 160, 176, 192, 208, 224, 240};
    obecnyKolorT�a = koloryTla[rand() % 15];
    return obecnyKolorT�a + obecnyKolorCzcionki;
}
//Funkcja maj�ca pom�c u�ytkownikowi w rozpoznaniu wszystkich mo�liwo�ci programu.
//Zamkni�cie jej odbywa si� poprzez wci�ni�cie jakiegokolwiek przycisku, a wywo�ywana jest przez wci�ni�cie 'h'.
void help() {
    system("CLS");

    std::cout << "-----------------------------------------------POMOC-----------------------------------------------" << std::endl;
    std::cout << " Dzia�anie klawiszy:" << std::endl;
    std::cout << "\tStrza�ki - poruszaj� figur�" << std::endl;
    std::cout << "\tPlus i minus - zwi�kszaja/zmniejsza rozmiar figury (maksymalnie do rozmiar�w okna konsoli)" << std::endl;
    std::cout << "\tSpace - zmienia kolory czcionki konsoli na losowo wybrany przez program." << std::endl;
    std::cout << "\tTabulator - zmienia kolor t�a konsoli na losowo wybrany przez program." << std::endl;
    std::cout << "\t'h' - wy�wietla t� pomoc (ten komunikat)" << std::endl;
    std::cout << "\tBackspace - zaka�cza dzia�anie programu\n" << std::endl;
    system("pause");
}

/*
    Funkcja odpowiadaj�ca za stworzenie instancji struktury FIGURE, kt�r� b�dzie nasz iks,
    zmian� tytu�u programu na co� odpowiedniego, stworzenie uchwyt�w do bufor�w konsoli, przyj�cie
    argument�w u�ytkownika, i wreszcie narysowanie samej figury
*/
void menu() {
    FIGURA iks;
    SetConsoleTitle(L"Rysownik figur");
    HANDLE whnd = GetStdHandle(STD_OUTPUT_HANDLE);
    HANDLE rhnd = GetStdHandle(STD_INPUT_HANDLE);

    changeCursorVisibility(whnd, 0);
    iks.wybierzPoczatek(whnd);
    system("CLS");
    rysujFigure(whnd, iks);
    obsluzInput(whnd, rhnd, iks);
}