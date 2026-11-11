using System.Text.Json;
using System.Text.Json.Serialization;

namespace GTA6.Installer;

internal sealed class ReleaseManifest
{
    [JsonPropertyName("product")]
    public string Product { get; init; } = "";

    [JsonPropertyName("version")]
    public string Version { get; init; } = "";

    [JsonPropertyName("packageFile")]
    public string PackageFile { get; init; } = "";

    [JsonPropertyName("packageUrl")]
    public string? PackageUrl { get; init; }

    [JsonPropertyName("sha256")]
    public string Sha256 { get; init; } = "";

    [JsonPropertyName("executable")]
    public string Executable { get; init; } = "";

    public static ReleaseManifest Load(Stream stream)
    {
        var manifest = JsonSerializer.Deserialize<ReleaseManifest>(stream,
            new JsonSerializerOptions { PropertyNameCaseInsensitive = true })
            ?? throw new InvalidDataException("Offline manifest je prázdný nebo neplatný.");
        manifest.Validate();
        return manifest;
    }

    public void Validate()
    {
        if (!string.Equals(Product, "GTA6", StringComparison.Ordinal))
            throw new InvalidDataException("Manifest nepatří produktu GTA6.");
        if (!System.Version.TryParse(Version, out _))
            throw new InvalidDataException("Verze musí mít formát například 0.1.0.");
        if (string.IsNullOrWhiteSpace(PackageFile) ||
            Path.GetFileName(PackageFile) != PackageFile ||
            !PackageFile.EndsWith(".zip", StringComparison.OrdinalIgnoreCase))
            throw new InvalidDataException("Název balíku není bezpečný název ZIP souboru.");
        if (Sha256.Length != 64 || !Sha256.All(Uri.IsHexDigit))
            throw new InvalidDataException("SHA-256 musí obsahovat přesně 64 hexadecimálních znaků.");
        if (string.IsNullOrWhiteSpace(Executable) || Path.IsPathRooted(Executable) ||
            Executable.Split('/', '\\').Any(part => part is "" or "." or ".."))
            throw new InvalidDataException("Cesta ke spustitelnému souboru není bezpečná relativní cesta.");
        if (PackageUrl is not null)
            throw new InvalidDataException("Offline manifest nesmí obsahovat vzdálenou adresu balíku.");
    }
}
