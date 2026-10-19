#!/usr/bin/env python3
"""Inspect version, public identity and exact exported files. Does not publish."""
import argparse, hashlib, json, subprocess, tarfile, zipfile
from pathlib import Path, PurePosixPath
ROOT=Path(__file__).resolve().parents[1]
def package_files(path):
    if zipfile.is_zipfile(path):
        with zipfile.ZipFile(path) as a: return {x.filename:a.read(x) for x in a.infolist() if not x.is_dir()}
    with tarfile.open(path) as a:
        result={}
        for x in a.getmembers():
            if x.issym() or x.islnk(): raise ValueError('Archive links forbidden')
            if x.isfile(): result[x.name]=a.extractfile(x).read()
        return result
def normalised_files(files):
    result={}
    for name,data in files.items():
        name=name.removeprefix('./');p=PurePosixPath(name)
        if p.is_absolute() or '..' in p.parts or '\\' in name or ':' in name: raise ValueError('Unsafe archive path')
        result[name]=data
    if 'library.json' not in result:
        if len({PurePosixPath(n).parts[0] for n in result})!=1: raise ValueError('Ambiguous archive root')
        result={n.split('/',1)[1]:d for n,d in result.items() if '/' in n}
    return result

def validate_review_contract():
    import re
    manifest=json.loads((ROOT/'library.json').read_text())
    header=(ROOT/'src'/manifest['headers'][0]).read_text()
    versions=re.findall(r'^#define\s+SUPERJUPITER_\w+_VERSION\s+"([^"]+)"',header,re.M)
    assert versions==[manifest['version']], 'Public version macro mismatch'
    assert all(a['name']=='SuperJupiter' for a in manifest['authors'])
    assert manifest['license']=='MIT'
    assert 'hardware validation pending' in (ROOT/'README.md').read_text().lower()
    assert (ROOT/'docs/INTEROPERABILITY.md').is_file()
    assert (ROOT/'tests/review_regressions.cpp').is_file()

def main():
    validate_review_contract()
    p=argparse.ArgumentParser();p.add_argument('--tag');p.add_argument('--archive',type=Path);p.add_argument('--require-clean',action='store_true');a=p.parse_args()
    m=json.loads((ROOT/'library.json').read_text());name=m['name'];v=m['version']
    props=dict(x.split('=',1) for x in (ROOT/'library.properties').read_text().splitlines() if '=' in x)
    assert name==props['name'] and v==props['version'] and props['author']=='SuperJupiter'
    assert 'hardware validation pending' in m['description'].lower() and 'hardware validation pending' in props['paragraph'].lower()
    assert 'VERSION '+v+' LANGUAGES' in (ROOT/'CMakeLists.txt').read_text()
    assert '"'+v+'"' in (ROOT/'src'/m['headers'][0]).read_text()
    assert '@'+v in (ROOT/'README.md').read_text()
    if a.tag: assert a.tag=='v'+v
    if a.require_clean: assert not subprocess.check_output(['git','status','--porcelain'],cwd=ROOT,text=True).strip()
    if a.archive:
        files=normalised_files(package_files(a.archive))
        required=['tests/review_regressions.cpp','docs/INTEROPERABILITY.md','library.json','library.properties','README.md','LICENSE','CMakeLists.txt',
            'src/'+m['headers'][0],'docs/API.md','docs/CONVENTIONS.md','docs/VALIDATION.md',
            'docs/MIGRATION.md','examples/Minimal/Minimal.ino','tests/test.cpp','tests/consumer/main.cpp',
            'tests/consumer/CMakeLists.txt','tests/namespace_isolation.cpp','tools/check_release.py']
        for file in required: assert file in files,'Missing '+file
        for file,data in files.items():
            assert not file.startswith(('build/','.git/','.pio/')),file
            assert (ROOT/file).is_file(),'Unexpected '+file
            assert data.replace(b'\r\n',b'\n')==(ROOT/file).read_bytes().replace(b'\r\n',b'\n'),'Mismatch '+file
        print('Archive SHA256 '+hashlib.sha256(a.archive.read_bytes()).hexdigest())
    print('PASS '+name+' '+v)
if __name__=='__main__': main()
