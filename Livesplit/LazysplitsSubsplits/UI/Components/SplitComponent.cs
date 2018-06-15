using LiveSplit.Model;
using LiveSplit.TimeFormatters;
using System;
using System.Collections.Generic;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Drawing.Imaging;
using System.Linq;
using System.Windows.Forms;
using System.Text.RegularExpressions;

using LiveSplit.UI;
using LiveSplit.UI.Components;

namespace LiveSplit.Lazysplits.Subsplits
{
    public class SplitComponent : IComponent
    {
        public bool Header { get; set; }
        public int TopSplit { get; set; }
        public bool ForceIndent { get; set; }
        public bool CollapsedSplit { get; set; }
        public bool oddSplit { get; set; }

        private bool Indent { get { return Settings.IndentSubsplits && (IsSubsplit || ForceIndent); } }

        public ISegment Split { get; set; }
        protected bool blankOut = false;

        protected SimpleLabel NameLabel { get; set; }
        protected SimpleLabel TimeLabel { get; set; }
        protected SimpleLabel MeasureTimeLabel { get; set; }
        protected SimpleLabel MeasureDeltaLabel { get; set; }
        protected SimpleLabel DeltaLabel { get; set; }
        public SplitsSettings Settings { get; set; }

        protected int FrameCount { get; set; }

        public GraphicsCache Cache { get; set; }
        protected bool NeedUpdateAll { get; set; }
        protected bool IsActive { get; set; }
        protected bool IsHighlight { get; set; }
        protected bool IsSubsplit { get; set; }

        protected TimeAccuracy CurrentAccuracy { get; set; }
        protected TimeAccuracy CurrentDeltaAccuracy { get; set; }
        protected TimeAccuracy CurrentHeaderTimesAccuracy { get; set; }
        protected TimeAccuracy CurrentSectionTimerAccuracy { get; set; }
        protected bool CurrentDropDecimals { get; set; }

        protected ITimeFormatter TimeFormatter { get; set; }
        protected ITimeFormatter DeltaTimeFormatter { get; set; }
        protected ITimeFormatter HeaderTimesFormatter { get; set; }
        protected ITimeFormatter SectionTimerFormatter { get; set; }

        protected int IconWidth => DisplayIcon ? (int)(Settings.IconSize + 7.5f) : 0;

        public bool DisplayIcon { get; set; }

        public Image ShadowImage { get; set; }
        protected Image OldImage { get; set; }

        public float PaddingTop => 0f;
        public float PaddingLeft => 0f;
        public float PaddingBottom => 0f;
        public float PaddingRight => 0f;

        public IEnumerable<ColumnData> ColumnsList { get; set; }
        public IList<SimpleLabel> LabelsList { get; set; }

        private Regex SubsplitRegex = new Regex(@"^{(.+)}\s*(.+)$", RegexOptions.Compiled);

        public float VerticalHeight { get; set; }

        public float MinimumWidth => Math.Max(CalculateHeaderWidth(), CalculateLabelsWidth()) + IconWidth + 10;

        public float HorizontalWidth
            => Math.Max(CalculateHeaderWidth(), CalculateLabelsWidth()) + Settings.SplitWidth + IconWidth;

        public float MinimumHeight { get; set; }

        public IDictionary<string, Action> ContextMenuControls => null;

        public SplitComponent(SplitsSettings settings, IEnumerable<ColumnData> columnsList)
        {
            NameLabel = new SimpleLabel()
            {
                HorizontalAlignment = StringAlignment.Near,
                X = 8,
                Y = 3,
                Text = ""
            };
            TimeLabel = new SimpleLabel()
            {
                HorizontalAlignment = StringAlignment.Far,
                Y = 3,
                Text = ""
            };
            MeasureTimeLabel = new SimpleLabel();
            DeltaLabel = new SimpleLabel()
            {
                HorizontalAlignment = StringAlignment.Far,
                Y = 3,
                Text = ""
            };
            MeasureDeltaLabel = new SimpleLabel();
            Settings = settings;
            ColumnsList = columnsList;
            TimeFormatter = new RegularSplitTimeFormatter(Settings.SplitTimesAccuracy);
            DeltaTimeFormatter = new DeltaSplitTimeFormatter(Settings.DeltasAccuracy, Settings.DropDecimals);
            HeaderTimesFormatter = new RegularSplitTimeFormatter(Settings.HeaderAccuracy);
            SectionTimerFormatter = new RegularSplitTimeFormatter(Settings.SectionTimerAccuracy);
            MinimumHeight = 25;
            VerticalHeight = 31;

            NeedUpdateAll = true;
            IsActive = false;

            Cache = new GraphicsCache();
            LabelsList = new List<SimpleLabel>();
        }

        private void DrawGeneral(Graphics g, LiveSplitState state, float width, float height, LayoutMode mode, Region clipRegion)
        {
            if (NeedUpdateAll)
                UpdateAll(state);

            if (Settings.BackgroundGradient == ExtendedGradientType.Alternating)
                g.FillRectangle(new SolidBrush(
                    oddSplit
                    ? Settings.BackgroundColor
                    : Settings.BackgroundColor2
                    ), 0, 0, width, height);

            if ((IsSubsplit || ForceIndent) && (Settings.OverrideSubsplitColor))
            {
                var gradientBrush = new LinearGradientBrush(
                    new PointF(0, 0),
                    Settings.SubsplitGradient == GradientType.Horizontal
                    ? new PointF(width, 0)
                    : new PointF(0, height),
                    Settings.SubsplitTopColor,
                    Settings.SubsplitGradient == GradientType.Plain
                    ? Settings.SubsplitTopColor
                    : Settings.SubsplitBottomColor);
                g.FillRectangle(gradientBrush, 0, 0, width, height);
            }

            MeasureTimeLabel.Text = TimeFormatter.Format(new TimeSpan(24, 0, 0));
            MeasureDeltaLabel.Text = DeltaTimeFormatter.Format(new TimeSpan(0, 9, 0, 0));

            MeasureTimeLabel.Font = state.LayoutSettings.TimesFont;
            MeasureTimeLabel.IsMonospaced = true;
            MeasureDeltaLabel.Font = state.LayoutSettings.TimesFont;
            MeasureDeltaLabel.IsMonospaced = true;

            MeasureTimeLabel.SetActualWidth(g);
            MeasureDeltaLabel.SetActualWidth(g);
            TimeLabel.SetActualWidth(g);
            DeltaLabel.SetActualWidth(g);

            NameLabel.ShadowColor = state.LayoutSettings.ShadowsColor;
            NameLabel.OutlineColor = state.LayoutSettings.TextOutlineColor;
            foreach (var label in LabelsList)
            {
                label.ShadowColor = state.LayoutSettings.ShadowsColor;
                label.OutlineColor = state.LayoutSettings.TextOutlineColor;
            }

            if (Settings.SplitTimesAccuracy != CurrentAccuracy)
            {
                TimeFormatter = new RegularSplitTimeFormatter(Settings.SplitTimesAccuracy);
                CurrentAccuracy = Settings.SplitTimesAccuracy;
            }
            if (Settings.DeltasAccuracy != CurrentDeltaAccuracy || Settings.DropDecimals != CurrentDropDecimals)
            {
                DeltaTimeFormatter = new DeltaSplitTimeFormatter(Settings.DeltasAccuracy, Settings.DropDecimals);
                CurrentDeltaAccuracy = Settings.DeltasAccuracy;
                CurrentDropDecimals = Settings.DropDecimals;
            }
            if (Settings.HeaderAccuracy != CurrentHeaderTimesAccuracy)
            {
                HeaderTimesFormatter = new RegularSplitTimeFormatter(Settings.HeaderAccuracy);
                CurrentHeaderTimesAccuracy = Settings.HeaderAccuracy;
            }
            if (Settings.SectionTimerAccuracy != CurrentSectionTimerAccuracy)
            {
                SectionTimerFormatter = new RegularSplitTimeFormatter(Settings.SectionTimerAccuracy);
                CurrentSectionTimerAccuracy = Settings.SectionTimerAccuracy;
            }

            if (Split != null)
            {

                if (mode == LayoutMode.Vertical)
                {
                    NameLabel.VerticalAlignment = StringAlignment.Center;
                    NameLabel.Y = 0;
                    NameLabel.Height = height;
                    foreach (var label in LabelsList)
                    {
                        label.VerticalAlignment = StringAlignment.Center;
                        label.Y = 0;
                        label.Height = height;
                    }
                }
                else
                {
                    NameLabel.VerticalAlignment = StringAlignment.Near;
                    NameLabel.Y = 0;
                    NameLabel.Height = 50;
                    foreach (var label in LabelsList)
                    {
                        label.VerticalAlignment = StringAlignment.Far;
                        label.Y = height - 50;
                        label.Height = 50;
                    }
                }

                if (IsActive)
                {
                    var currentSplitBrush = new LinearGradientBrush(
                        new PointF(0, 0),
                        Settings.CurrentSplitGradient == GradientType.Horizontal
                        ? new PointF(width, 0)
                        : new PointF(0, height),
                        Settings.CurrentSplitTopColor,
                        Settings.CurrentSplitGradient == GradientType.Plain
                        ? Settings.CurrentSplitTopColor
                        : Settings.CurrentSplitBottomColor);
                    g.FillRectangle(currentSplitBrush, 0, 0, width, height);
                }

                if (IsHighlight)
                {
                    Pen highlightPen = new Pen(Color.White);
                    g.DrawRectangle(highlightPen, 0, 0, width - 1, height - 1);
                }

                var icon = Split.Icon;
                if (DisplayIcon && icon != null)
                {
                    var shadow = ShadowImage;

                    if (OldImage != icon)
                    {
                        ImageAnimator.Animate(icon, (s, o) => { });
                        ImageAnimator.Animate(shadow, (s, o) => { });
                        OldImage = icon;
                    }

                    var drawWidth = Settings.IconSize;
                    var drawHeight = Settings.IconSize;
                    var shadowWidth = Settings.IconSize * (5 / 4f);
                    var shadowHeight = Settings.IconSize * (5 / 4f);
                    if (icon.Width > icon.Height)
                    {
                        var ratio = icon.Height / (float)icon.Width;
                        drawHeight *= ratio;
                        shadowHeight *= ratio;
                    }
                    else
                    {
                        var ratio = icon.Width / (float)icon.Height;
                        drawWidth *= ratio;
                        shadowWidth *= ratio;
                    }

                    ImageAnimator.UpdateFrames(shadow);
                    if (Settings.IconShadows && shadow != null)
                    {
                        g.DrawImage(
                            shadow,
                            7 + (Settings.IconSize * (5 / 4f) - shadowWidth) / 2 - 0.7f + (Indent ? 20 : 0),
                            (height - Settings.IconSize) / 2.0f + (Settings.IconSize * (5 / 4f) - shadowHeight) / 2 - 0.7f,
                            shadowWidth,
                            shadowHeight);
                    }

                    ImageAnimator.UpdateFrames(icon);

                    g.DrawImage(
                        icon,
                        7 + (Settings.IconSize - drawWidth) / 2 + (Indent ? 20 : 0),
                        (height - Settings.IconSize) / 2.0f + (Settings.IconSize - drawHeight) / 2,
                        drawWidth,
                        drawHeight);
                }

                NameLabel.Font = state.LayoutSettings.TextFont;
                NameLabel.HasShadow = state.LayoutSettings.DropShadows;

                if (ColumnsList.Count() == LabelsList.Count)
                {
                    var curX = width - 7;
                    var nameX = width - 7;
                    foreach (var label in LabelsList.Reverse())
                    {
                        var column = ColumnsList.ElementAt(LabelsList.IndexOf(label));

                        var labelWidth = 0f;
                        if (column.Type == ColumnType.DeltaorSplitTime || column.Type == ColumnType.SegmentDeltaorSegmentTime)
                            labelWidth = Math.Max(MeasureDeltaLabel.ActualWidth, MeasureTimeLabel.ActualWidth);
                        else if (column.Type == ColumnType.Delta || column.Type == ColumnType.SegmentDelta)
                            labelWidth = MeasureDeltaLabel.ActualWidth;
                        else
                            labelWidth = MeasureTimeLabel.ActualWidth;
                        label.Width = labelWidth + 20;
                        curX -= labelWidth + 5;
                        label.X = curX - 15;

                        label.Font = state.LayoutSettings.TimesFont;
                        label.HasShadow = state.LayoutSettings.DropShadows;
                        label.IsMonospaced = true;
                        label.Draw(g);

                        if (!string.IsNullOrEmpty(label.Text))
                            nameX = curX + labelWidth + 5 - label.ActualWidth;
                    }

                    NameLabel.Width = (mode == LayoutMode.Horizontal ? width - 10 : nameX) - IconWidth;
                    NameLabel.X = 5 + IconWidth;
                    if (Indent)
                    {
                        NameLabel.Width -= 20;
                        NameLabel.X += 20;
                    }

                    NameLabel.Draw(g);
                }
            }
        }

        private void DrawHeader(Graphics g, LiveSplitState state, float width, float height, LayoutMode mode, Region clipRegion)
        {
            if (Settings.BackgroundGradient == ExtendedGradientType.Alternating)
                g.FillRectangle(new SolidBrush(
                    oddSplit
                    ? Settings.BackgroundColor
                    : Settings.BackgroundColor2
                    ), 0, 0, width, height);

            var currentSplitBrush = new LinearGradientBrush(
                new PointF(0, 0),
                Settings.HeaderGradient == GradientType.Horizontal
                ? new PointF(width, 0)
                : new PointF(0, height),
                Settings.HeaderTopColor,
                Settings.HeaderGradient == GradientType.Plain
                ? Settings.HeaderTopColor
                : Settings.HeaderBottomColor);
            g.FillRectangle(currentSplitBrush, 0, 0, width, height);            

            MeasureTimeLabel.Text = HeaderTimesFormatter.Format(new TimeSpan(24, 0, 0));
            MeasureDeltaLabel.Text = SectionTimerFormatter.Format(new TimeSpan(0, 9, 0, 0));

            MeasureTimeLabel.Font = state.LayoutSettings.TimesFont;
            MeasureTimeLabel.IsMonospaced = true;
            MeasureDeltaLabel.Font = state.LayoutSettings.TimesFont;
            MeasureDeltaLabel.IsMonospaced = true;

            MeasureTimeLabel.SetActualWidth(g);
            MeasureDeltaLabel.SetActualWidth(g);
            TimeLabel.SetActualWidth(g);
            DeltaLabel.SetActualWidth(g);

            NameLabel.ShadowColor = state.LayoutSettings.ShadowsColor;
            NameLabel.OutlineColor = state.LayoutSettings.TextOutlineColor;
            TimeLabel.ShadowColor = state.LayoutSettings.ShadowsColor;
            TimeLabel.OutlineColor = state.LayoutSettings.TextOutlineColor;
            DeltaLabel.ShadowColor = state.LayoutSettings.ShadowsColor;
            DeltaLabel.OutlineColor = state.LayoutSettings.TextOutlineColor;

            if (Settings.SplitTimesAccuracy != CurrentAccuracy)
            {
                TimeFormatter = new RegularSplitTimeFormatter(Settings.SplitTimesAccuracy);
                CurrentAccuracy = Settings.SplitTimesAccuracy;
            }
            if (Settings.DeltasAccuracy != CurrentDeltaAccuracy || Settings.DropDecimals != CurrentDropDecimals)
            {
                DeltaTimeFormatter = new DeltaSplitTimeFormatter(Settings.DeltasAccuracy, Settings.DropDecimals);
                CurrentDeltaAccuracy = Settings.DeltasAccuracy;
                CurrentDropDecimals = Settings.DropDecimals;
            }
            if (Settings.HeaderAccuracy != CurrentHeaderTimesAccuracy)
            {
                HeaderTimesFormatter = new RegularSplitTimeFormatter(Settings.HeaderAccuracy);
                CurrentHeaderTimesAccuracy = Settings.HeaderAccuracy;
            }
            if (Settings.SectionTimerAccuracy != CurrentSectionTimerAccuracy)
            {
                SectionTimerFormatter = new RegularSplitTimeFormatter(Settings.SectionTimerAccuracy);
                CurrentSectionTimerAccuracy = Settings.SectionTimerAccuracy;
            }
            
            if (mode == LayoutMode.Vertical)
            {
                NameLabel.VerticalAlignment = StringAlignment.Center;
                DeltaLabel.VerticalAlignment = StringAlignment.Center;
                TimeLabel.VerticalAlignment = StringAlignment.Center;
                NameLabel.Y = 0;
                DeltaLabel.Y = 0;
                TimeLabel.Y = 0;
                NameLabel.Height = height;
                DeltaLabel.Height = height;
                TimeLabel.Height = height;
            }
            else
            {
                NameLabel.VerticalAlignment = StringAlignment.Near;
                DeltaLabel.VerticalAlignment = StringAlignment.Far;
                TimeLabel.VerticalAlignment = StringAlignment.Far;
                NameLabel.Y = 0;
                DeltaLabel.Y = height - 50;
                TimeLabel.Y = height - 50;
                NameLabel.Height = 50;
                DeltaLabel.Height = 50;
                TimeLabel.Height = 50;
            }

            var icon = Split.Icon;
            if (DisplayIcon && icon != null)
            {
                var shadow = ShadowImage;

                if (OldImage != icon)
                {
                    ImageAnimator.Animate(icon, (s, o) => { });
                    ImageAnimator.Animate(shadow, (s, o) => { });
                    OldImage = icon;
                }

                var drawWidth = Settings.IconSize;
                var drawHeight = Settings.IconSize;
                var shadowWidth = Settings.IconSize * (5 / 4f);
                var shadowHeight = Settings.IconSize * (5 / 4f);
                if (icon.Width > icon.Height)
                {
                    var ratio = icon.Height / (float)icon.Width;
                    drawHeight *= ratio;
                    shadowHeight *= ratio;
                }
                else
                {
                    var ratio = icon.Width / (float)icon.Height;
                    drawWidth *= ratio;
                    shadowWidth *= ratio;
                }

                ImageAnimator.UpdateFrames(shadow);
                if (Settings.IconShadows && shadow != null)
                {
                    g.DrawImage(
                        shadow,
                        7 + (Settings.IconSize * (5 / 4f) - shadowWidth) / 2 - 0.7f + ((Settings.IndentSubsplits && IsSubsplit) ? 20 : 0),
                        (height - Settings.IconSize) / 2.0f + (Settings.IconSize * (5 / 4f) - shadowHeight) / 2 - 0.7f,
                        shadowWidth,
                        shadowHeight);
                }

                ImageAnimator.UpdateFrames(icon);

                g.DrawImage(
                    icon,
                    7 + (Settings.IconSize - drawWidth) / 2 + ((Settings.IndentSubsplits && IsSubsplit) ? 20 : 0),
                    (height - Settings.IconSize) / 2.0f + (Settings.IconSize - drawHeight) / 2,
                    drawWidth,
                    drawHeight);
            }

            NameLabel.Font = state.LayoutSettings.TextFont;

            NameLabel.X = 5 + IconWidth;
            NameLabel.HasShadow = state.LayoutSettings.DropShadows;

            TimeLabel.Font = state.LayoutSettings.TimesFont;

            TimeLabel.Width = Math.Max(MeasureDeltaLabel.ActualWidth, MeasureTimeLabel.ActualWidth) + 20;
            TimeLabel.X = width - Math.Max(MeasureDeltaLabel.ActualWidth, MeasureTimeLabel.ActualWidth) - 27;

            TimeLabel.HasShadow = state.LayoutSettings.DropShadows;
            TimeLabel.IsMonospaced = true;

            DeltaLabel.Font = state.LayoutSettings.TimesFont;
            DeltaLabel.X = width - MeasureTimeLabel.ActualWidth - MeasureDeltaLabel.ActualWidth - 32;
            DeltaLabel.Width = MeasureDeltaLabel.ActualWidth + 20;
            DeltaLabel.HasShadow = state.LayoutSettings.DropShadows;
            DeltaLabel.IsMonospaced = true;

            NameLabel.Width = width - IconWidth - (mode == LayoutMode.Vertical ? DeltaLabel.ActualWidth + (string.IsNullOrEmpty(DeltaLabel.Text) ? TimeLabel.ActualWidth : MeasureTimeLabel.ActualWidth + 5) + 10 : 10);

            Color originalColor = DeltaLabel.ForeColor;
            if (Settings.SectionTimer && Settings.SectionTimerGradient)
            {
                var bigFont = state.LayoutSettings.TimerFont;
                var sizeMultiplier = bigFont.Size / (bigFont.FontFamily.GetEmHeight(bigFont.Style));
                var ascent = sizeMultiplier * bigFont.FontFamily.GetCellAscent(bigFont.Style);
                var descent = sizeMultiplier * bigFont.FontFamily.GetCellDescent(bigFont.Style);
                
                if (state.Run.IndexOf(Split) >= state.CurrentSplitIndex)
                {
                    double h, s, v;
                    originalColor.ToHSV(out h, out s, out v);

                    Color bottomColor = ColorExtensions.FromHSV(h, s, 0.8 * v);
                    Color topColor = ColorExtensions.FromHSV(h, 0.5 * s, Math.Min(1, 1.5 * v + 0.1));

                    var bigTimerGradiantBrush = new LinearGradientBrush(
                        new PointF(DeltaLabel.X, DeltaLabel.Y),
                        new PointF(DeltaLabel.X, DeltaLabel.Y + ascent + descent),
                        topColor,
                        bottomColor);

                    DeltaLabel.Brush = bigTimerGradiantBrush;
                }
            }

            NameLabel.Draw(g);
            TimeLabel.Draw(g);
            DeltaLabel.Draw(g);

            DeltaLabel.Brush = new SolidBrush(originalColor);
        }

        public void DrawVertical(Graphics g, LiveSplitState state, float width, Region clipRegion)
        {
            if (Settings.Display2Rows)
            {
                VerticalHeight = Settings.SplitHeight + 0.85f * (g.MeasureString("A", state.LayoutSettings.TimesFont).Height + g.MeasureString("A", state.LayoutSettings.TextFont).Height);
                if (Header)
                    DrawHeader(g, state, width, VerticalHeight, LayoutMode.Horizontal, clipRegion);
                else
                    DrawGeneral(g, state, width, VerticalHeight, LayoutMode.Horizontal, clipRegion);
            }
            else
            {
                VerticalHeight = Settings.SplitHeight + 25;
                if (Header)
                    DrawHeader(g, state, width, VerticalHeight, LayoutMode.Vertical, clipRegion);
                else
                    DrawGeneral(g, state, width, VerticalHeight, LayoutMode.Vertical, clipRegion);
            }
        }

        public void DrawHorizontal(Graphics g, LiveSplitState state, float height, Region clipRegion)
        {
            MinimumHeight = 0.85f * (g.MeasureString("A", state.LayoutSettings.TimesFont).Height + g.MeasureString("A", state.LayoutSettings.TextFont).Height);
            if (Header)
                DrawHeader(g, state, HorizontalWidth, height, LayoutMode.Horizontal, clipRegion);
            else
                DrawGeneral(g, state, HorizontalWidth, height, LayoutMode.Horizontal, clipRegion);
        }

        public string ComponentName => "Split";


        public Control GetSettingsControl(LayoutMode mode)
        {
            throw new NotSupportedException();
        }

        public void SetSettings(System.Xml.XmlNode settings)
        {
            throw new NotSupportedException();
        }


        public System.Xml.XmlNode GetSettings(System.Xml.XmlDocument document)
        {
            throw new NotSupportedException();
        }

        public string UpdateName
        {
            get { throw new NotSupportedException(); }
        }

        public string XMLURL
        {
            get { throw new NotSupportedException(); }
        }

        public string UpdateURL
        {
            get { throw new NotSupportedException(); }
        }

        public Version Version
        {
            get { throw new NotSupportedException(); }
        }

        private TimeSpan? getSectionTime(LiveSplitState state, int splitNumber, int topNumber, string comparison, TimingMethod method)
        {
            if (topNumber > state.CurrentSplitIndex)
                return null;

            if (splitNumber < state.CurrentSplitIndex)
                return (state.Run[splitNumber].SplitTime[method] - (topNumber > 0 ? state.Run[topNumber - 1].SplitTime[method] : TimeSpan.Zero));

            //equal
            return state.CurrentTime[method] - (topNumber > 0 ? state.Run[topNumber - 1].SplitTime[method] : TimeSpan.Zero);
        }

        private TimeSpan? getSectionComparison(LiveSplitState state, int splitNumber, int topNumber, string comparison, TimingMethod method)
        {
            return (state.Run[splitNumber].Comparisons[comparison][method] - (topNumber > 0 ? state.Run[topNumber - 1].Comparisons[comparison][method] : TimeSpan.Zero));
        }
        
        private TimeSpan? getSectionDelta(LiveSplitState state, int splitNumber, int topNumber, string comparison, TimingMethod method)
        {
            return getSectionTime(state, splitNumber, topNumber, comparison, method) - getSectionComparison(state, splitNumber, topNumber, comparison, method);
        }

        private Color? GetSectionColor(LiveSplitState state, TimeSpan? timeDifference, TimeSpan? delta)
        {
            Color? splitColor = null;

            if (timeDifference != null)
            {
                if (timeDifference < TimeSpan.Zero)
                {
                    splitColor = state.LayoutSettings.AheadGainingTimeColor;
                    if (delta != null && delta > TimeSpan.Zero)
                        splitColor = state.LayoutSettings.AheadLosingTimeColor;
                }
                else
                {
                    splitColor = state.LayoutSettings.BehindLosingTimeColor;
                    if (delta != null && delta < TimeSpan.Zero)
                        splitColor = state.LayoutSettings.BehindGainingTimeColor;
                }
            }
            else
            {
                if (delta != null)
                    if (delta < TimeSpan.Zero)
                        splitColor = state.LayoutSettings.AheadGainingTimeColor;
                    else
                        splitColor = state.LayoutSettings.BehindLosingTimeColor;
            }

            return splitColor;
        }

        protected void UpdateAll(LiveSplitState state)
        {
            if (Split != null)
            {
                IsActive = (state.CurrentPhase == TimerPhase.Running
                            || state.CurrentPhase == TimerPhase.Paused) &&
                            ((!Settings.HideSubsplits && state.CurrentSplit == Split) ||
                            (SplitsSettings.SectionSplit != null && SplitsSettings.SectionSplit == Split));
                IsHighlight = (SplitsSettings.HilightSplit == Split);
                IsSubsplit = Split.Name.StartsWith("-") && Split != state.Run.Last();

                if (IsSubsplit)
                    NameLabel.Text = Split.Name.Substring(1);
                else
                {
                    Match match = SubsplitRegex.Match(Split.Name);
                    if (match.Success) {
                        if (CollapsedSplit || Header)
                            NameLabel.Text = match.Groups[1].Value;
                        else
                            NameLabel.Text = match.Groups[2].Value;
                    } else
                        NameLabel.Text = Split.Name;
                }
                
                if( NameLabel.Text.Contains("$lzs") && NameLabel.Text.Contains("[") && NameLabel.Text.Contains("]") )
                {
                    int SplitDataStart = NameLabel.Text.IndexOf("$lzs");
                    int SplitDataLength = NameLabel.Text.IndexOf("]") - SplitDataStart + 1;
                    NameLabel.Text = NameLabel.Text.Remove( SplitDataStart, SplitDataLength );
                }

                var splitIndex = state.Run.IndexOf(Split);

                if (Header)
                {
                    var comparison = Settings.HeaderComparison == "Current Comparison" ? state.CurrentComparison : Settings.HeaderComparison;
                    if (!state.Run.Comparisons.Contains(comparison))
                        comparison = state.CurrentComparison;

                    var timingMethod = state.CurrentTimingMethod;
                    if (Settings.HeaderTimingMethod == "Real Time")
                        timingMethod = TimingMethod.RealTime;
                    else if (Settings.HeaderTimingMethod == "Game Time")
                        timingMethod = TimingMethod.GameTime;

                    TimeSpan? deltaTime = getSectionDelta(state, splitIndex, TopSplit, comparison, timingMethod);
                    if ((splitIndex >= state.CurrentSplitIndex) && (deltaTime < TimeSpan.Zero))
                    {
                        deltaTime = null;
                    }

                    var color = GetSectionColor(state, null, deltaTime);
                    if (color == null)
                        color = Settings.OverrideHeaderColor ? Settings.HeaderTimesColor : state.LayoutSettings.TextColor;
                    TimeLabel.ForeColor = color.Value;
                    NameLabel.ForeColor = Settings.OverrideHeaderColor ? Settings.HeaderTextColor : state.LayoutSettings.TextColor;

                    if (deltaTime != null)
                        TimeLabel.Text = DeltaTimeFormatter.Format(deltaTime);
                    else
                        if (splitIndex < state.CurrentSplitIndex)
                            TimeLabel.Text = TimeFormatConstants.DASH;
                        else
                            TimeLabel.Text = HeaderTimesFormatter.Format(getSectionComparison(state, splitIndex, TopSplit, comparison, timingMethod));

                    TimeSpan? sectionTime = getSectionTime(state, splitIndex, TopSplit, comparison, timingMethod);
                    DeltaLabel.Text = SectionTimerFormatter.Format(sectionTime);
                    if (splitIndex < state.CurrentSplitIndex)
                        DeltaLabel.ForeColor = (Settings.OverrideHeaderColor ? Settings.HeaderTimesColor : state.LayoutSettings.TextColor);
                    else
                        DeltaLabel.ForeColor = Settings.SectionTimerColor;

                    if (!Settings.HeaderText)
                        NameLabel.Text = "";
                    if (!Settings.HeaderTimes)
                        TimeLabel.Text = "";
                    if (!Settings.SectionTimer)
                        DeltaLabel.Text = "";
                }
                else
                {
                    RecreateLabels();

                    if (splitIndex < state.CurrentSplitIndex)
                    {
                        NameLabel.ForeColor = Settings.OverrideTextColor ? Settings.BeforeNamesColor : state.LayoutSettings.TextColor;
                    }
                    else
                    {
                        if (IsActive)
                            NameLabel.ForeColor = Settings.OverrideTextColor ? Settings.CurrentNamesColor : state.LayoutSettings.TextColor;
                        else
                            NameLabel.ForeColor = Settings.OverrideTextColor ? Settings.AfterNamesColor : state.LayoutSettings.TextColor;
                    }

                    foreach (var label in LabelsList)
                    {
                        var column = ColumnsList.ElementAt(LabelsList.IndexOf(label));
                        if (CollapsedSplit)
                            UpdateCollapsedColumn(state, label, column);
                        else
                            UpdateColumn(state, label, column);
                    }
                }
            }
        }

        protected void UpdateColumn(LiveSplitState state, SimpleLabel label, ColumnData data)
        {
            var comparison = data.Comparison == "Current Comparison" ? state.CurrentComparison : data.Comparison;
            if (!state.Run.Comparisons.Contains(comparison))
                comparison = state.CurrentComparison;

            var timingMethod = state.CurrentTimingMethod;
            if (data.TimingMethod == "Real Time")
                timingMethod = TimingMethod.RealTime;
            else if (data.TimingMethod == "Game Time")
                timingMethod = TimingMethod.GameTime;

            var type = data.Type;

            var splitIndex = state.Run.IndexOf(Split);
            if (splitIndex < state.CurrentSplitIndex)
            {
                if (type == ColumnType.SplitTime || type == ColumnType.SegmentTime)
                {
                    label.ForeColor = Settings.OverrideTimesColor ? Settings.BeforeTimesColor : state.LayoutSettings.TextColor;

                    if (type == ColumnType.SplitTime)
                    {
                        label.Text = TimeFormatter.Format(Split.SplitTime[timingMethod]);
                    }
                    else //SegmentTime
                    {
                        var segmentTime = LiveSplitStateHelper.GetPreviousSegmentTime(state, splitIndex, timingMethod);
                        label.Text = TimeFormatter.Format(segmentTime);
                    }
                }

                if (type == ColumnType.DeltaorSplitTime || type == ColumnType.Delta)
                {
                    var deltaTime = Split.SplitTime[timingMethod] - Split.Comparisons[comparison][timingMethod];
                    var color = LiveSplitStateHelper.GetSplitColor(state, deltaTime, splitIndex, true, true, comparison, timingMethod);
                    if (color == null)
                        color = Settings.OverrideTimesColor ? Settings.BeforeTimesColor : state.LayoutSettings.TextColor;
                    label.ForeColor = color.Value;

                    if (type == ColumnType.DeltaorSplitTime)
                    {
                        if (deltaTime != null)
                            label.Text = DeltaTimeFormatter.Format(deltaTime);
                        else
                            label.Text = TimeFormatter.Format(Split.SplitTime[timingMethod]);
                    }

                    else if (type == ColumnType.Delta)
                        label.Text = DeltaTimeFormatter.Format(deltaTime);
                }

                else if (type == ColumnType.SegmentDeltaorSegmentTime || type == ColumnType.SegmentDelta)
                {
                    var segmentDelta = LiveSplitStateHelper.GetPreviousSegmentDelta(state, splitIndex, comparison, timingMethod);
                    var color = LiveSplitStateHelper.GetSplitColor(state, segmentDelta, splitIndex, false, true, comparison, timingMethod);
                    if (color == null)
                        color = Settings.OverrideTimesColor ? Settings.BeforeTimesColor : state.LayoutSettings.TextColor;
                    label.ForeColor = color.Value;

                    if (type == ColumnType.SegmentDeltaorSegmentTime)
                    {
                        if (segmentDelta != null)
                            label.Text = DeltaTimeFormatter.Format(segmentDelta);
                        else
                            label.Text = TimeFormatter.Format(LiveSplitStateHelper.GetPreviousSegmentTime(state, splitIndex, timingMethod));
                    }
                    else if (type == ColumnType.SegmentDelta)
                    {
                        label.Text = DeltaTimeFormatter.Format(segmentDelta);
                    }
                }
            }
            else
            {
                if (type == ColumnType.SplitTime || type == ColumnType.SegmentTime || type == ColumnType.DeltaorSplitTime || type == ColumnType.SegmentDeltaorSegmentTime)
                {
                    if (IsActive)
                        label.ForeColor = Settings.OverrideTimesColor ? Settings.CurrentTimesColor : state.LayoutSettings.TextColor;
                    else
                        label.ForeColor = Settings.OverrideTimesColor ? Settings.AfterTimesColor : state.LayoutSettings.TextColor;

                    if (type == ColumnType.SplitTime || type == ColumnType.DeltaorSplitTime)
                    {
                        label.Text = TimeFormatter.Format(Split.Comparisons[comparison][timingMethod]);
                    }
                    else //SegmentTime or SegmentTimeorSegmentDeltaTime
                    {
                        var previousTime = TimeSpan.Zero;
                        for (var index = splitIndex - 1; index >= 0; index--)
                        {
                            var comparisonTime = state.Run[index].Comparisons[comparison][timingMethod];
                            if (comparisonTime != null)
                            {
                                previousTime = comparisonTime.Value;
                                break;
                            }
                        }
                        label.Text = TimeFormatter.Format(Split.Comparisons[comparison][timingMethod] - previousTime);
                    }
                }

                //Live Delta
                var splitDelta = type == ColumnType.DeltaorSplitTime || type == ColumnType.Delta;
                var bestDelta = LiveSplitStateHelper.CheckLiveDelta(state, splitDelta, comparison, timingMethod);
                if (bestDelta != null && IsActive &&
                    (type == ColumnType.DeltaorSplitTime || type == ColumnType.Delta || type == ColumnType.SegmentDeltaorSegmentTime || type == ColumnType.SegmentDelta))
                {
                    label.Text = DeltaTimeFormatter.Format(bestDelta);
                    label.ForeColor = Settings.OverrideDeltasColor ? Settings.DeltasColor : state.LayoutSettings.TextColor;
                }
                else if (type == ColumnType.Delta || type == ColumnType.SegmentDelta)
                {
                    label.Text = "";
                }
            }
        }

        private TimeSpan? CheckLiveDeltaCollapsed(LiveSplitState state, int splitIndex, bool splitDelta, string comparison, TimingMethod method)
        {
            if (state.CurrentPhase == TimerPhase.Running || state.CurrentPhase == TimerPhase.Paused)
            {
                var curSplit = state.Run[splitIndex].Comparisons[comparison][method];
                var currentTime = state.CurrentTime[method];
                var segmentDelta = getSectionDelta(state, splitIndex, TopSplit, comparison, method);

                if (splitDelta && currentTime > curSplit || segmentDelta > TimeSpan.Zero)
                {
                    if (splitDelta)
                        return currentTime - curSplit;
                    return segmentDelta;
                }
            }
            return null;
        }

        protected void UpdateCollapsedColumn(LiveSplitState state, SimpleLabel label, ColumnData data)
        {
            var comparison = data.Comparison == "Current Comparison" ? state.CurrentComparison : data.Comparison;
            if (!state.Run.Comparisons.Contains(comparison))
                comparison = state.CurrentComparison;

            var timingMethod = state.CurrentTimingMethod;
            if (data.TimingMethod == "Real Time")
                timingMethod = TimingMethod.RealTime;
            else if (data.TimingMethod == "Game Time")
                timingMethod = TimingMethod.GameTime;

            var type = data.Type;

            var splitIndex = state.Run.IndexOf(Split);
            if (splitIndex < state.CurrentSplitIndex)
            {
                if (type == ColumnType.SplitTime || type == ColumnType.SegmentTime)
                {
                    label.ForeColor = Settings.OverrideTimesColor ? Settings.BeforeTimesColor : state.LayoutSettings.TextColor;

                    if (type == ColumnType.SplitTime)
                    {
                        label.Text = TimeFormatter.Format(Split.SplitTime[timingMethod]);
                    }
                    else //SegmentTime
                    {
                        var segmentTime = getSectionTime(state, splitIndex, TopSplit, comparison, timingMethod);
                        label.Text = TimeFormatter.Format(segmentTime);
                    }
                }

                if (type == ColumnType.DeltaorSplitTime || type == ColumnType.Delta)
                {
                    var deltaTime = Split.SplitTime[timingMethod] - Split.Comparisons[comparison][timingMethod];
                    var segmentDelta = getSectionDelta(state, splitIndex, TopSplit, comparison, timingMethod);
                    var color = GetSectionColor(state, deltaTime, segmentDelta);
                    if (color == null)
                        color = Settings.OverrideTimesColor ? Settings.BeforeTimesColor : state.LayoutSettings.TextColor;
                    label.ForeColor = color.Value;

                    if (type == ColumnType.DeltaorSplitTime)
                    {
                        if (deltaTime != null)
                            label.Text = DeltaTimeFormatter.Format(deltaTime);
                        else
                            label.Text = TimeFormatter.Format(Split.SplitTime[timingMethod]);
                    }

                    else if (type == ColumnType.Delta)
                        label.Text = DeltaTimeFormatter.Format(deltaTime);
                }

                else if (type == ColumnType.SegmentDeltaorSegmentTime || type == ColumnType.SegmentDelta)
                {
                    var segmentDelta = getSectionDelta(state, splitIndex, TopSplit, comparison, timingMethod);
                    var color = GetSectionColor(state, null, segmentDelta);
                    if (color == null)
                        color = Settings.OverrideTimesColor ? Settings.BeforeTimesColor : state.LayoutSettings.TextColor;
                    label.ForeColor = color.Value;

                    if (type == ColumnType.SegmentDeltaorSegmentTime)
                    {
                        if (segmentDelta != null)
                            label.Text = DeltaTimeFormatter.Format(segmentDelta);
                        else
                        {
                            var segmentTime = getSectionTime(state, splitIndex, TopSplit, comparison, timingMethod);
                            label.Text = TimeFormatter.Format(segmentTime);
                        }
                    }
                    else if (type == ColumnType.SegmentDelta)
                    {
                        label.Text = DeltaTimeFormatter.Format(segmentDelta);
                    }
                }
            }
            else
            {
                if (type == ColumnType.SplitTime || type == ColumnType.SegmentTime || type == ColumnType.DeltaorSplitTime || type == ColumnType.SegmentDeltaorSegmentTime)
                {
                    if (IsActive)
                        label.ForeColor = Settings.OverrideTimesColor ? Settings.CurrentTimesColor : state.LayoutSettings.TextColor;
                    else
                        label.ForeColor = Settings.OverrideTimesColor ? Settings.AfterTimesColor : state.LayoutSettings.TextColor;

                    if (type == ColumnType.SplitTime || type == ColumnType.DeltaorSplitTime)
                    {
                        label.Text = TimeFormatter.Format(Split.Comparisons[comparison][timingMethod]);
                    }
                    else //SegmentTime or SegmentTimeorSegmentDeltaTime
                    {
                        var previousTime = TopSplit > 0 ? state.Run[TopSplit - 1].Comparisons[comparison][timingMethod] : TimeSpan.Zero;
                        label.Text = TimeFormatter.Format(Split.Comparisons[comparison][timingMethod] - previousTime);
                    }
                }

                //Live Delta
                var splitDelta = type == ColumnType.DeltaorSplitTime || type == ColumnType.Delta;
                var bestDelta = CheckLiveDeltaCollapsed(state, splitIndex, splitDelta, comparison, timingMethod);
                if (bestDelta != null && IsActive &&
                    (type == ColumnType.DeltaorSplitTime || type == ColumnType.Delta || type == ColumnType.SegmentDeltaorSegmentTime || type == ColumnType.SegmentDelta))
                {
                    label.Text = DeltaTimeFormatter.Format(bestDelta);
                    label.ForeColor = Settings.OverrideDeltasColor ? Settings.DeltasColor : state.LayoutSettings.TextColor;
                }
                else if (type == ColumnType.Delta || type == ColumnType.SegmentDelta)
                {
                    label.Text = "";
                }
            }
        }

        protected float CalculateHeaderWidth()
        {
            var width = 0f;
            if (Header)
            {
                if (Settings.SectionTimer || Settings.HeaderTimes)
                    width += MeasureTimeLabel.ActualWidth + 5;
                if (Settings.SectionTimer)
                    width += MeasureDeltaLabel.ActualWidth + 5;
            }
            return width;
        }

        protected float CalculateLabelsWidth()
        {
            if (!Header && ColumnsList != null)
            {
                var mixedCount = ColumnsList.Count(x => x.Type == ColumnType.DeltaorSplitTime || x.Type == ColumnType.SegmentDeltaorSegmentTime);
                var deltaCount = ColumnsList.Count(x => x.Type == ColumnType.Delta || x.Type == ColumnType.SegmentDelta);
                var timeCount = ColumnsList.Count(x => x.Type == ColumnType.SplitTime || x.Type == ColumnType.SegmentTime);
                return mixedCount * (Math.Max(MeasureDeltaLabel.ActualWidth, MeasureTimeLabel.ActualWidth) + 5)
                    + deltaCount * (MeasureDeltaLabel.ActualWidth + 5)
                    + timeCount * (MeasureTimeLabel.ActualWidth + 5);
            }
            return 0f;
        }

        protected void RecreateLabels()
        {
            if (ColumnsList != null && LabelsList.Count != ColumnsList.Count())
            {
                LabelsList.Clear();
                foreach (var column in ColumnsList)
                {
                    LabelsList.Add(new SimpleLabel
                    {
                        HorizontalAlignment = StringAlignment.Far
                    });
                }
            }
        }

        public void Update(IInvalidator invalidator, LiveSplitState state, float width, float height, LayoutMode mode)
        {
            if (Split != null)
            {
                UpdateAll(state);
                NeedUpdateAll = false;

                Cache.Restart();
                Cache["Icon"] = Split.Icon;
                if (Cache.HasChanged)
                {
                    if (Split.Icon == null)
                        FrameCount = 0;
                    else
                        FrameCount = Split.Icon.GetFrameCount(new FrameDimension(Split.Icon.FrameDimensionsList[0]));
                }
                Cache["SplitName"] = NameLabel.Text;
                Cache["DeltaLabel"] = DeltaLabel.Text;
                Cache["TimeLabel"] = TimeLabel.Text;
                Cache["IsActive"] = IsActive;
                Cache["IsHighlight"] = IsHighlight;
                Cache["NameColor"] = NameLabel.ForeColor.ToArgb();
                Cache["TimeColor"] = TimeLabel.ForeColor.ToArgb();
                Cache["DeltaColor"] = DeltaLabel.ForeColor.ToArgb();
                Cache["Indent"] = Indent;
                Cache["DisplayIcon"] = DisplayIcon;
                Cache["ColumnsCount"] = ColumnsList.Count();
                for (var index = 0; index < LabelsList.Count; index++)
                {
                    var label = LabelsList[index];
                    Cache["Columns" + index + "Text"] = label.Text;
                    Cache["Columns" + index + "Color"] = label.ForeColor.ToArgb();
                }
                Cache["MeasureTimeActualWidth"] = MeasureTimeLabel.ActualWidth;
                Cache["MeasureDeltaActualWidth"] = MeasureDeltaLabel.ActualWidth;

                if (invalidator != null && (Cache.HasChanged || FrameCount > 1 || blankOut))
                {
                    invalidator.Invalidate(0, 0, width, height);
                }

                blankOut = false;
            }
            else if (!blankOut)
            {
                blankOut = true;
                IsSubsplit = false;
                invalidator.Invalidate(0, 0, width, height);
            }
        }

        public void Dispose()
        {
        }
    }
}
