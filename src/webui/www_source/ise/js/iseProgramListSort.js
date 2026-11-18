/** Sort the two program lists using their ReGa records and native row IDs. */
var iseProgramListSort = (function() {
  var columns = ['name', 'desc', 'id', 'savedAt'];
  var collator = new Intl.Collator(undefined, {numeric: true, sensitivity: 'base'});
  var selections = {};
  var groupNameDirection = 1;

  /** Compare raw IDs and timestamps without parsing the displayed text. */
  function compare(left, right, column, direction) {
    if (column === 'savedAt' && (left.savedAt <= 0 || right.savedAt <= 0)) {
      return Number(left.savedAt <= 0) - Number(right.savedAt <= 0);
    }
    if (column === 'id' || column === 'savedAt') {
      return direction * (Number(left[column]) - Number(right[column]));
    }
    return direction * collator.compare(left[column], right[column]);
  }

  /** Keep native headers and group boundaries in place while moving program rows. */
  function orderRows(table, programs, column, direction) {
    var byRow = new Map();
    programs.forEach(function(program) {
      var row = document.getElementById(program.trid);
      if (row && row.closest('table') === table) { byRow.set(row, program); }
    });
    Array.from(table.tBodies).forEach(function(body) {
      var run = [];
      function insertSortedRows(before) {
        run.sort(function(left, right) {
          return compare(byRow.get(left), byRow.get(right), column, direction);
        });
        run.forEach(function(row) { body.insertBefore(row, before); });
        run = [];
      }
      Array.from(body.rows).forEach(function(row) {
        if (byRow.has(row)) { run.push(row); }
        else { insertSortedRows(row); }
      });
      insertSortedRows(null);
    });
  }

  /** Connect the sortable headers as part of the native list render. */
  function bind(table, view, programs) {
    var buttons = Array.from(table.querySelectorAll('button[data-program-sort]'));
    var state = selections[view] || {column: 'name', direction: 1};

    function showDirection() {
      buttons.forEach(function(button) {
        var selected = button.getAttribute('data-program-sort') === state.column;
        var cell = button.parentNode;
        if (selected) {
          cell.setAttribute('aria-sort', state.direction === 1 ? 'ascending' : 'descending');
        } else {
          cell.removeAttribute('aria-sort');
        }
        button.querySelector('[aria-hidden]').textContent =
          selected ? (state.direction === 1 ? '\u2191' : '\u2193') : '\u2195';
      });
    }

    buttons.forEach(function(button) {
      var column = button.getAttribute('data-program-sort');
      if (columns.indexOf(column) === -1) { return; }
      button.onclick = function() {
        var direction = state.column === column ? -state.direction : 1;
        var grouped = column === 'name' && typeof ProgramGroups !== 'undefined' &&
          ProgramGroups.sort(view);
        if (grouped) {
          // The group controller retains its own name order when switching columns.
          groupNameDirection = -groupNameDirection;
          direction = groupNameDirection;
        } else {
          orderRows(table, programs, column, direction);
        }
        state = {column: column, direction: direction, grouped: grouped};
        selections[view] = state;
        showDirection();
      };
    });
    if (!state.grouped) { orderRows(table, programs, state.column, state.direction); }
    showDirection();
  }

  return {bind: bind};
})();
