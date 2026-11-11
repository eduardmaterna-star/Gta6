using System.IO.Compression;
using System.Security.Cryptography;
using System.Text;
using GTA6.Installer;
using Xunit;

namespace GTA6.Installer.Tests;

public sealed class PackageInstallerTests
{
    [Fact]
    public async Task InstallsValidPackageAndWritesInstallMetadata()
    {
        using var temp = new TemporaryDirectory();
        var zip = CreateZip(("OpenWorldNextGen.exe", "game".ToBytes()));
        var manifest = CreateManifest(zip);
        using var stream = new MemoryStream(zip);

        await new PackageInstaller().InstallAsync(manifest, stream,
            Path.Combine(temp.Path, "GTA6"), new TestProgress(), CancellationToken.None);

        Assert.Equal("game", await File.ReadAllTextAsync(Path.Combine(temp.Path, "GTA6", "OpenWorldNextGen.exe")));
        Assert.Contains("\"version\":\"0.1.0\"", await File.ReadAllTextAsync(Path.Combine(temp.Path, "GTA6", "gta6-install.json")));
    }

    [Fact]
    public async Task RejectsPackageWithIncorrectSha256BeforeCreatingInstall()
    {
        using var temp = new TemporaryDirectory();
        var zip = CreateZip(("OpenWorldNextGen.exe", "game".ToBytes()));
        var manifest = CreateManifest(zip) with { Sha256 = new string('0', 64) };
        using var stream = new MemoryStream(zip);
        var target = Path.Combine(temp.Path, "GTA6");

        await Assert.ThrowsAsync<InvalidDataException>(() =>
            new PackageInstaller().InstallAsync(manifest, stream, target, new TestProgress(), CancellationToken.None));
        Assert.False(Directory.Exists(target));
    }

    [Fact]
    public async Task RejectsZipSlipEntryWithoutWritingOutsideStagingDirectory()
    {
        using var temp = new TemporaryDirectory();
        var zip = CreateZip(
            ("../outside.txt", "malicious".ToBytes()),
            ("OpenWorldNextGen.exe", "game".ToBytes()));
        var manifest = CreateManifest(zip);
        using var stream = new MemoryStream(zip);
        var target = Path.Combine(temp.Path, "GTA6");

        await Assert.ThrowsAsync<InvalidDataException>(() =>
            new PackageInstaller().InstallAsync(manifest, stream, target, new TestProgress(), CancellationToken.None));
        Assert.False(File.Exists(Path.Combine(temp.Path, "outside.txt")));
        Assert.False(Directory.Exists(target));
    }

    [Fact]
    public void ReadsAppendedPayloadAndManifestFromSingleExecutableFile()
    {
        using var temp = new TemporaryDirectory();
        var zip = CreateZip(("OpenWorldNextGen.exe", "game".ToBytes()));
        var manifestJson = Encoding.UTF8.GetBytes(ManifestJson(zip));
        var executable = Path.Combine(temp.Path, "setup.exe");

        using (var file = new FileStream(executable, FileMode.CreateNew, FileAccess.Write))
        {
            file.Write("MZ-test-launcher".ToBytes());
            file.Write(zip);
            file.Write(manifestJson);
            file.Write(Encoding.ASCII.GetBytes("GTA6PAY1"));
            file.Write(BitConverter.GetBytes((long)zip.Length));
            file.Write(BitConverter.GetBytes(manifestJson.Length));
        }

        using var payload = OfflinePayload.Open(executable);
        Assert.Equal("0.1.0", payload.Manifest.Version);
        using var extractedZip = new MemoryStream();
        payload.PackageStream.CopyTo(extractedZip);
        Assert.Equal(zip, extractedZip.ToArray());
    }

    [Fact]
    public void RejectsInvalidPayloadFooter()
    {
        using var temp = new TemporaryDirectory();
        var executable = Path.Combine(temp.Path, "bad.exe");
        File.WriteAllBytes(executable, "not-an-installer".ToBytes());
        Assert.Throws<InvalidDataException>(() => OfflinePayload.Open(executable));
    }

    private static ReleaseManifest CreateManifest(byte[] zip) =>
        new()
        {
            Product = "GTA6",
            Version = "0.1.0",
            PackageFile = "GTA6-Windows-0.1.0.zip",
            PackageUrl = null,
            Sha256 = Convert.ToHexString(SHA256.HashData(zip)),
            Executable = "OpenWorldNextGen.exe"
        };

    private static string ManifestJson(byte[] zip) =>
        $$"""{"product":"GTA6","version":"0.1.0","packageFile":"GTA6-Windows-0.1.0.zip","packageUrl":null,"sha256":"{{Convert.ToHexString(SHA256.HashData(zip))}}","executable":"OpenWorldNextGen.exe"}""";

    private static byte[] CreateZip(params (string Name, byte[] Content)[] entries)
    {
        using var output = new MemoryStream();
        using (var archive = new ZipArchive(output, ZipArchiveMode.Create, leaveOpen: true))
        {
            foreach (var item in entries)
            {
                var entry = archive.CreateEntry(item.Name, CompressionLevel.Optimal);
                using var entryStream = entry.Open();
                entryStream.Write(item.Content);
            }
        }
        return output.ToArray();
    }

    private sealed class TestProgress : IProgress<string>
    {
        public void Report(string value) { }
    }

    private sealed class TemporaryDirectory : IDisposable
    {
        public string Path { get; } = System.IO.Path.Combine(System.IO.Path.GetTempPath(), Guid.NewGuid().ToString("N"));
        public TemporaryDirectory() => Directory.CreateDirectory(Path);
        public void Dispose()
        {
            if (Directory.Exists(Path))
                Directory.Delete(Path, recursive: true);
        }
    }
}

internal static class TestStringExtensions
{
    public static byte[] ToBytes(this string value) => Encoding.UTF8.GetBytes(value);
}
