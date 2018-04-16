using System;
using System.Collections.Generic;
using System.Linq;

namespace LiveSplit.Lazysplits
{
    abstract class LzsObservable
    {
        public LzsObservable()
        {
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
        public void NotifyAll()
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
                    Observer.OnSubjectNotify();
                }
            }
        }

        private List<WeakReference> ObserverList;
    }

    interface ILzsObserver
    {
        void OnSubjectNotify();
    }
} //namespace LiveSplit.Lazysplits