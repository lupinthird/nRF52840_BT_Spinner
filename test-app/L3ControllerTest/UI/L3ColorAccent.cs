using Microsoft.Xna.Framework;
using L3Controller.Input;

namespace L3ControllerTest.UI;

internal static class L3ColorAccent
{
    public static Color ForControllerColor(L3ControllerColor color) =>
        color switch
        {
            L3ControllerColor.Red => new Color(220, 70, 70),
            L3ControllerColor.Blue => new Color(80, 140, 230),
            L3ControllerColor.Green => new Color(80, 190, 110),
            L3ControllerColor.Yellow => new Color(230, 200, 70),
            _ => Color.LightGray
        };
}
