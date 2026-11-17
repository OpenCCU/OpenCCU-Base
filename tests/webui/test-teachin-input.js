#!/usr/bin/env node
// Tests for the input of the teach-in dialog in
// src/webui/www/config/cp_add_device.cgi: SGTIN and KEY of the local HmIP
// teach-in (getHmIPTeachInSgtin(), getHmIPTeachInKey()), the serial number of
// the BidCos-RF teach-in (getBidCosTeachInSerial()) and the input filter
// (filterTeachInInput() with grouping like on the label and the QR code
// content filling both HmIP fields). Values copied from a label or an OCR scan may contain
// hyphens, blanks or line breaks; a value of wrong length or with wrong
// characters must be rejected (null) instead of being converted to a wrong key
// or sent to the device. The KEY in Base32 is converted by
// convertHmIPKeyBase32ToBase16() of src/webui/www_source/config/js/ic_common.js.
// Run with: node --test tests/webui/test-teachin-input.js
'use strict';

const assert = require('node:assert/strict');
const fs = require('node:fs');
const path = require('node:path');
const test = require('node:test');
const vm = require('node:vm');

const WEBUI = path.join(__dirname, '..', '..', 'src', 'webui');
const DIALOG = path.join(WEBUI, 'www', 'config', 'cp_add_device.cgi');
const IC_COMMON = path.join(WEBUI, 'www_source', 'config', 'js', 'ic_common.js');

// Extracts the code from startMarker to the end of the function that starts
// with endMarker; closing is the line that ends that function. Both files are
// Latin-1, the CGI has CRLF.
function extract(file, startMarker, endMarker, closing) {
  const text = fs.readFileSync(file, 'latin1').replace(/\r\n/g, '\n');
  const start = text.indexOf(startMarker);
  const end = text.indexOf(endMarker, start);
  assert.ok(start >= 0 && end >= start, startMarker + ' not found in ' + file);
  const close = text.indexOf('\n' + closing + '\n', end);
  assert.ok(close > end, 'end of ' + endMarker + ' not found in ' + file);
  return text.slice(start, close + closing.length + 1);
}

// Only these parts are needed; the rest of both files needs the browser and Tcl.
function loadFunctions() {
  const context = { document: { elements: {}, getElementById(id) { return this.elements[id]; } } };
  vm.runInNewContext(
    extract(IC_COMMON, 'convertHmIPKeyBase32ToBase16 = function',
      'convertHmIPKeyBase32ToBase16 = function', '};') + '\n' +
    extract(DIALOG, 'HMIP_SGTIN_LENGTH = 24;', 'getHmIPTeachInKey = function', '      };'),
    context);
  return context;
}

const f = loadFunctions();
const BASE32 = '0123456789ABCEFGHJKLMNPQRSTUWXYZ';

// Encodes 32 hex digits as 26 digits of the eQ-3 Base32 alphabet.
function toBase32(hex) {
  let n = BigInt('0x' + hex);
  let s = '';
  for (let i = 0; i < 26; i++) { s = BASE32[Number(n & 31n)] + s; n >>= 5n; }
  return s;
}

const SGTIN = '3014F711A0001234567890AB';
const KEYS = ['00112233445566778899AABBCCDDEEFF', 'FFEEDDCCBBAA99887766554433221100',
  '0123456789ABCDEF0123456789ABCDEF'];

test('SGTIN with hyphens, blanks, line breaks and lower case is accepted', () => {
  assert.equal(f.getHmIPTeachInSgtin(SGTIN), SGTIN);
  assert.equal(f.getHmIPTeachInSgtin('3014-F711-A000-1234-5678-90AB'), SGTIN);
  assert.equal(f.getHmIPTeachInSgtin(' 3014 F711 A000\n1234 5678 90ab\t'), SGTIN);
});

test('SGTIN of wrong length or with non-hex characters is rejected', () => {
  assert.equal(f.getHmIPTeachInSgtin(''), null);
  assert.equal(f.getHmIPTeachInSgtin(SGTIN.slice(0, 23)), null);
  assert.equal(f.getHmIPTeachInSgtin(SGTIN + '0'), null);
  assert.equal(f.getHmIPTeachInSgtin(SGTIN.slice(0, 23) + 'G'), null);
});

test('hex KEY is accepted with separators', () => {
  for (const key of KEYS) {
    assert.equal(f.getHmIPTeachInKey(key), key);
    assert.equal(f.getHmIPTeachInKey(key.toLowerCase().replace(/(.{4})/g, '$1 ')), key);
  }
});

test('Base32 KEY of the label is converted, also with blanks and hyphens', () => {
  for (const key of KEYS) {
    const b32 = toBase32(key);
    assert.equal(f.getHmIPTeachInKey(b32), key);
    assert.equal(f.getHmIPTeachInKey(b32.replace(/(.{5})/g, '$1-')), key);
    assert.equal(f.getHmIPTeachInKey(' ' + b32.slice(0, 5) + ' ' + b32.slice(5) + '\n'), key);
  }
});

test('KEY of wrong length or with wrong characters is rejected', () => {
  const b32 = toBase32(KEYS[0]);
  for (const value of ['', b32.slice(0, 25), b32 + '0', KEYS[0].slice(0, 31), KEYS[0] + '0',
    KEYS[0].slice(0, 31) + 'G', 'O' + b32.slice(1), b32.slice(0, 25) + 'D',
    '8' + b32.slice(1)]) {
    assert.equal(f.getHmIPTeachInKey(value), null, JSON.stringify(value));
  }
});

test('BidCos serial number with blanks, hyphens and lower case is accepted', () => {
  assert.equal(f.getBidCosTeachInSerial('MEQ0123456'), 'MEQ0123456');
  assert.equal(f.getBidCosTeachInSerial(' meq0123456 '), 'MEQ0123456');
  assert.equal(f.getBidCosTeachInSerial('MEQ 012-3456\n'), 'MEQ0123456');
});

test('BidCos serial number of wrong length is rejected', () => {
  for (const value of ['', 'MEQ012345', 'MEQ01234567', '   ']) {
    assert.equal(f.getBidCosTeachInSerial(value), null, JSON.stringify(value));
  }
});

// Minimal input element: id, value and caret, as filterTeachInInput() uses it.
function fakeInput(value, caret, id) {
  return {
    id: id || 'input',
    value: value,
    selectionStart: caret === undefined ? value.length : caret,
    setSelectionRange(start) { this.selectionStart = start; }
  };
}

// Types or pastes: sets value and caret as the browser does, then the handler.
function input(field, element, value, caret, inputType) {
  element.value = value;
  element.selectionStart = caret === undefined ? value.length : caret;
  f.filterTeachInInput(element, field, { inputType: inputType || 'insertText' });
  return element;
}

test('input filter removes invalid characters, converts to upper case and groups', () => {
  const sgtin = input(f.TEACHIN_FIELD_HMIP_SGTIN, fakeInput(''), '3014 f711 a0g0');
  assert.equal(sgtin.value, '3014-F711-A00');

  const key = input(f.TEACHIN_FIELD_HMIP_KEY, fakeInput(''), '0d-v9z');
  assert.equal(key.value, '0D9Z');

  const serial = input(f.TEACHIN_FIELD_BIDCOS_SERIAL, fakeInput(''), 'meq 012-345ß6');
  assert.equal(serial.value, 'MEQ0123456');
});

test('values are grouped like on the label, a hex KEY in groups of 4', () => {
  const sgtin = input(f.TEACHIN_FIELD_HMIP_SGTIN, fakeInput(''), '0123456789ABCDEF01234567');
  assert.equal(sgtin.value, '0123-4567-89AB-CDEF-0123-4567');
  assert.equal(f.getHmIPTeachInSgtin(sgtin.value), '0123456789ABCDEF01234567');

  const b32 = toBase32(KEYS[0]);
  const keyB32 = input(f.TEACHIN_FIELD_HMIP_KEY, fakeInput(''), b32);
  assert.equal(keyB32.value, b32.replace(/^(.{5})(.{5})(.{5})(.{5})/, '$1-$2-$3-$4-'));
  assert.equal(f.getHmIPTeachInKey(keyB32.value), KEYS[0]);

  const keyHex = input(f.TEACHIN_FIELD_HMIP_KEY, fakeInput(''), KEYS[0]);
  assert.equal(keyHex.value, KEYS[0].replace(/(.{4})(?!$)/g, '$1-'));
  assert.equal(f.getHmIPTeachInKey(keyHex.value), KEYS[0]);

  // no separator behind a complete group at the end, too long stays in the last group
  assert.equal(input(f.TEACHIN_FIELD_HMIP_SGTIN, fakeInput(''), '01234567').value, '0123-4567');
  assert.equal(input(f.TEACHIN_FIELD_HMIP_SGTIN, fakeInput(''), '0123456789ABCDEF0123456789').value,
    '0123-4567-89AB-CDEF-0123-456789');
});

test('pasted values with separators of the label are taken over', () => {
  const sgtin = input(f.TEACHIN_FIELD_HMIP_SGTIN, fakeInput(''), ' 0123-4567-89ab\n-CDEF-0123-4567 ', undefined, 'insertFromPaste');
  assert.equal(sgtin.value, '0123-4567-89AB-CDEF-0123-4567');
});

test('input filter turns O and I into 0 and 1 only where they are invalid', () => {
  assert.equal(input(f.TEACHIN_FIELD_HMIP_SGTIN, fakeInput(''), '3o14-fi11').value, '3014-F111');
  assert.equal(input(f.TEACHIN_FIELD_HMIP_KEY, fakeInput(''), '1O2i-3D').value, '10213-D');
  // O and I are valid in a BidCos serial number and stay letters
  assert.equal(input(f.TEACHIN_FIELD_BIDCOS_SERIAL, fakeInput(''), 'oeqi123456').value, 'OEQI123456');
});

test('caret stays behind the same character when separators move', () => {
  // "X" typed between "012" and "3-4567": the following groups shift
  const sgtin = input(f.TEACHIN_FIELD_HMIP_SGTIN, fakeInput(''), '0123-4567');
  input(f.TEACHIN_FIELD_HMIP_SGTIN, sgtin, '012A3-4567', 4);
  assert.equal(sgtin.value, '012A-3456-7');
  assert.equal(sgtin.selectionStart, 4);

  // invalid "-" typed in the middle of a serial number
  const serial = input(f.TEACHIN_FIELD_BIDCOS_SERIAL, fakeInput(''), 'MEQ0123456');
  input(f.TEACHIN_FIELD_BIDCOS_SERIAL, serial, 'MEQ01-23456', 6);
  assert.equal(serial.value, 'MEQ0123456');
  assert.equal(serial.selectionStart, 5);

  // valid value stays untouched, caret too
  const plain = input(f.TEACHIN_FIELD_HMIP_SGTIN, fakeInput(''), '3014-F7');
  input(f.TEACHIN_FIELD_HMIP_SGTIN, plain, '3014-F7', 2);
  assert.equal(plain.value, '3014-F7');
  assert.equal(plain.selectionStart, 2);
});

test('deleting a separator deletes the character next to it', () => {
  // Backspace behind the separator of "0123-|4567"
  const back = input(f.TEACHIN_FIELD_HMIP_SGTIN, fakeInput(''), '01234567');
  input(f.TEACHIN_FIELD_HMIP_SGTIN, back, '01234567', 4, 'deleteContentBackward');
  assert.equal(back.value, '0124-567');
  assert.equal(back.selectionStart, 3);

  // Delete in front of the separator of "0123|-4567"
  const fwd = input(f.TEACHIN_FIELD_HMIP_SGTIN, fakeInput(''), '01234567');
  input(f.TEACHIN_FIELD_HMIP_SGTIN, fwd, '01234567', 4, 'deleteContentForward');
  assert.equal(fwd.value, '0123-567');
  assert.equal(fwd.selectionStart, 4);

  // Backspace on a character works as usual
  const normal = input(f.TEACHIN_FIELD_HMIP_SGTIN, fakeInput(''), '01234567');
  input(f.TEACHIN_FIELD_HMIP_SGTIN, normal, '0123-567', 5, 'deleteContentBackward');
  assert.equal(normal.value, '0123-567');
  assert.equal(normal.selectionStart, 5);
});

test('QR code content pasted into the SGTIN or KEY field fills both', () => {
  // invented values, structure of the QR code of a HmIP label
  const SG = '0123456789ABCDEF01234567';
  const QR = 'EQ01SG' + SG + 'DLK' + KEYS[1];
  for (const target of ['sgtin', 'key']) {
    const sgtin = fakeInput('', 0, 'serialHmIPLocal_HmIP-RF');
    const key = fakeInput('', 0, 'keyHmIPLocal_HmIP-RF');
    f.document.elements = { 'serialHmIPLocal_HmIP-RF': sgtin, 'keyHmIPLocal_HmIP-RF': key };
    const field = target == 'sgtin' ? f.TEACHIN_FIELD_HMIP_SGTIN : f.TEACHIN_FIELD_HMIP_KEY;
    input(field, target == 'sgtin' ? sgtin : key, ' ' + QR.toLowerCase() + '\n', undefined, 'insertFromPaste');
    assert.equal(f.getHmIPTeachInSgtin(sgtin.value), SG, target);
    assert.equal(f.getHmIPTeachInKey(key.value), KEYS[1], target);
    assert.equal(sgtin.value, '0123-4567-89AB-CDEF-0123-4567', target);
  }
});

test('QR code content is not taken from the serial number field or when incomplete', () => {
  const serial = input(f.TEACHIN_FIELD_BIDCOS_SERIAL, fakeInput(''), 'EQ01SG0123');
  assert.equal(serial.value, 'EQ01SG0123');
  // incomplete QR code content stays as it is (a scan may still be running),
  // the SGTIN check on submit rejects it
  const sgtin = input(f.TEACHIN_FIELD_HMIP_SGTIN, fakeInput('', 0, 'serialHmIPLocal_HmIP-RF'),
    'EQ01SG0123456789ABCDEF01234567DLK0123');
  assert.equal(sgtin.value, 'EQ01SG0123456789ABCDEF01234567DLK0123');
  assert.equal(f.getHmIPTeachInSgtin(sgtin.value), null);
});

// Types the text character by character at the caret, as a scanner with
// keyboard output does.
function typeChars(field, element, text) {
  for (const ch of text) {
    const pos = element.selectionStart;
    input(field, element, element.value.slice(0, pos) + ch + element.value.slice(pos), pos + 1);
  }
}

test('QR code content typed by a scanner into the empty SGTIN or KEY field fills both', () => {
  // invented values, structure of the QR code of a HmIP label
  const SG = '0123456789ABCDEF01234567';
  const QR = 'EQ01SG' + SG + 'DLK' + KEYS[2];
  for (const target of ['sgtin', 'key']) {
    const sgtin = fakeInput('', 0, 'serialHmIPLocal_HmIP-RF');
    const key = fakeInput('', 0, 'keyHmIPLocal_HmIP-RF');
    f.document.elements = { 'serialHmIPLocal_HmIP-RF': sgtin, 'keyHmIPLocal_HmIP-RF': key };
    const field = target == 'sgtin' ? f.TEACHIN_FIELD_HMIP_SGTIN : f.TEACHIN_FIELD_HMIP_KEY;
    typeChars(field, target == 'sgtin' ? sgtin : key, QR);
    assert.equal(sgtin.value, '0123-4567-89AB-CDEF-0123-4567', target);
    assert.equal(f.getHmIPTeachInSgtin(sgtin.value), SG, target);
    assert.equal(f.getHmIPTeachInKey(key.value), KEYS[2], target);
  }
});

test('typing by hand is filtered as usual once it leaves the QR code pattern', () => {
  const key = fakeInput('', 0, 'keyHmIPLocal_HmIP-RF');
  typeChars(f.TEACHIN_FIELD_HMIP_KEY, key, 'e');
  assert.equal(key.value, 'E');
  typeChars(f.TEACHIN_FIELD_HMIP_KEY, key, '0123');
  assert.equal(key.value, 'E0123');
  typeChars(f.TEACHIN_FIELD_HMIP_KEY, key, '4');
  assert.equal(key.value, 'E0123-4');

  // an interrupted scan ("EQ01S" then a hex digit) is filtered like typed text
  const sgtin = fakeInput('', 0, 'serialHmIPLocal_HmIP-RF');
  typeChars(f.TEACHIN_FIELD_HMIP_SGTIN, sgtin, 'EQ01S');
  assert.equal(sgtin.value, 'EQ01S');
  typeChars(f.TEACHIN_FIELD_HMIP_SGTIN, sgtin, 'A');
  assert.equal(sgtin.value, 'E01A');

  // the BidCos serial number field does not wait for a QR code
  const serial = fakeInput('');
  typeChars(f.TEACHIN_FIELD_BIDCOS_SERIAL, serial, 'eq01sg');
  assert.equal(serial.value, 'EQ01SG');
});

test('tooltip of the help icon is attached after translatePage()', () => {
  // translatePage() rewrites the cells of the dialog and replaces the
  // elements, a tooltip attached before would be lost
  const text = fs.readFileSync(DIALOG, 'latin1');
  const translate = text.indexOf("puts \"translatePage('#messagebox');\"");
  const tooltip = text.indexOf('jQuery(".j_hmIPTeachInQrCodeHelp")');
  assert.ok(translate > 0 && tooltip > 0, 'translatePage or tooltip not found');
  assert.ok(tooltip > translate, 'tooltip attached before translatePage()');
});

