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
        private MultiDisplay _OperationWindow = new MultiDisplay();

        public event DisplayEventHandler DisplayEvent;

        public MultiDisplay OperationWindow
        {
            get { return _OperationWindow; }
            set { _OperationWindow = value; OnPropertyChanged(); }
        }

        public Dictionary<Type, IViewComponentInterface> SupportedInterface { get; set; } = new Dictionary<Type, IViewComponentInterface>();

        public VMCollectionCube(CollectionModel Model) : base(Model)
        {
            VCICollectionEditor CEIS = new VCICollectionEditor(
                Accept: eAcceptOrCancelColEditor,
                Cancel: eAcceptOrCancelColEditor);
            SupportedInterface.Add(CEIS.GetInterfaceType(), CEIS);

            if (Model.IsCollapsedCollection)
            {
                Model.IsCollapsedCollection = false;
                Model.Sync();
            }

            VMCardGroupDisplay collectionCubeVM = new VMCardGroupDisplay(Model.CollectionItems);
            OperationWindow.SetNewDisplay(
                Name: "CollectionView",
                NewDisplay: new VCardGroupDisplay(),
                DataContext: collectionCubeVM,
                Persist: false);
            OperationWindow.DisplayEvent += DisplayEventHandler;
        }

        public List<StoreFrontMenuItem> GetMenuItems()
        {
            List<StoreFrontMenuItem> lstRetVal = new List<StoreFrontMenuItem>();

            StoreFrontMenuItem openCollectionEditor = new StoreFrontMenuItem("Edit Collection", eDisplayCollectionEditorCommand);
            StoreFrontMenuItem saveCollection = new StoreFrontMenuItem("Save Collection", eSaveCollection);
            StoreFrontMenuItem switchToDeckBox = new StoreFrontMenuItem("View as Deckbox", eSwitchToDeckboxView);

            lstRetVal.Add(openCollectionEditor);
            lstRetVal.Add(saveCollection);
            lstRetVal.Add(switchToDeckBox);

            return lstRetVal;
        }


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

        private void inDisplayEventHandler(object source, DisplayEventArgs e)
        {
            if (SupportedInterface.ContainsKey(source.GetType()))
            {
                SupportedInterface?[source.GetType()]?.TryInvoke(e.Key, e.Args);
            }
        }

        private void eDisplayCollectionEditorCommand(object canExecute)
        {
            VMCollectionEditor collectionsOverviewVM = new VMCollectionEditor(Model);
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
    }
}
