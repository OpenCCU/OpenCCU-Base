// ReGa suggestions for the two native script editors.
const KEYWORDS = "if else elseif while foreach break continue return quit var object string integer real boolean time idarray true false null dom system ID_DEVICES ID_CHANNELS ID_DATAPOINTS ID_SYSTEM_VARIABLES ID_PROGRAMS OT_DEVICE OT_CHANNEL OT_DP OT_VARDP".split(" ");
const FUNCTIONS = ["Write", "WriteLine", "WriteURL", "WriteXML", "WriteHTML", "Debug", "Dump"];
const METHODS = {
  dom: ["GetObject"],
  system: ["Date"],
  object: ["ID", "Name", "Type", "TypeName", "IsTypeOf", "State", "Value", "Address", "Channels", "DPs", "DPByHssDP", "HssType", "Device", "Channel", "EnumUsedIDs", "EnumUsedNames", "Count", "Get", "GetAt"],
  string: ["Length", "Substr", "Find", "StrValueByIndex", "ToInteger", "ToFloat", "ToString"]
};

let catalog = [];
let catalogSession = "";
let catalogTime = 0;
let catalogRequest = null;

function sessionId() {
  if (typeof SessionId !== "undefined") return String(SessionId);
  return typeof getSessionId === "function" ? String(getSessionId()) : "";
}

// Build one shared catalog per session. Device requests are limited to two at a
// time so opening completion does not flood a CCU with parallel API calls.
function objectCatalog() {
  const session = sessionId();
  if (session !== catalogSession) {
    catalogSession = session;
    catalog = [];
    catalogTime = 0;
    catalogRequest = null;
  }
  if (!session || typeof homematic !== "function") return Promise.resolve(catalog);
  if (catalogRequest) return catalogRequest;
  if (Date.now() - catalogTime < 300000) return Promise.resolve(catalog);

  const items = [];
  function add(item, kind, deviceName = "") {
    if (item && /^\d+$/.test(String(item.id)) && item.name) {
      items.push({id: String(item.id), name: String(item.name), kind,
        address: String(item.address || ""), deviceName});
    }
  }
  function request(method, params) {
    return new Promise((resolve) => homematic(method, params, (value, error) => resolve(error ? null : value)));
  }
  async function devices() {
    const ids = await request("Device.listAll", {});
    if (!Array.isArray(ids)) return;
    let next = 0;
    async function worker() {
      while (next < ids.length) {
        const id = ids[next++];
        const device = await request("Device.get", {id});
        if (!device) continue;
        add(device, "Gerät");
        for (const channel of device.channels || []) add(channel, "Kanal", device.name);
      }
    }
    await Promise.all([worker(), worker()]);
  }
  catalogRequest = Promise.all([
    request("SysVar.getAll", {}).then((variables) => {
      if (Array.isArray(variables)) for (const variable of variables) add(variable, "Systemvariable");
    }),
    devices()
  ]).then(() => {
    if (session === catalogSession) {
      catalog = items;
      catalogTime = Date.now();
    }
    return catalog;
  }).finally(() => { if (session === catalogSession) catalogRequest = null; });
  return catalogRequest;
}

// A completion is suppressed inside a string or comment, except for the
// object-name argument of dom.GetObject("...").
function inLiteral(text) {
  let quote = "";
  for (let i = 0; i < text.length; i++) {
    const ch = text[i];
    if (quote) {
      if (ch === "\\") i++;
      else if (ch === quote) quote = "";
    } else if (ch === '"' || ch === "'") quote = ch;
    else if (ch === "!" && (i === 0 || /\s/.test(text[i - 1]))) {
      const end = text.indexOf("\n", i);
      if (end < 0) return true;
      i = end;
    }
  }
  return !!quote;
}

function methodOption(label) {
  return {label, type: "function", apply(view, _completion, from, to) {
    const hasBracket = view.state.sliceDoc(to, to + 1) === "(";
    const insert = hasBracket ? label : `${label}()`;
    view.dispatch({changes: {from, to, insert},
      selection: {anchor: from + label.length + 1}});
  }};
}

async function objectOptions(context, match) {
  const objects = await objectCatalog();
  if (context.aborted) return null;
  const from = context.pos - match[2].length;
  const quote = match[1];
  const names = new Map();
  for (const item of objects) names.set(item.name, (names.get(item.name) || 0) + 1);
  return {from, validFor: /^[^"'\r\n]*$/, options: objects.map((item) => ({
    label: item.name,
    type: item.kind === "Gerät" ? "class" : "variable",
    detail: `${item.deviceName ? item.deviceName + " · " : ""}${item.kind} · ${item.address} · ID ${item.id}`,
    info: names.get(item.name) > 1 ? "Mehrfacher Name: Die Objekt-ID wird eingefügt." : undefined,
    apply(view, _completion, start, end) {
      const tail = view.state.sliceDoc(end).match(/^[^"'\r\n]*/)[0];
      end += tail.length;
      if (names.get(item.name) > 1) {
        if (view.state.sliceDoc(end, end + 1) === quote) end++;
        view.dispatch({changes: {from: start - 1, to: end, insert: item.id},
          selection: {anchor: start - 1 + item.id.length}});
      } else {
        const name = item.name.replace(/\\/g, "\\\\").split(quote).join("\\" + quote);
        view.dispatch({changes: {from: start, to: end, insert: name},
          selection: {anchor: start + name.length}});
      }
    }
  }))};
}

export function regaCompletion(context) {
  const before = context.state.sliceDoc(0, context.pos);
  const objectMatch = /\bGetObject\s*\(\s*(["'])((?:\\.|[^"'\\\r\n])*)$/.exec(before);
  if (objectMatch) {
    return inLiteral(before.slice(0, objectMatch.index)) ? null : objectOptions(context, objectMatch);
  }
  if (inLiteral(before)) return null;

  const token = context.matchBefore(/[A-Za-z_][A-Za-z_0-9]*$/);
  const from = token ? token.from : context.pos;
  const prefix = token ? token.text : "";
  const member = /([A-Za-z_][A-Za-z_0-9]*|\))\.$/.exec(before.slice(0, from));
  if (member) {
    let family = member[1] === "dom" ? "dom" : member[1] === "system" ? "system" : "object";
    if (family === "object" && member[1] !== ")" &&
        new RegExp(`\\bstring\\s+${member[1]}\\b`).test(before)) family = "string";
    return {from, options: METHODS[family].map(methodOption), validFor: /^\w*$/};
  }
  if (!prefix && !context.explicit) return null;
  const options = KEYWORDS.map((label) => ({label, type: "keyword"}));
  options.push(...FUNCTIONS.map(methodOption));
  const locals = new Set();
  const declaration = /\b(?:var|object|string|integer|real|boolean|time|idarray)\s+([A-Za-z_]\w*)/g;
  let match;
  while ((match = declaration.exec(before))) {
    if (!locals.has(match[1])) {
      locals.add(match[1]);
      options.push({label: match[1], type: "variable", detail: "Lokale Variable"});
    }
  }
  return {from, options, validFor: /^\w*$/};
}
