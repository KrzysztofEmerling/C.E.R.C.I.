# CERCI - Chess Engine with Reinforcement-based Cognitive Identities

CERCI (Chess Engine with Reinforcement-based Cognitive Identities) to zaawansowany silnik szachowy łączący backend napisany w **C++** z siecią neuronową **DNN** uzyskaną z treningu nienadzorowanego z wykorzystaniem **MCTS**.  
Celem projektu jest stworzenie silnika, który rozwija **unikalne style gry**, mimo zastosowania uczenia nienadzorowanego, wpływając na proces treningu poprzez wstępny trening na syntetycznych danych dla stylów: **Pozycyjny** i **Agresywny**. Zastosowane podejście nie wymagało uprzednio zaetykietowanych danych na konkretne style.

---

## Spis treści
- [Wymagania](#wymagania)
- [Instalacja](#instalacja)
  - [Szybki start C++ / CMake](#szybki-start-c--cmake)
  - [Szybki start Python / venv](#szybki-start-python-venv)
- [Framework testowy](#framework-testowy)

---

## Wymagania

- **C++23** lub nowszy  
- **CMake** >= 3.16  
- **Python** >= 3.11 
- Biblioteki Python: zawarte w `requirements.txt` (Keras, TensorFlow, NumPy, itp.)

---

## Instalacja

### Szybki start C++ / CMake

```bash
# Sklonuj repozytorium
git clone https://github.com/KrzysztofEmerling/C.E.R.C.I..git
cd C.E.R.C.I.

bash ./cmakeSetup.sh
cmake --build build

./build/CERCI-ChessEngine
```
### Szybki start Python / venv
```bash
# Stwórz środowisko wirtualne
python -m venv chesEnv
source chesEnv/bin/activate  # Linux / macOS
chesEnv\Scripts\activate     # Windows

# Zainstaluj wymagane biblioteki
pip install --upgrade pip
pip install -r requirements.txt
```

## Framework testowy

CERCI zawiera również **framework testowy** napisany w Pythonie i dostępny w formie **notatników Jupyter**.  
Framework umożliwia:

- Testów Perft umożliwiających debugowanie generatora ruchów 
- Testowanie silnika w różnych konfiguracjach i stylach gry pod kątem wydajności
- siły gry końcówkowek
- Testów regresji w porównaniu do pierwszej wersji

