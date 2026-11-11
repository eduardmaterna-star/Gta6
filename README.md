# GTA6 — originální open-world projekt

Samostatná originální open-world akční hra inspirovaná kriminálním žánrem, zasazená do herní interpretace Prahy. Nejde o oficiální produkt Rockstar Games ani o kopii jejich chráněné mapy, postav nebo příběhu.

## Potvrzený směr
- Windows PC, Unreal Engine 5.8.
- Realistická, ale optimalizovaná grafika.
- Dlouhodobý cíl světa: 20 × 20 km.
- Tři hlavní postavy a kriminální příběh gangů.
- Důležité budovy budou mít interiéry.
- Rozsáhlá silniční a veřejná doprava.
- Nejprve singleplayer; multiplayer se posoudí v pozdější fázi.
- Výkonový cíl 1080p / 120 FPS je požadavek k ověření, ne naměřený výsledek.

Podrobnosti: `Docs/GAME_DESIGN_DECISIONS.md`.

## Aktuální stav zdrojů
**Milestone 002 — první runtime greybox čtvrť.** Obsahuje C++ third-person prototyp a procedurálně poskládanou blokovou čtvrť. Nejde zatím o detailní Prahu ani o finální mapu.

**Milestone 004 — projektová základna a vertikální řez.** Připraveny jsou projektové konfigurační soubory, kontrola konzistence v CI a dokumentace pro lokální Windows build. Samotná kompilace UE a tvorba editorového `.umap` vyžadují Unreal Editor na Windows.

## Požadavky pro lokální build
- Unreal Engine **5.8.x** (doporučeno nainstalovat nejnovější dostupný 5.8 hotfix).
- Visual Studio 2022 nebo kompatibilní novější toolchain s C++ desktop/game development komponentami a Windows SDK.
- Python 3 pro lokální statickou kontrolu (není nutný pro samotnou kompilaci).

Otevři `OPEN_WORLD_Next_Gen.uproject` v Unreal Editoru, nech vygenerovat IDE soubory a sestav konfiguraci Development Editor. Přesný postup je v `Docs/WINDOWS_BUILD_AND_TEST.md`.

## Ovládání prototypu
- `W A S D` — pohyb
- myš — kamera
- `Space` — skok
- `E` — interakce s podporovaným objektem

## Ověření
GitHub Actions kontroluje JSON projektu, přítomnost základních C++ modulů a konzistenci konfiguračních cest. Tato kontrola **nenahrazuje** kompilaci Unreal Engine.

V tomto prostředí nebyl dostupný Unreal Editor ani MSVC. Kompilace, spuštění, herní testy a FPS proto nejsou potvrzené. Současná čtvrť používá engine cube mesh a zůstává greyboxem. Neobsahuje hotové assety, dopravní simulaci, chodce ani finální interiéry.
