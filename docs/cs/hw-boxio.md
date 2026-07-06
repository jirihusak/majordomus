# BoxIO

BoxIO je multifunkční jednotka do rozvaděče s rozhraním RS-485 a USB. Poskytuje 8 digitálních vstupů, 6 silových výstupů a 2 analogové vstupy a výstupy. Montuje se na DIN lištu a zabírá šířku 6 modulů.

Silové výstupy jsou řešeny jako **výměnné moduly** — podle potřeby je osadíte elektromagnetickým relé (max. 16 A) nebo polovodičovým SSR (max. 2 A).

---

## Vlastnosti

- Průmyslové rozhraní RS-485
- Rozhraní USB-C (vestavěný FTDI převodník) — jednotku lze připojit i přímo do PC
- **Manuální ovládání výstupů** — třístavový přepínač pro každý výstup: manuálně zapnuto / manuálně vypnuto / automatika (řízení ze sběrnice)
- LED indikace aktivních vstupů a výstupů
- 8× digitální vstup s optickým oddělením
- Funkce čítače digitálních vstupů s pamětí FRAM
- 6× výměnný modul výstupu (relé max. 16 A, nebo SSR max. 2 A)
- 2× analogový vstup 0–30 V
- 2× analogový výstup 0–10 V
- 2× port 1-Wire pro externí teploměry DS18B20
- 1× port I2C pro externí zařízení

!!! tip "Manuální režim — dům funguje i bez serveru"
    Třístavové přepínače umožňují ovládat výstupy ručně přímo v rozvaděči. Pokud řídící počítač vypadne nebo probíhá servis, zapnete topení či světla přepínačem — bez software, bez počítače.

---

## Parametry

| Parametr | Hodnota |
|---|---|
| Vstupní napájení | 10–30 V DC |
| Komunikační rozhraní | RS-485 (ASCII protokol), Modbus, USB-C |
| Instalace | DIN lišta (šířka 6 modulů) |
| Mikrokontrolér | PIC18F57K42 |
| Aktualizace FW po RS-485 | Ano |
| Stupeň krytí | IP20 |
| Teplotní rozsah | −20 až +60 °C |

---

## Typické použití

BoxIO je určen do rozvaděče — tam, kde se sbíhá silová elektroinstalace:

- Spínání okruhů osvětlení přes relé moduly
- Ovládání ventilů topení, čerpadel a bojleru
- Spínání zásuvkových okruhů
- Odečty elektroměrů a vodoměrů s impulsním výstupem (čítače na digitálních vstupech)
- Řízení stmívatelných LED zdrojů a proporcionálních ventilů přes výstupy 0–10 V

!!! tip "Objednání a aktuální ceny"
    Aktuální ceny a objednávkový formulář najdete na [majordomus.tech/boxio](https://majordomus.tech/boxio/).
