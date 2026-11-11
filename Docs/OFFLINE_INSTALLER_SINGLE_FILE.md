# GTA6 — kompletní offline instalátor pro Windows

Distribuční formát je jeden Windows x64 soubor EXE, který obsahuje instalační program i všechny soubory zabalené hry. Instalace nevyžaduje internet ani další ZIP/manifest soubory.

## Zvolená technologie

Používáme **Inno Setup 6.5.2 nebo novější**. Jeho vlastní instalační formát je vhodnější než připojování dat za .NET single-file EXE: .NET single-file host očekává vlastní bundle metadata na konci souboru, takže prosté připojení dat za hotový EXE není bezpečný formát.

Inno Setup od verze 6.5.2 umí vytvořit jeden Setup.exe o velikosti téměř 4 GiB. To je zároveň praktický limit Windows PE souboru. Pokud komprimovaná hra limit překročí, požadavek na jediný EXE nebude možné splnit bez změny distribuce nebo zmenšení herního balíku. Zdroj: https://jrsoftware.org/files/is6-whatsnew.htm.

## Build

1. Zabalit Unreal Engine 5.8 projekt pro Windows x64.
2. Umístit celý výstup do Build/Windows.
3. Ověřit, že hlavní spustitelný soubor je OpenWorldNextGen.exe v kořeni této složky.
4. Nainstalovat Inno Setup 6.5.2+ na Windows build stroj.
5. Spustit z kořene repozitáře:

    .\\Scripts\\Build-OfflineInstaller.ps1 -Version 0.1.0

Výstup: Build/Installer/GTA6-Setup-0.1.0-x64.exe.

Inno Setup zabalí celý obsah Build/Windows přímo do jediného offline instalátoru. Build skript zkontroluje, že výsledný EXE nepřekročil hranici 4 GiB, a vypíše SHA-256.

## Vlastnosti instalace

- Veškeré herní soubory jsou uvnitř instalátoru.
- Žádné stahování, online manifest ani externí archiv.
- Instalace do %LOCALAPPDATA%\\Programs\\GTA6 bez administrátorských práv.
- Odinstalace přes standardní Windows seznam aplikací.
- Volitelná ikona na ploše a spuštění hry po instalaci.
- Inno Setup kontroluje integritu instalovaných dat pomocí svého instalačního formátu.

## CI

GitHub Actions instaluje Inno Setup a kompiluje instalační skript s malým testovacím souborem místo skutečné hry. Tím ověřuje syntaxi skriptu a vznik instalačního EXE, nikoli funkčnost skutečného Unreal buildu.

## Aktuální omezení

V repozitáři zatím není skutečný kompletní Windows build hry. Proto zatím neexistuje finální instalační EXE, nelze změřit jeho velikost a nebyl proveden test instalace na čistém Windows bez internetu. Reálný herní balík musí být vytvořen v Unreal Engine na Windows.
