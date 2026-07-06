#!/bin/sh
# Baut das installierbare CCU-Addon-Paket asksinanalyzer-<version>.tar.gz
# (Installation ueber WebUI -> Systemsteuerung -> Zusatzsoftware)
set -e
cd "$(dirname "$0")"

VERSION=$(cat VERSION)
OUT="asksinanalyzer-${VERSION}.tar.gz"
STAGE=$(mktemp -d)
trap 'rm -rf "${STAGE}"' EXIT

# Paketlayout: update_script im Archiv-Root, Payload darunter
cp pkg/update_script "${STAGE}/"
mkdir -p "${STAGE}/rc.d"
cp pkg/rc.d/asksinanalyzer "${STAGE}/rc.d/"
mkdir -p "${STAGE}/asksinanalyzer/www"
cp VERSION "${STAGE}/asksinanalyzer/"
cp www/index.html www/api.cgi "${STAGE}/asksinanalyzer/www/"

chmod 755 "${STAGE}/update_script" "${STAGE}/rc.d/asksinanalyzer" \
          "${STAGE}/asksinanalyzer/www/api.cgi"

tar -czf "${OUT}" -C "${STAGE}" \
    --owner=root --group=root --numeric-owner \
    update_script rc.d asksinanalyzer

echo "erstellt: $(pwd)/${OUT}"
tar -tzf "${OUT}"
