using System;
using System.IO;
using System.Collections.Generic;
using System.Threading;
using System.Drawing;
using System.Drawing.Imaging;

using NLog;

using LiveSplit.UI;

namespace LiveSplit.Lazysplits
{
    class LzsStatusIcon
    {
        public Image Image { get; private set; }
        public bool bImageValid { get; private set; }
        public ImageAttributes ImageAttributes { get; private set; }
        public Color ActiveColor {get; private set; }
        public Color InactiveColor { get; private set; }
        private ColorMatrix ColorMatrix;
        public Mutex ColorMatrixMutex;
        public bool bActive { get; private set; }
        public long MsWhenChanged { get; private set; }
        public int ActiveMs { get; private set; }
        
        //NLog
        private static Logger Log = LogManager.GetCurrentClassLogger();

        public LzsStatusIcon( string imagePath, bool active, int activeMs )
        {
            bImageValid = true;
            try
            {
                Image = Image.FromFile(imagePath);
            }
            catch( FileNotFoundException e ){
                Log.Error(e.Message);
                bImageValid = false;
            }
            
            
            ImageAttributes = new ImageAttributes();
            ActiveColor = Color.White;
            InactiveColor = Color.DarkGray;
            ColorMatrix = new ColorMatrix();
            ColorMatrixMutex = new Mutex();
            ColorMatrix.Matrix00 = 1.0F;
            ColorMatrix.Matrix11 = 1.0F;
            ColorMatrix.Matrix22 = 1.0F;
            ColorMatrix.Matrix33 = 1.0F;
            ColorMatrix.Matrix44 = 1.0F;
            ImageAttributes.SetColorMatrix(ColorMatrix);

            bActive = active;
            ActiveMs = activeMs;
            MsWhenChanged = 0;

            ToggleEnabled(bActive);
        }

        private long GetCurrentMs()
        {
            return DateTime.Now.Ticks / TimeSpan.TicksPerMillisecond;
        }
        public void SetActiveColor( Color color )
        {
            ActiveColor = color;
            if(bActive){ SetImageColor(ActiveColor); }
        }
        public void SetInactiveColor( Color color )
        {
            InactiveColor = color;
            if(!bActive){ SetImageColor(InactiveColor); }
        }
        private void SetImageColor( Color color )
        {
            ColorMatrixMutex.WaitOne();

            ColorMatrix.Matrix40 = -1.0F + ( color.R*(1.0F/255.0F) );
            ColorMatrix.Matrix41 = -1.0F + ( color.G*(1.0F/255.0F) );
            ColorMatrix.Matrix42 = -1.0F + ( color.B*(1.0F/255.0F) );
            ColorMatrix.Matrix43 = -1.0F + ( color.A*(1.0F/255.0F) );
            ImageAttributes.SetColorMatrix(ColorMatrix);

            ColorMatrixMutex.ReleaseMutex();
        }
        public void Update()
        {
            if( bActive && ActiveMs > 0 )
            {
                if( GetCurrentMs() - MsWhenChanged >= ActiveMs )
                {
                    ToggleEnabled(false);
                }
            }
        }
        public void ToggleEnabled( bool enabled )
        {
            bActive = enabled;
            MsWhenChanged = GetCurrentMs();

            if(bActive)
            {
                SetImageColor(ActiveColor);
            }
            else{
                SetImageColor(InactiveColor);
            }
        }
    }
    class LzsStatusIcons
    {
        private GraphicsCache Cache;
        private List<LzsStatusIcon> Icons;
        private LzsStatusIcon ImageConnected;
        private LzsStatusIcon ImageInData;
        private LzsStatusIcon ImageOutData;
        private LzsStatusIcon ImageWarning;
        private LzsStatusIcon ImageError;

        public LzsStatusIcons()
        {
            Cache = new GraphicsCache();
            Icons = new List<LzsStatusIcon>();
            
            ImageConnected = new LzsStatusIcon( "components/Lazysplits/icons/icon_connected.png", false, 0 );
            Icons.Add(ImageConnected);
            ImageInData = new LzsStatusIcon( "components/Lazysplits/icons/icon_message_received.png", false, 30 );
            Icons.Add(ImageInData);
            ImageOutData = new LzsStatusIcon( "components/Lazysplits/icons/icon_message_sent.png", false, 30 );
            Icons.Add(ImageOutData);
            ImageWarning = new LzsStatusIcon( "components/Lazysplits/icons/icon_warning.png", false, 5000 );
            Icons.Add(ImageWarning);
            ImageError = new LzsStatusIcon( "components/Lazysplits/icons/icon_error.png", false, 5000 );
            Icons.Add(ImageError);
        }
        
        public bool CacheHasChanged()
        {
            Cache.Restart();
            Cache["ImageConnectedEnabled"] = ImageConnected.bActive;
            Cache["ImageInDataEnabled"] = ImageInData.bActive;
            Cache["ImageOutDataEnabled"] = ImageOutData.bActive;
            Cache["ImageWarning"] = ImageWarning.bActive;
            Cache["ImageError"] = ImageError.bActive;
            return Cache.HasChanged;
        }
        public void DrawStatusHorizontal( Graphics g, LazysplitsComponentSettings settings, float width, float height )
        {
            if( ImageConnected.ActiveColor != settings.ConnectionIconColor ){ ImageConnected.SetActiveColor(settings.ConnectionIconColor); }
            if( ImageInData.ActiveColor != settings.IncomingDataIconColor ){ ImageInData.SetActiveColor(settings.IncomingDataIconColor); }
            if( ImageOutData.ActiveColor != settings.OutgoingDataIconColor ){ ImageOutData.SetActiveColor(settings.OutgoingDataIconColor); }
            if( ImageWarning.ActiveColor != settings.WarningIconColor ){ ImageWarning.SetActiveColor(settings.WarningIconColor); }
            if( ImageError.ActiveColor != settings.ErrorIconColor ){ ImageError.SetActiveColor(settings.ErrorIconColor); }
            
            int StartY = 0;
            StartY += settings.IconMargin;
            foreach( var icon in Icons ){
                if( icon.bImageValid )
                {
                    if ( icon.InactiveColor != settings.InactiveIconColor ){ icon.SetInactiveColor(settings.InactiveIconColor); }
                
                    float ImageScaling = width/icon.Image.Width;
                    int DrawWidth = (int)( icon.Image.Width * ImageScaling );
                    int DrawHeight = (int)( icon.Image.Height * ImageScaling );
                    
                    icon.ColorMatrixMutex.WaitOne();
                    g.DrawImage(
                        icon.Image,
                        new Rectangle(
                            settings.IconPadding,
                            StartY + settings.IconPadding,
                            DrawWidth - (settings.IconPadding*2),
                            DrawHeight - (settings.IconPadding*2) 
                        ),
                        0,
                        0,
                        icon.Image.Width,
                        icon.Image.Height,
                        GraphicsUnit.Pixel,
                        icon.ImageAttributes
                    );
                    icon.ColorMatrixMutex.ReleaseMutex();

                    StartY += DrawHeight + settings.IconMargin;
                }
            }
        }
        public void DrawStatusVertical( Graphics g, LazysplitsComponentSettings settings, float width, float height )
        {
            if( ImageConnected.ActiveColor != settings.ConnectionIconColor ){ ImageConnected.SetActiveColor(settings.ConnectionIconColor); }
            if( ImageInData.ActiveColor != settings.IncomingDataIconColor ){ ImageInData.SetActiveColor(settings.IncomingDataIconColor); }
            if( ImageOutData.ActiveColor != settings.OutgoingDataIconColor ){ ImageOutData.SetActiveColor(settings.OutgoingDataIconColor); }
            if( ImageWarning.ActiveColor != settings.WarningIconColor ){ ImageWarning.SetActiveColor(settings.WarningIconColor); }
            if( ImageError.ActiveColor != settings.ErrorIconColor ){ ImageError.SetActiveColor(settings.ErrorIconColor); }

            int StartX = 0;
            StartX += settings.IconMargin;
            foreach( var icon in Icons ){
                if( icon.bImageValid )
                {
                    if ( icon.InactiveColor != settings.InactiveIconColor ){ icon.SetInactiveColor(settings.InactiveIconColor); }
                
                    float ImageScaling = height/icon.Image.Height;
                    int DrawWidth = (int)( icon.Image.Width * ImageScaling );
                    int DrawHeight = (int)( icon.Image.Height * ImageScaling );
                    
                    icon.ColorMatrixMutex.WaitOne();
                    g.DrawImage(
                        icon.Image,
                        new Rectangle(
                            StartX + settings.IconPadding,
                            settings.IconPadding,
                            DrawWidth - (settings.IconPadding*2),
                            DrawHeight - (settings.IconPadding*2) 
                        ),
                        0,
                        0,
                        icon.Image.Width,
                        icon.Image.Height,
                        GraphicsUnit.Pixel,
                        icon.ImageAttributes
                    );
                    icon.ColorMatrixMutex.ReleaseMutex();

                    StartX += DrawWidth + settings.IconMargin;
                }
            }
        }
        public void Update()
        {
            foreach( var icon in Icons ){
                if(icon.bImageValid){ icon.Update(); }
            }
        }
        
        public void Connected()
        {
            ImageConnected.ToggleEnabled(true);
        }
        public void Disconnected()
        {
            ImageConnected.ToggleEnabled(false);
        }
        public void MessageReceived()
        {
            ImageInData.ToggleEnabled(true);
        }
        public void MessageSent()
        {
            ImageOutData.ToggleEnabled(true);
        }
        public void Warning()
        {
            ImageWarning.ToggleEnabled(true);
        }
        public void Error()
        {
            ImageError.ToggleEnabled(true);
        }
    }
}
