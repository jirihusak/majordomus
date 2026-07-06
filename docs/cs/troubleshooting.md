# Řešení problémů

Když něco nefunguje, postupujte **po vrstvách odspodu nahoru** — od napájení přes sběrnici a sériový port až po aplikace. Každá kapitola končí stavem, který musí platit, než má smysl pokračovat na další vrstvu.

```mermaid
graph LR
    A[1. Napájení] --> B[2. Sběrnice RS-485]
    B --> C[3. Linux — sériový port]
    C --> D[4. Majordomus Control]
    D --> E[5. MQTT]
    E --> F[6. Home Assistant]
```

---

## 1. Napájení

**Co zkontrolovat:**

- Svítí/žije jednotka? RoomSensor po připojení napájení rozsvítí displej. BoxIO má LED indikaci.
- Změřte multimetrem napětí **přímo na svorkách jednotky** (ne u zdroje) — na dlouhém UTP kabelu vzniká úbytek. Napětí musí být v pracovním rozsahu jednotky (RoomSensor 7–27 V, RoomIO 6–30 V, BoxIO 10–30 V).
- Zkontrolujte polaritu — vodiče 12V a GND.
- Pokud je napětí u zdroje v pořádku, ale na jednotce ne: zkrat na lince, nebo příliš tenký/dlouhý kabel. Převodník RS485 2 USB má **vratnou pojistku** — při zkratu odpojí napájení linky a po odstranění zkratu se sama obnoví.

✅ *Dál pokračujte, až má každá jednotka na svorkách napětí v pracovním rozsahu.*

---

## 2. Sběrnice RS-485 a převodník

Převodník RS485 2 USB má **indikační LED pro TX a RX** — to je nejrychlejší diagnostika sběrnice:

| TX | RX | Význam |
|---|---|---|
| bliká | bliká | Vše v pořádku — master se ptá, jednotky odpovídají |
| bliká | nesvítí | Dotazy odchází, ale nikdo neodpovídá → problém na lince nebo v jednotkách |
| nesvítí | nesvítí | Majordomus Control nekomunikuje → pokračujte kapitolou 3 a 4 |

**Když TX bliká a RX ne:**

- **Prohozené vodiče A a B** — nejčastější chyba. Zkuste je na převodníku prohodit.
- Zkontrolujte průchodnost vodičů A a B po celé délce linky (jednotky jsou zapojené průběžně — přerušení uprostřed odpojí všechny další).
- Zkontrolujte **zakončovací odpor** — na převodníku se aktivuje jumperem.
- Zkuste jednotku připojit **napřímo krátkým kabelem** na stole — oddělíte tak problém kabeláže od problému jednotky.
- Neodpovídá jen jedna jednotka? Může mít špatné/duplicitní `id` — viz Autodetect v kapitole 4.

✅ *Dál pokračujte, až blikají obě LED.*

---

## 3. Linux — sériový port

**Vidí systém převodník?**

```bash
ls /dev/ttyUSB*
```

Měl by se zobrazit `/dev/ttyUSB0` (případně další). Pokud ne:

```bash
# je převodník vidět na USB? (čip FTDI FT230X)
lsusb | grep -i ftdi

# co se stalo po připojení kabelu?
sudo dmesg | tail -20
```

- **`lsusb` převodník nevidí** → problém s USB kabelem (zkuste jiný — některé USB-C kabely jsou jen napájecí), portem nebo převodníkem.
- **`lsusb` ho vidí, ale `/dev/ttyUSB0` neexistuje** → na desktopových distribucích port často zabírá služba `brltty` (podpora braillských řádků). Odinstalujte ji: `sudo apt remove brltty` a převodník znovu připojte.

**Máte práva k portu?** Uživatel musí být ve skupině `dialout`:

```bash
groups                # je tam "dialout"?
sudo usermod -a -G dialout $USER   # přidání; poté se odhlaste a přihlaste
```

**Nesahá na port něco jiného?** Port může držet jen jedna aplikace současně:

```bash
sudo fuser -v /dev/ttyUSB0
```

!!! note "Více převodníků"
    Čísla `ttyUSB0`, `ttyUSB1`... se přidělují podle pořadí připojení a po restartu se mohou prohodit. Pokud máte více linek, ověřte, který port patří které lince (odpojte/připojte převodník a sledujte `dmesg`).

✅ *Dál pokračujte, až existuje `/dev/ttyUSB0` a jste ve skupině `dialout`.*

---

## 4. Majordomus Control (port 8899)

Otevřete webové rozhraní `http://<IP_ADRESA>:8899`.

**Stránka se nenačte?**

```bash
# běží služba?
sudo systemctl status majordomus

# poslouchá na portu?
ss -tlnp | grep 8899
```

Projděte log služby: `journalctl -u majordomus -e`. Typická chyba je špatný název sériového portu v konfiguraci nebo port obsazený jinou aplikací.

**Stránka běží — zkontrolujte zařízení:**

- V sekci **Devices** je u každého zařízení stav **online / offline**.
- Zařízení offline? Zkontrolujte, že souhlasí **id zařízení** (rozlišuje velikost písmen), **typ** a **sériový port** (linka), na kterém je připojené.

!!! tip "Autodetect — nechte zařízení najít automaticky"
    V sekci konfigurace klikněte na **🔍 Autodetect**. Majordomus Control projde všechny sériové porty, osloví zařízení na sběrnici a nabídne nalezenou konfiguraci (porty, id a typy zařízení) k uložení. Je to nejrychlejší způsob, jak zjistit, „co na lince opravdu je".

- Zařízení se objevuje a mizí (střídá online/offline)? Podezření na **duplicitní id** dvou jednotek nebo rušení na dlouhé lince (zkontrolujte zakončovací odpor a kvalitu kabelu).

✅ *Dál pokračujte, až jsou všechna zařízení online.*

---

## 5. MQTT

**Běží broker a chodí do něj data?**

```bash
sudo systemctl status mosquitto

# sledování všech témat Majordomu
mosquitto_sub -h localhost -t "majordomus/#" -v
```

- **Zprávy chodí** → MQTT vrstva je v pořádku, pokračujte kapitolou 6.
- **Nic nechodí** → zkontrolujte v Majordomus Control sekci **MQTT Broker**: adresu (`tcp://localhost:1883`), uživatelské jméno a heslo. Chybu připojení uvidíte v logu (`journalctl -u majordomus -e`).
- **Connection refused** → broker neběží, poslouchá jen na jiné adrese, nebo blokuje anonymní přístup (viz `/etc/mosquitto/mosquitto.conf`).

**Test ovládání** — zapněte výstup ručně:

```bash
mosquitto_pub -h localhost -t "majordomus/<zařízení>/cmd/do0" -m "1"
```

✅ *Dál pokračujte, až v `mosquitto_sub` vidíte proud zpráv.*

---

## 6. Home Assistant

**Zařízení se v HA neobjevují?**

- V Majordomus Control musí být povolená **Home Assistant Integration** (u doplňku volba `ha_discovery: true`).
- Home Assistant musí být připojený ke **stejnému brokeru** — Nastavení → Zařízení a služby → MQTT.
- Discovery prefix musí souhlasit (`homeassistant/` na obou stranách).
- Ověřte, že discovery zprávy odchází: `mosquitto_sub -h localhost -t "homeassistant/#" -v` — po restartu Majordomus Control se musí zobrazit `config` zprávy.

**Entity existují, ale jsou „nedostupné" (unavailable)?**

- Zařízení je offline na RS-485 (téma `state/online` = `offline`) → vraťte se ke kapitole 4.
- Hodnota `NaN` u konkrétního senzoru = senzor není osazen/připojen — to je v pořádku.

---

## Rychlá tabulka nejčastějších problémů

| Příznak | Nejčastější příčina | Řešení |
|---|---|---|
| Jednotka nejeví známky života | Úbytek napětí na kabelu, přehozená polarita | Změřit napětí na svorkách jednotky |
| TX bliká, RX ne | Prohozené vodiče A/B | Prohodit A a B |
| Chybí `/dev/ttyUSB0` | Nabíjecí USB kabel, `brltty` | Vyměnit kabel, `sudo apt remove brltty` |
| „Permission denied" na portu | Uživatel není v `dialout` | `sudo usermod -a -G dialout $USER` + přihlásit znovu |
| Zařízení offline v aplikaci | Špatné id / typ / port v konfiguraci | Spustit **Autodetect** |
| Zařízení střídá online/offline | Duplicitní id, rušení na lince | `newId5050`, zakončovací odpor, kabeláž |
| Žádná data v MQTT | Špatná adresa/přihlášení k brokeru | Zkontrolovat sekci MQTT Broker + log |
| Zařízení nejsou v HA | Vypnuté discovery, jiný broker | Zapnout HA integraci, ověřit MQTT integraci v HA |
