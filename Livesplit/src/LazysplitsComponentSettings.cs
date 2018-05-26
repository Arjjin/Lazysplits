using System;
using System.Windows.Forms;
using System.Drawing;
using System.Xml;
using LiveSplit.UI;

using NLog;

namespace LiveSplit.Lazysplits
{
    public partial class LazysplitsComponentSettings : UserControl
    {
        private string _SharedDataRootDir;
        public event EventHandler SharedDataRootDirChanged;
        public string SharedDataRootDir {
            get { return _SharedDataRootDir; }
            set
            {
                _SharedDataRootDir = value;
                SharedDataRootDirChanged?.Invoke( this, EventArgs.Empty );
            }
        }
        public Color BackgroundColor { get; set; }
        public Color BackgroundColor2 { get; set; }
        public GradientType BackgroundGradient { get; set; }
        public string GradientString
        {
            get { return BackgroundGradient.ToString(); }
            set { BackgroundGradient = (GradientType)Enum.Parse(typeof(GradientType), value); }
        }
        public bool OpenPipeOnStart { get; set; }
        
        //NLog
        private static Logger Log = LogManager.GetCurrentClassLogger();

        public LazysplitsComponentSettings()
        {
            InitializeComponent();

            SharedDataRootDir = string.Empty;
            BackgroundColor = Color.Transparent;
            BackgroundColor2 = Color.Transparent;
            BackgroundGradient = GradientType.Plain;
            OpenPipeOnStart = true;

            SharedDataDirText.DataBindings.Add( "Text", this, "SharedDataRootDir", false, DataSourceUpdateMode.OnPropertyChanged);
            btnColor1.DataBindings.Add( "BackColor", this, "BackgroundColor", false, DataSourceUpdateMode.OnPropertyChanged);
            btnColor2.DataBindings.Add( "BackColor", this, "BackgroundColor2", false, DataSourceUpdateMode.OnPropertyChanged);
            cmbGradientType.DataBindings.Add( "SelectedItem", this, "GradientString", false, DataSourceUpdateMode.OnPropertyChanged);
            checkBoxPipeStart.DataBindings.Add( "Checked", this, "OpenPipeOnStart", false, DataSourceUpdateMode.OnPropertyChanged );
        }
        
        public void SetSettings(XmlNode node)
        {
            var element = (XmlElement)node;
            SharedDataRootDir = SettingsHelper.ParseString(element["SharedDataRootDir"]);
            BackgroundColor = SettingsHelper.ParseColor(element["BackgroundColor"]);
            BackgroundColor2 = SettingsHelper.ParseColor(element["BackgroundColor2"]);
            GradientString = SettingsHelper.ParseString(element["BackgroundGradient"]);
            OpenPipeOnStart = SettingsHelper.ParseBool(element["OpenPipeOnStart"]);
        }
        public XmlNode GetSettings(XmlDocument document)
        {
            var parent = document.CreateElement("Settings");
            CreateSettingsNode(document, parent);
            return parent;
        }
        public int GetSettingsHashCode()
        {
            return CreateSettingsNode(null, null);
        }
        private int CreateSettingsNode(XmlDocument document, XmlElement parent)
        {
            return SettingsHelper.CreateSetting( document, parent, "Version", "1.0" ) ^
            SettingsHelper.CreateSetting( document, parent, "SharedDataRootDir", SharedDataRootDir ) ^
            SettingsHelper.CreateSetting( document, parent, "BackgroundColor", BackgroundColor ) ^
            SettingsHelper.CreateSetting( document, parent, "BackgroundColor2", BackgroundColor2 ) ^
            SettingsHelper.CreateSetting( document, parent, "BackgroundGradient", BackgroundGradient ) ^
            SettingsHelper.CreateSetting( document, parent, "OpenPipeOnStart", OpenPipeOnStart );
        }

        /* shared data path stuff */

        private void ChangeSharedDataText( string text )
        {
            SharedDataDirText.Text = text;
        }
        
        private void SharedDataDirButton_Click( object sender, EventArgs e )
        {
            var dialog = new FolderBrowserDialog();
            if ( dialog.ShowDialog() == DialogResult.OK )
            {
                SharedDataRootDir = dialog.SelectedPath;
            }
        }

        /* color stuff */
        
        private void ColorButtonClick(object sender, EventArgs e)
        {
            SettingsHelper.ColorButtonClick((Button)sender, this);
        }
        void cmbGradientType_SelectedIndexChanged(object sender, EventArgs e)
        {
            btnColor1.Visible = cmbGradientType.SelectedItem.ToString() != "Plain";
            btnColor2.DataBindings.Clear();
            btnColor2.DataBindings.Add("BackColor", this, btnColor1.Visible ? "BackgroundColor2" : "BackgroundColor", false, DataSourceUpdateMode.OnPropertyChanged);
            GradientString = cmbGradientType.SelectedItem.ToString();
        }

        private void LazysplitsComponentSettings_Load(object sender, EventArgs e)
        {

        }

        /* other UI settings */

        private void checkBoxPipeStart_CheckedChanged(object sender, EventArgs e)
        {
            OpenPipeOnStart = checkBoxPipeStart.Checked;
        }
    }
} //namespace LiveSplit.Lazysplits
