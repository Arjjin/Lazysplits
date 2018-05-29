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
        public bool bOpenPipeOnStart { get; set; }

        /* UI drawing settings */

        public Color BackgroundColor { get; set; }
        public Color BackgroundColor2 { get; set; }
        public GradientType BackgroundGradient { get; set; }
        public string GradientString
        {
            get { return BackgroundGradient.ToString(); }
            set { BackgroundGradient = (GradientType)Enum.Parse(typeof(GradientType), value); }
        }
        private bool _bStatusIconsEnabled;
        public event EventHandler StatusIconsEnabledChanged;
        public bool bStatusIconsEnabled {
            get { return _bStatusIconsEnabled; }
            set
            {
                _bStatusIconsEnabled = value;
                StatusIconsEnabledChanged?.Invoke( this, EventArgs.Empty );
            }
        }
        public int IconPadding { get; set; }
        public int IconMargin { get; set; }
        public Color ConnectionIconColor { get; set; }
        public Color IncomingDataIconColor { get; set; }
        public Color OutgoingDataIconColor { get; set; }
        public Color WarningIconColor { get; set; }
        public Color ErrorIconColor { get; set; }
        public Color InactiveIconColor { get; set; }
        
        //NLog
        private static Logger Log = LogManager.GetCurrentClassLogger();

        public LazysplitsComponentSettings()
        {
            InitializeComponent();

            //default settings
            SharedDataRootDir = string.Empty;
            bOpenPipeOnStart = true;
            BackgroundColor = Color.Transparent;
            BackgroundColor2 = Color.Transparent;
            BackgroundGradient = GradientType.Plain;
            bStatusIconsEnabled = true;
            IconPadding = 3;
            IconMargin = 10;
            ConnectionIconColor = Color.White;
            IncomingDataIconColor = Color.GreenYellow;
            OutgoingDataIconColor = Color.DarkOrange;
            WarningIconColor = Color.Gold;
            ErrorIconColor = Color.Salmon;
            InactiveIconColor = Color.FromArgb( 60, 70, 70, 70 );

            //bindings
            SharedDataDirText.DataBindings.Add( "Text", this, "SharedDataRootDir", false, DataSourceUpdateMode.OnPropertyChanged);
            btnColor1.DataBindings.Add( "BackColor", this, "BackgroundColor", false, DataSourceUpdateMode.OnPropertyChanged);
            btnColor2.DataBindings.Add( "BackColor", this, "BackgroundColor2", false, DataSourceUpdateMode.OnPropertyChanged);
            cmbGradientType.DataBindings.Add( "SelectedItem", this, "GradientString", false, DataSourceUpdateMode.OnPropertyChanged);
            checkBoxPipeStart.DataBindings.Add( "Checked", this, "bOpenPipeOnStart", false, DataSourceUpdateMode.OnPropertyChanged );
            checkBoxStatusIconsEnabled.DataBindings.Add( "Checked", this, "bStatusIconsEnabled", false, DataSourceUpdateMode.OnPropertyChanged );
            numUpDownIconPadding.DataBindings.Add( "Value", this, "IconPadding", false, DataSourceUpdateMode.OnPropertyChanged );
            numUpDownIconMargin.DataBindings.Add( "Value", this, "IconMargin", false, DataSourceUpdateMode.OnPropertyChanged );
            btnColorConnection.DataBindings.Add( "BackColor", this, "ConnectionIconColor", false, DataSourceUpdateMode.OnPropertyChanged);
            btnColorInData.DataBindings.Add( "BackColor", this, "IncomingDataIconColor", false, DataSourceUpdateMode.OnPropertyChanged);
            btnColorOutData.DataBindings.Add( "BackColor", this, "OutgoingDataIconColor", false, DataSourceUpdateMode.OnPropertyChanged);
            btnColorWarning.DataBindings.Add( "BackColor", this, "WarningIconColor", false, DataSourceUpdateMode.OnPropertyChanged);
            btnColorError.DataBindings.Add( "BackColor", this, "ErrorIconColor", false, DataSourceUpdateMode.OnPropertyChanged);
            btnColorInactive.DataBindings.Add( "BackColor", this, "InactiveIconColor", false, DataSourceUpdateMode.OnPropertyChanged);
        }
        
        public void SetSettings(XmlNode node)
        {
            var element = (XmlElement)node;
            SharedDataRootDir = SettingsHelper.ParseString(element["SharedDataRootDir"]);
            BackgroundColor = SettingsHelper.ParseColor(element["BackgroundColor"]);
            BackgroundColor2 = SettingsHelper.ParseColor(element["BackgroundColor2"]);
            GradientString = SettingsHelper.ParseString(element["BackgroundGradient"]);
            bOpenPipeOnStart = SettingsHelper.ParseBool(element["bOpenPipeOnStart"]);
            bStatusIconsEnabled = SettingsHelper.ParseBool(element["bStatusIconsEnabled"]);
            IconPadding = SettingsHelper.ParseInt(element["IconPadding"]);
            IconMargin = SettingsHelper.ParseInt(element["IconMargin"]);
            ConnectionIconColor = SettingsHelper.ParseColor(element["ConnectionIconColor"]);
            IncomingDataIconColor = SettingsHelper.ParseColor(element["IncomingDataIconColor"]);
            OutgoingDataIconColor = SettingsHelper.ParseColor(element["OutgoingDataIconColor"]);
            WarningIconColor = SettingsHelper.ParseColor(element["WarningIconColor"]);
            ErrorIconColor = SettingsHelper.ParseColor(element["ErrorIconColor"]);
            InactiveIconColor = SettingsHelper.ParseColor(element["InactiveIconColor"]);
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
            return SettingsHelper.CreateSetting( document, parent, "Version",               "1.0"                 ) ^
                   SettingsHelper.CreateSetting( document, parent, "SharedDataRootDir",     SharedDataRootDir     ) ^
                   SettingsHelper.CreateSetting( document, parent, "BackgroundColor",       BackgroundColor       ) ^
                   SettingsHelper.CreateSetting( document, parent, "BackgroundColor2",      BackgroundColor2      ) ^
                   SettingsHelper.CreateSetting( document, parent, "BackgroundGradient",    BackgroundGradient    ) ^
                   SettingsHelper.CreateSetting( document, parent, "bOpenPipeOnStart",      bOpenPipeOnStart      ) ^
                   SettingsHelper.CreateSetting( document, parent, "bStatusIconsEnabled",   bStatusIconsEnabled   ) ^
                   SettingsHelper.CreateSetting( document, parent, "ConnectionIconColor",   ConnectionIconColor   ) ^
                   SettingsHelper.CreateSetting( document, parent, "IconPadding",           IconPadding           ) ^
                   SettingsHelper.CreateSetting( document, parent, "IconMargin",            IconMargin            ) ^
                   SettingsHelper.CreateSetting( document, parent, "IncomingDataIconColor", IncomingDataIconColor ) ^
                   SettingsHelper.CreateSetting( document, parent, "OutgoingDataIconColor", OutgoingDataIconColor ) ^
                   SettingsHelper.CreateSetting( document, parent, "WarningIconColor",      WarningIconColor      ) ^
                   SettingsHelper.CreateSetting( document, parent, "ErrorIconColor",        ErrorIconColor        ) ^
                   SettingsHelper.CreateSetting( document, parent, "InactiveIconColor",     InactiveIconColor     );
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
            checkBoxStatusIconsEnabled_CheckedChanged(null, null);
        }

        /* other UI settings */
        
        private void checkBoxPipeStart_CheckedChanged(object sender, EventArgs e)
        {
            //bOpenPipeOnStart = checkBoxPipeStart.Checked;
        }
        private void checkBoxStatusIconsEnabled_CheckedChanged(object sender, EventArgs e)
        {
            numUpDownIconMargin.Enabled = checkBoxStatusIconsEnabled.Checked;
            numUpDownIconPadding.Enabled = checkBoxStatusIconsEnabled.Checked;
            labelColorConnection.Enabled = checkBoxStatusIconsEnabled.Checked;
            btnColorConnection.Enabled = checkBoxStatusIconsEnabled.Checked;
            labelColorInData.Enabled = checkBoxStatusIconsEnabled.Checked;
            btnColorInData.Enabled = checkBoxStatusIconsEnabled.Checked;
            labelColorOutData.Enabled = checkBoxStatusIconsEnabled.Checked;
            btnColorOutData.Enabled = checkBoxStatusIconsEnabled.Checked;
            labelColorWarning.Enabled = checkBoxStatusIconsEnabled.Checked;
            btnColorWarning.Enabled = checkBoxStatusIconsEnabled.Checked;
            labelColorError.Enabled = checkBoxStatusIconsEnabled.Checked;
            btnColorError.Enabled = checkBoxStatusIconsEnabled.Checked;
            labelColorInactive.Enabled = checkBoxStatusIconsEnabled.Checked;
            btnColorInactive.Enabled = checkBoxStatusIconsEnabled.Checked;
        }
    }
} //namespace LiveSplit.Lazysplits
