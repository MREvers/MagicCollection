using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Threading;
using System.Windows.Threading;
using System.Windows;

namespace StoreFrontPro.Server
{
   partial class ServerInterface
   {
      #region Static Interface
      public const string LoadIndication = "Loading";
      public const string DoneIndication = "Complete";
      public const string ServerReady = "Ready";

      // The static singleton is exposed via the 3 public properties Server, Card, and Collection
      // The Singleton's constructor ensures that the private static member SCI is initiated prior
      // to use. 3 Different public exposures for groups of functions gives the flat interface 
      // structure that is more readable.
      private static ServerInterface singleton;

      // SCI is the Managed C++ interface. It is a flat interface, meaning ALL available server
      // functions are here. It acts as a BLOB class for ease of use due to the nature of 
      // managed <-> unmanaged communications, despite being an 'anti-pattern'.
      private static ServerClientInterface SCI;

      // PUBLIC interface
      public static ServerIFace Server
      {
         get
         {
            if (singleton == null) { InitServer(); }
            return singleton._Server;
         }
      }
      public static CardIFace Card
      {
         get
         {
            if (singleton == null) { InitServer(); }
            return singleton._Card;
         }
      }
      public static CollectionIFace Collection
      {
         get
         {
            if (singleton == null) { InitServer(); }
            return singleton._Collection;
         }
      }

      public static void ListenForDebug(IServerObserver Observer)
      {
         singleton.m_lstDebugObservers.Add(Observer);
      }

      public static bool IsReady()
      {
         return SCI != null;
      }

      private static void InitServer()
      {
         singleton = new ServerInterface();
      }
      #endregion

      #region Private Fields
      // The action arguments are curried at call-time.
      private List<Action> m_lstServerQueue;
      private Mutex m_QueueLock;

      // Debug is output through here.
      private List<IServerObserver> m_lstDebugObservers;

      private ServerIFace _Server;
      private CardIFace _Card;
      private CollectionIFace _Collection;
      #endregion

      #region Private Methods
      /// <summary>
      /// The Constructor initilizes all the objects in the correct order.
      /// The SCI can be added as a service here becuase the thread ensures
      /// that the SCI is present before any other services can run.
      /// </summary>
      private ServerInterface()
      {
         _Server = new ServerIFace();
         _Card = new CardIFace();
         _Collection = new CollectionIFace();

         m_QueueLock = new Mutex();
         m_lstServerQueue = new List<Action>();

         m_lstDebugObservers = new List<IServerObserver>();

         Thread pServiceThread = new Thread(pServices);
         pServiceThread.IsBackground = true;
         pServiceThread.Start();
      }

      /// <summary>
      /// 'p' indicates "process" or thread.
      /// </summary>
      private void pServices()
      {

         if( SCI == null )
         {
            SCI = new ServerClientInterface();

            if( !SCI.IsConfigLoaded() )
            {
               MessageBox.Show("Could Not Find Config File. Please Fix.");
            }

            fireDebug(ServerReady);
         }

         while (true)
         {
            Thread.Sleep(40);

            if (m_lstServerQueue.Count == 0) { continue; }
            m_QueueLock.WaitOne();
            Action currentTask = m_lstServerQueue.First();
            m_lstServerQueue.RemoveAt(0);
            m_QueueLock.ReleaseMutex();

            // On concern I have is if the object on which this method is called is 
            // garbage collected between queuing and execution. I tested it breifly
            // and it seems like it may not be a problem. I'd like to profile this
            // in the future but the ANTS profiler trial ran out.
            currentTask();
         }
      }

      /// <summary>
      /// Prepares the task to be processed by the services thread.
      /// </summary>
      /// <param name="aTask"></param>
      /// <param name="UICallback"></param>
      /// <param name="WaitForLoad"></param>
      private void enqueueService(Action aTask, bool UICallback = false, bool WaitForLoad = false)
      {
         Action wrappedTask;
         if( WaitForLoad )
         {
            wrappedTask = () =>
            {
               // Show the load indication on the wpf thread.
               toDispatch(() => { fireDebug(LoadIndication); }).Invoke();

               aTask();

               //close the load indication on the wpf thread.
               toDispatch(() => { fireDebug(DoneIndication); }).Invoke();
            };
         }
         else
         {
            wrappedTask = aTask;
         }

         Action queueAction;
         if( UICallback )
         {
            queueAction = toDispatch(wrappedTask);
         }
         else
         {
            queueAction = wrappedTask;
         }

         m_QueueLock.WaitOne();
         m_lstServerQueue.Add(queueAction);
         m_QueueLock.ReleaseMutex();
      }

      /// <summary>
      /// Sends debug string to all the server listeners.
      /// </summary>
      /// <param name="Message"></param>
      private void fireDebug(string Message)
      {
         foreach(var listener in m_lstDebugObservers)
         {
            listener.ServerMessage(Message);
         }
      }

      private Action toDispatch(Action aTask)
      {
         return () =>
         {
            if (!Application.Current.Dispatcher.CheckAccess())
            {
               Application.Current.Dispatcher.BeginInvoke(aTask);
            }
            else
            {
               aTask.Invoke();
            }
         };
      }
      #endregion
   }
}
