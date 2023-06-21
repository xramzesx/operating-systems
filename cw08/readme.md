# `lab8:` Wątki

W archiwum załączonym do tego zadania znajdziesz implementację [gry w życie Conway-a](https://en.wikipedia.org/wiki/Conway%27s_Game_of_Life). Na podstawie tego kodu wykonaj poniższe zadania:

### Zadanie 1
`(70%)` Napisz współbieżną wersję gry w życie. Utwórz jeden wątek dla każdej komórki planszy. Każdy z utworzonych wątków powinien obliczać stan swojej przypisanej komórki w następnym kroku czasowym. Wątek główny powinien zajmować się tylko zlecaniem prac i wyświetlaniem wyników. Wątki powinny być tworzone tylko raz na początku programu i powinny wykonywać swoją pracę raz na iterację. (sugestia rozwiązania: pause() i pthread_kill())
### Zadanie 2
`(30%)` Przygotuj zmodyfikowaną wersję programu z punktu 1., gdzie ilość wątków jest ograniczona i podana z góry jako parametr wywołania programu. Rozdziel prace pomiędzy wątkami maksymalnie równomiernie.

### Dodatkowe informacje
Do skompilowania gry potrzebna jest biblioteka  [ncurses](https://invisible-island.net/ncurses/ncurses-intro.html). W niektórych dystrybucjach Linux-a trzeba zainstalować nagłówki tej biblioteki. Np.: apt install ncurses-devel.

Gra wykorzystuje dwie tablice dla obecnego i następnego kroku czasowego. Każdy wątek zapisuje dane tylko do jednej komórki tablicy następnego kroku, a zatem dostęp do tych tablic nie musi być synchronizowany.

Pomiędzy wyświetleniami planszy program czeka 0,5 sekundy na wykonanie obliczeń. W tym zadaniu ignorujemy możliwość nieskończenia obliczeń w wyznaczonym czasie, tj. żadna dodatkowa synchronizacja wątków nie jest wymagana.