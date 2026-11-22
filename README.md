# NEXXT GEN — originální open-world Crime Simulator

NEXXT GEN je ambiciózní originální open-world hra zasazená do fiktivního města inspirovaného Prahou. GTA 5 a GTA 6 jsou pouze žánrovou inspirací; město, postavy, příběh, hudba, značky a herní obsah budou vlastní.

## Hlavní vize

- Fotorealistická grafika s maximální prioritou na prostředí, osvětlení, animace a příběh.
- Cílový svět 20 × 20 km: jedno velké město, předměstí, venkov a průmyslové oblasti.
- Tři hlavní postavy a příběh s vězeňskou částí, útěkem a následnými důsledky.
- Cíl přibližně 40 hodin příběhu, 100 hlavních a 200 vedlejších misí.
- Dlouhodobé cíle: až 500 NPC v městské simulaci a přibližně 200 vozidel.
- Dynamické počasí, denní/noční cyklus, roční období, interiéry a změny světa.
- Nejdříve singleplayer; kooperace a dedikované servery až v pozdější etapě.
- Bez sociální sítě a příspěvků NPC.

## Hlavní zadání

**[Docs/NEXXT_GEN_GAME_DESIGN.md](Docs/NEXXT_GEN_GAME_DESIGN.md)** je hlavní produktové a herní zadání. Obsahuje všechny části dotazníku, technické cíle, pracovní rozhodnutí, vývojové etapy a otevřené body.

## Technologie a grafika

- Aktuální zdrojový projekt: Unreal Engine 5.8.
- Cílové technologie: Lumen, Nanite, Virtual Shadow Maps, TSR a volitelný ray tracing.
- DLSS, FSR a XeSS budou volitelné podle kompatibility GPU, platformy a ověřené verze pluginu.
- Grafický vertical slice bude nejdříve vytvořen a změřen v jedné kvalitně zpracované čtvrti.

Grafické požadavky a akceptační testy jsou popsány v [Docs/QA_ACCEPTANCE_MATRIX.md](Docs/QA_ACCEPTANCE_MATRIX.md). Samotný záměr ještě neznamená, že renderer nebo výsledná kvalita prošly testem.

## Platformy a distribuce

- První vydávací cíl: Windows 10/11 x64.
- Pozdější cíle: samostatné Linux a macOS buildy s vlastní testovací maticí.
- Hráč nebude muset instalovat Unreal Engine ani Unreal Editor.
- Windows verze má mít offline instalátor bez nutnosti stahovat hru během instalace.
- Orientační velikost kolem 5 GB není pevný limit. Kvalita obsahu má přednost; skutečná velikost se změří po zabalení hry.

## Aktuální technický stav

Repozitář obsahuje počáteční UE 5.8 C++ základ, runtime greybox čtvrť, první prototyp civilní AI, prototyp vozidla a základ ukládání/načítání. Obsahuje také samostatné browser demo. Tyto části nejsou finální hrou.

Fotorealistické assety, kompletní mapa 20 × 20 km, celý příběh, kompletní katalog vozidel, plná simulace 500 NPC, multiplatformní buildy a multiplayer zatím nejsou dokončené. UE C++ build, editorový runtime a výkon zabalené hry musí být ověřeny na Windows build stroji.

## Vývoj a QA

- [Docs/GAME_DESIGN_DECISIONS.md](Docs/GAME_DESIGN_DECISIONS.md) — stručný přehled potvrzeného směru.
- [Docs/TECHNICAL_ARCHITECTURE.md](Docs/TECHNICAL_ARCHITECTURE.md) — hranice systémů a technická architektura.
- [Docs/QA_ACCEPTANCE_MATRIX.md](Docs/QA_ACCEPTANCE_MATRIX.md) — povinné testy a důkazy.
- [Docs/WINDOWS_BUILD_AND_TEST.md](Docs/WINDOWS_BUILD_AND_TEST.md) — lokální sestavení a testování.

Žádná funkce, FPS hodnota ani cílová platforma nebude označena za hotovou bez odpovídajícího testu.
