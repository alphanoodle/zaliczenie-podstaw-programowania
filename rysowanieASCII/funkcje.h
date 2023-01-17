char wybierzPoczatek(char& znak, int& rozmiar);
void zmienPozycjeKursora(HANDLE hStdout, int pozycjaX, int pozycjaY, COORD tanslacja);
void rysujFigure(HANDLE hStdout, int rozmiaryFigury, char znak);
void przywitanie();
void help();
void menu(HANDLE whnd, HANDLE rhnd);
void changeCursorVisibility(HANDLE whnd, BOOL visibility);