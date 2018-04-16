using System;
using LiveSplit.Model;
using LiveSplit.UI.Components;
using LiveSplit.Lazysplits;


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
        public Version Version{ get { return Version.Parse("1.0"); } }

        public IComponent Create(LiveSplitState state)
        {
            var ComponentIt = state.Layout.Components.GetEnumerator();
            while( ComponentIt.MoveNext() )
            {
                if( ComponentIt.Current.ComponentName == "Lazysplits" )
                {
                    throw new Exception("Lazysplits component does not allow duplicate instaces");
                }
            }

            return new LazysplitsComponent(state);
        }
    }
} //namespace LiveSplit.Lazysplits
