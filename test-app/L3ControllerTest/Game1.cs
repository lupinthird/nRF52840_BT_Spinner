using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;
using Microsoft.Xna.Framework.Input;
using L3Controller.Input;
using L3ControllerTest.UI;

namespace L3ControllerTest;

public class Game1 : Game
{
    private readonly GraphicsDeviceManager _graphics;
    private SpriteBatch _spriteBatch = null!;
    private SpriteFont _font = null!;
    private Texture2D _pixel = null!;
    private ControllerManager _controllerManager = null!;
    private SlotPanel _slotPanel = null!;

    public Game1()
    {
        _graphics = new GraphicsDeviceManager(this)
        {
            PreferredBackBufferWidth = 1280,
            PreferredBackBufferHeight = 720
        };
        Content.RootDirectory = "Content";
        IsMouseVisible = true;
        Window.Title = "L3 Controller Test";
        IsFixedTimeStep = true;
        TargetElapsedTime = TimeSpan.FromSeconds(1.0 / 60.0);
    }

    protected override void Initialize()
    {
        _controllerManager = new ControllerManager();
        _controllerManager.Initialize();
        _slotPanel = new SlotPanel();
        base.Initialize();
    }

    protected override void LoadContent()
    {
        _spriteBatch = new SpriteBatch(GraphicsDevice);
        _font = Content.Load<SpriteFont>("Fonts/Default");
        _pixel = new Texture2D(GraphicsDevice, 1, 1);
        _pixel.SetData(new[] { Color.White });
    }

    protected override void Update(GameTime gameTime)
    {
        if (GamePad.GetState(PlayerIndex.One).Buttons.Back == ButtonState.Pressed ||
            Keyboard.GetState().IsKeyDown(Keys.Escape))
            Exit();

        _controllerManager.Update((float)gameTime.ElapsedGameTime.TotalSeconds);
        base.Update(gameTime);
    }

    protected override void Draw(GameTime gameTime)
    {
        GraphicsDevice.Clear(new Color(24, 24, 28));

        _spriteBatch.Begin(SpriteSortMode.Deferred, BlendState.AlphaBlend, SamplerState.PointClamp);

        var title = $"L3 Controller Test - {_controllerManager.ClaimedCount}/4 active";
        FontText.Draw(_spriteBatch, _font, title, new Vector2(16, 8), Color.White);

        var panelWidth = GraphicsDevice.Viewport.Width / 2;
        var panelHeight = (GraphicsDevice.Viewport.Height - 48) / 2;

        for (var slot = 0; slot < 4; slot++)
        {
            var controller = _controllerManager.GetSlot(slot);
            if (controller == null)
                continue;

            var col = slot % 2;
            var row = slot / 2;
            var bounds = new Rectangle(
                col * panelWidth,
                40 + row * panelHeight,
                panelWidth - 1,
                panelHeight - 1);

            _slotPanel.Draw(_spriteBatch, _font, _pixel, bounds, controller);
        }

        if (_controllerManager.ClaimedCount == 0)
        {
            var hint = "Connect an L3 controller and move a control to claim a slot";
            var size = _font.MeasureString(hint);
            var pos = new Vector2(
                (GraphicsDevice.Viewport.Width - size.X) * 0.5f,
                GraphicsDevice.Viewport.Height * 0.5f);
            FontText.Draw(_spriteBatch, _font, hint, pos, Color.Gray);
        }

        _spriteBatch.End();
        base.Draw(gameTime);
    }

    protected override void UnloadContent()
    {
        _pixel?.Dispose();
        base.UnloadContent();
    }
}
