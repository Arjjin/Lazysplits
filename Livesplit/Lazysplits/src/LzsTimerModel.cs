using System;

using LiveSplit.Model;
using LiveSplit.Model.Input;

namespace LiveSplit.Lazysplits
{
    class LzsTimerModel : TimerModel, ITimerModel
    {
        public new event EventHandler OnSplit;
        public new event EventHandler OnUndoSplit;
        public new event EventHandler OnSkipSplit;
        public new event EventHandler OnStart;
        public new event EventHandlerT<TimerPhase> OnReset;
        public new event EventHandler OnPause;
        public new event EventHandler OnUndoAllPauses;
        public new event EventHandler OnResume;
        public new event EventHandler OnScrollUp;
        public new event EventHandler OnScrollDown;
        public new event EventHandler OnSwitchComparisonPrevious;
        public new event EventHandler OnSwitchComparisonNext;

        public LzsTimerModel()
        {
            base.OnSplit += OnBaseSplit;
            base.OnUndoSplit += OnBaseUndoSplit;
            base.OnSkipSplit += OnBaseSkipSplit;
            base.OnStart += OnBaseStart;
            base.OnReset += OnBaseReset;
            base.OnPause += OnBasePause;
            base.OnUndoAllPauses += OnBaseUndoAllPauses;
            base.OnResume += OnBaseResume;
            base.OnScrollUp += OnBaseScrollUp;
            base.OnScrollDown += OnBaseScrollDown;
            base.OnSwitchComparisonPrevious += OnBaseSwitchComparisonPrevious;
            base.OnSwitchComparisonNext += OnBaseSwitchComparisonNext;
        }

        public void AdjustedStart( int offsetMs )
        {
            if (CurrentState.CurrentPhase == TimerPhase.NotRunning)
            {
                TimeSpan AdjustedTimeSpan = new TimeSpan( 0, 0, 0, 0, offsetMs );

                CurrentState.CurrentPhase = TimerPhase.Running;
                CurrentState.CurrentSplitIndex = 0;
                CurrentState.AttemptStarted = TimeStamp.CurrentDateTime;
                CurrentState.AdjustedStartTime = CurrentState.StartTimeWithOffset = TimeStamp.Now - CurrentState.Run.Offset - AdjustedTimeSpan;
                CurrentState.StartTime = TimeStamp.Now;
                CurrentState.TimePausedAt = CurrentState.Run.Offset;
                CurrentState.IsGameTimeInitialized = false;
                CurrentState.Run.AttemptCount++;
                CurrentState.Run.HasChanged = true;
                
                OnStart?.Invoke(this,null);
            }
        }

        public void AdjustedSplit( int offsetMs )
        {
            if (CurrentState.CurrentPhase == TimerPhase.Running && CurrentState.CurrentTime.RealTime > TimeSpan.Zero)
            {
                TimeSpan AdjustedTimeSpan = new TimeSpan( 0, 0, 0, 0, offsetMs );
                Time AdjTime = new Time();
                AdjTime.RealTime = CurrentState.CurrentTime.RealTime + AdjustedTimeSpan;

                CurrentState.CurrentSplit.SplitTime = AdjTime;
                CurrentState.CurrentSplitIndex++;
                if (CurrentState.Run.Count == CurrentState.CurrentSplitIndex)
                {
                    CurrentState.CurrentPhase = TimerPhase.Ended;
                    CurrentState.AttemptEnded = TimeStamp.CurrentDateTime;
                }
                CurrentState.Run.HasChanged = true;

                OnSplit?.Invoke(this, null);
            }
        }
        
        public void OnBaseSplit( object sender, EventArgs e ){ OnSplit?.Invoke( sender, e ); }
        public void OnBaseUndoSplit( object sender, EventArgs e ){ OnUndoSplit?.Invoke( sender, e ); }
        public void OnBaseSkipSplit( object sender, EventArgs e ){ OnSkipSplit?.Invoke( sender, e ); }
        public void OnBaseStart( object sender, EventArgs e ){ OnStart?.Invoke( sender, e ); }
        public void OnBaseReset( object sender, TimerPhase timerPhase ){ OnReset?.Invoke( sender, timerPhase ); }
        public void OnBasePause( object sender, EventArgs e ){ OnPause?.Invoke( sender, e ); }
        public void OnBaseUndoAllPauses( object sender, EventArgs e ){ OnUndoAllPauses?.Invoke( sender, e ); }
        public void OnBaseResume( object sender, EventArgs e ){ OnResume?.Invoke( sender, e ); }
        public void OnBaseScrollUp( object sender, EventArgs e ){ OnScrollUp?.Invoke( sender, e ); }
        public void OnBaseScrollDown( object sender, EventArgs e ){ OnScrollDown?.Invoke( sender, e ); }
        public void OnBaseSwitchComparisonPrevious( object sender, EventArgs e ){ OnSwitchComparisonPrevious?.Invoke( sender, e ); }
        public void OnBaseSwitchComparisonNext( object sender, EventArgs e ){ OnSwitchComparisonNext?.Invoke( sender, e ); }
    }
}
