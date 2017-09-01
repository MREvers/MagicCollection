using StoreFrontPro.Server;
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
                                   bool CollapsedView = false) : base(Model, RoutingName)
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

         VFancyCollectionItem fancyCollectionItemV;
         VMFancyCollectionItem fancyCollectionItemVM;

         foreach (CardModel cm in Model.Item)
         {
            string szTypeGroup = cm.GetAttr("types");
            szTypeGroup = szTypeGroup.Contains("Creature") ? "Creature" : szTypeGroup;
            if (!mapCardTypeGroup.ContainsKey(szTypeGroup))
            {
               mapCardTypeGroup.Add(szTypeGroup, new List<CardModel>() { cm });
            }
            else
            {
               mapCardTypeGroup[szTypeGroup].Add(cm);
            }
         }

         // Land, Creature, Non Creature.
         if (mapCardTypeGroup.ContainsKey("Land"))
         {
            fancyCollectionItemV = new VFancyCollectionItem();
            fancyCollectionItemVM = new VMFancyCollectionItem(new CardModel("Land", ""), "", 1);
            fancyCollectionItemV.DataContext = fancyCollectionItemVM;
            Cards.Add(fancyCollectionItemV);

            foreach (CardModel cm in mapCardTypeGroup["Land"])
            {
               fancyCollectionItemV = new VFancyCollectionItem();
               fancyCollectionItemVM = new VMFancyCollectionItem(cm, "");
               fancyCollectionItemV.DataContext = fancyCollectionItemVM;
               Cards.Add(fancyCollectionItemV);
            }

            mapCardTypeGroup.Remove("Land");
         }

         if (mapCardTypeGroup.ContainsKey("Creature"))
         {
            fancyCollectionItemV = new VFancyCollectionItem();
            fancyCollectionItemVM = new VMFancyCollectionItem(new CardModel("Creature", ""), "", 1);
            fancyCollectionItemV.DataContext = fancyCollectionItemVM;
            Cards.Add(fancyCollectionItemV);

            foreach (CardModel cm in mapCardTypeGroup["Creature"])
            {
               fancyCollectionItemV = new VFancyCollectionItem();
               fancyCollectionItemVM = new VMFancyCollectionItem(cm, "");
               fancyCollectionItemV.DataContext = fancyCollectionItemVM;
               Cards.Add(fancyCollectionItemV);
            }

            mapCardTypeGroup.Remove("Creature");
         }

         if (mapCardTypeGroup.Keys.Count > 0)
         {
            fancyCollectionItemV = new VFancyCollectionItem();
            fancyCollectionItemVM = new VMFancyCollectionItem(new CardModel("Other", ""), "", 1);
            fancyCollectionItemV.DataContext = fancyCollectionItemVM;
            Cards.Add(fancyCollectionItemV);

            foreach (string szKey in mapCardTypeGroup.Keys)
            {
               foreach (CardModel cm in mapCardTypeGroup[szKey])
               {
                  fancyCollectionItemV = new VFancyCollectionItem();
                  fancyCollectionItemVM = new VMFancyCollectionItem(cm, "");
                  fancyCollectionItemV.DataContext = fancyCollectionItemVM;
                  Cards.Add(fancyCollectionItemV);
               }
            }
         }

         DiagnosticTools.Stop("View Deckbox");
      }

      #region IViewModel
      public override void ModelUpdated()
      {
         SyncWithModel();
      }
      #endregion
   }
}
