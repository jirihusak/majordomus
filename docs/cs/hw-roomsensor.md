# RoomSensor

RoomSensor je švýcarský nožík mezi senzory do místnosti. Instaluje se do běžné elektroinstalační krabice (KP 68) a v jediném kompaktním zařízení pokryje vše, co v místnosti potřebujete — místo čtyř různých krabiček stačí jeden RoomSensor:

- **Jednotka vstupů a výstupů** — 4× digitální vstup, 4× digitální výstup, 2× analogový vstup, 2× analogový výstup
- **Měření veličin prostředí** — teplota, relativní vlhkost, kvalita vzduchu (VOC, NOx, CO₂)
- **Detekce přítomnosti** — PIR senzor, hlukoměr, intenzita osvětlení
- **Rozhraní pro uživatele** — grafický displej, kapacitní tlačítka a akustická signalizace

---

## Vlastnosti

### Senzory prostředí

| Veličina | Senzor |
|---|---|
| Teplota | TMP1075 |
| Relativní vlhkost | SHT-45 (s ochrannou membránou) |
| CO₂ | STCC4 |
| VOC a NOx | SGP-41 |
| Intenzita osvětlení | VEML7700 |
| Hluk | MEMS mikrofon MP34DT05-A |
| Pohyb | PIR EKMC1607113 |

### Vstupy a výstupy

- 4× digitální vstup s optickým oddělením
- 4× digitální výstup
- 2× analogový vstup 0–30 V
- 2× analogový výstup 0–10 V
- 2× port 1-Wire pro externí teploměry DS18B20
- 1× port I2C pro externí zařízení

### Uživatelské rozhraní

- Grafický displej 128×32 px (NHD-C12832A1Z)
- 3 kapacitní tlačítka (mínus, enter, plus)
- Piezo pípák pro akustické upozornění
- LED přísvit

---

## Parametry

| Parametr | Hodnota |
|---|---|
| Vstupní napájení | 7–27 V DC |
| Komunikační rozhraní | RS-485 (ASCII protokol), Modbus |
| Instalační krabice | Přístrojová krabice KP 68 |
| Design | ABB Tango |
| Mikrokontrolér | PIC18F57K42 |
| Aktualizace FW po RS-485 | Ano |
| Stupeň krytí | IP30 |
| Teplotní rozsah | −20 až +60 °C |

---

## Typické použití

Jeden RoomSensor v každé obytné místnosti — obývák, ložnice, dětský pokoj, kuchyň:

- Řízení topení podle skutečné teploty v místnosti
- Spouštění ventilace / rekuperace podle CO₂ a vlhkosti
- Automatické osvětlení podle přítomnosti a intenzity denního světla
- Ovládání světel a žaluzií kapacitními tlačítky
- Zobrazení teploty a dalších hodnot na displeji

Přes digitální a analogové vstupy/výstupy lze navíc přímo z jednotky ovládat stmívatelné LED osvětlení (0–10 V), číst okenní kontakty nebo spínat relé.

!!! tip "Objednání a aktuální ceny"
    Aktuální ceny a objednávkový formulář najdete na [majordomus.tech/roomsensor](https://majordomus.tech/roomsensor/).
