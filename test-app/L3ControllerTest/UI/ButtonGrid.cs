using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;
using L3Controller.Input;

namespace L3ControllerTest.UI;

public sealed class ButtonGrid
{
    private const int Columns = 5;
    private const int CellWidth = 56;
    private const int CellHeight = 22;
    private const int Padding = 4;

    public void Draw(SpriteBatch spriteBatch, SpriteFont font, Texture2D pixel, Vector2 origin, bool[] buttons, int visibleCount)
    {
        var count = Math.Min(Math.Min(buttons.Length, InputMapping.ButtonLabels.Length), visibleCount);
        for (var i = 0; i < count; i++)
        {
            var col = i % Columns;
            var row = i / Columns;
            var rect = new Rectangle(
                (int)origin.X + col * (CellWidth + Padding),
                (int)origin.Y + row * (CellHeight + Padding),
                CellWidth,
                CellHeight);

            var pressed = buttons[i];
            var bg = pressed ? new Color(80, 180, 80) : new Color(45, 45, 45);
            var border = pressed ? Color.LightGreen : new Color(90, 90, 90);

            DrawHelper.DrawFilledRect(spriteBatch, pixel, rect, bg);
            DrawHelper.DrawFilledRect(spriteBatch, pixel, new Rectangle(rect.X, rect.Y, rect.Width, 1), border);
            DrawHelper.DrawFilledRect(spriteBatch, pixel, new Rectangle(rect.X, rect.Bottom - 1, rect.Width, 1), border);
            DrawHelper.DrawFilledRect(spriteBatch, pixel, new Rectangle(rect.X, rect.Y, 1, rect.Height), border);
            DrawHelper.DrawFilledRect(spriteBatch, pixel, new Rectangle(rect.Right - 1, rect.Y, 1, rect.Height), border);

            var label = InputMapping.ButtonLabels[i];
            var textSize = font.MeasureString(label);
            var textPos = new Vector2(
                rect.X + (rect.Width - textSize.X) * 0.5f,
                rect.Y + (rect.Height - textSize.Y) * 0.5f);
            FontText.Draw(spriteBatch, font, label, textPos, Color.White);
        }
    }
}
