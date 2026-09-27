# CLion Cheat Sheet (Windows default keymap)

> These shortcuts assume the default **Windows** keymap. If one doesn't work, press `Ctrl+Shift+A` (Find Action), type the action name, and it shows the key bound to it. You can also open **Help → Keyboard Shortcuts PDF**.

---

## 📂 Code Folding (collapse / expand)

| Action | Shortcut | No-numpad alternative |
|---|---|---|
| Collapse the current block | `Ctrl+NumPad -` | `Ctrl+-` |
| Expand the current block | `Ctrl+NumPad +` | `Ctrl+=` |
| **Collapse all** in the file | `Ctrl+Shift+NumPad -` | `Ctrl+Shift+-` |
| **Expand all** in the file | `Ctrl+Shift+NumPad +` | `Ctrl+Shift+=` |
| Collapse recursively (block + nested) | `Ctrl+Alt+NumPad -` | — |
| Expand recursively | `Ctrl+Alt+NumPad +` | — |
| Expand to level N (current block) | `Ctrl+NumPad *`, then `1`–`5` | — |
| Expand all to level N (e.g. `1` shows only function signatures) | `Ctrl+Shift+NumPad *`, then `1`–`5` | — |
| Collapse the enclosing `{ }` block | `Ctrl+Shift+.` | — |
| **Fold selection** (custom fold of any selected lines) / remove a custom fold | `Ctrl+.` | — |
| Toggle a fold with the mouse | Click the ⊖/⊕ arrow in the gutter | — |

### What CLion can fold on its own
- Function bodies and `{}` blocks (including `if` / `for` / `switch` bodies)
- struct / enum / union bodies
- Multiline comments and doc comments
- `#include` groups
- `#if` / `#ifdef` preprocessor blocks

### Multiline declarations
A function signature or declaration that spans several lines (a long parameter list or initializer, for example) can't be folded by default. To fold one, select the lines and press **`Ctrl+.`**.

### Named regions
Named regions fold as one unit, and they persist in the source:

```c
// region Lexer helpers
...
// endregion
```

`#pragma region Name` / `#pragma endregion` works too. To wrap a selection in a region, use `Ctrl+Alt+T` (Surround With) and pick *region…endregion*.

### Folding defaults
Under **Settings → Editor → General → Code Folding** you can choose what starts collapsed when a file opens (for example, fold `#include`s and doc comments).

---

## 🧭 Navigation

| Action | Shortcut |
|---|---|
| Search everywhere | `Shift` `Shift` |
| Find action / setting | `Ctrl+Shift+A` |
| Go to file / symbol | `Ctrl+Shift+N` / `Ctrl+Alt+Shift+N` |
| Go to declaration | `Ctrl+B` or `Ctrl+Click` |
| Go to implementation | `Ctrl+Alt+B` |
| **Switch header ↔ source** | `Ctrl+Alt+Home` |
| Find usages | `Alt+F7` |
| File structure popup (all functions in the file) | `Ctrl+F12` |
| Recent files | `Ctrl+E` |
| Back / forward | `Ctrl+Alt+←` / `Ctrl+Alt+→` |
| Go to line | `Ctrl+G` |
| Previous / next function | `Alt+↑` / `Alt+↓` |
| Jump to matching brace | `Ctrl+Shift+M` |
| Block start / end | `Ctrl+[` / `Ctrl+]` |
| Next error / warning | `F2` / `Shift+F2` |

---

## ✏️ Editing

| Action | Shortcut |
|---|---|
| Quick fix / context actions | `Alt+Enter` |
| Duplicate line | `Ctrl+D` |
| Delete line | `Ctrl+Y` |
| Line comment / block comment | `Ctrl+/` / `Ctrl+Shift+/` |
| Expand / shrink selection | `Ctrl+W` / `Ctrl+Shift+W` |
| Add next occurrence to selection (multi-caret) | `Alt+J` (`Alt+Shift+J` removes the last one) |
| Select all occurrences | `Ctrl+Alt+Shift+J` |
| Add a caret with the mouse | `Alt+Shift+Click` |
| Move line up / down | `Alt+Shift+↑` / `Alt+Shift+↓` |
| Move statement (e.g. a whole `case` or function) up / down | `Ctrl+Shift+↑` / `Ctrl+Shift+↓` |
| Reformat code | `Ctrl+Alt+L` |
| Complete statement (adds `;` or `{}`) | `Ctrl+Shift+Enter` |
| Surround with (`if`, `while`, region…) | `Ctrl+Alt+T` |
| Parameter info | `Ctrl+P` |
| Quick documentation | `Ctrl+Q` |
| Quick definition | `Ctrl+Shift+I` |

---

## 🔧 Refactoring

| Action | Shortcut |
|---|---|
| Rename | `Shift+F6` |
| Extract variable | `Ctrl+Alt+V` |
| Extract function | `Ctrl+Alt+M` |
| Extract constant | `Ctrl+Alt+C` |
| Inline | `Ctrl+Alt+N` |
| Change signature | `Ctrl+F6` |
| Refactor This menu | `Ctrl+Alt+Shift+T` |

---

## ▶️ Build, Run, Debug

| Action | Shortcut |
|---|---|
| Build | `Ctrl+F9` |
| Run / Debug | `Shift+F10` / `Shift+F9` |
| Choose a run configuration | `Alt+Shift+F10` |
| Stop | `Ctrl+F2` |
| Toggle breakpoint | `Ctrl+F8` |
| View all breakpoints | `Ctrl+Shift+F8` |
| Step over / into / out | `F8` / `F7` / `Shift+F8` |
| Run to cursor | `Alt+F9` |
| Resume | `F9` |
| Evaluate expression | `Alt+F8` |

> **Compiler-project tip:** the debugger has a **Disassembly** view and a **Memory** view (right-click a pointer and choose *Show in Memory View*). You'll likely want both when you debug the assembly your compiler generates.

---

## 🪟 Windows & Search

| Action | Shortcut |
|---|---|
| Project tool window | `Alt+1` |
| Run tool window | `Alt+4` |
| Debug tool window | `Alt+5` |
| Terminal | `Alt+F12` |
| Maximize the editor (hide all panels) | `Ctrl+Shift+F12` |
| Return to the editor | `Esc` |
| Hide the active tool window | `Shift+Esc` |
| Find / replace in the file | `Ctrl+F` / `Ctrl+R` |
| Find / replace in all files | `Ctrl+Shift+F` / `Ctrl+Shift+R` |
| Settings | `Ctrl+Alt+S` |
