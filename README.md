# System alarmowy na Arduino

## Opis projektu
System alarmowy oparty na platformie Arduino. Umożliwia monitorowanie czujnika PIR (ruch) oraz czujnika temperatury TMP36 (próg 30°C). Obsługa przez menu na wyświetlaczu LCD 16x2 oraz przyciski UP/DOWN/OK.

## Funkcje
- ✅ Menu na LCD HD44780
- ✅ Uzbrajanie z odliczaniem 5 sekund
- ✅ Wykrywanie ruchu (PIR)
- ✅ Wykrywanie temperatury powyżej 30°C
- ✅ Sygnalizacja alarmu (buzzer + migająca dioda LED)
- ✅ Wyświetlacz statusu (CD4511) – stany 0-4

## Wykorzystane elementy
| Element | Ilość |
|---------|------|
| Arduino UNO | 1 |
| LCD 16x2 HD44780 | 1 |
| Czujnik PIR | 1 |
| Czujnik TMP36 | 1 |
| Buzzer | 1 |
| Dioda LED | 1 |
| CD4511 | 1 |
| Przyciski UP/DOWN/OK | 3 |

## Schemat połączeń
| Element | Pin Arduino |
|---------|-------------|
| LCD RS | 7 |
| LCD E | 6 |
| LCD D4 | 8 |
| LCD D5 | 9 |
| LCD D6 | 10 |
| LCD D7 | 11 |
| Przycisk UP | A1 |
| Przycisk DOWN | A0 |
| Przycisk OK | A2 |
| PIR | 13 |
| TMP36 | A3 |
| Buzzer | 5 |
| Dioda LED | 12 |
| CD4511 A | 2 |
| CD4511 B | 3 |
| CD4511 C | A5 |
| CD4511 D | A4 |

## Stany systemu
| Stan | CD4511 | Opis |
|------|--------|------|
| Rozbrojony | 0 | Brak monitorowania |
| Uzbrojony | 1 | Monitoring aktywny |
| Odliczanie | 2 | 5 sekund do uzbrojenia |
| Alarm | 3 | Naruszono czujnik |
| Test | 4 | Czujnik naruszony (rozbrojony) |

<img width="784" height="560" alt="image" src="https://github.com/user-attachments/assets/58812379-da4d-41f4-b166-144d4fbcb531" />

## Autor
Małgorzata Librant
