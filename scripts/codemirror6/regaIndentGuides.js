import {Decoration, ViewPlugin} from "@codemirror/view";

function indentation(text, tabSize) {
  let columns = 0;
  for (const character of text) {
    if (character === " ") columns++;
    else if (character === "\t") columns += tabSize - columns % tabSize;
    else break;
  }
  return columns;
}

// Empty lines inherit only the indentation that surrounds them, so a guide
// remains continuous inside a block but stops when that block closes.
function lineLevels(doc, unit, tabSize) {
  const lines = doc.toString().split("\n");
  const levels = lines.map((line) => indentation(line, tabSize));
  const preceding = [];
  let depth = 0;
  lines.forEach((line, index) => {
    preceding[index] = depth;
    if (line.trim()) depth = levels[index] + (/\{\s*$/.test(line) ? unit : 0);
  });
  depth = 0;
  for (let index = lines.length - 1; index >= 0; index--) {
    if (lines[index].trim()) {
      depth = levels[index] + (/^\s*[}\])]/.test(lines[index]) ? unit : 0);
    } else {
      levels[index] = Math.max(levels[index], Math.min(preceding[index], depth));
    }
  }
  return levels;
}

function guideLayers(columns, unit) {
  const layers = [];
  for (let offset = 0; offset + unit <= columns && layers.length < 64; offset += unit) {
    const left = `calc(2px + ${offset}ch)`;
    const right = `calc(3px + ${offset}ch)`;
    layers.push(`linear-gradient(to right, transparent ${left}, var(--rega-indent-guide) ${left}, var(--rega-indent-guide) ${right}, transparent ${right})`);
  }
  return layers.join(",");
}

export function regaIndentGuides(options) {
  const unit = Math.max(1, options.indentUnit || 2);
  const tabSize = Math.max(1, options.tabSize || 2);

  function draw(view) {
    const levels = lineLevels(view.state.doc, unit, tabSize);
    const marks = [];
    const seen = new Set();
    for (const visible of view.visibleRanges) {
      let position = visible.from;
      while (position <= visible.to) {
        const line = view.state.doc.lineAt(position);
        if (!seen.has(line.from)) {
          seen.add(line.from);
          const layers = guideLayers(levels[line.number - 1], unit);
          if (layers) marks.push(Decoration.line({class: "cm-rega-indent-guide",
            attributes: {style: `--rega-indent-layers: ${layers}`}}).range(line.from));
        }
        if (line.to >= view.state.doc.length) break;
        position = line.to + 1;
      }
    }
    return Decoration.set(marks, true);
  }

  return ViewPlugin.fromClass(class {
    constructor(view) { this.decorations = draw(view); }
    update(update) {
      if (update.docChanged || update.viewportChanged || update.geometryChanged) {
        this.decorations = draw(update.view);
      }
    }
  }, {decorations: (plugin) => plugin.decorations});
}
