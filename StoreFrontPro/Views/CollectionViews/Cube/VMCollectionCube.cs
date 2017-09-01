using StoreFrontPro.Server;
using StoreFrontPro.Support.MultiDisplay;
using StoreFrontPro.Views.Interfaces.CollectionChanger;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls.Primitives;

namespace StoreFrontPro.Views.CollectionViews.Cube
{
   class VMCollectionCube : ViewModel<CollectionModel>, IViewComponent, IVCISupporter
   {
      private const string cszCardGroupDisplayName = "CGDN";
      private const string cszCollectionEditorName = "CEN";

      private MultiDisplay _OperationWindow = new MultiDisplay();

      public event DisplayEventHandler DisplayEvent;

      public MultiDisplay OperationWindow
      {
         get { return _OperationWindow; }
         set { _OperationWindow = value; OnPropertyChanged(); }
      }

      public VMCollectionCube(CollectionModel Model, string RoutingName) : base(Model, RoutingName)
      {
         if (Model.IsCollapsedCollection)
         {
            Model.IsCollapsedCollection = false;
            Model.Sync();
         }

         VMCardGroupDisplay collectionCubeVM = new VMCardGroupDisplay(Model.CollectionItems, cszCardGroupDisplayName);
         OperationWindow.SetNewDisplay(
             Name: "CollectionView",
             NewDisplay: new VCardGroupDisplay(),
             DataContext: collectionCubeVM,
             Persist: false);
         OperationWindow.DisplayEvent += DisplayEventHandler;
      }

      #region Event Handlers
      private void eDisplayCollectionEditorCommand(object canExecute)
      {
         VMCollectionEditor collectionsOverviewVM = new VMCollectionEditor(Model, cszCollectionEditorName);
         OperationWindow.ShowOverlay(new VCollectionEditor() { DataContext = collectionsOverviewVM });
      }

      private void eSaveCollection(object canExecute)
      {
         Model.SaveCollection();
      }

      private void eSwitchToDeckboxView(object canExecute)
      {
         DisplayEventArgs eventArgs = new DisplayEventArgs(VCICollectionCube.SwitchToDeckBox, Model);
         DisplayEvent(this, eventArgs);
      }

      private void eAcceptOrCancelColEditor()
      {
         OperationWindow.CloseOverlay();
      }
      #endregion

      #region IViewComponent
      public List<StoreFrontMenuItem> GetMenuItems()
      {
         List<StoreFrontMenuItem> lstRetVal = new List<StoreFrontMenuItem>();

         StoreFrontMenuItem openCollectionEditor = new StoreFrontMenuItem("Edit Collection", eDisplayCollectionEditorCommand);
         StoreFrontMenuItem saveCollection       = new StoreFrontMenuItem("Save Collection", eSaveCollection);
         StoreFrontMenuItem switchToDeckBox      = new StoreFrontMenuItem("View as Deckbox", eSwitchToDeckboxView);

         lstRetVal.Add(openCollectionEditor);
         lstRetVal.Add(saveCollection);
         lstRetVal.Add(switchToDeckBox);

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
         VCICollectionEditor CEIS = new VCICollectionEditor(
            Accept: (x) => { return (x as VMCollectionCube).eAcceptOrCancelColEditor; },
            Cancel: (x) => { return (x as VMCollectionCube).eAcceptOrCancelColEditor; });
         _IRouter.AddInterface(CEIS);
      }
      #endregion

      #region IViewModel
      public override void ModelUpdated()
      {
         throw new NotImplementedException();
      }
      #endregion
   }
}
