using System.Diagnostics;
using System.Net;
using System.Security.Cryptography;
using System.Text.Json;
using System.Text.Json.Serialization;

namespace GTA6Installer;

internal sealed class ReleaseManifest
{
    [JsonPropertyName("product")] public string Product { get; init; } = "";
    [JsonPropertyName("version")] public string Version { get; init; } = "";
    [JsonPropertyName("entryPoint")] public string EntryPoint { get; init; } = "";
    [JsonPropertyName("files")] public List<ReleaseFile> Files { get; init; } = [];
}
internal sealed class ReleaseFile
{
    [JsonPropertyName("path")] public string Path { get; init; } = "";
    [JsonPropertyName("url")] public string Url { get; init; } = "";
    [JsonPropertyName("size")] public long Size { get; init; }
    [JsonPropertyName("sha256")] public string Sha256 { get; init; } = "";
}
internal sealed class InstallerForm : Form
{
    private const string ManifestUrl = "https://raw.githubusercontent.com/eduardmaterna-star/Gta6/main/distribution/manifest.json";
    private static readonly HttpClient Http = new(new HttpClientHandler { AutomaticDecompression = DecompressionMethods.GZip | DecompressionMethods.Deflate })
    { Timeout = TimeSpan.FromMinutes(30) };
    private readonly Label status = new() { AutoSize = true, Text = "Připraveno." };
    private readonly Label detail = new() { AutoSize = true, ForeColor = Color.DimGray };
    private readonly ProgressBar progress = new() { Dock = DockStyle.Top, Height = 20 };
    private readonly TextBox folder = new() { ReadOnly = true, Dock = DockStyle.Fill };
    private readonly Button install = new() { Text = "Zkontrolovat a instalovat", AutoSize = true };
    private readonly Button browse = new() { Text = "Vybrat složku…", AutoSize = true };

    public InstallerForm()
    {
        Text = "GTA6 – instalátor"; MinimumSize = new Size(560, 250); Size = new Size(640, 280);
        StartPosition = FormStartPosition.CenterScreen; FormBorderStyle = FormBorderStyle.FixedDialog; MaximizeBox = false;
        folder.Text = Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.LocalApplicationData), "Programs", "GTA6");
        install.Click += async (_, _) => await InstallAsync();
        browse.Click += (_, _) => BrowseFolder();
        var title = new Label { Text = "GTA6", Font = new Font("Segoe UI", 22, FontStyle.Bold), AutoSize = true, ForeColor = Color.FromArgb(30, 110, 190) };
        var row = new TableLayoutPanel { Dock = DockStyle.Fill, ColumnCount = 2, AutoSize = true };
        row.ColumnStyles.Add(new ColumnStyle(SizeType.Percent, 100)); row.ColumnStyles.Add(new ColumnStyle(SizeType.AutoSize));
        row.Controls.Add(folder, 0, 0); row.Controls.Add(browse, 1, 0);
        var bottom = new FlowLayoutPanel { Dock = DockStyle.Fill, FlowDirection = FlowDirection.TopDown, AutoSize = true };
        bottom.Controls.Add(status); bottom.Controls.Add(detail); bottom.Controls.Add(install);
        var layout = new TableLayoutPanel { Dock = DockStyle.Fill, Padding = new Padding(22), ColumnCount = 1, RowCount = 5 };
        layout.RowStyles.Add(new RowStyle(SizeType.AutoSize)); layout.RowStyles.Add(new RowStyle(SizeType.AutoSize));
        layout.RowStyles.Add(new RowStyle(SizeType.AutoSize)); layout.RowStyles.Add(new RowStyle(SizeType.Percent, 100));
        layout.RowStyles.Add(new RowStyle(SizeType.AutoSize));
        layout.Controls.Add(title, 0, 0); layout.Controls.Add(new Label { Text = "Instalační složka:", AutoSize = true }, 0, 1);
        layout.Controls.Add(row, 0, 2); layout.Controls.Add(progress, 0, 3); layout.Controls.Add(bottom, 0, 4); Controls.Add(layout);
    }
    private void BrowseFolder()
    {
        using var dialog = new FolderBrowserDialog { SelectedPath = folder.Text, ShowNewFolderButton = true };
        if (dialog.ShowDialog(this) == DialogResult.OK) folder.Text = dialog.SelectedPath;
    }
    private async Task InstallAsync()
    {
        install.Enabled = browse.Enabled = false; progress.Value = 0;
        try
        {
            status.Text = "Načítám informace o vydání…";
            using var response = await Http.GetAsync(ManifestUrl, HttpCompletionOption.ResponseHeadersRead);
            response.EnsureSuccessStatusCode();
            await using var stream = await response.Content.ReadAsStreamAsync();
            var release = await JsonSerializer.DeserializeAsync<ReleaseManifest>(stream)
                ?? throw new InvalidDataException("Distribuční manifest je neplatný.");
            ValidateManifest(release);
            if (release.Files.Count == 0)
            {
                status.Text = "Hra zatím není zveřejněna.";
                detail.Text = "Instalátor je připraven, ale herní soubory ještě nejsou dostupné.";
                MessageBox.Show(this, "Herní build ještě nebyl zveřejněn. Instalátor je připraven na budoucí vydání.",
                    "GTA6 – bez vydání", MessageBoxButtons.OK, MessageBoxIcon.Information);
                return;
            }
            var root = Path.GetFullPath(folder.Text); Directory.CreateDirectory(root);
            long total = release.Files.Sum(f => f.Size), completed = 0;
            foreach (var file in release.Files)
            {
                var destination = SafeDestination(root, file.Path);
                Directory.CreateDirectory(Path.GetDirectoryName(destination)!);
                var temp = destination + ".part-" + Guid.NewGuid().ToString("N");
                try
                {
                    status.Text = "Stahuji " + file.Path;
                    await DownloadAndVerifyAsync(file, temp, bytes =>
                    {
                        progress.Value = total == 0 ? 0 : (int)Math.Clamp((completed + bytes) * 100d / total, 0, 100);
                        detail.Text = $"{FormatBytes(completed + bytes)} / {FormatBytes(total)}";
                    });
                    File.Move(temp, destination, true); completed += file.Size;
                }
                finally { if (File.Exists(temp)) File.Delete(temp); }
            }
            var exe = SafeDestination(root, release.EntryPoint);
            if (!File.Exists(exe)) throw new InvalidDataException("Spustitelný soubor hry nebyl nalezen.");
            progress.Value = 100; status.Text = $"GTA6 {release.Version} je nainstalováno."; detail.Text = "Instalace dokončena.";
            if (MessageBox.Show(this, "Instalace dokončena. Spustit GTA6?", "GTA6", MessageBoxButtons.YesNo, MessageBoxIcon.Information) == DialogResult.Yes)
                Process.Start(new ProcessStartInfo(exe) { WorkingDirectory = root, UseShellExecute = true });
        }
        catch (Exception ex) when (ex is HttpRequestException or IOException or InvalidDataException or UnauthorizedAccessException or JsonException or CryptographicException)
        {
            status.Text = "Instalace se nezdařila."; detail.Text = ex.Message;
            MessageBox.Show(this, ex.Message, "Chyba instalace", MessageBoxButtons.OK, MessageBoxIcon.Error);
        }
        finally { install.Enabled = browse.Enabled = true; }
    }
    private static void ValidateManifest(ReleaseManifest r)
    {
        if (r.Product != "GTA6") throw new InvalidDataException("Manifest nepatří produktu GTA6.");
        if (string.IsNullOrWhiteSpace(r.Version) || r.Version.Length > 64) throw new InvalidDataException("Neplatná verze.");
        if (r.Files.Count > 100000) throw new InvalidDataException("Příliš mnoho souborů v manifestu.");
        var seen = new HashSet<string>(StringComparer.OrdinalIgnoreCase);
        foreach (var f in r.Files)
        {
            _ = SafeRelativePath(f.Path);
            if (!seen.Add(f.Path)) throw new InvalidDataException("Duplicitní cesta v manifestu.");
            if (f.Size < 0 || f.Size > 20L * 1024 * 1024 * 1024) throw new InvalidDataException("Neplatná velikost souboru.");
            if (f.Sha256.Length != 64 || !f.Sha256.All(Uri.IsHexDigit)) throw new InvalidDataException("Neplatný SHA-256 hash.");
            if (!Uri.TryCreate(f.Url, UriKind.Absolute, out var uri) || uri.Scheme != Uri.UriSchemeHttps)
                throw new InvalidDataException("Všechny soubory musí používat HTTPS.");
        }
        _ = SafeRelativePath(r.EntryPoint);
        if (r.Files.Count > 0 && !seen.Contains(r.EntryPoint)) throw new InvalidDataException("Entry point není v seznamu souborů.");
    }
    private static string SafeRelativePath(string path)
    {
        if (string.IsNullOrWhiteSpace(path) || Path.IsPathRooted(path)) throw new InvalidDataException("Neplatná relativní cesta.");
        var normalized = path.Replace('\\', '/');
        if (normalized.Split('/').Any(p => p is "" or "." or "..") || normalized.Contains(':'))
            throw new InvalidDataException("Cesta opouští instalační složku.");
        return normalized.Replace('/', Path.DirectorySeparatorChar);
    }
    private static string SafeDestination(string root, string relative)
    {
        var prefix = Path.GetFullPath(root).TrimEnd(Path.DirectorySeparatorChar) + Path.DirectorySeparatorChar;
        var full = Path.GetFullPath(Path.Combine(root, SafeRelativePath(relative)));
        if (!full.StartsWith(prefix, StringComparison.OrdinalIgnoreCase)) throw new InvalidDataException("Neplatná cílová cesta.");
        return full;
    }
    private static async Task DownloadAndVerifyAsync(ReleaseFile file, string temp, Action<long> progressCallback)
    {
        using var response = await Http.GetAsync(file.Url, HttpCompletionOption.ResponseHeadersRead);
        response.EnsureSuccessStatusCode();
        if (response.Content.Headers.ContentLength is long length && length != file.Size) throw new InvalidDataException("Nesouhlasí velikost: " + file.Path);
        await using var input = await response.Content.ReadAsStreamAsync();
        await using var output = new FileStream(temp, FileMode.CreateNew, FileAccess.Write, FileShare.None, 131072, true);
        using var hash = IncrementalHash.CreateHash(HashAlgorithmName.SHA256);
        var buffer = new byte[131072]; long received = 0;
        while (true)
        {
            var n = await input.ReadAsync(buffer); if (n == 0) break;
            received += n; if (received > file.Size) throw new InvalidDataException("Soubor překročil deklarovanou velikost.");
            hash.AppendData(buffer, 0, n); await output.WriteAsync(buffer.AsMemory(0, n)); progressCallback(received);
        }
        await output.FlushAsync();
        if (received != file.Size) throw new InvalidDataException("Neúplné stažení: " + file.Path);
        if (!CryptographicOperations.FixedTimeEquals(Convert.FromHexString(file.Sha256), hash.GetHashAndReset()))
            throw new InvalidDataException("SHA-256 kontrola selhala: " + file.Path);
    }
    private static string FormatBytes(long n) => n < 1048576 ? $"{n / 1024d:0.0} KB" : $"{n / 1048576d:0.0} MB";
}