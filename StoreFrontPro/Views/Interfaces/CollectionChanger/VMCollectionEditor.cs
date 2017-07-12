﻿using StoreFrontPro.Server;
using StoreFrontPro.Views.Components.SuggestionsSearchBox;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;

namespace StoreFrontPro.Views.Interfaces.CollectionChanger
{
    class VMCollectionEditor : ViewModel<CollectionModel>, IViewComponent, IVCISupporter
    {
        public ObservableCollection<string> TextChangesList { get; set; } = new ObservableCollection<string>();

        public VSuggestionsSearchBox AddCardSearchBox { get; set; }
        public VSuggestionsSearchBox RemoveCardSearchBox { get; set; }

        public RelayCommand AcceptCommand { get; set; }
        public RelayCommand CancelCommand { get; set; }

        private List<string> m_lstRealChanges { get; set; }

        public Dictionary<Type, IViewComponentInterface> SupportedInterface { get; set; } = new Dictionary<Type, IViewComponentInterface>();

        public event DisplayEventHandler DisplayEvent;

        public VMCollectionEditor(CollectionModel Model) : base(Model)
        {
            AcceptCommand = new RelayCommand(eAcceptCommand);
            CancelCommand = new RelayCommand(eCancelCommand);
            m_lstRealChanges = new List<string>();

            MSuggestionsSearchBox ssBoxMA = new MSuggestionsSearchBox(
                ActionName: "Add Card",
                SearchCollection: (szSearch) => { return ServerInterface.Server.GetAllCardsStartingWith(szSearch); });
            VMSuggestionsSearchBox ssBoxVMA = new VMSuggestionsSearchBox(ssBoxMA);
            AddCardSearchBox = new VSuggestionsSearchBox() { DataContext = ssBoxVMA };
            ssBoxVMA.DisplayEvent += eAddCardEventHandler;

            MSuggestionsSearchBox ssBoxMR = new MSuggestionsSearchBox(
                ActionName: "Remove/Replace Card",
                Collection: Model.CollectionItems.Select(x => x.CardName));
            VMSuggestionsSearchBox ssBoxVMR = new VMSuggestionsSearchBox(ssBoxMR);
            RemoveCardSearchBox = new VSuggestionsSearchBox() { DataContext = ssBoxVMR };
            ssBoxVMR.DisplayEvent += eRemoveCardEventHandler;
        }

        private void eRemoveCardEventHandler(object Source, DisplayEventArgs Event)
        {
            // Only one event comes from the SSBox so we dont have to worry about checking whether this is the right function
            VMSuggestionsSearchBox removeBoxVM = (RemoveCardSearchBox.DataContext as VMSuggestionsSearchBox);
            if (removeBoxVM.ComboBoxList.Count > 0 &&
                removeBoxVM.ComboBoxList.Contains(removeBoxVM.TextBoxValue))
            {
                string szBaseCard = removeBoxVM.TextBoxValue;
                string szCmdString = "";
                string szDisplay = "";

                CardModel oRemoveCard = Model.CollectionItems.Where(x => x.CardName == szBaseCard).FirstOrDefault();
                if (oRemoveCard != null)
                {
                    VMSuggestionsSearchBox addBoxVM = (AddCardSearchBox.DataContext as VMSuggestionsSearchBox);

                    if (addBoxVM.ComboBoxList.Count > 0 &&
                        addBoxVM.ComboBoxList.Contains(addBoxVM.TextBoxValue))
                    {
                        string szAddBaseCard = addBoxVM.TextBoxValue;

                        szDisplay += "% " + oRemoveCard.GetIdealIdentifier();
                        szDisplay += " -> " + szAddBaseCard;
                        // Replace
                        szCmdString += "% " + oRemoveCard.GetFullIdentifier();
                        szCmdString += " -> " + szAddBaseCard;
                        addBoxVM.TextBoxValue = "";
                    }
                    else
                    {
                        szDisplay += "- " + oRemoveCard.GetIdealIdentifier();
                        szCmdString += "- " + oRemoveCard.GetFullIdentifier();
                    }

                    TextChangesList.Add(szDisplay);
                    m_lstRealChanges.Add(szCmdString);
                    removeBoxVM.TextBoxValue = "";
                }
            }
        }

        private void eAddCardEventHandler(object Source, DisplayEventArgs Event)
        {
            // This gets the first item in the dropdown box list.
            VMSuggestionsSearchBox addBoxVM = (AddCardSearchBox.DataContext as VMSuggestionsSearchBox);
            if (addBoxVM.ComboBoxList.Count > 0 &&
                addBoxVM.ComboBoxList.Contains(addBoxVM.TextBoxValue))
            {
                string szBaseCard = addBoxVM.TextBoxValue;
                string szCmdString = "";
                szCmdString += "+ " + szBaseCard;
                m_lstRealChanges.Add(szCmdString);
                TextChangesList.Add(szCmdString);
                addBoxVM.TextBoxValue = "";
            }
        }

        private void eAcceptCommand(object canExecute)
        {
            DisplayEventArgs eventArgs = new DisplayEventArgs(VCICollectionEditor.Accept);
            Model.SubmitBulkEdits(m_lstRealChanges, ()=> { DisplayEvent(this, eventArgs); });
            DisplayEvent(this, eventArgs);
        }

        private void eCancelCommand(object canExecute)
        {
            DisplayEventArgs eventArgs = new DisplayEventArgs(VCICollectionEditor.Cancel);
            DisplayEvent(this, eventArgs);
        }

        public List<StoreFrontMenuItem> GetMenuItems()
        {
            return new List<StoreFrontMenuItem>();
        }

        public void DisplayEventHandler(object source, DisplayEventArgs e)
        {
            // throw new NotImplementedException();
        }
    }
}
