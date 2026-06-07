import json
import os
import re
import subprocess
import tempfile
import time
from pathlib import Path

repo = Path('/home/lancer1977/code/MilkDrop3-api-seams')
sender = repo / '.tmp_sender' / 'publish' / 'Sender.exe'
if not sender.exists():
    raise SystemExit(f'sender exe missing: {sender}')

logdir = Path(tempfile.mkdtemp(prefix='md3-run-'))
print(f'LOGDIR={logdir}')

xvfb = subprocess.Popen(['Xvfb', ':99', '-screen', '0', '1280x720x24'], stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
try:
    env = os.environ.copy()
    env['DISPLAY'] = ':99'
    env['WINEPREFIX'] = str(repo / '.wine-test')

    milkdrop = subprocess.Popen(
        ['timeout', '60s', str(repo / '.wine-deploy/run-milkdrop3-wine.sh')],
        cwd=str(repo),
        env=env,
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
        text=True,
    )

    found = False
    for _ in range(30):
        time.sleep(1)
        tree = subprocess.run(['xwininfo', '-root', '-tree'], env=env, capture_output=True, text=True)
        if 'MilkDrop 3.0.A - Press F1 for help' in tree.stdout or 'MILKDROP ERROR' in tree.stdout:
            found = True
            break
        if milkdrop.poll() is not None:
            break
    print(f'WINDOW_FOUND={int(found)}')

    def run_sender(payload: str):
        r = subprocess.run(
            ['wine', str(sender), 'MilkDrop', payload],
            cwd=str(repo),
            env=env,
            capture_output=True,
            text=True,
        )
        print('--- sender payload ---')
        print(payload.replace('\n', '\\n'))
        print(f'rc={r.returncode}')
        if r.stdout:
            print(r.stdout.strip())
        if r.stderr:
            print('stderr:')
            print(r.stderr.strip())
        return r

    def parse_reply(stdout: str):
        lines = [ln for ln in stdout.splitlines() if ln.startswith('reply=')]
        if not lines:
            return None
        text = lines[-1][len('reply='):]
        text = text.replace('\\r', '\r').replace('\\n', '\n')
        fields = {}
        for line in text.split('\n'):
            if '=' in line:
                k, v = line.split('=', 1)
                fields[k] = v
        return fields

    snapshot = run_sender('version=1\nrequest_id=42\ncommand=state_snapshot')
    snap_fields = parse_reply(snapshot.stdout)
    if not snap_fields:
        print('STATE_SNAPSHOT_REPLY=NONE')
        current_file = ''
        sprites = []
    else:
        detail = snap_fields.get('detail', '')
        print(f'STATE_SNAPSHOT_STATUS={snap_fields.get("status", "")})')
        print(f'STATE_SNAPSHOT_DETAIL={detail}')
        try:
            snap_json = json.loads(detail)
        except Exception as e:
            print(f'STATE_SNAPSHOT_JSON_ERROR={e}')
            snap_json = {}
        current_file = snap_json.get('current_file', '') or ''
        sprites = snap_json.get('sprites', []) or []
        print(f'CURRENT_FILE={current_file}')
        print(f'INITIAL_SPRITES={json.dumps(sprites)}')

    # Exercise commands.
    results = {}
    results['ping'] = run_sender('command=ping').returncode
    results['launch_sprite'] = run_sender('command=launch_sprite\nsprite=01\nslot=-1').returncode

    after_launch = run_sender('version=1\nrequest_id=43\ncommand=state_snapshot')
    after_fields = parse_reply(after_launch.stdout)
    launch_slot = None
    if after_fields and after_fields.get('detail'):
        try:
            after_json = json.loads(after_fields['detail'])
            sprites_after = after_json.get('sprites', []) or []
            print(f'AFTER_LAUNCH_SPRITES={json.dumps(sprites_after)}')
            for sp in sprites_after:
                if isinstance(sp, dict) and sp.get('active'):
                    launch_slot = sp.get('slot') if sp.get('slot') is not None else sp.get('index')
                    break
        except Exception as e:
            print(f'AFTER_LAUNCH_JSON_ERROR={e}')

    if launch_slot is None:
        launch_slot = 0
    print(f'KILL_SLOT={launch_slot}')
    results['kill_sprite'] = run_sender(f'command=kill_sprite\nslot={launch_slot}').returncode

    if current_file:
        results['load_preset'] = run_sender('command=load_preset\npath=' + current_file.replace('\\', '\\\\') + '\nblend=1.7').returncode
    else:
        results['load_preset'] = None
        print('LOAD_PRESET_SKIPPED=no current_file')

    results['random_preset'] = run_sender('command=random_preset\nblend=1.7').returncode
    results['invalid_command'] = run_sender('command=nope').returncode

    print('RESULTS=' + json.dumps(results, indent=2))

    tree = subprocess.run(['xwininfo', '-root', '-tree'], env=env, capture_output=True, text=True)
    print('--- xwininfo ---')
    print(tree.stdout)

    try:
        milkdrop.wait(timeout=5)
    except subprocess.TimeoutExpired:
        pass
    print(f'WINE_EXIT={milkdrop.returncode}')
    if milkdrop.stdout is not None:
        remaining = milkdrop.stdout.read()
        if remaining:
            print('--- wine output ---')
            print(remaining)
finally:
    xvfb.terminate()
    try:
        xvfb.wait(timeout=5)
    except subprocess.TimeoutExpired:
        xvfb.kill()
