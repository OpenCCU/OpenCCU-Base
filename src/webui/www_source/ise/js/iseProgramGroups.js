/** Program group management and native program list rendering. */
var ProgramGroups = (function() {
  var current = null;
  var table = null;
  var view = '';
  var closed = {};
  var direction = 1;
  var catalog = [];
  var dialog = null;

  function read(source) {
    var raw = source ? source.getAttribute('data-groups') || '' : '';
    var separator = raw.indexOf('|');
    try {
      var data = separator < 0 ? {} : JSON.parse(decodeURIComponent(raw.substr(separator + 1)));
      return {
        revision: separator < 0 ? '0' : raw.substr(0, separator),
        next: Number(data.next) || 0,
        groups: Array.isArray(data.groups) ? data.groups : [],
        assignments: data.assignments && typeof data.assignments === 'object' ? data.assignments : {},
        assignmentNames: data.assignmentNames && typeof data.assignmentNames === 'object' ? data.assignmentNames : {}
      };
    } catch (e) { return {revision:'0', next:0, groups:[], assignments:{}, assignmentNames:{}}; }
  }

  // Reconnect an object after its internal ID changes, only when its name is unique.
  function reconcile(objects) {
    var byName = {};
    objects.forEach(function(object) {
      if (!byName[object.name]) { byName[object.name] = []; }
      byName[object.name].push(object.id);
    });
    var savedNames = {};
    Object.keys(current.assignmentNames).forEach(function(id) {
      var name = current.assignmentNames[id];
      if (!savedNames[name]) { savedNames[name] = []; }
      savedNames[name].push(id);
    });
    Object.keys(current.assignments).forEach(function(oldId) {
      if (objects.some(function(object) { return object.id === oldId; })) { return; }
      var name = current.assignmentNames[oldId];
      if (name && byName[name] && byName[name].length === 1 && savedNames[name].length === 1) {
        var newId = byName[name][0];
        if (!current.assignments[newId]) {
          current.assignments[newId] = current.assignments[oldId];
          current.assignmentNames[newId] = name;
          delete current.assignments[oldId];
          delete current.assignmentNames[oldId];
        }
      }
    });
  }

  function storageKey() { return 'webui.programGroups.' + view; }
  function remember() {
    try { sessionStorage.setItem(storageKey(), Object.keys(closed).filter(function(id) { return closed[id]; }).join(',')); }
    catch (e) { /* Group state remains usable until the page closes. */ }
  }
  function loadState() {
    closed = {};
    try {
      (sessionStorage.getItem(storageKey()) || '').split(',').forEach(function(id) {
        if (/^[1-9]\d*$/.test(id)) { closed[id] = true; }
      });
    } catch (e) { /* The default is expanded. */ }
  }

  function members() {
    return Array.prototype.filter.call(table.querySelectorAll('tr[id]'), function(row) {
      return /^(?:program|pr)\d+$/.test(row.id);
    });
  }
  function groupRows() {
    return Array.prototype.slice.call(table.querySelectorAll('tr.programGroupRow'));
  }
  function orderedGroups() {
    return current.groups.slice().sort(function(a, b) {
      return direction * a.name.localeCompare(b.name, undefined, {sensitivity:'base'});
    });
  }
  function toggle(id) {
    closed[id] = !closed[id];
    remember();
    visibility();
  }
  function visibility() {
    if (!table) { return; }
    groupRows().forEach(function(row) {
      var id = row.getAttribute('data-group-id');
      var button = row.querySelector('button');
      button.textContent = (closed[id] ? '+ ' : '\u2212 ') + button.getAttribute('data-group-name');
      button.setAttribute('aria-expanded', closed[id] ? 'false' : 'true');
    });
    members().forEach(function(row) {
      var id = row.getAttribute('data-group-id');
      row.classList.toggle('programGroupClosed', !!closed[id]);
    });
  }

  function arrange() {
    if (!table || !current || !current.groups.length) { return; }
    var rows = members();
    var parent = rows.length ? rows[0].parentNode : table.tBodies[0];
    if (!parent) { return; }
    groupRows().forEach(function(row) { row.remove(); });
    var byGroup = {};
    var ungrouped = [];
    current.groups.forEach(function(group) { byGroup[group.id] = []; });
    rows.forEach(function(row) {
      var id = row.id.replace(/\D/g, '');
      var groupId = current.assignments[id];
      row.setAttribute('data-group-id', byGroup[groupId] ? groupId : '');
      row.classList.toggle('programGroupMember', !!byGroup[groupId]);
      if (byGroup[groupId]) { byGroup[groupId].push(row); }
      else { ungrouped.push(row); }
    });
    orderedGroups().forEach(function(group) {
      var row = document.createElement('tr');
      row.className = 'programGroupRow';
      row.setAttribute('data-group-id', group.id);
      var cell = row.insertCell();
      var button = document.createElement('button');
      button.type = 'button';
      button.className = 'programGroupToggle';
      button.setAttribute('data-group-name', group.name);
      button.onclick = function() { toggle(group.id); };
      cell.appendChild(button);
      var countCell = row.insertCell();
      var count = document.createElement('span');
      count.className = 'programGroupCount';
      count.textContent = translateKey('programGroupCount') + ': ' + byGroup[group.id].length;
      countCell.appendChild(count);
      var rest = row.insertCell();
      rest.colSpan = Math.max(1, table.rows[0].cells.length - 2);
      parent.appendChild(row);
      byGroup[group.id].forEach(function(member) { parent.appendChild(member); });
    });
    ungrouped.forEach(function(row) { parent.appendChild(row); });
    visibility();
  }

  function save(previous) {
    var catalogNames = {};
    catalog.forEach(function(program) { catalogNames[program.id] = program.name; });
    Object.keys(current.assignments).forEach(function(id) {
      if (catalogNames[id]) { current.assignmentNames[id] = catalogNames[id]; }
    });
    Object.keys(current.assignmentNames).forEach(function(id) {
      if (!current.assignments[id]) { delete current.assignmentNames[id]; }
    });
    var data = encodeURIComponent(JSON.stringify({next:current.next, groups:current.groups, assignments:current.assignments, assignmentNames:current.assignmentNames}));
    var body = 'string action = "SaveProgramGroups";';
    body += 'string revision = "' + ReGa.escapeString(current.revision) + '";';
    body += 'string groups = "' + ReGa.escapeString(data) + '";';
    new Ajax.Request('/esp/programs.htm?sid=' + SessionId, {
      postBody: ReGa.encode(body),
      onComplete: function(response) {
        var answer = response.responseText.trim();
        if (answer.indexOf('OK|') !== 0) {
          current = JSON.parse(previous);
          alert(translateKey('programGroupSaveFailed'));
          return;
        }
        current.revision = answer.substr(3);
        closeDialog();
        managerRows();
      }
    });
  }

  function closeDialog() {
    if (dialog) { dialog.close(); dialog.remove(); dialog = null; }
  }
  function edit(id) {
    if (dialog) { return; }
    var group = current.groups.filter(function(item) { return item.id === id; })[0];
    var selected = {};
    Object.keys(current.assignments).forEach(function(program) {
      if (current.assignments[program] === id) { selected[program] = true; }
    });
    dialog = document.createElement('dialog');
    dialog.className = 'programGroupDialog';
    var heading = document.createElement('h2');
    heading.textContent = translateKey(group ? 'programGroupEdit' : 'programGroupNew');
    dialog.appendChild(heading);
    var name = document.createElement('input');
    name.type = 'text'; name.maxLength = 80; name.value = group ? group.name : '';
    name.setAttribute('aria-label', translateKey('programGroupName'));
    dialog.appendChild(name);
    var picker = document.createElement('div');
    picker.className = 'programGroupPicker';
    catalog.forEach(function(program) {
      var label = document.createElement('label');
      var check = document.createElement('input');
      check.type = 'checkbox'; check.checked = !!selected[program.id];
      check.onchange = function() { selected[program.id] = check.checked; };
      label.appendChild(check);
      label.appendChild(document.createTextNode(program.name));
      picker.appendChild(label);
    });
    dialog.appendChild(picker);
    var back = document.createElement('button');
    back.textContent = translateKey('footerBtnPageBack');
    back.onclick = closeDialog;
    var okay = document.createElement('button');
    okay.textContent = translateKey('footerBtnOk');
    okay.onclick = function() {
      var title = name.value.trim();
      if (!title || current.groups.some(function(other) { return other.id !== id && other.name.toLowerCase() === title.toLowerCase(); })) {
        alert(translateKey('programGroupInvalidName')); return;
      }
      var previous = JSON.stringify(current);
      var existing = current.groups.filter(function(item) { return item.id === id; })[0];
      if (!existing) { id = String(++current.next); current.groups.push({id:id, name:title}); }
      else { existing.name = title; }
      Object.keys(current.assignments).forEach(function(program) {
        if (current.assignments[program] === id) { delete current.assignments[program]; }
      });
      Object.keys(selected).forEach(function(program) {
        if (selected[program]) { current.assignments[program] = id; }
      });
      save(previous);
    };
    dialog.appendChild(back); dialog.appendChild(okay);
    document.body.appendChild(dialog);
    dialog.showModal();
    name.focus();
  }

  function managerRows() {
    var tableBody = document.querySelector('#programGroupManager tbody');
    if (!tableBody) { return; }
    tableBody.innerHTML = '';
    orderedGroups().forEach(function(group) {
      var row = tableBody.insertRow();
      row.insertCell().textContent = group.name;
      var assigned = catalog.filter(function(program) { return current.assignments[program.id] === group.id; });
      row.insertCell().textContent = assigned.length;
      row.insertCell().textContent = assigned.slice(0, 5).map(function(program) { return program.name; }).join('; ') + (assigned.length > 5 ? '; ...' : '');
      var actions = row.insertCell();
      var change = document.createElement('button');
      change.textContent = translateKey('footerBtnEdit');
      change.onclick = function() { edit(group.id); };
      actions.appendChild(change);
      var remove = document.createElement('button');
      remove.textContent = translateKey('footerBtnDelete');
      remove.onclick = function() {
        if (!confirm(translateKey('programGroupDeleteConfirm'))) { return; }
        var previous = JSON.stringify(current);
        current.groups = current.groups.filter(function(item) { return item.id !== group.id; });
        Object.keys(current.assignments).forEach(function(program) {
          if (current.assignments[program] === group.id) { delete current.assignments[program]; }
        });
        save(previous);
      };
      actions.appendChild(remove);
    });
  }

  return {
    render: function(id, kind) {
      table = document.getElementById(id);
      view = kind;
      current = read(document.getElementById('programGroupData'));
      catalog = Array.prototype.map.call(document.querySelectorAll('#programGroupCatalog option'), function(option) {
        return {id:option.value, name:option.textContent};
      });
      reconcile(catalog);
      loadState();
      arrange();
      table.classList.toggle('programGroupsEdit', kind === 'edit' && current.groups.length > 0);
      var footer = document.getElementById('footer');
      if (footer && current.groups.length) {
        var button = document.createElement('button');
        button.className = 'FooterButton';
        button.textContent = translateKey('programGroupToggleAll');
        button.onclick = function() {
          var allClosed = current.groups.every(function(group) { return !!closed[group.id]; });
          current.groups.forEach(function(group) { closed[group.id] = !allClosed; });
          remember(); visibility();
        };
        var row = footer.querySelector('tr');
        if (row) { row.insertCell().appendChild(button); }
      }
    },
    sort: function(kind) {
      if (!current || view !== kind || !current.groups.length) { return false; }
      direction = -direction;
      arrange();
      return true;
    },
    manager: function() {
      current = read(document.getElementById('programGroupData'));
      catalog = Array.prototype.map.call(document.querySelectorAll('#programGroupCatalog option'), function(option) {
        return {id:option.value, name:option.textContent};
      });
      reconcile(catalog);
      managerRows();
    },
    create: function() { edit('0'); }
  };
})();
