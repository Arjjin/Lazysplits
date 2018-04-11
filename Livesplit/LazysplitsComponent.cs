using System;
using System.Collections.Generic;
using System.Windows.Forms;
using System.Drawing;
using System.Drawing.Drawing2D;

using LiveSplit.Model;
using LiveSplit.UI;
using LiveSplit.UI.Components;

namespace LiveSplit.Lazysplits
{
    public class LazysplitsComponent : IComponent
    {
        public LazysplitsComponentSettings Settings { get; set; }
        public string ComponentName{ get { return "Lazysplits"; } }
        public IDictionary<string, Action> ContextMenuControls{ get { return null; } }
        private LiveSplitState state;
        
        public float VerticalHeight { get; set; }
        public float MinimumHeight { get; set; }
        public float HorizontalWidth { get; set; }
        public float MinimumWidth { get; set; }

        public float PaddingTop { get; set; }
        public float PaddingLeft { get; set; }
        public float PaddingBottom { get; set; }
        public float PaddingRight { get; set; }

        public LazysplitsComponent(LiveSplitState state)
        {
            this.state = state;
            this.Settings = new LazysplitsComponentSettings();
        }
        
        public Control GetSettingsControl(LayoutMode mode)
        {
            return this.Settings;
        }
        public System.Xml.XmlNode GetSettings(System.Xml.XmlDocument document)
        {
            return this.Settings.GetSettings(document);
        }
        public void SetSettings(System.Xml.XmlNode settings)
        {
            this.Settings.SetSettings(settings);
        }

        public void Update(IInvalidator invalidator, Model.LiveSplitState state, float width, float height, LayoutMode mode)
        {
            this.state = state;
        }
        
        private void DrawBase(Graphics g, Model.LiveSplitState state, float width, float height, LayoutMode mode)
        {
        }
        public void DrawHorizontal(Graphics g, Model.LiveSplitState state, float height, Region clipRegion)
        {
            DrawBase(g, state, HorizontalWidth, height, LayoutMode.Horizontal);
        }

        public void DrawVertical(System.Drawing.Graphics g, Model.LiveSplitState state, float width, Region clipRegion)
        {
            DrawBase(g, state, width, VerticalHeight, LayoutMode.Vertical);
        }

        //IDisposable
        public void Dispose()
        { }
    }
}
