# GTA6 — originální open-world projekt

Pracovní název repozitáře je `Gta6`. Projekt je samostatná originální hra v Unreal Engine 5 zasazená do fiktivní herní verze Prahy; nejde o oficiální produkt Rockstar Games ani o kopii jejich mapy či příběhu.

## Aktuální milestone

**Milestone 002 — první hratelný městský blockout**

- C++ third-person pohyb a kamera
- automaticky generovaná městská čtvrť o rozloze přibližně 1,1 × 1,1 km
- silniční síť, chodníky, různě vysoké budovy a centrální park
- automatický spawn hráče

Podrobnosti: `Docs/MILESTONE_002_CITY_MAP.md`.

## Požadavky

- Unreal Engine 5 (kompatibilní verze)
- Visual Studio 2022 s workloady **Desktop development with C++** a **Game development with C++**

Otevři `OPEN_WORLD_Next_Gen.uproject`, nech Unreal vygenerovat IDE soubory a projekt zkompiluj.

## Ovládání

- `W A S D` — pohyb
- myš — kamera
- `Space` — skok
- `E` — interakce s podporovaným objektem

## Stav ověření

Jde o zdrojový prototyp. V tomto prostředí nebyl dostupný Unreal Engine ani MSVC, takže kompilace, spuštění v editoru a FPS nejsou potvrzené. Mapa používá základní engine cube mesh a je pouze greybox — neobsahuje hotové budovy, interiéry, dopravu ani NPC.
