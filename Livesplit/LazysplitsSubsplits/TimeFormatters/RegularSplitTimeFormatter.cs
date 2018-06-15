﻿using System;

namespace LiveSplit.TimeFormatters
{
    class RegularSplitTimeFormatter : ITimeFormatter
    {
        public TimeAccuracy Accuracy { get; set; }

        public RegularSplitTimeFormatter(TimeAccuracy accuracy)
        {
            Accuracy = accuracy;
        }
        public string Format(TimeSpan? time)
        {
            var formatter = new RegularTimeFormatter(Accuracy);
            if (time == null)
                return TimeFormatConstants.DASH;

            return formatter.Format(time);
        }
    }
}
