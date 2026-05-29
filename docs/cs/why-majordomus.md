# Proč Majordomus?

## Srovnání s dostupnými řešeními
 
Než se pustíte do výběru chytré domácnosti, je dobré vědět, jaké máte možnosti — a kde má každá z nich své limity.
 
---
 
## Komerční systémy (KNX, Loxone, ...)
 
<!-- TODO: Doplnit konkrétní cenové srovnání -->
<!-- TODO: Doplnit příklady vendor lock-in situací -->
 
**Co nabízí:** Profesionální instalace, certifikace, zákaznická podpora, hotové ekosystémy.
 
**Kde narazíte:**
 
- **Cena.** <!-- TODO: Konkrétní příklad — kolik stojí pokrytí jedné místnosti u Loxone vs Majordomus? -->
- **Uzavřenost.** Jste závislí na ekosystému jednoho výrobce. Chcete přidat senzor CO2 od jiné firmy? Smůla.
- **Životnost a dostupnost.** Co až výrobce přestane vyrábět váš model? Co až ukončí cloudovou službu?
- **Množství zařízení.** Pro plné pokrytí místnosti potřebujete často několik samostatných komponent.
---
 
## Bastlířská řešení (Arduino, ESP32, Zigbee, Tasmota...)

**Co nabízí:** Nízká cena, obrovská komunita, neomezená flexibilita.

**Kde narazíte:**

- **Fragmentace.** Každý senzor je samostatný projekt. CO2 od jednoho autora, relé od druhého, teploměr od třetího.
- **Spolehlivost.** Wi-Fi senzory na baterie, výpadky sítě, ztráta spojení.
- **Vzhled a kompaktnost.** Hromada krabiček na zdech, žádný jednotný design.
- **Údržba.** Kdo to bude opravovat za pět let? Dokumentace zastaralá, autor přestal projekt udržovat.

---

## Levná asijská řešení (Waveshare, Tuya, Zigbee senzory, ...)

**Co nabízí:** Nejnižší pořizovací cena, okamžitá dostupnost, velký výběr hotových produktů.

**Kde narazíte:**

- **Kvalita komponent.** Levné součástky znamenají kratší životnost. Senzor, který za rok odchází, je nakonec dražší než kvalitní řešení.
- **Neudržitelnost.** Výrobce přestane model vyrábět, firmware se přestane aktualizovat, cloudová služba se vypne. Investice do integrace přijde nazmar.
- **Fragmentace a nekompatibilita.** Každý výrobce má své vlastní API, cloud, aplikaci. Integrace do jednotného systému vyžaduje obcházení a hacky.
- **Bezpečnost.** Levná zařízení s čínským cloudem — vaše data o pohybu v domě, přítomnosti, spotřebě.
- **Skalování.** Při větším počtu zón se správa desítek různých zařízení od různých výrobců stává noční můrou.

!!! note "Majordomus vs. levná asijská řešení"
    Majordomus s levnými asijskými výrobci cenově přímo nesoutěží — a ani to není cílem. Nabízí promyšlený, modulární systém s kvalitními komponenty, otevřeným hardwarem i softwarem a dlouhodobou udržitelností. Je to investice do infrastruktury domu — stejně jako elektroinstalace nebo rozvod vody.

---
 
## Majordomus — to nejlepší z obou světů

| | KNX / Loxone | Levná asijská řešení | Bastlířské projekty | Majordomus |
|---|---|---|---|---|
| Otevřenost | Uzavřený ekosystém | Částečně (cloud závislost) | Plně otevřené | Plně open-source |
| Spolehlivost | Vysoká (drát) | Nízká (Wi-Fi, baterie) | Nízká (Wi-Fi/baterie) | Vysoká (drát, RS485) |
| Pokrytí místnosti | Více krabiček | Více krabiček | Více krabiček | 1 jednotka |
| Kvalita komponent | Vysoká | Nízká | Různá | Vysoká |
| Údržba za 10 let | Závislost na výrobci | Nejistá (EOL produkty) | Nejistá | Otevřený HW i SW |
| Integrace | Omezená | Různorodá, nestabilní | Různorodá | MQTT, Node-RED, HA |
| Profesionální vzhled | Ano | Ne | Ne | Ano |
| Provoz bez cloudu | Ano | Ne | Ano | Ano |

