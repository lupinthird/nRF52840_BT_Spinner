using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;
using L3Controller.Input;

namespace L3ControllerTest.UI;

internal static class DrawHelper
{
    public static void DrawLine(SpriteBatch spriteBatch, Texture2D pixel, Vector2 start, Vector2 end, Color color, int thickness = 1)
    {
        var edge = end - start;
        var angle = MathF.Atan2(edge.Y, edge.X);
        var length = edge.Length();

        spriteBatch.Draw(
            pixel,
            start,
            null,
            color,
            angle,
            Vector2.Zero,
            new Vector2(length, thickness),
            SpriteEffects.None,
            0f);
    }

    public static void DrawCircleOutline(
        SpriteBatch spriteBatch,
        Texture2D pixel,
        Vector2 center,
        float radius,
        Color color,
        int segments = 64,
        int thickness = 2)
    {
        if (segments < 8)
            segments = 8;

        var step = MathF.Tau / segments;
        var previous = center + new Vector2(MathF.Cos(0f), MathF.Sin(0f)) * radius;

        for (var i = 1; i <= segments; i++)
        {
            var angle = step * i;
            var next = center + new Vector2(MathF.Cos(angle), MathF.Sin(angle)) * radius;
            DrawLine(spriteBatch, pixel, previous, next, color, thickness);
            previous = next;
        }
    }

    public static void DrawFilledRect(
        SpriteBatch spriteBatch,
        Texture2D pixel,
        Rectangle rect,
        Color color)
    {
        spriteBatch.Draw(pixel, rect, color);
    }

    public static void DrawCrosshair(
        SpriteBatch spriteBatch,
        Texture2D pixel,
        Vector2 center,
        float size,
        Color color,
        int thickness = 1)
    {
        DrawLine(spriteBatch, pixel, center - new Vector2(size, 0f), center + new Vector2(size, 0f), color, thickness);
        DrawLine(spriteBatch, pixel, center - new Vector2(0f, size), center + new Vector2(0f, size), color, thickness);
    }
}
