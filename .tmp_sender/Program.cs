using System;
using System.Diagnostics;
using System.Runtime.InteropServices;
using System.Text;
using System.Windows.Forms;

[StructLayout(LayoutKind.Sequential)]
struct COPYDATASTRUCT
{
    public IntPtr dwData;
    public int cbData;
    public IntPtr lpData;
}

class ReplyWindow : NativeWindow
{
    public string? LastReply;

    protected override void WndProc(ref Message m)
    {
        if (m.Msg == 0x004A)
        {
            var cds = Marshal.PtrToStructure<COPYDATASTRUCT>(m.LParam);
            var chars = Math.Max(0, cds.cbData / 2 - 1);
            LastReply = chars > 0 ? Marshal.PtrToStringUni(cds.lpData, chars) : string.Empty;
            Console.WriteLine($"reply={Escape(LastReply ?? string.Empty)}");
            m.Result = new IntPtr(1);
            return;
        }

        base.WndProc(ref m);
    }

    private static string Escape(string text) => text.Replace("\r", "\\r").Replace("\n", "\\n");
}

class Program
{
    private const int WM_COPYDATA = 0x004A;

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

    private static string Escape(string text) => text.Replace("\r", "\\r").Replace("\n", "\\n");

    private static int Send(ReplyWindow replyWindow, string titleFragment, string payload)
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
            var sendResult = SendMessageTimeout(hwnd, WM_COPYDATA, replyWindow.Handle, ref cds, 0x2, 5000, out result);
            if (sendResult == IntPtr.Zero)
            {
                Console.WriteLine($"timeout payload={Escape(payload)}");
                return 4;
            }

            Console.WriteLine(result != IntPtr.Zero
                ? $"accepted payload={Escape(payload)}"
                : $"rejected payload={Escape(payload)}");
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

        var replyWindow = new ReplyWindow();
        replyWindow.CreateHandle(new CreateParams { Caption = "md3sender-hidden" });

        var rc = Send(replyWindow, args[0], args[1]);
        Application.DoEvents();
        System.Threading.Thread.Sleep(100);
        Application.DoEvents();
        return rc;
    }
}
