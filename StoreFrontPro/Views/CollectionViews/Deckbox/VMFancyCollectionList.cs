using StoreFrontPro.Server;
using StoreFrontPro.Support.SortingList;
using StoreFrontPro.Tools;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Collections.Specialized;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;

namespace StoreFrontPro.Views.CollectionViews.Deckbox
{
   class VMFancyCollectionList : ViewModel<BasicModel<List<CardModel>>>
   {
      public ObservableCollection<VFancyCollectionItem> Cards { get; set; } = 
         new ObservableCollection<VFancyCollectionItem>();

      private bool _CollapsedView = false;
      private bool CollapsedView
      {
         get
         {
            return _CollapsedView;
         }
      }

      public VMFancyCollectionList(BasicModel<List<CardModel>> Model, string RoutingName,
                                   bool CollapsedView = false)
         : base(Model, RoutingName)
      {
         _CollapsedView = CollapsedView;
         SyncWithModel();
         Model.Register(this);
      }

      private void eSyncWithModel(object source, NotifyCollectionChangedEventArgs e)
      {
         SyncWithModel();
      }

      public void SyncWithModel()
      {
         if (!Application.Current.Dispatcher.CheckAccess())
         {
            Action aAction = new Action(() => { SyncWithModel(); });
            Application.Current.Dispatcher.BeginInvoke(aAction);
            return;
         }

         Cards.Clear();
         List<CardModel> lstCMs = Model.Item.ToList();
         Dictionary<string, List<CardModel>> mapCardTypeGroup = new Dictionary<string, List<CardModel>>();

         SortingList grouping = new SortingList(Model);
         grouping.SortOn("types", "Creature", "Land");
         grouping.SortOn("cmc");
         grouping.SortOnFeature = true;

         foreach( var group in grouping.GetGroups() )
         {
            if( group.GroupList.Count() == 0 ) { continue; }

            CardModel modelGroupHeader = new CardModel(group.Grouping, "");
            ViewClass listItem = ViewFactory.CreateFancyCollectionItem(modelGroupHeader, 1, "");
            Cards.Add((VFancyCollectionItem)listItem.View);

            foreach( var model in group.GroupList )
            {
               listItem = ViewFactory.CreateFancyCollectionItem(model, 3, "");
               Cards.Add((VFancyCollectionItem)listItem.View);
            }
         }
      }

      #region IViewModel
      public override void ModelUpdated()
      {
         SyncWithModel();
      }
      #endregion
   }
}
