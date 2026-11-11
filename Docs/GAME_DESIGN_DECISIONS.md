# GTA6 — potvrzené směřování projektu

Aktualizováno: 2026-11-11

## Produkt
- Originální open-world akční kriminální hra inspirovaná žánrem; nejde o oficiální produkt Rockstar Games ani kopii jejich chráněného obsahu.
- Platforma: Windows PC.
- Grafika: realistická, ale optimalizovaná.
- Herní svět: cíl 20 × 20 km, Praha co nejvěrněji zachycená.
- Budovy: přístupné budou především důležité a příběhové interiéry.
- Hlavní postavy: tři.
- Příběh: kriminální příběh gangů.
- Doprava: rozsáhlý systém různých kategorií vozidel a veřejné dopravy.
- Herní systémy: obyvatelé/NPC, doprava, policie, gangy, ekonomika, mise a příběh.
- Režim vydání: nejprve singleplayer; multiplayer až v pozdější fázi.
- Cílový výkon: 1080p / 120 FPS jako optimalizační cíl, nikoliv garantovaná specifikace.

## Technologie
- Engine: Unreal Engine 5.8 stable branch. UE 5.9 je k datu dokumentu Preview, proto se na něm nezačne produkční vývoj.
- Základ pro velký svět: World Partition a streaming buněk.
- Procedurální obsah: PCG pouze tam, kde pomůže s variací a rychlostí tvorby; důležité lokace budou ručně kontrolované.
- Geometrie: Nanite/LOD podle typu assetu a měřeného výkonu.
- Assety: kombinace vlastních originálních assetů a legálních bezplatných assetů s ověřenou licencí.
- Zdrojový projekt bude v GitHubu; velké binární assety a buildy se nemají bez rozmyslu ukládat jako běžné Git soubory. Později se rozhodne o Git LFS nebo odděleném úložišti.

## Vývojová strategie
Vývoj bude probíhat po ověřitelných milnících. První cíl není okamžitě naplnit celých 400 km², ale vytvořit reprezentativní, hratelný a měřitelný městský výřez:
1. Stabilní UE projekt a opakovatelný build.
2. Vertikální řez: jedna čtvrť, hráč, základní pohyb, silnice, pěší zóny, několik interiérů.
3. Vozidla a dopravní AI.
4. Obyvatelé a základní simulace města.
5. Mise, ekonomika a příběhové systémy.
6. Rozšiřování Prahy po čtvrtích až k cílovému rozsahu.
7. Optimalizace, QA a pozdější posouzení multiplayeru.

## Výkon a realita
1080p/120 FPS je ambiciózní cíl pro realistický open-world titul. Bude se měřit na skutečném Windows buildu a na cílovém PC. Kvalita obrazu, hustota NPC, vzdálenost vykreslování a simulace budou mít nastavitelné profily. Žádná hodnota FPS nebude označena jako dosažená bez měření.

## Co je potřeba od uživatele
V tuto chvíli nic dalšího k základnímu směřování. Při přechodu na lokální kompilaci a testování bude potřeba potvrdit instalovanou verzi Unreal Engine, Visual Studio a dostupné místo na disku.
