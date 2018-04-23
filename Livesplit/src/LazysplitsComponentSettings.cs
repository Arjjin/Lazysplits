using System;
using System.Windows.Forms;
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
        
        //NLog
        private static Logger Log = LogManager.GetCurrentClassLogger();

        public LazysplitsComponentSettings()
        {
            InitializeComponent();

            SharedDataRootDir = string.Empty;

            SharedDataDirText.DataBindings.Add("Text", this, "SharedDataRootDir", false, DataSourceUpdateMode.OnPropertyChanged);
        }
        
        public XmlNode GetSettings(XmlDocument document)
        {
            var settings_node = document.CreateElement("Settings");
            SettingsHelper.CreateSetting( document, settings_node, "Version", "1.0" );
            SettingsHelper.CreateSetting( document, settings_node, "SharedDataRootDir", SharedDataRootDir );

            return settings_node;
        }
        public void SetSettings(XmlNode node)
        {
            var element = (XmlElement)node;
            SharedDataRootDir = SettingsHelper.ParseString(element["SharedDataRootDir"]);
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
        
    }
} //namespace LiveSplit.Lazysplits
