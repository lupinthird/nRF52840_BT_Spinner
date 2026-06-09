using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;

namespace L3ControllerTest.UI;

public sealed class PaddleGauge
{
    // Pot full-left and full-right both point down (6 o'clock). Turning right
    // rotates the nib a full 360 degrees between those endpoints.
    private const float DownDegrees = 180f;

    public void Draw(
        SpriteBatch spriteBatch,
        SpriteFont font,
        Texture2D pixel,
        Vector2 center,
        float radius,
        float position,
        string label)
    {
        DrawHelper.DrawCircleOutline(spriteBatch, pixel, center, radius, Color.Silver, 64, 2);

        var angle = MapPositionToAngle(position);
        DrawStopTick(spriteBatch, pixel, center, radius, DownDegrees, Color.Gray);

        var radians = MathHelper.ToRadians(angle - 90f);
        var nibEnd = center + new Vector2(MathF.Cos(radians), MathF.Sin(radians)) * (radius - 6f);
        DrawHelper.DrawLine(spriteBatch, pixel, center, nibEnd, Color.White, 3);

        FontText.Draw(spriteBatch, font, label, center + new Vector2(-radius, -radius - 18f), Color.LightGray);
        spriteBatch.DrawString(font, "L", center + new Vector2(-radius, radius + 4f), Color.Gray);
        spriteBatch.DrawString(font, "R", center + new Vector2(radius - 10f, radius + 4f), Color.Gray);
    }

    private static float MapPositionToAngle(float position)
    {
        var t = (Math.Clamp(position, -1f, 1f) + 1f) * 0.5f;
        return DownDegrees + t * 360f;
    }

    private static void DrawStopTick(
        SpriteBatch spriteBatch,
        Texture2D pixel,
        Vector2 center,
        float radius,
        float degrees,
        Color color)
    {
        var inner = TickPoint(center, radius - 10f, degrees);
        var outer = TickPoint(center, radius, degrees);
        DrawHelper.DrawLine(spriteBatch, pixel, inner, outer, color, 2);
    }

    private static Vector2 TickPoint(Vector2 center, float radius, float degrees)
    {
        var radians = MathHelper.ToRadians(degrees - 90f);
        return center + new Vector2(MathF.Cos(radians), MathF.Sin(radians)) * radius;
    }
}
