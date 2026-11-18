/**
 * Live search shared by native list templates and their existing filters.
 */
iseProgramListFilter = {
  filters: new WeakMap(),

  /** Create and restore the toolbar for a single list. */
  create: function(table, view, showStatus, refresh, columns) {
    var defaults = columns || [0, 1, 2, 3];
    var key = 'iseProgramListFilter_' + view;
    var saved = {};
    try { saved = JSON.parse(sessionStorage.getItem(key)) || {}; } catch (e) {}
    var state = {
      query: typeof saved.query === 'string' ? saved.query : '',
      columns: Array.isArray(saved.columns) ? saved.columns.filter(function(index) {
        return defaults.indexOf(index) !== -1;
      }) : defaults.slice(),
      status: showStatus && /^(active|inactive)$/.test(saved.status) ? saved.status : 'all'
    };
    var toolbar = document.createElement('div');
    toolbar.className = 'programListSearch';
    var search = document.createElement('input');
    search.type = 'search';
    search.value = state.query;
    search.placeholder = translateKey('thFilter');
    search.setAttribute('aria-label', translateKey('thFilter'));
    var german = /^de/i.test(getLang());
    var clear = document.createElement('button');
    clear.type = 'button';
    clear.className = 'StdButton';
    clear.textContent = german ? 'Suche leeren' : 'Clear search';
    clear.onclick = function() {
      search.value = '';
      state.query = '';
      changed();
      search.focus();
    };
    var indicator = document.createElement('span');
    indicator.textContent = german ? 'Filter aktiv' : 'Filter active';
    var count = document.createElement('span');
    count.className = 'programListSearchCount';
    count.setAttribute('role', 'status');
    var choices = document.createElement('details');
    var summary = document.createElement('summary');
    summary.textContent = german ? 'Spalten ausw\xE4hlen' : 'Choose columns';
    choices.appendChild(summary);
    var headers = table.rows[0].cells;
    defaults.forEach(function(index) {
        if (!headers[index]) { return; }
        var label = document.createElement('label');
        var checkbox = document.createElement('input');
        checkbox.type = 'checkbox';
        checkbox.checked = state.columns.indexOf(index) !== -1;
        checkbox.onchange = function() {
          if (checkbox.checked) {
            state.columns.push(index);
          } else {
            state.columns = state.columns.filter(function(column) { return column !== index; });
          }
          changed();
        };
        label.appendChild(checkbox);
        label.appendChild(document.createTextNode(' ' + headers[index].textContent.trim()));
        choices.appendChild(label);
    });
    var status;
    if (showStatus) {
      status = document.createElement('select');
      status.setAttribute('aria-label', translateKey('lblActiv'));
      (german ? [['all', 'Alle Programme'], ['active', 'Nur aktive Programme'],
        ['inactive', 'Nur deaktivierte Programme']] :
        [['all', 'All programs'], ['active', 'Active programs'],
         ['inactive', 'Inactive programs']]).forEach(function(item) {
        var option = document.createElement('option');
        option.value = item[0];
        option.textContent = item[1];
        status.appendChild(option);
      });
      status.value = state.status;
      status.onchange = function() { state.status = status.value; changed(); };
    }
    function updateControls() {
      clear.disabled = !state.query;
      indicator.hidden = !state.query.trim() && state.status === 'all' &&
        state.columns.length === defaults.length;
    }
    function changed() {
      updateControls();
      try { sessionStorage.setItem(key, JSON.stringify(state)); } catch (e) {}
      refresh();
    }
    updateControls();
    search.oninput = function() { state.query = search.value; changed(); };
    toolbar.appendChild(search);
    toolbar.appendChild(clear);
    toolbar.appendChild(indicator);
    if (status) { toolbar.appendChild(status); }
    toolbar.appendChild(count);
    toolbar.appendChild(choices);
    table.parentNode.insertBefore(toolbar, table);
    table.addEventListener('change', function(event) {
      if (event.target.matches('input[onclick*="SetActive("]')) { refresh(); }
    });
    var filter = {
      accepts: function(row) {
        if (!row) { return false; }
        if (state.status !== 'all') {
          var active = row.querySelector('input[onclick*="SetActive("]');
          if (!active || active.checked !== (state.status === 'active')) { return false; }
        }
        var terms = state.query.toLocaleLowerCase().trim().split(/\s+/).filter(Boolean);
        if (!terms.length) { return true; }
        var content = state.columns.map(function(index) {
          return row.cells[index] ? row.cells[index].textContent : '';
        }).join(' ').toLocaleLowerCase();
        return terms.every(function(term) { return content.indexOf(term) !== -1; });
      },
      setCount: function(shown, total) {
        count.textContent = shown + ' / ' + total;
      },
      updateCount: function() {
        var rows = Array.prototype.filter.call(table.rows, function(row) {
          return /^(program|pr)\d+$/.test(row.id);
        });
        var shown = rows.filter(function(row) { return row.style.display !== 'none'; }).length;
        count.textContent = shown + ' / ' + rows.length;
      },
      clear: function() {
        state.query = '';
        state.status = 'all';
        state.columns = defaults.slice();
        search.value = '';
        if (status) { status.value = 'all'; }
        Array.prototype.forEach.call(choices.querySelectorAll('input'), function(input) {
          input.checked = true;
        });
        changed();
      }
    };
    this.filters.set(table, filter);
    return filter;
  },

  /** Clear the live search from the existing footer reset action. */
  clearList: function(id) {
    var filter = this.filters.get(document.getElementById(id));
    if (filter) { filter.clear(); }
  },

  /** Apply live search without changing native visibility or group state. */
  attach: function(id, view, columns, items) {
    var table = document.getElementById(id);
    if (!table || this.filters.has(table)) { return; }
    var entries = items.map(function(item) {
      return {
        row: document.getElementById(item.trid),
        control: document.getElementById(item.ctrlId || '')
      };
    }).filter(function(entry) {
      return entry.row && entry.row.closest('table') === table;
    });
    if (!entries.length) { return; }

    function refresh() {
      var total = 0;
      var shown = 0;
      entries.forEach(function(entry) {
        var matches = filter.accepts(entry.row);
        entry.row.toggleAttribute('data-live-filter-hidden', !matches);
        if (entry.control && entry.control.closest('table') === table) {
          entry.control.toggleAttribute('data-live-filter-hidden', !matches);
        }
      });
      entries.forEach(function(entry) {
        if (!entry.row.hidden && entry.row.style.display !== 'none' &&
            !entry.row.classList.contains('hidden')) {
          total++;
          if (entry.row.getClientRects().length) { shown++; }
        }
      });
      filter.setCount(shown, total);
    }

    var filter = this.create(table, view, false, refresh, columns);
    // Native controls and group toggles can update the same rows after render.
    // Search visibility uses its own attribute, excluded from the observer.
    var observer = new MutationObserver(refresh);
    observer.observe(table, {subtree: true, childList: true, characterData: true,
      attributes: true, attributeFilter: ['style', 'class', 'hidden']});
    table.addEventListener('change', refresh);
    refresh();
  }
};
