#!/usr/bin/env node
// Tests for translateSessionInvalid() in src/webui/www/webui/js/lang/translate.js.
// ReGaHss marks its error page for an invalid or expired session, which the
// WebUI loads into #content, with the id "sessionInvalid", and its message and
// link with the keys of their translations as name. The WebUI replaces them by
// the texts of its language (translateJSTemplate()), the link keeps its href.
// Run with: node --test tests/webui/test-session-invalid.js
'use strict';

const assert = require('node:assert/strict');
const fs = require('node:fs');
const path = require('node:path');
const test = require('node:test');
const vm = require('node:vm');

const SOURCE = path.join(__dirname, '..', '..', 'src', 'webui', 'www', 'webui',
  'js', 'lang', 'translate.js');

// A minimal element with a name attribute (span or link).
function element(tagName, name, text, attributes = {}) {
  return {
    tagName,
    textContent: text,
    innerHTML: text,
    attributes: { name, ...attributes },
    getAttribute(key) { return this.attributes[key]; },
    setAttribute(key, value) { this.attributes[key] = value; },
  };
}

// Loads translate.js with a stub of jQuery for the given elements of the page
// (the elements with a name attribute within #sessionInvalid) and language.
function load(page, lang) {
  const selected = [];
  const jQuery = (selector) => {
    selected.push(selector);
    if (selector === '#sessionInvalid') {
      return { length: page === null ? 0 : 1 };
    }
    if (selector === '#sessionInvalid *[name]') {
      return page || [];
    }
    return [];
  };
  jQuery.each = (arr, fn) => arr.forEach((v, i) => fn.call(v, i, v));
  const context = {
    unescape: globalThis.unescape,
    getLang: () => lang,
    conInfo: () => {},
    loadTextResource: () => {},
    HMIdentifier: { de: { x: 1 }, en: { x: 1 } },
    langJSON: {
      de: { sessionInvalid: 'Die Sitzung ist ung%FCltig oder abgelaufen!', sessionInvalidLogin: 'Neu anmelden' },
      en: { sessionInvalid: 'The session is invalid or expired', sessionInvalidLogin: 'Log in again' },
    },
    jQuery,
  };
  vm.runInNewContext(fs.readFileSync(SOURCE, 'utf8'), context);
  return { context, selected };
}

// The error page of ReGaHss: message and link with the German texts.
function sessionInvalidPage() {
  return [
    element('SPAN', 'sessionInvalid', 'Die Sitzung ist ungültig oder abgelaufen!\r\n'),
    element('A', 'sessionInvalidLogin', 'Neu anmelden', { href: '/index.htm' }),
  ];
}

test('translates the message and the link into English', () => {
  const page = sessionInvalidPage();
  load(page, 'en').context.translateSessionInvalid();
  assert.equal(page[0].textContent, 'The session is invalid or expired');
  assert.equal(page[1].textContent, 'Log in again');
  assert.equal(page[1].getAttribute('href'), '/index.htm');
});

test('translates the message and the link into German (decoded)', () => {
  const page = sessionInvalidPage();
  load(page, 'de').context.translateSessionInvalid();
  assert.equal(page[0].textContent, 'Die Sitzung ist ungültig oder abgelaufen!');
  assert.equal(page[1].textContent, 'Neu anmelden');
});

test('does nothing without the error page', () => {
  const { context, selected } = load(null, 'en');
  context.translateSessionInvalid();
  assert.deepEqual(selected, ['#sessionInvalid']);
});
