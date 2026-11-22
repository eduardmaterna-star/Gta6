# NEXXT GEN — produktové a herní zadání

**Verze zadání:** 1.0  
**Datum:** 22. 11. 2026  
**Stav:** schválený směr projektu; jednotlivé funkce se teprve implementují a ověřují.

> Tento dokument je hlavní zdroj pravdy pro produktové požadavky hry NEXXT GEN. Požadavek v tomto dokumentu neznamená, že je funkce již naprogramovaná nebo otestovaná.

## 1. Identita a vize

- **Název:** NEXXT GEN.
- **Žánr:** Crime Simulator — příběhová open-world hra pro jednoho hráče.
- **Vizuální cíl:** fotorealistický, filmově působící svět s důrazem na prostředí, animace, osvětlení a detail.
- **Inspirace:** GTA 5 a GTA 6 jako žánrová a kvalitativní inspirace; NEXXT GEN musí mít vlastní město, postavy, příběh, značky, vozidla, hudbu a herní identitu.
- **Produktový cíl:** ambiciózní AAA-style zážitek vytvořený postupně malým týmem jednoho vývojáře s podporou AI. Označení „AAA“ je aspirace na kvalitu a rozsah, nikoli tvrzení o současném stavu nebo rozpočtu.
- **Hlavní priority:** grafika, příběh, svět, animace a uvěřitelnost prostředí.
- **Co ve hře nebude:** sociální síť ani feed příspěvků NPC.

## 2. Zásady a stav požadavků

V dokumentaci i Issues se používají tyto stavy:

- **Cíl:** požadavek pro výslednou hru.
- **Prototyp:** první omezená verze systému určená k ověření.
- **Rozpracováno:** existuje zdrojový kód, ale nejsou splněny všechny akceptační testy.
- **Ověřeno:** funkce prošla odpovídajícími testy v Unreal Editoru nebo zabalené hře.
- **Odloženo:** plánováno do pozdější etapy.

Nikdy neoznačovat cíl nebo samotný zdrojový kód za dokončenou funkci bez skutečného testu.

## 3. Herní svět a mapa

### Cílový rozsah

- Jedno hlavní, fiktivní město inspirované Prahou.
- Cílová rozloha světa: **20 × 20 km (400 km²)**.
- Oblasti: historické a moderní centrum, obytná předměstí, venkov, okrajové obce, průmyslové a logistické zóny, parky a dopravní uzly.
- Všechny plánované oblasti mají být přístupné hráči.
- Svět se má postupně měnit podle času, počasí, příběhového postupu a vybraných herních událostí.

### Architektura světa

- Pro velký svět použít **World Partition**, streamování buněk, HLOD a Data Layers.
- Město rozšiřovat po měřitelných čtvrtích. Nejdříve vytvořit jednu reprezentativní čtvrť, až potom rozšiřovat mapu.
- Hlavní dominanty, silnice, náměstí a orientační body ručně kontrolovat; PCG používat pro opakující se a variabilní obsah.
- Vzdálené oblasti a interiéry se nesmí všechny držet současně plně načtené v paměti.

### Interiéry a destrukce

- Přístupné interiéry mají být ve výsledku co nejpočetnější.
- Důležité příběhové budovy dostanou vlastní, detailně zpracované interiéry.
- Ostatní budovy mohou používat opakovaně použitelné modulární interiéry, které se streamují podle potřeby.
- Částečná destrukce se týká vybraných objektů, vybavení a předem připravených částí prostředí. Neplánuje se fyzikálně zničit celé 400km² město.
- Změny světa se budou ukládat selektivně, aby save soubory a výkon zůstaly pod kontrolou.

## 4. Hlavní postavy

Hráč bude ovládat tři dospělé hlavní postavy. Následující jména a profily jsou první autorský návrh a lze je upravit v předprodukci:

| Postava | Pohlaví | Charakter a příběhová role |
|---|---|---|
| Adam Vacek | Muž | Klidný, přemýšlivý a vytrvalý. Jeho příběh tvoří hlavní osu vězeňské části a následného návratu do města. |
| Klára Dvořáková | Žena | Samostatná, všímavá a praktická. Má vlastní vazby na město, rodinu a události mimo hlavní dějovou linku. |
| Filip Mareš | Muž | Mladší, společenský a impulzivnější. Zná různé části města a propojuje několik skupin postav. |

Požadavky pro všechny tři postavy:

- vlastní příběhový oblouk, motivace, přátelé a vztahy;
- vlastní domov a osobní zázemí;
- možnost přepínání mezi postavami;
- úprava vzhledu, oblečení a vybraných osobních prvků;
- rozdílné dialogy, reakce světa a dostupné příběhové možnosti;
- přepnutí postavy musí bezpečně uložit a obnovit její stav.

Konkrétní herní schopnosti postav se navrhnou tak, aby nabízely odlišný styl hraní, ale žádná postava nesmí být povinná jen kvůli jediné nepřeskočitelné mechanice.

## 5. Příběh a mise

- Cílová délka hlavního příběhu: přibližně **40 hodin**.
- Cíl obsahu: **100 hlavních misí a 200 vedlejších misí**.
- Hlavní námět: kriminální příběh, vězení, útěk z vězení a následné důsledky pro tři hlavní postavy.
- Více zakončení podle důležitých rozhodnutí hráče.
- Filmové cutscény, výrazné animace, rozsáhlé dialogy a vedlejší příběhy.
- Volné hraní po dokončení hlavní příběhové linky.
- Vedlejší mise mají mít vlastní motivaci a následky; nemají být pouze opakovanými úkoly bez kontextu.

Počet hodin a misí je cílový obsah plné hry. Bude se rozpracovávat po kapitolách a ověřovat testováním tempa, kvality a opakovatelnosti. V první hratelné verzi se nebude předstírat, že je hotových všech 300 misí.

## 6. Pohyb, kamera a interakce

- Kombinovaný pohyb: chůze, běh, sprint, skok a situační pohyb podle prostředí.
- Kamera: první i třetí osoba s možností přepnutí.
- Plavání a lezení/šplhání.
- Interakce a dialogy s NPC.
- Sbírání předmětů, inventář a systém vybavení.
- Samostatná menu pro inventář a další přehledy.
- Ovládání klávesnicí a myší; vlastní přemapování kláves.
- Podpora gamepadu není v aktuálním zadání požadována.

## 7. Vozidla a doprava

- Dlouhodobý katalog přibližně **200 vozidel a variant**.
- Kategorie: osobní auta, motocykly, skútry, taxi, užitková vozidla, veřejná doprava, letadla a vrtulníky. Lodě lze doplnit jako další kategorii při detailním návrhu mapy.
- Fyzika má působit co nejrealističtěji, ale zůstane hratelná a konfigurovatelná.
- Poškození, opravy, úpravy, nákup a ukládání vlastněných vozidel.
- Interiéry vozidel pro pohled z první osoby.
- Dopravní systém bude propojen s počasím, denní dobou a simulací města.

Dvě stě vozidel znamená především cílový katalog obsahu. Nejdříve se vytvoří několik kvalitních reprezentativních kategorií, jednotný fyzikální základ a výrobní pipeline pro další varianty.

## 8. NPC a simulace města

Cílová populace: až **500 NPC současně přítomných v rámci městské simulace**.

NPC mají mít:

- denní režim, domov a práci;
- osobnostní parametry a vztahy;
- komunikaci a situační reakce na okolí;
- reakce na déšť, sníh, teplotu a denní dobu;
- omezenou paměť důležitých setkání s hráčem;
- přirozené animace a pohyb po chodnících a veřejných prostorech.

### Výkonová architektura NPC

Pojem „500 NPC“ neznamená, že všech 500 postav bude neustále provádět plnou navigaci, rozhodování a animaci na maximální frekvenci.

Použijí se úrovně simulace:

1. **Blízko hráče:** plná AI, navigace, animace, dialogy a interakce.
2. **Ve stejné čtvrti:** zjednodušená rozhodnutí a méně časté aktualizace.
3. **Mimo dohled:** časově posunutá simulace směn, přesunů a důležitých stavů bez plného pathfindingu a animace.
4. **Neaktivní oblasti:** pouze uložený stav a řídké aktualizace při vstupu hráče do oblasti.

Limity jednotlivých úrovní se určí profilováním. Cílem je důvěryhodné město, ne zbytečné vytížení CPU.

## 9. Policie a záchranné složky

Ve světě budou vystupovat:

- policie;
- zdravotnická záchranná služba;
- hasiči;
- specializované zásahové jednotky;
- armáda jako vzácný příběhový nebo světový prvek.

Systémy mají obsahovat hledanost, sdílení informací mezi složkami a dynamické události. Jejich chování bude navržené jako herní simulace s jasnými stavy, limity a možností bezpečného ukončení události. Detailní realistické taktické postupy nejsou cílem produktu.

## 10. Ekonomika, telefon a obchody

- Peníze lze získávat různými herními aktivitami a příběhovými úkoly.
- Bankovní systém, hotovost a transakce.
- Obchody s vlastním sortimentem.
- Nabídka obchodů a ceny se mohou měnit podle stavu herního světa.
- Ekonomika se oddělí od multiplayerové ekonomiky, pokud bude multiplayer později realizován.
- Mobilní telefon hráče nabídne mapu/GPS, bankovnictví, kontakty a herní funkce.
- Žádná sociální síť, feed příspěvků ani automatické příspěvky NPC.

## 11. Domovy, garáže a vlastnictví

V dotazníku jsou dvě protichůdné volby: v ekonomice je nákup nemovitostí označen „Ne“, ale v části nemovitostí je požadováno vlastnit neomezený počet nemovitostí a upravovat byty.

**Pracovní výklad pro návrh:** hráč nekupuje pozemky ani podniky na volném realitním trhu. Domovy a byty získává odemykáním v příběhu nebo postupem ve hře; jejich vybavení lze upravovat. Garáže a úložné prostory pro vozidla lze získávat samostatně. Počet vlastních míst se nemá uměle omezit na několik málo slotů, ale musí být technicky omezený validací uložených dat a výkonu.

Nákup nábytku není plánován. Úpravy bytů se proto budou řešit výběrem stylů a odemykatelnými variantami vybavení, nikoli nákupem jednotlivých kusů z obchodu. Funkční vybavení domácnosti zůstává požadavkem.

Tento výklad zachovává „nekupovat nemovitosti“ a současně umožňuje vlastnit více domovů a garáží. Pokud se později rozhodne jinak, upraví se ekonomický design.

## 12. Počasí, čas a roční období

- Plný denní/noční cyklus.
- Dynamický déšť, sníh, mlha, bouřky, vítr, oblačnost a změny teploty.
- Čtyři roční období.
- Počasí ovlivňuje chování NPC, viditelnost, povrchy a dopravu.
- Sezónní změny vegetace, osvětlení, oblečení NPC a vzhledu prostředí.
- Počasí nesmí být jen vizuální efekt; musí mít konzistentní a testovatelné herní dopady.

## 13. Grafika a rendering

Grafika má **maximální prioritu**. Vizuální cíl je fotorealistický, s detailní architekturou, materiály, animacemi a kvalitním nasvícením.

### Cílové technologie

- Unreal Engine 5.8 jako aktuální vývojový základ.
- Lumen pro globální osvětlení a odrazy.
- Nanite pro vhodné statické mesh assety.
- Virtual Shadow Maps pro detailní stíny.
- Ray tracing jako volitelná funkce podle GPU, operačního systému a výkonu.
- TSR jako obecná základní metoda vyhlazování/upscalingu.
- DLSS, AMD FSR a Intel XeSS jako volitelné cesty podle kompatibility zařízení a verze pluginu.

Žádný vendor-specific upscaler se nesmí vynucovat na nepodporované kartě. Hra musí mít bezpečnou výchozí cestu pro hardware bez DLSS/FSR/XeSS. Jednotlivé pluginy musí projít kontrolou licence, kompatibility, sestavení a balení pro každý cílový operační systém.

### Obsah a assety

- Vlastní modulární architektura města, fasády, střechy, okna, chodníky, mobiliář a vegetace.
- PBR materiály s kvalitními texturami a správným mip streamingem.
- 2K textury jako výchozí standard pro běžně viditelné prostředí; 4K pro hlavní dominanty a blízké hero assety, pokud to dovolí paměť.
- Originální animace a filmové přechody.
- World Partition, HLOD a streamování interiérů a regionů.
- Kvalitní denní, noční, interiérové a nepříznivé povětrnostní světelné scénáře.

### Kvalita a výkon

Grafická kvalita má přednost před pevným limitem 5 GB, ale hra musí mít škálovatelné profily. Vizuální nastavení se budou měřit při 1080p, 1440p a 4K. **120 FPS je cílová hodnota, nikoli záruka při každém rozlišení a na každé grafické kartě.**

## 14. Zvuk, hudba a jazyky

- Originální soundtrack.
- Cíl: 14 tematicky odlišných rádiových stanic.
- Namluvené dialogy NPC a hlavních postav.
- Prostorový zvuk a prostorová akustika interiérů.
- Čeština jako hlavní jazyk.
- Angličtina a němčina jako další lokalizace.
- První prioritou je český dabing a kompletní české texty; rozsah dabingu v angličtině a němčině se bude plánovat podle rozpočtu a výrobní kapacity.
- Hudba a hlasový obsah musí být originální nebo řádně licencovaný.

## 15. Multiplayer — pozdější etapa

- Základní produkt se nejprve vyvíjí jako singleplayer.
- Multiplayer se začne řešit až po stabilizaci příběhu, ukládání, ekonomiky, vozidel a NPC.
- Cíl: kooperace až pro **10 hráčů**.
- Dedikované servery, server-authoritative stav světa a oddělená multiplayerová ekonomika.
- Multiplayer nebude blokovat dokončení singleplayeru.
- Před zahájením multiplayeru bude vytvořen samostatný síťový a bezpečnostní návrh.

## 16. Herní režimy a obtížnost

- Obtížnosti: Lehká, Normální, Těžká, Ultra těžká.
- Volné hraní po dokončení příběhu.
- Survival režim se nevytváří.
- Asistenty a vybrané pomocné prvky lze vypnout.
- Obtížnost má měnit nastavení výzvy, nikoli nepředvídatelně rozbíjet chování NPC nebo ekonomiky.

## 17. Uživatelské rozhraní a přístupnost

- Moderní, minimalistický HUD.
- Herní mapa s GPS a navigací.
- Samostatné menu inventáře, telefonu, nastavení a přehledu úkolů.
- Přemapování kláves a myši.
- Ovládání gamepadem není v aktuálním rozsahu.
- Lokalizované texty a čitelné titulky.

## 18. Platformy a technická distribuce

Hráč nemá instalovat Unreal Engine ani Unreal Editor. Dostane **zabalenou samostatnou hru** pro svůj operační systém. To nevylučuje běžné systémové ovladače nebo případné redistribuovatelné runtime komponenty.

| Platforma | Plán |
|---|---|
| Windows 10/11 x64 | První a hlavní vydávací cíl; offline instalátor. Windows 11 je preferovaný testovací systém. |
| Linux | Samostatný nativní build. Nejprve ověřit podporovanou referenční distribuci a následně rozšiřovat testovací matici. Nelze slíbit funkčnost na každé existující linuxové distribuci bez testu. |
| macOS | Samostatný macOS build a testy na Apple Silicon; grafické funkce a kvalita se mohou lišit od Windows. |

Unreal Engine umí připravovat balíčky pro Windows, macOS a Linux. Jednotlivé platformy ale vyžadují samostatné buildy, SDK, pluginy a testování. „Všechny Linuxy“ se proto převádí na podporovanou a zveřejněnou testovací matici.

Grafické funkce nebudou na všech platformách totožné. UE 5.8 uvádí pro macOS softwarový Lumen na Apple Silicon M1+, zatímco Nanite a Virtual Shadow Maps mají na Apple Silicon M2+ beta podporu; Windows DX12 cesta proto nebude automaticky kopírována na Mac. DLSS vyžaduje kompatibilní NVIDIA GPU. AMD FSR a Intel XeSS mají vlastní hardwarové, RHI a platformní požadavky; aktuální Intel XeSS UE plugin dokumentuje pouze Windows x64 s DirectX 12. Výběr upscaleru musí být podmíněný dostupností a hra musí mít obecnou TSR cestu.

### Grafické karty

Cíl je podpora NVIDIA, AMD a Intel na PC. Funkce jako ray tracing a vendor-specific upscaling budou aktivní jen na kompatibilním hardwaru. Mac používá odlišnou grafickou cestu Metal a nedostává automaticky stejnou sadu funkcí jako Windows DX12.

### Rozlišení a výkon

- Režimy: 1920×1080, 2560×1440 a 3840×2160.
- Cíl: až 120 FPS na odpovídajícím hardwaru a profilu.
- Testovat průměrné FPS, 1% low, CPU/GPU frame time, RAM, VRAM, načítání a plynulost streamování.
- Cílový vývojový počítač: Ryzen 5 5600, RTX 5050 8 GB, 16 GB DDR4 RAM, SSD.
- Pro vývoj v Unreal Editoru je 16 GB RAM omezení; pro pohodlnější práci s velkým obsahem se doporučuje 32 GB systémové paměti.

## 19. Velikost hry a instalátor

- Orientační cíl první plné instalace: kolem **5 GB**, pokud to kvalita a obsah dovolí.
- 5 GB není pevný strop. Pokud bude potřeba více prostoru pro kvalitní textury, modely, hlasový obsah a svět, nebude se obsah odstraňovat jen kvůli číslu.
- První distribuce: Windows x64 offline instalátor.
- Offline instalace nesmí vyžadovat stažení hry z internetu.
- Aktuální plán jednoho EXE má limit velikosti souboru; pokud jej skutečný komprimovaný balík překročí, použije se vícedílné offline médium. Hráč stále nebude potřebovat internet.
- Velikost se bude vykazovat jako velikost staženého instalátoru i velikost nainstalované hry — nejsou to stejné hodnoty.

## 20. Rozpočet a cena

Uvedené rozmezí 50–1 000 Kč chápu jako předběžnou **cílovou prodejní cenu podle výsledné kvality**, nikoli jako rozpočet na výrobu celé hry. Skutečný vývojový rozpočet a náklady na dabing, hudbu, assety, testování a distribuci nejsou zatím vyčísleny. Finální cenu určit až podle rozsahu a kvality skutečně dokončené hry.

## 21. Etapy vývoje

### Etapa 0 — projektová základna
- stabilní UE projekt, CI a build pipeline;
- pravidla pro assety, licence, zálohování a verzování;
- základní technická rozhodnutí a měřitelná kritéria QA.

### Etapa 1 — hratelný vizuální vertical slice
- jedna malá, detailně zpracovaná čtvrť;
- hráč, kamera, pohyb a interakce;
- jeden reprezentativní interiér;
- několik kvalitních vozidel;
- první NPC a denní/noční cyklus;
- grafický profil, benchmark a test zabalené hry.

### Etapa 2 — systémy města
- rozšíření NPC simulace a dopravy;
- počasí a roční období;
- policie a záchranné události na úrovni herní simulace;
- ekonomika, obchody, telefon, domovy a garáže;
- robustní save/load a přetrvávající změny světa.

### Etapa 3 — příběh a produkce obsahu
- tři hratelné postavy;
- hlavní a vedlejší mise po kapitolách;
- cutscény, dialogy, dabing a soundtrack;
- rozšiřování města a interiérů;
- větší katalog vozidel.

### Etapa 4 — rozsah plné hry
- postupné dosažení cílové mapy 20 × 20 km;
- cílová populace 500 NPC ve více simulačních úrovních;
- cílový katalog přibližně 200 vozidel;
- cílový obsah 100 hlavních a 200 vedlejších misí;
- všechna roční období, více zakončení a endgame volné hraní.

### Etapa 5 — multiplatformní vydání
- samostatné Windows, Linux a macOS buildy;
- kompatibilita grafických pluginů a náhradní renderer cesty;
- testovací matice hardware/OS;
- offline instalace a bezpečný aktualizační proces.

### Etapa 6 — multiplayer
- síťový model a dedikované servery;
- kooperace do 10 hráčů;
- samostatná multiplayerová ekonomika;
- zátěžové, síťové a bezpečnostní testy.

Etapy nejsou pevné termíny. Každá se uzavře až po definovaných testech, ne pouze po napsání zdrojového kódu.

## 22. Současný stav repozitáře

Při sepsání tohoto dokumentu:

- existuje základní C++ projekt Unreal Engine 5.8;
- existuje jednoduchá runtime greybox čtvrť z primitivní geometrie;
- existuje první prototyp civilního NPC a spawneru;
- existuje první fyzikální prototyp vozidla;
- existuje zdrojový základ ukládání a načítání;
- v repozitáři je také samostatný browser demo prototyp, který není finální zabalenou UE hrou;
- finální fotorealistické assety, kompletní 20 × 20 km World Partition mapa, plný příběh, katalog 200 vozidel, 500 plně simulovaných NPC, multiplatformní buildy a multiplayer nejsou hotové;
- není potvrzen UE C++ build, editorový runtime test ani měření výkonu finální zabalené hry.

Další programování musí vycházet z aktuálního main a otevřených PR. Neprovádět hromadné přepisování existujícího kódu jen kvůli změně dokumentu.

## 23. Otevřená rozhodnutí

1. **Nemovitosti:** pracovní výklad v části 11 řeší rozpor mezi „nekupovat nemovitosti“ a „vlastnit neomezený počet“. Před implementací ekonomiky jej může uživatel změnit.
2. **Dabing:** český dabing je hlavní priorita; rozsah anglického a německého dabingu je závislý na kapacitě.
3. **Referenční Linux distribuce:** nejdříve vybrat a testovat konkrétní distribuci; následně rozšiřovat podporu.
4. **Cílová cena:** určit až podle reálného rozsahu a kvality, protože uvedené rozmezí není vývojový rozpočet.

## 24. Oficiální technické reference

- Unreal Engine 5.8 — [Hardware and Software Specifications](https://dev.epicgames.com/documentation/en-us/unreal-engine/hardware-and-software-specifications-for-unreal-engine)
- Unreal Engine 5.8 — [Packaging Your Project](https://dev.epicgames.com/documentation/en-us/unreal-engine/packaging-your-project)
- Unreal Engine 5.8 — [World Partition](https://dev.epicgames.com/documentation/en-us/unreal-engine/world-partition-in-unreal-engine)
- Unreal Engine 5.8 — [macOS Development Requirements](https://dev.epicgames.com/documentation/unreal-engine/macos-development-requirements-for-unreal-engine?lang=en-US)
- Unreal Engine 5.8 — [Linux Game Development](https://dev.epicgames.com/documentation/unreal-engine/linux-game-development-in-unreal-engine)
- NVIDIA — [DLSS Unreal Engine plugin](https://developer.nvidia.com/rtx/dlss)
- AMD GPUOpen — [FSR Unreal Engine plugin](https://gpuopen.com/unreal-engine/)
- Intel — [XeSS Unreal Engine plugin](https://www.intel.com/content/www/us/en/developer/articles/technical/xess-plugin-for-unreal-engine.html)
