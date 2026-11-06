#!/usr/bin/env node
// Tests for translateString() in src/webui/www/webui/js/lang/translate.js.
// translateString() replaces ${key} placeholders with their translation. The
// string can contain user texts like names, so only known keys may be
// %-decoded and a translation must be inserted literally.
// Run with: node --test tests/webui/test-translate.js
'use strict';

const assert = require('node:assert/strict');
const fs = require('node:fs');
const path = require('node:path');
const test = require('node:test');
const vm = require('node:vm');

const SOURCE = path.join(__dirname, '..', '..', 'src', 'webui', 'www', 'webui',
  'js', 'lang', 'translate.js');

function loadTranslate() {
  const context = {
    unescape: globalThis.unescape,
    getLang: () => 'de',
    conInfo: () => {},
    loadTextResource: () => {},
    HMIdentifier: { de: { x: 1 } },
    langJSON: { de: { lblX: 'Gr%FC%DFe', lblDollar: 'a $& b' } },
    jQuery: { each: (arr, fn) => arr.forEach((v, i) => fn(i, v)) },
  };
  vm.runInNewContext(fs.readFileSync(SOURCE, 'utf8'), context);
  return context;
}

const T = loadTranslate();

test('translates and decodes known keys', () => {
  assert.equal(T.translateString('${lblX}!'), 'Grüße!');
});

test('leaves text around the placeholders unchanged', () => {
  assert.equal(T.translateString('${lblX} %3Cb%3E'), 'Grüße %3Cb%3E');
});

test('does not decode unknown keys', () => {
  assert.equal(T.translateString('${unknownKey}'), 'unknownKey');
  assert.equal(T.translateString('${%3Cimg/src=x/onerror=alert(1)%3E}'),
    '%3Cimg/src=x/onerror=alert(1)%3E');
  assert.equal(T.translateString('${%24%7BlblX%7D}%3Cb%3Ea'), '%24%7BlblX%7D%3Cb%3Ea');
});

test('inserts translations literally', () => {
  assert.equal(T.translateString('${lblDollar}'), 'a $& b');
});

test('returns strings without placeholders unchanged', () => {
  assert.equal(T.translateString('O\'Brien <b>&amp;</b> 100%'), 'O\'Brien <b>&amp;</b> 100%');
});
