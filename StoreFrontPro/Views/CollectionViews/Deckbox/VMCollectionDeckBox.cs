using StoreFrontPro.Server;
using StoreFrontPro.Support.MultiDisplay;
using StoreFrontPro.Views.Components.CardImageDock;
using StoreFrontPro.Views.Components.RequestTextOverlay;
using StoreFrontPro.Views.Components.VCardImageDock;
using StoreFrontPro.Views.Interfaces.CollectionChanger;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;

namespace StoreFrontPro.Views.CollectionViews.Deckbox
{
   class VMCollectionDeckBox : ViewModel<CollectionModel>, IViewComponent, IVCISupporter
   {
      #region Names
      public const string CollectionEditor = "CE";
      public const string ItemDock = "ITD";
      #endregion

      #region Bindings
      private MultiDisplay _OperationWindow = new MultiDisplay();
      public MultiDisplay OperationWindow
      {
         get { return _OperationWindow; }
         set { _OperationWindow = value; OnPropertyChanged(); }
      }

      private VMCardImageDock Dock;
      private UserControl _CollectionDock;
      public UserControl CollectionDock
      {
         get { return _CollectionDock; }
         set { _CollectionDock = value; OnPropertyChanged(); }
      }
      #endregion

      public event DisplayEventHandler DisplayEvent;

      public VMCollectionDeckBox(CollectionModel Model, string RoutingName) : base(Model, RoutingName)
      {
         // The collection list update automatically from the collection editor.
         // If the list is changed via some other method, we need to sync.
         OperationWindow.SetNewDisplay(
             Name: "List",
             NewDisplay: new VFancyCollectionList(),
             DataContext: new VMFancyCollectionList(Model.CollectionItems, "FancyColList", true),
             Persist: false);
         OperationWindow.DisplayEvent += DisplayEventHandler;

         ViewClass itemDock = ViewFactory.CreateItemDock(ItemDock);
         ((VMCardImageDock)itemDock.ViewModel).DisplayEvent += DisplayEventHandler;
         Dock = (VMCardImageDock)itemDock.ViewModel;
         CollectionDock = (VCardImageDock)itemDock.View;
      }

      #region Event Handlers
      private void eItemClicked(CardModel model)
      {
         Dock.SetDisplay(model);
      }

      private void eDisplayCollectionEditorCommand(object canExecute)
      {
         VMCollectionEditor collectionsOverviewVM = new VMCollectionEditor(Model, CollectionEditor);
         OperationWindow.ShowOverlay(new VCollectionEditor() { DataContext = collectionsOverviewVM });
      }

      private void eDisplayCreateChildCollection(object canExecute)
      {
         VMRequestText collectionsOverviewVM = new VMRequestText("", "");
         OperationWindow.ShowOverlay(new VRequestText() { DataContext = collectionsOverviewVM });
      }

      private void eSaveCollection(object canExecute)
      {
         Model.SaveCollection();
      }

      private void eSwitchToCubeView(object canExecute)
      {
         DisplayEventArgs eventArts = new DisplayEventArgs(VCICollectionDeckBox.SwitchToCube, Model);
         DisplayEvent(this, eventArts);
      }

      private void eAcceptOrCancelColEditor()
      {
         OperationWindow.CloseOverlay();
      }

      private void eSubCollectionAccept(string aszCollectionName)
      {
         Model.CreateChildCollection(aszCollectionName);
         OperationWindow.CloseOverlay();
      }

      private void eSubCollectionCancelled()
      {
         OperationWindow.CloseOverlay();
      }

      /// <summary>
      /// This should be called when changes have been made to the server,
      /// but the list has not syncronized yet
      /// </summary>
      private void eCollectionModified()
      {
         Model.Sync(ASync: false);
      }

      private void eListSynced()
      {
         // Check if the model we were viewing, still exists
         // For whatever reason, this is always returning false.
         // I need to make sure that the collectionItems arent'
         // being recreated entirely each sync (cuz that would cause this)
         // and also need to make sure that the comparison is accurate.
         if( Model.CollectionItems.Item.Contains(Dock.Model.ModelDisplay) )
         {
            // Notify the dock that the model may have changed.
            Dock.Model.ModelUpdated();
         }
         else
         {
            // Check to see if the viewing item's active UIDs is contained
            // in some other Model.
            List<string> lstUIDs = Dock.GetEdittingUIDs();
            CardModel matchingModel = null;
            if(lstUIDs.Count > 0)
            {
                matchingModel = Model
                  .CollectionItems.Item
                  .Where(x => x.UIDs.Contains(lstUIDs[0]))
                  .FirstOrDefault();
            }

            if( matchingModel == null )
            {
               Dock.Model.SetDisplay("");
            }
            else
            {
               Dock.Model.SetDisplay(matchingModel);
            }
         }
      }
      #endregion

      #region IViewComponent
      public List<StoreFrontMenuItem> GetMenuItems()
      {
         List<StoreFrontMenuItem> lstRetVal = new List<StoreFrontMenuItem>();

         StoreFrontMenuItem openCollectionEditor = new StoreFrontMenuItem("Edit Collection", eDisplayCollectionEditorCommand);
         StoreFrontMenuItem saveCollection = new StoreFrontMenuItem("Save Collection", eSaveCollection);
         StoreFrontMenuItem createChild = new StoreFrontMenuItem("Create Child Collection", eDisplayCreateChildCollection);
         StoreFrontMenuItem switchToCube = new StoreFrontMenuItem("View as Cube", eSwitchToCubeView);

         lstRetVal.Add(openCollectionEditor);
         lstRetVal.Add(saveCollection);
         lstRetVal.Add(createChild);
         lstRetVal.Add(switchToCube);

         return lstRetVal;
      }
      #endregion

      #region IVCISupporter
      public void DisplayEventHandler(object source, DisplayEventArgs e)
      {
         if (!Application.Current.Dispatcher.CheckAccess())
         {
            Action aAction = new Action(() => { inDisplayEventHandler(source, e); });
            Application.Current.Dispatcher.BeginInvoke(aAction);
            return;
         }

         inDisplayEventHandler(source, e);
      }

      public void inDisplayEventHandler(object source, DisplayEventArgs e)
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

         VCIRequestText RTIS = new VCIRequestText(
             Accept: (x) => { return (x as VMCollectionDeckBox).eSubCollectionAccept; },
             Cancel: (x) => { return (x as VMCollectionDeckBox).eSubCollectionCancelled; });
         _IRouter.AddInterface(RTIS);

         VCICollectionEditor CEIS = new VCICollectionEditor(
            Accept: (x) => { return (x as VMCollectionDeckBox).eAcceptOrCancelColEditor; },
            Cancel: (x) => { return (x as VMCollectionDeckBox).eAcceptOrCancelColEditor; });
         _IRouter.AddInterface(CEIS);

         VCIFancyCollectionList FCIS = new VCIFancyCollectionList(
            Clicked: (x) => { return (x as VMCollectionDeckBox).eItemClicked; },
            ListUpdated: (x) => { return (x as VMCollectionDeckBox).eListSynced; });
         _IRouter.AddInterface(FCIS);

         VCICardImageDock CIDIS = new VCICardImageDock(
            ServerChange: (x) => { return (x as VMCollectionDeckBox).eCollectionModified; });
         _IRouter.AddInterface(CIDIS);
      }
      #endregion

      #region IViewModel
      public override void ModelUpdated()
      {

      }
      #endregion
   }
}
