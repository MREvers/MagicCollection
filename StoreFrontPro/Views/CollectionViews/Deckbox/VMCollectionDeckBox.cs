using StoreFrontPro.Server;
using StoreFrontPro.Support.MultiDisplay;
using StoreFrontPro.Views.Interfaces.CollectionChanger;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;

namespace StoreFrontPro.Views.CollectionViews.Deckbox
{
    class VMCollectionDeckBox: ViewModel<CollectionModel>, IViewComponent, IVCISupporter
    {
        private MultiDisplay _OperationWindow = new MultiDisplay();

        public MultiDisplay OperationWindow
        {
            get { return _OperationWindow; }
            set { _OperationWindow = value; OnPropertyChanged(); }
        }

        public Dictionary<Type, IViewComponentInterface> SupportedInterface { get; set; } = new Dictionary<Type, IViewComponentInterface>();

        public event DisplayEventHandler DisplayEvent;

        public VMCollectionDeckBox(CollectionModel Model) : base(Model)
        {
            VCICollectionEditor CEIS = new VCICollectionEditor(
                Accept: eAcceptOrCancelColEditor,
                Cancel: eAcceptOrCancelColEditor);
            SupportedInterface.Add(CEIS.GetInterfaceType(), CEIS);

            if (!Model.IsCollapsedCollection)
            {
                Model.IsCollapsedCollection = true;
                Model.Sync();
            }

            OperationWindow.SetNewDisplay(
                Name: "List",
                NewDisplay: new VFancyCollectionList(),
                DataContext: new VMFancyCollectionList(Model.CollectionItems, true),
                Persist: false);
            OperationWindow.DisplayEvent += DisplayEventHandler;

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

        public List<StoreFrontMenuItem> GetMenuItems()
        {
            List<StoreFrontMenuItem> lstRetVal = new List<StoreFrontMenuItem>();

            StoreFrontMenuItem openCollectionEditor = new StoreFrontMenuItem("Edit Collection", eDisplayCollectionEditorCommand);
            StoreFrontMenuItem saveCollection = new StoreFrontMenuItem("Save Collection", eSaveCollection);
            StoreFrontMenuItem switchToCube = new StoreFrontMenuItem("View as Cube", eSwitchToCubeView);

            lstRetVal.Add(openCollectionEditor);
            lstRetVal.Add(saveCollection);
            lstRetVal.Add(switchToCube);

            return lstRetVal;
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

        private void eSwitchToCubeView(object canExecute)
        {
            DisplayEventArgs eventArts = new DisplayEventArgs(VCICollectionDeckBox.SwitchToCube, Model);
            DisplayEvent(this, eventArts);
        }

        private void eAcceptOrCancelColEditor()
        {
            OperationWindow.CloseOverlay();
        }

    }
}
