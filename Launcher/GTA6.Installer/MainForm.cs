using System.Diagnostics;
using System.Text.Json;

namespace GTA6.Installer;

internal sealed class MainForm : Form
{
    private readonly TextBox _installPath = new() { Text = Path.Combine(
        Environment.GetFolderPath(Environment.SpecialFolder.LocalApplicationData), "Programs", "GTA6"), Width = 390 };
    private readonly TextBox _sourcePath = new() { Width = 390, ReadOnly = true };
    private readonly Label _status = new() { AutoSize = true, Text = "Připraveno." };
    private readonly Button _installButton = new() { Text = "Instalovat GTA6", AutoSize = true };
    private readonly PackageInstaller _installer = new();
    private readonly OpenFileDialog _manifestDialog = new() { Filter = "GTA6 manifest (manifest.json)|manifest.json|JSON (*.json)|*.json" };
    private readonly FolderBrowserDialog _offlineDialog = new() { Description = "Vyber složku s offline balíkem GTA6 (manifest.json a ZIP)." };
    private readonly ProgressBar _progress = new() { Width = 420, Height = 18, Style = ProgressBarStyle.Marquee, Visible = false };

    public MainForm()
    {
        Text = "GTA6 Installer";
        MinimumSize = new Size(560, 300);
        StartPosition = FormStartPosition.CenterScreen;
        FormBorderStyle = FormBorderStyle.FixedDialog;
        MaximizeBox = false;

        var title = new Label { Text = "GTA6", Font = new Font("Segoe UI", 24, FontStyle.Bold), AutoSize = true };
        var subtitle = new Label { Text = "Instalace hry • Online i offline", AutoSize = true };
        var browseInstall = new Button { Text = "Vybrat…", AutoSize = true };
        var browseSource = new Button { Text = "Vybrat offline balík…", AutoSize = true };
        var installRow = new FlowLayoutPanel { AutoSize = true, WrapContents = false };
        installRow.Controls.AddRange([_installPath, browseInstall]);
        var sourceRow = new FlowLayoutPanel { AutoSize = true, WrapContents = false };
        sourceRow.Controls.AddRange([_sourcePath, browseSource]);
        var layout = new FlowLayoutPanel
        {
            Dock = DockStyle.Fill, FlowDirection = FlowDirection.TopDown, Padding = new Padding(22),
            WrapContents = false, AutoScroll = true
        };
        layout.Controls.AddRange([title, subtitle, new Label { Text = "Cílová složka:", AutoSize = true },
            installRow, new Label { Text = "Offline zdroj (nepovinné):", AutoSize = true }, sourceRow,
            _installButton, _progress, _status]);
        Controls.Add(layout);

        browseInstall.Click += (_, _) =>
        {
            using var dialog = new FolderBrowserDialog { Description = "Vyber cílovou složku GTA6" };
            if (dialog.ShowDialog(this) == DialogResult.OK)
                _installPath.Text = Path.Combine(dialog.SelectedPath, "GTA6");
        };
        browseSource.Click += (_, _) =>
        {
            if (_offlineDialog.ShowDialog(this) == DialogResult.OK)
                _sourcePath.Text = _offlineDialog.SelectedPath;
        };
        _installButton.Click += async (_, _) => await InstallAsync();
    }

    private async Task InstallAsync()
    {
        _installButton.Enabled = false;
        _progress.Visible = true;
        using var cancellation = new CancellationTokenSource();
        var progress = new Progress<string>(message => _status.Text = message);
        string? tempPackage = null;

        try
        {
            ReleaseManifest manifest;
            string packagePath;

            if (!string.IsNullOrWhiteSpace(_sourcePath.Text))
            {
                var manifestPath = Path.Combine(_sourcePath.Text, "manifest.json");
                manifest = ReleaseManifest.Load(manifestPath);
                packagePath = Path.Combine(_sourcePath.Text, manifest.PackageFile);
            }
            else
            {
                var settingsPath = Path.Combine(AppContext.BaseDirectory, "installer.settings.json");
                if (!File.Exists(settingsPath))
                    throw new FileNotFoundException("Chybí installer.settings.json s HTTPS adresou release manifestu.");
                using var settings = JsonDocument.Parse(await File.ReadAllTextAsync(settingsPath));
                var manifestUrl = settings.RootElement.GetProperty("manifestUrl").GetString();
                if (!Uri.TryCreate(manifestUrl, UriKind.Absolute, out var uri) || uri.Scheme != Uri.UriSchemeHttps)
                    throw new InvalidDataException("Adresa release manifestu musí používat HTTPS.");

                using var http = new HttpClient { Timeout = TimeSpan.FromMinutes(2) };
                var manifestJson = await http.GetStringAsync(uri, cancellation.Token);
                manifest = JsonSerializer.Deserialize<ReleaseManifest>(manifestJson)
                    ?? throw new InvalidDataException("Online manifest je neplatný.");
                manifest.Validate();
                var packageUri = new Uri(uri, manifest.PackageUrl
                    ?? throw new InvalidDataException("Online manifest neobsahuje packageUrl."));
                tempPackage = Path.Combine(Path.GetTempPath(), $"gta6-{Guid.NewGuid():N}.zip");
                packagePath = await _installer.DownloadPackageAsync(packageUri, tempPackage, progress, cancellation.Token);
            }

            await _installer.InstallAsync(manifest, packagePath, _installPath.Text, progress, cancellation.Token);
            _status.Text = $"GTA6 {manifest.Version} bylo nainstalováno.";
            var launch = MessageBox.Show(this, "Instalace dokončena. Spustit GTA6?", "GTA6", MessageBoxButtons.YesNo, MessageBoxIcon.Information);
            if (launch == DialogResult.Yes)
            {
                var executable = Path.GetFullPath(Path.Combine(_installPath.Text, manifest.Executable));
                Process.Start(new ProcessStartInfo(executable) { UseShellExecute = true, WorkingDirectory = Path.GetDirectoryName(executable)! });
            }
        }
        catch (Exception ex)
        {
            _status.Text = "Instalace se nezdařila.";
            MessageBox.Show(this, ex.Message, "Chyba instalace", MessageBoxButtons.OK, MessageBoxIcon.Error);
        }
        finally
        {
            if (tempPackage is not null && File.Exists(tempPackage))
                File.Delete(tempPackage);
            _progress.Visible = false;
            _installButton.Enabled = true;
        }
    }
}
