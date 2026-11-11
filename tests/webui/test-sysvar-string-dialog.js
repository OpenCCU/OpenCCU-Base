#!/usr/bin/env node
// Tests for the text field of a string system variable in the system variable
// dialog (loadChar8859Edit() in
// src/webui/rega/www/pages/tabs/admin/msg/newSysVar.htm).
//
// The page writes the stored value with ReGa's UriEncode(), the dialog decodes
// it into the text field, and saving sends the field through ReGa.encode() to
// ReGa, which decodes the request body with uriDecode(). Opening the dialog
// and saving it unchanged must give back the stored value.
//
// uriEncode() and uriDecode() below follow ise_string::uriEncode() and
// ise_string::uriDecode() of ReGaHss (Platform/Common/ise_ntl.cpp). Stored
// values are ISO-8859 bytes, written here as characters U+0000 to U+00FF.
// Run with: node --test tests/webui/test-sysvar-string-dialog.js
'use strict';

const assert = require('node:assert/strict');
const fs = require('node:fs');
const path = require('node:path');
const test = require('node:test');
const vm = require('node:vm');

const ROOT = path.join(__dirname, '..', '..', 'src', 'webui');
const DIALOG = path.join(ROOT, 'rega', 'www', 'pages', 'tabs', 'admin', 'msg', 'newSysVar.htm');
const REGA = path.join(ROOT, 'www_source', 'ise', 'js', 'rega.js');

// Returns the source of "name = function(...) { ... };" from the dialog page.
function extractFunction(source, name) {
  const start = source.indexOf('  ' + name + ' = function');
  assert.notEqual(start, -1, `${name} not found`);
  const end = source.indexOf('\n  };', start);
  assert.notEqual(end, -1, `end of ${name} not found`);
  return source.slice(start, end + 5);
}

function loadDialog() {
  const source = fs.readFileSync(DIALOG, 'latin1');
  const elements = {};
  const context = {
    $: (id) => (elements[id] = elements[id] || {}),
    escape: globalThis.escape,
    unescape: globalThis.unescape,
  };
  vm.runInNewContext(extractFunction(source, 'loadChar8859Edit') + '\n' +
    extractFunction(source, 'svEscapeRegaString') + '\n' +
    fs.readFileSync(REGA, 'latin1'), context);
  return { context, elements };
}

const { context, elements } = loadDialog();

// ReGa's UriEncode(): only 0-9, A-Z and a-z are kept, every other byte becomes %XX.
function uriEncode(value) {
  let out = '';
  for (const ch of value) {
    const code = ch.charCodeAt(0);
    assert.ok(code < 256, 'stored values are bytes');
    out += /[0-9A-Za-z]/.test(ch) ? ch : '%' + code.toString(16).toUpperCase().padStart(2, '0');
  }
  return out;
}

// ReGa's uriDecode(): replaces %XX with two hex digits, leaves anything else.
function uriDecode(value) {
  return value.replace(/%([0-9A-Fa-f]{2})/g, (m, hex) => String.fromCharCode(parseInt(hex, 16)));
}

// Text shown in the field when the dialog opens for a stored value.
function openDialog(stored) {
  delete elements.txValue;
  context.loadChar8859Edit(uriEncode(stored));
  return elements.txValue.value;
}

// Value ReGa stores when the dialog is saved with the given field text: the
// request body is ReGa.encode() of "string sValue = '...';", which ReGa
// decodes with uriDecode() and then parses as a single-quoted literal.
function save(text) {
  const body = context.ReGa.encode("string sValue = '" + context.svEscapeRegaString(text) + "';");
  const prefix = '<prototypejs><![CDATA[';
  assert.ok(body.startsWith(prefix));
  const script = uriDecode(body.slice(prefix.length, body.indexOf(']')));
  const match = /^string sValue = '((?:[^'\\]|\\.)*)';$/s.exec(script);
  assert.ok(match, `unexpected script ${JSON.stringify(script)}`);
  return match[1].replace(/\\(.)/g, '$1');
}

const VALUES = [
  'plain text',
  'A%41 B%20C',
  '100%',
  'discount %99',
  '%C3%A4 and %E4',
  '%25',
  'x+y=z & a;b',
  'Gr\u00fc\u00dfe \u00e4\u00f6\u00fc \u00c4\u00d6\u00dc',
  '5 \u00a4 (byte 0xA4, the euro sign in ISO-8859-15)',
  'C:\\temp\\ and \'quoted\'',
  '<b>bold</b> </script>',
  'line 1\nline 2',
  '',
];

test('the text field shows the stored value unchanged', () => {
  for (const value of VALUES) {
    assert.equal(openDialog(value), value);
  }
});

test('saving the dialog unchanged keeps the stored value', () => {
  for (const value of VALUES) {
    assert.equal(save(openDialog(value)), value);
  }
});

test('percent sequences in the value are not decoded a second time', () => {
  assert.equal(openDialog('A%41'), 'A%41');
  assert.equal(openDialog('%20'), '%20');
  // decodeURIComponent() throws for %99, which left the field empty
  assert.doesNotThrow(() => openDialog('%99'));
  assert.equal(openDialog('%99'), '%99');
});
