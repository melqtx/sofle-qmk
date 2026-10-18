"""Check the saved layout against the compiled firmware and validate the UF2."""
import hashlib,json,re,struct
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
def symbol(path,name):
 b=path.read_bytes(); off=struct.unpack_from('<I',b,32)[0]; es,n=struct.unpack_from('<HH',b,46)
 secs=[struct.unpack_from('<10I',b,off+i*es) for i in range(n)]
 for sec in secs:
  if sec[1]!=2:continue
  strs=secs[sec[6]]; names=b[strs[4]:strs[4]+strs[5]]
  for p in range(sec[4],sec[4]+sec[5],sec[9]):
   no,val,size,info,other,idx=struct.unpack_from('<IIIBBH',b,p)
   if names[no:].split(b'\0')[0].decode()==name:
    seg=secs[idx]; start=seg[4]+val-seg[3]; return b[start:start+size]
 raise ValueError(name)
header=(ROOT/'vial-qmk/quantum/keycodes.h').read_text()
values={}
pairs=re.findall(r'^\s*(\w+)\s*=\s*(0x[0-9A-Fa-f]+|\w+)\s*,',header,re.M)
for _ in range(4):
 for name,value in pairs:
  if value.startswith('0x'):values[name]=int(value,16)
  elif value in values:values[name]=values[value]
legacy={'KC_RCTRL':'KC_RCTL','KC_LCTRL':'KC_LCTL','KC_LSHIFT':'KC_LSFT','KC_RSHIFT':'KC_RSFT','KC_BSPACE':'KC_BSPC','KC_SCOLON':'KC_SCLN','KC_BSLASH':'KC_BSLS','KC_LBRACKET':'KC_LBRC','KC_RBRACKET':'KC_RBRC','KC_PGDOWN':'KC_PGDN','KC_WH_D':'MS_WHLD','KC_WH_U':'MS_WHLU','KC_VOLD':'KC_AUDIO_VOL_DOWN','KC_VOLU':'KC_AUDIO_VOL_UP'}
def number(code):
 if isinstance(code,int):return code
 if code.startswith('0x'):return int(code,16)
 m=re.fullmatch(r'(\w+)\((.+)\)',code)
 if m:
  fn,arg=m.groups()
  if fn=='MO':return values['QK_MOMENTARY']|int(arg)
  return {'LSFT':0x200,'LGUI':0x800,'SGUI':0xa00}[fn]|number(arg)
 return values[legacy.get(code,code)]
def verify():
 export=json.loads((ROOT/'layout.vil').read_text())
 saved=[number(k) for layer in export['layout'] for row in layer for k in row]
 elf=ROOT/'vial-qmk/.build/sofle_rev1_vial_rp2040_ce.elf'
 raw=symbol(elf,'keymaps'); actual=list(struct.unpack('<180H',raw))
 assert actual==saved,'layout.vil differs from compiled keymap'
 eraw=symbol(elf,'encoder_map'); enc=list(struct.unpack('<12H',eraw))
 # Firmware stores CW,CCW; the Vial backup and readback use CCW,CW.
 enc_ccw_cw=[v for i in range(0,len(enc),2) for v in (enc[i+1],enc[i])]
 eexpected=[number(k) for layer in export['encoder_layout'] for knob in layer for k in knob]
 assert enc_ccw_cw==eexpected,'encoder mismatch'
 assert len(export['layout'])==len(export['encoder_layout'])==3
 config=(ROOT/'keymap/config.h').read_text();assert '#define DYNAMIC_KEYMAP_LAYER_COUNT 3' in config
 assert actual[4*6:4*6+5]==[0xe0,0xe3,0x2a,0x4100,0xe1]
 assert actual[9*6:9*6+5]==[0xe7,0x5222,0x28,0xe4,0x2c]
 assert actual[2*6+3]==0x2807 and actual[7*6+3]==0x380e
 uf2=ROOT/'dist/sofle-v2.uf2';b=uf2.read_bytes();assert len(b)%512==0
 for i in range(len(b)//512):
  h=struct.unpack_from('<8I',b,i*512)
  assert h[0:2]==(0x0A324655,0x9E5D5157)
  assert h[2]&0x2000 and h[3]==0x10000000+i*256 and h[4]==256
  assert h[5]==i and h[6]==len(b)//512 and h[7]==0xE48BFF56
  assert struct.unpack_from('<I',b,i*512+508)[0]==0x0AB16F30
 version=(ROOT/'vial-qmk/.build/obj_sofle_rev1_vial_rp2040_ce/src/version.h').read_text()
 build_id=re.search(r'#define BUILD_ID .*?(0x[0-9A-F]+)',version)[1]
 manifest={'installed':False,'file':uf2.name,'sha256':hashlib.sha256(b).hexdigest(),'size':len(b),'layers':3,'key_bindings_verified':180,'encoder_directions_verified':12,'vial_uid':export['uid'],'build_id':build_id,'source_commit':(ROOT/'vial-source-commit.txt').read_text().strip()}
 return manifest,actual,enc_ccw_cw
if __name__=='__main__':
 m,k,e=verify()
 previous=ROOT/'dist/manifest.json'
 if previous.exists():
  old=json.loads(previous.read_text())
  if old.get('sha256')==m['sha256']:
   for field in ('installed',):
    if field in old:m[field]=old[field]
 (ROOT/'dist/manifest.json').write_text(json.dumps(m,indent=2)+'\n')
 (ROOT/'dist/SHA256SUMS').write_text(m['sha256']+'  '+m['file']+'\n')
 print(json.dumps(m,indent=2))
