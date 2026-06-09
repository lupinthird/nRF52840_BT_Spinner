using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;

namespace L3ControllerTest.UI;

public sealed class StickIndicator
{
    public void Draw(
        SpriteBatch spriteBatch,
        SpriteFont font,
        Texture2D pixel,
        Vector2 center,
        float radius,
        Vector2 stick,
        string label,
        bool showRightStick,
        Vector2? rightStick = null)
    {
        DrawHelper.DrawCrosshair(spriteBatch, pixel, center, radius, new Color(70, 70, 70), 1);
        DrawHelper.DrawCircleOutline(spriteBatch, pixel, center, radius, new Color(120, 120, 120), 48, 1);

        var dot = center + stick * radius * 0.85f;
        DrawHelper.DrawFilledRect(spriteBatch, pixel, new Rectangle((int)dot.X - 4, (int)dot.Y - 4, 8, 8), Color.CornflowerBlue);

        if (showRightStick && rightStick.HasValue)
        {
            var rightDot = center + rightStick.Value * radius * 0.55f;
            DrawHelper.DrawFilledRect(spriteBatch, pixel, new Rectangle((int)rightDot.X - 3, (int)rightDot.Y - 3, 6, 6), Color.Orange);
        }

        FontText.Draw(spriteBatch, font, label, center + new Vector2(-radius, -radius - 18f), Color.LightGray);
    }
}
