using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;
using L3Controller.Input;

namespace L3ControllerTest.UI;

public sealed class SlotPanel
{
    private readonly ButtonGrid _buttonGrid = new();
    private readonly StickIndicator _stickIndicator = new();
    private readonly SpinnerGauge _spinnerGauge = new();
    private readonly PaddleGauge _paddleGauge = new();

    public void Draw(
        SpriteBatch spriteBatch,
        SpriteFont font,
        Texture2D pixel,
        Rectangle bounds,
        TrackedController controller)
    {
        DrawHelper.DrawFilledRect(spriteBatch, pixel, bounds, new Color(30, 30, 30));
        DrawHelper.DrawFilledRect(spriteBatch, pixel, new Rectangle(bounds.X, bounds.Y, bounds.Width, 1), new Color(70, 70, 70));
        DrawHelper.DrawFilledRect(spriteBatch, pixel, new Rectangle(bounds.X, bounds.Y, 1, bounds.Height), new Color(70, 70, 70));

        var info = ResolveDeviceInfo(controller);
        var primaryLabel = info.IsValid
            ? info.Serial
            : controller.SlotIndex.HasValue
                ? "Identifying..."
                : "Waiting for input...";

        var headerX = bounds.X + 12;
        var headerY = bounds.Y + 8f;
        FontText.Draw(spriteBatch, font, primaryLabel, new Vector2(headerX, headerY), Color.White);
        headerY += font.LineSpacing + 4f;

        if (info.IsValid)
        {
            var accentColor = L3ColorAccent.ForControllerColor(info.Color);
            FontText.Draw(spriteBatch, font, info.DisplayName, new Vector2(headerX, headerY), accentColor);
            headerY += font.LineSpacing + 4f;

            var identityDetail = FormatIdentityDetail(info);
            FontText.Draw(spriteBatch, font, identityDetail, new Vector2(headerX, headerY), Color.Gray);
            headerY += font.LineSpacing + 10f;
        }
        else
        {
            headerY += 8f;
        }

        var snapshot = controller.Current;
        var buttonCount = controller.Profile == DeviceProfile.GenericGamepad ? 14 : 3;
        _buttonGrid.Draw(spriteBatch, font, pixel, new Vector2(headerX, headerY), snapshot.Buttons, buttonCount);

        var showSpinner = controller.SpinnerSource != SpinnerSourceKind.None;
        var showPaddle = controller.PaddleSource != PaddleSourceKind.None;
        var showSticks = controller.Profile == DeviceProfile.GenericGamepad
            && (snapshot.HasLeftStick || snapshot.HasRightStick);

        if (showSticks)
        {
            _stickIndicator.Draw(
                spriteBatch,
                font,
                pixel,
                new Vector2(bounds.X + 120, bounds.Y + 250),
                48f,
                snapshot.LeftStick,
                "Left Stick",
                snapshot.HasRightStick,
                snapshot.RightStick);
        }

        if (showSpinner)
        {
            var spinnerLabel = controller.SpinnerSource switch
            {
                SpinnerSourceKind.RawZ => "Spinner (HID Z)",
                SpinnerSourceKind.JoystickZ => "Spinner (Z)",
                SpinnerSourceKind.LeftStickX => "Spinner (X)",
                _ => "Spinner"
            };
            var spinnerCenter = showPaddle
                ? new Vector2(bounds.X + bounds.Width * 0.38f, bounds.Y + 210)
                : new Vector2(bounds.X + bounds.Width * 0.55f, bounds.Y + 210);
            _spinnerGauge.Draw(spriteBatch, font, pixel, spinnerCenter, 56f, controller.SpinnerAngleDegrees, spinnerLabel);
        }

        if (showPaddle)
        {
            var paddleLabel = controller.PaddleSource switch
            {
                PaddleSourceKind.RawRz => "Paddle (HID Rz)",
                PaddleSourceKind.JoystickRz => "Paddle (Rz)",
                PaddleSourceKind.LeftStickY => "Paddle (Y)",
                _ => "Paddle"
            };
            var paddleCenter = showSpinner
                ? new Vector2(bounds.X + bounds.Width * 0.72f, bounds.Y + 210)
                : new Vector2(bounds.X + bounds.Width * 0.55f, bounds.Y + 210);
            _paddleGauge.Draw(spriteBatch, font, pixel, paddleCenter, 56f, controller.PaddlePosition, paddleLabel);
        }

        if (snapshot.LeftTrigger > 0.01f || snapshot.RightTrigger > 0.01f)
        {
            DrawTriggerBar(spriteBatch, font, pixel, new Vector2(bounds.X + 12, bounds.Bottom - 36), "LT", snapshot.LeftTrigger);
            DrawTriggerBar(spriteBatch, font, pixel, new Vector2(bounds.X + 120, bounds.Bottom - 36), "RT", snapshot.RightTrigger);
        }
    }

    private static void DrawTriggerBar(
        SpriteBatch spriteBatch,
        SpriteFont font,
        Texture2D pixel,
        Vector2 origin,
        string label,
        float value)
    {
        var bar = new Rectangle((int)origin.X + 24, (int)origin.Y, 80, 10);
        DrawHelper.DrawFilledRect(spriteBatch, pixel, bar, new Color(40, 40, 40));
        var fill = new Rectangle(bar.X, bar.Y, (int)(bar.Width * Math.Clamp(value, 0f, 1f)), bar.Height);
        DrawHelper.DrawFilledRect(spriteBatch, pixel, fill, Color.SteelBlue);
        FontText.Draw(spriteBatch, font, label, origin, Color.LightGray);
    }

    private static L3DeviceInfo ResolveDeviceInfo(TrackedController controller)
    {
        if (controller.TryGetDeviceInfo(out var info))
            return info;

        return L3DeviceInfo.FromSerial(controller.DeviceSerialNumber);
    }

    private static string FormatIdentityDetail(L3DeviceInfo info)
    {
        var typeLabel = info.Type switch
        {
            DeviceProfile.Spinner => "Spinner",
            DeviceProfile.Paddle => "Paddle",
            DeviceProfile.Combo => "Combo",
            DeviceProfile.GenericGamepad => "Gamepad",
            _ => "Unknown"
        };

        if (!string.IsNullOrWhiteSpace(info.Unit))
            return $"Type: {typeLabel} - Unit: {info.Unit}";

        return $"Type: {typeLabel}";
    }
}
