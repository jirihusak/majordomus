# Konfigurace Majordomus Control

Veškerá konfigurace se provádí přes webové rozhraní na adrese `http://<IP_ADRESA>:8899`. Žádné ruční editování konfiguračních souborů není potřeba — vše nastavíte z prohlížeče.

![Webové rozhraní Majordomus Control](../assets/majordomusControl.png)

---

## MQTT Broker

Nastavení připojení k MQTT brokeru (Mosquitto), který slouží jako komunikační vrstva mezi Majordomus Control a aplikacemi (Node-RED, Home Assistant...).

- **IP adresa a port** — adresa MQTT brokeru (typicky `localhost:1883`, pokud běží na stejném zařízení)
- **Autentizace** — uživatelské jméno a heslo pro připojení
- **TLS certifikáty** — pro šifrovanou komunikaci v produkčním nasazení

---

## Home Assistant Integration

Povolí automatické odesílání konfiguračních informací o zařízeních Majordomus do Home Assistantu. Díky tomu se vaše senzory, spínače a další zařízení **automaticky objeví** v Home Assistantu bez nutnosti ruční konfigurace.

!!! tip "Auto-discovery"
    Po povolení této integrace stačí v Home Assistantu přejít do Nastavení → Zařízení a služby — vaše Majordomus jednotky se tam samy zobrazí.

---

## Serial Connections

Nastavení sériových portů, na kterých jsou připojeny převodníky USB → RS-485. Každý sériový port odpovídá jedné lince sběrnice.

Typické názvy portů:

| Systém | Příklad |
|---|---|
| Linux | `/dev/ttyUSB0`, `/dev/ttyUSB1`, `/dev/ttyUSB2` |
| Windows | `COM3`, `COM4`, `COM5` |

!!! note "Více linek"
    Pokud máte v domě více linek (např. jednu na patro), každá linka má svůj převodník a tedy svůj sériový port. Všechny porty nastavíte zde.

---

## Devices

Seznam všech HW jednotek v systému a jejich přiřazení k jednotlivým sériovým portům (linkám). Podle tohoto seznamu Majordomus Control ví, s jakými zařízeními má komunikovat a na které lince je hledat.

Pro každé zařízení nastavíte:

- **Typ** — RoomSensor, RoomIO, BoxIO
- **Adresa** — unikátní adresa zařízení na sběrnici
- **Sériový port** — na které lince je zařízení připojeno
- **Název** — volitelný popisek pro snadnou orientaci (např. „Obývák", „Koupelna 2.NP")

---

## Konfigurační soubor

Konfigurace se automaticky ukládá do souboru `config.xml` v pracovním adresáři aplikace Majordomus Control (typicky `~/majordomus/config.xml`). Soubor se generuje automaticky přes webové rozhraní — ruční editace není nutná, ale je možná.

