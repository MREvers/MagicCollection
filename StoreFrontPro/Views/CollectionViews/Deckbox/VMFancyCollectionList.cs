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
   class VMFancyCollectionList : ViewModel<BasicModel<List<CardModel>>>, IViewComponent, IVCISupporter
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

         clearCards();
         List<CardModel> lstCMs = Model.Item.ToList();
         Dictionary<string, List<CardModel>> mapCardTypeGroup = new Dictionary<string, List<CardModel>>();

         SortingList grouping = new SortingList(Model);
         grouping.SortOn("types", "Creature", "Land");
         grouping.SortOn("cmc");
         grouping.SortOnFeature = true;

         foreach( var group in grouping.GetGroups() )
         {
            if( group.GroupList.Count() == 0 ) { continue; }

            string szGroupName = group.Grouping + " (" + countCardsInGroup(group.GroupList) + ")";
            CardModel modelGroupHeader = new CardModel(szGroupName, "");
            ViewClass listItem = ViewFactory.CreateFancyCollectionItem(modelGroupHeader, 1, "");
            Cards.Add((VFancyCollectionItem)listItem.View);

            foreach( var model in group.GroupList )
            {
               addCard(model);
            }
         }

         eListChanged();
      }

      private void addCard(CardModel model)
      {
         ViewClass listItem = ViewFactory.CreateFancyCollectionItem(model, 3, "");
         ((VMFancyCollectionItem)listItem.ViewModel).DisplayEvent += DisplayEventHandler;
         Cards.Add((VFancyCollectionItem)listItem.View);
      }

      private void clearCards()
      {
         foreach( var display in Cards )
         {
             ((VMFancyCollectionItem)display.DataContext).DisplayEvent -= DisplayEventHandler;
         }

         Cards.Clear();
      }

      private int countCardsInGroup(IEnumerable<CardModel> alstModels) 
      {
         int iCount = 0;
         foreach( CardModel model in alstModels ) 
         {
            iCount += model.Count;
         }

         return iCount;
      }

      private void eChildClicked(CardModel model)
      {
         DisplayEventArgs eventArgs = new DisplayEventArgs(VCIFancyCollectionList.ChildClicked, model);
         DisplayEvent?.Invoke(this, eventArgs);
      }

      private void eListChanged()
      {
         DisplayEventArgs eventArgs = new DisplayEventArgs(VCIFancyCollectionList.ListChanged);
         DisplayEvent?.Invoke(this, eventArgs);
      }

      #region IViewModel
      public override void ModelUpdated()
      {
         SyncWithModel();
      }
      #endregion

      #region IVCISupporter
      public void DisplayEventHandler(object source, DisplayEventArgs e)
      {
         GetRouter().Call(source.GetType(), this, e.Key, e.Args);
      }

      public InterfaceRouter GetRouter()
      {
         return IRouter;
      }

      static InterfaceRouter _IRouter = null;
      static InterfaceRouter IRouter
      {
         get
         {
            if (_IRouter == null) { BuildInterface(); }
            return _IRouter;
         }
      }

      static void BuildInterface()
      {
         _IRouter = new InterfaceRouter();

         VCIFancyCollectionItem RTIS = new VCIFancyCollectionItem(
             Clicked: (x) => { return (x as VMFancyCollectionList).eChildClicked; });
         _IRouter.AddInterface(RTIS);
      }
      #endregion

      #region IViewComponent
      public event DisplayEventHandler DisplayEvent;

      
      public List<StoreFrontMenuItem> GetMenuItems()
      {
         throw new NotImplementedException();
      }
      #endregion
   }
}
