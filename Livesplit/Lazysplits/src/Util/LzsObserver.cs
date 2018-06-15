using System;
using System.Collections.Generic;
using System.Linq;

namespace LiveSplit.Lazysplits
{
    public abstract class LzsObservable
    {
        public LzsObservable( string subjectName )
        {
            SubjectName = subjectName;
            ObserverList = new List<WeakReference>();
        }
        public void AttachObserver( ILzsObserver observer )
        {
            ObserverList.Add( new WeakReference(observer) );
        }
        public void DetachObserver( ILzsObserver observerTarget )
        {
            for( int i = 0; i < ObserverList.Count(); i++ )
            {
               ILzsObserver Observer = ObserverList[i].Target as ILzsObserver;
                if( Observer == null || Observer == observerTarget )
                {
                    ObserverList.RemoveAt(i);
                }
            }
        }
        public void NotifyAll( string message = "" )
        {
            foreach( WeakReference observer in ObserverList )
            {
                ILzsObserver Observer = observer.Target as ILzsObserver;
                if( Observer == null )
                {
                    ObserverList.Remove(observer);
                }
                else
                {
                    Observer.OnSubjectNotify( SubjectName, message );
                }
            }
        }

        private string SubjectName;
        private List<WeakReference> ObserverList;
    }

    public interface ILzsObserver
    {
        void OnSubjectNotify( string subjectName, string message );
    }
} //namespace LiveSplit.Lazysplits