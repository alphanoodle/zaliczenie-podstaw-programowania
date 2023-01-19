//z tego co przeczyta³em w internecie dobrym nawykiem jest dawanie sta³ym nazw, które maj¹ znaczenie w póŸniejszej analizie kodu
//dlatego minimalny rozmiar figury 
#define MINIMALNY_ROZMIAR_FIGURY 3

//deklaracje u¿ywanych funkcji
void wybierzPoczatek(char& znak, int& rozmiar);
void zmienPozycjeKursora(HANDLE hStdout, int pozycjaX, int pozycjaY, COORD& tanslacja);
void help();
void menu();
void changeCursorVisibility(HANDLE whnd, BOOL visibility);
int generatorKoloruCzcionki(int& obecnyKolorCzcionki, int obecnyKolorT³a);
int generatorKoloruT³a(int obecnyKolorCzcionki, int& obecnyKolorT³a);


/*
    Struktura odpowiadaj¹ca za podstawowe w³aœciwoœci rysowanej figury.
    Oprócz rozmiaru i znaki iksa odpowiada równie¿ za przyjêcie pocz¹tkowych wartoœci oraz zmiany jej rozmiaru w przyjêtych granicach.
*/
struct FIGURA {
    int rozmiar = 5;
    unsigned char znak = 'x';
    COORD translacja = { 0,0 };

    void wybierzPoczatek(HANDLE whnd) {
        CONSOLE_SCREEN_BUFFER_INFO infoOBufferze;
        GetConsoleScreenBufferInfo(whnd, &infoOBufferze);
        SHORT maksymalnyRozmiarFigury = infoOBufferze.srWindow.Bottom;      //wybieram maksymalny rozmiar przez zdobycie ostatniej wartoœci osi rzêdnych æwiartki p³aszczyzny kartezjañskiej na której pojawi siê figura

        std::cout << "WITAM!\nOto program stworzony w celu wyprodukowania figury, której rozmiary, znak ASCII jaki zostanie u¿yty do jej stworzenia\noraz kolor wybierasz w³aœnie ty!.\n" << std::endl;


        std::cout << "Wpisz znak ASCII który chcesz u¿yæ: " <<  std::endl;
        std::cin >> znak;
        std::cin.ignore(INT_MAX, '\n');     //ignoruje wszystkie znaki oprócz pierwszego, który wybierzemy do rysowania

        while (true){       //pêtla opiekuj¹ca siê rozmiarami pierwszego rysowania figury
            std::cout << "Podaj rozmiar figury (pomiêdzy 3 a " << infoOBufferze.srWindow.Bottom << "): " << std::endl;
            std::cin >> rozmiar;
            if (!std::cin || rozmiar < MINIMALNY_ROZMIAR_FIGURY || rozmiar > maksymalnyRozmiarFigury) {
                std::cin.clear();       //resetuje flagê b³êdu cinu (dziêki czemu przysz³y input bêdzie poprawnie obs³u¿ony)
                std::cin.ignore(INT_MAX, '\n');     //ignoruje nieprawid³owy input bêd¹cy w buforze
                continue;
            }
            else break;
        }
        system("CLS");
    }
    //Funkcja zmiany rozmiaru zaoparzona w blokadê przed wychodzenie figury poza ramy okna.
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

//dalsza czêœæ deklaracji po stworzeniu struktury FIGURA
void rysujFigure(HANDLE whnd, FIGURA& figura);
void obsluzInput(HANDLE whnd, HANDLE rhnd, FIGURA& figura);
bool obsluzEvent(HANDLE whnd, INPUT_RECORD eventBuffer[], FIGURA& figura, DWORD liczbaCzytanychEventow);