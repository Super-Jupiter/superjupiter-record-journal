#!/usr/bin/env python3
"""Choose submit/update from the actual official Arduino index; fail closed offline."""
import gzip
import json
from pathlib import Path
import urllib.request

name = json.loads((Path(__file__).resolve().parents[1] / 'library.json').read_text())['name']
with urllib.request.urlopen('https://downloads.arduino.cc/libraries/library_index.json.gz', timeout=60) as response:
    index = json.loads(gzip.decompress(response.read()))
print('update' if any(entry['name'] == name for entry in index['libraries']) else 'submit')
