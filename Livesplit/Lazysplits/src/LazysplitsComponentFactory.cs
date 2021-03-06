﻿using System;
using LiveSplit.Model;
using LiveSplit.UI.Components;
using LiveSplit.Lazysplits;

using NLog;
using NLog.Targets;
using NLog.Config;

[assembly: ComponentFactory(typeof(LazysplitsComponentFactory))]

namespace LiveSplit.Lazysplits
{
    public class LazysplitsComponentFactory : IComponentFactory
    {
        public string ComponentName{ get { return "Lazysplits"; } }
        public string Description{ get { return "Lazysplits component"; } }
        public ComponentCategory Category{ get { return ComponentCategory.Control; } }
        
        public string UpdateName{ get {return ComponentName; } }
        public string XMLURL{ get; }
        public string UpdateURL{ get; }
        public Version Version{ get { return Version.Parse("1.0.0"); } }

        public IComponent Create(LiveSplitState state)
        {
            return new LazysplitsComponent(state);
        }
    }
} //namespace LiveSplit.Lazysplits
