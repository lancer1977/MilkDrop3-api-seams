<#
.SYNOPSIS
  Send a UTF-16 WM_COPYDATA payload to a running MilkDrop3 window.

.DESCRIPTION
  This tiny local-only smoke sender finds a top-level window by a title fragment,
  wraps the payload in the v1 envelope by default, and sends the message with
  SendMessageTimeout so it can be used from local automation or manual smoke tests.

.PARAMETER WindowTitle
  Title fragment used to find the MilkDrop3 top-level window. Defaults to "MilkDrop".

.PARAMETER Payload
  Newline-separated key=value payload. Use commands such as launch_sprite,
  kill_sprite, load_preset, random_preset, ping, or related live sprite commands.

.PARAMETER TimeoutMs
  Timeout for SendMessageTimeout in milliseconds.

.PARAMETER Legacy
  Disable the v1 envelope wrapper and send the raw payload as-is.

.PARAMETER RequestId
  Optional request_id to use when the v1 envelope is active.

.EXAMPLE
  ./tools/send-milkdrop-copydata.ps1 -WindowTitle MilkDrop -Payload "command=ping"

.EXAMPLE
  ./tools/send-milkdrop-copydata.ps1 -WindowTitle MilkDrop -Payload "command=launch_sprite`nsprite=01`nslot=-1"

.EXAMPLE
  ./tools/send-milkdrop-copydata.ps1 -WindowTitle MilkDrop -Payload "command=load_preset`npath=C:\\MilkDrop3\\presets\\Example.milk`nblend=1.7"
#>

param(
    [string]$WindowTitle = "MilkDrop",
    [Parameter(Mandatory=$true)]
    [string]$Payload,
    [int]$TimeoutMs = 5000,
    [switch]$Legacy,
    [string]$RequestId
)

$signature = @"
using System;
using System.Diagnostics;
using System.Runtime.InteropServices;
using System.Text;

public static class MilkDropCopyDataSender
{
    public const int WM_COPYDATA = 0x004A;

    [StructLayout(LayoutKind.Sequential)]
    public struct COPYDATASTRUCT
    {
        public IntPtr dwData;
        public int cbData;
        public IntPtr lpData;
    }

    [DllImport("user32.dll", CharSet = CharSet.Unicode, SetLastError = true)]
    public static extern IntPtr SendMessageTimeout(
        IntPtr hWnd,
        int Msg,
        IntPtr wParam,
        ref COPYDATASTRUCT lParam,
        int fuFlags,
        int uTimeout,
        out IntPtr lpdwResult);

    public static IntPtr FindWindowByTitleFragment(string titleFragment)
    {
        foreach (Process process in Process.GetProcesses())
        {
            if (process.MainWindowHandle != IntPtr.Zero &&
                process.MainWindowTitle != null &&
                process.MainWindowTitle.IndexOf(titleFragment, StringComparison.OrdinalIgnoreCase) >= 0)
            {
                return process.MainWindowHandle;
            }
        }

        return IntPtr.Zero;
    }
}
"@

Add-Type -TypeDefinition $signature -ErrorAction Stop

$hwnd = [MilkDropCopyDataSender]::FindWindowByTitleFragment($WindowTitle)
if ($hwnd -eq [IntPtr]::Zero) {
    throw "No top-level window title contains '$WindowTitle'. Start MilkDrop3 or pass -WindowTitle."
}

$hasVersion = $Payload -match '(?im)^\s*version\s*='
$hasRequestId = $Payload -match '(?im)^\s*request_id\s*='
$envelope = @()
if (-not $Legacy -and -not $hasVersion) {
    $envelope += 'version=1'
}
if (-not $Legacy -and -not $hasRequestId) {
    if ([string]::IsNullOrWhiteSpace($RequestId)) {
        $RequestId = [guid]::NewGuid().ToString()
    }
    $envelope += "request_id=$RequestId"
}
$wirePayload = if ($envelope.Count -gt 0) { ($envelope + $Payload) -join "`n" } else { $Payload }
$bytes = [Text.Encoding]::Unicode.GetBytes($wirePayload + "`0")
$buffer = [Runtime.InteropServices.Marshal]::AllocHGlobal($bytes.Length)
try {
    [Runtime.InteropServices.Marshal]::Copy($bytes, 0, $buffer, $bytes.Length)
    $copyData = New-Object MilkDropCopyDataSender+COPYDATASTRUCT
    $copyData.dwData = [IntPtr]::Zero
    $copyData.cbData = $bytes.Length
    $copyData.lpData = $buffer

    $result = [IntPtr]::Zero
    $sendResult = [MilkDropCopyDataSender]::SendMessageTimeout($hwnd, [MilkDropCopyDataSender]::WM_COPYDATA, [IntPtr]::Zero, [ref]$copyData, 0x2, $TimeoutMs, [ref]$result)
    if ($sendResult -eq [IntPtr]::Zero) {
        throw "WM_COPYDATA send timed out or failed."
    }

    if ($result -eq [IntPtr]::Zero) {
        if ($Legacy -or [string]::IsNullOrWhiteSpace($RequestId)) {
            Write-Output "rejected"
        } else {
            Write-Output "rejected request_id=$RequestId"
        }
        exit 2
    }

    if ($Legacy -or [string]::IsNullOrWhiteSpace($RequestId)) {
        Write-Output "accepted"
    } else {
        Write-Output "accepted request_id=$RequestId"
    }
}
finally {
    [Runtime.InteropServices.Marshal]::FreeHGlobal($buffer)
}
