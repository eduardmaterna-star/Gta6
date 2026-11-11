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

## Distribuce
Vybraná varianta je **jeden kompletní offline instalátor pro Windows x64**. Výsledný EXE bude obsahovat instalační aplikaci i celý zabalený herní payload. Instalace nebude potřebovat internet ani samostatný ZIP.

- Zdroj instalátoru: Installer/GTA6.iss (Inno Setup)
- Build skript: Scripts/Build-OfflineInstaller.ps1
- Architektura a stav: Docs/OFFLINE_INSTALLER_SINGLE_FILE.md

Velikost 50 MB není realistická pro kompletní hru. Finální instalátor obsahuje komprimované herní soubory. Jeden Windows EXE má praktický limit necelé 4 GiB; pokud komprimovaný herní balík tento limit překročí, bude nutné zmenšit balík nebo změnit distribuční formát. Přesnou velikost zjistíme až po vytvoření skutečného Unreal Windows buildu.

## Aktuální stav hry
**Milestone 002 — první runtime greybox čtvrť.** Obsahuje C++ third-person prototyp a procedurálně poskládanou blokovou čtvrť. Nejde zatím o detailní Prahu ani o finální mapu.

**Milestone 004 — projektová základna.** Připraveny jsou projektové konfigurační soubory, statické CI kontroly a dokumentace pro lokální Windows build. Skutečný World Partition .umap musí být vytvořen v Unreal Editoru.

## Požadavky pro lokální build hry
- Unreal Engine 5.8.x.
- Visual Studio 2022 s C++ workloady a Windows SDK.

Podrobnosti: Docs/WINDOWS_BUILD_AND_TEST.md.

## Ověření
GitHub Actions ověřuje kompilaci Inno Setup instalačního skriptu s testovacím souborem. To nenahrazuje kompilaci Unreal Engine ani test instalace skutečné hry.

V tomto prostředí nebyl dostupný Unreal Editor, MSVC ani Windows .NET SDK. Kompilace, spuštění, herní testy, instalace a velikost výsledného instalátoru proto nejsou potvrzené.
