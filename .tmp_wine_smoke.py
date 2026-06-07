import os
import subprocess
import tempfile
import time
from pathlib import Path

repo = Path('/home/lancer1977/code/MilkDrop3-api-seams')
logdir = Path(tempfile.mkdtemp(prefix='md3-smoke-'))

xvfb = subprocess.Popen(['Xvfb', ':99', '-screen', '0', '1280x720x24'], stdout=open(logdir/'xvfb.log', 'w'), stderr=subprocess.STDOUT)
try:
    env = os.environ.copy()
    env['DISPLAY'] = ':99'
    env['WINEPREFIX'] = str(repo / '.wine-test')
    wine_log = open(logdir/'wine.log', 'w')
    proc = subprocess.Popen(['timeout', '25s', str(repo/'.wine-deploy/run-milkdrop3-wine.sh')], cwd=str(repo), env=env, stdout=wine_log, stderr=subprocess.STDOUT)
    found = 0
    for _ in range(20):
        time.sleep(1)
        tree = subprocess.run(['xwininfo', '-root', '-tree'], capture_output=True, text=True, env=env)
        if 'MilkDrop' in tree.stdout:
            found = 1
            break
        if proc.poll() is not None:
            break
    rc = proc.wait()
    wine_log.close()
    wine_exit = None
    exit_file = logdir/'wine.exit'
    if exit_file.exists():
        wine_exit = exit_file.read_text().strip()
    print(f'LOGDIR={logdir}')
    print(f'WINDOW_FOUND={found}')
    print(f'WINE_RC={rc}')
    print('--- wine.log ---')
    print((logdir/'wine.log').read_text(errors='replace'))
    print('--- xwininfo ---')
    tree = subprocess.run(['xwininfo', '-root', '-tree'], capture_output=True, text=True, env=env)
    print(tree.stdout)
finally:
    xvfb.terminate()
    try:
        xvfb.wait(timeout=5)
    except subprocess.TimeoutExpired:
        xvfb.kill()
