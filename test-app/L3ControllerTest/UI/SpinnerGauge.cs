using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;

namespace L3ControllerTest.UI;

public sealed class SpinnerGauge
{
    public void Draw(
        SpriteBatch spriteBatch,
        SpriteFont font,
        Texture2D pixel,
        Vector2 center,
        float radius,
        float angleDegrees,
        string label)
    {
        DrawHelper.DrawCircleOutline(spriteBatch, pixel, center, radius, Color.Silver, 64, 2);

        var radians = MathHelper.ToRadians(angleDegrees - 90f);
        var nibEnd = center + new Vector2(MathF.Cos(radians), MathF.Sin(radians)) * (radius - 6f);
        DrawHelper.DrawLine(spriteBatch, pixel, center, nibEnd, Color.White, 3);

        FontText.Draw(spriteBatch, font, label, center + new Vector2(-radius, -radius - 18f), Color.LightGray);
        FontText.Draw(spriteBatch, font, $"{angleDegrees:F0} deg", center + new Vector2(-18f, radius + 6f), Color.Gray);
    }
}
