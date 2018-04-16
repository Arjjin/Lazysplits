using System;
using System.Windows.Forms;
using System.Xml;
using LiveSplit.UI;

namespace LiveSplit.Lazysplits
{
    public partial class LazysplitsComponentSettings : UserControl
    {
        public LazysplitsComponentSettings()
        {
            InitializeComponent();
        }
        
        public XmlNode GetSettings(XmlDocument document)
        {
            var XmlSettings = document.CreateElement("Settings");
            SettingsHelper.CreateSetting(document, XmlSettings, "Version", "1.0");
            return XmlSettings;
        }
        public void SetSettings(XmlNode node)
        {
        }
    }
} //namespace LiveSplit.Lazysplits
