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

## Distribuce pro Windows
Projekt obsahuje prototyp kombinovaného instalátoru:
- malá aplikace Windows pro online i offline instalaci,
- online zdroj přes HTTPS manifest a ZIP balík,
- offline zdroj z lokální složky se stejným manifestem a ZIP balíkem,
- kontrola SHA-256, bezpečné rozbalení a instalace přes staging adresář.

Dokumentace: `Docs/DISTRIBUTION_ARCHITECTURE.md` a `Launcher/README.md`.

Cíl přibližně 50 MB se týká pouze instalátoru, nikoli celé hry. Aktuální prototyp je framework-dependent .NET 8 WinForms aplikace a na cílovém počítači proto vyžaduje .NET 8 Desktop Runtime. Skutečnou velikost EXE zatím nelze uvést, protože nebyl proveden Windows publish. Online server, herní balík a podepsané vydání zatím nejsou připravené.

## Aktuální stav hry
**Milestone 002 — první runtime greybox čtvrť.** Obsahuje C++ third-person prototyp a procedurálně poskládanou blokovou čtvrť. Nejde zatím o detailní Prahu ani o finální mapu.

**Milestone 004 — projektová základna.** Připraveny jsou projektové konfigurační soubory, statické CI kontroly a dokumentace pro lokální Windows build. Skutečný World Partition `.umap` musí být vytvořen v Unreal Editoru.

## Požadavky pro lokální build hry
- Unreal Engine 5.8.x.
- Visual Studio 2022 s C++ workloady a Windows SDK.

Podrobnosti: `Docs/WINDOWS_BUILD_AND_TEST.md`.

## Ověření
GitHub Actions provádí statické kontroly metadat projektu a zdrojů instalátoru. Tyto kontroly nenahrazují kompilaci Unreal Engine ani .NET aplikace.

V tomto prostředí nebyl dostupný Unreal Editor, MSVC ani Windows .NET SDK. Kompilace, spuštění, herní testy, instalace a velikost výsledného instalátoru proto nejsou potvrzené.
