# GTA6 — originální open-world projekt

Samostatná originální open-world akční hra inspirovaná kriminálním žánrem, zasazená do herní interpretace Prahy. Nejde o oficiální produkt Rockstar Games ani o kopii jejich chráněné mapy, postav nebo příběhu.

## Potvrzený směr
- Windows PC, Unreal Engine 5.8.
- Grafika má prioritu: Lumen, Nanite-ready assety, Virtual Shadow Maps a TSR.
- První vizuální cíl je detailní 1080p desktop zážitek; 1080p / 120 FPS zůstává cílem k ověření měřením.
- Dlouhodobý cíl světa: 20 × 20 km.
- Tři hlavní postavy a kriminální příběh gangů.
- Důležité budovy budou mít interiéry.
- Rozsáhlá silniční a veřejná doprava.
- Nejprve singleplayer; multiplayer se posoudí v pozdější fázi.
- Instalovaná velikost kolem 5 GB je přijatelná jako první orientační cíl, nikoli pevný limit. Kvalita a obsah mají přednost před umělým omezením velikosti.

## Grafický profil
Základní nastavení rendereru a pravidla pro assety jsou v [Docs/HIGH_FIDELITY_GRAPHICS_PROFILE.md](Docs/HIGH_FIDELITY_GRAPHICS_PROFILE.md). Konfigurace připravuje DX12, Lumen GI/reflections, Virtual Shadow Maps, Nanite support a TSR. Skutečná kvalita ale závisí i na detailních modelech, materiálech, texturách a úrovních vytvořených v Unreal Editoru.

## Distribuce
Vybraná varianta je **offline instalace pro Windows x64 bez povinného internetového připojení**.

- Zdroj instalátoru: Installer/GTA6.iss (Inno Setup)
- Build skript: Scripts/Build-OfflineInstaller.ps1
- Aktuální single-EXE architektura: Docs/OFFLINE_INSTALLER_SINGLE_FILE.md

Jednotný offline EXE má praktický limit necelé 4 GiB. Pokud komprimovaný herní balík tento limit překročí, upraví se distribuce na vícesouborové offline médium; grafické assety se nebudou odstraňovat jen kvůli zachování jediného EXE. Přesnou velikost určí až skutečný zabalený Unreal Windows build.

## Aktuální stav hry
**Milestone 002 — první runtime greybox čtvrť.** Obsahuje C++ third-person prototyp a procedurálně poskládanou blokovou čtvrť. Nejde zatím o detailní Prahu ani o finální mapu.

**Milestone 004 — projektová základna.** Připraveny jsou projektové konfigurační soubory, statické CI kontroly a dokumentace pro lokální Windows build. Runtime greybox má nastavitelné `GenerationSeed`, který reprodukovatelně mění počty a rozměry budov, a omezené parametry `DistrictCellCount` / `DistrictCellSize` pro bezpečné testování různě velkých prototypových čtvrtí. Přidaný je první C++ základ civilního NPC s AIControllerem a časovačem řízeným pohybem po dosažitelných bodech NavMeshe; pro jeho testování je stále nutné vytvořit NavMeshBoundsVolume v editoru. Skutečný World Partition .umap musí být vytvořen v Unreal Editoru.

## Požadavky pro lokální build hry
- Unreal Engine 5.8.x.
- Visual Studio 2022 s C++ workloady a Windows SDK.

Podrobnosti: Docs/WINDOWS_BUILD_AND_TEST.md.

## Architektura a QA
- `Docs/TECHNICAL_ARCHITECTURE.md` — hranice herních systémů, datové toky a implementační pořadí.
- `Docs/QA_ACCEPTANCE_MATRIX.md` — požadované testy a důkazy pro dokončení funkcí.

## Ověření
GitHub Actions ověřuje statické kontroly repozitáře a kompilaci instalačního skriptu s testovacím souborem. To nenahrazuje kompilaci Unreal Engine ani test instalace skutečné hry.

V tomto prostředí nebyl dostupný Unreal Editor ani MSVC. Kompilace, spuštění, herní testy, měření výkonu, instalace a velikost výsledného instalátoru proto nejsou potvrzené.

## Save/load development
See [save/load setup and verification requirements](Docs/SAVE_LOAD_SETUP.md) for the initial versioned persistence slice and its remaining UE 5.8 runtime tests.

## Vehicle prototype
See [vehicle prototype setup and limitations](Docs/VEHICLE_PROTOTYPE_SETUP.md) for the first physics-driven vehicle source slice and its pending UE 5.8 tests.
