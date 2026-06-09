using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;

namespace L3ControllerTest.UI;

internal static class FontText
{
    public static string Sanitize(string? text)
    {
        if (string.IsNullOrEmpty(text))
            return string.Empty;

        Span<char> buffer = stackalloc char[text.Length];
        var count = 0;
        foreach (var c in text)
        {
            buffer[count++] = c is >= (char)32 and <= (char)126 ? c : '?';
        }

        return new string(buffer[..count]);
    }

    public static void Draw(SpriteBatch batch, SpriteFont font, string text, Vector2 position, Color color) =>
        batch.DrawString(font, Sanitize(text), position, color);
}
