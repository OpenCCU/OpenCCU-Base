#!/usr/bin/env bash
#
# Regenerates the Bootstrap assets of the WebUI from the versions pinned in
# package.json:
#
#   www/webui/js/extern/bootstrap.bundle.min.js(.map)  copied from bootstrap
#   www/webui/css/extern/main.min.css(.map)            compiled from
#                                                      www/webui/scss/main.scss
#
# Requires Node.js with npm and network access to the npm registry.

set -o errexit
set -o nounset
set -o pipefail

tooling=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)
webui="${tooling}/../www/webui"
temporary=$(mktemp -d)
trap 'rm -rf "$temporary"' EXIT

cd "$tooling"
npm install --no-audit --no-fund

cp node_modules/bootstrap/dist/js/bootstrap.bundle.min.js \
   node_modules/bootstrap/dist/js/bootstrap.bundle.min.js.map \
   "${webui}/js/extern/"

# main.scss imports Bootstrap from ../node_modules, and the source map refers
# to the sources relative to css/extern. Compile in a copy of the webui layout
# with node_modules next to scss/ so that both stay unchanged.
mkdir -p "${temporary}/webui/scss" "${temporary}/webui/css/extern"
cp "${webui}/scss/main.scss" "${temporary}/webui/scss/"
ln -s "${tooling}/node_modules" "${temporary}/webui/node_modules"
(cd "${temporary}/webui" && \
  "${tooling}/node_modules/.bin/sass" --style=compressed \
    scss/main.scss css/extern/main.min.css)
cp "${temporary}/webui/css/extern/main.min.css" \
   "${temporary}/webui/css/extern/main.min.css.map" \
   "${webui}/css/extern/"

echo "bootstrap $(node -p 'require("./node_modules/bootstrap/package.json").version')," \
     "sass $(node_modules/.bin/sass --version)"
