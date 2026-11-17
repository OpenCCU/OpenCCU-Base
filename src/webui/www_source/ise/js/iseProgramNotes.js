/** Program notes in the native program editor. */
var ProgramNotes = (function() {
  var notes = {};
  var initialized = false;
  var copyId = null;
  var storageKey = null;
  var limit = 2000;

  function controls() {
    return document.querySelectorAll('#prgrulecontent .programNote[data-note-id]');
  }

  function key(control) {
    return control.getAttribute('data-note-kind') + control.getAttribute('data-note-id');
  }

  function entities() {
    var result = [];
    var rules = document.querySelectorAll('#prgrulecontent > div[id^="rule"]');
    // The native editor recreates object IDs when a program copy is saved.
    // Keep notes with the visible branch and item order during that save.
    Array.prototype.forEach.call(rules, function(rule, branch) {
      ['r', 'c', 'a'].forEach(function(kind) {
        var found = rule.querySelectorAll('.programNote[data-note-kind="' + kind + '"]');
        Array.prototype.forEach.call(found, function(control, index) {
          result.push({ control: control, kind: kind, branch: branch, index: kind === 'r' ? 0 : index });
        });
      });
    });
    return result;
  }

  function readStored() {
    var source = document.getElementById('programNotesData');
    var result = {};
    if (!source) { return result; }
    (source.getAttribute('data-notes') || '').split(',').forEach(function(record) {
      var fields = record.split('|');
      if (fields.length === 4 && /^[rca]$/.test(fields[0]) && /^\d+$/.test(fields[1]) && /^\d+$/.test(fields[2])) {
        try { result[fields[0] + '|' + fields[1] + '|' + fields[2]] = decodeURIComponent(fields[3]); }
        catch (e) { /* Ignore a damaged record without hiding the editor. */ }
      }
    });
    return result;
  }

  function paint(control) {
    var value = notes[key(control)] || '';
    var editor = control.querySelector('textarea');
    var button = control.querySelector('button');
    if (editor && document.activeElement !== editor) { editor.value = value; }
    if (button) { button.className = value ? 'programNoteButton hasNote' : 'programNoteButton'; }
  }

  function remember() {
    if (!storageKey) { return; }
    try { sessionStorage.setItem(storageKey, JSON.stringify(notes)); }
    catch (e) { /* The editor remains usable without session storage. */ }
  }

  function encoded(value) {
    return encodeURIComponent(value).replace(/[!'()*]/g, function(character) {
      return '%' + character.charCodeAt(0).toString(16).toUpperCase();
    });
  }

  return {
    init: function(id) {
      if (initialized && copyId === id) { this.refresh(); return; }
      notes = {};
      initialized = false;
      copyId = id;
      storageKey = 'webui.programNotes.' + copyId;
      var restored = null;
      try { restored = JSON.parse(sessionStorage.getItem(storageKey)); }
      catch (e) { restored = null; }
      if (restored && typeof restored === 'object' && !Array.isArray(restored)) {
        notes = restored;
      } else {
        var stored = readStored();
        entities().forEach(function(entity) {
          notes[key(entity.control)] = stored[entity.kind + '|' + entity.branch + '|' + entity.index] || '';
        });
      }
      initialized = true;
      this.refresh();
      remember();
    },
    refresh: function() {
      if (!initialized) { return; }
      Array.prototype.forEach.call(controls(), function(control) {
        if (!Object.prototype.hasOwnProperty.call(notes, key(control))) { notes[key(control)] = ''; }
        paint(control);
      });
    },
    toggle: function(button) {
      if (!initialized) { return; }
      var control = button.parentNode;
      var editor = control.querySelector('textarea');
      editor.style.display = editor.style.display === 'block' ? 'none' : 'block';
      button.setAttribute('aria-expanded', editor.style.display === 'block' ? 'true' : 'false');
      if (editor.style.display === 'block') { editor.focus(); }
    },
    change: function(editor) {
      if (!initialized) { return; }
      notes[key(editor.parentNode)] = editor.value;
      paint(editor.parentNode);
      remember();
    },
    toggleAll: function(button) {
      var show = button.getAttribute('aria-pressed') !== 'true';
      Array.prototype.forEach.call(controls(), function(control) {
        var editor = control.querySelector('textarea');
        var toggle = control.querySelector('button');
        editor.style.display = show ? 'block' : 'none';
        toggle.setAttribute('aria-expanded', show ? 'true' : 'false');
      });
      button.setAttribute('aria-pressed', show ? 'true' : 'false');
    },
    serialize: function() {
      if (!initialized) { alert(translateKey('programNoteLoading')); return null; }
      var records = [];
      var valid = true;
      entities().forEach(function(entity) {
        var value = notes[key(entity.control)] || '';
        if (value.length > limit) { valid = false; }
        records.push(entity.kind + '|' + entity.branch + '|' + entity.index + '|' + encoded(value));
      });
      if (!valid) { alert(translateKey('programNoteTooLong')); return null; }
      return records.join(',');
    },
    clear: function() {
      if (storageKey) {
        try { sessionStorage.removeItem(storageKey); } catch (e) { /* Storage may be disabled. */ }
      }
      notes = {};
      initialized = false;
      copyId = null;
      storageKey = null;
    }
  };
})();
