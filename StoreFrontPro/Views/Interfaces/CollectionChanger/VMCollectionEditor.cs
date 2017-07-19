using StoreFrontPro.Server;
using StoreFrontPro.Tools;
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
        public ObservableCollection<VCollectionEditorItem> TextChangesList { get; set; } = new ObservableCollection<VCollectionEditorItem>();

        public VSuggestionsSearchBox AddCardSearchBox { get; set; }
        public VSuggestionsSearchBox RemoveCardSearchBox { get; set; }

        public RelayCommand AcceptCommand { get; set; }
        public RelayCommand CancelCommand { get; set; }

        private List<MCollectionEditorItem> m_lstItems { get; set; }

        public Dictionary<Type, IViewComponentInterface> SupportedInterface { get; set; } = new Dictionary<Type, IViewComponentInterface>();

        public event DisplayEventHandler DisplayEvent;

        public VMCollectionEditor(CollectionModel Model) : base(Model)
        {
            AcceptCommand = new RelayCommand(eAcceptCommand);
            CancelCommand = new RelayCommand(eCancelCommand);
            m_lstItems = new List<MCollectionEditorItem>();

            MSuggestionsSearchBox ssBoxMA = new MSuggestionsSearchBox(
                ActionName: "Add Card",
                SearchCollection: (szSearch) => { return ServerInterface.Server.GetAllCardsStartingWith(szSearch); });
            VMSuggestionsSearchBox ssBoxVMA = new VMSuggestionsSearchBox(ssBoxMA);
            AddCardSearchBox = new VSuggestionsSearchBox() { DataContext = ssBoxVMA };
            ssBoxVMA.DisplayEvent += eAddCardEventHandler;

            MSuggestionsSearchBox ssBoxMR = new MSuggestionsSearchBox(
                ActionName: "Remove/Replace Card",
                Collection: Model.CollectionItems.Select(x => getStandardShortId(x)));
            VMSuggestionsSearchBox ssBoxVMR = new VMSuggestionsSearchBox(ssBoxMR);
            RemoveCardSearchBox = new VSuggestionsSearchBox() { DataContext = ssBoxVMR };
            ssBoxVMR.DisplayEvent += eRemoveCardEventHandler;
        }

        public List<StoreFrontMenuItem> GetMenuItems()
        {
            return new List<StoreFrontMenuItem>();
        }

        public void DisplayEventHandler(object source, DisplayEventArgs e)
        {
            // throw new NotImplementedException();
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

                CardModel oRemoveCard = Model.CollectionItems.Where(x => getStandardShortId(x) == szBaseCard).FirstOrDefault();
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
                        szBaseCard = szAddBaseCard;
                    }
                    else
                    {
                        szDisplay += "- " + oRemoveCard.GetIdealIdentifier();
                        szCmdString += "- " + oRemoveCard.GetFullIdentifier();
                        szBaseCard = "";
                    }

                    int iMaxOps = 0;
                    Model.CollectionItems.ForEach(x => { if (oRemoveCard.GetMetaTag("__hash") == x.GetMetaTag("__hash")) { iMaxOps += x.Count; } });
                    if (iMaxOps > 0)
                    {
                        addEditorItem(szDisplay, szCmdString, getIdentifierOptions(szBaseCard), iMaxOps);
                    }

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
                addEditorItem(szCmdString, szCmdString, getIdentifierOptions(szBaseCard));
                addBoxVM.TextBoxValue = "";
            }
        }

        private void eAcceptCommand(object canExecute)
        {
            DisplayEventArgs eventArgs = new DisplayEventArgs(VCICollectionEditor.Accept);
            Model.SubmitBulkEdits(getFunctionList(), () => { DisplayEvent?.Invoke(this, eventArgs); });
            DisplayEvent?.Invoke(this, eventArgs);
        }

        private void eCancelCommand(object canExecute)
        {
            DisplayEventArgs eventArgs = new DisplayEventArgs(VCICollectionEditor.Cancel);
            DisplayEvent(this, eventArgs);
        }

        private void addEditorItem(string aszDisplay, string aszFunction, List<string> alstPossibleSetValues, int aiMaxOps = 99)
        {
            MCollectionEditorItem ceiM = new MCollectionEditorItem(aszDisplay, aszFunction, alstPossibleSetValues, aiMaxOps);
            VMCollectionEditorItem ceiVM = new VMCollectionEditorItem(ceiM);
            m_lstItems.Add(ceiM);
            TextChangesList.Add(new VCollectionEditorItem() { DataContext = ceiVM });
        }

        private string getSelectedSet(string szBaseCard)
        {
            return szBaseCard + "{ set=\"";
        }

        private List<string> getFunctionList()
        {
            foreach (MCollectionEditorItem item in m_lstItems)
            {
                item.FunctionText = item.FunctionText + " { set=\"" + item.SelectedSet + "\" } ";
            }

            List<string> lstOutput = m_lstItems
                .Select(x =>
                    x.FunctionText.Substring(0, 1) + " x" + x.Amount + x.FunctionText.Substring(1))
                .ToList();
            return lstOutput;
        }

        private List<string> getIdentifierOptions(string szCard)
        {
            return CardModel.GetIdentifierOptions(szCard).Where(x => x.Item1 == "set").FirstOrDefault()?.Item2;
        }

        private string getStandardShortId(CardModel aCM)
        {
            return aCM.GetIdealIdentifier();
        }
    }
}
