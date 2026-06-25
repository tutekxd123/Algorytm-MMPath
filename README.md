# Algorytm MMPath
Repozytorium ukazuje implmenetacje algorytmu **MMPATH** w ramach pracy inżynierskiej na UWM w języku C++

# Jaki problem rozwiązuje algorytm MMPath?
Algorytm MMPath służy do wyznaczenia optymalnej drogi między płaszczyznami. Takie płaszczyzny można często w grach komputerowych, gdzie każdy obszar/poziom może być osobną "płaszczyzną"
Algorytm uwzględnia możliwość wystąpienia kolizji oraz znalezienie skrótów z wagą 0, dzięki czemu można zaplanować naprzykład portale.

# Generator subpłaszczyzn
Projekt wykorzystuje zmmodyfikowany generator subpłaszczyzn, dostępny w osobnym repozytorium: [LINK](https://github.com/tutekxd123/Generator-Subplazczyzn)
Generator umożliwa tworzenie grafów pseudolosowych, determistycznych oraz zepsutych w których docelowa płaszczyzna nie jest osiągalna.


# Zaimplementowane funkcje
- Unit testy
- Różne warianty algorytmu MMPath z cache oraz heurystyką w celu wykonania benchmarków
- Generator subpłaszczyzn determistycznych oraz pseudolosowych



