using LiveSplit.Model;
using System;

using LiveSplit.UI.Components;

namespace LiveSplit.Lazysplits.Subsplits
{
    public class SplitsComponentFactory : IComponentFactory
    {
        public string ComponentName{ get { return "Lazysplits Subsplits"; } }
        public string Description{ get { return "Lazysplits Subsplits component"; } }
        public ComponentCategory Category{ get { return ComponentCategory.List; } }
        
        public string UpdateName{ get {return ComponentName; } }
        public string XMLURL{ get; }
        public string UpdateURL{ get; }
        public Version Version{ get { return Version.Parse("1.0.0"); } }
        
        public IComponent Create(LiveSplitState state)
        {
            return new SplitsComponent(state);
        }
    }
}
