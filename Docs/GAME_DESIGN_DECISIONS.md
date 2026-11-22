# GTA6 — potvrzené směřování projektu

Aktualizováno: 2026-11-22

## Produkt
- Originální open-world akční kriminální hra inspirovaná žánrem; nejde o oficiální produkt Rockstar Games ani kopii jejich chráněného obsahu.
- Platforma: Windows PC.
- Grafika: **fidelity-first**. Kvalita prostředí, materiálů, osvětlení a detailu má přednost před umělým omezováním velikosti instalace.
- Orientační velikost první instalované verze: přibližně 5 GB; nejde o pevný limit. Pokud obsah vyžaduje více místa, kvalita a hratelný obsah mají přednost.
- Herní svět: cíl 20 × 20 km, Praha co nejvěrněji zachycená.
- Budovy: přístupné budou především důležité a příběhové interiéry.
- Hlavní postavy: tři.
- Příběh: kriminální příběh gangů.
- Doprava: rozsáhlý systém různých kategorií vozidel a veřejné dopravy.
- Herní systémy: obyvatelé/NPC, doprava, policie, gangy, ekonomika, mise a příběh.
- Režim vydání: nejprve singleplayer; multiplayer až v pozdější fázi.
- Cílový výkon: 1080p / 120 FPS jako optimalizační cíl, nikoliv garantovaná specifikace.

## Technologie
- Engine: Unreal Engine 5.8 stable branch.
- Windows renderer: DirectX 12, Lumen GI/reflections, Virtual Shadow Maps, Nanite support a TSR.
- Lumen používá jako výchozí cestu software ray tracing; hardware ray tracing není vynucen globálně.
- Základ pro velký svět: World Partition a streaming buněk.
- Procedurální obsah: PCG pouze tam, kde pomůže s variací a rychlostí tvorby; důležité lokace budou ručně kontrolované.
- Geometrie: Nanite/LOD podle typu assetu a měřeného výkonu.
- Assety: kombinace vlastních originálních assetů a legálních bezplatných assetů s ověřenou licencí.
- Zdrojový projekt bude v GitHubu; velké binární assety a buildy se nemají bez rozmyslu ukládat jako běžné Git soubory. Později se rozhodne o Git LFS nebo odděleném úložišti.

## Grafická strategie
Podrobné zásady jsou v [HIGH_FIDELITY_GRAPHICS_PROFILE.md](HIGH_FIDELITY_GRAPHICS_PROFILE.md). Základní renderer je připravený, ale současná nativní UE čtvrť je stále tvořena jednoduchými primitivy. Finální kvalitu přinesou až originální modulární budovy, materiály, textury, vegetace, osvětlení, interiéry a ručně kontrolované dominanty. Běžné prostředí začne s 2K texturami; 4K bude vyhrazeno pro detailní dominanty a assety zblízka, pokud se vejdou do VRAM rozpočtu.

## Vývojová strategie
Vývoj bude probíhat po ověřitelných milnících. První cíl není okamžitě naplnit celých 400 km², ale vytvořit reprezentativní, hratelný a měřitelný městský výřez:
1. Stabilní UE projekt a opakovatelný build.
2. Vertikální řez: jedna čtvrť, hráč, základní pohyb, silnice, pěší zóny, několik interiérů.
3. Vizuální vertical slice: modulární architektura, PBR materiály, osvětlení a vegetace.
4. Vozidla a dopravní AI.
5. Obyvatelé a základní simulace města.
6. Mise, ekonomika a příběhové systémy.
7. Rozšiřování Prahy po čtvrtích až k cílovému rozsahu.
8. Optimalizace, QA a pozdější posouzení multiplayeru.

## Výkon a realita
1080p/120 FPS je ambiciózní cíl pro realistický open-world titul. Bude se měřit na skutečném Windows buildu a na cílovém PC: Ryzen 5 5600, RTX 5050 8 GB a 16 GB RAM. Kvalita obrazu, hustota NPC, vzdálenost vykreslování a simulace budou mít nastavitelné profily. 16 GB systémové RAM je omezením pro vývoj v editoru; doporučení Epicu pro UE vývoj je 32 GB RAM. Žádná hodnota FPS nebude označena jako dosažená bez měření.

## Distribuce
Cílem je offline instalace bez povinného internetu. Aktuální jediný EXE má limit necelé 4 GiB; pokud ho komprimovaný balík překročí, přejde se na vícesouborové offline médium. Velikost herního obsahu se nebude tvrdě omezovat na 5 GB.

## Co je potřeba od uživatele
V tuto chvíli nic dalšího k základnímu směřování. Při přechodu na lokální kompilaci a testování bude potřeba potvrdit instalovanou verzi Unreal Engine, Visual Studio a dostupné místo na disku.
