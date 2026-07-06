# Převodník RS485 2 USB

RS485 2 USB propojuje řídící počítač (Raspberry Pi, mini PC) se sběrnicí RS-485. Do počítače se připojuje konektorem USB-C a poskytuje jednu galvanicky izolovanou linku RS-485 — na ni doporučujeme připojit maximálně 10 jednotek Majordomus.

Každá linka (sběrnice) v domě potřebuje svůj převodník — viz [Topologie](topology.md).

---

## Vlastnosti

- Průmyslové rozhraní RS-485 s **galvanickým oddělením** — rušení z dlouhých linek se nepřenáší do řídícího počítače
- 1 nebo 4 komunikační linky včetně rozvodu napájení
- **Vratná pojistka** — chrání UTP kabel při zkratu na lince
- Indikační LED pro RX a TX — okamžitá vizuální kontrola komunikace
- Zakončovací odpor volitelný jumperem
- Vyrovnání GND potenciálu linky přes impedanci na PE
- Možnost připojit se přímo na UART

---

## Parametry

| Parametr | Hodnota |
|---|---|
| Vstupní napájení | 7–27 V DC |
| Komunikační rozhraní | USB (konektor USB-C) |
| Převodník | FTDI FT230XS |
| Montáž | DIN lišta |
| Stupeň krytí | IP30 |
| Teplotní rozsah | −20 až +60 °C |

---

## Připojení k počítači

Převodník používá čip FTDI, který mají Linux i Windows ve výbavě — po připojení se objeví jako sériový port bez instalace ovladačů:

| Systém | Název portu |
|---|---|
| Linux / Raspberry Pi OS | `/dev/ttyUSB0` (další převodníky `ttyUSB1`, `ttyUSB2`...) |
| Windows | `COM3`, `COM4`... |

Tento port poté nastavíte v Majordomus Control v sekci **Serial Connections** — viz [Konfigurace SW](sw-configuration.md).

!!! note "Práva k sériovému portu na Linuxu"
    Pokud Majordomus Control nemůže port otevřít, přidejte uživatele do skupiny `dialout` a znovu se přihlaste:

    ```bash
    sudo usermod -a -G dialout $USER
    ```

!!! tip "Objednání a aktuální ceny"
    Aktuální ceny a objednávkový formulář najdete na [majordomus.tech/rs485-2-usb](https://majordomus.tech/rs485-2-usb/).
