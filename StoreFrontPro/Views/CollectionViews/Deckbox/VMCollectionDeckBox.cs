﻿using StoreFrontPro.Server;
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
    class VMCollectionDeckBox: ViewModel<CollectionModel>, IViewComponent
    {
        private MultiDisplay _OperationWindow = new MultiDisplay();

        public MultiDisplay OperationWindow
        {
            get { return _OperationWindow; }
            set { _OperationWindow = value; OnPropertyChanged(); }
        }

        public event DisplayEventHandler DisplayEvent;

        public VMCollectionDeckBox(CollectionModel Model) : base(Model)
        {
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


        private void displayEventHandler(object source, DisplayEventArgs e)
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
            if (e.Source == "VMCollectionEditor")
            {
                if (e.Property == "AcceptCommand")
                {
                    OperationWindow.CloseOverlay();
                }
                else if (e.Property == "CancelCommand")
                {
                    OperationWindow.CloseOverlay();
                }
            }
        }

        private void eDisplayCollectionEditorCommand(object canExecute)
        {
            VMCollectionEditor collectionsOverviewVM = new VMCollectionEditor(Model);
            OperationWindow.ShowOverlay(new VCollectionEditor() { DataContext = collectionsOverviewVM });
            collectionsOverviewVM.DisplayEvent += displayEventHandler;
        }

        private void eSaveCollection(object canExecute)
        {
            Model.SaveCollection();
        }

        private void eSwitchToCubeView(object canExecute)
        {
            DisplayEventArgs eventArts = new DisplayEventArgs(Source: "VMCollectionDeckBox", Property: "SwitchToCube", Event: "Clicked");
            DisplayEvent(this, eventArts);
        }
    }
}
