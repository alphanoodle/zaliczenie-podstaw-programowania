#pragma once
#include <iostream>
#include <string>
#include <locale.h>
#include <Windows.h>
#include <stdio.h>
#include "funkcje.h"
#include <stdlib.h>

//funkcja zmieniaj¹ca atrybut kursora okreœlaj¹cy jego widocznoœæ, w ten sposób nie widaæ
//ka¿dego przejœcia kursora
void changeCursorVisibility(HANDLE whnd, BOOL visibility) {
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(whnd, &cursorInfo);        //pozyskanie informacji o kursorze
    cursorInfo.bVisible = visibility;       //zmiana wartoœci atrybutu odpowiadaj¹cego za widocznoœæ kursora
    SetConsoleCursorInfo(whnd, &cursorInfo);
}

//ta funkcja powsta³a z potrzeby zmiany pozycji kursora po ka¿dym znaku nowej linii
//bez wykorzystywania jej w ka¿dej kolejnej linii figury ca³a jej zawartoœæ by³aby umiejscowiona po prawej stronie 
//ekranu, a chcemy aby program by³ responsywny wobec dzia³ania strza³ek.
void zmienPozycjeKursora(HANDLE whnd, int pozycjaX, int pozycjaY, COORD &translacja) {
    CONSOLE_SCREEN_BUFFER_INFO infoOBuforze;
    GetConsoleScreenBufferInfo(whnd, &infoOBuforze);

    //wyznaczam granice ekranu na podstawie informacji o buforze
    SHORT gornaKrawedz = infoOBuforze.srWindow.Top;
    SHORT prawaKrawedz = infoOBuforze.srWindow.Right;

    SHORT dolnaKrawedz = infoOBuforze.srWindow.Bottom;
    SHORT lewaKrawedz = infoOBuforze.srWindow.Left;

    static COORD punktOdniesienia = {       //punkt do którego bêdê siê odnosi³ w wypadku zmiany rozmiarów okna, w ten sposób uniezale¿niaj¹c rozmiar figury od rozmiarów sameg okna
        prawaKrawedz,
        dolnaKrawedz
    };
    COORD c;
    
    //czêœæ odpowiadaj¹ca za trzymanie figury w granicach ekranu
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
    //finalne ustawienie wspó³rzêdnych, które zmieni¹ pozycje kursora na osiach X i Y
    c.X = punktOdniesienia.X - pozycjaX + translacja.X;     //prawa krawêdŸ - rozmiar figury (poniewa¿ tym jest przekazywany argument pozycjaX) + dodatkowe przesuniêcie strza³kami
    c.Y = gornaKrawedz + pozycjaY - 1 + translacja.Y;       //prawy górny róg okna + szczególny wierz figury + dodatkowe przesuniêcie strza³kami
    SetConsoleCursorPosition(whnd, c);
}

/*
    Po stworzeniu po raz pierwszy figury ta funkcja ma za zadanie ci¹g³e dbanie o responsywnoœæ programu.
    Pêtla "while(true)" dzia³a a¿ do jej przerwania przez u¿ytkownika, wykorzystuj¹c bufor konsoli do przyjmowania
    informacji z klawiatury i reakcji na niego w czasie rzeczywistym.
*/
void obsluzInput(HANDLE whnd, HANDLE rhnd, FIGURA &figura) {
    DWORD events = 0;
    DWORD eventsread = 0;
    while (true) {
        GetNumberOfConsoleInputEvents(rhnd, &events);       //odczytaj aktualn¹ liczbe wydarzeñ oczekuj¹cych na odpowiedŸ w buforze
        if (events != 0) {
            INPUT_RECORD* eventBuffer = new INPUT_RECORD[events];       //stworz tablice tych wydarzeñ, która je przechowa nie dopuszczaj¹c do ich stracenia

            ReadConsoleInput(rhnd, eventBuffer, events, &eventsread);       //wczytaj te wydarzenia w buforze do tablicy
            if (obsluzEvent(whnd, eventBuffer, figura, eventsread)) {       //je¿eli jest to wydarzenie (event), na które program reaguje, to:
                figura.zmienRozmiar(whnd, 0);       //upewnij siê, ¿e po reakcji na wydarzenie figura jest nadal w ramach okna konsoli
                rysujFigure(whnd, figura);      //narysuj now¹ figurê
            }
        }
    }
}
/*
    Funkcja odpowiadaj¹ na pytanie "Czy sta³o siê na klawiaturze coœ, co warto obs³u¿yæ?"
    oraz posiadaj¹ca konkretne reakcje na input.
*/
bool obsluzEvent(HANDLE whnd, INPUT_RECORD eventBuffer[], FIGURA& figura, DWORD liczbaCzytanychEventow) {
    CONSOLE_SCREEN_BUFFER_INFO infoOBuforze;
    GetConsoleScreenBufferInfo(whnd, &infoOBuforze);


    static int kolorCzcionki = infoOBuforze.wAttributes;        //zmienn¹ inicjalizuje atrybutem tekstu sczytnym z bufora w momencie pierwszego narysowania figury, aby konsola by³a w stanie w jakim u¿ytkownik j¹ uruchomi³
    static int kolorT³a = 0;
    bool czyRysowacFlag = 0;        //zwracana zmienna

    for (DWORD i = 0; i < liczbaCzytanychEventow; ++i) {
        czyRysowacFlag = 0;
        if (eventBuffer[i].EventType == KEY_EVENT && eventBuffer[i].Event.KeyEvent.bKeyDown) {      //sprawdzam czy wydarzenie pochodzi z klawiatury oraz czy jest to wciœniêcie przycisku
            switch (eventBuffer[i].Event.KeyEvent.wVirtualKeyCode) {
                case VK_OEM_PLUS:       //powiêkszanie figury
                    system("CLS");
                    czyRysowacFlag = 1;
                    figura.zmienRozmiar(whnd, 1);
                    break;
                case VK_OEM_MINUS:      //zmniejszanie figury
                    system("CLS");
                    czyRysowacFlag = 1;
                    figura.zmienRozmiar(whnd, -1);
                    break;
                case VK_UP:     //przesuwanie w góre
                    system("CLS");
                    czyRysowacFlag = 1;
                    figura.translacja.Y--;
                    break;
                case VK_DOWN:       //przesuwanie w dó³
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
                    SetConsoleTextAttribute(whnd, generatorKoloruCzcionki(kolorCzcionki, kolorT³a));
                    system("CLS");
                    break;
                case VK_TAB:        //zmiana koloru t³a
                    czyRysowacFlag = 1;
                    SetConsoleTextAttribute(whnd, generatorKoloruT³a(kolorCzcionki, kolorT³a));
                    system("CLS");
                    break;
                case 0x48:      //wyœwietlenie pomocy dla u¿ytkownika
                    czyRysowacFlag = 1;
                    SetConsoleTextAttribute(whnd, 7);       //zmiana koloru tekstu na coœ czytelnego
                    help();
                    SetConsoleTextAttribute(whnd, kolorCzcionki + kolorT³a);        //zmiana do ustawieñ pozostawionych przed wywo³aniem help()
                    system("CLS");
                    break;
                case VK_BACK:       //zakoñczenie pracy programu przycieksiem Backspace
                    exit(0);
                default:        //domyœlnie, je¿eli klawisz nie jest obs³ugiwany, wyœwietlam komunikat o przycisku wyœwietlaj¹cym pomoc
                    system("CLS");
                    czyRysowacFlag = 1;
                    std::cout << "By uzyskaæ pomoc wciœnij 'h'." << std::endl;
                    break;
            }
        }
        return czyRysowacFlag;
    }
    return czyRysowacFlag;
}

//Funkcja która faktycznie rysuje figure opiera sie na rozwiazaniu problemu "Ile pustych znaków musi pojawiæ sie miêdzy znakami ASCII? w poszczególnycg"
//Innym podejœciem, ktore mo¿na zastosowaæ, jest rysowanie znaku tylko w wybranych miejscach.
void rysujFigure(HANDLE whnd, FIGURA &figura) {
    int srodek;
    int wiersz;

    /*
        Sprawdzam parzystosc rozmiaru figury, któr¹ zamierzam narysowaæ.
        Ten test zdeterminuje jak¹ wartoœæ ma œrodkowy wiersz figury podczas jej rysowania.
        Metoda rysowania jak¹ przyj¹³em sk³ada siê z trzech kroków:
            1. narysuj czêœæ figury do jej œrodka
            2. narysuj œrodek
            3. narysuj od œrodka do koñca
        Kroki 1 i 3 sk³adaj¹ siê z pêtel, które: przechodz¹ do kolejnej linii na odpowiedni¹ pozycje dziêki
        funkcji zmiany pozycji kursora, rysuj¹ znaki podany przez u¿ytkownia oraz znaki spacji miêdzy nimi.
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
    Kolejne dwie funkcje s³u¿¹ do zmiany kolorów czcionki i t³a konsoli.
    Parametry przekazywane przez referencje maj¹ za zadanie zmieniæ wartoœæ na now¹,
    zaœ te przekazywane przez wartosci maj¹ pozostaæ niezmienne.
    Ma to na celu rozdzielenie t³a od czcionki, aby u¿ytkownik zmienia³ tylko jedn¹ wartoœæ na raz.
*/
int generatorKoloruCzcionki(int& obecnyKolorCzcionki, int obecnyKolorT³a) {
    obecnyKolorCzcionki = rand() % 15;
    return obecnyKolorT³a + obecnyKolorCzcionki;
}

/*
    Tablica kolory t³a przechowuje wartoœci ró¿nych mo¿liwych kolorów t³a.
    Proœciej by³o mi wypisaæ dok³adne wartoœci zamiast stworzyæ osobn¹ pêtle do ich wygenerowania z racji na naturê funkcji "SetConsoleTextAttribute".
    Jej atrybuty "wAttributes" s¹ stale zdefiniowane, jednak posiadaj¹ równie¿ swoj¹ reprezentacje w systemie szesnastowym.
    Podstawowymi wartoœciami kolorów t³a s¹ BLUE, GREEN, RED i INTENSITY, których odpowiedniki to 0x0010, 0x0020, 0x0040, 0x0080.
    Dodaj¹c ich wartoœci powstaje zbiór kombinacji umieszczony w tabeli koloryT³a, z której nastêpnie losowana jest jedna wartoœæ.
*/
int generatorKoloruT³a(int obecnyKolorCzcionki, int& obecnyKolorT³a) {
    int koloryTla[] = { 16, 32, 48, 64, 80, 96, 112, 128, 144, 160, 176, 192, 208, 224, 240};
    obecnyKolorT³a = koloryTla[rand() % 15];
    return obecnyKolorT³a + obecnyKolorCzcionki;
}
//Funkcja maj¹ca pomóc u¿ytkownikowi w rozpoznaniu wszystkich mo¿liwoœci programu.
//Zamkniêcie jej odbywa siê poprzez wciœniêcie jakiegokolwiek przycisku, a wywo³ywana jest przez wciœniêcie 'h'.
void help() {
    system("CLS");

    std::cout << "-----------------------------------------------POMOC-----------------------------------------------" << std::endl;
    std::cout << " Dzia³anie klawiszy:" << std::endl;
    std::cout << "\tStrza³ki - poruszaj¹ figur¹" << std::endl;
    std::cout << "\tPlus i minus - zwiêkszaja/zmniejsza rozmiar figury (maksymalnie do rozmiarów okna konsoli)" << std::endl;
    std::cout << "\tSpace - zmienia kolory czcionki konsoli na losowo wybrany przez program." << std::endl;
    std::cout << "\tTabulator - zmienia kolor t³a konsoli na losowo wybrany przez program." << std::endl;
    std::cout << "\t'h' - wyœwietla t¹ pomoc (ten komunikat)" << std::endl;
    std::cout << "\tBackspace - zakañcza dzia³anie programu\n" << std::endl;
    system("pause");
}

/*
    Funkcja odpowiadaj¹ca za stworzenie instancji struktury FIGURE, któr¹ bêdzie nasz iks,
    zmianê tytu³u programu na coœ odpowiedniego, stworzenie uchwytów do buforów konsoli, przyjêcie
    argumentów u¿ytkownika, i wreszcie narysowanie samej figury
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