# Konfigurace Home Assistant

Po instalaci zbývají dva kroky: nastavit doplněk Majordomus a propojit Home Assistant s MQTT brokerem. Poté se všechna zařízení Majordomus objeví v Home Assistantu automaticky.

---

## Konfigurace doplňku Majordomus

Nastavení najdete v **Nastavení → Aplikace → Majordomus → Konfigurace** (při instalaci přes Docker odpovídají tyto volby proměnným prostředí v `docker-compose.yml`).

| Volba | Výchozí hodnota | Význam |
|---|---|---|
| `mqtt_host` | `core-mosquitto` | Adresa MQTT brokeru. Výchozí hodnota ukazuje na doplněk Mosquitto v HAOS. |
| `mqtt_port` | `1883` | Port MQTT brokeru. |
| `mqtt_user` | *(prázdné)* | Uživatelské jméno pro MQTT. |
| `mqtt_password` | *(prázdné)* | Heslo pro MQTT. |
| `mqtt_topic` | `majordomus/` | Kořenové MQTT téma, pod kterým Majordomus publikuje data. |
| `ha_discovery` | `true` | Automatické ohlašování zařízení do Home Assistantu (auto-discovery). |
| `ha_topic` | `homeassistant/` | Téma pro auto-discovery — musí odpovídat nastavení MQTT integrace v HA (výchozí hodnotu neměňte). |

Po změně konfigurace doplněk restartujte.

!!! note "Nastavení HW jednotek"
    Sériové porty a seznam HW jednotek se nenastavují zde, ale ve webovém rozhraní Majordomus Control (položka **Majordomus** v bočním panelu). Podrobnosti v kapitole [Konfigurace SW](sw-configuration.md).

---

## Připojení Home Assistant k MQTT

Aby Home Assistant viděl data z Majordomu, musí se připojit ke stejnému MQTT brokeru:

1. Přejděte do **Nastavení → Zařízení a služby**.
2. Pokud používáte doplněk Mosquitto, Home Assistant broker sám objeví — stačí kliknout na **Konfigurovat** u nalezené integrace MQTT a potvrdit.
3. Jinak klikněte na **Přidat integraci**, vyhledejte **MQTT** a zadejte adresu brokeru, uživatelské jméno a heslo ručně.

!!! tip "Ověření komunikace"
    V integraci MQTT klikněte na **Konfigurovat → Naslouchat tématu** a zadejte `majordomus/#`. Pokud vše funguje, uvidíte proud zpráv z vašich jednotek.

---

## Auto-discovery — zařízení se objeví sama

Majordomus Control s povoleným `ha_discovery` ohlašuje všechna zařízení automaticky. V **Nastavení → Zařízení a služby → integrace MQTT** se objeví každá HW jednotka jako samostatné zařízení se všemi svými entitami:

- **RoomSensor** — teplota, vlhkost, CO₂, VOC, osvětlení, hluk, pohyb, tlačítka, stmívání světel
- **RoomIO / BoxIO** — digitální vstupy a výstupy, relé

Žádné YAML soubory, žádná ruční konfigurace entit. Přidáte-li později novou jednotku v Majordomus Control, v Home Assistantu se objeví sama.

---

## Dashboard a mobilní aplikace

Jakmile jsou entity v Home Assistantu, můžete je začít používat:

- **Dashboard** — v **Nastavení → Dashboardy** vytvořte přehled domu; entity Majordomus přidáváte jako jakékoliv jiné (karty s teplotou, grafy, přepínače světel...).
- **Mobilní aplikace** — nainstalujte [Home Assistant Companion](https://companionapp.home-assistant.io/) pro Android nebo iOS a ovládejte dům odkudkoliv.
- **Historie** — data ze všech senzorů se automaticky ukládají; grafy najdete u každé entity nebo v záložce **Historie**.

!!! tip "Automatizace: Home Assistant, nebo Node-RED?"
    Jednoduché automatizace (notifikace na mobil, rozsvícení při pohybu) zvládne Home Assistant sám v **Nastavení → Automatizace**. Kritickou logiku domu — topení, žaluzie, ventilaci — doporučujeme držet v [Node-RED](sw-node-red.md), který běží nezávisle na Home Assistantu.
