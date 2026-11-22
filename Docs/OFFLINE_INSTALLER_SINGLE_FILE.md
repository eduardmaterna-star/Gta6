# NEXXT GEN — Windows offline instalátor

## Cíl

Výsledný hráčský produkt se jmenuje **NEXXT GEN**. Hráč si stáhne a nainstaluje zabalenou hru; nemusí instalovat Unreal Engine ani Unreal Editor. Instalace Windows verze má fungovat bez připojení k internetu.

Přítomnost běžných systémových ovladačů nebo runtime komponent neznamená, že by hráč musel instalovat vývojový engine.

## Aktuální jednosouborová varianta

Používáme Inno Setup 6.5.2 nebo novější. Instalátor:
- zobrazuje produkt NEXXT GEN;
- instaluje do %LOCALAPPDATA%\Programs\NexxtGen bez administrátorských práv;
- obsahuje všechny soubory Windows buildu uvnitř offline EXE;
- vytváří nabídku Start a volitelnou ikonu na ploše;
- vypisuje SHA-256 výsledného souboru.

Interní zdrojový soubor Installer/GTA6.iss a název spustitelného souboru OpenWorldNextGen.exe zatím zůstávají kvůli návaznosti současného repozitáře. Hráčská značka a název instalátoru jsou NEXXT GEN.

## Velikost

Cíl instalované velikosti kolem 5 GB je orientační, nikoli tvrdý limit kvality. Jedno Windows EXE má praktickou hranici necelé 4 GiB. Komprimovaný instalátor se může pod tuto hranici vejít i pro větší instalovanou hru, ale musí se to změřit na skutečném buildu.

Pokud komprimovaný obsah překročí limit jednoho EXE, distribuce se změní na více souborů offline média. Hra se nebude zmenšovat ani zbavovat grafických assetů jen kvůli zachování jednoho souboru.

## Build

1. Zabalit Unreal Engine projekt pro Windows x64.
2. Umístit kompletní výstup do Build/Windows.
3. Ověřit, že kořen adresáře obsahuje OpenWorldNextGen.exe.
4. Nainstalovat Inno Setup 6.5.2+ na Windows build stroj.
5. Spustit z kořene repozitáře:

    .\Scripts\Build-OfflineInstaller.ps1 -Version 0.1.0

Výstup: Build/Installer/NexxtGen-Setup-0.1.0-x64.exe.

## CI a ověření

GitHub Actions staticky kontroluje zdroj instalátoru a může zkompilovat testovací balíček. To neprokazuje funkčnost kompletní zabalené UE hry.

Před vydáním musí proběhnout:
- vytvoření skutečného UE 5.8 Windows buildu;
- kompilace instalátoru se všemi herními soubory;
- instalace na čistém Windows bez internetu;
- spuštění hry, kontrola aktualizace a odinstalace;
- kontrola volného místa, SHA-256 a podepsání release artefaktů;
- měření výsledné velikosti staženého i nainstalovaného balíku.

## Aktuální omezení

V repozitáři zatím není kompletní zabalený Windows build hry. Finální instalátor, jeho velikost a instalace bez internetu proto nejsou ověřené.
