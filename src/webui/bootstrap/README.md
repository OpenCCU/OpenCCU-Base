# WebUI Bootstrap assets

The WebUI loads [Bootstrap](https://getbootstrap.com/) 5 from
`src/webui/www/webui`:

| File | Source |
| --- | --- |
| `js/extern/bootstrap.bundle.min.js` (+ `.map`) | copied unmodified from the `bootstrap` npm package (includes Popper) |
| `css/extern/main.min.css` (+ `.map`) | compiled from `scss/main.scss`, which imports only the Bootstrap parts the WebUI needs and adds the WebUI colors |
| `css/extern/hmBootstrap.css` | hand-written; reverts Bootstrap defaults that conflict with the existing WebUI styles |

This directory holds the tooling to regenerate the first two. It is not
installed into the firmware image (only `src/webui/www` and
`src/webui/rega/www` are).

## Updating Bootstrap

1. Set the new versions in `package.json` (exact versions, no ranges).
   Use the Sass version the Bootstrap release itself is built with (see its
   `package.json`); newer Dart Sass releases emit many deprecation warnings
   for Bootstrap 5.3.
2. Run `./update.sh` (needs Node.js with npm and access to the npm registry).
   It installs the packages into `node_modules`, copies the bundle and
   recompiles `main.min.css` and its source map.
3. Check `scss/main.scss` against the release notes of the new version, for
   example new Sass maps that the WebUI colors have to extend.
4. Review the diff of the generated files and check the WebUI pages that use
   Bootstrap (for example the DutyCycle and CarrierSense bars).

`node_modules` and `package-lock.json` are not committed. With the versions
in `package.json` unchanged, `./update.sh` reproduces the committed files
byte for byte.
