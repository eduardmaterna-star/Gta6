using System.Diagnostics;
using System.Reflection;

namespace GTA6.Installer;

internal sealed class MainForm : Form
{
    private readonly TextBox _installPath = new()
    {
        Text = Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.LocalApplicationData), "Programs", "GTA6"),
        Width = 390
    };
    private readonly Label _status = new() { AutoSize = true, Text = "Připraveno k offline instalaci." };
    private readonly Button _installButton = new() { Text = "Nainstalovat GTA6", AutoSize = true };
    private readonly ProgressBar _progress = new() { Width = 420, Height = 18, Style = ProgressBarStyle.Marquee, Visible = false };
    private readonly PackageInstaller _installer = new();

    public MainForm()
    {
        Text = "GTA6 – Offline instalátor";
        MinimumSize = new Size(560, 260);
        StartPosition = FormStartPosition.CenterScreen;
        FormBorderStyle = FormBorderStyle.FixedDialog;
        MaximizeBox = false;

        var title = new Label { Text = "GTA6", Font = new Font("Segoe UI", 24, FontStyle.Bold), AutoSize = true };
        var subtitle = new Label { Text = "Kompletní instalace bez připojení k internetu", AutoSize = true };
        var browse = new Button { Text = "Vybrat…", AutoSize = true };
        var row = new FlowLayoutPanel { AutoSize = true, WrapContents = false };
        row.Controls.AddRange([_installPath, browse]);
        var layout = new FlowLayoutPanel
        {
            Dock = DockStyle.Fill, FlowDirection = FlowDirection.TopDown, Padding = new Padding(22),
            WrapContents = false, AutoScroll = true
        };
        layout.Controls.AddRange([title, subtitle, new Label { Text = "Cílová složka:", AutoSize = true },
            row, _installButton, _progress, _status]);
        Controls.Add(layout);

        browse.Click += (_, _) =>
        {
            using var dialog = new FolderBrowserDialog { Description = "Vyber cílovou složku GTA6" };
            if (dialog.ShowDialog(this) == DialogResult.OK)
                _installPath.Text = Path.Combine(dialog.SelectedPath, "GTA6");
        };
        _installButton.Click += async (_, _) => await InstallAsync();
    }

    private async Task InstallAsync()
    {
        _installButton.Enabled = false;
        _progress.Visible = true;
        using var cancellation = new CancellationTokenSource();
        var progress = new Progress<string>(message => _status.Text = message);

        try
        {
            var assembly = Assembly.GetExecutingAssembly();
            await using var manifestStream = assembly.GetManifestResourceStream("GTA6.Installer.offline-manifest.json")
                ?? throw new InvalidDataException("Instalátor neobsahuje offline manifest.");
            var manifest = ReleaseManifest.Load(manifestStream);

            var tempPackage = Path.Combine(Path.GetTempPath(), $"gta6-offline-{Guid.NewGuid():N}.zip");
            try
            {
                await using var embeddedPackage = assembly.GetManifestResourceStream("GTA6.Installer.OfflinePackage.zip")
                    ?? throw new InvalidDataException("Instalátor neobsahuje herní balík.");
                await using (var output = new FileStream(tempPackage, FileMode.CreateNew, FileAccess.Write, FileShare.None, 1024 * 1024, useAsync: true))
                    await embeddedPackage.CopyToAsync(output, cancellation.Token);

                await _installer.InstallAsync(manifest, tempPackage, _installPath.Text, progress, cancellation.Token);
            }
            finally
            {
                if (File.Exists(tempPackage))
                    File.Delete(tempPackage);
            }

            _status.Text = $"GTA6 {manifest.Version} bylo nainstalováno.";
            if (MessageBox.Show(this, "Instalace dokončena. Spustit GTA6?", "GTA6",
                    MessageBoxButtons.YesNo, MessageBoxIcon.Information) == DialogResult.Yes)
            {
                var executable = Path.GetFullPath(Path.Combine(_installPath.Text, manifest.Executable.Replace('/', Path.DirectorySeparatorChar)));
                Process.Start(new ProcessStartInfo(executable)
                {
                    UseShellExecute = true,
                    WorkingDirectory = Path.GetDirectoryName(executable)!
                });
            }
        }
        catch (OperationCanceledException)
        {
            _status.Text = "Instalace byla zrušena.";
        }
        catch (Exception ex)
        {
            _status.Text = "Instalace se nezdařila.";
            MessageBox.Show(this, ex.Message, "Chyba instalace", MessageBoxButtons.OK, MessageBoxIcon.Error);
        }
        finally
        {
            _progress.Visible = false;
            _installButton.Enabled = true;
        }
    }
}
