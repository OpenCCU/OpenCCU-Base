#!/usr/bin/env node
// Tests for getLangInfo() and decodeLocalized() in
// src/webui/www/config/easymodes/js/ic_translate.js.
// Many easymode localization files store umlauts as %XX escapes
// ("T%FCr %F6ffnen"). The texts are inserted into the page through TrimPath
// and innerHTML, so getLangInfo() has to decode them when it loads them.
// Run with: node --test tests/webui/test-easymode-translate.js
'use strict';

const assert = require('node:assert/strict');
const fs = require('node:fs');
const path = require('node:path');
const test = require('node:test');
const vm = require('node:vm');

const WWW = path.join(__dirname, '..', '..', 'src', 'webui', 'www');
const SOURCE = path.join(WWW, 'config', 'easymodes', 'js', 'ic_translate.js');
const EASYMODES = path.join(WWW, 'config', 'easymodes');
const ESCAPE = /%[0-9A-Fa-f]{2}/;
// Tables which cannot be loaded for reasons unrelated to the decoding: the
// last entry of these files is not followed by a comma.
const BROKEN = new Set([
  'BLIND_VIRTUAL_RECEIVER/COND_SWITCH_TRANSMITTER_RAIN_QUANTITY.txt',
  'BLIND_VIRTUAL_RECEIVER/COND_SWITCH_TRANSMITTER_WIND_DIRECTION.txt',
]);

function loadTranslate() {
  const context = {
    getLang: () => 'de',
    langJSON: { de: {} },
    failures: [],
    // Synchronous replacement of Prototype's Ajax.Request, which reads the
    // files from the source tree. The WebUI serves them as ISO-8859-1.
    Ajax: {
      Request: function (url, options) {
        const file = path.join(WWW, url);
        if (fs.existsSync(file)) {
          options.onSuccess({ responseText: fs.readFileSync(file, 'latin1') });
        } else {
          options.onFailure({ statusText: 'Not Found' });
        }
      },
    },
    MessageBox: { show: (title, msg) => context.failures.push(msg) },
  };
  vm.runInNewContext(fs.readFileSync(SOURCE, 'latin1'), context);
  return context;
}

const T = loadTranslate();

function decoded(value) {
  const loc = [{ key: value }];
  T.decodeLocalized(loc);
  return loc[0].key;
}

test('decodes %XX escapes', () => {
  assert.equal(decoded('T%FCr %F6ffnen'), 'Tür öffnen');
  assert.equal(decoded('%C4%D6%DC%E4%F6%FC%DF'), 'ÄÖÜäöüß');
  assert.equal(decoded('unterst%fctzt'), 'unterstützt');
});

test('leaves literal percent signs unchanged', () => {
  assert.equal(decoded('100%'), '100%');
  assert.equal(decoded('0% - 100%, %zz'), '0% - 100%, %zz');
  assert.equal(decoded('%u00FC'), '%u00FC');
});

test('leaves HTML and entities unchanged', () => {
  const html = '<span class="translated">bet&auml;tigt</span><br/>';
  assert.equal(decoded(html), html);
});

test('decodes only once', () => {
  assert.equal(decoded('%2541'), '%41');
});

test('ignores values which are not strings and missing tables', () => {
  const loc = [{ n: 1, o: { a: '%FC' }, s: '%FC' }];
  T.decodeLocalized(loc);
  assert.equal(loc[0].n, 1);
  assert.deepEqual(loc[0].o, { a: '%FC' });
  assert.equal(loc[0].s, 'ü');
  T.decodeLocalized(undefined);
  T.decodeLocalized({});
  T.decodeLocalized([]);
});

test('getLangInfo() decodes the texts of the door lock profiles', () => {
  T.getLangInfo('ACCESS_TRANSCEIVER', 'DOOR_LOCK_TRANSCEIVER');
  assert.deepEqual(T.failures, []);
  assert.equal(T.localized[0].switch_open, 'Tür öffnen');
  assert.equal(T.localized[0].switch_unlock, 'Türschloss entriegeln');
  assert.match(T.localized[0].description_2, /wird die Tür geöffnet\./);
  assert.equal(T.localized[0].UP_TIME_DELAY_FACTOR_DESCR,
    '<span class="translated">Hochfahrverzögerungszeit</span>');
});

test('getLangInfo() leaves no %XX escapes in any easymode table', () => {
  const checked = [];
  for (const actor of fs.readdirSync(EASYMODES)) {
    const dir = path.join(EASYMODES, actor, 'localization', 'de');
    if (actor === 'etc' || !fs.existsSync(path.join(dir, 'GENERIC.txt'))) continue;
    const genericEscaped = ESCAPE.test(fs.readFileSync(path.join(dir, 'GENERIC.txt'), 'latin1'));
    for (const file of fs.readdirSync(dir)) {
      if (!file.endsWith('.txt') || file === 'GENERIC.txt') continue;
      if (BROKEN.has(`${actor}/${file}`)) continue;
      const sender = file.slice(0, -4);
      if (!genericEscaped && !ESCAPE.test(fs.readFileSync(path.join(dir, file), 'latin1'))) continue;
      T.failures.length = 0;
      T.getLangInfo(sender, actor);
      assert.deepEqual(T.failures, [], `${actor}/${file}`);
      for (const [key, value] of Object.entries(T.localized[0])) {
        if (typeof value === 'string') {
          assert.doesNotMatch(value, ESCAPE, `${actor}/${file}: ${key}`);
        }
      }
      checked.push(`${actor}/${file}`);
    }
  }
  assert.ok(checked.length > 50, `only ${checked.length} tables checked`);
});
