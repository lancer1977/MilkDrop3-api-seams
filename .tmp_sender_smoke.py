import json
import os
import subprocess
import tempfile
import textwrap
import time
from pathlib import Path

repo = Path('/home/lancer1977/code/MilkDrop3-api-seams')
work = Path(tempfile.mkdtemp(prefix='md3sender-'))
proj = work / 'Sender'
proj.mkdir()

(proj / 'Sender.csproj').write_text(textwrap.dedent('''
<Project Sdk="Microsoft.NET.Sdk">
  <PropertyGroup>
    <OutputType>Exe</OutputType>
    <TargetFramework>net10.0-windows</TargetFramework>
    <RuntimeIdentifier>win-x64</RuntimeIdentifier>
    <ImplicitUsings>enable</ImplicitUsings>
    <Nullable>enable</Nullable>
    <UseAppHost>true</UseAppHost>
    <SelfContained>true</SelfContained>
  </PropertyGroup>
</Project>
''').strip() + '\n')

(proj / 'Program.cs').write_text(textwrap.dedent('''
using System;
using System.Diagnostics;
using System.Runtime.InteropServices;
using System.Text;

class Program
{
    private const int WM_COPYDATA = 0x004A;

    [StructLayout(LayoutKind.Sequential)]
    private struct COPYDATASTRUCT
    {
        public IntPtr dwData;
        public int cbData;
        public IntPtr lpData;
    }

    [DllImport("user32.dll", CharSet = CharSet.Unicode, SetLastError = true)]
    private static extern IntPtr SendMessageTimeout(
        IntPtr hWnd,
        int Msg,
        IntPtr wParam,
        ref COPYDATASTRUCT lParam,
        int fuFlags,
        int uTimeout,
        out IntPtr lpdwResult);

    private static IntPtr FindWindowByTitleFragment(string titleFragment)
    {
        foreach (var p in Process.GetProcesses())
        {
            try
            {
                if (p.MainWindowHandle != IntPtr.Zero &&
                    !string.IsNullOrEmpty(p.MainWindowTitle) &&
                    p.MainWindowTitle.Contains(titleFragment, StringComparison.OrdinalIgnoreCase))
                {
                    return p.MainWindowHandle;
                }
            }
            catch
            {
                // Ignore access-denied or zombie processes.
            }
        }

        return IntPtr.Zero;
    }

    private static int Send(string titleFragment, string payload)
    {
        payload = payload.Replace("\\n", "\n");

        var hwnd = FindWindowByTitleFragment(titleFragment);
        if (hwnd == IntPtr.Zero)
        {
            Console.WriteLine($"not_found title_fragment={titleFragment}");
            return 3;
        }

        var bytes = Encoding.Unicode.GetBytes(payload + "\0");
        var buffer = Marshal.AllocHGlobal(bytes.Length);
        try
        {
            Marshal.Copy(bytes, 0, buffer, bytes.Length);
            var cds = new COPYDATASTRUCT
            {
                dwData = IntPtr.Zero,
                cbData = bytes.Length,
                lpData = buffer,
            };

            IntPtr result;
            var sendResult = SendMessageTimeout(hwnd, WM_COPYDATA, IntPtr.Zero, ref cds, 0x2, 5000, out result);
            if (sendResult == IntPtr.Zero)
            {
                Console.WriteLine($"timeout payload={payload.Replace("\n", "\\n")}");
                return 4;
            }

            Console.WriteLine(result != IntPtr.Zero
                ? $"accepted payload={payload.Replace("\n", "\\n")}" 
                : $"rejected payload={payload.Replace("\n", "\\n")}");
            return result != IntPtr.Zero ? 0 : 2;
        }
        finally
        {
            Marshal.FreeHGlobal(buffer);
        }
    }

    private static int Main(string[] args)
    {
        if (args.Length < 2)
        {
            Console.Error.WriteLine("usage: Sender <title-fragment> <payload>");
            return 1;
        }

        return Send(args[0], args[1]);
    }
}
''').strip() + '\n')

publish = subprocess.run(
    ['dotnet', 'publish', str(proj / 'Sender.csproj'), '-c', 'Release', '-o', str(work / 'publish')],
    capture_output=True,
    text=True,
)
print('PUBLISH_EXIT=', publish.returncode)
print('--- publish stdout ---')
print(publish.stdout)
print('--- publish stderr ---')
print(publish.stderr)
if publish.returncode != 0:
    raise SystemExit(publish.returncode)

published = sorted(p.name for p in (work / 'publish').iterdir())
print('PUBLISHED_FILES=', published)

xvfb = subprocess.Popen(['Xvfb', ':99', '-screen', '0', '1280x720x24'], stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
try:
    env = os.environ.copy()
    env['DISPLAY'] = ':99'
    env['WINEPREFIX'] = str(repo / '.wine-test')

    milkdrop = subprocess.Popen(
        ['timeout', '25s', str(repo / '.wine-deploy/run-milkdrop3-wine.sh')],
        cwd=str(repo),
        env=env,
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
        text=True,
    )

    found = False
    for _ in range(20):
        time.sleep(1)
        tree = subprocess.run(['xwininfo', '-root', '-tree'], env=env, capture_output=True, text=True)
        if 'MilkDrop' in tree.stdout:
            found = True
            break
        if milkdrop.poll() is not None:
            break

    print('WINDOW_FOUND=', found)

    payloads = [
        'command=ping',
        'command=launch_sprite\\nsprite=01\\nslot=-1',
        'command=kill_sprite\\nslot=0',
        'command=load_preset\\npath=C:\\\\MilkDrop3\\\\presets\\\\Example.milk\\nblend=1.7',
        'command=random_preset\\nblend=1.7',
    ]
    results = []
    sender = work / 'publish' / 'Sender.exe'
    for payload in payloads:
        r = subprocess.run(
            ['wine', str(sender), 'MilkDrop', payload],
            cwd=str(repo),
            env=env,
            capture_output=True,
            text=True,
        )
        results.append({
            'payload': payload,
            'returncode': r.returncode,
            'stdout': r.stdout.strip(),
            'stderr': r.stderr.strip(),
        })
    print('SEND_RESULTS=', json.dumps(results, indent=2))

    tree = subprocess.run(['xwininfo', '-root', '-tree'], env=env, capture_output=True, text=True)
    print('--- xwininfo ---')
    print(tree.stdout)

    try:
        milkdrop.wait(timeout=5)
    except subprocess.TimeoutExpired:
        pass
    print('WINE_EXIT=', milkdrop.returncode)
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
