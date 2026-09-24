#!/usr/bin/env python3
"""Build SilverSt.ZDL."""
import json, subprocess, sys
from pathlib import Path
HERE=Path(__file__).resolve().parent; ROOT=HERE.parent.parent.parent
sys.path[:0]=[str(ROOT/'build'),str(ROOT/'src'/'airwindows'/'common')]
from linker import LinkerConfig, link, params_from_manifest
from manifest_params import write_param_header
from custom_covers import make_cover
TI=Path('/Applications/ti/ti-cgt-c6000_8.5.0.LTS'); CL6X=TI/'bin'/'cl6x'
def main():
 m=json.loads((HERE/'manifest_pedal.json').read_text()); write_param_header(m,HERE/'silverst_params.h','SL')
 obj=HERE/'silverst.obj'; out=ROOT/'dist'/'SilverSt.ZDL'
 subprocess.run([str(CL6X),'--c99','--opt_level=2','-mv6740','--abi=eabi','--mem_model:data=far',f'--include_path={TI}/include','--define=SILVERST_AUDIO_FUNC='+m['audio_func_name'],'-k','--debug_software_pipeline','-c',str(HERE/'silverst.c'),f'--output_file={obj}'],check=True,cwd=HERE)
 for n in ('compiler.opt','linker.cmd'):
  p=HERE/n
  if p.exists(): p.unlink()
 link(LinkerConfig(effect_name=m['effect_name'],audio_func_name=m['audio_func_name'],screen_image=make_cover(m['effect_name'],[p['name'] for p in m['params']]),gid=m['gid'],fxid=m['fxid'],params=params_from_manifest(m['params']),obj_path=obj,output_path=out,fxid_version=b'1.00',flags_byte=1,materialize_init=True,knob_positions=[(2,14,46),(3,55,46),(4,96,46)],use_object_edit_handlers=False,synthesize_linesel_edit_handlers=True,synth_edit_start_index=2,knob3_blob_path='/tmp/__nonexistent__'))
 print(out)
if __name__=='__main__': main()
