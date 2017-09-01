using StoreFrontPro.Server;
using StoreFrontPro.Views;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Net;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Xml;

namespace StoreFrontPro
{
   class DiagnosticTools
   {
      class Timed
      {
         public string Name = "";
         public ulong Time = 0;
      }

      static List<Timed> times = new List<Timed>();
      static Stopwatch timer = new Stopwatch();
      static public void Start()
      {
         timer.Reset();
         timer.Start();
      }

      static public void Stop(string aszName)
      {
         timer.Stop();
         times.Add(new Timed { Name = aszName, Time = (ulong)timer.ElapsedMilliseconds });
      }
   }

   /// <summary>
   /// This class controls the main window essentially.
   /// Switches sub-views and displays other windows.
   /// </summary>
   class StoreFront: IModel
   {
      public const string cszStoreFrontVMName = "SFVM";

      public BasicModel<List<CollectionModel>> Collections { get; private set; }
      public VMStoreFront StoreFrontVM { get; private set; }

      // Maintain a reference to the ViewModel.
      private Window m_ucMainWindow;

      #region Public Functions
      public StoreFront(Window MainWindow)
      {
         ServerInterface.Server.Register(() => { Collections.NotifyViewModel(); });

         Collections = new BasicModel<List<CollectionModel>>(ServerInterface.Server.Collections, Sync);
         StoreFrontVM = new VMStoreFront(Model: this, RoutingName: cszStoreFrontVMName);
         m_ucMainWindow = MainWindow;
      }

      public void CloseApplication()
      {
         m_ucMainWindow.Close();
      }

      public void LoadLatestJSON()
      {
         ServerInterface.Server.ImportJSONCollectionAS();
      }

      public void CreateCollection(string aszCollectionName)
      {
         ServerInterface.Server.CreateCollection(aszCollectionName);
      }

      public void LoadCollection(string aszFileName)
      {
         ServerInterface.Server.LoadCollectionAS(aszFileName);
      }
      #endregion

      #region IModel
      private List<IViewModel> m_lstViewers = new List<IViewModel>();
      public void Register(IViewModel item)
      {
         m_lstViewers.Add(item);
      }

      public void UnRegister(IViewModel item)
      {
         m_lstViewers.Remove(item);
      }

      public void NotifyViewModel()
      {
         m_lstViewers.ForEach(x => x.ModelUpdated());
      }

      public void Sync(bool ASync = true)
      {
         Collections.NotifyViewModel();
      }

      public void EnableNotification(bool abNotify)
      {
         throw new NotImplementedException();
      }

      public void DisableNotification()
      {
         throw new NotImplementedException();
      }
      #endregion
   }
}
