#!/usr/bin/env node
// Tests for the value dialog of a system variable (ID_EDIT_SYSVAR_VALUE,
// src/webui/rega/www/pages/tabs/admin/msg/newSysVar.htm with editValueOnly).
//
// The dialog sets only the value through the action setSysVarValue
// (src/webui/rega/www/esp/system.fn), instead of saving the whole variable
// through saveSysVar, which also rewrites name, description, type, unit, value
// names, value list, range, channel and alarm settings. It is also offered
// for the presence variable, which saveSysVar would turn into a logic
// variable.
// Run with: node --test tests/webui/test-sysvar-value-dialog.js
'use strict';

const assert = require('node:assert/strict');
const fs = require('node:fs');
const path = require('node:path');
const test = require('node:test');
const vm = require('node:vm');

const ROOT = path.join(__dirname, '..', '..', 'src', 'webui');
const DIALOG = path.join(ROOT, 'rega', 'www', 'pages', 'tabs', 'admin', 'msg', 'newSysVar.htm');
const REGA = path.join(ROOT, 'www_source', 'ise', 'js', 'rega.js');
const SYSTEM_FN = path.join(ROOT, 'rega', 'www', 'esp', 'system.fn');
const SYSTEM_HTM = path.join(ROOT, 'rega', 'www', 'esp', 'system.htm');
const STARTPAGE = path.join(ROOT, 'rega', 'www', 'pages', 'tabs', 'startpage.htm');
const SYSVARS = path.join(ROOT, 'rega', 'www', 'pages', 'tabs', 'control', 'sysvars.htm');

const read = (file) => fs.readFileSync(file, 'latin1').replace(/\r\n/g, '\n');
const dialogSource = read(DIALOG);

// Returns the source of "name = function(...) { ... };" from the dialog page.
function extractFunction(name) {
  const start = dialogSource.indexOf('  ' + name + ' = function');
  assert.notEqual(start, -1, `${name} not found`);
  const end = dialogSource.indexOf('\n  };', start);
  assert.notEqual(end, -1, `end of ${name} not found`);
  return dialogSource.slice(start, end + 5);
}

// The value-only branch at the start of saveSysVars(), with the ReGa output
// of the variable ID replaced by a number.
function extractValueOnlyBranch(varid) {
  const start = dialogSource.indexOf('    if (svEditValueOnly)\n    {');
  assert.notEqual(start, -1, 'value-only branch not found');
  const end = dialogSource.indexOf('\n      return;\n    }', start);
  assert.notEqual(end, -1, 'end of the value-only branch not found');
  const code = dialogSource.slice(start, end + '\n      return;\n    }'.length);
  return code.replace('<% Write(system.GetVar("varid")); %>', String(varid));
}

// ReGa's uriDecode(): replaces %XX with two hex digits, leaves anything else.
function uriDecode(value) {
  return value.replace(/%([0-9A-Fa-f]{2})/g, (m, hex) => String.fromCharCode(parseInt(hex, 16)));
}

// Decodes a request body of ReGa.encode() like ReGa and returns its statements.
function decodeBody(body) {
  const prefix = '<prototypejs><![CDATA[';
  assert.ok(body.startsWith(prefix));
  return uriDecode(body.slice(prefix.length, body.indexOf(']')));
}

function makeDialog(elements, varid) {
  const alerts = [];
  const requests = [];
  const reloads = [];
  const context = {
    $: (id) => elements[id] || null,
    alert: (text) => alerts.push(text),
    translateKey: (key) => key,
    escape: globalThis.escape,
    SessionId: 'SID',
    svEditValueOnly: true,
    reloadSysVars: () => reloads.push(true),
    DeviceList: { getChannel: () => null, beginUpdateDevice: () => {} },
    Ajax: {
      Request: function(url, opts) {
        requests.push({ url, opts });
        opts.onComplete();
      },
    },
  };
  vm.runInNewContext(
    'String.prototype.strip = function() { return this.trim(); };\n' +
    fs.readFileSync(REGA, 'latin1') + '\n' +
    extractFunction('svEscapeRegaString') + '\n' +
    extractFunction('svEditedValue') + '\n' +
    'saveValue = function() {\n' + extractValueOnlyBranch(varid) + '\n  return "full save";\n};',
    context);
  return { context, alerts, requests, reloads };
}

test('svEditedValue() reads the value of each value editor', () => {
  assert.equal(makeDialog({ selValue: { value: '1' } }).context.svEditedValue(), '1');
  assert.equal(makeDialog({ selEnum: { value: '2' } }).context.svEditedValue(), '2');
  assert.equal(makeDialog({ selEnum: { value: '' } }).context.svEditedValue(), '0');
  assert.equal(makeDialog({ txValue: { value: 'A%41 \'x\' \\ y' } }).context.svEditedValue(), 'A%41 \'x\' \\ y');
  assert.equal(makeDialog({ txValue: { value: '' } }).context.svEditedValue(), '');
});

test('svEditedValue() limits a number to the range and rejects text', () => {
  const number = (value) => ({ inpValue: { value }, inpMin: { value: '0' }, inpMax: { value: '100' } });
  assert.equal(makeDialog(number('42.5')).context.svEditedValue(), '42.5');
  assert.equal(makeDialog(number('150')).context.svEditedValue(), '100');
  assert.equal(makeDialog(number('-3')).context.svEditedValue(), '0');
  for (const value of ['', '  ', 'abc']) {
    const dialog = makeDialog(number(value));
    assert.equal(dialog.context.svEditedValue(), null);
    assert.deepEqual(dialog.alerts, ['alertValOutOfRange']);
  }
});

test('the value dialog sends only the value through setSysVarValue', () => {
  for (const value of ['plain', 'O\'Brien \\ C:\\temp', '42', '1']) {
    const dialog = makeDialog({ txValue: { value }, chnId: { value: '-1' } }, 4711);
    assert.equal(dialog.context.saveValue(), undefined);
    assert.equal(dialog.requests.length, 1);
    assert.equal(dialog.requests[0].url, '/esp/system.htm?sid=SID');
    const script = decodeBody(dialog.requests[0].opts.postBody);
    const match = /^action = 'setSysVarValue';integer varid = 4711;string sValue = '((?:[^'\\]|\\.)*)';$/s.exec(script);
    assert.ok(match, `unexpected script ${JSON.stringify(script)}`);
    assert.equal(match[1].replace(/\\(.)/g, '$1'), value);
    assert.equal(dialog.reloads.length, 1);
  }
});

test('an invalid number sends nothing', () => {
  const dialog = makeDialog({ inpValue: { value: 'x' }, inpMin: { value: '0' }, inpMax: { value: '1' }, chnId: { value: '-1' } }, 1);
  assert.equal(dialog.context.saveValue(), undefined);
  assert.equal(dialog.requests.length, 0);
});

test('setSysVarValue only sets the state of a system variable for administrators', () => {
  const source = read(SYSTEM_FN);
  const start = source.indexOf('function ::setSysVarValue()');
  assert.notEqual(start, -1, 'setSysVarValue not found');
  const end = source.indexOf('\nfunction ::', start + 1);
  const body = source.slice(start, end).split('\n').filter((line) => !line.trim().startsWith('!')).join('\n');
  assert.match(body, /oUser\.UserLevel\(\) == iulAdmin/);
  assert.match(body, /IsTypeOf\( OT_VARDP \)/);
  assert.match(body, /IsTypeOf\( OT_ALARMDP \)/);
  assert.match(body, /sv\.State\( system\.GetVar\("sValue"\) \);/);
  for (const call of ['Name(', 'DPInfo(', 'ValueUnit(', 'ValueType(', 'ValueSubType(', 'MetaData(', 'Channel(', 'AlArm(', 'DeleteObject(', 'CreateObject(']) {
    assert.ok(!body.includes(call), `setSysVarValue must not call ${call}`);
  }
  assert.match(read(SYSTEM_HTM), /action == "setSysVarValue"\s*\) \{ Call\( "system\.fn::setSysVarValue\(\)" \);/);
});

test('the value dialog is offered for the presence variable', () => {
  for (const file of [STARTPAGE, SYSVARS]) {
    const source = read(file);
    assert.ok(source.includes('ID_EDIT_SYSVAR_VALUE'), `${path.basename(file)} has no value dialog`);
    assert.ok(!source.includes('istPresent'), `${path.basename(file)} still excludes the presence variable`);
  }
  assert.match(dialogSource, /if \(\(sv\.ValueSubType\(\) == istPresent\) && svEditValueOnly\) \{\n.*\n\s*Write\("loadLogicEdit\(/);
});
