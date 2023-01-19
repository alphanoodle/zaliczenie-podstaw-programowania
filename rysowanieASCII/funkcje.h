//z tego co przeczyta�em w internecie dobrym nawykiem jest dawanie sta�ym nazw, kt�re maj� znaczenie w p�niejszej analizie kodu
//dlatego minimalny rozmiar figury 
#define MINIMALNY_ROZMIAR_FIGURY 3

//deklaracje u�ywanych funkcji
void wybierzPoczatek(char& znak, int& rozmiar);
void zmienPozycjeKursora(HANDLE hStdout, int pozycjaX, int pozycjaY, COORD& tanslacja);
void help();
void menu();
void changeCursorVisibility(HANDLE whnd, BOOL visibility);
int generatorKoloruCzcionki(int& obecnyKolorCzcionki, int obecnyKolorT�a);
int generatorKoloruT�a(int obecnyKolorCzcionki, int& obecnyKolorT�a);


/*
    Struktura odpowiadaj�ca za podstawowe w�a�ciwo�ci rysowanej figury.
    Opr�cz rozmiaru i znaki iksa odpowiada r�wnie� za przyj�cie pocz�tkowych warto�ci oraz zmiany jej rozmiaru w przyj�tych granicach.
*/
struct FIGURA {
    int rozmiar = 5;
    unsigned char znak = 'x';
    COORD translacja = { 0,0 };

    void wybierzPoczatek(HANDLE whnd) {
        CONSOLE_SCREEN_BUFFER_INFO infoOBufferze;
        GetConsoleScreenBufferInfo(whnd, &infoOBufferze);
        SHORT maksymalnyRozmiarFigury = infoOBufferze.srWindow.Bottom;      //wybieram maksymalny rozmiar przez zdobycie ostatniej warto�ci osi rz�dnych �wiartki p�aszczyzny kartezja�skiej na kt�rej pojawi si� figura

        std::cout << "WITAM!\nOto program stworzony w celu wyprodukowania figury, kt�rej rozmiary, znak ASCII jaki zostanie u�yty do jej stworzenia\noraz kolor wybierasz w�a�nie ty!.\n" << std::endl;


        std::cout << "Wpisz znak ASCII kt�ry chcesz u�y�: " <<  std::endl;
        std::cin >> znak;
        std::cin.ignore(INT_MAX, '\n');     //ignoruje wszystkie znaki opr�cz pierwszego, kt�ry wybierzemy do rysowania

        while (true){       //p�tla opiekuj�ca si� rozmiarami pierwszego rysowania figury
            std::cout << "Podaj rozmiar figury (pomi�dzy 3 a " << infoOBufferze.srWindow.Bottom << "): " << std::endl;
            std::cin >> rozmiar;
            if (!std::cin || rozmiar < MINIMALNY_ROZMIAR_FIGURY || rozmiar > maksymalnyRozmiarFigury) {
                std::cin.clear();       //resetuje flag� b��du cinu (dzi�ki czemu przysz�y input b�dzie poprawnie obs�u�ony)
                std::cin.ignore(INT_MAX, '\n');     //ignoruje nieprawid�owy input b�d�cy w buforze
                continue;
            }
            else break;
        }
        system("CLS");
    }
    //Funkcja zmiany rozmiaru zaoparzona w blokad� przed wychodzenie figury poza ramy okna.
    void zmienRozmiar(HANDLE whnd, int wielkoscZmiany) {
        CONSOLE_SCREEN_BUFFER_INFO infoOBufferze;
        GetConsoleScreenBufferInfo(whnd, &infoOBufferze);

        if (wielkoscZmiany > 0 || rozmiar != 3) {
            rozmiar += wielkoscZmiany;
            if (rozmiar >= infoOBufferze.srWindow.Bottom) {
                rozmiar = infoOBufferze.srWindow.Bottom;
            }
        }
    }
};

//dalsza cz�� deklaracji po stworzeniu struktury FIGURA
void rysujFigure(HANDLE whnd, FIGURA& figura);
void obsluzInput(HANDLE whnd, HANDLE rhnd, FIGURA& figura);
bool obsluzEvent(HANDLE whnd, INPUT_RECORD eventBuffer[], FIGURA& figura, DWORD liczbaCzytanychEventow);