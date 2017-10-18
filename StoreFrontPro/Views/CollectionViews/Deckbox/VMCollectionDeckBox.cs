using StoreFrontPro.Server;
using StoreFrontPro.Support.MultiDisplay;
using StoreFrontPro.Views.Components.RequestTextOverlay;
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
         OperationWindow.SetNewDisplay(
             Name: "List",
             NewDisplay: new VFancyCollectionList(),
             DataContext: new VMFancyCollectionList(Model.CollectionItems, "FancyColList", true),
             Persist: false);
         OperationWindow.DisplayEvent += DisplayEventHandler;

         ViewClass itemDock = ViewFactory.CreateItemDock(ItemDock);
         CollectionDock = itemDock.View;
      }

      #region Event Handlers
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
      }
      #endregion

      #region IViewModel
      public override void ModelUpdated()
      {

      }
      #endregion
   }
}
