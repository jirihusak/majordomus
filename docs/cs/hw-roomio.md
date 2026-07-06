# RoomIO

RoomIO je multifunkční jednotka vstupů a výstupů s rozhraním RS-485. Poskytuje 8 digitálních vstupů, 8 digitálních výstupů, 2 analogové vstupy, 2 analogové výstupy a 2 open-drain výstupy. Lze ji rozšířit o teploměry a další senzory přes 1-Wire a I2C.

Díky kompaktnímu designu je možné jednotku snadno skrýt — instaluje se přímo do instalační krabice (KP 68) a překryje záslepkou, případně se umístí do podhledu.

---

## Vlastnosti

- Průmyslové rozhraní RS-485
- 8× digitální vstup s optickým oddělením
- Funkce čítače digitálních vstupů s pamětí FRAM (64bit čítač — hodnoty přežijí výpadek napájení)
- 8× digitální výstup (6–30 V dle napájení, do 100 mA)
- 2× analogový vstup 0–30 V
- 2× analogový výstup 0–10 V
- 2× port 1-Wire pro externí teploměry DS18B20
- 1× port I2C pro externí zařízení
- 2× open-drain výstup pro ovládání RGB LED WS2812

---

## Parametry

| Parametr | Hodnota |
|---|---|
| Vstupní napájení | 6–30 V DC |
| Komunikační rozhraní | RS-485 (ASCII protokol), Modbus |
| Instalační krabice | Přístrojová krabice KP 68 |
| Mikrokontrolér | PIC18F57K42 |
| Aktualizace FW po RS-485 | Ano |
| Stupeň krytí | IP30 |
| Teplotní rozsah | −20 až +60 °C |

---

## Typické použití

**Digitální vstupy** — připojení tlačítek, magnetických (okenních/dveřních) kontaktů nebo mikrospínačů. Každý vstup má 64bitový čítač impulsů s pamětí FRAM — ideální pro odečty elektroměrů, vodoměrů a plynoměrů s impulsním výstupem.

**Digitální výstupy** — spínání elektromagnetických relé, SSR relé nebo indikačních LED. Vstupy i výstupy pracují s napájecím napětím jednotky (typicky 12 nebo 24 V DC).

**Analogové vstupy 0–30 V** — snímače tlaku, ultrazvukové snímače vzdálenosti (např. hladina v nádrži) a podobně.

**Analogové výstupy 0–10 V** — stmívatelné zdroje LED osvětlení nebo proporcionální ventily topení.

**1-Wire** — až několik teploměrů DS18B20 na jednom portu, např. pro měření teploty podlahy nebo venkovní teploty.

!!! tip "Objednání a aktuální ceny"
    Aktuální ceny a objednávkový formulář najdete na [majordomus.tech/roomio](https://majordomus.tech/roomio/).
