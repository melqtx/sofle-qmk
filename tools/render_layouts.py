"""Render the saved Vial layout using QMK's Sofle switch coordinates."""
import html
import hashlib
import re
import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
LAYOUT = json.loads((ROOT / 'layout.vil').read_text())['layout']
POSITIONS = json.loads((ROOT / 'vial-qmk/keyboards/sofle/rev1/keyboard.json').read_text())['layouts']['LAYOUT']['layout']
NAMES = {
    'KC_ESCAPE': 'Esc', 'KC_TAB': 'Tab', 'KC_LCTRL': 'Ctrl', 'KC_RCTRL': 'Ctrl',
    'KC_LSHIFT': 'Shift', 'KC_RSHIFT': 'Shift', 'KC_LALT': 'Opt',
    'KC_LGUI': 'Cmd', 'KC_RGUI': 'Cmd', 'KC_ENTER': 'Enter',
    'KC_SPACE': 'Space', 'KC_BSPACE': 'Bksp', 'KC_DELETE': 'Del',
    'KC_GRAVE': '`', 'KC_QUOTE': "'", 'KC_SCOLON': 'O',
    'KC_LBRACKET': '[', 'KC_RBRACKET': ']', 'KC_BSLASH': '\\',
    'KC_SLASH': '/', 'KC_DOT': '.', 'KC_COMMA': ',',
    'KC_MINUS': '-', 'KC_EQUAL': '=', 'KC_PGUP': 'PgUp',
    'KC_PGDOWN': 'PgDn', 'KC_HOME': 'Home', 'KC_END': 'End',
    'KC_UP': '↑', 'KC_DOWN': '↓', 'KC_LEFT': '←', 'KC_RIGHT': '→',
    'KC_NO': '—', 'KC_MUTE': 'Mute', 'KC_MPLY': 'Play',
    'QK_REPEAT_KEY': 'Repeat', '0x2807': 'S / Cmd', '0x380e': 'E / Cmd', '0x4100': 'Rep / SYM', '0x422a': 'Bksp/NAV', 'MO(1)': 'SYM', 'MO(2)': 'NAV',
    'LGUI(KC_Z)': 'Undo', 'LGUI(KC_X)': 'Cut', 'LGUI(KC_C)': 'Copy',
    'LGUI(KC_V)': 'Paste', 'SGUI(KC_Z)': 'Redo',
}
for raw, typed in zip('QWERTYUIOPASDFGHJKL;ZXCVBNM', 'QWFPGJLUY;ARSTDHNEIOZXCVBKM'):
    NAMES['KC_' + raw if raw != ';' else 'KC_SCOLON'] = typed
DUAL = {'0x2807': ('tap S', 'hold Cmd'), '0x380e': ('tap E', 'hold Cmd'),
        '0x4100': ('tap Repeat', 'hold SYM'), '0x422a': ('tap Bksp', 'hold NAV')}
SHIFT = dict(zip(['KC_' + str(i) for i in range(1, 10)] + ['KC_0'], '!@#$%^&*()'))
SHIFT.update({'KC_EQUAL': '+', 'KC_MINUS': '_', 'KC_GRAVE': '~',
              'KC_SLASH': '?', 'KC_COMMA': '<', 'KC_DOT': '>',
              'KC_BSLASH': '|', 'KC_LBRACKET': '{', 'KC_RBRACKET': '}',
              'KC_P': ':', 'KC_QUOTE': '"'})

def label(code):
    if code.startswith('LSFT('):
        return SHIFT[code[5:-1]]
    return NAMES.get(code, code.removeprefix('KC_'))

def render(index, name, instruction):
    unit, pad, top = 60, 32, 76
    svg = [f'<svg xmlns="http://www.w3.org/2000/svg" width="1056" height="470" viewBox="0 0 1056 470" role="img" aria-labelledby="title desc">',
           f'<title id="title">Sofle V2 — {name}</title>',
           '<desc id="desc">Firmware layout, labeled as typed through macOS Colemak. Faded keys inherit Base.</desc>',
           '<rect width="1056" height="470" rx="12" fill="#15171c"/>',
           '<g font-family="-apple-system, BlinkMacSystemFont, Segoe UI, sans-serif" text-anchor="middle">',
           f'<text x="528" y="30" fill="#f1f3f7" font-size="22" font-weight="500">{name}</text>',
           f'<text x="528" y="54" fill="#bfc5cf" font-size="15">{html.escape(instruction)}</text>']
    for position in POSITIONS:
        row, col = position['matrix']
        raw = LAYOUT[index][row][col]
        inherited = raw == 'KC_TRNS'
        code = LAYOUT[0][row][col] if inherited else raw
        x, y = pad + position['x'] * unit, top + position['y'] * unit
        fill = '#2c3039'
        if code.startswith('MO(') or code in ('0x4100', '0x422a'): fill = '#285577'
        elif code in ('QK_REPEAT_KEY', '0x2807', '0x380e'): fill = '#624d78'
        opacity = '0.52' if inherited else '1'
        svg.append(f'<g data-matrix="{row},{col}" data-keycode="{html.escape(code, quote=True)}" opacity="{opacity}">')
        svg.append(f'<title>{html.escape(code)}{" — inherited from Base" if inherited else ""}</title>')
        if col == 5 and row in (4, 9):
            svg.append(f'<circle cx="{x+27}" cy="{y+27}" r="25" fill="#343945" stroke="#454c59"/>')
            text = 'Vol' if row == 4 else 'Scroll'
            cy = y + 27
        else:
            height = position.get('h', 1) * unit - 6
            svg.append(f'<rect x="{x}" y="{y}" width="54" height="{height}" rx="6" fill="{fill}" stroke="#424854"/>')
            text, cy = label(code), y + height / 2
        if code in DUAL:
            tap, hold = DUAL[code]
            svg.append(f'<text x="{x+27}" y="{cy-8}" dominant-baseline="central" fill="#f4f6fa" font-size="10.5">{tap}</text>')
            svg.append(f'<text x="{x+27}" y="{cy+9}" dominant-baseline="central" fill="#d6dfeb" font-size="10.5">{hold}</text></g>')
        else:
            svg.append(f'<text x="{x+27}" y="{cy}" dominant-baseline="central" fill="#f4f6fa" font-size="{12 if len(text) > 7 else 15 if len(text) > 5 else 17}">{html.escape(text)}</text></g>')
    svg.append('</g></svg>')
    return '\n'.join(svg) + '\n'

if __name__ == '__main__':
    output = ROOT / 'docs/images'
    output.mkdir(parents=True, exist_ok=True)
    images = []
    for index, stem, name, instruction in [
        (0, 'base', 'Base', 'Left: Cmd · Ctrl · Bksp · Repeat/SYM · Shift    |    Right: Space · Enter · Bksp/NAV · Ctrl · Cmd'),
        (1, 'symbols', 'Symbols', 'Hold left SYM; release to return'),
        (2, 'navigation', 'Navigation / numbers / F-keys', 'Tap right Bksp/NAV to delete; hold for navigation; release to return'),
    ]:
        content = render(index, name, instruction)
        filename = f'{stem}-{hashlib.sha256(content.encode()).hexdigest()[:8]}.svg'
        (output / filename).write_text(content)
        images.append((name, filename))
    (ROOT / 'README.md').write_text('\n\n'.join(
        f'![{name}](docs/images/{filename})' for name, filename in images) + '\n')
    current = {filename for _, filename in images}
    for previous in output.glob('*.svg'):
        if previous.name not in current and re.fullmatch(r'(base|symbols|navigation)(-[0-9a-f]{8})?\.svg', previous.name):
            previous.unlink()
    assert len(POSITIONS) == 60
    print('Generated Base, Symbols and Navigation images from layout.vil.')
