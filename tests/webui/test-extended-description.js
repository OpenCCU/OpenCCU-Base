#!/usr/bin/env node
// Tests for getExtendedDescription() in src/webui/www_source/ise/js/functions.js,
// which returns the extended description of a channel shown in the device
// list, the device settings and the channel selection of direct links.
// The channel type named by the caller wins, but the channel object is looked
// up as well: its visibility decides whether a virtual receiver of the
// HmIP-PS family is reported as unsupported, its mode whether a blind channel
// is described as blind or shutter (OpenCCU/OpenCCU#4797).
// The function is run in a small stub of the page functions.
// Run with: node --test tests/webui/test-extended-description.js
'use strict';

const assert = require('node:assert/strict');
const fs = require('node:fs');
const path = require('node:path');
const test = require('node:test');
const vm = require('node:vm');

const ROOT = path.join(__dirname, '..', '..', 'src', 'webui');
const SOURCE = fs.readFileSync(path.join(ROOT, 'www_source', 'ise', 'js', 'functions.js'), 'latin1');
const START = SOURCE.indexOf('getExtendedDescription = function');
const END = SOURCE.indexOf('\n};', START);
assert.ok(START >= 0 && END > START, 'getExtendedDescription() not found');
const CODE = SOURCE.slice(START, END + 3);

// Calls getExtendedDescription(descr). channels are the channels of the
// device list by address, metadata the answer of Interface.getMetadata.
function describe(descr, channels, metadata) {
  const context = {
    DeviceList: { getChannelByAddress: (address) => (channels || {})[address] },
    translateKey: (key) => '{' + key + '}',
    homematic: () => metadata,
    jQuery: { each: () => {} },
  };
  vm.createContext(context);
  vm.runInContext(CODE, context);
  return context.getExtendedDescription(descr);
}

function channel(props) {
  return Object.assign({
    id: '4711',
    nameExtention: '',
    getVirtChannelType() { return this.virtChannelType; },
  }, props);
}

const SWITCH = '{chType_SWITCH_VIRTUAL_RECEIVER}';
const UNSUPPORTED = '{lblHmIP_NotSupported}';
const PS_FAMILY = ['HmIP-PS', 'HmIP-PSM', 'HmIP-PSM-PE', 'HmIP-PS-UK', 'HmIP-PSM-IT', 'HmIP-PSM-CH'];

for (const deviceType of PS_FAMILY) {
  const address = '0001D3C99C6AB3:4';
  const cgi = { deviceType, channelType: 'SWITCH_VIRTUAL_RECEIVER', channelIndex: '4', channelAddress: address };
  const visible = { [address]: channel({ channelType: 'SWITCH_VIRTUAL_RECEIVER', isVisible: true }) };
  const hidden = { [address]: channel({ channelType: 'SWITCH_VIRTUAL_RECEIVER', isVisible: false }) };

  test(deviceType + ': a visible virtual receiver is a switch actuator', () => {
    // (device settings and channel selection name the channel type, the
    // visibility then comes from the caller or from the channel object)
    assert.equal(describe(Object.assign({ isVisible: true }, cgi), visible), SWITCH);
    assert.equal(describe(Object.assign({ isVisible: true }, cgi), {}), SWITCH);
    assert.equal(describe(cgi, visible), SWITCH);
    // (the list of direct links names no channel type)
    assert.equal(describe({ deviceType, channelIndex: '4', channelAddress: address }, visible), SWITCH);
  });

  test(deviceType + ': a hidden virtual receiver is not supported', () => {
    assert.equal(describe(Object.assign({ isVisible: false }, cgi), hidden), UNSUPPORTED);
    assert.equal(describe(Object.assign({ isVisible: false }, cgi), {}), UNSUPPORTED);
    assert.equal(describe(cgi, hidden), UNSUPPORTED);
    assert.equal(describe({ deviceType, channelIndex: '4', channelAddress: address }, hidden), UNSUPPORTED);
  });

  test(deviceType + ': the visibility of the caller wins over the channel object', () => {
    assert.equal(describe(Object.assign({ isVisible: true }, cgi), hidden), SWITCH);
    assert.equal(describe(Object.assign({ isVisible: false }, cgi), visible), UNSUPPORTED);
  });

  test(deviceType + ': a channel of unknown visibility is not reported as unsupported', () => {
    assert.equal(describe(cgi, {}), SWITCH);
  });
}

test('other switch actuators ignore the visibility', () => {
  const descr = { deviceType: 'HmIP-BSM', channelType: 'SWITCH_VIRTUAL_RECEIVER', channelIndex: '4', channelAddress: 'A:4' };
  assert.equal(describe(Object.assign({ isVisible: false }, descr), {}), SWITCH);
});

for (const deviceType of ['HmIP-DRBLI4', 'HmIP-BBL-2', 'HmIPW-DRBL4']) {
  const address = '00111A498A3B5C:2';
  // (the device description always names the blind type)
  const cgi = { deviceType, channelType: 'BLIND_VIRTUAL_RECEIVER', channelIndex: '2', channelAddress: address, isVisible: true };
  const inMode = (virtChannelType) => ({
    [address]: channel({ channelType: 'BLIND_VIRTUAL_RECEIVER', isVisible: true, virtChannelType }),
  });

  test(deviceType + ': a channel in shutter mode is a shutter actuator', () => {
    assert.equal(describe(cgi, inMode('SHUTTER_VIRTUAL_RECEIVER')), '{chType_SHUTTER_VIRTUAL_RECEIVER}');
  });

  test(deviceType + ': a channel in blind mode is a blind actuator', () => {
    assert.equal(describe(cgi, inMode('BLIND_VIRTUAL_RECEIVER')), '{chType_BLIND_VIRTUAL_RECEIVER}');
  });

  test(deviceType + ': the named type is used while the mode is unknown', () => {
    // (the channel has not set its virtual channel type yet, or is not in
    // the device list)
    assert.equal(describe(cgi, inMode(undefined)), '{chType_BLIND_VIRTUAL_RECEIVER}');
    assert.equal(describe(cgi, {}), '{chType_BLIND_VIRTUAL_RECEIVER}');
  });
}

for (const deviceType of ['HmIP-WGT', 'HmIP-WGTC']) {
  const address = '002A1D8999F1E2:8';
  const cgi = { deviceType, channelType: 'SWITCH_VIRTUAL_RECEIVER', channelIndex: '8', channelAddress: address, isVisible: true };
  const channels = { [address]: channel({ channelType: 'SWITCH_VIRTUAL_RECEIVER', isVisible: true }) };

  test(deviceType + ': the channel mode selects the description', () => {
    assert.equal(describe(cgi, channels, 'modeBWTH'), '{chType_SWITCH_VIRTUAL_RECEIVER_BWTH}');
    assert.equal(describe(cgi, channels, ''), SWITCH);
  });

  test(deviceType + ': a channel missing in the device list is a switch actuator', () => {
    assert.equal(describe(cgi, {}, 'modeBWTH'), SWITCH);
  });
}

test('the device settings and the channel selection pass the visible flag', () => {
  for (const [file, array] of [['ic_deviceparameters.cgi', 'ch_descr'], ['ic_selchannel.cgi', 'dev_descr']]) {
    const cgi = fs.readFileSync(path.join(ROOT, 'www', 'config', file), 'latin1');
    const calls = cgi.split('\n').filter((line) => line.includes('getExtendedDescription('));
    assert.equal(calls.length, 1, file);
    assert.ok(calls[0].includes('\\"isVisible\\" : [expr {($' + array + '(FLAGS) & 1) ? "true" : "false"}]'), file);
  }
});
