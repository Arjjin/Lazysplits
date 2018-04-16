using System;
using System.Threading;

namespace LiveSplit.Lazysplits.Threading
{

public abstract class LzsThread : IDisposable
{
    protected volatile bool bIsLive;
    protected volatile bool bShouldSleep;
    protected volatile bool bIsSleeping;
    private AutoResetEvent WakeEvent;
    
    protected volatile bool bShouldTerminate;
    
    protected string ThreadName;
    protected Thread ThreadInstance;

    public LzsThread( string threadName )
    {
        bShouldSleep = false;
        bIsSleeping = false;
        WakeEvent = new AutoResetEvent(false);

        bShouldTerminate = false;
        
        ThreadName = threadName;
    }
    public bool ThreadIsLive()
    {
        if( ThreadInstance == null ){ return false; }
        return ThreadInstance.IsAlive;
    }
    public void ThreadCreate()
    {  
        if( !ThreadIsLive() )
        {
            ThreadInstance = new Thread( new ThreadStart(ThreadFunc) );
            ThreadInstance.Start();
        }
    }
    public void ThreadJoin()
    {
        if( ThreadIsLive() ){ ThreadInstance.Join(); }
    }
    public void ThreadSleep(){ bShouldSleep = true; }
    private void SleepInternal()
    {
        bIsSleeping = true;
        bShouldSleep = false;
        WakeEvent.WaitOne();
        bIsSleeping = false;
    }
    public void ThreadWake(){ if(bIsSleeping){ WakeEvent.Set(); } }
    public virtual void ThreadTerminate(){
        bShouldTerminate = true;
        if(bIsSleeping){ ThreadWake(); }
    }
    
    protected abstract void ThreadFunc();
    protected virtual void ThreadFuncInit()
    {
        bShouldSleep = false;
        bIsSleeping = false;
        bShouldTerminate = false;
    }
    protected virtual bool ThreadFuncShouldLoop()
    {
        //check for sleep first, so after thread wakes it immediately checks if termination was scheduled
        //due to this, add check for termination in conditional for sleep as well
        if( bShouldSleep && !bIsSleeping && !bShouldTerminate ){ SleepInternal(); }
        if( bShouldTerminate ){ return false; }

        return true;
    }
    protected virtual void ThreadFuncCleanup()
    {
        bShouldSleep = false;
        bIsSleeping = false;
        bShouldTerminate = false;
    }

    //IDisposable
    private bool disposed = false;
    public void Dispose()
    {
        Dispose(true);
        GC.SuppressFinalize(this);
    }
    protected virtual void Dispose(bool disposing)
    {
        if(!disposed)
        {
            if(disposing)
            {
                WakeEvent.Dispose();
            }
            //no unamanaged resources to dispose here
        }
        disposed = true;
    }
}

} //namespace LiveSplit.Lazysplits.Threading
