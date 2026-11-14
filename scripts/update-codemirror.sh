#!/bin/bash
# Builds the CodeMirror 6 bundle of the WebUI script editor
# (src/webui/www/webui/js/extern/codemirror6) from scripts/codemirror6 with the
# latest npm releases of CodeMirror. The bundle is only rebuilt when one of the
# bundled packages has a new version, or when FORCE=1 is set (e.g. after a
# change of scripts/codemirror6).
set -euo pipefail

SCRIPT_DIR=$(cd "$(dirname "$0")" && pwd -P)
REPO_ROOT=$(cd "${SCRIPT_DIR}/.." && pwd -P)
DEST_DIR="${REPO_ROOT}/src/webui/www/webui/js/extern/codemirror6"
VERSION_FILE="${DEST_DIR}/VERSION"
ENTRY_FILE="${REPO_ROOT}/scripts/codemirror6/entry.js"
CSS_FILE="${REPO_ROOT}/scripts/codemirror6/codemirror6.css"
FORCE="${FORCE:-0}"

TMP_DIR=$(mktemp -d)
trap 'rm -rf -- "${TMP_DIR}"' EXIT

pushd "${TMP_DIR}" >/dev/null
npm init -y >/dev/null
npm install --silent --no-package-lock \
  codemirror@latest \
  @codemirror/autocomplete@latest \
  @codemirror/commands@latest \
  @codemirror/language@latest \
  @codemirror/legacy-modes@latest \
  @codemirror/search@latest \
  @codemirror/state@latest \
  @codemirror/view@latest \
  @lezer/highlight@latest \
  esbuild@latest >/dev/null

# Versions of all installed packages that end up in the bundle (everything
# except the esbuild build tool), one "PACKAGE <name> = <version>" line each.
# shellcheck disable=SC2016 # JavaScript template literals, not shell
node -e '
  const fs = require("fs");
  const path = require("path");
  const out = [];
  const add = (dir, name) => {
    const pkg = JSON.parse(fs.readFileSync(path.join(dir, "package.json"), "utf8"));
    out.push(`PACKAGE ${name} = ${pkg.version}`);
  };
  for (const entry of fs.readdirSync("node_modules")) {
    if (entry.startsWith(".") || entry === "esbuild" || entry === "@esbuild") {
      continue;
    }
    if (entry.startsWith("@")) {
      for (const sub of fs.readdirSync(path.join("node_modules", entry))) {
        add(path.join("node_modules", entry, sub), `${entry}/${sub}`);
      }
    } else {
      add(path.join("node_modules", entry), entry);
    }
  }
  console.log(out.sort().join("\n"));
' >"${TMP_DIR}/packages.txt"

NEW_VERSION=$(node -p 'require("./node_modules/codemirror/package.json").version')
CURRENT_PACKAGES=$(grep '^PACKAGE ' "${VERSION_FILE}" 2>/dev/null || true)

if [[ "${FORCE}" != "1" && "${CURRENT_PACKAGES}" == "$(cat "${TMP_DIR}/packages.txt")" ]]; then
  echo "codemirror6: the bundled packages are already current, no update required"
  exit 0
fi

cp -f "${ENTRY_FILE}" "${TMP_DIR}/entry.js"
npx esbuild "${TMP_DIR}/entry.js" \
  --bundle \
  --format=iife \
  --platform=browser \
  --target=es2018 \
  --minify \
  --outfile="${TMP_DIR}/codemirror6.bundle.js" \
  --log-level=error

mkdir -p "${DEST_DIR}"
cp -f "${TMP_DIR}/codemirror6.bundle.js" "${DEST_DIR}/codemirror6.bundle.js"
cp -f "${CSS_FILE}" "${DEST_DIR}/codemirror6.bundle.css"

{
  echo "CODEMIRROR6_VERSION = ${NEW_VERSION}"
  echo "BUNDLED_AT = $(date -u +%Y-%m-%dT%H:%M:%SZ)"
  echo "ESBUILD_VERSION = $(node -p 'require("./node_modules/esbuild/package.json").version')"
  cat "${TMP_DIR}/packages.txt"
} >"${VERSION_FILE}"

popd >/dev/null

echo "codemirror6: bundle rebuilt (codemirror ${NEW_VERSION})"
