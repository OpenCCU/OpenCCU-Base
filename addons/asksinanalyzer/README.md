# AskSinAnalyzer CCU

Port des [AskSinAnalyzerXS](https://github.com/psi-4ward/AskSinAnalyzerXS) direkt auf die CCU:
Logging und grafische Auswertung aller Homematic-Funktelegramme (BidCos RX/TX dekodiert
inkl. RSSI, HmIP als Rohdaten) — ohne zusätzlichen Arduino-Sniffer.

## Architektur

```
Funkmodul ⇄ multimacd (TrafficLogger)
                │  schreibt /var/log/multimacd-traffic-YYYY-MM-DD.log
                ▼
        www/api.cgi (tclsh, lighttpd)
                │  Offset-Tailing, Geräteliste (rfd-XML-RPC + ReGa)
                ▼
        www/index.html (Single-Page-UI, kein externes CDN)
```

- **Datenquelle** ist der `TrafficLogger` in `src/multimacd` (Konfig-Schalter
  `Traffic Log = 1`, `Traffic Log Directory = /var/log` in der multimacd.conf
  bzw. `/etc/config_templates/multimacd.conf`).
- **`www/api.cgi`** — Endpunkte:
  - `?cmd=dates` — verfügbare Log-Tage (JSON)
  - `?cmd=data&date=YYYY-MM-DD&offset=N` — neue Logzeilen ab Datei-Offset (Live-Polling)
  - `?cmd=devlist[&refresh=1]` — Geräteliste: RF-Adresse via rfd `listDevices`
    (Port 2001), Namen via ReGa; Cache `/tmp/asksinanalyzer.devlist.json` (600 s)
- **`www/index.html`** — Telegramm-Tabelle (live), Filter (Suche, Richtung,
  Protokoll, Typ, Gerät), Analyse-Tab (RSSI-Verlauf, Telegramm-Rate,
  Geräte-Statistik mit DutyCycle-Schätzung), CSV-Export.

## Installation auf der CCU

Voraussetzung: multimacd mit TrafficLogger (dieses Repo) und aktiviertem
`Traffic Log = 1` im Konfig-Template.

### Variante 1: Addon-Paket (WebUI)

```sh
./build-addon.sh        # erzeugt asksinanalyzer-<version>.tar.gz
```

Das Paket über die WebUI installieren: *Einstellungen → Systemsteuerung →
Zusatzsoftware → Datei auswählen → Installieren*. Das Addon erscheint danach
in der Zusatzsoftware-Liste (mit Uninstall und Link zur UI).
Paketinhalt: `update_script` (Installer), `rc.d/asksinanalyzer`
(WebUI-Integration: info/uninstall), `asksinanalyzer/` (Payload).

### Variante 2: manuell per SSH

```sh
CCU=root@ccu
ssh $CCU 'mkdir -p /usr/local/addons/asksinanalyzer/www'
scp www/index.html www/api.cgi $CCU:/usr/local/addons/asksinanalyzer/www/
ssh $CCU 'chmod 755 /usr/local/addons/asksinanalyzer /usr/local/addons/asksinanalyzer/www /usr/local/addons/asksinanalyzer/www/api.cgi
          ln -sfn /usr/local/addons/asksinanalyzer/www /usr/local/etc/config/addons/www/asksinanalyzer'
```

Aufruf: `http://<ccu>/addons/asksinanalyzer/`

## Hinweise

- `/var/log` ist tmpfs: Logs sind nach einem Reboot weg (~5–10 MB/Tag).
  Optionale Bereinigung alter Tageslogs per Cron:
  `10 0 * * * find /var/log -name "multimacd-traffic-*.log" -mtime +7 -delete`
- HmIP-Payloads sind AES-verschlüsselt und werden nur als Rohdaten angezeigt.
